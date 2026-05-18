obj-m := device_tree_BBB_kernel_module.o

KERNEL = /home/as/toolLinuxEmbedded/kernelbuildscripts/KERNEL

CROSS = /usr/bin/arm-linux-gnueabihf-

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL) M=$(shell pwd) modules

clean: 
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL) M=$(shell pwd) clean

build_dtb:
	dtc -I dts -O dtb -o am335x-boneblack.dtb am335x-boneblack.dts

copy_dtb:
	scp am335x-boneblack.dtb debian@192.168.1.100:/home/debian