#!/bin/bash

BASE_DIR=$PROJECTS/linux

qemu-system-x86_64 \
  -m 4096 \
  -gdb tcp::1234,server,nowait \
  -append "console=ttyS0 nokaslr" \
  -kernel $BASE_DIR/linux-5.19.17/arch/x86_64/boot/bzImage  \
  -initrd $BASE_DIR/busybox-1.36.1/rootfs.img.gz   \
  -append "root=/dev/ram init=/linuxrc ramdisk_size=10485760"  \
  -serial file:output.txt \
  -netdev bridge,br=br-qemu,id=net0 \
  -device virtio-net-pci,netdev=net0,mac=52:54:00:12:34:56 \
  -netdev bridge,br=br-qemu,id=net1 \
  -device igb,netdev=net1,mac=52:54:00:12:34:57
