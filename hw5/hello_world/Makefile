ifneq ($(KERNELRELEASE),)
obj-m := hello_world.o

else
KDIR := /lib/modules/$(shell uname -r)/build
# PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(shell pwd) modules
	rm -r -f .tmp_versions *.mod.c .*.cmd *.o *.symvers

endif