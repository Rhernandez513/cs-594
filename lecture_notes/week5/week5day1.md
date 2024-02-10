Professor begins by going over the paper reading section

Each student will have to chose a paper to read and give a presentation on the paper itself to the class.  Expect to have to put together a small PPT to convey the core ideas of the paper

Final Project proposal will be due this Friday, 1 page proposal


Recap: Linux CFS
Linux CFS does not use an absolute time-slice
- The time slice a process receives is a function of the load of the systems (i.e., a proportion of the CPU)
- In addition, that time-slice is weighted by the process priority
- When a Process P becomes runnable:
    - P will preempt the currently running process C if P consumer as smaller proportion o the CPU than C 
    - Recall the Video Encoder and Text editor example from last lecture


Recap: sched_entity

Task_struct has scheduler-related fields

It has a struct called sched_enttiy

And sched_entity has a vruntime field (how much time has the process run)

Sched_class: an acstract base class for all scheduler classes

// TODO get lecture slides

If we want to define a scheduler class we should define the above

CFS implementation
Four main components of CFS
- Time accounting
- Process selection
- Scheduler entry point: schedule(), scheduler_tick()
- Sleeping and waking up

Time accounting in CFS
- Virtual runtime how much time a process has been executed
// TODO get lecture slides
- Upon every timer interrupt, CFS accounts the task's execution time
- Uses nice value plus execution time to calculate vruntime
    
    
Process selection in CFS
- CFS maintains a rbtree of tasks indexed by vruntime (i.e., runqueue)
  - Always pick a task with the smallest vruntime, the left-most node
  - // TODO add lecture slides


Small Quiz:
Process Selection in CFS
There are four processes running in user-space the props are as follows:

Process Selection in CFS

When CFS need to choose which runnable process to run next:
-  the process with the smallerst vruntime is selcted
- It is the leftmost node in the treee


Adding a task to a runqueue
- When a task is woken up, it is added to a runqueue

Removing a task from a runqueue
// TODO GET SLIDES

__dequeue_entity


Real-time scheduling policies
Linux provides two soft real-time scheduling classes
- SCHED_DEADLINE; SCHED_FIFO, SCHED_RR
- Best effort, no guaranteee

Real-time task of any scheduling class will always run before non-realtime ones (CFS: SCHED_NORMAL, SCHED_BATCH)
// TODO GET SLIDES

SCHED_FIFO
- Tasks run until it blocks/yield
- Only a higher priority RT task can preempt it
- Round-robin for tasks of same priority
SCHED_RR
- Same as SCHED_FIFO, but with a fixed time slice
    `$ cat /proc/sys/kernel/sched_rr_timeslice_ms`
    > 100

SCHED_DEADLINE
- Real-time policies mainlined in v3.14 enabling predicatble RT scheduling
- Earyl deadline first (EDF) scheduling based on a period of activation and a "worst-case execution time" (WCET) for each task

CFS on Multi-core machines
Per-CPU runqueues (rbtrees)
- To avoid costly accesses to shared data structures

Runqueues must be kept balanced
- E.g., dual-core with one long runqueue of high-priority processes, and a short one with low-priority processes
    - High-priority processes get less CPU time than low-priority ones
- A load balancer runs periodically based on priority and CPU usage
    
Preemption and context switch

A **context switch** is the action of swapping the process currently running on the CPU to another one
Performed by the context_switch(), which is called by schedule()
- Switch the address space through switch_mm()
- Switch the CPU state (registers) through switch_to()

    /* kernel/sched/core.c */
    schedule() -> __schedule() -> context_switch() -> switch_mm(), switch_to ()
Then, when schedule() will be called?
- A task can voluntarily relinquish the CPU by calling schedule()
- A current task needs to be preempted if
    - It runs long enough (i.e., it's vruntime is not the smallest anymore)
    - A task with higher priority is woken up

// TODO get slide titled 
"Scheduling-related system calls"


Summary: task = process | thread

Struct task_struct
    - A process or a thread
Struct mm
    - Virtual mm space

task = process | thread
// TODO get slide 


Other topics about processes?
How can we capture a process?>
- Checkpoint/restore in User space, or CRIU
    - https://criu.org/Main_page
- Freeze a process (or a running container) and checkpoint its state to a disk
- The data saved can be used to restore the process and run it exactly at the time of the freeze

Linux CRIU
(this is a mater's thesis for one of the professor's graduate students)

// TODO get lecture slide 

Linux CRIU use cases
- Container live migration
- Slow-boot service speed up
- Seamless kernel upgrade
- Move "forgotten" applications into "screen"
- https://criu.org/Usage_scenarios

Other use cases of CRIU?
Transform a process
- Can we transform a process and disable some unused code features?
    - e.g., init code
    - //TODO get lecture screenshort
- Visualization of process memorty footprints for executed basic blocks (blue and red), unused basic blocks (gray), and init-related basic blocks (red) in SPEC INT2017 605.mcf_s benchmark and Lighttpd web server
- Transform a process
    - Transform a process to run across the architecture boundary
    - https://github.com/dapper-project/demo/blob/main/dapper-demo.gif
    - // TODO get image from slides
        
        
        
        
Next steps:
// TODO GET SLIDES

HW5: 
  - Hash table, rbtree, and Xarray (due Feb 9th)

Final project Prop: Feb 9th

Paper reading assign:
  - Due on 15th 


Further reading LKD3: Chapt 4
