#!/bin/sh

VER="4.0.5"
LINUXBASE="$HOME/linux-$VER"

cd $LINUXBASE
cp ~/Documents/kernelhack/config_template .config


make -j2 && make modules && make INSTALL_MOD_PATH=$LINUXBASE/modules_compiled modules_install  && \
  scp -c arcfour $LINUXBASE/System.map $LINUXBASE/arch/x86_64/boot/bzImage user@172.16.5.129:~

mkdir -p $LINUXBASE/modules_compiled
cd $LINUXBASE/modules_compiled
tar cvf $LINUXBASE/_modules.tar lib/modules && \
scp -c arcfour $LINUXBASE/_modules.tar user@172.16.5.129:~
rm $LINUXBASE/_modules.tar
