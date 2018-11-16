# scull 

### why to register cdev in driver code
https://stackoverflow.com/questions/14705189/why-to-register-struct-cdev-in-driver-code

`struct cdev` represent a character device within the kernel. 

```
cdev_init() - used to initialize struct cdev with the defined file_operations
cdev_add()  - used to add a character device to the system. 
cdev_del()  - used to remove a character device from the system
```

`struct cdev` structure encapsulates the `file_operations structure` and some other important driver information(major/minor no)

`struct file_operations` is the most important data structure used. This structure is used to implement the basic file i/o - open(), read(), write(), close(), ioctl(), etc... functionality for the the device.

`struct cdev` is one of the elements of the `inode` structure

### how to debug
use printk 
```
#define PDEBUG(fmt, args...) printk( KERN_DEBUG "scull: " fmt, ## args)
```

### ioctl

```
int ioctl(int fd, unsigned long request, ...); in <sys/ioctl.h>
```

A request code has 4 main parts
>    1. A Magic number - 8 bits
>    2. A sequence number - 8 bits
>    3. Argument type (typically 14 bits), if any.
>    4. Direction of data transfer (2 bits).  

The ioctl() system call manipulates the underlying device parameters
of special files.  In particular, many operating characteristics of
character special files (e.g., terminals) may be controlled with
ioctl() requests.

https://en.wikipedia.org/wiki/Ioctl
https://stackoverflow.com/questions/15807846/ioctl-linux-device-driver

### what is module parameter used for


### function explanation

```
f	int scull_trim(struct scull_dev *dev)	[:53 4]
// free all the memory, in this function, we can infer that,
// the memory is linked list in which every node point to a 2d array(Java style)

# debug
f	int scull_read_procmem(struct seq_file *s, void *v)	[:80 4]
f	static void *scull_seq_start(struct seq_file *s, loff_t *pos)	[:113 4]
f	static void *scull_seq_next(struct seq_file *s, void *v, loff_t *pos)	[:120 4]
f	static void scull_seq_stop(struct seq_file *s, void *v)	[:128 4]
f	static int scull_seq_show(struct seq_file *s, void *v)	[:133 4]
f	static int scullmem_proc_open(struct inode *inode, struct file *file)	[:171 4]
f	static int scullseq_proc_open(struct inode *inode, struct file *file)	[:176 4]
f	static void scull_create_proc(void)	[:205 4]
f	static void scull_remove_proc(void)	[:213 4]
# debug over

// open and release
f	int scull_open(struct inode *inode, struct file *filp)	[:231 4]
f	int scull_release(struct inode *inode, struct file *filp)	[:248 4]

// operation on memory
f	struct scull_qset *scull_follow(struct scull_dev *dev, int n)	[:255 4]
f	ssize_t scull_read(struct file *filp, char __user *buf, size_t count,	[:285 4]
f	ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,	[:329 4]
f	loff_t scull_llseek(struct file *filp, loff_t off, int whence)	[:515 4]

// ioctl
f	long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)	[:386 4]

// init and clean
f	void scull_cleanup_module(void)	[:562 4]
f	static void scull_setup_cdev(struct scull_dev *dev, int index)	[:593 4]
f	int scull_init_module(void)	[:607 4]
```
