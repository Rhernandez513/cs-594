# Recap: Completely fair scheduling implementation

Real time takes priority over CFS In more 

Deadline

Real Time

Fair


# Today: "interrupt"

A Mechanism to implement abstraction and multiplexing

Interrupt: asking for a service to the kernel
- By software (e.g., int) or by hardware (e.g., keyboard)

Interrupt handling in Linux
- Top half + bottom half

Interrupt

Compared to the CPU, devices are slow (e.g., 10 msec)
- The kernel must be free to go and handle other work, dealing with the hardware only after that hardware has completed its work
- **How to know the completion of a hardware operation?**
    - Polling: the kernel periodically checks the status of the hardware
    - Interrupt: the hardware signals its completion to the processor

Interrupt controller

    Device->
    Device-> interrupt_controller-> CPU
    Device->

Interrupts are electrical signals multiplexed by the interrupt controller
- Sent on a specific pin of the CPU

Once an interrupt is received, a dedicated function is executed
- Interrupt Handler
The kernel/userspace can be interrupted at any time 


Advanced PIC (API, I/O APIC) I/O APIC
- System chipset (or south bridge)
- Redistribute interrupts to local APICs

Local APIC
- Inside a processor chip
- Has a timer, which raises **timer interrupt**
- Issues an IPI (inter-process interrupt)


A Bigger Picture 

Interrupt request (IRQ)

**Interrupt line  or interrupt request (irq)**
- Device identifier
- E.g., intel 8259A interrupt lines
    - IRQ 0: system timer, IRQ 1: keyboard controller
    - IRQ 3: serial port, IRQ 4: terminal
- Some interrupt lines can be shared
- True for most modern devices (PCIe)
    
Exception

**Exception** are interrupt issued by the CPU executing some **code**

- **Software interrupts**, as opposed to hardware ones( devices)
- Examples: 
    - Program faults: divide-by-zero
    - Voluntary exceptions: int assembly instruction, for example syscall invocation (e.g, int x80)
- // TODO Get last bullets from slides

**Interface to hardware interrupt**`

Non-Maskable Interrupt (NMI)
- Never ignored, e.g., power failure, memory error
- In x86, vector 2, prevents other interrupts from executing

Maskable interrupt
- Ignored when IF in EFLAGS is 0
- Enabling / disabling: - sti set interrupt - CLI clear interrupt

Interrupt descriptor table
- // TODO get from slides

**Interrupt descriptor entry**

Offset is 64-bit, pointing to the dest IP
- Split in multiple places

Segment selector points to the destination CS in the kernel
Present flag indicates that this is a valid entry


**Predefined interrupt vectors**

    0: Divide Error
    1: Debug Exception
    2: Non-Maskable Interrupt
    3: Breakpoint Exception (e.g., int 3)
    4: Invalid opcode
    13: General Protection Fault
    14: Page Fault
    18: Machine Check (abort)
    32-255: User Defined Interrupts


The INT instruction (1/2)

Decide the vector number, in this case it’s the 0x80 in int 0x80
Fetch the interrupt descriptor for the vector 0x80 from the IDT
- The CPU finds it by taking the 0x80'th 16-byte entry starting at the physical address that the IDTR CPU register points to

Check that CPI <= DPL in the 
Save ESP in the stack information

- Load SS and ESP from TSS (Task State Segment)
- Push user SS
- Push user ESP
- Push user  EFFLAPS
- Push user CS
- Push user EIP
- Clear some EFLAGS bits
- SET CS and EIP from IDT descriptor segment selector and offset 


**INTERRUPT SERVICE ROUTINE (ISR)**

Interrupt handler or Interrupt Service Routine
- Function executed by the CPI in response to a specific interrupt
- In Linux, a normal C function matching a specific prototype the to pass the handler information
- // TODO get notes from slides


**Two conflicting goals of ISR**
1.  Interrupt processing must be fast
    - We are indeed interrupting user processes executing (user/kernel space)
    - In addition, other interrupts may need to be disabled during and interrupt processing
2. It sometimes involves performing significant amount of work 
    - So, it will take time
    - e.g., processing a network packet from the network cards

**Top half vs bottom half**
In many modern OSes, including Linux, interrupt processing is split into two parts:

**Top-half**: run immediately upon receipt of the interrupt
- Immediately run
- Acknowledge interrupt received
- Reset hardware

**Bottom-half**: less critical & time consuming work
- Run later


**Ex: network packet processing**
Top-half: interrupt service routine
    - Ack the hardware
    - Copies the new network packings in the main memory
    - Readies the network card for more packets
    - **Time critical** because the packet buffer on the network cards is limited in size -> packet drop
    - // TODO get the rest of this slides


**Shared handlers**
The IRQF_SHARED flag must be set in the flags arg to request_irg()

The dev_id argument must be unique to each registered handler
    - A pointer to any per-device structure is sufficient (e.g., struct device)
// TODO get the rest of the slide


**Interrupt context**

Process context: normal task execution, syscall, and exception

Interrupt context: interrupt service routine (ISR)
- Sleeping / blocking is not possible because the ISR is not a schedule entity
- No kmallor(size, GFP_KERNEL): use GFP_ATOMIC
- No blocking locking (e.g., mustex): use spinlock
- No printk: use trace_printk

Small stack sizes
- Only 2 pages


**Interrupt handling in Liunx**

Specific entry point for each interrupt line
-  saves the interrupt number and the current registers
- // TODO get the rest of the slide

**/proc/interrupts**

**Interrupt control**

Kernel code sometimes needs to disable interrupts to ensure **atomic execution** of a section of code 
- By disabling interrupts, you can guarantee that an interrupt handler will not preempt your curret code
- Moreover, disabling interrupts aslo disables kernel preemption

Note that disabling interrupts does not protect agaisnt concurrent access from other cores
- // TODO get the rest of the slide 

We can disable all IRQ or we can disable a specific IRQ, you can also use "nosync" so disable an IRQ without waiting 

**Further reading**

LWN: Debugging the kernel using Ftrace - part 1
0xAX: Interrupts and Interrupt Handling


**Next week**

Interrupt handler: bottom 
Kernel synchronization

