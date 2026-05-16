obj-m := led.o

FILE1 ?= ""
FILE2 ?= ""

KERNEL_SRC = /home/as/toolLinuxEmbedded/kernelbuildscripts/KERNEL
ccflags-y += -fno-stack-protector
CROSS = /usr/bin/arm-linux-gnueabihf-

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL_SRC) M=$(shell pwd) modules
	arm-linux-gnueabihf-gcc -o led_app led_app.c

clean:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL_SRC) M=$(shell pwd) clean
	rm -rf led_app.o

copy:
	scp led.ko led_app debian@192.168.1.100:/home/debian

build_app:
	arm-linux-gnueabihf-gcc -o led_app led_app.c