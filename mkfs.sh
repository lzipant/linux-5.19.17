#!/bin/bash

BASE_DIR=$PROJECTS/linux/busybox-1.36.1

rm -rf $BASE_DIR/rootfs.ext4
rm -rf $BASE_DIR/fs

# bs是指block size
# count是指block的数量
# 所有这里of所指的分区的大小时bs*count, 注意不要超过Linux内核参数ramdisk的size.
dd if=/dev/zero of=$BASE_DIR/rootfs.ext4 bs=1M count=1000
mkfs.ext4 $BASE_DIR/rootfs.ext4
mkdir $BASE_DIR/fs
# 这里将rootfs.ext4挂载为伪设备
mount -o loop $BASE_DIR/rootfs.ext4 $BASE_DIR/fs
cp -rf $BASE_DIR/_install/* $BASE_DIR/fs

cp -rf ./lzipant/* $BASE_DIR/fs/usr/local/

# Java
#JAVA_BASE=/home/parallels/Documents/projects/openjdk/openjdk8
#mkdir -p $BASE_DIR/fs/opt/java
#cp -rf $JAVA_BASE/build/linux-x86_64-normal-server-slowdebug/jdk/* $BASE_DIR/fs/opt/java/
#cp -rf $JAVA_BASE/jdk8-test/out/production/jdk8-test $BASE_DIR/fs/opt/java/

umount $BASE_DIR/fs
gzip --best -c $BASE_DIR/rootfs.ext4 > $BASE_DIR/rootfs.img.gz
