#include <linux/ioctl.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>


#define LKP_ENC_IOCTL_KEYREAD _IOR('q', 1, unsigned int)
#define LKP_ENC_IOCTL_KEYWRITE _IOW('q', 1, unsigned int)

#define LKP_ENC_IOCTL_CHARREAD _IOR('q', 2, unsigned int)
#define LKP_ENC_IOCTL_CHARWRITE _IOW('q', 2, unsigned int)

#define DEVICE_BASE_PHYS_ADDR 0xfebf2000

#define MAX_LKP_ENC_BUFFER_SIZE 128

void *lkp_enc_devmem = 0x0;
unsigned long lkp_enc_data = 0;
char str_data [MAX_LKP_ENC_BUFFER_SIZE];

static long lkp_enc_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int i = 0;
    unsigned long num_bytes = 0;

    if(!lkp_enc_devmem) {
        printk(KERN_ERR "Failed to map device registers in memory");
        return -1; // memory not accessible
    }

    switch (cmd) {
        case LKP_ENC_IOCTL_KEYREAD:
            lkp_enc_data = (unsigned long) ioread32(lkp_enc_devmem);
            printk("lkp_enc_data from ioread32 as unsigned long: %lu", lkp_enc_data);

            num_bytes = copy_to_user((unsigned long *)arg, &lkp_enc_data, sizeof(unsigned long));
            if(num_bytes != 0) {
                pr_info("Failed to copy data to user space\n");
                return -EFAULT;
            }
            break;

        case LKP_ENC_IOCTL_KEYWRITE:
            num_bytes = copy_from_user(&lkp_enc_data, (unsigned long *)arg, sizeof(unsigned long));
            if(num_bytes != 0) {
                pr_info("Failed to copy data from user space\n");
                return -EFAULT;
            }
            iowrite32(lkp_enc_data, lkp_enc_devmem);
            break;

        case LKP_ENC_IOCTL_CHARREAD:
            // We start from 1 as the first byte is the encryption key
            i = 1;
            while ((char) lkp_enc_data != '\0') {
                lkp_enc_data = ioread32(lkp_enc_devmem+i);
                printk("data returned from ioread32 as char: %c\n", (char) lkp_enc_data);
                str_data[i-1] = (char) lkp_enc_data;
                i++;
            }

            num_bytes = copy_to_user((char *)arg, str_data, sizeof(str_data));
            if (num_bytes != 0) {
                pr_info("Failed to copy data to user space\n");
                return -EFAULT;
            }
            break;

        case LKP_ENC_IOCTL_CHARWRITE:
            num_bytes = copy_from_user(str_data, (char *)arg, sizeof(str_data));
            if(num_bytes != 0) {
                pr_info("Failed to copy data from user space\n");
                return -EFAULT;
            }
            i = 0;
            while(str_data[i] != '\0') {
                // We start from 1 as the first byte is the encryption key
                iowrite32((int) str_data[i], (lkp_enc_devmem + i + 1));
                i++;
            }
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

    if (register_chrdev(251, "lkp_enc_driver", &lkp_enc_fops) < 0) {
        printk(KERN_ERR "Failed to register lkp_enc_driver\n");
        return -1;
    }

    pr_info("lkp_enc_driver loaded, registered ioctls 0x%lx (read "
        "the encryption key) and 0x%lx (set the encryption key) \n",
        LKP_ENC_IOCTL_KEYREAD, LKP_ENC_IOCTL_KEYWRITE);

    pr_info("lkp_enc_driver loaded, registered ioctls 0x%lx (read "
        "the string) and 0x%lx (write the string) \n",
        LKP_ENC_IOCTL_CHARWRITE, LKP_ENC_IOCTL_CHARREAD);
    return 0;
}

static void __exit lkp_enc_driver_exit(void) {
    unregister_chrdev(251, "lkp_enc_driver");

    if(lkp_enc_devmem)
        iounmap(lkp_enc_devmem);

    pr_info("lkp_enc_driver unloaded\n");
}

module_init(lkp_enc_driver_init);
module_exit(lkp_enc_driver_exit);

