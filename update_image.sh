#!/bin/bash

sudo losetup -v /dev/loop0 floppy.img
sudo mount -v -t ext2 /dev/loop0 /mnt
sudo cp -v --remove-destination kernel/kernel /mnt/kernel
sudo umount -v /mnt
sudo losetup -v -d /dev/loop0

