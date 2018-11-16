ifeq ($(KERNELRELEASE),)
all:
	echo "exec in current module"
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
else
obj-m += hello-2.o
endif
