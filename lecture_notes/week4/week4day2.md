# Process Scheduling

Summary of past lectures
Tools: building, exploring, and debugging Linux Kernel

Core kernel infrastructure
- Syscalls, module, kernel data structures

Process management

Recap: process status: task ->_state
// TODO get lecture slide

Recap: process creation and termination

Fork() and exec()
- Fork() creates a child copy of the parent process
  - Copy-on-Write --memory (page tables)
  - Through clone() system call (flags indicating sharing -- thread)
- Exec() loads into a process adddress space a new executable

Termination on invoking the exit() sytem call
- Can be implicitly inserted by the compiler
- // TODO ge the rest of this from slide

Recap: kernel thread

 Used to performa background ops in therkenel
- Simialr to user space threads

To create user kthread_create()
// TODO get the rest of this slide from lecture


Today's agenda:
What is process scheduling?
History of Linux CPU scheduler
// TODO get the rest of this slide from lecture


## Process scheduler

Decides which process runs next, when and for how long

Responsible for making the best use of the processor (CPU)
- e.g. Do not waste CPU cycles for waiting process
- e.g.g, give higher priority to higher-priority processes
- e.g., do not starve low-priority processes


## Multitasking
Simultaneously interleaved execution of more than one process

Single core
// TODO GET the rest of this slide

Multi core 
// TODO GET the rest of this slide



### Types of multitasking OS

Cooperative multitasking: old Oses (e.g., Windows 3.1) and few language runtimes (e.g., Golang)
- A processs does not stop running until it decides to yield CPUI
- The OS cannot enforce fair scheduling

Preemtive multitasking: almost all modern Oses
- The OS can interrupt the execution of a process (i.e. premption)
- After the process expires its time-slice
- Which is decided by process-priority

// TODO get image of 3 different process types from slides


### I/O vs CPU-bound tasks

Scheudling plicyu: a set of rules determining what runs when
I/O-bound processes
- Spend most of their time waiting for I/O: disk, network, keyboard, mouse, etc.
- Runs for only short duration
- Response time is important

CPU-bound processes
- Heavy use of CPU: MATLAB, scientitific compurations, etc.
- Caches stay hot when they run for a long time

Linux process priority
Linux has two priority ranges
- Nice value: ranges from -20 to +19 (default is 0)
  - Higher values of nice means lower priotiy
  - e.g., nice -n 5 vim; sudo renice -n -5 -p $(pidof vim)
- Real time priority of 0 to 99
- // TODO GET the rest of this slide


Scheduling policy: time slice

Time slice: the period for which a process is allowed to run uninterrupted in a preemptive multitasking OS

Defining the default time slice is an absolute way is tricky:
- Too long -> bad interactive performance
- Too short -> high context switching overhead

Example: Real-time Round Robin Scheduling (SCHED_RR)

$ cat /proc/
// TODO GET the rest of this slide


### Scheduling policy example

Two tasks in the system:
- Text editor: I/O bound, latency sensitive (interactive)
- Video encoder: CPU-bound, background job

### Scheduling goals
- Text: editor
    - When ready to run, need to reempot the video encoder for interactive performance
- Video encoder
    - Run as long as possible for better CPU cache usage

Gives higher prioritty to text editor
- Not because 
- // TODO GET the rest of this slide

Policy: time slice in Linux CFS

### CFS: Completely fair scheduler

Linux CFS does not use an absolute time-slice
- The time slice a process receives is a function of the load of the system (i.e., a proportion of the CPU)
- In addition, that time slice is weighted by the process priority
- When a process P becomes runnable:
   - P will preempt the currently running process C if P consumer a smaller proportion of the CPI than C

### Policy: example in Linux CFS

CFS guarantees the text editor a specific porportion of CPU time
- CFS keeps track of the actual CPU time used by each program

e.g. text editor: video encoder = 50% : 50%
- The text editor mostly sleeps waitign for suer input and the video encoder keeps running until preempted
- // TODO GET the rest of this slide


Policy: example in Linux CFS
    // TODO GET the IMAGES for this slide
    
    
Linux CFS Design 
    At each moment, each process of the same priority has received the examct same amount of CPU time
    If we could run n tasks in parallel on a CPU, give each 1/n of the CPU processing power
    CFS runs a process for some time, then swaps it for the runnabvle process that has run the least
    
    
For homework, we will be implementing some type of kernel module to profile the scheduler similar to what $ perf does 


### Linux CFS Design 

No default time slice, CFS calcs how long a process should run according to the nbumnber of runnable processes
- The dynamic time slice is weighted by the process priority (nice)
- Time slice = weight of task / 
// TODO GET the rest of this slide


### Linux CFS Design 

// TODO GET the IMAGES of this slid


### Scheduler class deisgn
// TODO GET the rest of this slide

Time sharing scheduling (non-real time)

- SCHED_NORMAL default scheduling policy
- SCHED_BATCH 
    
    
### Scheduler class implementation
The base schedule code triggers scheduling operation in two cases:
// TODO GET the rest of this slide
    

Final Project Discussion
