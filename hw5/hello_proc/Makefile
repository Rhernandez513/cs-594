
ifneq ($(KERNELRELEASE),)
obj-m := hello_proc.o 

else
KDIR := /lib/modules/$(shell uname -r)/build
default: 
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules 
	rm -r -f .tmp_versions *.mod.c .*.cmd *.o *.symvers 

endif

clean:
	rm *.ko *.mod modules.order
