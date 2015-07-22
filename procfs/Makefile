#EXTRA_CFLAGS="-fsyntax-only -Wno-unused-variable -Wno-unused-value"

obj-m := uptimetest.o

ifndef KERNEL_DIR
	KERNEL_DIR=~/linux-4.0.9/
endif

all:
		${MAKE} -C ${KERNEL_DIR} M=`pwd`

clean:
		${MAKE} -C ${KERNEL_DIR} M=`pwd` clean

install:
		cp uptimetest.ko ${TARGET_DIR}
