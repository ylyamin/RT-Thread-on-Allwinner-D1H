## RT-Thread overview

RT-Thread OS was chosen because it looks like has the most extensive HAL layer (Hardware Abstraction Layer) for Allwinner D1.<br>
HAL is similar to the Linux kernel but not so overcomplicated and is placed in one folder.<br>
RT-Thread repository: https://github.com/RT-Thread/rt-thread/

"RT-Thread was born in 2006, it is an open source, neutral, and community-based real-time operating system (RTOS)."<br>
"RT-Thread is mainly written in C language, easy to understand and easy to port"<br>
https://www.rt-thread.io/document/site/<br>

RT-Thread source code catalog is shown as follow:

|  Name        |	Description |
|  :---        | :---         |
|  bsp         |  Board Support Package based on the porting of various development boards   |
|  components  |	Components, such as finsh shell, file system, protocol stack etc.       |
|  include     |  Head files of RT-Thread kernel   |
|  libcpu      |  CPU porting code such as ARM/MIPS/RISC-V etc.   |
|  src         |  The source files for the RT-Thread kernel.   |
|  tools       |  The script files for the RT-Thread command build tool.  |

### RT-Thread kernel
RT-Thread kernel implementation and kernel library placed in **rt-thread/src** folder.

#### Thread Scheduling 
Thread is the smallest scheduling unit in the RT-Thread operating system.<br>
Thread scheduling algorithm is a priority-based full preemptive multi-thread scheduling algorithm.<br>
https://www.rt-thread.io/document/site/programming-manual/thread/thread/

#### Synchronization between Threads 
RT-Thread uses thread semaphores, mutexes, and event sets to achieve inter-thread synchronization.<br>
https://www.rt-thread.io/document/site/programming-manual/ipc1/ipc1/

#### Inter-Thread Communication
RT-Thread supports communication mechanisms such as mailbox, message queue, etc.<br>
https://www.rt-thread.io/document/site/programming-manual/ipc2/ipc2/

### BSP

For our architecture Allwinner D1H with XuanTie C906 RISC-V Processor available implementation in folders:

- bsp/allwinner/d1/
- bsp/allwinner/d1s/

rt-thread/bsp/allwinner/d1s/board/board.c define how board will be initiated:
```c
   rt_hw_interrupt_init();

   /* init hal hardware */
   hal_clock_init();
   hal_gpio_init();
   hal_dma_init();

   /* init rtthread hardware */
   rt_hw_uart_init();
   rt_hw_tick_init();
   rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
   rt_components_board_init();
```

Folder **rt-thread/bsp/allwinner/libraries/drivers** define driver initialization in OS-level interaction, for example driver for UART in file rt-thread/bsp/allwinner/libraries/drivers/drv_uart.c<br>
Some drivers have the automatic initialization mechanism https://www.rt-thread.io/document/site/programming-manual/basic/basic/#rt-thread-automatic-initialization-mechanism, when initialization function declared by macro definition at the function definition, and it will be executed during system startup.<br> 
Like macros INIT_BOARD_EXPORT(rt_hw_i2c_init) in file rt-thread/bsp/allwinner/libraries/drivers/drv_i2c.c will be called before scheduler starting.<br>

In folder **rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source** contains HAL implementation.<br>
In folder **rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/test** contains small test programs for HAL.<br>
In folder **rt-thread/bsp/allwinner/libraries/sunxi-hal/osal** contains operating system abstraction layer is interlayer between HAL and RTT Kernel.<br>

### LibCPU

Folder **rt-thread/libcpu/risc-v/t-head/c906** contains specifics of XuanTie C906 RISC-V Processor with library's for handle registers, interruptions, memory, etc.

### Build system

RT-Thread used python based **Scons** as a build system https://www.rt-thread.io/document/site/programming-manual/scons/scons/

To build RTT for D1H need to execute:
```sh
cd rt-thread/bsp/allwinner/d1s/ 
scons
```

File rt-thread/bsp/allwinner/d1s/rtconfig.py define what compiler to use for compilation.<br> 
RTT expected to use RISC-V Linux MUSL GCC toolchain:

```python
import os

# toolchains options
ARCH        ='risc-v'
VENDOR      ='t-head'
CPU         ='c906'
CROSS_TOOL  ='gcc'

if  CROSS_TOOL == 'gcc':
    PLATFORM    = 'gcc'
    EXEC_PATH   = r'/opt/riscv64_musl/bin'
```

I use RTT version of toolchain:

```sh
TOOLCHAIN_INSTALL_DIR ?= $(shell pwd)/toolchain
wget -P $(TOOLCHAIN_INSTALL_DIR) https://github.com/RT-Thread/toolchains-ci/releases/download/v1.7/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu_latest.tar.bz2
tar -C $(TOOLCHAIN_INSTALL_DIR) -xf $(TOOLCHAIN_INSTALL_DIR)/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu_latest.tar.bz2
RISCV64_MUSL_DIR = $(TOOLCHAIN_INSTALL_DIR)/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu
```

Then to build project execute:
```sh
RTT_EXEC_PATH=$(RISCV64_MUSL_BIN) scons
```
To clean project:
```sh
RTT_EXEC_PATH=$(RISCV64_MUSL_BIN) scons -c
```

To configure RT-Thread and HAL need to use command:
```sh
RTT_EXEC_PATH=$(RISCV64_MUSL_BIN) scons --menuconfig
```

Configure system in RTT use Kconfig https://www.kernel.org/doc/html/v6.6/kbuild/kconfig-language.html<br>
Is used configuration menu entries defined in Kconfig files in different project folders, like this: rt-thread/bsp/allwinner/d1s/Kconfig.<br>

When we call menuconfig generated config file rt-thread/bsp/allwinner/d1s/.config and based of this generated C header with define expressions rt-thread/bsp/allwinner/d1s/rtconfig.h<br> 
Also this config values used in SConscript scripts used by Scons build: rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/SConscript<br>

### Compile RT-Thread for D1H platform:

I tried to compile RTT for D1H, first try with Master brunch.

#### RT-Thread [Master](https://github.com/RT-Thread/rt-thread/commit/2866da37a02dae72200e02ad87480718002760a5) or [v5.1.0](https://github.com/RT-Thread/rt-thread/releases/tag/v5.1.0)

1) bsp/allwinner/d1/ - not compiled looks outdated, had error due compilation:
```sh
board/board.c:45:5: error: unknown type name 'rt_mmu_info'
   45 |     rt_mmu_info mmu_info;
      |     ^~~~~~~~~~~
```

2) bsp/allwinner/d1s/ - not compiled, had error due compilation:
```sh
/mnt/hgfs/allwinner_d1/rt-thread/components/dfs/dfs_v2/filesystems/devfs/devfs.c:68:30: error: 'struct rt_device' has no member named 'ops'; did you mean 'fops'?
   68 |             else if (device->ops && file->vnode->ref_count == 1)
      |                              ^~~
      |                              fops
```

#### RT-Thread [v5.0.2](https://github.com/RT-Thread/rt-thread/releases/tag/v5.0.2):  

3) bsp/allwinner/d1/ - not compiled looks outdated, had error due compilation:
```sh
board/board.c:45:5: error: unknown type name 'rt_mmu_info'
   45 |     rt_mmu_info mmu_info;
      |     ^~~~~~~~~~~
```

4) bsp/allwinner/d1s/ - **compiled**
Is generat rt-thread/bsp/allwinner/d1s/rtthread.bin kenel binary file, but when I run it in board was no any output to UART. 

#### Conclusion:

At the original repository [RT-Thread](https://github.com/RT-Thread/rt-thread) the compilation is not streamlined for D1H.<br>
Create a [issue 9063](https://github.com/RT-Thread/rt-thread/issues/9063). (Later this issue was solved by [PR](https://github.com/RT-Thread/rt-thread/pull/9142) in Master)

So as v5.0.2 seems to be the closest to be success for D1H - was performed fork from [v5.0.2](https://github.com/RT-Thread/rt-thread/releases/tag/v5.0.2) and introduced several changes to make it runnable in D1H.
- All rt-thread folders moved to sub-folder rt-thread, also created folders for bootloaders, debugger, etc..
- bsp/allwinner/d1s/ was taken as base to development and renamed to bsp/allwinner/d1s_d1h/
- Other bsp and libcpu was removed

Created repository in github: https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H

## Change bootloaders

How in RTT prepares an image for booting D1s or D1H ?<br>
In file rt-thread/bsp/allwinner/d1s/rtconfig.py defined:
```python
POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n' + './mksdimg.sh\n'
```

So is called mksdimg.sh script with:
```sh
#/bin/sh
./mkimage -T sunxi_toc1 -d toc1.cfg sd.bin
# offset: 8K of spl，+48K of toc1
#sudo dd if=boot0_sdcard_sun20iw1p1_d1s.bin of=/dev/sdb bs=1024 seek=8
#sudo dd if=sd.bin of=/dev/sdb bs=1024 seek=56
```

And in file toc1.cfg defined: 
```conf
[opensbi]
file = sbi.bin
addr = 0x40000000
[dtb]
file = u-boot.dtb
addr = 0x40200000
[kernel]
file = rtthread.bin
addr = 0x40400000
```
Also look to articles:
- https://github.com/RT-Thread/rt-thread/blob/master/bsp/allwinner/d1s/README-M7.md
- https://github.com/RT-Thread/rt-thread/blob/master/bsp/allwinner/d1s/README-MQ.md
- https://club.rt-thread.org/ask/article/389ac36250b57737.html

Looks like:
- bsp/allwinner/d1s/tools/boot0_sdcard_sun20iw1p1_d1s.bin - is Samuel Holland Secondary Program Loader.
- bsp/allwinner/d1s/sbi.bin - is looks like OpenSBI
- mkimage - tool from U-Boot

But when I run board with this image bootloaders not show anything at UART, looks like targeted to D1s UART pins that different from D1H.<br>
So I replaced this binary's to relevant bootloaders:
- https://github.com/smaeul/sun20i_d1_spl.git
- https://github.com/riscv-software-src/opensbi
- https://github.com/smaeul/u-boot

After that bootloaders start show info at UART, but RT-Thread not.
<details><summary>UART output:</summary>

```sh
[158]HELLO! BOOT0 is starting!
[161]BOOT0 commit : 0ad88bf
[163]set pll start
[165]periph0 has been enabled
[168]set pll end
[170]board init ok
[172]DRAM only have internal ZQ!!
[175]get_pmu_exist() = -1
[177]ddr_efuse_type: 0x0
[180][AUTO DEBUG] single rank and full DQ!
[184]ddr_efuse_type: 0x0
[187][AUTO DEBUG] rank 0 row = 16
[190][AUTO DEBUG] rank 0 bank = 8
[193][AUTO DEBUG] rank 0 page size = 2 KB
[197]DRAM BOOT DRIVE INFO: V0.24
[200]DRAM CLK = 792 MHz
[202]DRAM Type = 3 (2:DDR2,3:DDR3)
[206]DRAMC ZQ value: 0x7b7bfb
[208]DRAM ODT value: 0x42.
[211]ddr_efuse_type: 0x0
[214]DRAM SIZE =1024 M
[217]DRAM simple test OK.
[220]dram size =1024
[222]card no is 0
[223]sdcard 0 line count 4
[226][mmc]: mmc driver ver 2021-04-2 16:45
[235][mmc]: Wrong media type 0x0
[238][mmc]: ***Try SD card 0***
[256][mmc]: HSSDR52/SDR25 4 bit
[258][mmc]: 50000000 Hz
[261][mmc]: 15193 MB
[263][mmc]: ***SD/MMC 0 init OK!!!***
[312]Loading boot-pkg Succeed(index=1).
[315]Entry_name        = opensbi
[318]Entry_name        = dtb
[321]Entry_name        = u-boot
[324]Adding DRAM info to DTB.
[329]Jump to second Boot.
OpenSBI v0.9-188-gd78eef3
   ____                    _____ ____ _____
  / __ \                  / ____|  _ \_   _|
 | |  | |_ __   ___ _ __ | (___ | |_) || |
 | |  | | '_ \ / _ \ '_ \ \___ \|  _ < | |
 | |__| | |_) |  __/ | | |____) | |_) || |_
  \____/| .__/ \___|_| |_|_____/|____/_____|
        | |
        |_|

Platform Name             : Sipeed Lichee RV Dock
Platform Features         : medeleg
Platform HART Count       : 1
Platform IPI Device       : aclint-mswi
Platform Timer Device     : aclint-mtimer @ 24000000Hz
Platform Console Device   : uart8250
Platform HSM Device       : ---
Platform Reboot Device    : sunxi-wdt-reset
Platform Shutdown Device  : ---
Firmware Base             : 0x40000000
Firmware Size             : 252 KB
Runtime SBI Version       : 0.3

Domain0 Name              : root
Domain0 Boot HART         : 0
Domain0 HARTs             : 0*
Domain0 Region00          : 0x0000000014008000-0x000000001400bfff (I)
Domain0 Region01          : 0x0000000014000000-0x0000000014007fff (I)
Domain0 Region02          : 0x0000000040000000-0x000000004003ffff ()
Domain0 Region03          : 0x0000000000000000-0xffffffffffffffff (R,W,X)
Domain0 Next Address      : 0x000000004a000000
Domain0 Next Arg1         : 0x0000000044000000
Domain0 Next Mode         : S-mode
Domain0 SysReset          : yes

Boot HART ID              : 0
Boot HART Domain          : root
Boot HART ISA             : rv64imafdcvsux
Boot HART Features        : scounteren,mcounteren,mcountinhibit,time
Boot HART PMP Count       : 16
Boot HART PMP Granularity : 2048
Boot HART PMP Address Bits: 38
Boot HART MHPM Count      : 0
Boot HART MIDELEG         : 0x0000000000000222
Boot HART MEDELEG         : 0x000000000000b109
```
</details>
<br>

## Change UART for D1H

### Configure UART pins for D1H
```patch
diff --git a/bsp/allwinner/d1s_d1h/.config b/bsp/allwinner/d1s_d1h/.config
index bc8986a8e..17f622d73 100644
--- a/bsp/allwinner/d1s_d1h/.config
+++ b/bsp/allwinner/d1s_d1h/.config

+CONFIG_BOARD_allwinnerd1=y
+CONFIG_BSP_USING_UART0=y
+CONFIG_UART0_TX_USING_GPIOB8=y
+CONFIG_UART0_RX_USING_GPIOB9=y
```

### Change UART baud rate in driver
```patch
diff --git a/bsp/allwinner/libraries/drivers/drv_uart.c b/bsp/allwinner/libraries/drivers/drv_uart.c
index 18956e5e8..7b7efb7c4 100644
--- a/bsp/allwinner/libraries/drivers/drv_uart.c
+++ b/bsp/allwinner/libraries/drivers/drv_uart.c
@@ -20,8 +20,10 @@
 #include <hal_clk.h>
 #include <hal_reset.h>
 #include <hal_gpio.h>
-#define UART_DEFAULT_BAUDRATE 500000
+#define UART_DEFAULT_BAUDRATE 115200
```

### Extend config to support simultaneously D1S and D1H SOC
```patch
git diff f40e63c...16181b0 > diff.patch

diff --git a/bsp/allwinner/d1s_d1h/Kconfig b/bsp/allwinner/d1s_d1h/Kconfig
index e5306596d..d11a63db0 100644
--- a/bsp/allwinner/d1s_d1h/Kconfig
+++ b/bsp/allwinner/d1s_d1h/Kconfig
@@ -18,14 +18,30 @@ config PKGS_DIR
 source "$RTT_DIR/Kconfig"
 source "$PKGS_DIR/Kconfig"
 
-config BOARD_allwinnerd1s
-    bool
-    select ARCH_RISCV64
-    select RT_USING_COMPONENTS_INIT
-    select RT_USING_USER_MAIN
-    select RT_USING_CACHE
-    select ARCH_MM_MMU
-    default y
+choice
+    prompt "Select Allwinner D1 type"
+    default BOARD_allwinnerd1
+
+    config BOARD_allwinnerd1
+        bool "Allwinner D1"
+        select ARCH_RISCV64
+        select RT_USING_COMPONENTS_INIT
+        select RT_USING_USER_MAIN
+        select RT_USING_CACHE
+        select ARCH_MM_MMU
+
+    config BOARD_allwinnerd1s
+        bool "Allwinner D1s"
+        select ARCH_RISCV64
+        select RT_USING_COMPONENTS_INIT
+        select RT_USING_USER_MAIN
+        select RT_USING_CACHE
+        select ARCH_MM_MMU
+endchoice
```

After this changes board started and shown out to UART.
But loading frozen after "card_detect insert" message:
```sh
 \ | /
- RT -     Thread Smart Operating System
 / | \     5.0.2 build Jun  9 2024 17:11:05
 2006 - 2022 Copyright by RT-Thread team
hal_sdc_create 0
card_detect insert
```

## Problem with SD card

Looks like is frozen on rt_thread_mdelay function by some reason in files
- bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.c
- bsp/allwinner/libraries/sunxi-hal/include/hal/sdmmc/osal/RT-Thread/_os_time.h

Change rt_thread_mdelay to rt_hw_us_delay function:  
```patch
diff --git a/bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.c b/bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.c
index 4b64354f8..f8bc5b204 100644
--- a/bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.c
+++ b/bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.c
@@ -218,7 +218,12 @@ rt_err_t sdmmc_init(rt_device_t dev)
     int mdelay = 500;
     while (!hal_sdc_init_timeout() && mdelay > 0)
     {
-        rt_thread_mdelay(50);
+        rt_hw_us_delay(50*1000);
         mdelay -= 50;
     }
     return 0;
 
diff --git a/bsp/allwinner/libraries/sunxi-hal/include/hal/sdmmc/osal/RT-Thread/_os_time.h b/bsp/allwinner/libraries/sunxi-hal/include/hal/sdmmc/osal/RT-Thread/_os_time.h
index d96bf502f..0ae8a703e 100644
--- a/bsp/allwinner/libraries/sunxi-hal/include/hal/sdmmc/osal/RT-Thread/_os_time.h
+++ b/bsp/allwinner/libraries/sunxi-hal/include/hal/sdmmc/osal/RT-Thread/_os_time.h
@@ -87,7 +87,14 @@ extern "C" {
 /**
  * @brief Macros used to sleep for the given time (milliseconds or seconds)
  */
-#define OS_MSleep(msec)         rt_thread_delay((rt_tick_t)OS_MSecsToTicks(msec))
+#define OS_MSleep(msec)         rt_hw_us_delay(msec*1000)
+
 #define OS_Sleep(sec)           OS_MSleep((sec) * OS_MSEC_PER_SEC)
 #define OS_SSleep(sec)          OS_Sleep(sec)

diff --git a/bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.h b/bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.h
index c8aa75c44..9893e5e5c 100644
--- a/bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.h
+++ b/bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.h
@@ -13,7 +13,7 @@
 
 #include <rtdef.h>
 
-#define SDMMC_CARD_NR 2
+#define SDMMC_CARD_NR 1 //replace original 2 to 1
```

## Finlay RTT works with D1H:
```sh
 \ | /
- RT -     Thread Smart Operating System
 / | \     5.0.2 build Jun  9 2024 17:11:05
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
msh />
msh />
```