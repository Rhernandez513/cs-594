#include <linux/ioctl.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>


#define LKP_ENC_IOCTL_RAND _IOR('q', 1, unsigned int)
#define LKP_ENC_IOCTL_SEED _IOW('q', 1, unsigned int)

#define DEVICE_BASE_PHYS_ADDR 0xfebf2000

void *devmem = 0x0;
unsigned long data = 0;

static long lkp_enc_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    unsigned long num_bytes = 0;

    if(!devmem) {
        printk(KERN_ERR "Failed to map device registers in memory");
        return -1; // memory not accessible
    }

    switch (cmd) {
        case LKP_ENC_IOCTL_RAND:
            data = (unsigned long) ioread32(devmem);
            num_bytes = copy_to_user((unsigned long *)arg, &data, sizeof(unsigned long));
            if(num_bytes != 0) {
                pr_info("Failed to copy data to user space\n");
                return -EFAULT;
            }
            break;

        case LKP_ENC_IOCTL_SEED:
            num_bytes = copy_from_user(&data, (unsigned long *)arg, sizeof(unsigned long));
            if(num_bytes != 0) {
                pr_info("Failed to copy data from user space\n");
                return -EFAULT;
            }
            iowrite32(data, devmem+4);
            break;

        default:
            return -ENOTTY; // unknown command
    }

    return 0;
}

static struct file_operations lkp_enc_fops = {
    .unlocked_ioctl = lkp_enc_ioctl,
};

static int __init lkp_enc_driver_init(void) {
    devmem = ioremap(DEVICE_BASE_PHYS_ADDR, 4096);

    if(!devmem) {
        printk(KERN_ERR "Failed to map device registers in memory");
        return -1;
    }

    if (register_chrdev(251, "lkp_enc_driver", &lkp_enc_fops) < 0) {
        printk(KERN_ERR "Failed to register lkp_enc_driver\n");
        return -1;
    }

    pr_info("lkp_enc_driver loaded, registered ioctls 0x%lx (get a random "
        "number) and 0x%lx (seed the generator) \n", LKP_ENC_IOCTL_RAND,
        LKP_ENC_IOCTL_SEED);
    return 0;
}

static void __exit lkp_enc_driver_exit(void) {
    unregister_chrdev(251, "lkp_enc_driver");

    if(devmem)
        iounmap(devmem);

    pr_info("lkp_enc_driver unloaded\n");
}

module_init(lkp_enc_driver_init);
module_exit(lkp_enc_driver_exit);
