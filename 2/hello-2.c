#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

#include<fff.h>

MODULE_LICENSE("GPL");

static int __init hello_2_init(void){
  printk(KERN_INFO "i am back\n");
  return 0;
}


static void __exit hello_2_exit(void){
  printk(KERN_INFO "i will be back\n");
}

module_init(hello_2_init);
module_exit(hello_2_exit);
