# Post in community

## forum.clockworkpi.com

R-01 RISC-V Baremetal with RT-Thread. LCD work, USB in progress.

Hi all,

I successfully build and ran RT-Thread (a small embedded OS) on the Devterm R-01.<br>
It may also work in uConsole R-01, but I didn't have uConsole. I would appreciate if someone could test it.<br>

Honestly, R-01 RISC-V core Allwinner D1H is really pain - documentation very limited regarding peripherals, there are no examples and no standalone libraries.<br>
However if you enjoy brainteaser and some low-level stuff, you’re welcome.<br>
Intention was to make RT-Thread portable terminal with working display and keyboard.

I manage to:
- Create a working LCD MIPI DSI Display driver with frame buffer.
- Make some progress on a USB keyboard driver.
- Boot from SD card and communicate via UART
- Debugging with GDB

**Please note:** The battery charging/discharging controller is not yet configured in the driver, so I’m unsure how it affects the battery. I suggest not using the battery with RT-Thread and powering it via USB instead.

Should say: it's not truly low-level bare-metal from ground as RT-Thread operates with threads and events and the HAL level is quite complex. But it is easier to manage compared to working within the Linux kernel - at least for me, as I'm not an expert in Linux. RT-Thread has great potential, as it supports many packages for GUI, audio, Ethernet, and more.

The code is available at: https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H

Ready images for SD card:
- DevTerm R01: [sd_image_devterm.img](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/releases/download/v5.0.2.2/sd_image_devterm.img)
- uConsole R01: [sd_image_uconsole.img](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/image/sd_image_uconsole.img) (not tested yet)
- Sipeed Lichee RV: [sd_image_lichee.img](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/releases/download/v5.0.2.2/sd_image_lichee.img)

I’m writing a series of articles covering the knowledge and experience I’ve gained:

- [D1-1. Introduction](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_1_introduction.md) - Motivation and intention.
- [D1-2. Boot process](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_2_boot_process.md) - Existing Allwinner D1H bootloaders best practice in example with booting Linux and RT-Thread, also JTAG and GDB.
- [D1-3. RT-Thread overview and Build](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_3_rtt_overview_and_build.md) - Overview of RT-Thread structure and libraries. Compile RT-Thread for Allwinner D1H platform.
- [D1-4. Makefile and Hardware changes](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_4_make_and_hw.md) - My repository structure, Makefile and Hardware changes was needed to be done.
- [D1-5. LCD Display driver](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_5_lcd_driver.md) - Display driver for [DevTerm R-01](https://www.clockworkpi.com/home-devterm), [uConsole R-01](https://www.clockworkpi.com/uconsole) and [Sipeed Lichee RV](https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html).
- [D1-6. USB Keyboard](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_6_usb_keyboard.md) - USB Keyboard driver for ClockworkPi DevTerm, still not working.

*Finally, I dare to ask for help from the community, please:*

1. If someone could test whether the LCD display works in uConsole with the command "lcd_draw_point 100 100," I would appreciate it.
2. I’m really stuck with the USB driver. It can detect device connections, but communication has not started. Perhaps someone would enjoy diving deep into the USB driver to solve this riddle. Ready for issue reports and pull requests.

Picture of LCD Display test in RT-Thread running on Devterm R-01:
![devterm_lcd_mipi_work.jpg](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/Pics/devterm_lcd_mipi_work.jpg)


## club.rt-thread.io, club.rt-thread.org 

RT-Thread on Allwinner D1H. LCD work, USB in progress.

This is an experimentation with the RT-Thread Operating System with the Allwinner D1H RISC-V SOC and a Collection of Bootloaders and Developer environment for programming and debugging. Executed at hardware such as ClockworkPi DevTerm R-01 and Sipeed Lich

Hi all,

I successfully build and ran RT-Thread on the RISC-V Allwinner D1H SoC.
Experimenting on [ClockworkPi DevTerm R-01](https://www.clockworkpi.com/home-devterm) device and [Sipeed Lichee RV](https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html) board

Allwinner D1H documentation very limited regarding peripherals. However if you enjoy brainteaser and some low-level stuff, you’re welcome.
Intention was to make RT-Thread portable terminal with working display and keyboard.

I manage to:
- Create a working LCD MIPI DSI Display driver with frame buffer.
- Create a working LCD RGB Display driver with frame buffer.
- Make some progress on a USB keyboard driver.
- Boot from SD card and communicate via UART
- Debugging with GDB

The code is available at: https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H

Ready images for SD card:
- DevTerm R01: [sd_image_devterm.img](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/releases/download/v5.0.2.2/sd_image_devterm.img)
- uConsole R01: [sd_image_uconsole.img](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/image/sd_image_uconsole.img)
- Sipeed Lichee RV: [sd_image_lichee.img](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/releases/download/v5.0.2.2/sd_image_lichee.img)

Please note: The DevTerm/uConsole battery charging/discharging controller is not yet configured in the driver, so I’m unsure how it affects the battery. I suggest not using the battery with RT-Thread and powering it via USB instead.

I’m writing a series of articles covering the knowledge and experience I’ve gained:
- [D1-1. Introduction](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_1_introduction.md) - Motivation and intention.
- [D1-2. Boot process](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_2_boot_process.md) - Existing Allwinner D1H bootloaders best practice in example with booting Linux and RT-Thread, also JTAG and GDB.
- [D1-3. RT-Thread overview and Build](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_3_rtt_overview_and_build.md) - Overview of RT-Thread structure and libraries. Compile RT-Thread for Allwinner D1H platform.
- [D1-4. Makefile and Hardware changes](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_4_make_and_hw.md) - My repository structure, Makefile and Hardware changes was needed to be done.
- [D1-5. LCD Display driver](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_5_lcd_driver.md) - Display driver for [DevTerm R-01](https://www.clockworkpi.com/home-devterm), [uConsole R-01](https://www.clockworkpi.com/uconsole) and [Sipeed Lichee RV](https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html).
- [D1-6. USB Keyboard](https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_6_usb_keyboard.md) - USB Keyboard driver for ClockworkPi DevTerm, still not working.

Finally, I dare to ask for help from the community, please:
I’m really stuck with the USB driver. It can detect device connections, but communication has not started.
Perhaps someone would enjoy diving deep into the USB driver to solve this riddle. Ready for issue reports and pull requests.

Picture of LCD Display test in RT-Thread running on Devterm R-01:

![devterm_lcd_mipi_work.jpg](https://oss-club.rt-thread.org/uploads/20241017/a4b71b81adb31770f280599ce3d8d7ff.jpg.webp)

## whycan.com: 

RT-Thread on Allwinner D1H. LCD work, USB in progress.

Hi all,

I successfully build and ran RT-Thread (a small embedded OS) on the RISC-V Allwinner D1H SoC.
Experimenting on [url=https://www.clockworkpi.com/home-devterm]ClockworkPi DevTerm R-01[/url] device and [url=https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html]Sipeed Lichee RV[/url] board.

Intention was to make RT-Thread portable terminal with a working display and keyboard.

I manage to:
[list]
[*]Create a working LCD MIPI DSI Display driver with frame buffer.[/*]
[*]Create a working LCD RGB Display driver with frame buffer.[/*]
[*]Make some progress on a USB keyboard driver.[/*]
[*]Boot from SD card and communicate via UART[/*]
[*]Debugging with GDB[/*]
[/list]

The code is available at: [url]https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H[/url]

If you enjoy brainteaser and some low-level stuff, you’re welcome.
Should say: it's not truly low-level bare-metal from ground as RT-Thread operates with threads and events and the HAL level is quite complex. But it is easier to manage compared to working within the Linux kernel - at least for me, as I'm not an expert in Linux. RT-Thread has great potential, as it supports many packages for GUI, audio, Ethernet, and more.

Ready images for SD card:
[list]
[*]DevTerm R01: [url=https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/releases/download/v5.0.2.2/sd_image_devterm.img]sd_image_devterm.img[/url][/*]
[*]uConsole R01: [https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/image/sd_image_uconsole.img] sd_image_uconsole.img[/url] (not tested yet)[/*]
[*]Sipeed Lichee RV: [url=https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/releases/download/v5.0.2.2/sd_image_lichee.img]sd_image_lichee.img[/url][/*]
[/list]
[b]Please note:[/b] The DevTerm/uConsole battery charging/discharging controller is not yet configured in the driver, so I’m unsure how it affects the battery. I suggest not using the battery with RT-Thread and powering it via USB instead.

I’m writing a series of articles covering the knowledge and experience I’ve gained:
[list]
[*] [url=https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_1_introduction.md]D1-1. Introduction[/url] - Motivation and intention.[/*]
[*] [url=https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_2_boot_process.md]D1-2. Boot process[/url] - Existing Allwinner D1H bootloaders best practice in example with booting Linux and RT-Thread, also JTAG and GDB.[/*]
[*] [url=https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_3_rtt_overview_and_build.md]D1-3. RT-Thread overview and Build[/url] - Overview of RT-Thread structure and libraries. Compile RT-Thread for Allwinner D1H platform.[/*]
[*] [url=https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_4_make_and_hw.md]D1-4. Makefile and Hardware changes[/url] - My repository structure, Makefile and Hardware changes was needed to be done.[/*]
[*] [url=https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_5_lcd_driver.md]D1-5. LCD Display driver[/url] - Display driver for [url=https://www.clockworkpi.com/home-devterm]DevTerm R-01[/url], [url=https://www.clockworkpi.com/uconsole]uConsole R-01[/url] and [url=https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html]Sipeed Lichee RV[/url].[/*]
[*] [url=https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/D1_6_usb_keyboard.md]D1-6. USB Keyboard[/url] - USB Keyboard driver for ClockworkPi DevTerm, still not working.[/*]
[/list]

[u]Finally, I dare to ask for help from the community, please:[/u]

I’m really stuck with the USB driver. It can detect device connections, but communication has not started.<br>
Perhaps someone would enjoy diving deep into the USB driver to solve this riddle. Ready for issue reports and pull requests.

Picture of LCD Display test in RT-Thread running on Devterm R-01:
[img=Devterm]https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H/blob/master/documentation/Pics/devterm_lcd_mipi_work.jpg[/img]

# Discussions:
- https://forum.clockworkpi.com/t/r-01-risc-v-baremetal-with-rt-thread-lcd-work-usb-in-progress/14683
- https://club.rt-thread.org/ask/article/b66f7db40179a81b.html
- https://club.rt-thread.io/ask/article/18.html
- https://whycan.com/t_11510.html