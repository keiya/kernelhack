#!/bin/bash
sudo mv bzImage /boot/vmlinuz-custom
sudo mv System.map /boot/System.map-custom
sudo update-initramfs -c -k custom
sudo update-grub

