#EXTRA_CFLAGS="-fsyntax-only -Wno-unused-variable -Wno-unused-value"

obj-m := periodic_task.o

ifndef KERNEL_DIR
	KERNEL_DIR=~/linux-4.0.9/
endif

all:
		${MAKE} -C ${KERNEL_DIR} M=`pwd`

clean:
		${MAKE} -C ${KERNEL_DIR} M=`pwd` clean

install:
		cp periodic_task.ko ${TARGET_DIR}
