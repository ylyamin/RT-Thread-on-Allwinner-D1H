## Devterm Keyboard

Hoow is connected to D1: Keyboard chip STM32F103Rx => J502 - DM1/DP1 => GL850G (usb hub) => USB_DP/USB_DM => D1H USB1-DP/USB1-DM A8/B8 => USB2.0 HOST

### Power
DCDC1 -> SYS_3V -> SYS_5V
DLDO2 / DLDO3 / DLDO4 -> 3V3 -> TPS2553 -> VBUS -> J502 Keyboard

Add power for VBUS

<details>
<summary>Code for AXP</summary>

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
</details>

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




## USB RTT HAL variant
```C
hal_usb_core_init();
hal_usb_hcd_init(1);

usb_new_phy_init or usb_phy_init
hcd->driver->start mean ehci_run()
request_irq(sunxi_ehci->irq_no, ehci_irq_handler or hal_request_irq(sunxi_ehci->irq_no, ehci_irq_handler
```
Interapt work ehci_irq: highspeed device connect

## USB D1s Melis HAL variant

Failed in ehci_irq_handler() line &ehci->regs->status

## Cherry USB variant with RTT HAL
```C
    usbh_initialize(1, SUNXI_USB_HCI1_PBASE);
```
with chery init interapt not call

## Tiny USB variant with RTT or Melis HAL
```C
    init_tinyusb();
```

if only tiny init
hcd_int_handler int_status:EHCI_INT_MASK_FRAMELIST_ROLLOVER
hcd_int_handler int_status:EHCI_INT_MASK_PORT_CHANGE

if only hal init
hcd_int_handler int_status:EHCI_INT_MASK_PORT_CHANGE

https://github.com/clockworkpi/DevTerm/blob/main/Code/patch/d1/config
```conf
CONFIG_USB_HID=y
CONFIG_USB_OHCI_LITTLE_ENDIAN=y
CONFIG_USB_SUPPORT=y
CONFIG_USB_COMMON=y
CONFIG_USB_ARCH_HAS_HCD=y
CONFIG_USB=y

CONFIG_USB_EHCI_HCD=y
CONFIG_USB_EHCI_ROOT_HUB_TT=y
CONFIG_USB_EHCI_TT_NEWSCHED=y
CONFIG_USB_EHCI_HCD_SUNXI=y

CONFIG_USB_OHCI_HCD=y
CONFIG_USB_OHCI_HCD_SUNXI=y
CONFIG_USB_SUNXI_HCD=y
CONFIG_USB_SUNXI_HCI=y
CONFIG_USB_SUNXI_EHCI0=y
CONFIG_USB_SUNXI_EHCI1=y
CONFIG_USB_SUNXI_OHCI0=y
CONFIG_USB_SUNXI_OHCI1=y

CONFIG_USB_SUNXI_UDC0=y

CONFIG_USB_SUNXI_USB=y
CONFIG_USB_SUNXI_USB_MANAGER=y
CONFIG_USB_SUNXI_USB_DEBUG=y
CONFIG_USB_SUNXI_USB_ADB=y
```


## Notes
LOG_D("start enumnation");
https://github.com/smaeul/linux/blob/d1/all/arch/riscv/boot/dts/allwinner/sun20i-d1.dtsi

LINUX

```c

#define CLK_USB_OHCI0		97
#define CLK_USB_OHCI1		98
#define CLK_BUS_OHCI0		99
#define CLK_BUS_OHCI1		100
#define CLK_BUS_EHCI0		101
#define CLK_BUS_EHCI1		102
#define CLK_BUS_OTG		     103

#define RST_USB_PHY0		40
#define RST_USB_PHY1		41
#define RST_BUS_OHCI0		42
#define RST_BUS_OHCI1		43
#define RST_BUS_EHCI0		44
#define RST_BUS_EHCI1		45
#define RST_BUS_OTG		     46


[    0.877847] phy->clk name: usb0_phy
[    0.899453] phy->reset name: usb0_reset
[    0.912893] phy->clk name: usb1_phy
[    0.916606] phy->reset name: usb1_reset

	          clocks =       <&osc24M>,
				          <&osc24M>;
			clock-names =  "usb0_phy",
				          "usb1_phy";
			resets =       <&ccu RST_USB_PHY0>, 40
				          <&ccu RST_USB_PHY1>; 41
			reset-names =  "usb0_reset",
				          "usb1_reset";

	ehci1:
			clocks =  <&ccu CLK_BUS_OHCI1>,
				     <&ccu CLK_BUS_EHCI1>,    CLK_BUS_EHCI1		102
				     <&ccu CLK_USB_OHCI1>;

			resets =  <&ccu RST_BUS_OHCI1>,
				     <&ccu RST_BUS_EHCI1>;    RST_BUS_EHCI1		45

	ohci1:
			clocks =  <&ccu CLK_BUS_OHCI1>,
				     <&ccu CLK_USB_OHCI1>;

			resets =  <&ccu RST_BUS_OHCI1>;

clk_prepare_enable(phy->clk);           osc24M
clk_prepare_enable(phy->clk2);          0
reset_control_deassert(phy->reset);     RST_USB_PHY1 = 41

of_clk_get(dev->dev.of_node, clk);      CLK_BUS_EHCI1 = 102
reset_control_deassert(priv->rsts);     RST_BUS_EHCI1 = 45



```

RTT
```c
#define CLK_USB_OHCI0       100
#define CLK_USB_OHCI1       101
#define CLK_BUS_OHCI0       102
#define CLK_BUS_OHCI1       103
#define CLK_BUS_EHCI0       104
#define CLK_BUS_EHCI1       105
#define CLK_BUS_OTG         106
#define RST_USB_PHY0        42
#define RST_USB_PHY1        43
#define RST_BUS_OHCI0       44
#define RST_BUS_OHCI1       45
#define RST_BUS_EHCI0       46
#define RST_BUS_EHCI1       47
#define RST_BUS_OTG         48


	ehci-0
		.usb_clk = CLK_BUS_EHCI0 = 104
		.usb_rst = RST_BUS_EHCI0 = 46
		.phy_clk = 0,
		.phy_rst = RST_USB_PHY0  = 42
	ehci-1
		.usb_clk = CLK_BUS_EHCI1 = 105
		.usb_rst = RST_BUS_EHCI1 = 47
		.phy_clk = 0,
		.phy_rst = RST_USB_PHY1  = 43
	ohci-0
		.ohci_clk = CLK_USB_OHCI0,
		.usb_clk  = CLK_BUS_OHCI0,
		.usb_rst  = RST_BUS_OHCI0,
		.phy_clk  = 0,
		.phy_rst  = RST_USB_PHY0
	ohci-1
		.ohci_clk = CLK_USB_OHCI1,
		.usb_clk  = CLK_BUS_OHCI1,
		.usb_rst  = RST_BUS_OHCI1,
		.phy_clk  = 0,
		.phy_rst  = RST_USB_PHY1
	otg
          .usb_clk = CLK_BUS_OTG,
          .usb_rst = RST_BUS_OTG,
          .phy_clk = 0,
          .phy_rst = RST_USB_PHY0

	bus_clk_id    = usb_clk CLK_BUS_EHCI1 = 105
	reset_bus_clk = usb_rst RST_BUS_EHCI1 = 47
	phy_clk_id    = phy_clk  0
	reset_phy_clk = phy_rst  RST_USB_PHY1  = 43
	ohci_clk_id   = ohci_clk CLK_USB_OHCI1 = 101

reset_control_get(reset_phy_clk)   RST_USB_PHY1  = 43
reset_control_deassert(reset_phy)
reset_control_put(reset_phy)

reset_control_get(reset_bus_clk)   RST_BUS_EHCI1 = 47
reset_control_deassert(reset_hci)
reset_control_put(reset_hci)

clock_get(phy_clk_id)    0
clock_enable(phy_clk)

clock_get(bus_clk_id)    CLK_BUS_EHCI1 = 105
clock_enable(bus_clk)

clock_get(ohci_clk_id)   CLK_USB_OHCI1 = 101
clock_enable(ohci_clk)   

```



OTG_PBASE		0x04100000
phy otg        0x04100400

EHCI0     	0x04101000
OHCI0          0x04101400
phy hci0       0x04101800

EHCI1     	0x04200000
OHCI1          0x04200400
phy hci1       0x04200800



```s
sun4i_usb_phy_probe
sun4i_usb_phy_probe data->base 4125400

sun4i_usb_phy_init				phy writel 412d 810<-0			cntrl clear
								phy writel 4125 410<-20			| 5 & 3	SIDDQ
sun4i_usb_phy_passby			phy writel 412d 800<-701		| 10 9 8 0
phy->index == 0                                 
sun4i_usb_phy0_update_iscr		phy writel 4125 400<-40010000	& 0 | 16 
sun4i_usb_phy0_update_iscr		phy writel 4125 400<-40030000	& 0 | 17


(1)sun4i_usb_phy0_id_vbus_det_scan					 								Changing dr_mode to 1
(1)sun4i_usb_phy0_set_id_detect                    			
(1)sun4i_usb_phy0_update_iscr		phy writel 4125 400<-4003c000	2?3 << 14 ISCR_FORCE_ID
(1)sun4i_usb_phy0_set_vbus_detect                  
(1)sun4i_usb_phy0_update_iscr		phy writel 4125 400<-4103f000	2?3 << 12  ISCR_FORCE_VBUS				External vbus detected, not enabling our own vbus
(1)sun4i_usb_phy0_set_vbus_detect					 								ehci-platform 4101000.usb: EHCI Host Controller
(1)sun4i_usb_phy0_update_iscr		phy writel 4125 400<-4303e000	2?3 << 12  ISCR_FORCE_VBUS
											    
sun4i_usb_phy_init				phy writel 4141 810<-0			cntrl clear
sun4i_usb_phy_passby			phy writel 4141 800<-701		| 10 9 8 0		ehci-platform 4200000.usb: EHCI Host Controlle
(1)sun4i_usb_phy0_set_vbus_detect	                
(1)sun4i_usb_phy0_update_iscr		phy writel 4125 400<-4103f000					ohci-platform 4101400.usb: Generic Platform OHCI controller
(1)sun4i_usb_phy_passby			phy writel 412d 800<-0			& 10 9 8 0	
(1)sun4i_usb_phy0_reroute			phy writel 4125 420<-1			Route phy0 to MUSB

(2)sun4i_usb_phy0_id_vbus_det_scan                 
(2)sun4i_usb_phy0_set_vbus_detect                  
(2)sun4i_usb_phy0_update_iscr		phy writel 4125 400<-4303e000					ohci-platform 4200400.usb: Generic Platform OHCI controller
(2)sun4i_usb_phy0_set_vbus_detect                  
(2)sun4i_usb_phy0_update_iscr		phy writel 4125 400<-4103f000
								phy writel 4125 400<-4303b000
(2)sun4i_usb_phy_passby			phy writel 412d 800<-701
(2)sun4i_usb_phy0_reroute			phy writel 4125 420<-0			Route phy0 to EHCI/OHCI
```

```s
sun4i_usb_phy_probe
data->base phy_ctrl virt_addr: 4125 400
data->base phy_ctrl phy__addr: 4100 400		phy otg

phy->pmu pmu0 virt_addr: 412d 800
phy->pmu pmu0 phy__addr: 4101 800			phy hci0
							  
phy->pmu pmu1 virt_addr: 4141 800
phy->pmu pmu1 phy__addr: 4200 800			phy hci1 

sun4i_usb_phy_init			phy writel 4101 810<-0			cntrl clear
						phy writel 4100 410<-20			| 5 & 3	SIDDQ
sun4i_usb_phy_passby		phy writel 4101 800<-701		| 10 9 8 0
phy->index == 0                             
sun4i_usb_phy0_update_iscr	phy writel 4100 400<-40010000	& 0 | 16 
sun4i_usb_phy0_update_iscr	phy writel 4100 400<-40030000	& 0 | 17
										    
										    
sun4i_usb_phy_init			phy writel 4200 810<-0			cntrl clear
sun4i_usb_phy_passby		phy writel 4200 800<-701		| 10 9 8 0
```

```s
[ehci-usb1] insmod host driver!
USBC_Clean_SIDDP
phy write: 4200810<-0
phy_vbase : 0x4200800, usbc_no : 1, efuse : 0x1e9200f 
vref
usb_phy_tp_write
phy write ctrl: 4200810<-2
phy writeB: 4200810<-2
phy write: 4200810<-6002
phy writeB: 4200810<-2
phy writeB: 4200810<-3
phy writeB ctrl: 4200810<-2
phy writeB ctrl: 4200810<-0
usb_phy_tp_write
phy write ctrl: 4200810<-2
phy writeB: 4200810<-2
phy write: 4200810<-4402
phy writeB: 4200810<-2
phy writeB: 4200810<-3
phy writeB: 4200810<-2
phy write: 4200810<-4502
phy writeB: 4200810<-82
phy writeB: 4200810<-83
phy writeB: 4200810<-82
phy write: 4200810<-4682
phy writeB: 4200810<-2
phy writeB: 4200810<-3
phy writeB: 4200810<-2
phy write: 4200810<-4702
phy writeB: 4200810<-82
phy writeB: 4200810<-83
phy writeB ctrl: 4200810<-82
phy writeB ctrl: 4200810<-80
usb_phy_tp_write
phy write ctrl: 4200810<-82
phy writeB: 4200810<-82
phy write: 4200810<-3682
phy writeB: 4200810<-82
phy writeB: 4200810<-83
phy writeB: 4200810<-82
phy write: 4200810<-3782
phy writeB: 4200810<-82
phy writeB: 4200810<-83
phy writeB: 4200810<-82
phy write: 4200810<-3882
phy writeB: 4200810<-82
phy writeB: 4200810<-83
phy writeB ctrl: 4200810<-82
phy writeB ctrl: 4200810<-80
usb_passby
phy write: 4200800<-701
sunxi_set_vbus

[ohci-usb1] insmod host driver!
USBC_Clean_SIDDP
phy write: 4200810<-3880
phy_vbase : 0x4200800, usbc_no : 1, efuse : 0x1e9200f 
vref
usb_phy_tp_write
phy write ctrl: 4200810<-82
phy writeB: 4200810<-82
phy write: 4200810<-6082
phy writeB: 4200810<-2
phy writeB: 4200810<-3
phy writeB ctrl: 4200810<-2
phy writeB ctrl: 4200810<-0
usb_phy_tp_write
phy write ctrl: 4200810<-2
phy writeB: 4200810<-2
phy write: 4200810<-4402
phy writeB: 4200810<-2
phy writeB: 4200810<-3
phy writeB: 4200810<-2
phy write: 4200810<-4502
phy writeB: 4200810<-82
phy writeB: 4200810<-83
phy writeB: 4200810<-82
phy write: 4200810<-4682
phy writeB: 4200810<-2
phy writeB: 4200810<-3
phy writeB: 4200810<-2
phy write: 4200810<-4702
phy writeB: 4200810<-82
phy writeB: 4200810<-83
phy writeB ctrl: 4200810<-82
phy writeB ctrl: 4200810<-80
usb_phy_tp_write
phy write ctrl: 4200810<-82
phy writeB: 4200810<-82
phy write: 4200810<-3682
phy writeB: 4200810<-82
phy writeB: 4200810<-83
phy writeB: 4200810<-82
phy write: 4200810<-3782
phy writeB: 4200810<-82
phy writeB: 4200810<-83
phy writeB: 4200810<-82
phy write: 4200810<-3882
phy writeB: 4200810<-82
phy writeB: 4200810<-83
phy writeB ctrl: 4200810<-82
phy writeB ctrl: 4200810<-80
usb_passby
phy write: 4200800<-701
sunxi_set_vbus

```


TODO USB:
- clock init ?
- linux ehci reg
- DMA, MMU ?

TODO:
- try PR for d1s in d1h
- create PR for d1h common folder
- create article in forum


Articles:
1) Introdaction
2) D1 Boot process, bootloaders, debbuger, gdb
3) Make structure
4) RTT structure, first boot
5) LCD, AXP
6) USB