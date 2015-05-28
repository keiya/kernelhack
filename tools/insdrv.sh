#!/bin/bash
sudo insmod misc.ko
GR1=($(grep misc /proc/devices))
GR2=($(grep misc /proc/misc))
#echo ${GR2[0]}
sudo mknod ${GR2[1]} c ${GR1[0]} ${GR2[0]}
