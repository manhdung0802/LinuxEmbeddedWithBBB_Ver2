ARCH = arm
CROSS_COMPILE = arm-bootlin-linux-gnueabihf-

ifneq ($(KERNELRELEASE),)
obj-m := oled_spi.o
else
KDIR := /home/as/Desktop/linuxEmbeddedBBB/bootlin/linux-kernel/linux-kernel-bbb-labs/src/linux
all:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$$PWD clean
endif
