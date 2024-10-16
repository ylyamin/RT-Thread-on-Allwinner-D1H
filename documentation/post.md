# R-01 RISC-V Baremetal with RT-Thread

Hi all,

I successfully run RT-Thread (small embedded OS) in Devterm R-01.
And manage to working Display, and some progress with USB keyboard.

Maybe is work in uConsole too, I didn't have uConsole, will be apritiated if some om can test 


Is not truly low-level coding from ground as RT-Thread work with threads and events and HAL level quite complex.
But if you want to play with RISC-V Allwinner D1H in such low-level and not expert in Linux kernel like me is could be case.
RT-Thread have big potential as it includes many packages for GUI, audio, ethernet, and more.

The code is available at: https://github.com/ylyamin/RT-Thread-on-Allwinner-D1H <br>

I write series of articles about this (now stored in the same GitHub repo):

- [D1-1. Introduction](D1_1_introduction.md) - Motivation and intention.
- [D1-2. Boot process](D1_2_boot_process.md) - Existing Allwinner D1H bootloaders best practice in example with booting Linux and RT-Thread, also JTAG and GDB.
- [D1-3. RT-Thread overview and Build](D1_3_rtt_overview_and_build.md) - Overview of RT-Thread structure and libraries. Compile RT-Thread for Allwinner D1H platform.
- [D1-4. Makefile and Hardware changes](D1_4_make_and_hw.md) - My repository structure, Makefile and Hardware changes was needed to be done.
- [D1-5. LCD Display driver](D1_5_lcd_driver.md) - Display driver for [ClockworkPi DevTerm R-01](https://www.clockworkpi.com/home-devterm), [uConsole R-01](https://www.clockworkpi.com/uconsole) and [Sipeed Lichee RV](https://wiki.sipeed.com/hardware/en/lichee/RV/Dock.html).
- [D1-6. USB Keyboard](D1_6_usb_keyboard.md) - USB Keyboard driver for ClockworkPi DevTerm, still not work.

This is execution of command lcd_draw_point 100 100
![devterm_lcd_mipi_work.jpg](Pics/devterm_lcd_mipi_work.jpg)

Can't know if Display works as I didn'r have uConsole, previos Devterm implementation work perfectly.<br>
This driver could be tested by command:
```sh
lcd_draw_point 100 100
```

- Battery work not guaranteed

Maybe someone can help me with USB driver, please. I am ready for pull requests.
