#!/bin/bash

set -xe

#
# User parameters.
#
DISK_IMAGE=Platform.img
TOTAL_DISK_SIZE=256
BOOTFS_SIZE=16
ROOTFS_SIZE=64
DATAFS_SIZE=128

BOOT_ROOT=./BootFS
ROOT_ROOT=./BuildRoot/output/target
DATA_ROOT=./DataFS
UBOOT_IMAGE=BuildRoot/output/images/u-boot-sunxi-with-spl.bin



#
# Derived variables.
#
BLOCK_SIZE=1024
ONE_MB_IN_BLOCKS=1024
ROOTFS_SIZE_IN_BLOCKS=$((ROOTFS_SIZE * ONE_MB_IN_BLOCKS))
BOOTFS_SIZE_IN_BLOCKS=$((BOOTFS_SIZE * ONE_MB_IN_BLOCKS))
DATAFS_SIZE_IN_BLOCKS=$((DATAFS_SIZE * ONE_MB_IN_BLOCKS))

PARTITION1_OFFSET=1
PARTITION2_OFFSET=$(( $PARTITION1_OFFSET + $BOOTFS_SIZE ))
PARTITION3_OFFSET=$(( $PARTITION2_OFFSET + $ROOTFS_SIZE ))

#
# Copy the files to the correct fs 'root'.
#
cp BuildRoot/output/images/boot.scr $BOOT_ROOT
cp BuildRoot/output/images/sun8i-h2-plus-orangepi-zero.dtb $BOOT_ROOT
cp BuildRoot/output/images/zImage $BOOT_ROOT

#
# Generate the filesystem images
#
rm -f rootfs
rm -f datafs
rm -f bootfs

#genext2fs --root=$ROOT_ROOT --block-size $BLOCK_SIZE --size-in-blocks $ROOTFS_SIZE_IN_BLOCKS rootfs
cp ./BuildRoot/output/images/rootfs.ext4 rootfs

genext2fs --root=$DATA_ROOT --block-size $BLOCK_SIZE --size-in-blocks $DATAFS_SIZE_IN_BLOCKS datafs

dd if=/dev/zero of=bootfs bs=1M count=$BOOTFS_SIZE
mkfs.vfat bootfs
mcopy -i bootfs BootFS/* ::/

#
# Generate the disk image.
#
dd if=/dev/zero of=$DISK_IMAGE bs=1M count=$TOTAL_DISK_SIZE

#
# Generate the partition table description file.
#
PARTITION_TABLE_FILE=PartitionTable.sfdisk
echo label: dos > $PARTITION_TABLE_FILE
echo label-id: 0x0123abcd >> $PARTITION_TABLE_FILE
echo device: Platform.img >> $PARTITION_TABLE_FILE
echo unit: sectors >> $PARTITION_TABLE_FILE
echo Platform.img1 : start=        ${PARTITION1_OFFSET}M, size=       ${BOOTFS_SIZE}M, type=83 >> $PARTITION_TABLE_FILE
echo Platform.img2 : start=        ${PARTITION2_OFFSET}M, size=       ${ROOTFS_SIZE}M, type=83 >> $PARTITION_TABLE_FILE
echo Platform.img3 : start=        ${PARTITION3_OFFSET}M, size=       ${DATAFS_SIZE}M, type=83 >> $PARTITION_TABLE_FILE

#
# Create the partition table in the disk image.
#
sfdisk $DISK_IMAGE < $PARTITION_TABLE_FILE

#
# Copy the filesystem images into the disk image partitions.
#
dd if=bootfs of=$DISK_IMAGE bs=1M seek=$PARTITION1_OFFSET conv=notrunc
dd if=rootfs of=$DISK_IMAGE bs=1M seek=$PARTITION2_OFFSET conv=notrunc
dd if=datafs of=$DISK_IMAGE bs=1M seek=$PARTITION3_OFFSET conv=notrunc

#
# Make the disk bootable by copying u-boot into it.
#
dd if=$UBOOT_IMAGE of=$DISK_IMAGE bs=1K seek=8 conv=notrunc


