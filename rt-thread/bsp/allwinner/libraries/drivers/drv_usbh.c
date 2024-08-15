#include <stdio.h>
#include <kconfig.h>
#include <usb_os_platform.h>
#include <hal_log.h>
#include <hal_cmd.h>
#include <rtthread.h>
#include <rtdevice.h>

extern void _axp_USB_control(int on);

// #include <usbh_core.h> //CherryUSB

int drv_usb(void)
{
    //_axp_USB_control(1);

    //usbh_initialize(0, SUNXI_USB_HCI0_PBASE);
    //usbh_initialize(1, SUNXI_USB_HCI1_PBASE);
    //usbh_initialize(2, SUNXI_USB_OTG_PBASE);

    hal_usb_core_init();
	//hal_usb_hcd_init(0);
	hal_usb_hcd_init(1);
    hal_usb_manager_init();

    //rt_usb_host_init("hwsc"); 


}
MSH_CMD_EXPORT_ALIAS(drv_usb, usb, usb);
//INIT_DEVICE_EXPORT(drv_usb);