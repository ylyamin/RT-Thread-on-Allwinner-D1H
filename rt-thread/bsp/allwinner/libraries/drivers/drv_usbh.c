#include <stdio.h>
#include <kconfig.h>
#include <platform_usb.h>
#include <hal_log.h>
#include <hal_cmd.h>
#include <usb/hal_hci.h>

#include <rtthread.h>
#include <rtdevice.h>

/* extern struct sunxi_hci_hcd *g_sunxi_ehci;

#include <sunxi-hci.h>
#include <usb_gen_hcd.h>
 */


extern void _axp_USB_control(int on);

#include <usbh_core.h>
#include <usbh_hid.h>
#include "../sunxi-hal/hal/source/usb/platform/sun20iw1/usb_sun20iw1.h"


/* extern struct usbh_hid *usbh_hid_class_alloc(void);
extern void usbh_hid_run(struct usbh_hid *hid_class);
extern int usb_core_init(void); */

int drv_usb(void)
{

    usbh_initialize(0, SUNXI_USB_EHCI0_PBASE);
    usbh_initialize(1, SUNXI_USB_EHCI1_PBASE);
    //usbh_initialize(2, SUNXI_USB_OTG_PBASE);

    //_axp_USB_control(1);


    //struct usbh_hid *hid = usbh_hid_class_alloc();
    //usbh_hid_run(hid);

/*     
    hal_usb_core_init();
    hal_usb_hci_init();

    struct sunxi_hci_hcd *sunxi_ehci = g_sunxi_ehci;

    rt_kprintf("desc %s\n",sunxi_ehci->hcd->driver->product_desc);

    hal_usb_manager_init();    
    rt_device_t dev = rt_device_find("hwsc");
    rt_device_init(dev);
    rt_device_open(dev,RT_DEVICE_OFLAG_OPEN);
    rt_usb_host_init("hwsc"); 
*/

}
MSH_CMD_EXPORT_ALIAS(drv_usb, usb, usb);
INIT_DEVICE_EXPORT(drv_usb);