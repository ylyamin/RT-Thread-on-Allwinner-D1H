  #!/bin/bash
MODE="install"
INSTALL_PATH=$PWD

if [ $# -eq 0 ] ; then
    echo 'Usage: '$0' install[remove] installing_path'
    echo 'Default: '$0' install $PWD'
fi
if [ "$1" ]; then MODE=$1; fi
if [ "$2" ]; then INSTALL_PATH=$2; fi

if [ "$MODE" = "install" ]; then
    if [ ! -d "$INSTALL_PATH/riscv64-glibc-gcc-thead_20200702" ]; then
        echo "Installing riscv64-glibc-gcc-thead"
        wget -P $INSTALL_PATH https://github.com/YuzukiHD/sunxi-bsp-toolchains/releases/download/1.0.0/riscv64-glibc-gcc-thead_20200702.tar.xz
        tar -C $INSTALL_PATH -xf $INSTALL_PATH/riscv64-glibc-gcc-thead_20200702.tar.xz
    else
        echo "Directory $INSTALL_PATH/riscv64-glibc-gcc-thead_20200702 already exist"
    fi
else
    echo "Removing riscv64-glibc-gcc-thead"
	rm -rf $INSTALL_PATH/riscv64-glibc-gcc-thead_20200702*
fi