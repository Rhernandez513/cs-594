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
MODULE_AUTHOR("Robert D. Hernandez SOLUTION");
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
	 * 
	 * In this case, I have added the value to the end of the array.
	 */

	xarrentry *entry = kmalloc(sizeof(struct xarrentry), GFP_KERNEL);
	if (!entry) {
		printk(KERN_INFO "Failed to allocate memory for xarrentry\n");
		return -ENOMEM;
	}
	entry->val = val;

	// Add the element to the end of the XArray
	unsigned long index = 0;
	struct xarrentry *old_entry;
	while (1) {
		old_entry = xa_cmpxchg(&myxarray, index, NULL, entry, GFP_KERNEL);
		if (!old_entry)
			break;

		index++;
	}

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

	rbentry *entry = kmalloc(sizeof(struct rbentry), GFP_KERNEL);
	if (!entry) {
		printk(KERN_INFO "Failed to allocate memory for rbentry\n");
		return -ENOMEM;
	}

	entry->val = val;

	rb_insert(&myrbtree, &entry->rbnode);

	return 0;
}

static int store_value_hash_table(int val)
{
	/* TODO: [X3: 5 points]
	 * Allocate a new hentry struct
	 * and add the value to the hashtable
	 * */

	hentry *entry = kmalloc(sizeof(struct hentry), GFP_KERNEL);
	if (!entry) {
		printk(KERN_INFO "Failed to allocate memory for hentry\n");
		return -ENOMEM;
	}

	entry->val = val;	
	hash_add(myhtable, &entry->hash, val);

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

	hash_for_each(myhtable, bkt, current_elem, hash) {
		hash_del(&current_elem->hash);
		kfree(current_elem);
	}
}

static void destroy_rbtree_and_free(void)
{

	struct rb_node* node;
	struct rb_node* next;

	/* TODO: [X5: 5 points]
	 * Iterate over the rbtree,
	 * delete the node, and free the node memory
	 * */

	node = rb_first(&myrbtree);

	while (node) {
		next = node->rb_next(node);
		rb_erase(node, &myrbtree);
		kfree(node);
		node = next;
	}
}

static void destroy_xarray_and_free(void)
{

	unsigned long index;
	struct xarrentry* entry;

	/* TODO: [X6: 5 points]
	 * Iterate over the xarray,
	 * delete the element, and free the memory
	 * */

	xa_for_each(&myxarray, index, entry) {
		if (entry) {
			xa_erase(&myxarray, index);
			kfree(entry);
		}
	}
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
 *
 * this code defines an open operation for a /proc file entry 
 * inode is for the files permissions and other metadata
 * */
static int kern_ds_proc_open(struct inode *inode, struct  file *file)
{
	/* single_open is a helper the Linux Kernel provides 
	*  as a helper function for opening a file in /proc
	*
	* file is a pointer to the file struct that is being opened
	* 
	* kern_ds_proc_show is the function that will be called when 
	* the file is opened
	* 
	* NULL is that we are not passing data to kern_ds_proc_show 
	* */
	  return single_open(file, kern_ds_proc_show, NULL);
}

/* [X8: 1 point]
 * Explain the following code here.
 *
 * this code is for file operations "proc_ops" for a /proc 
 * file entry
 * 
 * struct proc_ops is to ID the file operations for a /proc 
 * file * and uses pointers to functions to define the operations
 * 
 * each item here: open, read, lseek, and release map
 * to function pointers
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
	 *
	 * 
	 * proc_create is a kernel helper function to 
	 * create a /proc file entry
	 * 
	 * "hw5" is the name of the file
	 * 
	 * 0 is the mode AKA permissions of the file
	 * 
	 * NULL is the parent directory of the file, 
	 * to indicate the /proc root
	 * 
	 * &kern_ds_proc_fops is the struct that holds
	 * the function pointers to the file operations
	 * for the /proc entry
	 * */
	proc_create("hw5", 0, NULL, &kern_ds_proc_fops);

	return err;
}

static void __exit hw5_exit(void)
{
	cleanup();

	/* [X10: 1 point]
	 * Explain the following code here.
	 *
	 * Here we are removing the /proc file entry
	 * 
	 * "hw5" is the name of the file
	 * 
	 * NULL is the parent directory of the file,
	 * indidate the /proc root
	 * */
	remove_proc_entry("hw5", NULL);

	return;
}

module_init(hw5_init);
module_exit(hw5_exit);
