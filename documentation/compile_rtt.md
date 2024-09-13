## First of all was trying to compile RT-Thread for D1H platform:

#### RT-Thread [Master](https://github.com/RT-Thread/rt-thread/commit/2866da37a02dae72200e02ad87480718002760a5) or [v5.1.0](https://github.com/RT-Thread/rt-thread/releases/tag/v5.1.0)
1) bsp/allwinner/d1/ - not compiled looks outdated
```
board/board.c:45:5: error: unknown type name 'rt_mmu_info'
   45 |     rt_mmu_info mmu_info;
      |     ^~~~~~~~~~~
```
2) bsp/allwinner/d1s/ - not compiled
```
/mnt/hgfs/allwinner_d1/rt-thread/components/dfs/dfs_v2/filesystems/devfs/devfs.c:68:30: error: 'struct rt_device' has no member named 'ops'; did you mean 'fops'?
   68 |             else if (device->ops && file->vnode->ref_count == 1)
      |                              ^~~
      |                              fops
```  
#### RT-Thread [v5.0.2](https://github.com/RT-Thread/rt-thread/releases/tag/v5.0.2):  
3) bsp/allwinner/d1/ - not compiled looks outdated
```
board/board.c:45:5: error: unknown type name 'rt_mmu_info'
   45 |     rt_mmu_info mmu_info;
      |     ^~~~~~~~~~~
```
4) bsp/allwinner/d1s/ - compiled, but no output to UART. 

## Conclude that at the original repository [RT-Thread](https://github.com/RT-Thread/rt-thread) the compilation is not streamlined for D1H [issue 9063](https://github.com/RT-Thread/rt-thread/issues/9063).  
So was performed fork from [v5.0.2](https://github.com/RT-Thread/rt-thread/releases/tag/v5.0.2) and introduced several changes to make it runnable in D1H. 
- All rt-thread folders moved to sub-folder rt-thread, also created folders for bootloaders, debugger, etc..
- bsp/allwinner/d1s/ was taken as base to development and renamed to bsp/allwinner/d1s_d1h/
- Other bsp was removed

## Change bootloaders

Existing bootloaders not show anything at UART, looks like targeted to D1s UART pins.
- bsp/allwinner/d1s/tools/boot0_sdcard_sun20iw1p1_d1s.bin 
- bsp/allwinner/d1s/sbi.bin 

Created own Makefile with build relevant bootloaders:
- https://github.com/smaeul/sun20i_d1_spl.git
- https://github.com/riscv-software-src/opensbi
- https://github.com/smaeul/u-boot

## Extend config to support simultaneously D1S and D1H SOC
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
## Configure UART pins for D1H
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
## Change UART baud rate in driver
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
## Problem SD card

After this changes board started and shown out to UART.
But loading frozen after "card_detect insert" message:
```
 \ | /
- RT -     Thread Smart Operating System
 / | \     5.0.2 build Jun  9 2024 17:11:05
 2006 - 2022 Copyright by RT-Thread team
hal_sdc_create 0
card_detect insert
```
Looks like is frozen on rt_thread_mdelay function by some reason in files
- bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.c
- bsp/allwinner/libraries/sunxi-hal/include/hal/sdmmc/osal/RT-Thread/_os_time.h

Change it to rt_hw_us_delay function:  
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
## Finlay looks like works:
```
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