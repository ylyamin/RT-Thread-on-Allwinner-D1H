#!/bin/bash
#make ARCH=riscv -C sunxi-linux/ O=$PWD/linux-build licheerv_defconfig
#make -j `nproc` -C linux-build ARCH=riscv CROSS_COMPILE=/home/yury/toolchain/riscv64-glibc-gcc-thead_20200702/bin/riscv64-unknown-linux-gnu-

IMG="image.img"
echo "Creating Blank Image ${IMG}"
rm -f "${IMG}"
truncate -s "4000M" "${IMG}"

LOOP="$(sudo losetup -f --show image.img | cut -d'/' -f3)"
LOOPDEV="/dev/${LOOP}"
echo "Partitioning loopback device ${LOOPDEV}"

#partition
#dd if=/dev/zero of=image.img bs=1M count=2000
sudo parted -s -a optimal -- "${LOOPDEV}" mklabel gpt
sudo parted -s -a optimal -- "${LOOPDEV}" mkpart primary ext2 40MiB 100MiB    
sudo parted -s -a optimal -- "${LOOPDEV}" mkpart primary ext4 100MiB -2GiB    
sudo parted -s -a optimal -- "${LOOPDEV}" mkpart primary linux-swap -2GiB 100%

sudo mkfs.ext2 "${LOOPDEV}p1"
sudo mkfs.ext4 "${LOOPDEV}p2"
sudo mkswap    "${LOOPDEV}p3"

#bootloader
sudo dd if=sun20i_d1_spl/nboot/boot0_sdcard_sun20iw1p1.bin "of=${LOOPDEV}" bs=8192 seek=16
sudo dd if=u-boot.toc1 "of=${LOOPDEV}" bs=512 seek=32800

sudo mkdir -p /mnt/sdcard_boot
sudo mkdir -p /mnt/sdcard_rootfs

### boot partition
sudo mount "${LOOPDEV}p1" /mnt/sdcard_boot
sudo cp ../linux-build/arch/riscv/boot/Image.gz /mnt/sdcard_boot
sudo cp boot.scr /mnt/sdcard_boot
ls -all -h /mnt/sdcard_boot

### rootfs partition
sudo mount "${LOOPDEV}p2" /mnt/sdcard_rootfs
sudo tar xfJ licheerv-debian-rootfs_2022-03-11.tar.xz -C /mnt/sdcard_rootfs
ls -all -h /mnt/sdcard_rootfs

sudo umount /mnt/sdcard_boot
sudo umount /mnt/sdcard_rootfs
sudo losetup -d "${LOOPDEV}"

#cp "${IMG}" /mnt/hgfs/allwinner_d1/