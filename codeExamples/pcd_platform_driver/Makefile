obj-m := pcd_device_setup.o pcd_platform_driver.o

CROSS_COMPILE = arm-linux-gnueabihf-
KERNEL = /home/as/Desktop/linuxEmbeddedBBB/ldd/source/linux_bbb_5.10.168-ti-rt-r76
KERNEL_HOST = /lib/modules/$(shell uname -r)/build
all:
	make ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL) M=$(shell pwd) modules
host:
	make -C $(KERNEL_HOST) M=$(shell pwd) modules
clean: 
	make ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL) M=$(shell pwd) clean
	make -C $(KERNEL_HOST) M=$(shell pwd) clean