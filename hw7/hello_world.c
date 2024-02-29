#include <linux/module.h>
#include <linux/stacktrace.h>
#include <linux/sched/signal.h>

int hello_init(void) {
    // struct task_struct *task = current;
    unsigned int depth;
    unsigned long stack_entries[32];  // Adjust the size as needed

    printk("Hello world!\n");

    /* Save the stack trace of the calling process */
    // unsigned int depth = stack_trace_save_tsk(task, stack_entries, ARRAY_SIZE(stack_entries), 0);

    depth = stack_trace_save(stack_entries, ARRAY_SIZE(stack_entries), 0);


    /* Print the stack trace with KERN_INFO level and depth */
    printk(KERN_INFO "Stack trace depth: %u\n", depth);
    // doesn't exist
    // print_stack_trace_from_task(task, stack_entries, depth, 0);

    return 0;
}

void hello_exit(void) {
    printk("Goodbye world!\n");
}

MODULE_LICENSE("GPL");
module_init(hello_init);
module_exit(hello_exit);
