#!/bin/bash
VER="4.0.5"
cd ~
sudo cp ~/bzImage /boot/vmlinuz-$VER
sudo cp ~/System.map /boot/System.map-$VER
tar xf ~/_modules.tar
sudo cp -r ~/lib/modules/* /lib/modules/$VER
sudo update-initramfs -c -k $VER
sudo update-grub

