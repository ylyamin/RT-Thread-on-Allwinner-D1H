# Introduction
This document describes changes in the code compare of to the original repo.  

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
## LCD Display driver
 
In original repo LCD driver init as a application inside main function, in this case is impossible to debug it in GDB as is executed in RTT thread so change it to init as device that executed before scheduler start.
```patch
git diff bde29ac...36594b5 rt-thread/ > diff.patch

diff --git a/rt-thread/bsp/allwinner/d1s_d1h/applications/main.c b/rt-thread/bsp/allwinner/d1s_d1h/applications/main.c
index 902db7082..994011733 100644
--- a/rt-thread/bsp/allwinner/d1s_d1h/applications/main.c
+++ b/rt-thread/bsp/allwinner/d1s_d1h/applications/main.c
@@ -13,11 +13,5 @@
 int main(void)
 {
     printf("Hello RISC-V\n");
-
-#ifdef BSP_USING_LCD
-    extern int rt_hw_lcd_init(void);
-    rt_hw_lcd_init();
-#endif // BSP_USING_LCD
-
     return 0;
 }

diff --git a/rt-thread/bsp/allwinner/libraries/drivers/drv_lcd.c b/rt-thread/bsp/allwinner/libraries/drivers/drv_lcd.c
index 327aa777f..0b1737df2 100644
--- a/rt-thread/bsp/allwinner/libraries/drivers/drv_lcd.c
+++ b/rt-thread/bsp/allwinner/libraries/drivers/drv_lcd.c
@@ -750,7 +750,7 @@ int rt_hw_lcd_init(void)
 
     return RT_EOK;
 }
-// INIT_DEVICE_EXPORT(rt_hw_lcd_init);
+ INIT_DEVICE_EXPORT(rt_hw_lcd_init); //when init as a device and not a application in main then could be debugged by gdb
 
 static int lcd_draw_point(int args, char *argv[])
 {
@@ -795,7 +795,7 @@ static int lcd_draw_point(int args, char *argv[])
     }
 
+     *((uint32_t *)lcd_drv->framebuffer + lcd_drv->lcd_info.width * y + x) = 0xffff0000;
-    // *((uint32_t *)lcd->framebuffer + lcd_drv->lcd_info.width * y + x + 2) = 0xff00ff00;
```
## Add LCD Display driver for RGB Display

Im my setup I use Sipeed Lichee RV + Dock + Lichee RV Dock extension LCD adapter board + 4.3 RGB LCD Display (043026-N6(ML)) with IC ST7001s (SPI)   
Use ST7001s SPI driver from https://github.com/Tina-Linux/LCD_Panel_Driver/tree/master/LCD/BH040I-01_ST7701s_RGB_480x480   
Also cross check with this implementation https://github.com/xboot/xboot/blob/master/src/arch/riscv64/mach-lichee86p/driver/fb-d1-rgb.c   
```patch
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/st7701s_rgb.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/st7701s_rgb.h
```
According https://github.com/xboot/xboot/blob/master/src/arch/riscv64/mach-lichee86p/romdisk/boot/lichee86p.json created configuration for this particular LCD board:
```patch
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/RGB_LCD_ST7001s.c
```
#### According this image:   
![display_pinout](Sipeed_Lichee_RV\20181022143917_27009.jpg)

#### Pinout for 4.3 RGB LCD Display:

| LCD         |   RV_Dock_EXT_3517   |  GPIO Function  |
|:---         |:---                  |:---             |
| 1   LEDK    |   pd22_bl (5v)       |                 |
| 2   LEDA    |   pd22_bl (5v)       |                 |
| 3   GND     |                      |                 |
| 4   VCC     |   out 3.3V           |                 |
| 5   R0      |       (gnd)          |                 |
| 6   R1      |       (gnd)          |                 |
| 7   R2      |   pd12               |   LCD0-D18      |
| 8   R3      |   .                  |   .             |
| 9   R4      |   .                  |   .             |
| 10  R5      |   .                  |   .             |
| 11  R6      |   .                  |   .             |
| 12  R7      |   pd17               |   LCD0-D23      |
| 13  G0      |       (gnd)          |                 |
| 14  G1      |       (gnd)          |                 |
| 15  G2      |   pd6                |   LCD0-D10      |
| 16  G3      |   .                  |   .             |
| 17  G4      |   .                  |   .             |
| 18  G5      |   .                  |   .             |
| 19  G6      |   .                  |   .             |
| 20  G7      |   pd11               |   LCD0-D15      |
| 21  B0      |       (gnd)          |                 |
| 22  B1      |       (gnd)          |                 |
| 23  B2      |   pd0                |   LCD0-D2       |
| 24  B3      |   .                  |   .             |
| 25  B4      |   .                  |   .             |
| 26  B5      |   .                  |   .             |
| 27  B6      |   .                  |   .             |
| 28  B7      |   pd5                |   LCD0-D7       |
| 29  GND     |                      |                 |
| 30  CLK     |   pd18               |   LCD0-CLK      |
| 31  DISP    |   (NC)               |                 |
| 32  Hsync   |   pd20               |   LCD0-HSYNK    |
| 33  Vsync   |   pd21               |   LCD0-VSYNK    |
| 34  DEN     |   pd19               |   LCD0-DE       |
| 35  NC      |   (NC)               |                 |
| 36  GND     |                      |                 |
| 37  XR      |   pg13               |   Reset         |
| 38  YD      |   pe12               |   SDI           |
| 39  XL      |   pe15               |   SCL           |
| 40  YU      |   pe16               |   CS ?          |

## Add LCD Display driver for MIPI DSI Display   
Im my setup I use ClockworkPi DevTerm R01 that use 6.86 inch LCD MIPI DSI Display with inc9707 chip.   
Use inc9707 driver from https://github.com/cuu/last_
linux-5.4/blob/master/drivers/video/fbdev/sunxi/disp2/disp/lcd/icn9707_480x1280.c
```patch
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/icn9707_480x1280.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/icn9707_480x1280.h
```
According https://github.com/clockworkpi/DevTerm/blob/main/Code/patch/d1/board.dts
```
&lcd0 {
	lcd_used            = <1>;

	lcd_driver_name     = "icn9707_480x1280";
	lcd_backlight       = <50>;
	lcd_if              = <4>;

	lcd_x               = <480>;
	lcd_y               = <1280>;
	lcd_width           = <60>;
	lcd_height          = <160>;
	lcd_dclk_freq       = <55>;
.
.
.
```
Created configuration for this particular LCD board:   
```patch
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/icn9707_mipi_config.c
```
## Add definitions of drivers structures
```patch
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/panels.c b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/panels.c
index 963f99356..7063c315f 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/panels.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/panels.c
@@ -80,6 +80,9 @@ struct __lcd_panel *panel_array[] = {
 #ifdef CONFIG_LCD_SUPPORT_ST7701S
     &st7701s_panel,
 #endif
+#ifdef CONFIG_LCD_SUPPORT_ST7701S_RGB
+    &st7701s_rgb_panel,
+#endif
 #ifdef CONFIG_LCD_SUPPORT_T30P106
     &t30p106_panel,
 #endif
@@ -111,6 +114,10 @@ struct __lcd_panel *panel_array[] = {
     &tft08006_panel,
 #endif
 
+#ifdef CONFIG_LCD_SUPPORT_ICN9707_480x1280
+    &icn9707_480x1280_panel,
+#endif
+
     NULL,
 };

diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/panels.h b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/panels.h
index 079b6545f..90897c039 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/panels.h
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/panels.h
@@ -126,6 +126,9 @@ extern struct __lcd_panel st7796s_panel;
 #ifdef CONFIG_LCD_SUPPORT_ST7701S
 extern struct __lcd_panel st7701s_panel;
 #endif
+#ifdef CONFIG_LCD_SUPPORT_ST7701S_RGB
+extern struct __lcd_panel st7701s_rgb_panel;
+#endif
 #ifdef CONFIG_LCD_SUPPORT_T30P106
 extern struct __lcd_panel t30p106_panel;
 #endif
@@ -144,9 +147,11 @@ extern struct __lcd_panel rt13qv005d_panel;
 #ifdef CONFIG_LCD_SUPPORT_ST7789V_CPU
 extern struct __lcd_panel st7789v_cpu_panel;
 #endif
-
 #ifdef CONFIG_LCD_SUPPORT_VVX07H005A10
 extern struct __lcd_panel VVX07H005A10_panel;
 #endif
+#ifdef CONFIG_LCD_SUPPORT_ICN9707_480x1280
+extern struct __lcd_panel icn9707_480x1280_panel;
+#endif
 
 #endif
```
## Add configs and make-files for drivers:
```patch
diff --git a/rt-thread/bsp/allwinner/libraries/drivers/Kconfig b/rt-thread/bsp/allwinner/libraries/drivers/Kconfig
index 36cef4f2d..f489985a9 100644
--- a/rt-thread/bsp/allwinner/libraries/drivers/Kconfig
+++ b/rt-thread/bsp/allwinner/libraries/drivers/Kconfig
@@ -3,10 +3,21 @@ menu "General Drivers Configuration"
 choice
     prompt "Choose Board"
 
-    default BSP_USING_M7
+    default BSP_USING_SIPEED_LICHEE_RV
 
     config BSP_USING_M7
         bool "Persimmon M7 board"
+
+    config BSP_USING_SIPEED_LICHEE_RV
+        bool "Sipeed Lichee RV board"
+        select LCD_SUPPORT_ST7701S_RGB
+        select RGB_LCD_ST7001S
+        select ARCH_SUN20IW1
+
+    config BSP_USING_CWP_DT_R01
+        bool "ClockworkPi DevTerm R01 board"
+        select LCD_SUPPORT_ICN9707_480x1280
+        select MIPI_DSI_LCD_ICN9707
+        select ARCH_SUN20IW1

diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/Kconfig b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/Kconfig
index 66ad6500f..00fef5fd8 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/Kconfig
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/Kconfig
@@ -114,6 +114,13 @@ config LCD_SUPPORT_ST7701S
     ---help---
         If you want to support ST7701S panel for display driver, select it.
 
+config LCD_SUPPORT_ST7701S_RGB
+    bool "LCD support ST7701S RGB panel"
+    default n
+    ---help---
+        If you want to support ST7701S RGB panel for display driver, select it.
+
 config LCD_SUPPORT_T30P106
     bool "LCD support T30P106 panel"
     default n
@@ -179,3 +186,9 @@ config LCD_SUPPORT_VVX07H005A10
     ---help---
         If you want to support VVX07H005A10 panel for display driver, select it.
 
+config LCD_SUPPORT_ICN9707_480x1280
+    bool "LCD support ICN9707_480x1280 panel"
+    default n
+    ---help---
+        If you want to support ICN9707_480x1280 panel for display driver, select it.
\ No newline at end of file

diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/Kconfig b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/Kconfig
index b1e7cd7d1..a4a5e1c6c 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/Kconfig
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/Kconfig
@@ -42,6 +42,20 @@ config DEFAULT_LVDS
     ---help---
       board, disp with lvds
 
+config RGB_LCD_ST7001S
+    bool "4.3 inch RGB LCD display 043026-N6(ML) with ST7001s (SPI)"
+    default n
+    ---help---
+      4.3 inch RGB LCD display 043026-N6(ML) with ST7001s (SPI)
+
+config MIPI_DSI_LCD_ICN9707
+    bool "Clockworkpi Devterm R01 board with MIPI DSI display ICN9707"
+    default n
+    ---help---
+      Clockworkpi Devterm R01 board with MIPI DSI display ICN9707
+
 endmenu
 
 menu "Soc Select"
 
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/kconfig.h b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/kconfig.h
index 278ea42c9..971fa38b0 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/kconfig.h
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/kconfig.h
@@ -30,10 +30,28 @@
 #define CONFIG_RT_CONSOLE_DEVICE_NAME "uart0"
 // #define CONFIG_MELIS_LEGACY_DRIVER_MAN 1
 #define CONFIG_RT_USING_MEMPOOL 1
+
 // #define CONFIG_LCD_SUPPORT_HE0801A068 1
 #if defined(LCD_SUPPORT_TFT08006)
 #define CONFIG_LCD_SUPPORT_TFT08006 1
 #endif
+
+#if defined(LCD_SUPPORT_ST7701S_RGB)
+#define CONFIG_LCD_SUPPORT_ST7701S_RGB 1
+#endif
+
+#if defined(RGB_LCD_ST7001S)
+#define CONFIG_RGB_LCD_ST7001S 1
+#endif
+
+#if defined(LCD_SUPPORT_ICN9707_480x1280)
+#define CONFIG_LCD_SUPPORT_ICN9707_480x1280 1
+#endif
+
+#if defined(MIPI_DSI_LCD_ICN9707)
+#define CONFIG_MIPI_DSI_LCD_ICN9707 1
+#endif
+
 #define CONFIG_RT_THREAD_PRIORITY_MAX 32
 #define CONFIG_CHARSET_UTF16LE_EN 1
 #define CONFIG_IOBOX_RWCHECK 1

diff --git a/rt-thread/bsp/allwinner/d1s_d1h/.config b/rt-thread/bsp/allwinner/d1s_d1h/.config
index 17f622d73..25fa3db03 100644
--- a/rt-thread/bsp/allwinner/d1s_d1h/.config
+++ b/rt-thread/bsp/allwinner/d1s_d1h/.config

+CONFIG_RT_USING_LCD=y
+CONFIG_BSP_USING_LCD=y
+CONFIG_DISP2_SUNXI=y
+CONFIG_ARCH_SUN20IW1=y
-CONFIG_LCD_SUPPORT_HE0801A068=y
+CONFIG_LCD_SUPPORT_ST7701S_RGB=y
+CONFIG_RGB_LCD_ST7001S=y
+#CONFIG_LCD_SUPPORT_ICN9707_480x1280 is not set
+#CONFIG_MIPI_DSI_LCD_ICN9707 is not set

diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/SConscript b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/SConscript
index a22bcdcb1..165cce4ac 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/SConscript
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/SConscript
@@ -132,10 +132,23 @@ disp2_src = Split('''
 if GetDepend('LCD_SUPPORT_TFTRGB'):
     if not GetDepend('LCD_USING_XML_CONFIG'):
         disp2_src += ['./source/disp2/soc/rgb_config.c']
+
 if GetDepend('LCD_SUPPORT_TFT08006'):
     # disp2_src += ['./source/disp2/soc/tft08006_mipi_config.c']
     disp2_src += ['./source/disp2/disp/lcd/tft08006.c']
 
+if GetDepend('LCD_SUPPORT_ST7701S_RGB'):
+    disp2_src += ['./source/disp2/disp/lcd/st7701s_rgb.c']
+
+if GetDepend('RGB_LCD_ST7001S'):
+    disp2_src += ['./source/disp2/soc/RGB_LCD_ST7001s.c']
+
+if GetDepend('LCD_SUPPORT_ICN9707_480x1280'):
+    disp2_src += ['./source/disp2/disp/lcd/icn9707_480x1280.c']
+
+if GetDepend('MIPI_DSI_LCD_ICN9707'):
+    disp2_src += ['./source/disp2/soc/icn9707_mipi_config.c']
+
 sdmmc_src = Split('''
 source/sdmmc/core.c
 source/sdmmc/hal_sdpin.c
@@ -271,7 +284,7 @@ if GetDepend('DRVIERS_UART'):
 
 if GetDepend('DISP2_SUNXI'):
     CPPPATH += disp2_path
-    src += disp2_src #+ disp2_test_src
+    src += disp2_src + disp2_test_src
 
 CPPPATH += rtc_path

diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/Makefile b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/Makefile
index a919605dc..aa4d40132 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/Makefile
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/Makefile
@@ -33,6 +33,7 @@ obj-$(CONFIG_LCD_SUPPORT_LH219WQ1) += lcd/lh219wq1.o
 obj-$(CONFIG_LCD_SUPPORT_ST7789V) += lcd/st7789v.o
 obj-$(CONFIG_LCD_SUPPORT_ST7796S) += lcd/st7796s.o
 obj-$(CONFIG_LCD_SUPPORT_ST7701S) += lcd/st7701s.o
+obj-$(CONFIG_LCD_SUPPORT_ST7701S_RGB) += lcd/st7701s_rgb.o
 obj-$(CONFIG_LCD_SUPPORT_WTL096601G03) += lcd/wtl096601g03.o
 obj-$(CONFIG_LCD_SUPPORT_T30P106) += lcd/t30p106.o
 obj-$(CONFIG_LCD_SUPPORT_TO20T20000) += lcd/to20t20000.o
@@ -45,5 +46,7 @@ obj-$(CONFIG_LCD_SUPPORT_ILI9881C) += lcd/ili9881c.o
 obj-$(CONFIG_LCD_SUPPORT_TM_DSI_PANEL) += lcd/tm_dsi_panel.o
 obj-$(CONFIG_LCD_SUPPORT_ST7789V_CPU) += lcd/st7789v_cpu.o
 obj-$(CONFIG_LCD_SUPPORT_VVX07H005A10) += lcd/VVX07H005A10.o
+obj-$(CONFIG_LCD_SUPPORT_ICN9707_480x1280) += lcd/icn9707_480x1280.o
+
 
 obj-y += $(obj_low)

diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/Makefile b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/Makefile
index 7341bfc53..987f7748f 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/Makefile
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/Makefile
@@ -11,3 +11,10 @@ obj-$(CONFIG_TFT08006_MIPI)+= tft08006_mipi_config.o
 obj-$(CONFIG_HE0801A068_MIPI)+= he0801a068_mipi_config.o
 obj-$(CONFIG_DEFAULT_LVDS)+= f133_lvds.o
 
+obj-$(CONFIG_RGB_LCD_ST7001S)+= RGB_LCD_ST7001s.o
+obj-$(CONFIG_MIPI_DSI_LCD_ICN9707)+= icn9707_mipi_config.o
```
## Remove hardcoded LCD config 
Remove hardcoded LCD config from lcd_cfg.c and replaced it to build time inserted configs from driver files:
```patch
diff --git a/rt-thread/bsp/allwinner/libraries/drivers/lcd_cfg.c b/rt-thread/bsp/allwinner/libraries/drivers/lcd_cfg.c
index 023b98d06..f79fc8bc0 100644
--- a/rt-thread/bsp/allwinner/libraries/drivers/lcd_cfg.c
+++ b/rt-thread/bsp/allwinner/libraries/drivers/lcd_cfg.c
@@ -14,1154 +14,12 @@
 struct property_t *g_lcd0_config = NULL;
 static struct lcd_cfg_panel_info _panel_info = DEFAULT_LCD_CONFIG;
 
-struct property_t g_lcd0_config_rgb[] = {
-struct property_t g_lcd1_config[] = {
-struct property_t g_disp_config[] = {
-u32 g_lcd0_config_len = sizeof(g_lcd0_config_rgb) / sizeof(struct property_t);
-u32 g_lcd1_config_len = sizeof(g_lcd1_config) / sizeof(struct property_t);
-u32 g_disp_config_len = sizeof(g_disp_config) / sizeof(struct property_t);
+extern struct property_t g_lcd0_config_soc[];
+extern struct property_t g_lcd1_config[];
+extern struct property_t g_disp_config[];
+extern u32 g_lcd0_config_len;
+extern u32 g_lcd1_config_len;
+extern u32 g_disp_config_len;
 
 static struct property_t *_lcd_property_find(const char *name, struct property_t *_config, const u32 config_len)
 {
@@ -1210,15 +68,35 @@ const struct lcd_cfg_panel_info *load_lcd_config_from_xml(void)
         goto _RET;
     }
 
-    g_lcd0_config = g_lcd0_config_rgb;
-    g_lcd0_config_len = sizeof(g_lcd0_config_rgb) / sizeof(struct property_t);
+    g_lcd0_config = g_lcd0_config_soc;
+
 _RET:
 
     init_state = 2;
     _config_item = _lcd_property_find("lcd_x", g_lcd0_config, g_lcd0_config_len);
-    _panel_info.width = _config_item->v.value;
+    if(_config_item != NULL)
+    {
+        _panel_info.width = _config_item->v.value;
+    }    
     _config_item = _lcd_property_find("lcd_y", g_lcd0_config, g_lcd0_config_len);
-    _panel_info.height = _config_item->v.value;
+    if(_config_item != NULL)
+    {
+        _panel_info.height = _config_item->v.value;
+    }
+    _config_item = _lcd_property_find("bl_pin", g_lcd0_config, g_lcd0_config_len);
+    if(_config_item != NULL)
+    {
+        _panel_info.bl_pin = _config_item->v.value;
+        _panel_info.bl_gpio_pin = _config_item->v.value;
+    }
+    _config_item = _lcd_property_find("bl_level", g_lcd0_config, g_lcd0_config_len);
+    if(_config_item != NULL)
+    { 
+        _panel_info.bl_level = _config_item->v.value;
+        _panel_info.bl_gpio_level = _config_item->v.value;
+    }
+    _panel_info.bl_mode = 0;
+    _panel_info.pwr_pin = -1;
 
     return &_panel_info;
 }
```

Couldn't use g_lcd0_config instead g_lcd0_config_soc for all drivers, looks like is just linking order proble.

## LCD driver stuck
With original implementation board stuck in function hal_msleep() by some reason, replace it to rt_hw_us_delay()

```patch
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/disp_sys_intf.c b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/disp_sys_intf.c
index 09c64b396..31f370d3d 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/disp_sys_intf.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/disp_sys_intf.c
@@ -157,13 +157,22 @@ hal_clk_id_t disp_sys_clk_get_parent(hal_clk_id_t clk)
 
 s32 disp_delay_ms(u32 ms)
 {
-    return hal_msleep(ms);
+    /*
+        with original implementation hal_msleep() 
+        board stuck by some reason, replace to rt_hw_us_delay()
+    */
+    //return hal_msleep(ms);
+    rt_hw_us_delay(ms*1000);
 }
 
 s32 disp_delay_us(u32 us)
 {
-
-    return hal_usleep(us);
+    /*
+        with original implementation hal_msleep() 
+        board stuck by some reason, replace to rt_hw_us_delay()
+    */
+    //return hal_usleep(us);
+    rt_hw_us_delay(us);
 }
```
## RGB LCD Display work
After all modification in code Sipeed Lichee RV - RGB LCD Display successfully started, tested by command:
```sh
lcd_draw_point 100 100:   
```
![lichee_lcd_rgb_work](lichee_lcd_rgb_work.jpg)

## LCD inc9707 chip reset
DevTerm LCD MIPI DSI Display still not started. Lets figure out why.   

Noticed in [ICNL9707_Datasheet.pdf](ClockworkPi_DevTerm\ICNL9707_Datasheet.pdf) that in startup siguence - first off all VCI and IOVCC voltage UP then reseted RESET pin.   

![icnl9707_startup](ClockworkPi_DevTerm/icnl9707_startup.png)   

But between powered up board, bootloaders, RTT loaded and the moment when the driver calls RESET pin obviously more time passes rather than on diagram.    
Hypotise that it looks like we need to force reset VCI and IOVCC just before RESET pin.  

According to pnout:
![pinout](ClockworkPi_DevTerm/6.86-inch-IPS-TFT-Stretched-Bar-LCD-MIPI-480x1280-TTW686VVC-01-4.png)   

IOVCC connected to LCD pins 18,19.  
And in Devterm to IOVCC provided 1.8V from DCDC3 - AXP228 Power Managment chip.   
VCI   connected to LCD pins 38,39.   
And in Devterm to VCI   provided 3.3V from ALDO2 - AXP228 Power Managment chip.   

in Devterm R01 AXP228 controlled by TWI interface from D1H:

| AXP228         | D1H   | Function |
|:---            |:---   |:---      |
| PMU-SCK        | PB-10 | TWI0-SCK |
| PMU-SDA        | PB-11 | TWI0-SDA |

Also confirmed by:

https://github.com/smaeul/linux/blob/d1/all/arch/riscv/boot/dts/allwinner/sun20i-d1-devterm-v3.14.dts

```json
&dsi {
	pinctrl-0 = <&dsi_4lane_pins>;
	pinctrl-names = "default";
	status = "okay";

	panel@0 {
		compatible = "clockwork,cwd686";
		reg = <0>;
		backlight = <&backlight>;
		reset-gpios = <&pio 3 19 GPIO_ACTIVE_LOW>; /* PD19/GPIO8 */
		rotation = <90>;
		iovcc-supply = <&reg_dcdc3>;
		vci-supply = <&reg_aldo2>;
	};
}
```

https://github.com/smaeul/linux/blob/d1/all/arch/riscv/boot/dts/allwinner/sun20i-d1-clockworkpi-v3.14.dts

```json
&i2c0 {
	pinctrl-0 = <&i2c0_pb10_pins>;
	pinctrl-names = "default";
	status = "okay";

	axp221: pmic@34 {
		compatible = "x-powers,axp228", "x-powers,axp221";
		reg = <0x34>;
...

			reg_dcdc3: dcdc3 {
				regulator-name = "sys-1v8";
				regulator-always-on;
				regulator-min-microvolt = <1800000>;
				regulator-max-microvolt = <1800000>;
			};

			reg_aldo2: aldo2 {
				regulator-name = "disp-3v3";
				regulator-always-on;
				regulator-min-microvolt = <3300000>;
				regulator-max-microvolt = <3300000>;
			};
```
Power siguence defined in DRM driver:

https://github.com/smaeul/linux/blob/d1/all/drivers/gpu/drm/panel/panel-clockwork-cwd686.c

```C
static int cwd686_prepare(struct drm_panel *panel)
{
...
	err = regulator_enable(ctx->vci);
	if (err < 0) {
		dev_err(ctx->dev, "failed to enable vci supply: %d\n", err);
		return err;
	}
	err = regulator_enable(ctx->iovcc);
	if (err < 0) {
		dev_err(ctx->dev, "failed to enable iovcc supply: %d\n", err);
		goto disable_vci;
	}

	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	/* T2 */
	msleep(10);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	/* T3 */
	msleep(20);

	/* Exit sleep mode and power on */

	err = cwd686_init_sequence(ctx);
	if (err) {
		dev_err(ctx->dev, "failed to initialize display (%d)\n", err);
		goto disable_iovcc;
	}

	err = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (err) {
		dev_err(ctx->dev, "failed to exit sleep mode (%d)\n", err);
		goto disable_iovcc;
	}
	/* T6 */
	msleep(120);

	err = mipi_dsi_dcs_set_display_on(dsi);
	if (err) {
		dev_err(ctx->dev, "failed to turn display on (%d)\n", err);
		goto disable_iovcc;
	}
	msleep(20);
...
```
According AXP 228 documentation [AXP228_V1.1_20130106..pdf](ClockworkPi_DevTerm/AXP228_V1.1_20130106..pdf) chip culd be ontrolled by TWI interface (I2C) and have control register:

REG 10H: DCDC1/2/3/4/5&ALDO1/2&DC5LDO Enable Set

| Bit  | Description 	   | Value 		  | R/W  | Default |
| :--- | :---		 	   | :---  		  | :--- |:---     |
| 7    | ALDO2 Enable Set  | 0: Off 1: On | RW   | X 	   |
| 6    | ALDO1 Enable Set  | 			  | RW   | X 	   |
| 5    | DCDC5 Enable Set  | 			  | RW   | X 	   |
| 4    | DCDC4 Enable Set  | 			  | RW   | X 	   |
| 3    | DCDC3 Enable Set  | 			  | RW   | X 	   |
| 2    | DCDC2 Enable Set  | 			  | RW   | X 	   |
| 1    | DCDC1 Enable Set  | 			  | RW   | X 	   |
| 0    | DC5LDO Enable Set | 			  | RW   | X 	   |

## So lets implement a simple driver for control AXP228 (ALDO2 and DCDC30) by TWI interface:

```patch
git diff 36594b5...b0ca16e rt-thread/ > diff.patch
                   
diff --git a/rt-thread/bsp/allwinner/libraries/drivers/SConscript b/rt-thread/bsp/allwinner/libraries/drivers/SConscript
index 5dcf08dd7..ffafee242 100644
--- a/rt-thread/bsp/allwinner/libraries/drivers/SConscript
+++ b/rt-thread/bsp/allwinner/libraries/drivers/SConscript
@@ -15,6 +15,7 @@ if GetDepend('BSP_USING_RTC'):
 
 if GetDepend('BSP_USING_I2C'):
     src += ['drv_i2c.c']
+    src += ['drv_axp228_simpl.c']
 
diff --git a/rt-thread/bsp/allwinner/libraries/drivers/drv_axp228_simpl.c b/rt-thread/bsp/allwinner/libraries/drivers/drv_axp228_simpl.c
new file mode 100644
index 000000000..f8339d8cc
--- /dev/null
+++ b/rt-thread/bsp/allwinner/libraries/drivers/drv_axp228_simpl.c
@@ -0,0 +1,272 @@
+/*
+ * Copyright (c) ylyamin
+ */
+#include <rthw.h>
+#include <rtdevice.h>
+
+#include "sunxi_hal_twi.h"
+
+#define DBG_LVL DBG_WARNING
+#define DBG_TAG "drv/axp"
+#include <rtdbg.h>
+
+#define AXP228_ADDR (0x69 >> 1)
+
+#define DCDC_CNTRL_REG 0x10
+#define ELDO_CNTRL_REG 0x12
+#define ALDO_CNTRL_REG 0x13
+
+#define DLDO1_Voltage_REG 0x15
+#define DLDO2_Voltage_REG 0x16
+#define DLDO3_Voltage_REG 0x17
+#define DLDO4_Voltage_REG 0x18
+#define ELDO1_Voltage_REG 0x19
+#define ELDO2_Voltage_REG 0x1A
+#define ELDO3_Voltage_REG 0x1B
+#define DC5LD_Voltage_REG 0x1C
+#define DCDC1_Voltage_REG 0x21
+#define DCDC2_Voltage_REG 0x22
+#define DCDC3_Voltage_REG 0x23
+#define DCDC4_Voltage_REG 0x24
+#define DCDC5_Voltage_REG 0x25
+#define ALDO1_Voltage_REG 0x28
+#define ALDO2_Voltage_REG 0x29
+#define ALDO3_Voltage_REG 0x2A
+
+//DCDC_CNTRL_REG
+#define ALDO2_Enable_mask BIT(7)
+#define ALDO1_Enable_mask BIT(6)
+#define DCDC5_Enable_mask BIT(5)
+#define DCDC4_Enable_mask BIT(4)
+#define DCDC3_Enable_mask BIT(3)
+#define DCDC2_Enable_mask BIT(2)
+#define DCDC1_Enable_mask BIT(1)
+#define DC5LD_Enable_mask BIT(0)
+
+//ELDO_CNTRL_REG
+#define DC1SW_Enable_mask BIT(7)
+#define DLDO4_Enable_mask BIT(6)
+#define DLDO3_Enable_mask BIT(5)
+#define DLDO2_Enable_mask BIT(4)
+#define DLDO1_Enable_mask BIT(3)
+#define ELDO3_Enable_mask BIT(2)
+#define ELDO2_Enable_mask BIT(1)
+#define ELDO1_Enable_mask BIT(0)
+
+//ALDO_CNTRL_REG
+#define ALDO3_Enable_mask BIT(7)
+struct dcdc_reg
+{
+    char name[40];
+    uint8_t addr;
+    uint8_t value;
+};
+
+#define ENABLE_REGISTERS {\
+            {"DCDC1-5 ALDO1-2 DC5LDO Control",DCDC_CNTRL_REG,0},\
+            {"ELDO1-3 DLDO1-4 DC1SW Control ",ELDO_CNTRL_REG,0},\
+            {"ADLDO3 Control                ",ALDO_CNTRL_REG,0},\
+                         }  
+
+#define VOLTAGE_REGISTERS {\
+            {"DLDO1  Voltage Set",DLDO1_Voltage_REG,0},\
+            {"DLDO2  Voltage Set",DLDO2_Voltage_REG,0},\
+            {"DLDO3  Voltage Set",DLDO3_Voltage_REG,0},\
+            {"DLDO4  Voltage Set",DLDO4_Voltage_REG,0},\
+            {"ELDO1  Voltage Set",ELDO1_Voltage_REG,0},\
+            {"ELDO2  Voltage Set",ELDO2_Voltage_REG,0},\
+            {"ELDO3  Voltage Set",ELDO3_Voltage_REG,0},\
+            {"DC5LDO Voltage Set",DC5LD_Voltage_REG,0},\
+            {"DCDC1  Voltage Set",DCDC1_Voltage_REG,0},\
+            {"DCDC2  Voltage Set",DCDC2_Voltage_REG,0},\
+            {"DCDC3  Voltage Set",DCDC3_Voltage_REG,0},\
+            {"DCDC4  Voltage Set",DCDC4_Voltage_REG,0},\
+            {"DCDC5  Voltage Set",DCDC5_Voltage_REG,0},\
+            {"ALDO1  Voltage Set",ALDO1_Voltage_REG,0},\
+            {"ALDO2  Voltage Set",ALDO2_Voltage_REG,0},\
+            {"ALDO3  Voltage Set",ALDO3_Voltage_REG,0},\
+    };  
+
+void _axp_set_reg(struct rt_i2c_bus_device *i2c_bus, uint8_t reg_addr, uint8_t reg_value)
+{
+    struct rt_i2c_msg msg[1];
+    uint8_t buf[2] = {reg_addr,reg_value};
+
+    msg[0].addr = AXP228_ADDR;
+    msg[0].flags = RT_I2C_WR;
+    msg[0].buf = buf;
+    msg[0].len = sizeof(buf);
+    rt_i2c_transfer(i2c_bus, &msg[0], 1);
+}
+
+void _axp_get_reg(struct rt_i2c_bus_device *i2c_bus, uint8_t reg_addr, uint8_t *reg_value)
+{
+    struct rt_i2c_msg msg[2];
+    uint8_t buf[1] = {reg_addr};
+
+    msg[0].addr = AXP228_ADDR;
+    msg[0].flags = RT_I2C_WR;
+    msg[0].buf = buf;
+    msg[0].len = sizeof(buf);
+
+    msg[1].addr = AXP228_ADDR;
+    msg[1].flags = RT_I2C_RD;
+    msg[1].buf = buf;
+    msg[1].len = sizeof(buf);
+
+    rt_i2c_transfer(i2c_bus, &msg[0], 2);
+    *reg_value = msg[1].buf[0];
+}
+
+void _axp_update_reg(struct rt_i2c_bus_device *i2c_bus, uint8_t reg_addr, uint8_t val, uint8_t mask)
+{
+    uint8_t reg_val = 0;
+    _axp_get_reg(i2c_bus, reg_addr, &reg_val);
+
+    if ((reg_val & mask) != val) {
+        reg_val = (reg_val & ~mask) | (val & mask);
+        _axp_set_reg(i2c_bus, reg_addr, reg_val);
+    }
+}
+
+static void _axp_get_regsisters(struct dcdc_reg *dcdc_regs, uint8_t dcdc_regs_len)
+{
+    uint8_t reg_value = 0;
+    struct rt_i2c_bus_device *i2c_bus;
+    i2c_bus = (struct rt_i2c_bus_device*)rt_device_find("i2c0");
+
+    if (!i2c_bus)
+    {
+        rt_kprintf("i2c0 not found\n");
+        return;
+    }
+
+    for (int i =0;i < dcdc_regs_len;i++)
+    {
+        _axp_get_reg(i2c_bus, dcdc_regs[i].addr, &reg_value);
+        dcdc_regs[i].value = reg_value;
+        //rt_hw_us_delay(100);
+    }
+}
+
+void _axp_LCD_control(int on)
+{
+    uint8_t value = 0;
+    struct rt_i2c_bus_device *i2c_bus;
+    i2c_bus = (struct rt_i2c_bus_device*)rt_device_find("i2c0");
+
+    if (!i2c_bus)
+    {
+        rt_kprintf("i2c0 not found\n");
+        return;
+    }
+
+    if (on) value = DCDC3_Enable_mask | ALDO2_Enable_mask;
+    _axp_update_reg(i2c_bus, DCDC_CNTRL_REG, value, DCDC3_Enable_mask | ALDO2_Enable_mask);
+}
+
+static void _axp_LCD_control_cmd(int argc, char *args[])
+{
+    if (argc < 2) return;
+    int on = atoi(args[1]);
+    _axp_LCD_control(on);
+}
+MSH_CMD_EXPORT_ALIAS(_axp_LCD_control_cmd, axp_LCD_control_cmd, axp_LCD_control_cmd);
+
+static void _axp_get_voltage(void)
+{
+    struct dcdc_reg dcdc_regs[] = VOLTAGE_REGISTERS;
+    uint8_t dcdc_regs_len = sizeof(dcdc_regs) / sizeof(struct dcdc_reg);
+
+    _axp_get_regsisters(dcdc_regs, dcdc_regs_len);
+
+    for (int i =0;i < dcdc_regs_len;i++)
+    {
+        rt_kprintf("Reg name:'%s' addr: %x value: %d\n",
+        dcdc_regs[i].name, 
+        dcdc_regs[i].addr, 
+        dcdc_regs[i].value);
+    }
+}
+MSH_CMD_EXPORT_ALIAS(_axp_get_voltage, axp_get_voltage, axp_get_voltage);
+
+static void _axp_get_enable(void)
+{
+    struct dcdc_reg dcdc_regs[] = ENABLE_REGISTERS;
+    uint8_t dcdc_regs_len = sizeof(dcdc_regs) / sizeof(struct dcdc_reg);
+
+    _axp_get_regsisters(dcdc_regs, dcdc_regs_len);
+
+    for (int i =0;i < dcdc_regs_len;i++)
+    {
+        rt_kprintf("Reg name:'%s' addr: %x value: %x\n",
+        dcdc_regs[i].name, 
+        dcdc_regs[i].addr, 
+        dcdc_regs[i].value);
+
+        if (dcdc_regs[i].addr == DCDC_CNTRL_REG)
+        {
+            rt_kprintf("    ALDO2_Enable %d\n",(bool)(dcdc_regs[i].value & ALDO2_Enable_mask));
+            rt_kprintf("    ALDO1_Enable %d\n",(bool)(dcdc_regs[i].value & ALDO1_Enable_mask));
+            rt_kprintf("    DCDC5_Enable %d\n",(bool)(dcdc_regs[i].value & DCDC5_Enable_mask));
+            rt_kprintf("    DCDC4_Enable %d\n",(bool)(dcdc_regs[i].value & DCDC4_Enable_mask));
+            rt_kprintf("    DCDC3_Enable %d\n",(bool)(dcdc_regs[i].value & DCDC3_Enable_mask));
+            rt_kprintf("    DCDC2_Enable %d\n",(bool)(dcdc_regs[i].value & DCDC2_Enable_mask));
+            rt_kprintf("    DCDC1_Enable %d\n",(bool)(dcdc_regs[i].value & DCDC1_Enable_mask));
+            rt_kprintf("    DC5LD_Enable %d\n",(bool)(dcdc_regs[i].value & DC5LD_Enable_mask));
+        }
+        //
+        if (dcdc_regs[i].addr == ELDO_CNTRL_REG)
+        {
+            rt_kprintf("    DC1SW_Enable %d\n",(bool)(dcdc_regs[i].value & DC1SW_Enable_mask));
+            rt_kprintf("    DLDO4_Enable %d\n",(bool)(dcdc_regs[i].value & DLDO4_Enable_mask));
+            rt_kprintf("    DLDO3_Enable %d\n",(bool)(dcdc_regs[i].value & DLDO3_Enable_mask));
+            rt_kprintf("    DLDO2_Enable %d\n",(bool)(dcdc_regs[i].value & DLDO2_Enable_mask));
+            rt_kprintf("    DLDO1_Enable %d\n",(bool)(dcdc_regs[i].value & DLDO1_Enable_mask));
+            rt_kprintf("    ELDO3_Enable %d\n",(bool)(dcdc_regs[i].value & ELDO3_Enable_mask));
+            rt_kprintf("    ELDO2_Enable %d\n",(bool)(dcdc_regs[i].value & ELDO2_Enable_mask));
+            rt_kprintf("    ELDO1_Enable %d\n",(bool)(dcdc_regs[i].value & ELDO1_Enable_mask));
+        }
+        if (dcdc_regs[i].addr == ALDO_CNTRL_REG)
+        {
+            rt_kprintf("    ALDO3_Enable %d\n",(bool)(dcdc_regs[i].value & ALDO3_Enable_mask));
+        }
+    }
+}
+MSH_CMD_EXPORT_ALIAS(_axp_get_enable, axp_get_enable, axp_get_enable);
```
## Enable I2C and configure TWI pins:

```patch

diff --git a/rt-thread/bsp/allwinner/d1s_d1h/.config b/rt-thread/bsp/allwinner/d1s_d1h/.config
index 25fa3db03..adb50af2f 100644
--- a/rt-thread/bsp/allwinner/d1s_d1h/.config
+++ b/rt-thread/bsp/allwinner/d1s_d1h/.config
@@ -189,7 +189,9 @@ CONFIG_RT_USING_TTY=y
+CONFIG_RT_USING_I2C=y
+CONFIG_BSP_USING_I2C=y
+CONFIG_BSP_USING_I2C0=y

diff --git a/rt-thread/bsp/allwinner/libraries/drivers/drv_i2c.c b/rt-thread/bsp/allwinner/libraries/drivers/drv_i2c.c
index ad4515d8f..edcca13c4 100644
--- a/rt-thread/bsp/allwinner/libraries/drivers/drv_i2c.c
+++ b/rt-thread/bsp/allwinner/libraries/drivers/drv_i2c.c
@@ -111,7 +111,31 @@ static struct hal_i2c_bus _i2c_bus_3 = {
 
 #define CFG_GPIO_PORT(p) ((p) - 'A' + 1)
 
+
+
 static const user_gpio_set_t _i2c_gpio_cfg[][2] = {
+#ifdef BSP_USING_CWP_DT_R01
+    {// twi0
+        {
+            .gpio_name = "twi0.sck",
+            .port = CFG_GPIO_PORT('B'),
+            .port_num = 10, // PB10
+            .mul_sel = 4,
+            .pull = 1, // pull up
+            .drv_level = 3,
+            .data = 1,
+        },
+        {
+            .gpio_name = "twi0.sda",
+            .port = CFG_GPIO_PORT('B'),
+            .port_num = 11, // PB11
+            .mul_sel = 4,
+            .pull = 1, // pull up
+            .drv_level = 3,
+            .data = 1,
+        }
+    },
+#else
     {// twi0
         {
             .gpio_name = "twi0.sck",
@@ -132,6 +156,7 @@ static const user_gpio_set_t _i2c_gpio_cfg[][2] = {
             .data = 1,
         }
     },
+#endif

```
## Modify LCD driver

```patch
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/icn9707_480x1280.c b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/icn9707_480x1280.c
index f0ea916dd..835a8c888 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/icn9707_480x1280.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/icn9707_480x1280.c
static s32 LCD_open_flow(u32 sel)
 {
     printk("raoyiming +++ LCD_open_flow\n");
-    LCD_OPEN_FUNC(sel, LCD_power_on, 100);   //open lcd power, and delay 50ms
-    LCD_OPEN_FUNC(sel, LCD_panel_init, 200);   //open lcd power, than delay 200ms
+   // LCD_OPEN_FUNC(sel, LCD_power_on, 100);   //open lcd power, and delay 50ms
+    LCD_OPEN_FUNC(sel, LCD_panel_init, 0);   //open lcd power, than delay 200ms
     LCD_OPEN_FUNC(sel, sunxi_lcd_tcon_enable, 200);     //open lcd controller, and delay 100ms
     LCD_OPEN_FUNC(sel, LCD_bl_open, 0);     //open lcd backlight, and delay 0ms
-
+    printk("raoyiming +++ LCD_open_flow finish\n");
     return 0;
 }
 
@@ -203,20 +207,32 @@ static struct lcd_setting_table lcd_init_setting[] = {
 
 };
 
+extern void _axp_LCD_control(bool on);
+
 static void LCD_panel_init(u32 sel)
 {
 	
     u32 i;
     printk("<0>raoyiming +++ LCD_panel_init\n");
-	
-    /**/
+	sunxi_lcd_power_enable(sel, 0);
+    sunxi_lcd_pin_cfg(sel, 1);
+
+    /*all off*/
+    _axp_LCD_control(0);
+    sunxi_lcd_delay_ms(100);
+
+    /*start*/
     panel_rst(1);
+    _axp_LCD_control(1);
+
+    /*T2*/
     sunxi_lcd_delay_ms(10);
     panel_rst(0);
-    sunxi_lcd_delay_ms(50);
-    panel_rst(1);
-    sunxi_lcd_delay_ms(200);
 
+    /*T3*/
+    sunxi_lcd_delay_ms(20);
+
+    /*init sequence*/
     for (i = 0; ; i++) {
         if(lcd_init_setting[i].cmd == REGFLAG_END_OF_TABLE) {
             break;
@@ -227,8 +243,10 @@ static void LCD_panel_init(u32 sel)
         }
     }
 
-   sunxi_lcd_dsi_clk_enable(sel);
-
+    sunxi_lcd_dsi_clk_enable(sel);
+	/* T6 */
+	sunxi_lcd_delay_ms(120);
+    printk("<0>raoyiming +++ LCD_panel_init finish\n");
     return;
 }
 
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/icn9707_mipi_config.c b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/icn9707_mipi_config.c
index 20cdaed52..a773d6b23 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/icn9707_mipi_config.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/icn9707_mipi_config.c

@@ -228,7 +241,7 @@ struct property_t g_lcd0_config_soc[] = {
         .v.gpio_list = {
             .gpio = GPIOD(19), //LCD_RESET
             .mul_sel = GPIO_DIRECTION_OUTPUT,
-            .pull = 1,
+            .pull = 2,
             .drv_level = 3,
             .data = 1,
         },
```
## MIPI DSI LCD Display work
After all modification in code DevTerm R01 - LCD MIPI DSI Display successfully started, tested by command:
```sh
lcd_draw_point 100 100:   
```
![devterm_lcd_mipi_work.jpg](devterm_lcd_mipi_work.jpg)

## Devterm Keyboard

Hoow is connected to D1: Keyboard chip STM32F103Rx => J502 - DM1/DP1 => GL850G (usb hub) => USB_DP/USB_DM => D1H USB1-DP/USB1-DM A8/B8 => USB2.0 HOST

### Power
DCDC1 -> SYS_3V -> SYS_5V
DLDO2 / DLDO3 / DLDO4 -> 3V3 -> TPS2553 -> VBUS -> J502 Keyboard

Add power for VBUS

```patch
diff --git a/rt-thread/bsp/allwinner/libraries/drivers/drv_axp228_simpl.c b/rt-thread/bsp/allwinner/libraries/drivers/drv_axp228_simpl.c
new file mode 100644
index 000000000..9bff5c7f4
--- /dev/null
+++ b/rt-thread/bsp/allwinner/libraries/drivers/drv_axp228_simpl.c

+void _axp_USB_control(int on)
+{
+    uint8_t value_1 = 0;
+    uint8_t value_2 = 0;
+    uint8_t value_3 = 0;
+
+    struct rt_i2c_bus_device *i2c_bus;
+    i2c_bus = (struct rt_i2c_bus_device*)rt_device_find("i2c0");
+
+    if (!i2c_bus)
+    {
+        rt_kprintf("i2c0 not found\n");
+        return;
+    }
+
+    if (on) {
+        value_1 = DCDC1_Enable_mask;
+        value_2 = DLDO2_Enable_mask;
+        value_3 = 0xff;
+    }
+    _axp_update_reg(i2c_bus, DCDC_CNTRL_REG, value_1, DCDC1_Enable_mask);
+    _axp_update_reg(i2c_bus, DLDO2_Voltage_REG, value_3, 0xff);
+    _axp_update_reg(i2c_bus, ELDO_CNTRL_REG, value_2, DLDO2_Enable_mask);
+}
+
+static void _axp_USB_control_cmd(int argc, char *args[])
+{
+    if (argc < 2) return;
+    int on = atoi(args[1]);
+    _axp_USB_control(on);
+}
+MSH_CMD_EXPORT_ALIAS(_axp_USB_control_cmd, axp_USB_control_cmd, axp_USB_control_cmd);
```

### Change config to support USB Host in Components and HAL

```patch
diff --git a/rt-thread/bsp/allwinner/d1s_d1h/.config b/rt-thread/bsp/allwinner/d1s_d1h/.config
index 25fa3db03..0619d4d8d 100644
--- a/rt-thread/bsp/allwinner/d1s_d1h/.config
+++ b/rt-thread/bsp/allwinner/d1s_d1h/.config

+CONFIG_RT_USING_USB=y
+CONFIG_RT_USING_USB_HOST=y
+CONFIG_RT_USBH_HID=y
+CONFIG_RT_USBH_HID_MOUSE=y
+CONFIG_RT_USBH_HID_KEYBOARD=y
+CONFIG_BSP_USING_USB=y
+CONFIG_DRIVERS_USB=y
+#
+# USB HOST
+#
+CONFIG_USB_HOST=y
+CONFIG_HAL_TEST_HCI=y
+# CONFIG_USB_STORAGE is not set
+# CONFIG_USB_CAMERA is not set
+CONFIG_USB_HID=y
+#
+# USB DEVICE
+#
+CONFIG_USB_DEVICE=y
+CONFIG_HAL_TEST_UDC=y
+CONFIG_USB_MANAGER=y
```
### Change build to support USB HAL
```patch
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/SConscript b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/SConscript
index 165cce4ac..beb29117f 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/SConscript
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/SConscript
@@ -237,11 +237,82 @@ if GetDepend('DRIVERS_TWI'):
     twi_src += ['source/twi/hal_twi.c']
     twi_path += [cwd + '/source/twi']
 
+# USB udc
 udc_src = Split('''
 source/usb/platform/sun20iw1/usb_sun20iw1.c
+source/usb/udc/hal_udc.c
 ''')
-# source/usb/udc/hal_udc.c
-udc_path = [cwd + '/source/usb/udc', cwd + '/source/usb/include']
+udc_path = [cwd + '/source/usb/udc']
+udc_path += [cwd + '/source/usb/include']
+
+udc_test_src = Split('''
+test/usb/udc/usb_msg.c
+test/usb/udc/main.c
+test/usb/udc/ed_test.c
+''')
+
+# USB host
+host_src = Split('''
+source/usb/include/usb_os_platform.c
+source/usb/include/list_head_ext.c
+source/usb/include/usb_gen_dev_mod.c
+source/usb/include/usb_drv_dev_macth.c
+source/usb/include/usb_utils_find_zero_bit.c
+source/usb/platform/sun20iw1/usb_sun20iw1.c
+source/usb/core/usb_virt_bus.c
+source/usb/core/usb_msg_base.c
+source/usb/core/usb_msg.c
+source/usb/core/usb_gen_hub_base.c
+source/usb/core/usb_gen_hub.c
+source/usb/core/usb_gen_hcd_rh.c
+source/usb/core/usb_gen_hcd.c
+source/usb/core/usb_core_interface.c
+source/usb/core/usb_core_init.c
+source/usb/core/usb_core_config.c
+source/usb/core/usb_core_base.c	
+source/usb/core/usb_driver_init.c
+source/usb/core/urb.c
+source/usb/host/ehci-hcd.c
+source/usb/host/sunxi-hci.c
+source/usb/host/ehci-sunxi.c
+source/usb/host/hal_hci.c
+source/usb/hid/Client/KeyBoard/KeyBoard.c
+source/usb/hid/Client/misc_lib.c
+source/usb/hid/Class/Hid.c
+source/usb/hid/Class/HidProtocol.c
+source/usb/hid/Class/HidTransport.c
+source/usb/manager/usb_manager.c
+source/usb/manager/usb_msg_center.c
+source/usb/manager/usb_hw_scan.c
+source/usb/hid/Client/Mouse/UsbMouse.c
+source/usb/hid/Client/Mouse/UsbMouse_DriftControl.c
+''')
+#source/usb/hid/Client/Mouse/UsbMouse.c
+#source/usb/hid/Client/Mouse/UsbMouse_DriftControl.c
+
+#source/usb/storage/Class/mscProtocol.c
+#source/usb/storage/Class/mscTransport.c
+#source/usb/storage/Class/mscTransport_i.c
+#source/usb/storage/Class/usb_msc.c
+#source/usb/storage/Disk/BlkDev.c
+#source/usb/storage/Disk/CD.c
+#source/usb/storage/Disk/Disk.c
+#source/usb/storage/Disk/LunMgr.c
+#source/usb/storage/Disk/Scsi2.c
+#source/usb/storage/Misc/usbh_buff_manager.c
+#source/usb/storage/Misc/usbh_disk_info.c
+#source/usb/storage/Misc/usbh_disk_remove_time.c
+
+host_test_src = Split('''
+test/usb/host/test_hci.c
+test/usb/host/hci_ed_test.c
+''')
+
+host_path =  [cwd + '/source/usb/include']
+host_path += [cwd + '/source/usb/core']
+host_path += [cwd + '/source/usb/storage/include']
+host_path += [cwd + '/source/usb/hid/Include']
+host_path += [cwd + '/source/usb/host']
 
 ce_src = Split('''
 source/ce/ce_common.c
@@ -312,8 +383,11 @@ if GetDepend('DRIVERS_G2D'):
 
 if GetDepend('DRIVERS_USB'):
     if GetDepend('USB_DEVICE'):
-        src += udc_src
+        src += udc_src + udc_test_src
         CPPPATH += udc_path
+    if GetDepend('USB_HOST'):
+        src += host_src + host_test_src
+        CPPPATH += host_path
 
 if GetDepend('DRIVERS_CE'):
     src += ce_src
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/kconfig.h b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/kconfig.h
index 971fa38b0..40ba78a6d 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/kconfig.h
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/kconfig.h
@@ -61,7 +61,7 @@
-#define CONFIG_USB_STORAGE 1
+//#define CONFIG_USB_STORAGE 1
+#define CONFIG_USB_HID 1
```
### Fix USB HAL problems
```patch
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hcd.c b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hcd.c
index 57c6ac5d2..4cf06807f 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hcd.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hcd.c
@@ -556,6 +556,8 @@ s32 usb_add_hc_gen_dev(struct hc_gen_dev *hcd, u32  irqnum, u32 irqflags)
     hal_log_info("----3--usb_add_hc_gen_dev\n");
     //--<2>--此hcd设备支持的速度
     rh_dev->speed = (hcd->driver->flags & HC_DRIVER_FLAG_HCD_USB2) ? USB_SPEED_HIGH : USB_SPEED_FULL;
+/*
+ * function start() is not assigned anywhere     
 
     //--<3>--start这个设备
     if ((retval = hcd->driver->start(hcd)) < 0)
@@ -563,6 +565,7 @@ s32 usb_add_hc_gen_dev(struct hc_gen_dev *hcd, u32  irqnum, u32 irqflags)
         hal_log_err("PANIC : usb_add_hc_gen_dev() :startup error %d", retval);
         goto err_hcd_driver_start;
     }
+*/
 
     hal_log_info("----4--usb_add_hc_gen_dev\n");
     /* hcd->driver->start() reported can_wakeup, probably with
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hub.c b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hub.c
index 45838d2a3..ddca5be68 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hub.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hub.c
@@ -1513,7 +1513,12 @@ static int config_descriptors_changed(struct usb_host_virt_dev *udev)
 static void hub_post_reset(struct usb_hub *hub)
 {
     hub_activate(hub);
-    hub_power_on(hub);
+/*
+ * firmware stuck by some reason in hub_power_on() function. 
+ * In Devterm power provided separetly from axp228
+ * comment this funtion
+ */
+    // hub_power_on(hub);
 }
 
 /*
@@ -1760,7 +1765,12 @@ static void hub_events(u32 flag)
             {
                 hal_log_err("over-current change on port %d", i);
                 clear_port_feature(hdev, i, USB_PORT_FEAT_C_OVER_CURRENT);
-                hub_power_on(hub);
+/*
+ * firmware stuck by some reason in hub_power_on() function. 
+ * In Devterm power provided separetly from axp228
+ * comment this funtion
+ */
+                //hub_power_on(hub);
             }
 
             if (portchange & USB_PORT_STAT_C_RESET)
@@ -1810,7 +1820,12 @@ static void hub_events(u32 flag)
                 hal_msleep(50);  /* Cool down */
 #endif
                 clear_hub_feature(hdev, C_HUB_OVER_CURRENT);
-                hub_power_on(hub);
+/*
+ * firmware stuck by some reason in hub_power_on() function. 
+ * In Devterm power provided separetly from axp228
+ * comment this funtion
+ */
+                //!hub_power_on(hub);
             }
         }
 
@@ -2028,8 +2043,13 @@ re_enumerate:
  * USB_STATE_NOTATTACHED then all of udev's descendants' states are also set
  * to USB_STATE_NOTATTACHED.
  */
+#ifdef RT_USING_USB 
+void usb_set_device_state(struct usb_host_virt_dev *udev,
+                          udevice_state_t new_state)
+#else
 void usb_set_device_state(struct usb_host_virt_dev *udev,
                           enum usb_device_state new_state)
+#endif
 {
     uint32_t flags;
     //USB_OS_ENTER_CRITICAL(flags);
@@ -2689,8 +2709,14 @@ static int _hub_config(struct usb_hub *hub, struct usb_endpoint_descriptor *endp
     {
         hub->indicator [0] = INDICATOR_CYCLE;
     }
+/*
+ * firmware stuck by some reason in hub_power_on() function. 
+ * In Devterm power provided separetly from axp228
+ * comment this funtion
+
 
     hub_power_on(hub);
+*/
     hal_log_info("_hub_config--15--");
     //--<6>--发送get status的urb
     hub_activate(hub);
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hub.h b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hub.h
index a2670c45b..cbd4dfdd3 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hub.h
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/core/usb_gen_hub.h
@@ -4,8 +4,11 @@
 
 #include "usb_gen_hub_base.h"
 #define usb_endpoint_out(ep_dir)    (!((ep_dir) & USB_DIR_IN))
-
+#ifdef RT_USING_USB 
+void usb_set_device_state(struct usb_host_virt_dev *udev, udevice_state_t new_state);
+#else
 void usb_set_device_state(struct usb_host_virt_dev *udev, enum usb_device_state new_state);
+#endif
 void usb_disable_endpoint(struct usb_host_virt_dev *dev, unsigned int epaddr);
 
 void usb_disconnect(struct usb_host_virt_dev **pdev);

 
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/host/ehci-sunxi.c b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/host/ehci-sunxi.c
index 180050279..ac0f56252 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/host/ehci-sunxi.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/host/ehci-sunxi.c
@@ -318,7 +318,11 @@ int sunxi_ehci_hcd_init(int hci_num)
         sprintf(sunxi_ehci->hci_name, "%s", hci_table[hci_num].name);
 
         hci_clock_init(sunxi_ehci);
+        /* 
+         * As in system we dont have USB config, 
+         * and seems this configuration related to GPIO that not used, comment this function
         sunxi_hci_get_config_param(sunxi_ehci);
+        */
 
         sunxi_ehci->open_clock          = open_clock;
         sunxi_ehci->close_clock         = close_clock;
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/ch9.h b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/ch9.h
index 586348eb5..f8a60dab3 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/ch9.h
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/ch9.h
@@ -404,6 +404,7 @@ struct usb_endpoint_descriptor {
 
 
 /* USB_DT_DEVICE_QUALIFIER: Device Qualifier descriptor */
+#ifndef RT_USING_USB 
 struct __attribute__((packed)) usb_qualifier_descriptor
 {
     uint8_t  bLength;
@@ -417,6 +418,7 @@ struct __attribute__((packed)) usb_qualifier_descriptor
     uint8_t  bNumConfigurations;
     uint8_t  bRESERVED;
 };
+#endif
 
 
 #define USB_DT_ENDPOINT_SIZE        7
@@ -544,15 +546,20 @@ enum usb_device_speed {
     USB_SPEED_SUPER,            /* usb 3.0 */
     USB_SPEED_SUPER_PLUS,           /* usb 3.1 */
 };
+#ifndef RT_USING_USB 
 enum usb_device_state {
     USB_STATE_NOTATTACHED = 0,
     USB_STATE_ATTACHED,
-    USB_STATE_POWERED,          /* wired */
-    USB_STATE_RECONNECTING,         /* auth */
-    USB_STATE_UNAUTHENTICATED,      /* auth */
-    USB_STATE_DEFAULT,          /* limited function */
+    USB_STATE_POWERED,          // wired
+    USB_STATE_RECONNECTING,         // auth 
+    USB_STATE_UNAUTHENTICATED,      // auth 
+    USB_STATE_DEFAULT,          // limited function 
     USB_STATE_ADDRESS,
-    USB_STATE_CONFIGURED,           /* most functions */
+    USB_STATE_CONFIGURED,           // most functions 
     USB_STATE_SUSPENDED
 };
+#else
+#include <drivers/usb_common.h>
+#endif
+
 #endif /*__USB_CH9_H__*/
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/usb.h b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/usb.h
index 0ef4dd27a..e85e50be1 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/usb.h
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/usb.h
@@ -465,7 +465,11 @@ struct usb_device {
     int     devnum;
     char        devpath[16];
     u32     route;
+    #ifdef RT_USING_USB 
+    udevice_state_t   state;
+    #else
     enum usb_device_state   state;
+    #endif
     enum usb_device_speed   speed;
 
     struct usb_tt   *tt;
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/usb_host_common.h b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/usb_host_common.h
index 73a69d4b5..c82520925 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/usb_host_common.h
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/include/usb_host_common.h
@@ -183,7 +183,11 @@ struct usb_host_virt_dev
     hal_sem_t    usb_virt_dev_semi;
 
     int devnum;                     /* Address on USB bus */
-    enum usb_device_state   state;  /* 设备当前的state,configured, not attached */
+    #ifdef RT_USING_USB 
+    udevice_state_t   state;  /* 设备当前的state,configured, not attached */
+    #else
+    enum usb_device_state   state; 
+    #endif
     enum usb_device_speed   speed;  /* 设备的当前速度 */
     int     ttport;                 /* device port on that tt hub  */
 
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/manager/usb_manager.c b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/manager/usb_manager.c
index 3a1e6e6ba..f90107831 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/manager/usb_manager.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/manager/usb_manager.c
@@ -185,6 +185,9 @@ static __s32 usb_script_parse(usb_cfg_t *cfg)
     user_gpio_set_t gpio_set = {0};
 
     /* usbc enable */
+/*
+ * As in system we dont have USB config, override all values
+
     ret = Hal_Cfg_GetKeyValue(SET_USB0, KEY_USB_ENABLE, (int32_t *)&value, 1);
     Usb_Manager_INFO("value:%d, ret:%d\n", value, ret);
     if (ret)
@@ -192,36 +195,47 @@ static __s32 usb_script_parse(usb_cfg_t *cfg)
         Usb_Manager_Err("script_parser_fetch %s %s fail \n", SET_USB0, KEY_USB_ENABLE);
         cfg->port.enable = 0;
         return -1;
-    }
-    cfg->port.enable = value;
-    if (cfg->port.enable == 0)
+    }   
+*/
+    cfg->port.enable = 1; //value;
+
+/*
+     if (cfg->port.enable == 0)
     {
         Usb_Manager_INFO("%s not used!\n", SET_USB0);
         return -1;
-    }
+    } 
+*/
     /* usbc port type */
+/*     
+    
     ret = Hal_Cfg_GetKeyValue(SET_USB0, KEY_USB_PORT_TYPE, (int32_t *)&value, 1);
     Usb_Manager_INFO("port_type:%d, ret:%d\n", value, ret);
     if (ret)
     {
         Usb_Manager_Err("script_parser_fetch %s %s fail \n", SET_USB0, KEY_USB_PORT_TYPE);
         return -1;
-    }
-    cfg->port.port_type = value;
+    } 
+*/
+    cfg->port.port_type = USB_PORT_TYPE_HOST; //value;
+/* 
     if (cfg->port.port_type != USB_PORT_TYPE_OTG)
     {
         Usb_Manager_INFO("%s cfg->port.port_type:%d!\n", SET_USB0, cfg->port.port_type);
         return 0;
-    }
+    }    
+*/
     /* usbc det mode */
-    ret = Hal_Cfg_GetKeyValue(SET_USB0, KEY_USB_DET_TYPE, (int32_t *)&value, 1);
+/*
+     ret = Hal_Cfg_GetKeyValue(SET_USB0, KEY_USB_DET_TYPE, (int32_t *)&value, 1);
     Usb_Manager_Debug("detect_type:%d, ret:%d\n", value, ret);
     if (ret)
     {
         Usb_Manager_Err("script_parser_fetch %s %s fail \n", SET_USB0, KEY_USB_DET_TYPE);
         return -1;
     }
-    cfg->port.detect_type = value;
+ */
+    cfg->port.detect_type = USB_DETECT_TYPE_DP_DM;//value;
 
     if (cfg->port.detect_type == USB_DETECT_TYPE_VBUS_ID)
     {
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/udc/hal_udc.c b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/udc/hal_udc.c
index 01e3a9e14..008a5136a 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/udc/hal_udc.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/usb/udc/hal_udc.c
@@ -1759,7 +1759,12 @@ int32_t hal_udc_init(void)
         log_udc_err("open udc clk failed\n");
         return -1;
     }
+    /*
+    * As in system we dont have USB config, 
+    * and seems this configuration related to GPIO that not used, comment this function
+
     sunxi_udc_get_config_param();
+    */
     udc_init();
 
     return 0;
 
diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/include/hal/sunxi_hal_common.h b/rt-thread/bsp/allwinner/libraries/sunxi-hal/include/hal/sunxi_hal_common.h
index ab2d8ad10..d6cad7f7f 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/include/hal/sunxi_hal_common.h
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/include/hal/sunxi_hal_common.h
@@ -32,6 +32,26 @@ extern "C"
 #include <stdio.h>
 #include <kconfig.h>
 
+/* 
+ * Add missing functions for usb hal
+ */
+
+#if (defined(__GNUC__) && (__GNUC__ >= 3))
+#define likely(expr)	(__builtin_expect(!!(expr), 1))
+#define unlikely(expr)	(__builtin_expect(!!(expr), 0))
+#else
+#define likely(expr)	(!!(expr))
+#define unlikely(expr)	(!!(expr))
+#endif
+
+#define ERR_PTR(err)    ((void *)((long)(err)))
+#define PTR_ERR(ptr)    ((long)(ptr))
+#define IS_ERR(ptr)     ((unsigned long)(ptr) > (unsigned long)(-1000))
+/* 
+ * End add missing functions for usb hal
+ */
```
### Add USB driver
```patch
diff --git a/rt-thread/bsp/allwinner/libraries/drivers/Kconfig b/rt-thread/bsp/allwinner/libraries/drivers/Kconfig
index f489985a9..def4a0100 100644
--- a/rt-thread/bsp/allwinner/libraries/drivers/Kconfig
+++ b/rt-thread/bsp/allwinner/libraries/drivers/Kconfig

+menuconfig BSP_USING_USB
+    bool "Enable USB"
+    default n
+

diff --git a/rt-thread/bsp/allwinner/libraries/drivers/SConscript b/rt-thread/bsp/allwinner/libraries/drivers/SConscript
index 5dcf08dd7..19b2a5862 100644
--- a/rt-thread/bsp/allwinner/libraries/drivers/SConscript
+++ b/rt-thread/bsp/allwinner/libraries/drivers/SConscript
@@ -15,6 +15,7 @@ if GetDepend('BSP_USING_RTC'):
 
 if GetDepend('BSP_USING_I2C'):
     src += ['drv_i2c.c']
 
 if GetDepend('BSP_USING_SPI'):
     src += ['drv_spi.c']
@@ -26,6 +27,9 @@ if GetDepend('BSP_USING_LCD'):
     src += ['drv_lcd.c']
     src += ['lcd_cfg.c']
 
+if GetDepend('BSP_USING_USB'):
+    src += ['drv_usbh.c']
+
 group = DefineGroup('Drivers', src, depend = [''], CPPPATH = CPPPATH)
 
 objs = [group]

diff --git a/rt-thread/bsp/allwinner/libraries/drivers/drv_usbh.c b/rt-thread/bsp/allwinner/libraries/drivers/drv_usbh.c
new file mode 100644
index 000000000..8b71888a1
--- /dev/null
+++ b/rt-thread/bsp/allwinner/libraries/drivers/drv_usbh.c
@@ -0,0 +1,38 @@
+#include <stdio.h>
+#include <kconfig.h>
+#include <platform_usb.h>
+#include <hal_log.h>
+#include <hal_cmd.h>
+#include <usb/hal_hci.h>
+
+#include <rtthread.h>
+#include <rtdevice.h>
+
+extern void _axp_USB_control(int on);
+
+#include <HidSpec.h>
+
+#include <Hid_i.h>
+
+int drv_usb(void)
+{
+    _axp_USB_control(1);
+
+    hal_usb_core_init();
+
+    for (int hci_num = 0; hci_num < USB_MAX_CONTROLLER_COUNT; hci_num++)
+    {
+        hal_usb_hcd_init(hci_num);
+    }
+    
+    //HidInit();
+
+    //HidDev_t HidDevM;
+
+    //usbMouseProbe(HidDevM);
+
+    //hal_usb_manager_init();    
+    //rt_usb_host_init("hwsc");
+ }
+ MSH_CMD_EXPORT_ALIAS(drv_usb, usb, usb);
+//INIT_DEVICE_EXPORT(drv_usb);
```
## After all Enumiration not happen

All threads in suspend state
```sh
hub-status-thread  suspend EINTRPT
hub-status-thread  suspend EINTRPT
hub-main-thread    suspend EINTRPT
```


```sh
components/

LOG_D("start enumnation");
    v
usbhost_core = rt_usbh_attatch_instance
	v
hub = rt_usbh_hub_port_change
	v
rt_usbh_hub_irq / rt_usbh_hub_thread_entry < USB_MSG_CONNECT_CHANGE
							v
				hub = rt_usbh_hub_init
							v
				usbhost = rt_usb_host_init
							v
						   test

USB_MSG_CONNECT_CHANGE
	v
rt_usbh_root_hub_connect_handler
```

/home/yury/toolchain/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu/bin/riscv64-unknown-linux-musl-addr2line -e rt-thread/bsp/allwinner/d1s_d1h/rtthread.elf -a -f 0x00000000404434d0 0x000000004044359c 0x000000004047d970 0x000000004046ce20 0x000000004046c8d2 0x0000000040449bb8

https://github.com/smaeul/linux/blob/d1/all/arch/riscv/boot/dts/allwinner/sun20i-d1.dtsi

- compare usb drivers with Melis
- try tinyUSB
- i2c thread seems also suspended with EINTRPT, need to check driver 
