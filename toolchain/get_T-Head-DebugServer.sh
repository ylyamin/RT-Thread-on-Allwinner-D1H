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
    if [ ! -d "$INSTALL_PATH/T-HEAD_DebugServer" ]; then
        echo "Installing T-Head-DebugServer"
        wget -P $INSTALL_PATH https://occ-oss-prod.oss-cn-hangzhou.aliyuncs.com/resource//1666331533949/T-Head-DebugServer-linux-x86_64-V5.16.5-20221021.sh.tar.gz
        tar -C $INSTALL_PATH -xvzf $INSTALL_PATH/T-Head-DebugServer-linux-x86_64-V5.16.5-20221021.sh.tar.gz 
        cd $INSTALL_PATH && sudo $INSTALL_PATH/T-Head-DebugServer-linux-x86_64-V5.16.5-20221021.sh -i
    else
        echo "Directory $INSTALL_PATH/T-HEAD_DebugServer already exist"
    fi
else
    echo "Removing T-Head-DebugServer"
	rm -rf $INSTALL_PATH/T-Head-DebugServer*
	sudo rm -rf $INSTALL_PATH/T-HEAD_DebugServer*
fi


