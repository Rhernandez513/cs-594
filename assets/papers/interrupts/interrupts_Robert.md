# CS 594: Linux Kernel Programming (Paper Reading Report)

```
Title: ""

    Authors:
    Amy Tai
    Igor Smolyar
    Michael Wei
    Dan Tsafrir


Name: Robert David Hernandez

        University of Illinois at Chicago
        CS594 Linux Kernel Programming
        Professor Xiaoguang Wang
        UID: 653217200
```

### 1. Summary of the paper. Do I like it? Hate it? Why?

Executive Summary:
The paper primarily deals in a concept taken from network card interactions with the CPU and applies it to storage - grouping or "coalescing" interrupts into a batch.  In the past storage and disk were some of the slowest components in a computer system and could only produce hundreds of interrupts per second.  Modern SSD and NVMe storage provides a new challenge with faster and highly concurrent data access now possible, leading to an order of magnitude or more interrupts per time period.  The paper flaims double-digit performance improvements over tradtional approaches.


### 2. What problem is it trying to solve? Does it work well (evaluation)?

The paper addresses the increasing amounts of hardware interrupts generated per second by new generation of NVMe storage, which is many times faster and more concurrent than previous storage mediums.


### 3. Strength of the paper

The paper identifies a domain where the strategy (interrupt coalescing) has worked in the past, an looks to apply it to recent generation hardware, pointing how that NVMe hardware now supports interrupt coalescing.

The paper puports an interesting improvement to storage interrupts given it's testing with NVMe.

The data renderings, and format of the paer is well laid out.


### 4. Weakness of the paper

Many times the paper mentions changing interfaces or protocols used by clients devices and systems, which gives an impression that adoption and implementation of the paper in real world application would be challenged at best.

The paper spends too much energy on using cinterrupts for networking and it detracts of the goal of the paper that focus on batching / cinterrupts for storage.

The supporting data for performance incrase in application isn't strong enough to wow me into wanting to investigate using coalecing interrupts for NVMe in real world application yet.  There was a lot of mention of software design style that I didn't particularly agree with, like breaking interfaces, editing the outtermost Ethernet frame header with interrupt bits, flashing firmware, and modifying hardware that all made me feel like perhaps cinterrupts aren't yet feasible.


### 5. What could be improved?

Minimization of hardware and firmware modifications would improve the portability and ability for coalesced interrupts to grow into the sotrage space in the real world.

Editing the networking aspects of the paper would help the reader stay focused.  Section 5 has some mention and section 6 might be better off as a basis for it's own paper.