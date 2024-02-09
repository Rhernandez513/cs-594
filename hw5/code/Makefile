MODULE = hw5

obj-m += $(MODULE).o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

all: $(MODULE)


%.o: %.c
	@echo "  CC      $<"
	@$(CC) -c $< -o $@

$(MODULE):
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) clean
