#include <linux/module.h>
#include <linux/stacktrace.h>

int hello_init(void) {
    unsigned int depth;
    unsigned long stack_entries[32];  // Adjust the size as needed

    printk("Hello stack_trace_save\n");

    /* Save the stack trace of the calling process */
    depth = stack_trace_save(stack_entries, ARRAY_SIZE(stack_entries), 0);


    /* Print the stack trace with KERN_INFO level and depth */
    printk(KERN_INFO "Stack trace depth: %u\n", depth);

    return 0;
}

void hello_exit(void) {
    printk("Goodbye stack_trace_save!\n");
}

MODULE_LICENSE("GPL");
module_init(hello_init);
module_exit(hello_exit);
