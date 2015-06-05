#!/bin/sh

cd ~/kernelhack/driver
make

make && scp -c arcfour misc.ko user@172.16.5.129:~
