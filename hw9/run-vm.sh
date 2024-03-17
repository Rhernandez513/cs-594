#!/bin/bash

qemu-system-x86_64 \
        -m 2G \
        -smp 2 \
        -kernel ./linux-6.1.72/arch/x86/boot/bzImage \
        -append "console=ttyS0 root=/dev/sda net.ifnames=0" \
        -drive file=./image/bookworm.img,format=raw \
        -net user,host=10.0.2.10,hostfwd=tcp:127.0.0.1:2222-:22 \
        -net nic,model=e1000 \
        -nographic
