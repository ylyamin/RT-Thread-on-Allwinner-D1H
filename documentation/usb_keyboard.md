# Devterm Keyboard
So we have working display as output from board now need to have way to input information - use keyboard.

## Keyboard connection
Keyboard in Devterm is actually standalone USB keyboard with STM32F103Rx chip connected to mainboard by USB interface. 

![usb_keyboard_connector_1](Pics/usb_keyboard_connector_1.png)

From [Mainboard PCB outline schematic file](ClockworkPi_DevTerm/pcb_3.14_botton.pdf) we can see is connected to J502 connector.

![usb_keyboard_connector_2](Pics/usb_keyboard_connector_2.png)

As you can see from the [Mainboard schematic file](ClockworkPi_DevTerm/clockwork_Mainboard_V3.14_V5_Schematic.pdf) from J502 connector (DM1/DP1 pins) is go to => GL850G chip (is USB hub) then from DM0/DP0 pins go to => USB_DP/USB_DM connected to R01 Core board.

![usb_keyboard_mb_comm](Pics/usb_keyboard_mb_comm.png)

In R01 Core [schematic file](ClockworkPi_DevTerm/clockwork_DevTerm_R01_Core_for_Mainboard_V3.14_Schematic.pdf) is connected to D1H USB1-DP/USB1-DM (A8/B8 pins)

![usb_keyboard_r01_comm](Pics/usb_keyboard_r01_comm.png)

From [D1H Datasheet](Allwinner_D1H/D1_Datasheet_V0.1_Draft_Version.pdf) we can found A8/B8 pins connected to USB2.0 HOST number 1.

![D1H_usb_pins](Pics/D1H_usb_pins.png)
![D1H_system_diagram_usb](Pics/D1H_system_diagram_usb.png)

The [D1H User manual](Allwinner_D1H/D1_User_Manual_V0.1_Draft_Version.pdf) document does not contain much information about USB interface. 

Defined that:
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

Lets modify AXP228 driver to add power control for VBUS. 
<details><summary>Implement _axp_USB_control() function</summary>

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
Now is can be powered. 

## USB HAL
Lets look to software side in [rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/](rt-thread/bsp/allwinner/libraries/sunxi-hal/hal/source/) we have folder usb.

File sunxi-hal/hal/SConscript defined build rules about HAL for RTT. 

Now for usb folder defined only one source usb/platform/sun20iw1/usb_sun20iw1.c. So lets define all host related stuff.

Lets look to sunxi-hal/hal/source/usb/Makefile and child Makefile's in folders to understand what should be included, add then to SConscript
<details><summary>Change build to support USB HAL</summary>

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

In test for USB HAL rt-thread\bsp\allwinner\libraries\sunxi-hal\hal\test\usb\host\test_hci.c is understandable how USB stack should be initiated.
Is done by functions:
* hal_usb_core_init()
* hal_usb_hcd_init()

So lets create driver with this functions for RTT to init HAL:

<details><summary>Add USB driver</summary>

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
+    //hal_usb_manager_init();    
+    //rt_usb_host_init("hwsc");
+ }
+ MSH_CMD_EXPORT_ALIAS(drv_usb, usb, usb);
+//INIT_DEVICE_EXPORT(drv_usb);
```
</details>

But after running RTT nothing happen. 

```shell
phy_vbase : 0x4100400, usbc_no : 0, efuse : 0x1e9200f
[ehci-usb0] insmod host driver!
[sunxi-ehci0]: sunxi_set_vbus cnt.
[usbh core]: adding sub dev  (config #1, interface 0)
[ohci-usb0] insmod host driver!
[sunxi-ohci0]: sunxi_set_vbus cnt.
[usbh core]: adding sub dev  (config #1, interface 0)
[ehci-usb1] insmod host driver!
[sunxi-ehci1]: sunxi_set_vbus cnt.
[usbh core]: adding sub dev  (config #1, interface 0)
[ohci-usb1] insmod host driver!
[sunxi-ohci1]: sunxi_set_vbus cnt.
[usbh core]: adding sub dev  (config #1, interface 0)
```
I expect some process of enumeration USB devices, but looks like is not rich this point.

Try to use RTT function rt_usb_host_init() from rt-thread\components\drivers\usb\usbhost\core\usbhost.c also not show anything.

### USB RTT HAL variant
Noticed when USB device connected is called interrupt handler ehci_irq_handler() and reach point where print message "ehci_irq: highspeed device connect" but then communication not start 

### USB D1s Melis HAL variant
Thinking maybe whole USB host HAL outdated or broken, lets look to other variants. I found different HAL https://github.com/Tina-Linux/d1s-melis/tree/master/ekernel/drivers
Is compiled run but also nothing really happen, when device attached is failed in ehci_irq_handler() line &ehci->regs->status.

## Documentation

In the [D1H User manual](Allwinner_D1H/D1_User_Manual_V0.1_Draft_Version.pdf) 
What we have about USB, not too much: 
* Short Overview 
* Controller and Physical layer connection diagram ![D1H_usb_host_phy](Pics/D1H_usb_host_phy.png)
* Registers list ![D1H_usb_register_list](Pics/D1H_usb_register_list.png)

What is EHCI and OHCI ? 

According [https://wiki.osdev.org/Enhanced_Host_Controller_Interface](https://wiki.osdev.org/Enhanced_Host_Controller_Interface) "The Enhanced Host Controller Interface (EHCI) is the single method of interfacing with USB 2.0"
"On a PC you will normally find the EHCI USB controller on the PCI bus - in fact it is the only access method specified in the specification. USB 2.0 supports interfacing with USB 1.0 devices. However, EHCI is NOT expected to support them. Instead, you will find an UHCI or OHCI companion controller. "

looking to [EHCI Intel specification](USB/ehci-specification-for-usb.pdf)

![ehci_system_block_diagram](Pics/ehci_system_block_diagram.png)

EHCI and OHCI is low level controllers for High and Full speed USB devices. Device can be routed to EHCI or OHCI depend what speed device is.

![ehci_host_controller](Pics/ehci_host_controller.png)

Lets first start with PHY does is initilised correctly and provide correct data to EHCI controller. As is no explanation how is should be initialized in documentation need to look to code. But I'm can't be sure that HAL code is correct. Need garanteed working example - is Linux

## Let see how is implemented in Linux.

I use article https://andreas.welcomes-you.com/boot-sw-debian-risc-v-lichee-rv and https://github.com/smaeul/linux to compile Linux kernel.

Created some scripts to atomized process:
* build/linux_image_create.sh
* build/linux_image_update_kernel.sh

In Linux USB is up successfully and device enumerated.

<details><summary>linux console output</summary>

```shell
Starting kernel ...
[    0.000000] Linux version 5.17.0-rc2-379425-g06b026a8b714-dirty (yury@yury) (riscv64-unknown-linux-gnu-gcc (C-SKY RISCV Tools V1.8.4 B20200702) 8.1.0, GNU ld (GNU Binutils) 2.32) #20 PREEMPT Mon Sep 9 07:05:32 UTC 2024

[    0.247663] usbcore: registered new interface driver usbfs
[    0.253218] usbcore: registered new interface driver hub
[    0.258568] usbcore: registered new device driver usb

[    0.441543] ehci_hcd: USB 2.0 'Enhanced' Host Controller (EHCI) Driver
[    0.448051] ehci-platform: EHCI generic platform driver
[    0.453655] ohci_hcd: USB 1.1 'Open' Host Controller (OHCI) Driver
[    0.459875] ohci-platform: OHCI generic platform driver

[    0.630352] usbcore: registered new interface driver snd-usb-audio

[    0.978834] phy phy-4100400.phy.0: Changing dr_mode to 1
[    1.842129] phy phy-4100400.phy.0: External vbus detected, not enabling our own vbus

[    1.850045] ehci-platform 4101000.usb: EHCI Host Controller
[    1.855779] ehci-platform 4101000.usb: new USB bus registered, assigned bus number 1
[    1.863877] ehci-platform 4101000.usb: irq 37, io mem 0x04101000
[    1.887139] ehci-platform 4101000.usb: USB 2.0 started, EHCI 1.00

[    1.900455] usb usb1: New USB device found, idVendor=1d6b, idProduct=0002, bcdDevice= 5.17
[    1.915139] usb usb1: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    1.922644] usb usb1: Product: EHCI Host Controller
[    1.927786] usb usb1: Manufacturer: Linux 5.17.0-rc2-379425-g06b026a8b714-dirty ehci_hcd
[    1.936129] usb usb1: SerialNumber: 4101000.usb
[    1.942243] hub 1-0:1.0: USB hub found
[    1.946548] hub 1-0:1.0: 1 port detected

[    1.970649] ehci-platform 4200000.usb: EHCI Host Controller
[    1.977091] ehci-platform 4200000.usb: new USB bus registered, assigned bus number 2
[    1.985107] ehci-platform 4200000.usb: irq 39, io mem 0x04200000
[    2.003583] ehci-platform 4200000.usb: USB 2.0 started, EHCI 1.00
[    2.010053] usb usb2: New USB device found, idVendor=1d6b, idProduct=0002, bcdDevice= 5.17
[    2.018383] usb usb2: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    2.025714] usb usb2: Product: EHCI Host Controller
[    2.030650] usb usb2: Manufacturer: Linux 5.17.0-rc2-379425-g06b026a8b714-dirty ehci_hcd
[    2.038778] usb usb2: SerialNumber: 4200000.usb
[    2.044319] hub 2-0:1.0: USB hub found
[    2.048243] hub 2-0:1.0: 1 port detected

[    2.053653] ohci-platform 4101400.usb: Generic Platform OHCI controller
[    2.060407] ohci-platform 4101400.usb: new USB bus registered, assigned bus number 3
[    2.068376] ohci-platform 4101400.usb: irq 38, io mem 0x04101400
[    2.135880] usb usb3: New USB device found, idVendor=1d6b, idProduct=0001, bcdDevice= 5.17
[    2.144213] usb usb3: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    2.151477] usb usb3: Product: Generic Platform OHCI controller
[    2.157449] usb usb3: Manufacturer: Linux 5.17.0-rc2-379425-g06b026a8b714-dirty ohci_hcd
[    2.165581] usb usb3: SerialNumber: 4101400.usb
[    2.171081] hub 3-0:1.0: USB hub found
[    2.175000] hub 3-0:1.0: 1 port detected

[    2.180667] ohci-platform 4200400.usb: Generic Platform OHCI controller
[    2.187649] ohci-platform 4200400.usb: new USB bus registered, assigned bus number 4
[    2.195661] ohci-platform 4200400.usb: irq 40, io mem 0x04200400
[    2.263855] usb usb4: New USB device found, idVendor=1d6b, idProduct=0001, bcdDevice= 5.17
[    2.272185] usb usb4: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    2.279452] usb usb4: Product: Generic Platform OHCI controller
[    2.285413] usb usb4: Manufacturer: Linux 5.17.0-rc2-379425-g06b026a8b714-dirty ohci_hcd
[    2.293539] usb usb4: SerialNumber: 4200400.usb
[    2.299038] hub 4-0:1.0: USB hub found
[    2.302957] hub 4-0:1.0: 1 port detected

[    2.608573] usb 4-1: new full-speed USB device number 2 using ohci-platform
[    2.850655] usb 4-1: New USB device found, idVendor=0483, idProduct=3748, bcdDevice= 1.00
[    2.859043] usb 4-1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[    2.866636] usb 4-1: Product: STM32 STLink
[    2.871152] usb 4-1: Manufacturer: STMicroelectronics
[    2.877704] usb 4-1: SerialNumber: 'Ä000002
```
</details>

Lets compare HAL and Linux drivers step by step to understand where can be mistake.

### Configuration

Configuration for Linux https://andreas.welcomes-you.com/media/files/licheerv_linux_defconfig define several USB related values:

```conf
CONFIG_PHY_SUN4I_USB=y
CONFIG_USB=y
CONFIG_USB_EHCI_HCD=y
CONFIG_USB_EHCI_HCD_PLATFORM=y
CONFIG_USB_OHCI_HCD=y
CONFIG_USB_OHCI_HCD_PLATFORM=y
```

This config also affect what files will be compiled according linux/drivers/usb/Makefile

```makefile
obj-$(CONFIG_USB)			+= core/
obj-$(CONFIG_USB_EHCI_HCD)	+= host/
```

On the other side in RTT config rt-thread\bsp\allwinner\libraries\sunxi-hal\hal\kconfig.h defined
```c
#define CONFIG_USB_HOST 1
#define CONFIG_USB_EHCI 1
#define CONFIG_USB_EHCI_0 1
#define CONFIG_USB_EHCI_1 1
#define CONFIG_USB_OHCI 1
#define CONFIG_USB_OHCI_0 1
#define CONFIG_USB_OHCI_1 1
```

Comparing Linux and RTT is nothing extra difference in config and build. 

lets looks to Linux Device Tree Source files that describes the system's hardware layout:

https://github.com/smaeul/linux/blob/d1/all/arch/riscv/boot/dts/allwinner/sun20i-d1.dtsi

```yaml
		usbphy: phy@4100400 {
			compatible = "allwinner,sun20i-d1-usb-phy";
			reg = <0x4100400 0x100>,
			      <0x4101800 0x100>,
			      <0x4200800 0x100>;
			reg-names = "phy_ctrl",
				    "pmu0",
				    "pmu1";
			clocks = <&osc24M>,
				 <&osc24M>;
			clock-names = "usb0_phy",
				      "usb1_phy";
			resets = <&ccu RST_USB_PHY0>,
				 <&ccu RST_USB_PHY1>;
			reset-names = "usb0_reset",
				      "usb1_reset";
			#phy-cells = <1>;
			status = "disabled";
		};

		ehci1: usb@4200000 {
			compatible = "allwinner,sun20i-d1-ehci",
				     "generic-ehci";
			reg = <0x4200000 0x100>;
			clocks = <&ccu CLK_BUS_OHCI1>,
				 <&ccu CLK_BUS_EHCI1>,
				 <&ccu CLK_USB_OHCI1>;
			resets = <&ccu RST_BUS_OHCI1>,
				 <&ccu RST_BUS_EHCI1>;
			interrupts = <49 IRQ_TYPE_LEVEL_HIGH>;
			phys = <&usbphy 1>;
			phy-names = "usb";
			status = "disabled";
		};

		ohci1: usb@4200400 {
			compatible = "allwinner,sun20i-d1-ohci",
				     "generic-ohci";
			reg = <0x4200400 0x100>;
			clocks = <&ccu CLK_BUS_OHCI1>,
				 <&ccu CLK_USB_OHCI1>;
			resets = <&ccu RST_BUS_OHCI1>;
			interrupts = <50 IRQ_TYPE_LEVEL_HIGH>;
			phys = <&usbphy 1>;
			phy-names = "usb";
			status = "disabled";
		};
```

According this memory layout for USB will be:
```c
OTG_PBASE		0x04100000
phy otg        	0x04100400

EHCI0     		0x04101000
OHCI0          	0x04101400
phy hci0       	0x04101800

EHCI1     		0x04200000
OHCI1          	0x04200400
phy hci1       	0x04200800
```

And in RTT rt-thread\bsp\allwinner\libraries\sunxi-hal\hal\source\usb_melis\platform\sun20iw1\usb_sun20iw1.h

```c
#define SUNXI_USB_OTG_PBASE			0x04100000
#define SUNXI_USB_HCI0_PBASE		0x04101000
#define SUNXI_USB_HCI1_PBASE		0x04200000
```
Base registers seems the same.

### Look to clocks. 

In sun20i-d1.dtsi included:
```c
#include <dt-bindings/clock/sun20i-d1-ccu.h>
#include <dt-bindings/reset/sun20i-d1-ccu.h>
```
That can be found in linux/include/dt-bindings/clock/sun20i-d1-ccu.h

```c
#define CLK_USB_OHCI0		97
#define CLK_USB_OHCI1		98
#define CLK_BUS_OHCI0		99
#define CLK_BUS_OHCI1		100
#define CLK_BUS_EHCI0		101
#define CLK_BUS_EHCI1		102
#define CLK_BUS_OTG			103
```

And linux/include/dt-bindings/reset/sun20i-d1-ccu.h

```c
#define RST_USB_PHY0		40
#define RST_USB_PHY1		41
#define RST_BUS_OHCI0		42
#define RST_BUS_OHCI1		43
#define RST_BUS_EHCI0		44
#define RST_BUS_EHCI1		45
#define RST_BUS_OTG		    46
```

In RTT file rt-thread\bsp\allwinner\libraries\sunxi-hal\hal\source\ccmu\sunxi-ng\ccu-sun8iw20.h
```c
#define CLK_USB_OHCI0       100
#define CLK_USB_OHCI1       101
#define CLK_BUS_OHCI0       102
#define CLK_BUS_OHCI1       103
#define CLK_BUS_EHCI0       104
#define CLK_BUS_EHCI1       105
#define CLK_BUS_OTG         106
```

And rt-thread\bsp\allwinner\libraries\sunxi-hal\hal\source\ccmu\sunxi-ng\rst-sun8iw20.h
```c
#define RST_USB_PHY0        42
#define RST_USB_PHY1        43
#define RST_BUS_OHCI0       44
#define RST_BUS_OHCI1       45
#define RST_BUS_EHCI0       46
#define RST_BUS_EHCI1       47
#define RST_BUS_OTG         48
```

Strange it shifted to 3 for clock and 2 for reset. Display and UART clock inited seccusefull with htis shift so assum is specific of clock driver implementation.

### Lets look how clock initialization implemented

In Linux drivers/phy/allwinner/phy-sun4i-usb.c file sun4i_usb_phy_init() function:
```c
clk_prepare_enable(phy->clk);           // is osc24M
clk_prepare_enable(phy->clk2);          // is 0
reset_control_deassert(phy->reset);     // is RST_USB_PHY1 = 41

of_clk_get(dev->dev.of_node, clk);      //is CLK_BUS_EHCI1 = 102
reset_control_deassert(priv->rsts);     //is RST_BUS_EHCI1 = 45
```

In RTT rt-thread\bsp\allwinner\libraries\sunxi-hal\hal\source\usb_melis\platform\sun20iw1\usb_sun20iw1.c

```c
	ehci-1
		.usb_clk = CLK_BUS_EHCI1 = 105
		.usb_rst = RST_BUS_EHCI1 = 47
		.phy_clk = 0,
		.phy_rst = RST_USB_PHY1  = 43

	ohci-1
		.ohci_clk = CLK_USB_OHCI1,
		.usb_clk  = CLK_BUS_OHCI1,
		.usb_rst  = RST_BUS_OHCI1,
		.phy_clk  = 0,
		.phy_rst  = RST_USB_PHY1
```

File rt-thread\bsp\allwinner\libraries\sunxi-hal\hal\source\usb_melis\host\sunxi-hci.c 

function hci_clock_init()

```c
	bus_clk_id    = usb_clk  // CLK_BUS_EHCI1 = 105
	reset_bus_clk = usb_rst  // RST_BUS_EHCI1 = 47
	phy_clk_id    = phy_clk  // 0
	reset_phy_clk = phy_rst  // RST_USB_PHY1  = 43
	ohci_clk_id   = ohci_clk // CLK_USB_OHCI1 = 101
```

function open_clock()

```c
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
Overall seems quite similar.

### Lets look how PHY initialization implemented

I add to drivers/phy/allwinner/phy-sun4i-usb.c help wrapper to understand what writed to registers

```c
void writel_(u32 v, void __iomem *c)
{
	writel(v,c);
	printk(KERN_WARNING "phy writel %x<-%x\n",c,v);
}
```
But in Linux registers is ioremap so add some debug to sun4i_usb_phy_probe()
```c
	printk(KERN_WARNING "phy->pmu %s virt_addr: %x\n",name,phy->pmu);
	printk(KERN_WARNING "phy->pmu %s phy__addr: %x\n",name,platform_get_resource_byname(pdev,IORESOURCE_MEM,name)->start);

```

<details><summary>linux console output</summary>

```shell
[    0.247663] usbcore: registered new interface driver usbfs
[    0.253218] usbcore: registered new interface driver hub
[    0.258568] usbcore: registered new device driver usb
[    0.441543] ehci_hcd: USB 2.0 'Enhanced' Host Controller (EHCI) Driver
[    0.448051] ehci-platform: EHCI generic platform driver
[    0.453655] ohci_hcd: USB 1.1 'Open' Host Controller (OHCI) Driver
[    0.459875] ohci-platform: OHCI generic platform driver
[    0.630352] usbcore: registered new interface driver snd-usb-audio
[    0.868509] sun4i_usb_phy_probe
[    0.871856] data->base phy_ctrl virt_addr: 4125400
[    0.876701] data->base phy_ctrl phy__addr: 4100400
[    0.882447] phy->clk name: usb0_phy
[    0.904067] phy->reset name: usb0_reset
[    0.908125] phy->pmu pmu0 virt_addr: 412d800
[    0.912816] phy->pmu pmu0 phy__addr: 4101800
[    0.917532] phy->clk name: usb1_phy
[    0.921247] phy->reset name: usb1_reset
[    0.925289] phy->pmu pmu1 virt_addr: 4141800
[    0.929669] phy->pmu pmu1 phy__addr: 4200800
[    0.943058] sun4i_usb_phy_init
[    0.946225] phy writel 412d810<-0
[    0.949651] phy writel 4125410<-20
[    0.953172] sun4i_usb_phy_passby
[    0.956571] phy writel 412d800<-701
[    0.960166] phy->index == 0
[    0.963023] sun4i_usb_phy0_update_iscr
[    0.966922] phy writel 4125400<-40010000
[    0.970945] sun4i_usb_phy0_update_iscr
[    0.974798] phy writel 4125400<-40030000
[    0.978834] phy phy-4100400.phy.0: Changing dr_mode to 1
[    1.842129] phy phy-4100400.phy.0: External vbus detected, not enabling our own vbus
[    1.850045] ehci-platform 4101000.usb: EHCI Host Controller
[    1.855779] ehci-platform 4101000.usb: new USB bus registered, assigned bus number 1
[    1.863877] ehci-platform 4101000.usb: irq 37, io mem 0x04101000
[    1.887139] ehci-platform 4101000.usb: USB 2.0 started, EHCI 1.00
[    1.900455] usb usb1: New USB device found, idVendor=1d6b, idProduct=0002, bcdDevice= 5.17
[    1.915139] usb usb1: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    1.922644] usb usb1: Product: EHCI Host Controller
[    1.927786] usb usb1: Manufacturer: Linux 5.17.0-rc2-379425-g06b026a8b714-dirty ehci_hcd
[    1.936129] usb usb1: SerialNumber: 4101000.usb
[    1.942243] hub 1-0:1.0: USB hub found
[    1.946548] hub 1-0:1.0: 1 port detected
[    1.956111] sun4i_usb_phy_init
[    1.959186] phy writel 4141810<-0
[    1.963807] sun4i_usb_phy_passby
[    1.967053] phy writel 4141800<-701
[    1.970649] ehci-platform 4200000.usb: EHCI Host Controller
[    1.977091] ehci-platform 4200000.usb: new USB bus registered, assigned bus number 2
[    1.985107] ehci-platform 4200000.usb: irq 39, io mem 0x04200000
[    2.003583] ehci-platform 4200000.usb: USB 2.0 started, EHCI 1.00
[    2.010053] usb usb2: New USB device found, idVendor=1d6b, idProduct=0002, bcdDevice= 5.17
[    2.018383] usb usb2: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    2.025714] usb usb2: Product: EHCI Host Controller
[    2.030650] usb usb2: Manufacturer: Linux 5.17.0-rc2-379425-g06b026a8b714-dirty ehci_hcd
[    2.038778] usb usb2: SerialNumber: 4200000.usb
[    2.044319] hub 2-0:1.0: USB hub found
[    2.048243] hub 2-0:1.0: 1 port detected
[    2.053653] ohci-platform 4101400.usb: Generic Platform OHCI controller
[    2.060407] ohci-platform 4101400.usb: new USB bus registered, assigned bus number 3
[    2.068376] ohci-platform 4101400.usb: irq 38, io mem 0x04101400
[    2.135880] usb usb3: New USB device found, idVendor=1d6b, idProduct=0001, bcdDevice= 5.17
[    2.144213] usb usb3: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    2.151477] usb usb3: Product: Generic Platform OHCI controller
[    2.157449] usb usb3: Manufacturer: Linux 5.17.0-rc2-379425-g06b026a8b714-dirty ohci_hcd
[    2.165581] usb usb3: SerialNumber: 4101400.usb
[    2.171081] hub 3-0:1.0: USB hub found
[    2.175000] hub 3-0:1.0: 1 port detected
[    2.180667] ohci-platform 4200400.usb: Generic Platform OHCI controller
[    2.187649] ohci-platform 4200400.usb: new USB bus registered, assigned bus number 4
[    2.195661] ohci-platform 4200400.usb: irq 40, io mem 0x04200400
[    2.263855] usb usb4: New USB device found, idVendor=1d6b, idProduct=0001, bcdDevice= 5.17
[    2.272185] usb usb4: New USB device strings: Mfr=3, Product=2, SerialNumber=1
[    2.279452] usb usb4: Product: Generic Platform OHCI controller
[    2.285413] usb usb4: Manufacturer: Linux 5.17.0-rc2-379425-g06b026a8b714-dirty ohci_hcd
[    2.293539] usb usb4: SerialNumber: 4200400.usb
[    2.299038] hub 4-0:1.0: USB hub found
[    2.302957] hub 4-0:1.0: 1 port detected
[    2.608573] usb 4-1: new full-speed USB device number 2 using ohci-platform
[    2.850655] usb 4-1: New USB device found, idVendor=0483, idProduct=3748, bcdDevice= 1.00
[    2.859043] usb 4-1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[    2.866636] usb 4-1: Product: STM32 STLink
[    2.871152] usb 4-1: Manufacturer: STMicroelectronics
[    2.877704] usb 4-1: SerialNumber: 'Ä000002
```
</details>

let's highlight in output the most important:
```sh
sun4i_usb_phy_probe
data->base phy_ctrl virt_addr: 4125 400
data->base phy_ctrl phy__addr: 4100 400		//phy otg

phy->pmu pmu0 virt_addr: 412d 800
phy->pmu pmu0 phy__addr: 4101 800			//phy hci0
							  
phy->pmu pmu1 virt_addr: 4141 800
phy->pmu pmu1 phy__addr: 4200 800			//phy hci1 

sun4i_usb_phy_init			phy writel 4101 810<-0			// cntrl clear
						phy writel 4100 410<-20				// | 5 & 3	SIDDQ
sun4i_usb_phy_passby		phy writel 4101 800<-701		// | 10 9 8 0
phy->index == 0                             
sun4i_usb_phy0_update_iscr	phy writel 4100 400<-40010000	// & 0 | 16 
sun4i_usb_phy0_update_iscr	phy writel 4100 400<-40030000	// & 0 | 17
										    
										    
sun4i_usb_phy_init			phy writel 4200 810<-0			// cntrl clear
sun4i_usb_phy_passby		phy writel 4200 800<-701		// | 10 9 8 0
```

RTT add in functions print's like this:
```c
	rt_kprintf("phy write: %x<-%x\n\r",(sunxi_hci->usb_vbase + SUNXI_HCI_PHY_CTRL),reg_value);
```

RTT Console output:
```shell
[ehci-usb1] insmod host driver!
USBC_Clean_SIDDP
phy write: 4200810<-0
usb_passby
phy write: 4200800<-701
```

Overall seems quite similar.
Lets asume clock and phy init is ok. So maybe problem in EHCI/OHCI

Code for EHCI in HAL for me is quite complicated, I tryed to find simpl opensource implementation. RTT support several packages for USB stack Cherry USB and Tiny USB


## Cherry USB variant with RTT HAL
```C
    usbh_initialize(1, SUNXI_USB_HCI1_PBASE);
```
with chery init interapt not call




## Tiny USB variant with RTT or Melis HAL
```C
    init_tinyusb();
```



RTT Console output
```shell
[ehci-usb1] insmod host driver!
phy write: 4200810<-0
phy write: 4200800<-701
[sunxi-ehci1]: sunxi_set_vbus cnt.
plic_irq_toggle irq:76,enable:1
[usbh core]: adding sub dev  (config #1, interface 0)
[ohci-usb1] insmod host driver!
phy write: 4200810<-0
phy write: 4200800<-701
[sunxi-ohci1]: sunxi_set_vbus cnt.
[usbh core]: adding sub dev  (config #1, interface 0)USBH init on controller 0
sizeof(usbh_device_t) = 312
sizeof(hcd_event_t) = 24
sizeof(_ctrl_xfer) = 40
sizeof(tuh_xfer_t) = 48
sizeof(tu_fifo_t) = 32
sizeof(tu_edpt_stream_t) = 144
osal: rt_mq_init result: 0
HID init
HUB init
tiny ehci_init
EHCI HCIVERSION:0x0100
EHCI HCSPARAMS:0x001101
EHCI HCCPARAMS:0xa026
EHCI HCI_interface:     1793
EHCI HCI_Control_3:     0
EHCI PHY_Control (810): 0
EHCI PHY_Status  (824): 0
EHCI HCI_SIE_Port:      3
tiny ohcd_init
plic_irq_toggle irq:49,enable:0
plic_irq_toggle irq:49,enable:1
plic_irq_toggle irq:50,enable:0
plic_irq_toggle irq:50,enable:1
tuh_task_ext: tusb_inited
```


```shell
[ehci-usb1] insmod host driver!
[sunxi-ehci1]: sunxi_set_vbus cnt.
[usbh core]: adding sub dev  (config #1, interface 0)
[ohci-usb1] insmod host driver!
[sunxi-ohci1]: sunxi_set_vbus cnt.
[usbh core]: adding sub dev  (config #1, interface 0)USBH init on controller 0
sizeof(usbh_device_t) = 312
sizeof(hcd_event_t) = 24
sizeof(_ctrl_xfer) = 40
sizeof(tuh_xfer_t) = 48
sizeof(tu_fifo_t) = 32
sizeof(tu_edpt_stream_t) = 144
osal: rt_mq_init result: 0
HID init
HUB init
tiny ehci_init
EHCI HCIVERSION:0x0100
EHCI HCSPARAMS:0x001101
EHCI HCCPARAMS:0xa026
EHCI HCI_interface:     1793
EHCI HCI_Control_3:     0
EHCI PHY_Control (810): 0
EHCI PHY_Status  (824): 0
EHCI HCI_SIE_Port:      3
plic_irq_toggle irq:49,enable:0
plic_irq_toggle irq:49,enable:1
tuh_task_ext: tusb_inited
msh />
msh />
msh />ehci hcd_int_handler: 4
regs->portsc = 1002
  Current Connect Status: 0
  Connect Status Change : 1
  Port Enabled          : 0
  Port Enabled Change   : 0
  Port Reset            : 0
  Port Power            : 1
  Port Owner            : 0
  Over current active   : 0
  Over current change   : 0
  Port suspend          : 0
  Port line_status      : 0
  Wake on connect enable: 0
hcd_event_device_remove
osal: rt_mq_send result: 0
osal: rt_mq_recv result: 24
tuh_task_ext: event.event_id 1
tuh_task_ext:  USBH DEVICE REMOVED 
[0:0:0] USBH DEVICE REMOVED
tuh_task_ext: tusb_inited
ehci hcd_int_handler: 4
regs->portsc = 1803
  Current Connect Status: 1
  Connect Status Change : 1
  Port Enabled          : 0
  Port Enabled Change   : 0
  Port Reset            : 0
  Port Power            : 1
  Port Owner            : 0
  Over current active   : 0
  Over current change   : 0
  Port suspend          : 0
  Port line_status      : 2
  Wake on connect enable: 0
hcd_event_device_attach
osal: rt_mq_send result: 0
osal: rt_mq_recv result: 24
tuh_task_ext: event.event_id 0
tuh_task_ext: USBH DEVICE ATTACH
[0:] USBH DEVICE ATTACH
Full Speed
regs->portsc = 1801
  Current Connect Status: 1
  Connect Status Change : 0
  Port Enabled          : 0
  Port Enabled Change   : 0
  Port Reset            : 0
  Port Power            : 1
  Port Owner            : 0
  Over current active   : 0
  Over current change   : 0
  Port suspend          : 0
  Port line_status      : 2
  Wake on connect enable: 0
process_enumeration start
process_enumeration state:3
[0:0] Open EP0 with Size = 8
Get 8 byte of Device Descriptor
[0:0] Get Descriptor: 80 06 00 01 00 00 08 00
tehci hcd_int_handler: 3
osal: rt_mq_send result: 0
Unhandled Exception 13:Load Page Fault
scause:0x0x000000000000000d,stval:0x0x0000000000000006,sepc:0x0x00000000404cb642
--------------Dump Registers-----------------
Function Registers:
        ra(x1) = 0x0x00000000404cb62e   user_sp = 0x0x000000004053a4f8
        gp(x3) = 0x0x000000004052a960   tp(x4) = 0x0x00000000deadbeef
Temporary Registers:
        t0(x5) = 0x0x8000000000000009   t1(x6) = 0x0x0000000000000008
        t2(x7) = 0x0x00000000405ea958
        t3(x28) = 0x0x00000000deadbeef  t4(x29) = 0x0x00000000deadbeef
        t5(x30) = 0x0x00000000deadbeef  t6(x31) = 0x0x00000000deadbeef
Saved Registers:
        s0/fp(x8) = 0x0x000000004053a558        s1(x9) = 0x0x00000000deadbeef
        s2(x18) = 0x0x00000000deadbeef  s3(x19) = 0x0x00000000deadbeef
        s4(x20) = 0x0x00000000deadbeef  s5(x21) = 0x0x00000000deadbeef
        s6(x22) = 0x0x00000000deadbeef  s7(x23) = 0x0x00000000deadbeef
        s8(x24) = 0x0x00000000deadbeef  s9(x25) = 0x0x00000000deadbeef
        s10(x26) = 0x0x00000000deadbeef s11(x27) = 0x0x00000000deadbeef
Function Arguments Registers:
        a0(x10) = 0x0x0000000000000001  a1(x11) = 0x0x0000000000000010
        a2(x12) = 0x0x0000000000000010  a3(x13) = 0x0x0000000000000080
        a4(x14) = 0x0x0000000000000001  a5(x15) = 0x0x0000000000000000
        a6(x16) = 0x0xffffffffffffffff  a7(x17) = 0x0x0000000000000002
sstatus = 0x0x0000000200040100
        Supervisor Interrupt Disabled
        Last Time Supervisor Interrupt Disabled
        Last Privilege is Supervisor Mode
        Permit to Access User Page
        Not Permit to Read Executable-only Page
satp = 0x0x800000000004056b
        Current Page Table(Physical) = 0x0x000000004056b000
        Current ASID = 0x0x0000000000000000
        Mode = Page-based 39-bit Virtual Addressing Mode
-----------------Dump OK---------------------
--------------Thread list--------------
current thread: tusb
--------------Backtrace--------------
$TOOLCHAIN_INSTALL_DIR/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu/bin/riscv64-unknown-linux-musl-addr2line -e rt-thread/bsp/allwinner/d1s_d1h/rtthread.elf -a -f 0x00000000404cb63e

``` 






### TODO USB:

- linux ehci reg
- DMA, MMU ?









### Some


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