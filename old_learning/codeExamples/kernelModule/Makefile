# Chỉ thị obj file nào sẽ được build thành kernel module
obj-m := hello_module.o

# trỏ kernel source đang lấy ở đâu, trong này chứa linker và memory map để build
KERNEL_SRC = /lib/modules/$(shell uname -r)/build

all: 
	make -C $(KERNEL_SRC) M=$(shell pwd) modules

clean:
	make -C $(KERNEL_SRC) M=$(shell pwd) clean