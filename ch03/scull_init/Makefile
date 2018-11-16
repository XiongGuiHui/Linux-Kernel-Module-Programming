ifeq ($(KERNELRELEASE),)
all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
else
obj-m += b.o ldd-a.o
endif
