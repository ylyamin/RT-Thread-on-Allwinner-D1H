# Boot process

https://linux-sunxi.org/Allwinner_Nezha
https://fedoraproject.org/wiki/Architectures/RISC-V/Allwinner
https://andreas.welcomes-you.com/boot-sw-debian-risc-v-lichee-rv/

https://github.com/RT-Thread/rt-thread/blob/master/bsp/allwinner/d1s/README-M7.md
https://club.rt-thread.org/ask/article/389ac36250b57737.html

https://github.com/hno/Allwinner-Info/blob/master/BROM/ffff4000.s

## BROM

![D1H_brom_overview.png](Pics/D1H_brom_overview.png) 
![D1H_fel_select.png](Pics/D1H_fel_select.png) 
![D1H_brom_memmory.png](Pics/D1H_brom_memmory.png) 
![D1H_dram_space.png](Pics/D1H_dram_space.png) 




memmory mapping

sd/xfel

![boot_process_1.png](Pics/boot_process_1.png) 

1. Start brom

eGON.BT0

The D1 boot ROM can read the boot0 SPL from two different locations:

starting at sector 16
starting at sector 256

256 = bs=8192 byte seek=16



2. Sector 16
3. Load SPL to memory

## SPL
![boot_process_2.png](Pics/boot_process_2.png) 

4. Run SPL
5. Sector 32800

bs=512 seek=32800    

6. Load TOC

## SBI
![boot_process_3.png](Pics/boot_process_3.png) 

7. Start SBI
8. Load DTB
9. Start U-BOOT
10. Find boot partition 
11. Load image

## Linux kernel
![boot_process_4.png](Pics/boot_process_4.png) 

12. starts kernel
13. load root fs

## RTT kernel
![boot_process_5.png](Pics/boot_process_5.png) 

1. Run SPL
2. Sector 32800
3. Load TOC
4. Start SBI
5. Load DTB
6. Start RTT
7. load bin

TODO:

1. diagramm overview
2. D1 boot0 sd/xfel
3. spl clock dram
4. u-boot
5. sbi (dts)
6. linux
7. xfel
7. gdb, jtag   







 



