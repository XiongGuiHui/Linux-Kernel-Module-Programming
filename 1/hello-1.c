#include<linux/module.h>
#include<linux/kernel.h>

MODULE_LICENSE("GPL");

int init_module(void){
  printk(KERN_INFO "hello workd 1.\n");
  return 0;
}

void cleanup_module(void){
  printk(KERN_INFO "Bye 1.\n");
}

