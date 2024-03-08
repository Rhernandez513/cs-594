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
#include <linux/stacktrace.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/bitmap.h>
#include <linux/jhash.h>
#include <linux/sched.h>
#include <linux/ktime.h>

static char symbol[KSYM_NAME_LEN] = "perftop_show";
module_param_string(symbol, symbol, KSYM_NAME_LEN, 0644);

/////////////////// HASH TABLE BEGIN ///////////////////////////
/*
 * Let's make our hash table have 2^10 = 1024 bins
 * */
#define MY_HASH_TABLE_BINS 10
static DEFINE_HASHTABLE(myhtable, MY_HASH_TABLE_BINS);

/* Hashtable entry struct */
struct hentry {
	int run_count;
	int pid;
	s64 cumulative_time;
	u32 jenkins_hash;
	ktime_t time_stamp;
	struct task_struct *task;
	struct hlist_node hash;
};
/////////////////// HASH TABLE END ///////////////////////////

#define MAX_STACK_TRACE_DEPTH 256  // Set the depth according to your needs

struct hentry *find_entry_by_pid(int pid);
static int store_value_pid_as_key_hash_table(int pid, u32 jenkins_hash, ktime_t time_stamp, struct task_struct *task, int run_count);
static void destroy_hash_table_and_free(void);

atomic_t atomic_entry_run_count = ATOMIC_INIT(0);

/* For each probe you need to allocate a kprobe structure */
static struct kprobe kp = {
	.symbol_name	= symbol,
};

static int perftop_show(struct seq_file *m, void *v) {
    seq_printf(m, "Hello from perftop_show\n");
	pr_info("Begin Stack Dump\n");
	dump_stack();
	pr_info("End Stack Dump\n");
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

/* kprobe pre_handler: called just before the probed instruction is executed */
static int __kprobes handler_pre(struct kprobe *p, struct pt_regs *regs)
{
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
	int i;
	u32 hash_result;
	ktime_t time_stamp;
	s64 time_delta_ns;
    unsigned int depth;
    unsigned long stack_entries[32];  // Adjust the size as needed
	struct hentry *found_entry;
	struct task_struct *task;
	int pid;

    printk("Hello from kprobe handler_pre for perftop_show \n");

    // Access the task_struct pointer from the "task" field of pt_regs
    task = (struct task_struct *)regs->di;

	// Do something with the task_struct pointer
    pr_info("perftop_show called. task_struct pointer: %p\n", task);

    // Get the PID from the task_struct
    pid = task_pid_nr(task);
	// Print the PID using printk with KERN_INFO
    pr_info("Process ID (PID): %d\n", pid);

    // // Find the entry based on PID
    found_entry = find_entry_by_pid(pid);

    /* Save the stack trace of the calling process */
    depth = stack_trace_save(stack_entries, ARRAY_SIZE(stack_entries), 0);

    /* Print the stack trace with KERN_INFO level and depth */
    printk(KERN_INFO "Stack trace depth: %u\n", depth);

	i = 0;
	while (i < depth) {
		pr_info("stack_entries[%d] = 0x%lx\n", i, stack_entries[i]);
		++i;
	}

	hash_result = jhash(stack_entries, sizeof(stack_entries), 0);
	printk(KERN_INFO "Jenkins Hash Result: %u\n", hash_result);

    time_stamp = ktime_get();
    if (found_entry) {
		pr_info("Entry found for jenkins_hash %u\n", hash_result);

		pr_info("All times in nanoseconds\n");
		pr_info("Last scheduled: %lld ns\n", ktime_to_ns(found_entry->time_stamp));
		pr_info("Time Stamp Counter is currently: %lld ns\n", ktime_to_ns(time_stamp));

		time_delta_ns = ktime_to_ns(ktime_sub(time_stamp, found_entry->time_stamp));
		pr_info("Time elapsed since last scheduled: %lld ns\n", time_delta_ns);

		found_entry->time_stamp = time_stamp;
		pr_info("Updated time_stamp_counter for jenkins_hash: %u\n", hash_result);

		// Read the run count, store in an atomic variable
		atomic_set(&atomic_entry_run_count, found_entry->run_count);

		// Increment and set atomically 
		found_entry->run_count = atomic_add_return(1, &atomic_entry_run_count);

		pr_info("Incremented run_count for jenkins_hash: %u\n", hash_result);
		pr_info("Run count: %d\n", found_entry->run_count);
    } else {
		pr_info("Entry not found for jenkins_hash: %u\n", hash_result);

		store_value_pid_as_key_hash_table(pid, hash_result, time_stamp, task, 1);
		pr_info("Stored new entry, PID: %d\n", pid);
		pr_info("Stored new entry, jenkins_hash: %u\n", hash_result);
		pr_info("All times in nanoseconds\n");
		pr_info("Started timer, Time Stamp Counter was: %lld ns\n", ktime_to_ns(time_stamp));
    }
	return 0;
}

/* kprobe post_handler: called after the probed instruction is executed */
static void __kprobes handler_post(struct kprobe *p, struct pt_regs *regs,
				unsigned long flags)
{
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

static int store_value_pid_as_key_hash_table(int pid, u32 jenkins_hash, ktime_t time_stamp, struct task_struct *task, int run_count)
{
	struct hentry *entry = kmalloc(sizeof(struct hentry), GFP_ATOMIC);

	if (!entry) {
		pr_info("Failed to allocate memory for hentry\n");
		return -ENOMEM;
	}

	entry->task = task;
	entry->pid = pid;
	entry->jenkins_hash = jenkins_hash;
	entry->run_count = run_count;

	hash_add(myhtable, &entry->hash, pid);

	return 0;
}

static void destroy_hash_table_and_free(void)
{
	struct hlist_node *tmp;
	struct hentry *current_elem;
	unsigned bkt;

	hash_for_each_safe(myhtable, bkt, tmp, current_elem, hash) {
		// current_elem->task = NULL;
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
