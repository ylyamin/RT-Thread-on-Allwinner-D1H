# Post in community

## R-01 RISC-V Baremetal with RT-Thread

Hi all,

I successfully build and ran RT-Thread (a small embedded OS) on the Devterm R-01.<br>
It may also work in uConsole R-01, but I didn't have uConsole. I would appreciate if someone could test it.<br>

Honestly, Allwinner D1H is really pain - documentation very limited regarding peripherals, there are no examples and no standalone libraries.<br>
However if you enjoy brainteaser and some low-level stuff, you’re welcome.<br>
Intention was to make RT-Thread portable terminal with working display and keyboard.

I manage to:
- Create a working LCD MIPI DSI Display driver with frame buffer.
- Make some progress on a USB keyboard driver.
- Boot from SD card and communicate via UART
- Debugging with GDB

Please note: The battery charging/discharging controller is not yet configured in the driver, so I’m unsure how it affects the battery. I suggest not using the battery with RT-Thread and powering it via USB instead.

Should say it's not truly low-level bare-metal from ground as RT-Thread operates with threads and events and the HAL level is quite complex. But it is easier to manage compared to working within the Linux kernel—at least for me, as I'm not an expert in Linux. RT-Thread has great potential, as it supports many packages for GUI, audio, Ethernet, and more.

The code is available at: https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H

Ready images for SD card:
- DevTerm R01: [https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/image/sd_image_devterm.img](image/sd_image_devterm.img)
- uConsole R01: [https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/image/sd_image_uconsole.img](image/sd_image_uconsole.img) (not tested yet)
- Sipeed Lichee RV: [https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/image/sd_image_lichee.img](image/sd_image_lichee.img)

I’m writing a series of articles covering the knowledge and experience I’ve gained:

- [D1-1. Introduction](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_1_introduction.md) - Motivation and intention.
- [D1-2. Boot process](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_2_boot_process.md) - Existing Allwinner D1H bootloaders best practice in example with booting Linux and RT-Thread, also JTAG and GDB.
- [D1-3. RT-Thread overview and Build](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_3_rtt_overview_and_build.md) - Overview of RT-Thread structure and libraries. Compile RT-Thread for Allwinner D1H platform.
- [D1-4. Makefile and Hardware changes](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_4_make_and_hw.md) - My repository structure, Makefile and Hardware changes was needed to be done.
- [D1-5. LCD Display driver](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_5_lcd_driver.md) - Display driver for [DevTerm R-01](https://www.clockworkpi.com/home-devterm), [uConsole R-01](https://www.clockworkpi.com/uconsole) and [Sipeed Lichee RV](https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html).
- [D1-6. USB Keyboard](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_6_usb_keyboard.md) - USB Keyboard driver for ClockworkPi DevTerm, still not working.

I dare to ask for help from the community, please:

1. If someone could test whether the LCD display works in uConsole with the command "lcd_draw_point 100 100," I would appreciate it.
2. I’m really stuck with the USB driver. It can detect device connections, but communication has not started. Perhaps someone would enjoy diving deep into the USB driver to solve this riddle.

I am ready for issue reports and pull requests.

Picture of LCD Display test in RT-Thread running on Devterm R-01:
![devterm_lcd_mipi_work.jpg](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/Pics/devterm_lcd_mipi_work.jpg)


## RT-Thread on Allwinner D1H. LCD work, USB in progress.

Hi all,

I successfully build and ran RT-Thread on the Allwinner D1H SoC.<br>
Experimenting on [ClockworkPi DevTerm R-01](https://www.clockworkpi.com/home-devterm) device and [Sipeed Lichee RV](https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html) board.<br>

Honestly, Allwinner D1H is really pain - documentation very limited regarding peripherals, there are no examples and no standalone libraries.<br>
However if you enjoy brainteaser and some low-level stuff, you’re welcome.<br>
Intention was to make RT-Thread portable terminal with working display and keyboard.

I manage to:
- Create a working LCD MIPI DSI Display driver with frame buffer.
- Create a working LCD RGB Display driver with frame buffer.
- Make some progress on a USB keyboard driver.
- Boot from SD card and communicate via UART
- Debugging with GDB

The code is available at: https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H

Ready images for SD card:
- DevTerm R01: [https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/image/sd_image_devterm.img](image/sd_image_devterm.img)
- uConsole R01: [https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/image/sd_image_uconsole.img](image/sd_image_uconsole.img) (not tested yet)
- Sipeed Lichee RV: [https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/image/sd_image_lichee.img](image/sd_image_lichee.img)

I’m writing a series of articles covering the knowledge and experience I’ve gained:

- [D1-1. Introduction](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_1_introduction.md) - Motivation and intention.
- [D1-2. Boot process](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_2_boot_process.md) - Existing Allwinner D1H bootloaders best practice in example with booting Linux and RT-Thread, also JTAG and GDB.
- [D1-3. RT-Thread overview and Build](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_3_rtt_overview_and_build.md) - Overview of RT-Thread structure and libraries. Compile RT-Thread for Allwinner D1H platform.
- [D1-4. Makefile and Hardware changes](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_4_make_and_hw.md) - My repository structure, Makefile and Hardware changes was needed to be done.
- [D1-5. LCD Display driver](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_5_lcd_driver.md) - Display driver for [DevTerm R-01](https://www.clockworkpi.com/home-devterm), [uConsole R-01](https://www.clockworkpi.com/uconsole) and [Sipeed Lichee RV](https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html).
- [D1-6. USB Keyboard](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_6_usb_keyboard.md) - USB Keyboard driver for ClockworkPi DevTerm, still not working.

I dare to ask for help from the community, please: <br>
I’m really stuck with the USB driver. It can detect device connections, but communication has not started.<br>
Perhaps someone would enjoy diving deep into the USB driver to solve this riddle.

I am ready for issue reports and pull requests.

Picture of LCD Display test in RT-Thread running on Devterm R-01:
![devterm_lcd_mipi_work.jpg](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/Pics/devterm_lcd_mipi_work.jpg)