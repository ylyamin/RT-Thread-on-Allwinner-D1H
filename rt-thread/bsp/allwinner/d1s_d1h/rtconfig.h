#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Project Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 20
#define RT_USING_SMART
#define RT_ALIGN_SIZE 8
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 100
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 16384
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 16384

/* kservice optimization */

#define RT_KSERVICE_USING_STDLIB
#define RT_KPRINTF_USING_LONGLONG
#define RT_USING_DEBUG
#define RT_DEBUGING_COLOR
#define RT_DEBUGING_INIT

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE
#define RT_USING_MESSAGEQUEUE_PRIORITY
#define RT_USING_SIGNALS

/* Memory Management */

#define RT_PAGE_MAX_ORDER 11
#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_MEMTRACE
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 256
#define RT_CONSOLE_DEVICE_NAME "uart0"
#define RT_VER_NUM 0x50002
#define ARCH_CPU_64BIT
#define RT_USING_CACHE
#define ARCH_MM_MMU
#define KERNEL_VADDR_START 0x40000000
#define ARCH_RISCV
#define ARCH_RISCV64

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 16384
#define RT_MAIN_THREAD_PRIORITY 10
#define RT_USING_MSH
#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 8192
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 10
#define FINSH_USING_OPTION_COMPLETION

/* DFS: device virtual file system */

#define RT_USING_DFS
#define DFS_USING_POSIX
#define DFS_USING_WORKDIR
#define DFS_FD_MAX 32
#define RT_USING_DFS_V1
#define DFS_FILESYSTEMS_MAX 8
#define DFS_FILESYSTEM_TYPES_MAX 4
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 936
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_LFN_UNICODE_0
#define RT_DFS_ELM_LFN_UNICODE 0
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 8
#define RT_DFS_ELM_MAX_SECTOR_SIZE 512
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_MUTEX_TIMEOUT 3000
#define RT_USING_DFS_DEVFS
#define RT_USING_FAL
#define FAL_DEBUG_CONFIG
#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG
#define RT_USING_LWP
#define RT_LWP_MAX_NR 30
#define LWP_TASK_STACK_SIZE 16384
#define RT_CH_MSG_MAX_NR 1024
#define LWP_CONSOLE_INPUT_BUFFER_SIZE 1024
#define LWP_TID_MAX_NR 64
#define RT_LWP_SHM_MAX_NR 64

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_UNAMED_PIPE_NUMBER 64
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 4096
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V2
#define RT_USING_TTY
#define RT_USING_I2C
#define RT_USING_PIN
#define RT_USING_NULL
#define RT_USING_ZERO
#define RT_USING_RANDOM
#define RT_USING_PWM
#define RT_USING_RTC
#define RT_USING_WDT
#define RT_USING_LCD
#define RT_USING_DEV_BUS
#define RT_USING_KTIME

/* Using USB */

#define RT_USING_USB
#define RT_USING_USB_HOST
#define RT_USBH_HID
#define RT_USBH_HID_MOUSE
#define RT_USBH_HID_KEYBOARD
#define RT_USBD_THREAD_STACK_SZ 4096

/* C/C++ and POSIX layer */

/* ISO-ANSI C layer */

/* Timezone and Daylight Saving Time */

#define RT_LIBC_USING_LIGHT_TZ_DST
#define RT_LIBC_TZ_DEFAULT_HOUR 8
#define RT_LIBC_TZ_DEFAULT_MIN 0
#define RT_LIBC_TZ_DEFAULT_SEC 0

/* POSIX (Portable Operating System Interface) layer */

#define RT_USING_POSIX_FS
#define RT_USING_POSIX_DEVIO
#define RT_USING_POSIX_STDIO
#define RT_USING_POSIX_POLL
#define RT_USING_POSIX_SELECT
#define RT_USING_POSIX_TERMIOS
#define RT_USING_POSIX_DELAY
#define RT_USING_POSIX_CLOCK
#define RT_USING_POSIX_TIMER

/* Interprocess Communication (IPC) */

#define RT_USING_POSIX_PIPE
#define RT_USING_POSIX_PIPE_SIZE 512

/* Socket is in the 'Network' category */


/* Network */


/* Utilities */

#define RT_USING_RESOURCE_ID
#define RT_USING_ADT
#define RT_USING_ADT_AVL
#define RT_USING_ADT_BITMAP
#define RT_USING_ADT_HASHMAP
#define RT_USING_ADT_REF

/* RT-Thread Utestcases */


/* RT-Thread online packages */

/* IoT - internet of things */


/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */


/* CYW43012 WiFi */


/* BL808 WiFi */


/* CYW43439 WiFi */


/* IoT Cloud */


/* security packages */


/* language packages */

/* JSON: JavaScript Object Notation, a lightweight data-interchange format */


/* XML: Extensible Markup Language */


/* multimedia packages */

/* LVGL: powerful and easy-to-use embedded GUI library */


/* u8g2: a monochrome graphic library */


/* tools packages */


/* system packages */

/* enhanced kernel services */


/* acceleration: Assembly language or algorithmic acceleration packages */


/* CMSIS: ARM Cortex-M Microcontroller Software Interface Standard */


/* Micrium: Micrium software products porting for RT-Thread */

#define PKG_USING_TINYUSB
#define PKG_TINYUSB_STACK_SIZE 2048
#define PKG_TINYUSB_THREAD_PRIORITY 8
#define PKG_TINYUSB_MEM_SECTION ".usb"
#define PKG_TINYUSB_MEM_ALIGN 4
#define PKG_TINYUSB_RHPORT_NUM_1
#define PKG_TINYUSB_RHPORT_NUM 0
#define PKG_TINYUSB_FULL_SPEED
#define PKG_TINYUSB_DEVICE_PORT_SPEED 0x0200
#define PKG_TINYUSB_HOST_ENABLE
#define PKG_TINYUSB_HOST_PORT 0
#define PKG_TINYUSB_HOST_PORT_SPEED 0x0200
#define PKG_TINYUSB_DEBUG_INFO
#define CFG_TUSB_DEBUG 3
#define PKG_USING_TINYUSB_V01401

/* peripheral libraries and drivers */

/* HAL & SDK Drivers */

/* STM32 HAL & SDK Drivers */


/* Infineon HAL Packages */


/* Kendryte SDK */


/* sensors drivers */


/* touch drivers */


/* AI packages */


/* Signal Processing and Control Algorithm Packages */


/* miscellaneous packages */

/* project laboratory */

/* samples: kernel and components samples */


/* entertainment: terminal games and other interesting software packages */


/* Arduino libraries */


/* Projects and Demos */


/* Sensors */


/* Display */


/* Timing */


/* Data Processing */


/* Data Storage */

/* Communication */


/* Device Control */


/* Other */


/* Signal IO */


/* Uncategorized */

#define BOARD_allwinnerd1
#define __STACKSIZE__ 16384

/* General Drivers Configuration */

#define BSP_USING_CWP_DT_R01

/* General Purpose UARTs */

#define BSP_USING_UART0
#define UART0_TX_USING_GPIOB8
#define UART0_RX_USING_GPIOB9
#define BSP_USING_I2C
#define BSP_USING_I2C0
#define BSP_USING_PWM
#define BSP_USING_SDMMC
#define SD_CARD_CHECK
#define BSP_USING_FS
#define BSP_USING_LCD
#define BSP_USING_USB

/* Board extended module Drivers */

#define RT_USING_SUNXI_HAL

/* UART Devices */

#define DRIVERS_UART
#define SUNXI_UART_REGISTER_UART0

/* CCMU Devices */

#define DRIVERS_CCMU
#define DRIVERS_SUNXI_CLK
#define DRIVERS_SUNXI_CCU

/* DMA Devices */

#define DRIVERS_DMA

/* GPIO Devices */

#define DRIVERS_GPIO

/* Video support for sunxi */

#define DISP2_SUNXI
#define SUNXI_DISP2_FB_DISABLE_ROTATE

/* LCD panels select */

#define LCD_SUPPORT_ICN9707_480x1280

/* Display engine feature select */

#define DISP2_SUNXI_SUPPORT_ENAHNCE

/* Soc and board select */

/* Board Select */

#define MIPI_DSI_LCD_ICN9707

/* Soc Select */

#define ARCH_SUN20IW1

/* SDMMC Devices */

#define DRIVERS_SDMMC
#define USE_SD
#define USE_SDIO
#define DETECT_CARD
#define SDC_DMA_USED
#define SDIO_IRQ_SUPPORT
#define SD_TEST
#define SDC_DMA_BUF_SIZE 64
#define DRIVERS_SDC_CDPIN_PRESENT_VAL 0

/* SPI Devices */

#define DRIVERS_SPI

/* TWI Devices */

#define DRIVERS_TWI

/* G2D Devices */

#define DRIVERS_G2D

/* USB Drivers */

#define USB
#define DRIVERS_USB
#define USB_UDC
#define HAL_TEST_USB
#define USB_HOST
#define HAL_TEST_HCI
#define USB_EHCI
#define USB_OHCI
#define USB_EHCI_0
#define USB_OHCI_0
#define USB_EHCI_1
#define USB_OHCI_1
#define USB_DEVICE
#define HAL_TEST_UDC
#define USB_GADGET
#define COMPONENT_ADBD
#define COMPONENTS_USB_GADGET_ADBD
#define DRIVERS_USB_GADGET_ADB

/* CE Devices */

#define DRIVERS_CE

/* EFUSE Devices */

#define DRIVERS_EFUSE

#endif
