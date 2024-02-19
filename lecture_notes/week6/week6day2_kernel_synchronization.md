# Kernel Synchronization

- Background on multicore processing
- Intro to synchronization
- Atomic operations
- Spinlock, reader-wrtier spinlock (RWLOCK)
- Semaphore, mutex
- Sequential lock (seqlock)
- Completion variable
- RCU

Single-core scaling stopped
// data chart from slides


Multi-core processors


## Kernel synchronization

The kernel is programmed using the shared memory model

Critical section (also called critical region)
    - Code paths that access and manipulate shared data
    - Must execute atomically
    - // TODO get the rest of the slide 

## Concurrent data accesses in kernel 
Q: What kind of kernel code will be considered to concurrently access data? Any scenarios?

*Real concurrent access on Multiple CPUs*
 - same as user-space thread programming
*Preemption on a single core*
    -  same as user-space thread programming
*Interrupt*
    - Only in kernel-space programming
    - Is a data structure accessed in an interrupt context, top half or bottom half?



## Locking
Atomic operations are not sufficient for protecting **shared data in long and complex critical regions**
- e.g., page_tree of an inode (page cache)

What is needed is a way of making sure that only one thread manipulates the data structure at a time

// TODO get last point from slide


## Deadlocks
Situations in which one or several threads are waiting on locks for one or several resources that will never be freed
-  none of the threads can continue

*Self-deadlock*
- A thread tries to aqquire a lock twice
- // todo get the last bullet

*Deadly embrace (ABBA deadlock)*

| Thread 1 | Thread 2
|-----------------------|-----------------------|
| Aquire Lock A         | Aquire Lock B
| Try to acquire lock B	| Try to acquire lock A
| Wait for lock B       | Wait for lock A


## Spinlocks 

The most commonly-used lock in the kernel
When a thread tries to acquire an already held lock, it spins while waiting for the lock to become available 
- Wasting processor time when spinning is too long

In an interrupt context (a thread cannot sleep)
- Kernel provides special spinlock API for data structures shared in interrupt context

In process context, do not sleep while holding a spinlock
- Kernel preemption is disabled

```c
/* include/linux/spinlock_types.h */

DEFINE_SPINLOCK(my_lock);

spin_lock(&my_lock);
/* critical region */
spin_unlock(&my_lock);
```

Lock/unlock methods *disable/enable kernel preemption and acquire/release the lock*
Lock is compiled away in uniprocessor systems 
 - // TODO Get last bullets 

## *Spinlocks in interrupt handlers*

Spin locks do not sleep so it is safe to use them in interrupt context
if a lock us used in an interrupt handler, local interrupts must first be disabled before acquiring a lock
- // TODO get slide


We can save IRQ state, or not save it, though we will lose some interrupt context if we get one and don't save the state

`spin_unlock_irq([…]); // don't save the state`


## Causes of concurrency

**Symmetrical multiprocessing (true concurrency)**
- Two or more processes can e

**Kernel preemption (pseduo-concurrency)**

**Sleeping and synchronization with user-space**
- A task in the kernel can sleep and thus invoke the scheduler, resulting in the running of a new process
- // TODO get the rest of the slide


## Concurrency safety
**SMB-SAFE**
- Code that is safe from concucrrency on symmetrical multiprocessing machines

**Preemption safe**
- Code that is safe from kernel preemption

**What to protect?**
- Protect the data not the code

## Checklist for locking
- Is the data global?
- Can a thread of execution other than the current on access it?
- Is the data shared between process context and interrupt context?
- Is it shared bewtween two different interrupt handles?
- If a process is rempeted while accesshing this data, can the newly scheduled process access the same data?
- // TODO get the rest of the slides

## Further Reading
- Moore's Law
- Amdahl's Law

## Next Lecture
- Sempahore, mutex
- Sequential lock (seqlock)
- Completion variable
- RCU
