# SPDX-License-Identifier: GPL-2.0-only
# builds the kprobes example kernel modules;
# then to use one (as root):  insmod <module_name.ko>

# obj-$(CONFIG_SAMPLE_KPROBES) += kprobe_example.o
# obj-$(CONFIG_SAMPLE_KRETPROBES) += kretprobe_example.o

ifneq ($(KERNELRELEASE),)
obj-m := perftop.o 
obj-m += kprobe_stack_trace_save.o
obj-m += kretprobe_perftop_show.o

else
KDIR := /lib/modules/$(shell uname -r)/build
default: 
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules 
	rm -r -f .tmp_versions *.mod.c .*.cmd *.o *.symvers 

endif

clean:
	rm *.ko *.mod modules.order
