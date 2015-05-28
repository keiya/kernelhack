#!/bin/sh

cd ~/linux-4.0.1
cp ~/Documents/kernelhack/config_template .config

make -j2 && make modules && scp -c arcfour ~/linux-4.0.1/System.map ~/linux-4.0.1/arch/x86_64/boot/bzImage user@172.16.5.129:~
