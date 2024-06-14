#Variables
BUILD = build
SD_IMAGE = image/sd_image.img
SD_MOUNT = /dev/sdb
TOOLCHAIN_INSTALL_DIR ?= $(shell pwd)/toolchain
U_BOOT_INSTALL_DIR ?= $(shell pwd)/bootloaders/u-boot
DEBUGGER_INSTALL_DIR = $(shell pwd)/debugger

RED=\033[0;31m
NC=\033[0m

#Toolcahin
RISCV64_MUSL_DIR = $(TOOLCHAIN_INSTALL_DIR)/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu
RISCV64_GLIBC_GCC_DIR = $(TOOLCHAIN_INSTALL_DIR)/riscv64-glibc-gcc-thead_20200702
T_HEAD_DEBUGSERVER_DIR = $(TOOLCHAIN_INSTALL_DIR)/T-HEAD_DebugServer

RISCV64_MUSL_BIN = $(RISCV64_MUSL_DIR)/bin
RISCV64_GLIBC_GCC_BIN = $(RISCV64_GLIBC_GCC_DIR)/bin/riscv64-unknown-linux-gnu-
T_HEAD_DEBUGSERVER_BIN = $(T_HEAD_DEBUGSERVER_DIR)/DebugServerConsole.elf

$(RISCV64_MUSL_DIR):
	wget -P $(TOOLCHAIN_INSTALL_DIR) https://github.com/RT-Thread/toolchains-ci/releases/download/v1.7/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu_latest.tar.bz2
	tar -C $(TOOLCHAIN_INSTALL_DIR) -xf $(TOOLCHAIN_INSTALL_DIR)/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu_latest.tar.bz2

$(RISCV64_MUSL_DIR)-remove:
	rm -rf $(TOOLCHAIN_INSTALL_DIR)/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu*

$(RISCV64_GLIBC_GCC_DIR):
	cd toolchain && ./get_riscv64-glibc-gcc-thead.sh install $(TOOLCHAIN_INSTALL_DIR)
	wget -P $(TOOLCHAIN_INSTALL_DIR) https://github.com/YuzukiHD/sunxi-bsp-toolchains/releases/download/1.0.0/riscv64-glibc-gcc-thead_20200702.tar.xz
	tar -C $(TOOLCHAIN_INSTALL_DIR) -xf $(TOOLCHAIN_INSTALL_DIR)/riscv64-glibc-gcc-thead_20200702.tar.xz

$(RISCV64_GLIBC_GCC_DIR)-remove:
	rm -rf $(TOOLCHAIN_INSTALL_DIR)/riscv64-glibc-gcc-thead_20200702*

$(T_HEAD_DEBUGSERVER_DIR):
	wget -P $(TOOLCHAIN_INSTALL_DIR) https://occ-oss-prod.oss-cn-hangzhou.aliyuncs.com/resource//1666331533949/T-Head-DebugServer-linux-x86_64-V5.16.5-20221021.sh.tar.gz
	tar -C $(TOOLCHAIN_INSTALL_DIR) -xvzf $(TOOLCHAIN_INSTALL_DIR)/T-Head-DebugServer-linux-x86_64-V5.16.5-20221021.sh.tar.gz 
	cd $(TOOLCHAIN_INSTALL_DIR) && sudo $(TOOLCHAIN_INSTALL_DIR)/T-Head-DebugServer-linux-x86_64-V5.16.5-20221021.sh -i

$(T_HEAD_DEBUGSERVER_DIR)-remove:
	rm -rf $(TOOLCHAIN_INSTALL_DIR)/T-Head-DebugServer*
	sudo rm -rf $(TOOLCHAIN_INSTALL_DIR)/T-HEAD_DebugServer*

toolchain: $(RISCV64_MUSL_DIR) $(RISCV64_GLIBC_GCC_DIR) $(T_HEAD_DEBUGSERVER_DIR)

toolchain-remove: $(RISCV64_MUSL_DIR)-remove $(RISCV64_GLIBC_GCC_DIR)-remove $(T_HEAD_DEBUGSERVER_DIR)-remove

#Bootloaders
submodules:
	git submodule update --init

opensbi:
	cd bootloaders/opensbi/ && make CROSS_COMPILE=$(RISCV64_GLIBC_GCC_BIN) PLATFORM=generic  FW_DYNAMIC=y FW_TEXT_START=0x40000000

sun20i_d1_spl:
	cd bootloaders/sun20i_d1_spl/ && make CROSS_COMPILE=$(RISCV64_GLIBC_GCC_BIN) p=sun20iw1p1 mmc

xfel:
	cd bootloaders/xfel/ && make && sudo make install

U_BOOT_INSTALL_DIR_ORIGIN = $(shell pwd)/bootloaders/u-boot
$(U_BOOT_INSTALL_DIR):
    ifneq ($(U_BOOT_INSTALL_DIR_ORIGIN),$(U_BOOT_INSTALL_DIR))
		#mkdir -p $(U_BOOT_INSTALL_DIR)
		#cd $(U_BOOT_INSTALL_DIR_ORIGIN) && tar cf - . | tar xf - -C $(U_BOOT_INSTALL_DIR) #Not make after this don't know why
		git clone https://github.com/smaeul/u-boot $(U_BOOT_INSTALL_DIR)
		cd $(U_BOOT_INSTALL_DIR) && git checkout d1-2022-04-05
    endif

u-boot: $(U_BOOT_INSTALL_DIR)
	cd $(U_BOOT_INSTALL_DIR) && make CROSS_COMPILE=$(RISCV64_GLIBC_GCC_BIN) lichee_rv_defconfig
	cd $(U_BOOT_INSTALL_DIR) && make CROSS_COMPILE=$(RISCV64_GLIBC_GCC_BIN) ARCH=riscv OPENSBI=${U_BOOT_INSTALL_DIR_ORIGIN}/../opensbi/build/platform/generic/firmware/fw_dynamic.bin

bootloaders: submodules opensbi sun20i_d1_spl xfel u-boot

#RT-thread
rt:
	cd rt-thread/bsp/allwinner/d1s_d1h/ && RTT_EXEC_PATH=$(RISCV64_MUSL_BIN) scons

rt_clean:
	cd rt-thread/bsp/allwinner/d1s_d1h/ && RTT_EXEC_PATH=$(RISCV64_MUSL_BIN) scons -c

rt_conf:
	cd rt-thread/bsp/allwinner/d1s_d1h/ && RTT_EXEC_PATH=$(RISCV64_MUSL_BIN) scons -menuconfig

#SD card
$(SD_IMAGE):
	/home/yury/Lichee/u-boot/tools/mkimage -T sunxi_toc1 -d $(BUILD)/toc1_D1H.cfg $(BUILD)/sd.bin
	sudo dd if=bootloaders/sun20i_d1_spl/nboot/boot0_sdcard_sun20iw1p1.bin of=$(SD_IMAGE) bs=8192 seek=16
	sudo dd if=$(BUILD)/sd.bin of=$(SD_IMAGE) bs=512 seek=32800

sd: $(SD_IMAGE)

sd_burn:
	sudo dd if=$(SD_IMAGE) of=$(SD_MOUNT) bs=512 seek=16 conv=sync

#Debug
$(DEBUGGER_INSTALL_DIR)/blisp:
	wget -P $(DEBUGGER_INSTALL_DIR) https://github.com/bouffalolab/bouffalo_sdk/blob/master/tools/cklink_firmware/bl702_cklink_whole_img_v2.2.bin
	wget -P $(DEBUGGER_INSTALL_DIR) https://github.com/pine64/blisp/releases/download/v0.0.4/blisp-linux-x86_64-v0.0.4.zip
	unzip $(DEBUGGER_INSTALL_DIR)/blisp-linux-x86_64-v0.0.4.zip -d $(DEBUGGER_INSTALL_DIR)

debug_burn: $(DEBUGGER_INSTALL_DIR)/blisp
	@echo "${RED}Press and hold the boot pin then plug the usb in the computer to go to the boot mode.${NC}"
	$(DEBUGGER_INSTALL_DIR)/blisp iot -c bl70x --reset -s $(DEBUGGER_INSTALL_DIR)/bl702_cklink_whole_img_v2.2.bin -l 0x0

debug:
	@echo "${RED}Press and hold the FEL pin then press RESET pin to go to the FEL mode.${NC}"
	xfel ddr d1
	xfel jtag
	$(T_HEAD_DEBUGSERVER_BIN)&
	$(RISCV64_GLIBC_GCC_BIN)gdb

all: rt



