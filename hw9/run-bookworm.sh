#!/bin/bash
# QEMU_BIN=${PWD}/qemu-8.2.1/build/qemu-system-x86_64
QEMU_BIN=/users/rherna70/code/qemu-8.2.1/build/qemu-system-x86_64
NCPU=4
MEMSIZE=4G

# KNL_SRC=${PWD}/linux-6.1.72
KNL_SRC=/users/rherna70/code/linux-6.1.72
BZIMAGE=${KNL_SRC}/arch/x86/boot/bzImage
# CMDLINE="nokaslr console=ttyS0 root=/dev/vda1"
CMDLINE="nokaslr console=ttyS0 root=/dev/sda"

# -drive if=virtio,format=qcow2,file=ubuntu2204.img \

sudo ${QEMU_BIN} \
  -nographic \
  -smp ${NCPU} -m ${MEMSIZE} \
  -device virtio-net-pci,netdev=net0 \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -drive file=/users/rherna70/code/image/bookworm.img,format=raw \
  -kernel ${BZIMAGE} -append "${CMDLINE}" \
  -device my_rng
