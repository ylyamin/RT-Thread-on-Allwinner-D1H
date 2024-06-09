#/bin/sh
./mkimage_D1H -T sunxi_toc1 -d toc1.cfg sd.bin
# offset: 8K of spl，+48K of toc1
# dd if=/dev/zero of=image.img bs=1M count=150
sudo dd if=tools/boot0_sdcard_sun20iw1p1.bin of=image.img bs=8192 seek=16 #bs=1024 seek=8
sudo dd if=sd.bin of=image.img bs=512 seek=32800 #bs=1024 seek=56

