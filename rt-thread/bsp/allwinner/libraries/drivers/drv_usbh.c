#include <stdio.h>
#include <kconfig.h>
#include <usb_os_platform.h>
#include <hal_log.h>
#include <hal_cmd.h>
#include <rtthread.h>
#include <rtdevice.h>
//#include <usbh_core.h> //CherryUSB

extern void _axp_USB_control(int on);

int drv_usb_hal(void)
{
    _axp_USB_control(1);
    //hal_udc_init();
    hal_usb_core_init();
	//hal_usb_hcd_init(0);
	hal_usb_hcd_init(1);

    //hal_usb_manager_init();
    //rt_usb_host_init("hwsc"); 
}
MSH_CMD_EXPORT_ALIAS(drv_usb_hal, usb_hal, usb);
//INIT_DEVICE_EXPORT(drv_usb_hal);

int drv_usb_cherry(void)
{
    _axp_USB_control(1);
    usbh_initialize(1, 0x04200000);
    //usbh_initialize(0, 0x04101000);
    //usbh_initialize(2, 0x04100000);
}
MSH_CMD_EXPORT_ALIAS(drv_usb_cherry, usb_cherry, usb);
//INIT_DEVICE_EXPORT(drv_usb_cherry);

extern int init_tinyusb(void);

int drv_usb_tiny(void)
{
    _axp_USB_control(1);
    init_tinyusb();
}
MSH_CMD_EXPORT_ALIAS(drv_usb_tiny, usb_tiny, usb);
//INIT_DEVICE_EXPORT(drv_usb_tiny);
//INIT_COMPONENT_EXPORT(init_tinyusb);
