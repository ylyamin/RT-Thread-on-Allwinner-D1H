TOOLCHAIN_INSTALL_DIR ?= "$(shell pwd)/toolchain"
RISCV64-GLIBC-GCC = "$(TOOLCHAIN_INSTALL_DIR)/riscv64-glibc-gcc-thead_20200702/bin/riscv64-unknown-linux-gnu-"
RISCV64-MUSL = "$(TOOLCHAIN_INSTALL_DIR)/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu/bin"

all: install_toolchain

install_toolchain:
	cd toolchain && ./get_riscv64-musle.sh install $(TOOLCHAIN_INSTALL_DIR)
	cd toolchain && ./get_riscv64-glibc-gcc-thead.sh install $(TOOLCHAIN_INSTALL_DIR)
	cd toolchain && ./get_T-Head-DebugServer.sh install $(TOOLCHAIN_INSTALL_DIR)

clear_toolchain:
	cd toolchain && ./get_riscv64-musle.sh remove $(TOOLCHAIN_INSTALL_DIR)
	cd toolchain && ./get_riscv64-glibc-gcc-thead.sh remove $(TOOLCHAIN_INSTALL_DIR)
	cd toolchain && ./get_T-Head-DebugServer.sh remove $(TOOLCHAIN_INSTALL_DIR)

compile_bootloaders:
	git submodule update --init
	cd bootloaders/opensbi/ && make CROSS_COMPILE=$(RISCV64-GLIBC-GCC) PLATFORM=generic  FW_DYNAMIC=y FW_TEXT_START=0x40000000
	cd bootloaders/sun20i_d1_spl/ && make CROSS_COMPILE=$(RISCV64-GLIBC-GCC) OPENSBI=../opensbi/build/platform/generic/firmware/fw_dynamic.bin p=sun20iw1p1 mmc
	cd bootloaders/u-boot/ && make CROSS_COMPILE=$(RISCV64-GLIBC-GCC) lichee_rv_defconfig #!!!
	cd bootloaders/u-boot/ && make CROSS_COMPILE=$(RISCV64-GLIBC-GCC)
	#xfel
compile_rt:
	cd rt-thread/bsp/allwinner/d1s_d1h/ && RTT_EXEC_PATH=$(RISCV64-MUSL) scons


