#include <stdio.h>
#include <kconfig.h>
#include <platform_usb.h>
#include <hal_log.h>
#include <hal_cmd.h>
#include <usb/hal_hci.h>

#include <rtthread.h>
#include <rtdevice.h>

extern void _axp_USB_control(int on);

#include <HidSpec.h>

#include <Hid_i.h>

int drv_usb(void)
{
    _axp_USB_control(1);

    hal_usb_core_init();

    for (int hci_num = 0; hci_num < USB_MAX_CONTROLLER_COUNT; hci_num++)
    {
        hal_usb_hcd_init(hci_num);
    }
    
    //HidInit();

    //HidDev_t HidDevM;

    //usbMouseProbe(HidDevM);

    //hal_usb_manager_init();    
    //rt_usb_host_init("hwsc");
 }
 MSH_CMD_EXPORT_ALIAS(drv_usb, usb, usb);
//INIT_DEVICE_EXPORT(drv_usb);