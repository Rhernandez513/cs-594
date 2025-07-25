#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

static char *int_str;

/* [X1: point 1]
 * 
 * MODULE_ here is a kernel module convention 
 * 
 * First we have the license of the module.
 * Followed by a kernel module convention to specify the author of the module.
 * Followed by a brief description of the module.
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[ROBERT DAVID HERNANDEZ]");
MODULE_DESCRIPTION("LKP Homework 4");

/* [X2: point 1]
 *
 * 
 * This is a macro that is used to define a module parameter.
 * the parameter is int_str, which is a char pointer, indicated
 * by "charp"
 * 
 * The following three items are the permissions for the module
 * they are S_IRUSR, S_IRGRP, and S_IROTH which stand for 
 * read permission for the owner, read permission for the group, and
 * others
 *
 */
module_param(int_str, charp, S_IRUSR | S_IRGRP | S_IROTH);

/* [X3: point 1]
 * Explain following in here.
 *
 * this is a linux kernel convention for module parameter description
 * here we describe the int_str parameter as a comma separated list of integers
 * 
 */
MODULE_PARM_DESC(int_str, "A comma-separated list of integers");

/* [X4: point 1]
 * Explain following in here.
 *
 * 
 * LIST_HEAD is part of the linux kernel API for linked lists 
 * and in this instance initializes a list_head named mylist
 */
static LIST_HEAD(mylist);

/* [X5: point 1]
 * Explain following in here.
 *
 * Here we define a struct named entry that contains an integer value
 * and a list_head named list
 */
struct entry {
	int val;
	struct list_head list;
};

static int store_value(int val)
{
	/* [X6: point 5]
	 *
	 * Allocate a struct entry of which val is val
	 * and add it to the tail of mylist.
	 * Return 0 if everything is successful.
	 * Otherwise (e.g., memory allocation failure),
	 * return corresponding error code in error.h (e.g., -ENOMEM).
	 */

	struct entry *entry_instance;
	entry_instance = kmalloc(sizeof(struct entry), GFP_KERNEL);

	if (!entry_instance) {
		return -ENOMEM;
	}

	entry_instance->val = val;

	list_add_tail(&entry_instance->list, &mylist);

	return 0;
}

static void test_linked_list(void)
{
	/* [X7: point 5]
	 * Print out value of all entries in mylist.
	 */
	struct entry *entry_instance;
	list_for_each_entry(entry_instance, &mylist, list) {
		printk(KERN_INFO "val = %d\n", entry_instance->val);
	}
}


static void destroy_linked_list_and_free(void)
{
	/* [X8: point 5]
	 * Free all entries in mylist.
	 */
	struct entry *next;
	struct entry *entry_instance;

	list_for_each_entry_safe(entry_instance, next, &mylist, list) {
		list_del(&entry_instance->list);
		kfree(entry_instance);
	}
}


static int parse_params(void)
{
	int val, err = 0;
	char *p, *orig, *params;


	/* [X9: point 1]
	 * 
	 * This is part of the Linux Kernel API that returns a new string
	 * pointer that is a duplicate of the string passed to it
	 * It also accepts a flag, in this fase GFP_KERNEL, which is a
	 * typically used GFP_MASK flag that is used to specify the type of
	 * memory allocation allowing the allowcation to sleep if necessary
	 * 
	 * if the allocation and duplication fails, we return -ENOMEM
	 * 
	 * the the allocation and duplication is successful, we 
	 * assign the pointer to the orig variable defined earlier in the function
	 */
	params = kstrdup(int_str, GFP_KERNEL);
	if (!params)
		return -ENOMEM;
	orig = params;

	/* [X10: point 1]
	 * Explain following in here.
	 * 
	 * strsep is a linux kernel API function that is used to tokenize
	 * a string. It takes a pointer to a string and a delimiter and returns
	 * the next token in the string. It also replaces the delimiter with a null
	 * 
	 * If the end of the string is reached, it returns NULL
	 * 
	 * So here we loop while the pointer returned is not NULL
	 */
	while ((p = strsep(&params, ",")) != NULL) {
		if (!*p)
			continue;
		/* [X11: point 1]
		 * 
		 * the above if statement checks if the p is pointing to a null
		 * if it is, it skips to the end of the loop and allows the
		 * while check to find a null and exit the loop
		 * 
		 * The below statement converts the string to an integer
		 * using the Linux Kernel API function kstrtoint
		 * 
		 * Here p is the pointer to the string, provided by the strsep function
		 * 0 is instruction to the function to autodetect the base of the number
		 * numbers beginning with 0x are treated as hex, 0 as octal, 
		 * and 1-9 as decimal
		 * 
		 * As val is an interget value, &val
		 * is the pointer to the integer that the function manipulate
		 * and return
		 * 
		 * If the kstrtoint function fails, it returns an error code
		 * 
		 * In the case that the err is not null, we break the loop
		 */
		err = kstrtoint(p, 0, &val);
		if (err)
			break;

		/* [X12: point 1]
		 * 
		 * Add a value to the end of mylist using the store_value function
		 * 
		 * If there is an error, we break the loop
		 */
		err = store_value(val);
		if (err)
			break;
	}

	/* [X13: point 1]
	 *
	 * Here we free the memory allocated for the orig string
	 * using the Linux Kernel API function kfree
	 * 
	 * We then return the error code, which is 0 if there is no error
	 */
	kfree(orig);
	return err;
}

static void run_tests(void)
{
	/* [X14: point 1]
	 * 
	 * This reads as though it's intent is simple to 
	 * run the functions prefixed with test_
	 */
	test_linked_list();

}

static void cleanup(void)
{
	/* [X15: point 1]
	 * Explain following in here.
	 * 
	 * 
	 * This is a linux kernel convention for logging
	 * 
	 * Followed by a call to destroy_linked_list_and_free
	 * which by the function name of cleanup, seems to be 
	 * the modules memory and resource release function
	 */
	printk(KERN_INFO "\nCleaning up...\n");

	destroy_linked_list_and_free();
}

static int __init hw4_init(void)
{
	int err = 0;

	/* [X16: point 1]
	 *
	 * This is a linux kernel convention for logging
	 * guarded by a check for the int_str parameter
	 * which is a parameter to the module's Linux Kernel API
	 * 
	 * If the parameter is not present the module will exit
	 * after printing a message
	 */
	if (!int_str) {
		printk(KERN_INFO "Missing \'int_str\' parameter, exiting\n");
		return -1;
	}

	/* [X17: point 1]
	 * 
	 * This is the a call to the module's input parameter parser
	 * Whwere if there is an error condition it will return the error
	 * triggering the module's exit function
	 */
	err = parse_params();
	if (err)
		goto out;

	/* [X18: point 1]
	 * Explain following in here.
	 * 
	 * This is the modules primary function that is called after 
	 * initialization completes, in this instance the module calls
	 * all of the tests that it has registered
	 * 
	 */
	run_tests();
out:
	/* [X19: point 1]
	 * 
	 * This is the exit path of the module if there is an error
	 * it will call the cleanup function and return the error
	 */
	cleanup();
	return err;
}

static void __exit hw4_exit(void)
{
	/* [X20: point 1]
	 * Explain following in here.
	 * 
	 * 
	 * This seems to be the modules exit function 
	 * and it doesn't seem to do anything
	 * other than serve as a hook into the module itself
	 * for any last minute cleanup or resource release
	 */
	return;
}

/* [X21: point 1]
 * Explain following in here.
 * 
 * This is a hook into the Linux Kernel Module API that allows
 * the system to calls the module's initialization function
 */
module_init(hw4_init);

/* [X22: point 1]
 * Explain following in here.
 * 
 * This is a hook into the Linux Kernel Module API that allows
 * the system to calls the module's exit function
 */
module_exit(hw4_exit);
