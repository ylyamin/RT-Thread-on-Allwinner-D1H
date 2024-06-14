# Overview
This is an experimentation with the [RT-Thread Operating System](https://www.rt-thread.io/) with the [Allwinner D1H](https://d1.docs.aw-ol.com/en/) RISC-V SOC and a Collection of Bootloaders and Developer environment for programming and debugging.  

Executed at hardware such as [Sipeed Lichee RV + Dock](https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html) and [ClockworkPi DevTerm R01](https://www.clockworkpi.com/home-devterm).  

The intention is to easily run peripherals in D1, exactly Display. Because looks like RT-thread OS has the most extensive HAL layer for D1, similar to the [Linux kernel](https://github.com/cuu/last_linux-5.4/tree/master/drivers/video/fbdev/sunxi/) but not so overcomplicated.

At the original repository [RT-Thread](https://github.com/RT-Thread/rt-thread) it looks like the compilation is not streamlined for D1H [issue 9063](https://github.com/RT-Thread/rt-thread/issues/9063). So was performed fork from [v5.0.2](https://github.com/RT-Thread/rt-thread/releases/tag/v5.0.2) and introduced several changes to make it runnable in D1H. Added latests bootloaders, compiler, debugger, build system.

## Installation
In repository exist prebuilded image for SD card [image/sd_image.img](image/sd_image.img), need to flash it to SD card and install to device.

### Windows
Could use https://etcher.balena.io/#download-etcher for flash image to SD card.

### Linux
Could use command 
```sh
make sd_burn
```
Or
```sh
sudo dd if=image/sd_image.img of=/dev/sdb bs=512 seek=16 conv=sync
```
*where /dev/sdb - your SD card device

## Run
Configyre UART device to 115200 baudrate, 8N1.  

Insert flashed SD card to device and power on, should see at the end out like this:
```
 \ | /
- RT -     Thread Smart Operating System
 / | \     5.0.2 build Jun  9 2024 21:11:05
 2006 - 2022 Copyright by RT-Thread team
hal_sdc_create 0
card_detect insert
Initial card success. capacity :15200MB
sdmmc bytes_per_secotr:200, sector count:1db0000
not found partition of mbr, construct sd0 at offset 8M, size:0xffffffffb5800000
[D/FAL] (fal_flash_init:47) Flash device |                  sdcard0 | addr: 0x00000000 | len: 0xb6000000 | blk_size: 0x00000200 |initialized finish.
[I/FAL] ==================== FAL partition table ====================
[I/FAL] | name       | flash_dev |   offset   |    length  |
[I/FAL] -------------------------------------------------------------
[I/FAL] | download   | sdcard0   | 0x00800000 | 0x00800000 |
[I/FAL] | easyflash  | sdcard0   | 0x01000000 | 0x00100000 |
[I/FAL] | filesystem | sdcard0   | 0x01100000 | 0x00c00000 |
[I/FAL] =============================================================
[I/FAL] RT-Thread Flash Abstraction Layer initialize success.
Hello RISC-V
msh />Mount "sd0p0" on "/" fail
```
## Repo structure
```sh
$tree -L 2

RT-Thread-on-Allwinner-D1H
    ├── bootloaders
    │   ├── opensbi
    │   ├── sun20i_d1_spl
    │   ├── u-boot
    │   └── xfel
    ├── build
    │   ├── sd.bin
    │   ├── sun20i-d1-lichee-rv-dock.dtb
    │   └── toc1_D1H.cfg
    ├── debugger (will be created by make !)
    │   ├── bl702_cklink_whole_img_v2.2.bin
    │   ├── blisp
    ├── documentation
    │   ├── Allwinner_D1H
    │   ├── ClockworkPi_DevTerm
    │   └── Sipeed_Lichee_RV
    ├── image
    │   └── sd_image.img
    ├── rt-thread
    │   ├── bsp/allwinner/d1s_d1h/
    │   ...
    ├── toolchain (will be created by make !)
    │   └── riscv64-linux-musleabi_for_x86_64-pc-linux-gnu
    │   └── riscv64-glibc-gcc-thead_20200702     
    │   └── T-HEAD_DebugServer
    ├── Makefile
    ├── environment.sh
    └── README.md
```
## Environment
Windows 10 64x was used as a host with IDU VSCode and Ubuntu 22.04.3 64x virtual machine as a guest for compilation.

Ar guest ubuntu need to install make enviroment:
```sh
sudo apt install git make build-essential libusb-1.0-0-dev
```
Repo was downloaded to a shared folder in Windows mounted in Ubuntu, /etc/fstab example
```sh
.host:/ /mnt/hgfs       fuse.vmhgfs-fuse    defaults,allow_other    0    0
```
Notice: in a virtual machine with a mounted shared folder, it isn't possible to create hard links. 
So as toolchain and u-boot use hard links is impossible to extract files to this folder. Need to use internal filesystem in a virtual machine.

For this case need to provide variables
- TOOLCHAIN_INSTALL_DIR - path for installation dir for toolchain ex.: $HOME/toolchain
- U_BOOT_INSTALL_DIR - path for installation dir for u-boot bootloader ex.: $HOME/u-boot

Please modify environment.sh script to specify this variables and execute:
```sh
 source ./environment.sh
```

## Build toolchain
Installing toolchain
```sh
make toolchain

* T-HEAD_DebugServer will request specify installation dir: 'Set full installing path:'
```
Will be installed:
- riscv64-linux-musleabi_for_x86_64-pc-linux-gnu (Used by RT-Thread compilation)
- riscv64-glibc-gcc-thead_20200702 (Need for using GDB)
- T-HEAD_DebugServer (Need for using JTAG connetion)

## Build bootloaders
To compile bootloaders use command
```sh
make bootloaders
```
Will be installed:
- sun20i_d1_spl (first bootloader after BROM, used for init DRAM and load TOC image from MMC)
- u-boot (could used to load Linux image but now just tool to creat TOC image)
- opensbi (Supervisor Binary Interface for platform in M mode)
- xfel (used for debug in when platform in FEL mode)

## Build RT-Thread
To compile RT-Thread use command:
```sh
make rt
```
For clean:
```sh
make rt_clean
```
For configure:
```sh
make rt_conf
```
## Build SD card image
To create SD card image in image/sd_image.img:
```sh
make sd
```
For flash to SD card:
```sh
sd_burn:
```

## Debugging
Flashing Sipeed RV-Debuggr Plus:
```sh
make debug_burn
```
GDB session:
```sh
make debug
```

## Hardware
- Sipeed Lichee RV + Dock
- ClockworkPi DevTerm R01
- FTDI 2248-c USB/UART board
- Sipeed RV-Debuggr Plus
- MicroSD_Sniffer

### Sipeed Lichee RV assembly
![Lichee_RV_assembly](documentation/Lichee_RV_assembly.png)

### ClockworkPi DevTerm R01 assembly
![\Devterm_R01_assembly](documentation/Devterm_R01_assembly.png)

I figureout that integrated UART work very unstable. Acording [discussion](https://forum.clockworkpi.com/t/devterm-r-01-ext-board-uart-is-read-only/8704)
"The problem is that the CH340C provides 5 V logic levels, whereas the D1 only supports 3.3 V I/O...A better solution would be to solder wires to pins 2 and 3 of the CH340C and use a different USB-UART adapter that runs at the correct voltage." So I do this:
![Devterm_R01_uart](documentation/Devterm_R01_uart.png)

## Links
https://linux-sunxi.org/Allwinner_Nezha  
https://andreas.welcomes-you.com/boot-sw-debian-risc-v-lichee-rv-2/  

## TODO
- debug
- run driver for rgb LCD 
