#Set gdb environment
set confirm off
set height  0
target remote localhost:1025

#Memory layout
#set $spl_addr = 0x0000000000020000
set $opensbi_addr = 0x40000000
set $dtb_addr = 0x40200000
set $kernel_addr = 0x40400000
set $dyninfo_addr = 0x43000000

#Load kernel
#restore bootloaders/sun20i_d1_spl/nboot/boot0_sdcard_sun20iw1p1.bin binary $spl_addr

restore bootloaders/opensbi/build/platform/generic/firmware/fw_dynamic.bin binary $opensbi_addr
#restore bootloaders/opensbi/build/platform/generic/firmware/fw_jump.bin binary $opensbi_addr
restore build/sun20i-d1-lichee-rv-dock.dtb binary $dtb_addr
restore rt-thread/bsp/allwinner/d1s_d1h/rtthread.bin binary $kernel_addr

file bootloaders/opensbi/build/platform/generic/firmware/fw_dynamic.elf
#file bootloaders/opensbi/build/platform/generic/firmware/fw_jump.elf
file rt-thread/bsp/allwinner/d1s_d1h/rtthread.elf

#file bootloaders/sun20i_d1_spl/nboot/boot0_sdcard.elf
#j *0x00000000000203c8

#Set opensbi dynamic info param
#- Magic
#- Version
#- Next booting stage address
#- Next booting stage mode 1 - S, 3 - M
#- Options for OpenSBI library 
#- boot_hart

set $a0 = 0
set $a1 = $dtb_addr
set *(unsigned long *)($dyninfo_addr)      = 0x4942534f
set *(unsigned long *)($dyninfo_addr + 8)  = 2
set *(unsigned long *)($dyninfo_addr + 16) = $kernel_addr
set *(unsigned long *)($dyninfo_addr + 24) = 1
set *(unsigned long *)($dyninfo_addr + 32) = 0
set *(unsigned long *)($dyninfo_addr + 40) = 0
set $a2 = $dyninfo_addr

b rt_components_board_init
j *$opensbi_addr