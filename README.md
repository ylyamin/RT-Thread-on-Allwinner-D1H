# Overview
This is an adaptation of the [RT-Thread Operating System](https://www.rt-thread.io/) for the [Allwinner D1H](https://d1.docs.aw-ol.com/en/) RISC-V soc and a collection of bootloaders and developer environment for programming and debugging.
Executed at hardware such as [Sipeed Lichee RV + Dock](https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html) and [ClockworkPi DevTerm R01](https://www.clockworkpi.com/home-devterm).

The intention is to easily run peripherals in D1, exactly Display. Looks like RT-thread OS has the most extensive HAL layer for D1, similar to the [Linux kernel](https://github.com/cuu/last_linux-5.4/tree/master/drivers/video/fbdev/sunxi/) but not so overcomplicated.

Unfortunatly in the original repository [RT-Thread](https://github.com/RT-Thread/rt-thread) we saw this picture:
- bsp/allwinner/d1/ - not compiled looks outdated
- bsp/allwinner/d1s/ - not compiled in master or v5.1.0, compiled in [v5.0.2](https://github.com/RT-Thread/rt-thread/releases/tag/v5.0.2) but not started in D1H

I forked [v5.0.2](https://github.com/RT-Thread/rt-thread/releases/tag/v5.0.2) and introduced several changes to make it runnable in D1H.

## Installation
In repository exist prebuilded image for SD card [image/sd_image.img](image/sd_image.img), need to flash it to SD card and install to device.

### Windows
Could use https://etcher.balena.io/#download-etcher and flash image to SD card.

### Linux
Could use command (where /dev/sdb - your SD card device):
```sh
sudo dd if=image/sd_image.img of=/dev/sdb bs=512 seek=16 conv=sync
```

## Build toolchain
sudo apt install make build-essential


## Build bootloaders

## Build RT-Thread
[RT-Thread README](rt-thread/README.md)

## Debugging

## Hardware
