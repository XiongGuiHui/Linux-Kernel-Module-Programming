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
MODULE_LICENSE("Dual BSD/GPL");

struct scull_dev *scull_devices;	/* allocated in scull_init_module */

struct file_operations scull_fops = {
  // emmmmmmmmm 暂时这些没有实现，也没有看懂
	// .owner =    THIS_MODULE,
	// .llseek =   scull_llseek,
	// .read =     scull_read,
	// .write =    scull_write,
	// .unlocked_ioctl =    scull_ioctl,
	// .open =     scull_open,
	// .release =  scull_release,
};


int scull_trim(struct scull_dev *dev) {
	struct scull_qset *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-NULL */
	int i;

	for (dptr = dev->data; dptr; dptr = next) { /* all the list items */
		if (dptr->data) {
			for (i = 0; i < qset; i++)
				kfree(dptr->data[i]);
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
  // 暂时没有friend
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
   *
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
   * emmmmmmmmm GFP_KERNEL 表示什么
   * dev 和　scull_devices 各自表示的东西:
   * 1. cdev 是字符设备,　系统提供的组件
   * 2. scull 是在cdev 的基础上继续添加封装，实现指定功能的效果
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
    scull_devices[i].qset = scull_qset;
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
