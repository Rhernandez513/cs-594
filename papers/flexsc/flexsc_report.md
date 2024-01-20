# CS 594: Linux Kernel Programming (Paper Reading Report)

```
Title: "FlexSC: Flexible System Call Scheduling with Exception-Less System Calls"

    Authors:
    Livio Soares University of Toronto, Michael Stumm University of Toronto

Name: Robert David Hernandez

        University of Illinois at Chicago
        CS594 Linux Kernel Programming
        Professor Xiaoguang Wang
        UID: 653217200
```

### 1. Summary of the paper. Do I like it? Hate it? Why?

Executive Summary:
*For over 30 years, system calls have been the standard interface for applications to request services from the operating system kernel. Traditionally implemented as synchronous, these calls negatively impact performance in system-intensive workloads due to issues like pipeline flushing and pollution of key processor structures. The paper suggests a solution called exception-less system calls, aiming to improve processor efficiency by offering flexibility in scheduling OS work. This leads to enhanced temporal and spatial locality of execution, reducing pollution effects on processor structures. The proposed mechanism, implemented in FlexSC for Linux, particularly benefits highly threaded server applications like web servers and database servers. FlexSC demonstrates performance improvements in Apache (up to 116%), MySQL (up to 40%), and BIND (up to 105%), without requiring modifications to applications.*

The paper begins by providing good data collected for the performance cost of system calls and interrupts, including the amount of processor instructions performed after an interrupt over time.  This data demonstrates that the impact of system performance cause by system calls is not insignificant.

The main technique introduced by the paper is to use a reserved space in memory called a syscall page and async syscall threads to handle requests.  The implementation is designed such that application programs do not need to change their interfaces to syscalls, though are responsible for checking the syscall page for completion which can be an additional challenge for developers, akin to event based programming.

To support this async execution, the paper introduces the concept of syscall threads which are best scheduled on a separate code from executing user space threads.  The implementation FlexSC uses batching of system calls by default to improve temporal locality in memory to great effect.

The paper then goes on to provide some performance data and results with system intensive programs that are recognizable to many programmers, Apache2 and MySQL, demonstrating significant performance improvement on both.  Using four cores, Figure 11 specifically shows Apache throughput using FlexSC achieving almost double the throughput to traditional system calls.

Overall I enjoyed the paper, it was technical and innovative.  I particularly enjoyed that the paper identified a clear problem with the normal order of operations of exception based system calls, collected and presented baseline data, implemented an improvement and then presented clear data demonstrating significant performance improvements.  The writing was clear and relatively concise, though some descriptions felt a bit repeated.  I would not be opposed to reading more work from the authors or about FlexSC.

### 2. What problem is it trying to solve? Does it work well (evaluation)?

The problem addressed by the paper was the perfomance hit caused by normal exception based system calls is not insignificant.  The paper attempted to address this problem by implementing a highly concurrent system call facility using both kernel space and user threads and in "syscall page" interface which can be described as a simple dedicated page table.  Of note is that in addition to this change in system call design handling, there was also a clear identification of spatial and temporal locality affecting system call (and subsequent return to user space) performance.  The paper goes on to design FlexSC in such a way to not only perform concurrently, but also to use batching to improve locality which also had a positive effect on FlexSC's performance.

### 3. Strength of the paper

The paper was clear with its presentation of the problem statement, had clear and sufficient data showing that the problem of exception based system calls were affecting user space performance. The authors addressed the problem in an effective way, demonstrated data and explained their solution clearly enough.  FlexSC shows that there is an impactful level of perfomance that can be extracted from changing the style of invocation of system calls, mostly in a way that the paper claims would not affect the interfaces of user space programs.

Generally speaking, the clarity of the writing is high, and to a reader like myself who is not intimately familiar with the intricacies of the Linux kernel or its system call methods, it was relatively easy to follow.  I only found 1 spelling mistake, the figures were easy to understand, although the tables could have benefited from using less acronyms for clarity.

The paper gave me enough of a basis to be curious about the real world adoption of an exception-less system call paradigm through FlexSC and it’s potential significant performance improvement that can be delivered across application programs.

### 4. Weakness of the paper

The paper claims that user space programs would not need to alter their PThread interfaces in order to leverage the performance imrovements of FlexSC, although the paper also explains some changes to user space programs - particularly with the idea that user space programs if not already designed in an event based programming paradigm would need to change their their behavior to no longer wait synchrounously for system calls to return.  This conflicting claim is confusing to the reader, and draws doubt in the reader's mind on just how feasible implementing FlexSC into the Linux Kernel proper would be, as it's certainly not reasonable to execpt every user space program to alter it's system call design to handle async behaviour.  There is an implication that user space programs would be told to wait for system calls to return and FlexSC could move onto the next system call handler to batch them, however its let open how this would affect the users experience.

One fault of the paper is that it stressed that both Apache2 and MySQL did not need modification, though it mentioned that application programs using FlexSC would need to handle async system calls.  It was not clear by the end of the paper why specifically Apache2 and MySQL did not need modification after section 4.2 discussing threads. 

### 5. What could be improved?

While the paper focuses mainly on server based applications where FlexSC looks to have the most significant impact, future work would be helpful to cover some other types of applications including client facing applications or mobile based ones.

