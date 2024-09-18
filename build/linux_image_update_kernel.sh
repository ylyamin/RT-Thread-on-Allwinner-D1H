#!/bin/bash
IMG="image.img"
LOOP="$(sudo losetup -f --partscan --show image.img | cut -d'/' -f3)"
LOOPDEV="/dev/${LOOP}"
echo "Partitioning loopback device ${LOOPDEV}"

sudo mkdir -p /mnt/sdcard_boot
sudo mkdir -p /mnt/sdcard_rootfs

### boot partition
sudo mount "${LOOPDEV}p1" /mnt/sdcard_boot
sudo cp ../linux-build/arch/riscv/boot/Image.gz /mnt/sdcard_boot
ls -all -h /mnt/sdcard_boot

sudo umount /mnt/sdcard_boot
sudo losetup -d "${LOOPDEV}"
#cp "${IMG}" /mnt/hgfs/allwinner_d1/