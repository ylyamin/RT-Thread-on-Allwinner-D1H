/*
 * tusb_config.h
 *
 *  Created on: May 5, 2021
 *      Author: Jeremiah McCarthy
 */

#ifndef TUSB_CONFIG_H_
#define TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <rtdef.h>
#include <rtconfig.h>

#define CFG_TUSB_DEBUG_PRINTF rt_kprintf

#define CFG_TUSB_MCU OPT_MCU_SUN20IW1

#if defined(BSP_USING_CWP_DT_R01) || defined(BSP_USING_CWP_UC_R01)
#define TUP_USBIP_EHCI 1
#endif

#if defined(BSP_USING_SIPEED_LICHEE_RV)
#define TUP_USBIP_OHCI 1
#endif

#define CFG_TUH_ENDPOINT_MAX 	16
#define CFG_TUSB_RHPORT0_MODE OPT_MODE_HOST

//#define CFG_TUH_MSC	1
#define CFG_TUH_HID	8
//#define CFG_TUH_CDC	12	
//#define CFG_TUH_BTH	1	
//#define CFG_TUH_VENDOR 0
#define CFG_TUH_HUB 12
#define CFG_TUH_DEVICE_MAX	12

#define CFG_TUSB_DEBUG 0	// 0..3
#define TUP_OHCI_RHPORTS 1

#define CFG_TUH_ENUMERATION_BUFSIZE	2048

//--------------------------------------------------------------------+
// Board Specific Configuration
//--------------------------------------------------------------------+

// RHPort number used for device can be defined by board.mk, default to port 0
#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT      0
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------
#define CFG_TUSB_OS           OPT_OS_RTTHREAD


#define BOARD_TUD_MAX_SPEED   OPT_MODE_HIGH_SPEED
// Default is max speed that hardware controller could support with on-chip PHY
#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */

#define CFG_TUSB_MEM_SECTION  rt_section(PKG_TINYUSB_MEM_SECTION)

//#define CFG_TUSB_MEM_ALIGN  rt_align(PKG_TINYUSB_MEM_ALIGN)
//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE    64
#endif

//------------- CLASS -------------//
//#define CFG_TUD_DFU               1

// DFU buffer size, it has to be set to the buffer size used in TUD_DFU_DESCRIPTOR
#define CFG_TUD_DFU_XFER_BUFSIZE  64//(TUD_OPT_HIGH_SPEED ? 512 : 64)

#define LPC_USB_BASE  (0x04200000+0x400)



#ifdef __cplusplus
 }
#endif

#endif /* TUSB_CONFIG_H_ */
