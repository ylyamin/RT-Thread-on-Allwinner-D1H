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
    if [ ! -d "$INSTALL_PATH/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu" ]; then
        echo "Installing riscv64-linux-musl"
        wget -P $INSTALL_PATH https://github.com/RT-Thread/toolchains-ci/releases/download/v1.7/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu_latest.tar.bz2
        tar -C $INSTALL_PATH -xf $INSTALL_PATH/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu_latest.tar.bz2
    else
        echo "Directory $INSTALL_PATH/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu already exist"
    fi
else
    echo "Removing riscv64-linux-musl"
	rm -rf $INSTALL_PATH/riscv64-linux-musleabi_for_x86_64-pc-linux-gnu*
fi