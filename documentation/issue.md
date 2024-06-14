https://github.com/RT-Thread/rt-thread/issues/9063

### Not compiled for Allwinner D1H.

I like this project but can't build for Allwinner D1H on Sipeed Lichee RV board
I use compiler https://github.com/RT-Thread/toolchains-ci/releases/download/v1.7/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu_latest.tar.bz2

I was trying:

### On [Master](https://github.com/RT-Thread/rt-thread/commit/2866da37a02dae72200e02ad87480718002760a5) or [v5.1.0](https://github.com/RT-Thread/rt-thread/releases/tag/v5.1.0):  

1) bsp/allwinner/d1/ - not compiled looks outdated
```
board/board.c:45:5: error: unknown type name 'rt_mmu_info'
   45 |     rt_mmu_info mmu_info;
      |     ^~~~~~~~~~~
```
2) bsp/allwinner/d1s/ - not compiled
```
/mnt/hgfs/allwinner_d1/rt-thread/components/dfs/dfs_v2/filesystems/devfs/devfs.c:68:30: error: 'struct rt_device' has no member named 'ops'; did you mean 'fops'?
   68 |             else if (device->ops && file->vnode->ref_count == 1)
      |                              ^~~
      |                              fops
```  
### On [v5.0.2](https://github.com/RT-Thread/rt-thread/releases/tag/v5.0.2):  

3) bsp/allwinner/d1/ - not compiled looks outdated
```
board/board.c:45:5: error: unknown type name 'rt_mmu_info'
   45 |     rt_mmu_info mmu_info;
      |     ^~~~~~~~~~~
```
4) bsp/allwinner/d1s/ - compiled, but no output to UART. 

### After I forked v5.0.2 and added:
- bsp\allwinner\d1s\tools\boot0_sdcard_sun20iw1p1_d1s.bin change to varian from https://github.com/smaeul/sun20i_d1_spl.git
- bsp\allwinner\d1s\sbi.bin change to varian from https://github.com/riscv-software-src/opensbi
- Change UART pins and boudrate in config 

After this changes board started and shown out to UART.
But loading frosen in rt_thread_mdelay function in files
- bsp/allwinner/libraries/drivers/sdmmc/drv_sdmmc.c
- bsp/allwinner/libraries/sunxi-hal/include/hal/sdmmc/osal/RT-Thread/_os_time.h

Chnage it to rt_hw_us_delay and is finnaly looks like works:
```
 \ | /
- RT -     Thread Smart Operating System
 / | \     5.0.2 build Jun  9 2024 17:11:05
 2006 - 2022 Copyright by RT-Thread team
hal_sdc_create 0
card_detect insert
Initial card success. capacity :15200MB
sdmmc bytes_per_secotr:200, sector count:1db0000
not found partition of mbr, construct sd0 at offset 8M, size:0xffffffffb5800000
[D/FAL] (fal_flash_init:47) Flash device |                  sdcard0 | addr: 0x00000000 | len: 0xb6000000 | blk_size: 0x00000200 |initialized finish.
[I/FAL] ==================== FAL partition table ====================
[I/FAL] | name       | flash_dev |   offset   |    length  |
[I/FAL] -------------------------------------------------------------
[I/FAL] | download   | sdcard0   | 0x00800000 | 0x00800000 |
[I/FAL] | easyflash  | sdcard0   | 0x01000000 | 0x00100000 |
[I/FAL] | filesystem | sdcard0   | 0x01100000 | 0x00c00000 |
[I/FAL] =============================================================
[I/FAL] RT-Thread Flash Abstraction Layer initialize success.
Hello RISC-V
msh />Mount "sd0p0" on "/" fail
msh />
msh />
```
This my changes https://github.com/RT-Thread/rt-thread/commit/ec2c0f10894bca703354e7e1d4c23fbaed5a034b

### Please could you make it compiled properly in master and figure out what happened with SDMMC driver?