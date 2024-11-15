/*
 * Copyright (c) ylyamin
 */
#include <stdio.h>
#include <rtthread.h>
#include <drv_usbh.h>
#include <interrupt.h>
#include <host/usbh.h>
#include <portable/ehci/ehci_api.h>
#include <tusb_rt_thread_port.h>

extern void _axp_USB_control(int on);
extern void hcd_int_handler(uint8_t rhport, bool in_isr);
void ehci_hcd_int_handler(void);

int tusb_board_init(void)
{
    _axp_USB_control(1);

	volatile uint32_t *usb_ctrl = (uint32_t * ) (EHCI1_BASE + 0x800);
	volatile uint32_t *phy_ctrl = (uint32_t * ) (EHCI1_BASE + 0x810);
	volatile uint32_t *portsc   = (uint32_t * ) (EHCI1_BASE + 0x054);
	volatile uint32_t *confflag = (uint32_t * ) (EHCI1_BASE + 0x050);

	CCU->USB1_CLK_REG &= ~(BV(24) | BV(25));
	CCU->USB1_CLK_REG |= BV(24); // usb clock 12M divided from 24MHz 
	CCU->USB1_CLK_REG |= BV(30) | BV(31); // rst USB1 phy, gating

	CCU->USB_BGR_REG &= ~BV(17);	// rst OHCI1
	CCU->USB_BGR_REG &= ~BV(21);	// rst EHCI1

	CCU->USB_BGR_REG |= BV(21); // rst EHCI1
	CCU->USB_BGR_REG |= BV(5); // gating EHCI1

	CCU->USB_BGR_REG |= BV(17); // rst OHCI1
	CCU->USB_BGR_REG |= BV(1); // gating OHCI1


	*phy_ctrl &= ~BV(3); // SIDDQ 0: Write 0 to enable phy

	// Use INCR16 when appropriate
	// Use INCR8 when appropriate
	// Use INCR4 when appropriate
	// Start INCRx burst only on burst x-align address
	// Enable UTMI interface, disable ULPI interface
	*usb_ctrl |= BV(11) | BV(10) | BV(9) | BV(8) | BV(0);

	//*confflag = 1;
	//*portsc |= BV(13);

    int irq_no = 49;
    request_irq(irq_no, ehci_hcd_int_handler, 0, "ehci", NULL);
    return 0;
}

bool hcd_init(uint8_t rhport)
{
  return ehci_init(rhport, (uint32_t) EHCI1_BASE, (uint32_t) EHCI1_BASE+0x10);
}

void ehci_hcd_int_handler(void)
{
	hcd_int_handler(0,0);
}

void hcd_int_enable(uint8_t rhport)
{
	(void)rhport;

    int irq_no = 49;
    enable_irq(irq_no);
}

void hcd_int_disable(uint8_t rhport)
{
	(void)rhport;

    int irq_no = 49;
    disable_irq(irq_no);
}


void drv_usb_tiny(void)
{
    init_tinyusb();
}
//MSH_CMD_EXPORT_ALIAS(drv_usb_tiny, usb_tiny, usb);
INIT_DEVICE_EXPORT(drv_usb_tiny);

/* 

int drv_usb_hal(void)
{
    _axp_USB_control(1);
    //hal_udc_init();
    hal_usb_core_init();
	hal_usb_hcd_init(0);
	//hal_usb_hcd_init(1);

    //hal_usb_manager_init();
    //rt_usb_host_init("hwsc"); 
}
MSH_CMD_EXPORT_ALIAS(drv_usb_hal, usb_hal, usb);
//INIT_DEVICE_EXPORT(drv_usb_hal); 

*/
