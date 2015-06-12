#!/bin/sh

LINUXBASE="$HOME/linux-4.0.5"

cd $LINUXBASE
cp ~/Documents/kernelhack/config_template .config

make -j2 && make modules && scp -c arcfour $LINUXBASE/System.map $LINUXBASE/arch/x86_64/boot/bzImage user@172.16.5.129:~
