// SPDX-License-Identifier: GPL-2.0-only
/*
 * Here's a sample kernel module showing the use of kprobes to dump a
 * stack trace and selected registers when pick_next_task_fair() is called.
 *
 * For more information on theory of operation of kprobes, see
 * Documentation/trace/kprobes.rst
 *
 * You will see the trace data in /var/log/messages and on the console
 * whenever pick_next_task_fair() is invoked to create a new process.
 */

#define pr_fmt(fmt) "%s: " fmt, __func__


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/hashtable.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static char symbol[KSYM_NAME_LEN] = "pick_next_task_fair";
module_param_string(symbol, symbol, KSYM_NAME_LEN, 0644);


/* For each probe you need to allocate a kprobe structure */
static struct kprobe kp = {
	.symbol_name	= symbol,
};

struct hentry *find_entry_by_pid(int pid);
static int store_value_hash_table(int pid, int run_count);
static void destroy_hash_table_and_free(void);

/////////////////// HASH TABLE BEGIN ///////////////////////////
/*
 * Let's make our hash table have 2^10 = 1024 bins
 * */
#define MY_HASH_TABLE_BINS 10
static DEFINE_HASHTABLE(myhtable, MY_HASH_TABLE_BINS);

/* Hashtable entry struct */
struct hentry {
	int pid;
	int run_count;
	struct hlist_node hash;
};
/////////////////// HASH TABLE END ///////////////////////////

static int perftop_show(struct seq_file *m, void *v) {
    struct hentry *current_elem;
    unsigned bkt;

    seq_printf(m, "Hello Perftop from kprobe_pick_next_task_fair\n");

    hash_for_each(myhtable, bkt, current_elem, hash) {
        seq_printf(m,"Element PID: %d\n", current_elem->pid);
        seq_printf(m, "Element run_count: %d\n", current_elem->run_count);
    }
    return 0;
}

EXPORT_SYMBOL(perftop_show);

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

atomic_t atomic_entry_run_count = ATOMIC_INIT(0);

/* kprobe pre_handler: called just before the probed instruction is executed */
static int __kprobes handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	struct task_struct *task;
	struct hentry *found_entry;
	int pid;

	pr_info("In handler_pre");
	// At this point, the current task will be scheduled out
	pr_info("using 'current' macro current->comm");
	pr_info("Task scheduled out: %s\n", current->comm);
	pr_info("using 'current' macro current->pid");
	pr_info("PID of the task scheduled out: %d\n", current->pid);

    // Access the task_struct pointer from the "task" field of pt_regs
    task = (struct task_struct *)regs->di;

	// Do something with the task_struct pointer
    pr_info("pick_next_task_fair called. task_struct pointer: %p\n", task);

    // Get the PID from the task_struct
    pid = task_pid_nr(task);
	// Print the PID using printk with KERN_INFO
	pr_info("using the PID from task_pid_nr(task) where task is the task_struct pointer from pt_regs\n");
    pr_info("Process ID (PID): %d\n", pid);

    // // Find the entry based on PID
    found_entry = find_entry_by_pid(pid);

    if (found_entry) {
        pr_info("Entry found for PID %d\n", pid);

		// Read the run count, store in an atomic variable
		atomic_set(&atomic_entry_run_count, found_entry->run_count);

		// Increment and set atomically 
		found_entry->run_count = atomic_add_return(1, &atomic_entry_run_count);
		pr_info("Incremented run_count for PID %d\n", pid);
		pr_info("Run count: %d\n", found_entry->run_count);
    } else {
		pr_info("Entry not found for PID %d\n", pid);
		store_value_hash_table(pid, 1);
		pr_info("Stored new entry for PID %d\n", pid);
    }
#ifdef CONFIG_X86
	pr_info("<%s> p->addr = 0x%p, ip = %lx, flags = 0x%lx\n",
		p->symbol_name, p->addr, regs->ip, regs->flags);
#endif
#ifdef CONFIG_PPC
	pr_info("<%s> p->addr = 0x%p, nip = 0x%lx, msr = 0x%lx\n",
		p->symbol_name, p->addr, regs->nip, regs->msr);
#endif
#ifdef CONFIG_MIPS
	pr_info("<%s> p->addr = 0x%p, epc = 0x%lx, status = 0x%lx\n",
		p->symbol_name, p->addr, regs->cp0_epc, regs->cp0_status);
#endif
#ifdef CONFIG_ARM64
	pr_info("<%s> p->addr = 0x%p, pc = 0x%lx, pstate = 0x%lx\n",
		p->symbol_name, p->addr, (long)regs->pc, (long)regs->pstate);
#endif
#ifdef CONFIG_ARM
	pr_info("<%s> p->addr = 0x%p, pc = 0x%lx, cpsr = 0x%lx\n",
		p->symbol_name, p->addr, (long)regs->ARM_pc, (long)regs->ARM_cpsr);
#endif
#ifdef CONFIG_RISCV
	pr_info("<%s> p->addr = 0x%p, pc = 0x%lx, status = 0x%lx\n",
		p->symbol_name, p->addr, regs->epc, regs->status);
#endif
#ifdef CONFIG_S390
	pr_info("<%s> p->addr, 0x%p, ip = 0x%lx, flags = 0x%lx\n",
		p->symbol_name, p->addr, regs->psw.addr, regs->flags);
#endif
#ifdef CONFIG_LOONGARCH
	pr_info("<%s> p->addr = 0x%p, era = 0x%lx, estat = 0x%lx\n",
		p->symbol_name, p->addr, regs->csr_era, regs->csr_estat);
#endif

	/* A dump_stack() here will give a stack backtrace */
	return 0;
}

/* kprobe post_handler: called after the probed instruction is executed */
static void __kprobes handler_post(struct kprobe *p, struct pt_regs *regs,
				unsigned long flags)
{    
	pr_info("In handler_post");
	// At this point, the current task will be scheduled in
	pr_info("using 'current' macro current->comm");
	pr_info("Task scheduled in: %s\n", current->comm);
	pr_info("using 'current' macro current->pid");
	pr_info("PID of the task scheduled in: %d\n", current->pid);

#ifdef CONFIG_X86
	pr_info("<%s> p->addr = 0x%p, flags = 0x%lx\n",
		p->symbol_name, p->addr, regs->flags);
#endif
#ifdef CONFIG_PPC
	pr_info("<%s> p->addr = 0x%p, msr = 0x%lx\n",
		p->symbol_name, p->addr, regs->msr);
#endif
#ifdef CONFIG_MIPS
	pr_info("<%s> p->addr = 0x%p, status = 0x%lx\n",
		p->symbol_name, p->addr, regs->cp0_status);
#endif
#ifdef CONFIG_ARM64
	pr_info("<%s> p->addr = 0x%p, pstate = 0x%lx\n",
		p->symbol_name, p->addr, (long)regs->pstate);
#endif
#ifdef CONFIG_ARM
	pr_info("<%s> p->addr = 0x%p, cpsr = 0x%lx\n",
		p->symbol_name, p->addr, (long)regs->ARM_cpsr);
#endif
#ifdef CONFIG_RISCV
	pr_info("<%s> p->addr = 0x%p, status = 0x%lx\n",
		p->symbol_name, p->addr, regs->status);
#endif
#ifdef CONFIG_S390
	pr_info("<%s> p->addr, 0x%p, flags = 0x%lx\n",
		p->symbol_name, p->addr, regs->flags);
#endif
#ifdef CONFIG_LOONGARCH
	pr_info("<%s> p->addr = 0x%p, estat = 0x%lx\n",
		p->symbol_name, p->addr, regs->csr_estat);
#endif
}

static int __init kprobe_init(void)
{
	int ret;
	kp.pre_handler = handler_pre;
	kp.post_handler = handler_post;

	ret = register_kprobe(&kp);
	if (ret < 0) {
		pr_err("register_kprobe failed, returned %d\n", ret);
		return ret;
	}
	pr_info("Planted kprobe at %p\n", kp.addr);

	proc_create("perftop", 0, NULL, &perftops_fops);
	pr_info("Created procfc entry at perftop\n");
	return 0;
}

// Function to find an entry in the hash table based on PID
struct hentry *find_entry_by_pid(int pid) {
    struct hentry *entry = NULL;

    hash_for_each_possible(myhtable, entry, hash, pid) {
        if (entry->pid == pid) {
            // Entry found, return the pointer to the struct
            return entry;
        }
    }

    // Entry not found
    return NULL;
}

static int store_value_hash_table(int pid, int run_count)
{

	struct hentry *entry = kmalloc(sizeof(struct hentry), GFP_ATOMIC);
	if (!entry) {
		pr_info("Failed to allocate memory for hentry\n");
		return -ENOMEM;
	}

	entry->pid = pid;
	entry->run_count = run_count;

	hash_add(myhtable, &entry->hash, pid);

	return 0;
}

static void destroy_hash_table_and_free(void)
{
	struct hentry *current_elem;
	unsigned bkt;

	hash_for_each(myhtable, bkt, current_elem, hash) {
		hash_del(&current_elem->hash);
		kfree(current_elem);
	}
}

static void __exit kprobe_exit(void)
{
	destroy_hash_table_and_free();
	unregister_kprobe(&kp);
	pr_info("kprobe at %p unregistered\n", kp.addr);

	remove_proc_entry("perftop", NULL);
	pr_info("Removed procfs entry perftop\n");
}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");
