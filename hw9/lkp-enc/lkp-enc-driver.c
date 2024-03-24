#include <linux/ioctl.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define LKP_ENC_IOCTL_RKEY _IOR('q', 1, unsigned int)
#define LKP_ENC_IOCTL_WKEY _IOW('q', 1, unsigned int)

#define LKP_ENC_IOCTL_RCH _IOR('q', 2, unsigned int)
#define LKP_ENC_IOCTL_WCH _IOW('q', 2, unsigned int)

#define DEVICE_BASE_PHYS_ADDR 0xfebf2000

void *lkp_enc_devmem = 0x0;

static long lkp_enc_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {

    switch (cmd) {
        
        case LKP_ENC_IOCTL_RKEY :
            break;

        case LKP_ENC_IOCTL_WKEY :
            break;

        case LKP_ENC_IOCTL_RCH :
            break;

        case LKP_ENC_IOCTL_WCH :
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
    lkp_enc_devmem = ioremap(DEVICE_BASE_PHYS_ADDR, 4096);

    if(!lkp_enc_devmem) {
        printk(KERN_ERR "Failed to map device registers in memory");
        return -1;
    }

    if (register_chrdev(250, "lkp_enc_driver", &lkp_enc_fops) < 0) {
        printk(KERN_ERR "Failed to register lkp_enc_driver\n");
        return -1;
    }

    printk("lkp_enc_driver loaded, registered ioctls 0x%lx (get enc key "
        ") and 0x%lx (write enc key) \n", LKP_ENC_IOCTL_RKEY,
        LKP_ENC_IOCTL_WKEY);

    printk("lkp_enc_driver loaded, registered ioctls 0x%lx (get chars "
        ") and 0x%lx (write chars) \n", LKP_ENC_IOCTL_RCH,
        LKP_ENC_IOCTL_WCH);
    return 0;
}

static void __exit lkp_enc_driver_exit(void) {
    unregister_chrdev(250, "lkp_enc_driver");

    if(lkp_enc_devmem)
        iounmap(lkp_enc_devmem);

    printk(KERN_INFO "lkp_enc_driver unloaded\n");
}

module_init(lkp_enc_driver_init);
module_exit(lkp_enc_driver_exit);
