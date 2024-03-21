#include <linux/ioctl.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>


#define MY_RNG_IOCTL_RAND _IOR('q', 1, unsigned int)
#define MY_RNG_IOCTL_SEED _IOW('q', 1, unsigned int)

#define DEVICE_BASE_PHYS_ADDR 0xfebf1000

void *devmem = 0x0;
unsigned long data = 0;

static long my_rng_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    unsigned long num_bytes = 0;

    if(!devmem) {
        printk("inside my_rng_ioctl\n");
        printk(KERN_ERR "Failed to map device registers in memory");
        return -1; // memory not accessible
    }

    switch (cmd) {
        case MY_RNG_IOCTL_RAND:
            pr_info("Calling ioread32");
            data = (unsigned long) ioread32(devmem);
            pr_info("Data from ioread32: %lu\n", data);
            num_bytes = copy_to_user((unsigned long *)arg, &data, sizeof(unsigned long));
            if(num_bytes != 0) {
                pr_info("Failed to copy data to user space\n");
                return -EFAULT;
            }
            break;

        case MY_RNG_IOCTL_SEED:
            num_bytes = copy_from_user(&data, (unsigned long *)arg, sizeof(unsigned long));
            if(num_bytes != 0) {
                pr_info("Failed to copy data from user space\n");
                return -EFAULT;
            }
            pr_info("Calling io write with data: %lu\n", data);
            iowrite32(data, devmem+4);
            break;

        default:
            return -ENOTTY; // unknown command
    }

    return 0;
}

static struct file_operations my_rng_fops = {
    .unlocked_ioctl = my_rng_ioctl,
};

static int __init my_rng_driver_init(void) {
    devmem = ioremap(DEVICE_BASE_PHYS_ADDR, 4096);

    if(!devmem) {
        printk(KERN_ERR "Failed to map device registers in memory");
        return -1;
    }

    if (register_chrdev(250, "my_rng_driver", &my_rng_fops) < 0) {
        printk(KERN_ERR "Failed to register my_rng_driver\n");
        return -1;
    }

    pr_info("my_rng_driver loaded, registered ioctls 0x%lx (get a random "
        "number) and 0x%lx (seed the generator) \n", MY_RNG_IOCTL_RAND,
        MY_RNG_IOCTL_SEED);
    return 0;
}

static void __exit my_rng_driver_exit(void) {
    unregister_chrdev(250, "my_rng_driver");

    if(devmem)
        iounmap(devmem);

    pr_info("my_rng_driver unloaded\n");
}

module_init(my_rng_driver_init);
module_exit(my_rng_driver_exit);
