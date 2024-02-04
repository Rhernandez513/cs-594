#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/hashtable.h>
#include <linux/types.h>
#include <linux/rbtree.h>
#include <linux/radix-tree.h>

/* This is where we will write our characters to
 * Don't want to deal with VLAs or heavy string manipulation
 * We assume that the input doesn't surpass 256 characters
 */
#define PRINT_BUFF_SIZE 256
static char print_buff[PRINT_BUFF_SIZE];
static int print_buff_head = 0;

static char *int_str;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("XXXXXXX SOLUTION");
MODULE_DESCRIPTION("LKP hw 5");

module_param(int_str, charp, S_IRUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(int_str, "A comma-separated list of integers");

/////////////////// HASH TABLE BEGIN ///////////////////////////
/*
 * Let's make our hash table have 2^10 = 1024 bins
 * */
#define MY_HASH_TABLE_BINS 10
static DEFINE_HASHTABLE(myhtable, MY_HASH_TABLE_BINS);

/* Hashtable entry struct */
struct hentry {
	int val;
	struct hlist_node hash;
};
/////////////////// HASH TABLE END ///////////////////////////

/////////////////// RBTree BEGIN ///////////////////////////
// Define an RBTree
static struct rb_root myrbtree = RB_ROOT;

// Define an rbtree entry
struct rbentry {
	int val;
	struct rb_node rbnode;
};

/////////////////// RBTree END ///////////////////////////

/////////////////// XARRAY BEGIN///////////////////////////
static DEFINE_XARRAY(myxarray);

struct xarrentry{
	int val;
};
/////////////////// XARRAY END ///////////////////////////

static int store_value_xarray(int val)
{
	/* TODO: [X1: 5 point]
	 * Allocate a struct of xarrentry
	 * and store the value to the myxarray.
	 */

	return 0;
}

/* https://www.kernel.org/doc/Documentation/rbtree.txt */
static int store_value_rbtree(int val)
{
	/* TODO: [X2: 5 ponints]
	 * Allocate a new rbentry struct
	 * and insert the new node at the root of the tree
	 * and trigger a rebalance
	 * */

	return 0;
}

static int store_value_hash_table(int val)
{
	/* TODO: [X3: 5 points]
	 * Allocate a new hentry struct
	 * and add the value to the hashtable
	 * */

	return 0;
}


static int store_value(int val)
{
	int retval;
	retval = store_value_hash_table(val);
	if (retval) { return retval; }

	retval = store_value_rbtree(val);
	if (retval) { return retval; }

	retval = store_value_xarray(val);
	if (retval) { return retval; }

	return 0;
}

static void add_val_to_print_buff(int val)
{
	int printed;
	printed = sprintf(print_buff+print_buff_head, "%d, ", val);
	print_buff_head += printed;

	return;
}

static void clear_print_buff(void)
{
	print_buff[0] = '\0';
	print_buff_head = 0;
}

static void test_hash_table(void)
{
	// Pointer used to hold the current item
	// while iterating
	struct hentry *current_elem;
	unsigned bkt;
	clear_print_buff();

	strcpy(print_buff, "Hash Table: \0");
	print_buff_head = 12;

	hash_for_each(myhtable, bkt, current_elem, hash) {
		add_val_to_print_buff(current_elem->val);
	}

	// Let's get rid of the last comma and space
	if (print_buff_head >= 2) {
		print_buff[print_buff_head-2] = '\0';
	}

}

static void test_rbtree(void)
{
	struct rb_node* node;

	clear_print_buff();
	strcpy(print_buff, "Red-Black Tree: \0");
	print_buff_head = 16;

	for(node = rb_first(&myrbtree); node; node=rb_next(node)) {
		struct rbentry* curr = rb_entry(node, struct rbentry, rbnode);
		add_val_to_print_buff(curr->val);
	}

	// Let's get rid of the last comma and space
	if (print_buff_head >= 2) {
		print_buff[print_buff_head-2] = '\0';
	}

}

static void test_xarray(void)
{
	unsigned long index;
	struct xarrentry* entry;

	clear_print_buff();
	strcpy(print_buff, "XArray: \0");
	print_buff_head = 8;

	xa_for_each(&myxarray, index, entry) {
		if (entry) {
			add_val_to_print_buff(entry->val);
		}
	}

	// Let's get rid of the last comma and space
	if (print_buff_head >= 2) {
		print_buff[print_buff_head-2] = '\0';
	}
}

static void destroy_hash_table_and_free(void)
{

	struct hentry *current_elem;
	unsigned bkt;

	/* TODO: [X4: 5 points]
	 * Iterate over the hash table,
	 * delete the node, and free the entry memory
	 * */

}

static void destroy_rbtree_and_free(void)
{

	struct rb_node* node;
	struct rb_node* next;

	/* TODO: [X5: 5 points]
	 * Iterate over the rbtree,
	 * delete the node, and free the node memory
	 * */

}

static void destroy_xarray_and_free(void)
{

	unsigned long index;
	struct xarrentry* entry;

	/* TODO: [X6: 5 points]
	 * Iterate over the xarray,
	 * delete the element, and free the memory
	 * */

}

static int parse_params(void)
{
	int val, err = 0;
	char *p, *orig, *params;

	params = kstrdup(int_str, GFP_KERNEL);
	if (!params)
		return -ENOMEM;
	orig = params;

	while ((p = strsep(&params, ",")) != NULL) {
		if (!*p)
			continue;

		err = kstrtoint(p, 0, &val);
		if (err)
			break;

		err = store_value(val);
		if (err)
			break;
	}

	kfree(orig);
	return err;
}

static void run_tests(void)
{
	test_hash_table();
	printk(KERN_INFO "%s\n", print_buff);
	test_rbtree();
	printk(KERN_INFO "%s\n", print_buff);
	test_xarray();
	printk(KERN_INFO "%s\n", print_buff);
}

static void cleanup(void)
{
	destroy_hash_table_and_free();
	destroy_rbtree_and_free();
	destroy_xarray_and_free();
}

static int kern_ds_proc_show(struct seq_file *m, void *v)
{
	test_hash_table();
	seq_printf(m, "%s\n", print_buff);
	test_rbtree();
	seq_printf(m, "%s\n", print_buff);
	test_xarray();
	seq_printf(m, "%s\n", print_buff);
	return 0;
}

/* [X7: 1 point]
 * Explain the following code here.
 * */
static int kern_ds_proc_open(struct inode *inode, struct  file *file)
{
	  return single_open(file, kern_ds_proc_show, NULL);
}

/* [X8: 1 point]
 * Explain the following code here.
 * */
static const struct proc_ops kern_ds_proc_fops = {
	  .proc_open = kern_ds_proc_open,
	  .proc_read = seq_read,
	  .proc_lseek = seq_lseek,
	  .proc_release = single_release,
};

static int __init hw5_init(void)
{
	int err = 0;

	if (!int_str) {
		printk(KERN_INFO "Missing \'int_str\' parameter, exiting\n");
		return -1;
	}

	err = parse_params();
	if (err) {
		cleanup();
	}

	run_tests();

	/* [X9: 1 point]
	 * Explain the following code here.
	 * */
	proc_create("hw5", 0, NULL, &kern_ds_proc_fops);

	return err;
}

static void __exit hw5_exit(void)
{
	cleanup();

	/* [X10: 1 point]
	 * Explain the following code here.
	 * */
	remove_proc_entry("hw5", NULL);

	return;
}

module_init(hw5_init);
module_exit(hw5_exit);
