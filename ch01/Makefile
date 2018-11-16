# make variable Make instead of make
# https://www.gnu.org/software/make/manual/html_node/MAKE-Variable.html

# this way is at risk, variable KERNELRELEASE shoulde be leveraged
# https://stackoverflow.com/questions/30031161/eudyptula-challenge-and-kernel-path


# kernel document is very precise
# https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/kbuild/modules.txt
ifeq ($(KERNELRELEASE),)
all:
	echo "exec in current module"
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
else
# you can not change `obj-m` to other symbol
# https://stackoverflow.com/questions/50742777/how-is-obj-m-variable-exported-to-sub-make
obj-m += hello-1.o
endif
