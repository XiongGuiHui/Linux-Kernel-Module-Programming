#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <linux/uaccess.h>	/* copy_*_user */

#include "scull.h"

/* 暂时没有什么作用 */


int scull_major =   SCULL_MAJOR;
int scull_minor =   0;
int scull_nr_devs = SCULL_NR_DEVS;	/* number of bare scull devices */
int scull_quantum = SCULL_QUANTUM;
int scull_qset =    SCULL_QSET;

module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

MODULE_AUTHOR("Hu bachelar");
MODULE_LICENSE("BSD/GPL");


struct scull_dev *scull_devices;	/* allocated in scull_init_module */



/**
 * follow the list
 */
struct scull_qset *scull_follow(struct scull_dev *dev, int n){
	struct scull_qset *qs = dev->data;

        /* Allocate first qset explicitly if need be */
	if (! qs) {
		qs = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
		if (qs == NULL)
			return NULL;  /* Never mind */
		memset(qs, 0, sizeof(struct scull_qset));
	}

	/* Then follow the list */
	while (n--) {
		if (!qs->next) {
			qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
			if (qs->next == NULL)
				return NULL;  /* Never mind */
			memset(qs->next, 0, sizeof(struct scull_qset));
		}
		qs = qs->next;
		continue;
	}
	return qs;
}


/*
 * Data management: read and write
 */
ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos){
	struct scull_dev *dev = filp->private_data; 
	struct scull_qset *dptr;	/* the first listitem */
	int quantum = dev->quantum, qset = dev->qset;
	int itemsize = quantum * qset; /* how many bytes in the listitem */
	int item, s_pos, q_pos, rest;
	ssize_t retval = 0;

	if (mutex_lock_interruptible(&dev->mutex))
		return -ERESTARTSYS;
	if (*f_pos >= dev->size)
		goto out;
	if (*f_pos + count > dev->size)
		count = dev->size - *f_pos;

	/* find listitem, qset index, and offset in the quantum */
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum; q_pos = rest % quantum;

	/* follow the list up to the right position (defined elsewhere) */
  // emmmmmmmmm 应该是遍历列表的内容
	dptr = scull_follow(dev, item);

	if (dptr == NULL || !dptr->data || ! dptr->data[s_pos])
		goto out; /* don't fill holes */

	/* read only up to the end of this quantum */
	if (count > quantum - q_pos)
		count = quantum - q_pos;

	if (copy_to_user(buf, dptr->data[s_pos] + q_pos, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

  out:
	mutex_unlock(&dev->mutex);
	return retval;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum, qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = -ENOMEM; /* value used in "goto out" statements */

	if (mutex_lock_interruptible(&dev->mutex))
		return -ERESTARTSYS;

	/* find listitem, qset index and offset in the quantum */
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum; q_pos = rest % quantum;

	/* follow the list up to the right position */
	dptr = scull_follow(dev, item);
	if (dptr == NULL)
		goto out;
	if (!dptr->data) {
		dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
		if (!dptr->data)
			goto out;
		memset(dptr->data, 0, qset * sizeof(char *));
	}
	if (!dptr->data[s_pos]) {
		dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
		if (!dptr->data[s_pos])
			goto out;
	}
	/* write only up to the end of this quantum */
	if (count > quantum - q_pos)
		count = quantum - q_pos;

	if (copy_from_user(dptr->data[s_pos]+q_pos, buf, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

        /* update the size */
	if (dev->size < *f_pos)
		dev->size = *f_pos;

  out:
	mutex_unlock(&dev->mutex);
	return retval;
}

/*
 * Open and close
 */
int scull_open(struct inode *inode, struct file *filp){
	struct scull_dev *dev; /* device information */
  printk(KERN_INFO "Bachelar: Device has been opened time(s)\n");

	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev; /* for other methods */

	/* now trim to 0 the length of the device if open was write-only */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
		if (mutex_lock_interruptible(&dev->mutex))
			return -ERESTARTSYS;
		scull_trim(dev); /* ignore errors */
		mutex_unlock(&dev->mutex);
	}
	return 0;          /* success */
}


int scull_release(struct inode *inode, struct file *filp){
  printk(KERN_INFO "Bachelar: Device has been opened time(s)\n");
	return 0;
}
 

struct file_operations scull_fops = {
	.owner =    THIS_MODULE,
	// .llseek =      scull_llseek,
	.read =     scull_read,
	.write =    scull_write,
	// .unlocked_ioctl =    scull_ioctl,
	.open =     scull_open,
	.release =  scull_release,
};


int scull_trim(struct scull_dev *dev) {
	struct scull_qset *next, *dptr; 
	int qset = dev->qset;   /* "dev" is not-NULL 　qset 当前数组大小 */
	int i;

	for (dptr = dev->data; dptr; dptr = next) { /* all the list items */

		if (dptr->data) {
      // 释放所有quantum 
			for (i = 0; i < qset; i++)
				kfree(dptr->data[i]);

      // 释放持有quantum 的数组
			kfree(dptr->data);

			dptr->data = NULL;
		}

		next = dptr->next;
		kfree(dptr);
	}

	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->data = NULL;
	return 0;
}


void scull_cleanup_module(void){
	int i;
	dev_t devno = MKDEV(scull_major, scull_minor);


	/* Get rid of our char dev entries */
	if (scull_devices) {
		for (i = 0; i < scull_nr_devs; i++) {
			scull_trim(scull_devices + i);
			cdev_del(&scull_devices[i].cdev);
		}
		kfree(scull_devices);
	}

#ifdef SCULL_DEBUG /* use proc only if debugging */
	scull_remove_proc();
#endif

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, scull_nr_devs);

	/* and call the cleanup functions for friend devices */
  // emmmmmmmmm 应该暂时没有任何用途，暂时不处理pipe line 等等的东西
	// scull_p_cleanup();
	// scull_access_cleanup();
}



/*
 * Set up the char_dev structure for this device.
 */
static void scull_setup_cdev(struct scull_dev *dev, int index){
	int err, devno = MKDEV(scull_major, scull_minor + index);

  // emmmmmmmmm 为什么需要设置两次scull_fops的结构体
	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;

	err = cdev_add (&dev->cdev, devno, 1);

	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}


int scull_init_module(void) {
	int result, i;
	dev_t dev = 0;
  printk(KERN_INFO "Bachelar i'am coming to it");

  /*
   * Get a range of minor numbers to work with, asking for a dynamic
   * major unless directed otherwise at load time.
   */
	if (scull_major) {
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	} else {
		result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs,
				"scull");
		scull_major = MAJOR(dev);
	}

	if (result < 0) {
		printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
		return result;
	}

  /* 
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
	scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);

	if (!scull_devices) {
		result = -ENOMEM; /* excellent erron number */
		goto fail;  /* Make this more graceful */
	}

	memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));

  /* Initialize each device. */
	for (i = 0; i < scull_nr_devs; i++) {
    scull_devices[i].quantum = scull_quantum;
    scull_devices[i].qset = scull_qset; //　长度一直都是常数而已
    mutex_init(&scull_devices[i].mutex);
    scull_setup_cdev(&scull_devices[i], i);
	}

  /* At this point call the init function for any friend device */
	// dev = MKDEV(scull_major, scull_minor + scull_nr_devs);
	// dev += scull_p_init(dev);
	// dev += scull_access_init(dev);

#ifdef SCULL_DEBUG
  scull_create_proc();
#endif

	return 0; /* succeed */

  fail:
	scull_cleanup_module();
	return result;
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);
