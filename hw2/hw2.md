# Homework 2 -- Build Linux kernel

## Objectives

This exercise instructs you how to build a Linux kernel and run it in a QEMU environment.

# Required Setup

Assume you already have a Ubuntu 22.04 server running either in a VirtualBox or native machine.

First, let's install a few pre-requisite Debian packages that will allow us to compile the Linux kernel.
```
sudo apt update
sudo apt install -y build-essential git libssl-dev flex bison wget pkg-config libelf-dev
sudo apt install -y qemu-system-x86 debootstrap
```

# Build the Linux kernel
We'll compile a minimal version of Linux that can run in a VM created by Qemu. We can download the kernel from the tarball (or use the Linux source code with the `v6.1` branch).
```
mkdir ~/hw2; cd ~/hw2
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.1.72.tar.xz
tar xf linux-6.1.72.tar.xz
```
Next, we'll use the default configuration for a basic x86-64 machine. That configuration can be generated as follows:
```
cd linux-6.1.72
make defconfig
```
The `make defconfig` will use `x86_64_defconfig` on an x86_64 machine. Once done launch the compilation of the kernel with the following command:
```
make -j4
```
This will take a while (10~30 minutes, depending on your machine settings). Once finished, the compiled kernel's binary is `arch/x86/boot/bzImage`.

> T1: **4 points** for **building the kernel**. You can take a screenshot and upload it to the blackboard.

# Try out the kernel with QEMU
Try to boot the kernel we just built with QEMU as follows:
```
qemu-system-x86_64 -m 1G -kernel arch/x86_64/boot/bzImage -nographic -append "console=ttyS0"
```
Here we tell QEMU to create a machine with `1G` of RAM and to use the kernel we compiled. The option `-append` indicates that the option `"console=ttyS0"` should be passed to the kernel, telling it to output its boot log on the VM's serial console.

You will see Linux starting to boot, but the boot process ends with the following error:
```
... ...
/dev/root: Can't open blockdev
VFS: Cannot open root device "(null)" or unknown-block(0,0): error -6
Please append a correct "root=" boot option; here are the available partitions:
... ...
Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(0,0)
```
Don't panic! This is because the kernel we built cannot find a root filesystem. The root filesystem contains all user-space utilities, such as shell.

To exit Qemu, press `Ctrl+a` followed by `x`.

# Create a root filesystem image for the kernel
There are many ways to get a root filesystem image. Here, we can use a script from the [syzkaller project](https://github.com/google/syzkaller/tree/master) to set up a Debian-like environment:
```
cd ~/hw2
mkdir image && cd image
wget https://raw.githubusercontent.com/google/syzkaller/master/tools/create-image.sh -O create-image.sh
chmod +x create-image.sh
./create-image.sh
```
There are some options in `create-image.sh` script. For example, you can specify the Debian distribution version with `./create-image.sh --distribution bookworm`. Please refer to the [syzkaller guide](https://github.com/google/syzkaller/blob/master/docs/linux/setup_ubuntu-host_qemu-vm_x86-64-kernel.md) for more details.

> T2: **4 points** for **the root filesystem image**. You can take a screenshot and upload it to the blackboard. You are also encouraged to use your preferred way to make this image (please briefly describe it).

# Launch the kernel with the QEMU image
Create a `run-vm.sh` script file under `~/hw2`, and write the following script inside:
```
#!/bin/bash

qemu-system-x86_64 \
        -m 2G \
        -smp 2 \
        -kernel ./linux-6.1.72/arch/x86/boot/bzImage \
        -append "console=ttyS0 root=/dev/sda net.ifnames=0" \
        -drive file=./image/bullseye.img,format=raw \
        -net user,host=10.0.2.10,hostfwd=tcp:127.0.0.1:2222-:22 \
        -net nic,model=e1000 \
        -nographic
```

Add the executable permission to this script file and launch the VM:
```
chmod +x run-vm.sh
./run-vm.sh
```
You should be able to see the QEMU VM boots with our new kernel. Press the `Enter` key to go into the shell.

Check the kernel version:
```
root@syzkaller:~# uname -a
Linux syzkaller 6.1.0 #1 SMP PREEMPT_DYNAMIC Wed Jan 10 18:27:15 MST 2024 x86_64 GNU/Linux
```
You can poweroff the VM with:
```
root@syzkaller:~# poweroff
```
Or in any case the kernel oops, you can always use press `Ctrl+a` followed by `x` to quit the QEMU.

> T3: **2 points** for **seeing the new kernel version**. You can take a screenshot and upload it to the blackboard.