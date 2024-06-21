# Introduction
This document describes the new changes made to the code copmare of to the original repo.  
At the original repository [RT-Thread](https://github.com/RT-Thread/rt-thread) the compilation is not streamlined for D1H [issue 9063](https://github.com/RT-Thread/rt-thread/issues/9063). So was performed fork from [v5.0.2](https://github.com/RT-Thread/rt-thread/releases/tag/v5.0.2) and introduced several changes to make it runnable in D1H. 

To show code changes used comands
```
git diff f40e63c...16181b0 > diff1.patch
(ed7bdc7 excluded as just rename folder)
git diff bde29ac...36594b5 rt-thread/ > diff2.patch
```
## In the original repo, did trying to compile for D1H:
### On [Master](https://github.com/RT-Thread/rt-thread/commit/2866da37a02dae72200e02ad87480718002760a5) or [v5.1.0](https://github.com/RT-Thread/rt-thread/releases/tag/v5.1.0)
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
### On [v5.0.2](https://github.com/RT-Thread/rt-thread/releases/tag/v5.0.2):  
3) bsp/allwinner/d1/ - not compiled looks outdated
```
board/board.c:45:5: error: unknown type name 'rt_mmu_info'
   45 |     rt_mmu_info mmu_info;
      |     ^~~~~~~~~~~
```
4) bsp/allwinner/d1s/ - compiled, but no output to UART. 

## Not succseed for D1H so was performed fork from v5.0.2

bsp/allwinner/d1s/ taken as base and renamed to bsp/allwinner/d1s_d1h/

Existing bootloaders not show anything at uart
- bsp\allwinner\d1s\tools\boot0_sdcard_sun20iw1p1_d1s.bin 
- bsp\allwinner\d1s\sbi.bin 

Created own Makefile with relevant bootloaders:
- https://github.com/smaeul/sun20i_d1_spl.git
- https://github.com/riscv-software-src/opensbi
- https://github.com/smaeul/u-boot

## Extend cvonfig to support D1H SOC
```patch
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
## Change UART boudrate in driver
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
But loading frosen in rt_thread_mdelay function in files
- bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.c
- bsp/allwinner/libraries/sunxi-hal/include/hal/sdmmc/osal/RT-Thread/_os_time.h

Chnage it to rt_hw_us_delay function  
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
## Finnaly looks like works:
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
# LCD Display driver
 
When driver init as a application inside main is impossible to debug it in GDB as is executed in RTT thread so change it to init as device.
```patch
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
# Add LCD Display driver for RGB Display

From https://github.com/Tina-Linux/LCD_Panel_Driver/tree/master/LCD/BH040I-01_ST7701s_RGB_480x480
https://github.com/xboot/xboot/blob/master/src/arch/riscv64/mach-lichee86p/driver/fb-d1-rgb.c
```patch
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/st7701s_rgb.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/st7701s_rgb.h
```
According https://github.com/xboot/xboot/blob/master/src/arch/riscv64/mach-lichee86p/romdisk/boot/lichee86p.json
```patch
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/RGB_LCD_ST7001s.c
```
## Pinout:

LCD             RV_Dock_EXT_3517    GPIO Function

1   LEDK    -   pd22_rgn_bl (5v)
2   LEDA
3   GND
4   VCC     -   out 3.3V
5   R0      -       (gnd)
6   R1      -       (gnd)
7   R2      -   pd12        -   LCD0-D18
8   R3          .
9   R4          .
10  R5          .
11  R6          .
12  R7      -   pd17        -   LCD0-D23
13  G0      -       (gnd)
14  G1      -       (gnd)
15  G2      -   pd6         -   LCD0-D10
16  G3          .
17  G4          .
18  G5          .
19  G6          .
20  G7      -   pd11        -   LCD0-D15
21  B0      -       (gnd)
22  B1      -       (gnd)
23  B2      -   pd0         -   LCD0-D2
24  B3          .
25  B4          .
26  B5          .
27  B6          .
28  B7      -   pd5         -   LCD0-D7
29  GND
30  CLK     -   pd18        -   LCD0-CLK
31  DISP    -   (NC)
32  Hsync   -   pd20        -   LCD0-HSYNK
33  Vsync   -   pd21        -   LCD0-VSYNK
34  DEN     -   pd19        -   LCD0-DE
35  NC      -   (NC)
36  GND
37  XR      -   pg13        -   Reset ?
38  YD      -   pe12
39  XL      -   pe15
40  YU      -   pe16

# Add LCD Display driver for MIPI DSI Display

From https://github.com/cuu/last_linux-5.4/blob/master/drivers/video/fbdev/sunxi/disp2/disp/lcd/icn9707_480x1280.c
```patch
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/icn9707_480x1280.c
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/icn9707_480x1280.h
```
according https://github.com/clockworkpi/DevTerm/blob/main/Code/patch/d1/board.dts
```patch
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/soc/icn9707_mipi_config.c
```
# Add definitions
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
# Add configs and makefiles:
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
+
+    config BSP_USING_CWP_DT_R01
+        bool "ClockworkPi DevTerm R01 board"
+        select LCD_SUPPORT_ICN9707_480x1280
+        select MIPI_DSI_LCD_ICN9707
+

diff --git a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/Kconfig b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/Kconfig
index 66ad6500f..00fef5fd8 100644
--- a/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/Kconfig
+++ b/rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/disp2/disp/lcd/Kconfig
@@ -114,6 +114,13 @@ config LCD_SUPPORT_ST7701S
     ---help---
         If you want to support ST7701S panel for display driver, select it.
 
+config LCD_SUPPORT_ST7701S_RGB
+    bool "LCD support ST7701S RGB panel"
+    depends on BSP_USING_SIPEED_LICHEE_RV
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
+    depends on BSP_USING_CWP_DT_R01
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
+    depends on BSP_USING_SIPEED_LICHEE_RV
+    ---help---
+      4.3 inch RGB LCD display 043026-N6(ML) with ST7001s (SPI)
+
+config MIPI_DSI_LCD_ICN9707
+    bool "Clockworkpi Devterm R01 board with MIPI DSI display ICN9707"
+    default n
+    depends on BSP_USING_CWP_DT_R01
+    ---help---
+      Clockworkpi Devterm R01 board with MIPI DSI display ICN9707
+
 endmenu
 
 menu "Soc Select"

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
+CONFIG_LCD_SUPPORT_ICN9707_480x1280=y
+CONFIG_MIPI_DSI_LCD_ICN9707=y

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
# Remove hardcoded LCD config replaced to build time inserted config
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

#  With original implementation hal_msleep() board stuck by some reason, replace to rt_hw_us_delay()
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

# I2C













