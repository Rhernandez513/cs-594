#!/bin/bash
QEMU_BIN=${PWD}/qemu-8.2.1/build/qemu-system-x86_64
NCPU=4
MEMSIZE=4G

KNL_SRC=${PWD}/linux
BZIMAGE=${KNL_SRC}/arch/x86/boot/bzImage
CMDLINE="nokaslr console=ttyS0 root=/dev/vda1"

# -drive if=virtio,format=qcow2,file=ubuntu2204.img \

sudo ${QEMU_BIN} \
  -nographic \
  -smp ${NCPU} -m ${MEMSIZE} \
  -device virtio-net-pci,netdev=net0 \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -drive file=./image/bookworm.img,format=raw \
  -enable-kvm \
  -kernel ${BZIMAGE} -append "${CMDLINE}"
