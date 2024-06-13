# Overview
This is an experimentation with the [RT-Thread Operating System](https://www.rt-thread.io/) with the [Allwinner D1H](https://d1.docs.aw-ol.com/en/) RISC-V soc and a collection of Bootloaders and Developer environment for programming and debugging.  

Executed at hardware such as [Sipeed Lichee RV + Dock](https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html) and [ClockworkPi DevTerm R01](https://www.clockworkpi.com/home-devterm).  

The intention is to easily run peripherals in D1, exactly Display. Looks like RT-thread OS has the most extensive HAL layer for D1, similar to the [Linux kernel](https://github.com/cuu/last_linux-5.4/tree/master/drivers/video/fbdev/sunxi/) but not so overcomplicated.

At the original repository [RT-Thread](https://github.com/RT-Thread/rt-thread) it looks like the compilation is not streamlined for D1H. So was perform fork from [v5.0.2](https://github.com/RT-Thread/rt-thread/releases/tag/v5.0.2) and introduced several changes to make it runnable in D1H. Added latests bootloaders, compiler, debugger, build system.

## Installation
In repository exist prebuilded image for SD card [image/sd_image.img](image/sd_image.img), need to flash it to SD card and install to device.

### Windows
Could use https://etcher.balena.io/#download-etcher for flash image to SD card.

### Linux
Could use command (where /dev/sdb - your SD card device):
```sh
sudo dd if=image/sd_image.img of=/dev/sdb bs=512 seek=16 conv=sync
```

## Build toolchain
Verified in Ubuntu 22.04.3 virtual machine.  
Please first install make enviroment:
```sh
sudo apt install make build-essential
```
Then install toolchain
```sh
make toolchain

T-HEAD_DebugServer will request specify installation dir: 'Set full installing path:'
```
Will be installed:
- riscv64-linux-musleabi_for_x86_64-pc-linux-gnu (Used by RT-Thread compilation)
- riscv64-glibc-gcc-thead_20200702 (Need for using GDB)
- T-HEAD_DebugServer (Need for using JTAG connetion)

(Notice: in a virtual machine with a mounted shared folder, it isn't possible to create hard links, so is impossible to extract files to this folder. Please install toolchain to other folder).  

Command could be perfomed with specified install folder
```sh
TOOLCHAIN_INSTALL_DIR=/home/etc make toolchain
```
## Build bootloaders
To compile bootloaders use command
```sh
make bootloaders
```
If in previus step was specified toolchain install folder need to do it again
```sh
TOOLCHAIN_INSTALL_DIR=/home/etc make bootloaders
```
Will be installed:
- opensbi
- sun20i_d1_spl
- u-boot
- xfel

## Build RT-Thread
To compile RT-Thread use command
```sh
make rt
```
If in previus step was specified toolchain install folder need to do it again
```sh
TOOLCHAIN_INSTALL_DIR=/home/etc make rt
```
## Debugging
```sh
xfel ddr d1
xfel jtag
TOOLCHAIN_INSTALL_DIR/T-HEAD_DebugServer/DebugServerConsole.elf
TOOLCHAIN_INSTALL_DIR/riscv64-glibc-gcc-thead_20200702/bin/riscv64-unknown-linux-gnu-gdb
```
## Hardware

## Links
https://linux-sunxi.org/Allwinner_Nezha  
https://andreas.welcomes-you.com/boot-sw-debian-risc-v-lichee-rv-2/  

## TODO
- rt-thread d1 compilation (issue)
- makefile rename targets
- u-boot install directory
- makefile install_toolchain artifacts check
- debugger build
- enviroment.sh
- source structure
- log from uart