
# CS 594: Linux Kernel Programming (Paper Reading Report)

```
Title: "OS Scheduling with Nest: Keeping Tasks Close Together on Warm Cores"

    Authors:
        Julia Lawall
        Inria
        Paris, France

        Himadri Chhaya-Shailesh
        Inria
        Paris, France

        Jean-Pierre Lozi
        Oracle Labs
        Zurich, Switzerland

        Baptiste Lepers
        University of Sydney
        Sydney, Australia

        Willy Zwaenepoel
        University of Sydney
        Sydney, Australia

        Gilles Muller
        Inria
        Paris, France

Name: Robert David Hernandez

        University of Illinois at Chicago
        CS594 Linux Kernel Programming
        Professor Xiaoguang Wang
        UID: 653217200
```

### 1. Summary of the paper. Do I like it? Hate it? Why?

Executive Summary:

The paper introduces a new scheduler in the linux kernel called NEST.  The objective of this type of scheduler is to schedule tasks on core that have recently been used for compute and thus are already at higher frequencies, a secondary aim is to use less socket resources.  The premise is that Linux CFS does not account for differences in core frequency when scheduling tasks, and the addition of this with a prefernce for using core already at higher frequencies has a positive affect on compute performance.

### 2. What problem is it trying to solve? Does it work well (evaluation)?

The primary goal of the paper is to demonstrate a marked improvement in compute performance using a different process sharing strategy than Linux CFS, and to demonstrate that consideration for the hardware's behavior can have a positive impact on compute performance.

The paper accomplishes this by introducing a new scheduling implementation called NEST that focuses on keeping processes running on or near non-idle cores, putting off snipping up idle cores as much as possible.  The paper accomplishes this by extending the Linux CFS (Completely Fair Scheduler)

The paper demonstrates some performance improvements, up to 20% on some workloads, with power savings of about 20% on some workloads, which is a considerable performance improvement.

The paper begins by examing what is intuitively a slower process, t_parent calling t_child, where the core that t_parent was running on (initially higher frequency) sleeps while a new core (initially idle) spins up on the new t_child process.  The paper considers the challenges of overloading cores with too many tasks, and describes a scheduling scheme that uses a "primary nest" of a few cores, a "reserve nest" with a fixed maximum size, and Linux CFS as a backup scheduling strategy.




### 3. Strength of the paper

One of the core strengths of the paper is the clear visual presentation and rendering of speedup data.  Figure 4 in particular leaves the reader confident in the practicality of an alternative scheduling strategy like NEST to show impact on various compute workloads and on various core counts.


### 4. Weakness of the paper

Figures 8 and 9 are particularly hard to understand, and on first impression clear on the metric conveyed.  Figure 11 shows mixed results, and the authors even mention in their writing in section 5.4 that the results in Figure 11 are difficult to interpret.  It is not clear that on DaCapo types of workloads that NEST provides any tangible performance benefit, though it can be surmized from the data presented that at least NEST doesn't consistently degrade performance when compared to CFS for DaCapo style workloads.


### 5. What could be improved?

More attention could have been paid to covering what specific types of workloads would be useful for using NEST over CFS.  Certainly the configuration testing scenarios covered in section 5 seem useful, and as a reader had me interested in if there is potential speedup in NEST for test suite runnners, compilers/build runners, or continuous integrattion / deployment workloads.
