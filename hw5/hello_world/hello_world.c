#include <linux/module.h>

int init(void) {
    printk("Hello world\n");
    return 0;
}

void exit(void) {
    printk("Goodbye world!\n");
}

MODULE_LICENSE("GPL");
module_init(init);
module_exit(exit);
