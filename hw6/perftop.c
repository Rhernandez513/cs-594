#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static int perftop_show(struct seq_file *m, void *v) {
    seq_printf(m, "Hello World\n");
    seq_printf(m, "CS594 HW6_1.1\n");
    return 0;
}

EXPORT_SYMBOL(perftop_show);

/////////////////// HASH TABLE BEGIN ///////////////////////////
/*
 * Let's make our hash table have 2^10 = 1024 bins
 * */
// #define MY_HASH_TABLE_BINS 10
// static DEFINE_HASHTABLE(myhtable, MY_HASH_TABLE_BINS);

/* Hashtable entry struct */
// struct hentry {
// 	int val;
// 	struct hlist_node hash;
// };
// EXPORT_SYMBOL(myhtable);
// EXPORT_SYMBOL(hentry);
/////////////////// HASH TABLE END ///////////////////////////


static int perftop_open(struct inode *inode, struct file *file) {
    return single_open(file, perftop_show, NULL);
}

static const struct file_operations perftops_fops = {
    .owner = THIS_MODULE,
    .open = perftop_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init perftop_init(void) {
    proc_create("perftop", 0, NULL, &perftops_fops);
    return 0;
}

static void __exit perftop_exit(void) {
    remove_proc_entry("perftop", NULL);
}

MODULE_LICENSE("GPL");
module_init(perftop_init);
module_exit(perftop_exit);
