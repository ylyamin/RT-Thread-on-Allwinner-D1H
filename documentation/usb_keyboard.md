## Devterm Keyboard

So we have working display as output from board need to have way to input information - keyboard.

Keyboard in Devterm is actually standalone USB keyboard with STM32F103Rx chip connected to mainboard by USB interface. 
![usb_keyboard_connector_1](Pics/usb_keyboard_connector_1.png)

From [Mainboard PCB outline schematic file](ClockworkPi_DevTerm/pcb_3.14_botton.pdf) we can see is connected to J502 connector
![usb_keyboard_connector_2](Pics/usb_keyboard_connector_2.png)

As you can see from the [Mainboard schematic file](ClockworkPi_DevTerm/clockwork_Mainboard_V3.14_V5_Schematic.pdf) From J502 connector (DM1/DP1 pins) is go to => GL850G chip (is USB hub) DM0/DP0 => USB_DP/USB_DM then to R01 Core board

![usb_keyboard_mb_comm](Pics/usb_keyboard_mb_comm.png)

In R01 Core [schematic file](ClockworkPi_DevTerm/clockwork_DevTerm_R01_Core_for_Mainboard_V3.14_Schematic.pdf) is connected to D1H USB1-DP/USB1-DM (A8/B8 pins)
![usb_keyboard_r01_comm](Pics/usb_keyboard_r01_comm.png)

From [D1H Datasheet](Allwinner_D1H/D1_Datasheet_V0.1_Draft_Version.pdf) we can found A8/B8 pins connected to USB2.0 HOST number 1.
![D1H_usb_pins](Pics/D1H_usb_pins.png)
![D1H_system_diagram_usb](Pics/D1H_system_diagram_usb.png)

The [D1H User manual](Allwinner_D1H/D1_User_Manual_V0.1_Draft_Version.pdf) document does not contain much information about USB interface. Defined that
* USB0 is USB2.0 Dual role device (Device/Host controller) we not interesting this.
* USB1 is USB2.0 Host where we have our keyboard

But before deep dive to this controller and communication lets look how this USB device powered.

### Keyboard power

So J502 1st pin connected to VBAT line that provided from TPS2553 Power switch chip. This chip have main power SYS_5V and looks like signal for Enabling from 3V3 line. USB hub chip powered from SYS_5V line.

![usb_keyboard_power_1](Pics/usb_keyboard_power_1.png)

This 3V3 line provided from DLDO2 from AXP228 power management chip that we already know from Display powering part above. SYS_5V provided from DCDC1 AXP228.
![usb_keyboard_power_2](Pics/usb_keyboard_power_2.png)

For powering USB need to enable  
* DCDC1 for SYS_3V -> SYS_5V
* DLDO2 for 3V3 -> VBUS

### Lets modify AXP228 driver to add power control for VBUS

<details><summary>add _axp_USB_control() function</summary>

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

Now is can be powered. Look to software side in [rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/](rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/) folder we have folder usb.

File defined build rules for RTT sunxi-hal/hal/SConscript define onlyusb/platform/sun20iw1/usb_sun20iw1.c file at all. So lets define all host related stuff:
### Change build to support USB HAL
<details><summary>build files</summary>

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
</details>

In test for HAL rt-thread\bsp\allwinner\libraries\sunxi-hal\hal\test\usb\host\test_hci.c understandable how USB stack should be inited so lets create driver for RTT to init HAL:

### Add USB driver
<details><summary>driver</summary>

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
</details>

But nothing happen

LOG_D("start enumnation");




## USB RTT HAL variant
```c
hal_usb_core_init();
hal_usb_hcd_init(1);

usb_new_phy_init or usb_phy_init
hcd->driver->start mean ehci_run()
request_irq(sunxi_ehci->irq_no, ehci_irq_handler or hal_request_irq(sunxi_ehci->irq_no, ehci_irq_handler
```
Interapt work ehci_irq: highspeed device connect




![D1H_usb_host_phy](Pics/D1H_usb_host_phy.png)

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

https://github.com/smaeul/linux/blob/d1/all/arch/riscv/boot/dts/allwinner/sun20i-d1.dtsi


### Clock

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


```c
OTG_PBASE		0x04100000
phy otg        0x04100400

EHCI0     	0x04101000
OHCI0          0x04101400
phy hci0       0x04101800

EHCI1     	0x04200000
OHCI1          0x04200400
phy hci1       0x04200800
```

### PHY up
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


### TODO USB:
- clock init ?
- linux ehci reg
- DMA, MMU ?