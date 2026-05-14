obj-m := led.o

FILE ?= ""

KERNEL_SRC = /home/as/toolLinuxEmbedded/kernelbuildscripts/KERNEL

CROSS = /usr/bin/arm-linux-gnueabihf-

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL_SRC) M=$(shell pwd) modules

clean:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL_SRC) M=$(shell pwd) clean

copy:
	scp $(FILE) debian@192.168.1.100:/home/debian