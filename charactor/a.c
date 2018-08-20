#include<linux/module.h>
#include<linux/kernel.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

static int __init init(void){
  printk(KERN_INFO "hello workd 1.\n");

  struct file_operations fops = {
    read: device_read,
    write: device_write,
    open: device_open,
    release: device_release
  };
	
  return 0;
}

static void __exit cleanup(void){
  printk(KERN_INFO "Bye 1.\n");
}

module_init(init);
module_exit(cleanup);


// static int __init hello_5_init(void){
	// printk(KERN_INFO "i'am back, world 5\n");
	// return 0;
// }
//
// static void __exit hello_5_exit(void)
// {
	// printk(KERN_INFO "Goodbye, world 5\n");
// }
//
// module_init(hello_5_init);
// module_exit(hello_5_exit);
