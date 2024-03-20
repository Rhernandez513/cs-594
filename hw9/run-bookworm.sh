#!/bin/bash
QEMU_BIN=/proj/nros-PG0/rherna70/code/qemu-8.2.1/build/qemu-system-x86_64
NCPU=4
MEMSIZE=4G

KNL_SRC=/proj/nros-PG0/rherna70/code/linux-6.1.72
BZIMAGE=${KNL_SRC}/arch/x86/boot/bzImage

BOOKWORM_IMAGE=/proj/nros-PG0/rherna70/code/image/bookworm.img

sudo ${QEMU_BIN} \
        -m ${MEMSIZE} \
        -smp ${NCPU} \
        -kernel ${BZIMAGE} \
        -append "console=ttyS0 root=/dev/sda net.ifnames=0" \
        -drive file=${BOOKWORM_IMAGE},format=raw \
        -net user,host=10.0.2.10,hostfwd=tcp:127.0.0.1:2222-:22 \
        -net nic,model=e1000 \
        -nographic \
        -device my_rng
