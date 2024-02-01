Summary of past lectures

Get, builld and explore the Linux Kernel
System Call: interface between applications and kernel
Kernel data structures
Kernel modules
Kernel debugging techniques


Recap: kernel debugging techniques
Print debug message: printk()
Assert your code: BUG_ON()
// TODO check slides

Recap debugging with QEMU/gbd
// TODO check slides

Recap: connect and debug with DEMU/gdb
// TODO add info from slides
// goes over setting breakpoints and stepping through code


Hw4 and hw5
Kernel module and data structures
	- Hw4 due this Friday (Feb 2nd)
	- Linked list and 
	- Kernel module
He5 due next Friday (Feb 9th)
	- Hash table, rbtree, and xarray
	- /proc file system
No hesitate to ask for help!

Today's agenda
Process management in Linus kernel
	- Process
	- The process descriptor: task_struct
	- Process creation
	- Threads
	- Process termination


Process
	- A program currently executing in the system
	- A process is composed of 
		○ Context
		○ Parent process
		○ CPU registers
		○ Program code (i.e. text section)
		○ State of memory segments (data, stack, etc)
		○ Kernel resources (open files, pending signals, etc)
		○ threads

Process from user-space view

	- pid_t fork(void)
		○ Creates a new process by duplicating the calling processed
	- Int execv(const char *path, cont char *arg, …)
		○ Replaces the current processes image with now process image
	- pid_t wait (int *wstatus)
		○ Wait for state changes in a child of the calling processes
		○ The child terminated; the child was stopped or resumed by a signal

Task_struct
Task_struct is dynamically allocated at heap because of potential exploit when overflowing the kernel stack
For efficient access of current task_struct, kernel maintains per-CPU variable, named current_task
	- Use current to get current_task


Process identifier (PID): pid_t
	Maximum is 32768 (int)
	Can be increased to 4 millions
	
	
Process status: task -> _state

TASK_RUNNING
	- A task is runnable (running or in a per-CPU scheduler run queue)
	- A task could be in user or kernel space

__TASK_TRACED
		○ Traced by another process (i.e., debugger)
	- __TASK_STOPPED
		○ Not running nor wating

Producer - consumer example

	- Producer
		○ Generate an event and wake up a consumer
	- Consumer
		○ Check if there is an event
		○ // TODO check slides for more info

Sleeping in the kernel

// TODO check slides for more info


Process context
	- The Kernel can execute in a process context or interrupt context
	- Current is meaningful only when the kernel executes in a process context such as executing a system call
	- Interrupt has its own context 

Process family tree
Fork-based process creation
	- My parent taskL: current->parent
	- My children tasks: current->children
	- Siblings under the parent: current->siblings
	- List of all tasks in the system: current->tasks
	- Macros to easy to explore:
		○ Next_task(t), for_each_process(t)

Process creation
	- Linux does not implement creating tasks from nothing (spawn or CreateProcess)
	- Fork() and exec()
		○ Fork() creates a child, copy of the parent processes
			§ Only PID, PPID, and some resources / stats differ
		○ Exec() loads a process address space a new executable
	- Q: then how to efficiently create a copy of the parent processes
		○ Copy on write, lazy copy until a write is needed then copy the mem into child process
Copy-on-Write (CoW)
	- On fork(), Linux duplicates the parent page tables and creates a new process descriptor
		○ Change page table access bits to read-only
		○ When a page is accessed for write ops, that page is copied and the corresponding page table entry is changed to read-write
	- Fork() is fast by delaying or altogether preventing copying of data
	- Fork() saves memory by sharing read-only pages among descendants 


Fork
	Copy_process()
		- Calls sched_fork() to set the child state to TASK_NEW
		- Copies parent information such as files, signal handles, etc.
		- Gets a new PID using alloc_pid()
		- Returns a pointer to the new child task_struct
	Finally, _do_fork() calls wake_up_new_task()
	- The new child task becomes TASK_RUNNING

Thread
	- Threads are concurrent flows of execution belonging to the same aprogram sharing the same address space
	- // TODO GET DIAGRAM FROM SLIDES


Kernel Thread

Used to perform background tasks 

// TODO get info from slides

To create a kernel thread, use kthread_create()



Process Termination
	- Termination on invoking the exit() system call
		- Can be implicitly inserted by the compiler on return from main()
		- Sys_exit() calls do_exit()
	Do_exit() (kernel/exit.c)
		- Calls exit_signals() which set the PF_EXITING flag in the task_struct
		- Set the exit code in the exit_code field of the task_struct, which will be retreived by the parent
		- Calls exit_mm() to release the mm_struct of the task
		- Calls exit_sem(): if the process is queued waiting for a semaphore, it is dequeued here
		- Calls exit_files(). // TODO look at slides
	- Calls exit_notify()
		- Sends signal to parent
		// TODO get info from slides


Zombie (or parentless) process
	- Q: What happens if a parent tasks exits before it's child?
	- A child task must be reparented
	- Exit_notify() calls forget_original_parent(), that calls find_new_reaper()
		- Returns the task_struct of another task in the thread group if it exits, otherwise init
		- Then, all the children of the currently dying task are reparented to the reaper


Further reading
Book: LKD3: Chapter 3 Process Management

// Link in Slides 

Kernel Korner - Sleeping in the Kernel
Exploiting Stack Overflows in the Linux Kernel

