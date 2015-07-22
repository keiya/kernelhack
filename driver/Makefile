#EXTRA_CFLAGS="-fsyntax-only -Wno-unused-variable -Wno-unused-value"

obj-m := misc.o

ifndef KERNEL_DIR
KERNEL_DIR=~/linux-4.0.7/
endif

all:
	${MAKE} -C ${KERNEL_DIR} M=`pwd`

clean:
	${MAKE} -C ${KERNEL_DIR} M=`pwd` clean

install:
	cp misc.ko ${TARGET_DIR}

