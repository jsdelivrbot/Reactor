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
# Generate the filesystem images
#
genext2fs --root=$ROOT_ROOT --block-size $BLOCK_SIZE --size-in-blocks $ROOTFS_SIZE_IN_BLOCKS rootfs.ext2
genext2fs --root=$BOOT_ROOT --block-size $BLOCK_SIZE --size-in-blocks $BOOTFS_SIZE_IN_BLOCKS bootfs.ext2
genext2fs --root=$DATA_ROOT --block-size $BLOCK_SIZE --size-in-blocks $DATAFS_SIZE_IN_BLOCKS datafs.ext2

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
dd if=bootfs.ext2 of=$DISK_IMAGE bs=1M seek=$PARTITION1_OFFSET
dd if=rootfs.ext2 of=$DISK_IMAGE bs=1M seek=$PARTITION2_OFFSET
dd if=datafs.ext2 of=$DISK_IMAGE bs=1M seek=$PARTITION3_OFFSET


