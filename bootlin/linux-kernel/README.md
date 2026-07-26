# Menu

- [Linux Kernel Introduction](#linux-kernel-introduction)
  - [Linux kernel in the system](#linux-kernel-in-the-system)
  - [System calls](#system-calls)
  - [Pseudo system](#pseudo-system)
  - [Linux kernel source code](#linux-kernel-source-code)
- [Linux kernel usage](#linux-kernel-usage)
  - [Kernel configuration](#kernel-configuration)
  - [Compiling and installing the kernel](#compiling-and-installing-the-kernel)
  - [Booting the kernel](#booting-the-kernel)
  - [Using kernel modules](#using-kernel-modules)
- [Developing kernel modules](#developing-kernel-modules)
- [Describing hardware devices](#describing-hardware-devices)
  - [Discoverable hardware: USB and PCI](#discoverable-hardware-usb-and-pci)
  - [Describing non-discoverable hardware](#describing-non-discoverable-hardware)
  - [Device tree](#device-tree)
  - [Thực hành](#thực-hành)
- [Introduction to pin muxing](#introduction-to-pin-muxing)
  - [What is pin muxing](#what-is-pin-muxing)
  - [Pin muxing in the Linux kernel](#pin-muxing-in-the-linux-kernel)
  - [Thực hành](#thực-hành-1)
- [Linux device and driver model](#linux-device-and-driver-model)
  - [Introduction](#introduction)
  - [Example of the USB bus](#example-of-the-usb-bus)
  - [Platform drivers](#platform-drivers)
- [Introduction to the I2C subsystem](#introduction-to-the-i2c-subsystem)
  - [What is I2C?](#what-is-i2c)
  - [An I2C bus example](#an-i2c-bus-example)
  - [The I2C bus driver](#the-i2c-bus-driver)
  - [Registering an I2C device driver](#registering-an-i2c-device-driver)
  - [Registering an I2C device driver: example](#registering-an-i2c-device-driver-example)
  - [Registering an I2C device: non-DT](#registering-an-i2c-device-non-dt)
  - [Registering an I2C device, non-DT example](#registering-an-i2c-device-non-dt-example)
  - [Registering an I2C device, in the DT](#registering-an-i2c-device-in-the-dt)
  - [probe() and remove()](#probe-and-remove)
  - [Communicating with the I2C device: raw API](#communicating-with-the-i2c-device-raw-api)
  - [Communicating with the I2C device: message transfer](#communicating-with-the-i2c-device-message-transfer)
  - [I2C: message transfer example](#i2c-message-transfer-example)
  - [I2C functionality](#i2c-functionality)
  - [Thực hành](#thực-hành-2)
- [Kernel framework for device drivers](#kernel-framework-for-device-drivers)
  - [Kernel and Device drivers](#kernel-and-device-drivers)
  - [User space vision of devices](#user-space-vision-of-devices)
  - [Devices: everything is a file](#devices-everything-is-a-file)
  - [Creating device files](#creating-device-files)
  - [Character drivers](#character-drivers)
  - [The concept of kernel frameworks](#the-concept-of-kernel-frameworks)
  - [Example: the input subsystem](#example-the-input-subsystem)
  - [Device-managed allocations](#device-managed-allocations)
  - [Driver data structures and links](#driver-data-structures-and-links)
  - [Thực hành](#thực-hành-3)
- [Memory Management](#memory-management)
  - [Physical and virtual memory](#physical-and-virtual-memory)
  - [Virtual memory organization](#virtual-memory-organization)

# Linux Kernel Introduction
## Linux kernel in the system
- ![alt text](images/image.png)
- Linux kernel nằm có vai trò cầu nối giữa user-space và hardware
- Chức năng chính:
    + quản lý tất cả phần cứng
    + cung cấp các API cho phép user-space app và lib sử dụng
    + quản lý truy cập phần cứng đồng thời của các app
## System calls
- là interface giữa kernel và userspace
- userspace thường không gọi systemcall trực tiếp và thông qua library
## Pseudo system
- Linux expose thông tin system và kernel tới userspace thông qua `pseudo filesystems`, hay còn gọi là `virtual filesystems`
- cho phép app quan sát được thư mục và file mà không tồn tại trong ổ cứng
- quan trọng nhất là: 
    + proc: thông tin hệ thống (process, memory, ...)
    + sysfs: thông tin phần cứng kết nói bởi các bus
## Linux kernel source code
- Kernel momery constraints
    + Kernel không có cơ chế bảo vệ momery
    + Kernel không có cơ chế phục hồi khi truy cập bộ nhớ trái phép

# Linux kernel usage
## Kernel configuration
- Kernel chứa hàng ngàn device driver, filesystem driver, network, ...
- có hàng ngàn option để chọn để compile cùng với kernel
- cấu hình kernel là quá trình thêm bớt option để compile cùng kernel
- việc cấu hình dựa vào target, khả năng mà developer muốn kernel có
- Kernel module:
    + mỗi module sẽ có 1 file đại diện trong hệ thống
    + không thể load lúc hệ thống đang boot vì lúc đang boot thì filesystem chưa available -> dẫn tới không thể tạo file cho module đó
- Kernel option dependencies:
    + B available nếu A available
        ```
        config B
            depends on A
        ```
    + B được enable thì A cũng bị enable theo, không thể off A thủ công
        ```
        config B
            select A
        ```
- xconfig:
    + make xconfig
    + giao diện GUI để cấu hình khi có màn hình
- menuconfig:
    + make menuconfig
- .config.old
    + lưu config trước đó 1 nhịp
    + dùng để restore nếu thiết lập kernel lỗi
## Compiling and installing the kernel
- cài kernel module cho hệ thống nhúng
    + `make modules` hoặc `make -j4` (nếu build cùng kernel)
    + `make INSTALL_MOD_PATH=<dir>/ modules_install`
- tổng quan việc compile và install kernel
    + ![alt text](images/image-1.png)
## Booting the kernel
- Hardware description
    + nhiều hệ thống embedded có nhiều non-discoverable hardware
    + những hardware này cần được mô tả trong device tree và pass vào Linux kernel
    + bằng cách này, kernel hỗ trợ nhiều SoC có thể biết được SoC nào tương thích với device tree nào
- Kernel command line - bootargs
    + hành vi của kernel được điều chỉnh mà không cần recompile bằng việc dùng bootargs
    + root=: đường dẫn rootfs
    + console=: nơi mà kernel in log ra
    + ví dụ: bootargs=console=ttyS0 root=/dev/mmc rootwait
    + ngay sau khi kernel start, nó sẽ chứa nội dung của bootargs
    + nội dung của bootargs được chứa trong `/proc/cmdline`
- Kernel log
    + kernel giữ message của nó trong 1 buffer, size của buffer được cấu hình bằng `CONFIG_LOG_BUF_SHIFT`
## Using kernel modules
- Advantage of modules:
    + dễ dàng develop mà không cần reboot hệ thống
    + giữ được size của kernel image nhỏ
    + giảm thời gian boot
    + kernel chỉ cho phép load signed modules
- Module utilities
    + extracing information
        - `modinfor <module>`: lây thông tin của module
    + loading
        - `insmod`: chỉ load 1 module
        - `modprobe`: load module và các dependencies của nó
        - `lsmod`
    + removals
        - `rmmod`
        - `modprobe -r`
- Passing parameters to modules
    + xem thông tin parameter của module: `modinfo <module>`
    + truyền value với insmod: `insmod name.ko param=0`
    + truyền value với modprove: set param trong /etc/modprobe.conf hoặc các file trong /etc/modprobe.d/
        - `option usb-storage delay_usb=0`
    + truyền value qua kernel command line, khi mà module được build static với kernel
        `usb-storage.delay_use=0`
- Check module parameter values
    + check trong `/sys/module/<name>/parameter`
    + mỗi param là 1 file
    + có thể dùng echo để ghi giá trị vào file chứa param đó

# Developing kernel modules
- Hello module
```c
// SPDX-License-Identifier: GPL-2.0
/* hello.c */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
static int __init hello_init(void)
{
 pr_alert("Good morrow to this fair assembly.\n");
 return 0;
}
static void __exit hello_exit(void)
{
 pr_alert("Alas, poor world, what treasure hast thou lost!\n");
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Greeting module");
MODULE_AUTHOR("William Shakespeare");
```
- `__init`: remove sau quá trình khởi tạo, memory sẽ được lấy lại sau khi kernel init xong
- `__exit`: bị loại bỏ nếu module được build static vào kernel hoặc khi tính năng unload không enable
- Hello module explanations:
    + header là của Linux, không dùng thư viện C thông thường
    + Hàm init khởi tạo:    
        - được gọi khi module được load
        - khai báo cùng macro `module_init()`
    + Hàm exit:
        - được gọi khi module được unload
        - khai báo cùng macro `module_exit()`
    + Metadata information: `MODULE_LICENSE()`, `MODULE_DESCRIPTION()`, `MODULE_AUTHOR()`
- Symbols exported to modules
    + kernel module chỉ có thể gọi 1 số function của kernel
    + function và biến phải được export rõ ràng bởi kernel thì kernel module mới gọi được
    + Macro được dùng để kernel export function và biến:
        - `EXPORT_SYSBOL(name)`: export tới tất cả modules
        - `EXPORT_SYMBOL_GPL(name)`: export tới những module có GPL licenses
    + 1 driver thông thường không nên dùng các function chưa được export
    + ![alt text](images/image-2.png)
- Module license
    + Được dùng để giới hạn kernel function mà module có thể dùng nếu module không phải là license GPL
    + Nếu nạp 1 kernel mã nguồn đóng, Linux kernel sẽ đánh dấu là tainted (bị bẩn)
    + check `/proc/sys/kernel/tainted`, nếu 100% là k bị tainted, k có module độc quyền nào được nạp
    + hoặc chạy `vrms` để check có tainted không
    + Nhóm license mã nguồn mở: GPL, GPL v2, ...
- Compiling a module
    + out-of-tree:
        - code nằm ngoài kernel source tree
        - k tích hợp vào kernel
        - build riêng
        - chỉ có thể build thành module
    + inside the kernel tree
        - tích hợp vào kernel
        - có thể build cùng kernel hoặc the module
- Compiling an out-of-tree module
    ```c
    ifneq ($(KERNELRELEASE),)
    obj-m := hello.o
    else
    KDIR := /path/to/kernel/sources
    all:
    <tab>$(MAKE) -C $(KDIR) M=$$PWD
    endif
    ```
    + KERNELRELEASE chưa được định nghĩa nên Makefile của module sẽ tìm tới Makefile của kernel, pass đường dẫn chứa module trong biến `M`
    + kernel Makefile biết cách để build module, và nhờ vào biến `M`, nó biết nơi chứa Makefile của module. Makefile của module này sau đó tích hợp cùng KERNELRELEASE được định nghĩa, vì vậy kernel có thể biết được cần biên dịch `hello.o`
- Modules and kernel version
    + để biên dịch, kernel module cần truy cập được đường dẫn của kernel header, nơi chứa định nghĩa hàm, type, ....
    + có 2 giải pháp để lấy được kernel header:
        - full kernel source
        - only kernel headers
    + source hoặc header cần phải được cấu hình (.config file)
    + kernel module được compile với ver X của kernel header không thể load được trong máy chạy ver Y của kernel header
- New driver in kernel sources
    + để add thêm driver vào kernel source, cần:
        - add source code vào thư mục phù hợp
        - driver code thường chỉ là 1 file, dù file lớn thì cũng 1 file. Chỉ khi code thực sự lớn mới tách
        - mô tả cấu hình interface cho driver mới bằng cách add thêm lệnh sau vào Kconfig (cách này thủ công k dùng Builtroot)
            ```
            config USB_SERIAL_NAVMAN
                tristate "USB Navman GPS device"
                depends on USB_SERIAL
                help
                    To compile this driver as a module, choose M
                    here: the module will be called navman.
            ```
        - add thêm dòng trong Makefile dựa theo Kconfig
            + `obj-$(CONFIG_xxx) += hello.o`
            + lệnh này nói với kernel build system rằng hãy build hello.c khi CONFIG_xxx được bật (cho cả build static hoặc module)
                - run `make xconfig` để thấy optione vừa được thêm
                - run `make` để build
        - sau khi build xong, module sẽ được khởi động cùng hệ thống, check lại /proc/version
- Hello module với parameter
    ```c
    // SPDX-License-Identifier: GPL-2.0
    /* hello_param.c */
    #include <linux/init.h>
    #include <linux/module.h>
    MODULE_LICENSE("GPL");
    static char *whom = "world";
    module_param(whom, charp, 0644);
    MODULE_PARM_DESC(whom, "Recipient of the hello message");
    static int howmany = 1;
    module_param(howmany, int, 0644);
    MODULE_PARM_DESC(howmany, "Number of greetings");
    static int __init hello_init(void)
    {
        int i;
        for (i = 0; i < howmany; i++)
        pr_alert("(%d) Hello, %s\n", i, whom);
        return 0;
    }
    static void __exit hello_exit(void)
    {
        pr_alert("Goodbye, cruel %s\n", whom);
    }
    module_init(hello_init);
    module_exit(hello_exit);
    ```
- Declaring a module parameter
    ```c
    module_param(
    name, /* name of an already defined variable */
    type, /* standard types (different from C types) are:
    * byte, short, ushort, int, uint, long, ulong
    * charp: a character pointer
    * bool: a bool, values 0/1, y/n, Y/N.
    * invbool: the above, only sense-reversed (N = true). */
    perm /* for /sys/module/<module_name>/parameters/<param>,
    * 0: no such module parameter value file */
    );
    /* Example: drivers/block/loop.c */
    static int max_loop;
    module_param(max_loop, int, 0444);
    MODULE_PARM_DESC(max_loop, "Maximum number of loop devices");
    ```
- Thực hành:
    + dùng utsname.h để in ra version kernel được build
    + Khai báo param truyền vào module lúc load:
        ```c
        static char *who = "";
        module_param(who, charp, 0644); //owner: read write, others: read
        MODULE_PARM_DESC(who, "Name of who"); // mô tả param
        ```
    + Linux cung cấp công cụ để kiểm tra coding standard: 
        - `~/linux-kernel-bbb-labs/src/linux/scripts/checkpatch.pl --file --no-tree hello_version.c`
    + truyền tham số do kernel module nếu được build static bằng cách thêm vào bootargs chuỗi: `hello.who=dung`

# Describing hardware devices
## Discoverable hardware: USB and PCI
- Discoverable hardware:
    + 1 số bus có cơ chế phát hiện phần cứng, phổ biến đó là USB và PCI
    + Hardware device được đánh số, và đặc tính của chúng được lấy ra bởi 1 driver hoặc bus controller
    + Lệnh:
        - `lsusb`: liệt kê các USB device
        - `lspci`: liệt kê các PCI device
    + 1 device được detect không có nghĩa rằng có 1 kernel driver gắn với nó
    + Việc gắn với kernel driver cần dựa vào product ID/vendor ID hoặc các đặc tính như: device class, device sub-class, ...
## Describing non-discoverable hardware
- Có 3 cách mô tả phần cứng non-discoverable
    + trực tiếp trong OS/Bootloader code
    + dùng ACPI table
    + dùng device tree
## Device tree
- Device tree: from source to blob (từ source tới file nhị phân)
    + `.dts`: device tree source
    + `dtc`: device tree compiler 
        - Lệnh build: `dtc -I dts -O dtb -o foo.dtb foo.dts`
        - Lệnh đọc: `dtc -I dtb -0 dts foo.dtb`
    + `.dtb`: device tree blob (device tree binary)
    + DTB còn gọi là FDT - Flattened device tree
- Where are Device tree sources located?    
    + `arch/<ARCH>/boot/dts/<vendor>`
    + trong U-boot, TF-A cũng có sẵn folder chứa device tree được copy từ Linux kernel source
- Device tree base syntax
    + ![alt text](images/image-3.png)
- DT overall structure: simplified example
    + ![alt text](images/image-4.png)
    + ![alt text](images/image-5.png)
    + ![alt text](images/image-6.png)
    + ![alt text](images/image-7.png)
    + ![alt text](images/image-8.png)
    + ![alt text](images/image-9.png)
- Device tree inheritance
    + device tree có thể được chia thành nhiều file
    + `.dtsi`: là file dùng để include vào file khác
        - chứa định nghĩa Soc-level
        - định nghĩa chung cho 1 số lượng board
    + `.dts`: là file device tree cuối cùng
    + việc ghi đè (overlay) device tree được tuân theo thứ tự include, cho phép file sau ghi đè giá trị của file trước
- DT inheritance in Bone Black support
    + ![alt text](images/image-10.png)
- Device tree design principles
    + device tree mô tả phần cứng chứ không phải cách cấu hình phần cứng
    + device tree đọc lập với OS, khi OS thay đổi thì device tree không cần đổi
    + device tree còn mô tả các hardware component mà tích hợp với hệ thống, không phải cách chúng hoạt động
- The properties
    + có thể dùng chung và áp dụng cho phần lớn các nodes: compatible, reg, ...
    + bao hàm mối quan hệ consumer-provider
    + đặc thù theo từng subsystem: tất cả device của cùng 1 class có thể dùng chúng, bắt đầu bằng class name
    + node cha (i2c controller, spi controller, ...) rồi đến node con (các device nối vào bus i2c, spi, ... đó)
    + đặc thù theo vendor, device: mô tả đặc tính cho riêng vendor/device, bắt đầu bằng tiền tố <vendor>
    + 1 số thuộc tính đã lỗi thời
- The compatible property
    + mô tả specific binding mà node đó tuân theo
    + định danh programming model của device
    + OS dùng thuộc tính này để tìm driver phù hợp cho device
    + thường mô tả kèm tên vendor, model: `arm,armv7-timer`, `gpio-keys`
    + giá trị đặc biệt `simple-bus`: là bus nơi mà các node con là memory-mapped device
- compatible property and Linux kernel drivers
    + Linux nhận diện các platform device thông qua:
        - các node ở cao nhất chứa `compatible`
        - các sub-note của `simple-bus`
    + sub-node của I2C controller là I2C device
    + sub-node của SPI controller là SPI device
    + mỗi Linux driver có bảng compatible hỗ trợ `struct of_device_id[]`
    + khi 1 device tree node match compatible với driver, device đó được liên kết với driver đó
- reg property
    + mô tả địa chỉ base physical address và size của memory-mapped register, có thể chứa nhiều entries cho nhiều cùng register
        ```c
        sai4: sai@5007 {
            reg = <0x5007 0x4>, <0x5100 0x4>;
        }
        ```
    + i2c device trên i2c bus
        ```c
        &i2c {
            hdmi-transmitter@39 {
                reg = <0x39>;
            };
            cs42l51: cs42l51@4a {
                reg = <0x4a>;
            };
        }
        ```
    + spi device: chip select number
        ```c
        &qspi {
            flash0: mx66l51235l@0 {
            reg = <0>;
            };
            flash1: mx66l51235l@1 {
            reg = <1>;
            };
        };
        ```
    + địa chỉ của node sau dấu @ phải là địa chỉ của vùng nhớ đầu tiên được khai báo trong `reg`
        ```c
        sai4: sai@50027000 {
            reg = <0x50027000 0x4>, <0x500273f0 0x10>;
        };
        ```
- cell property
    + giá trị của các thuộc tính phải vừa vặn trong vùng 32bit, được gọi là các cell
    + khi khai báo `reg = <0x50027000 0x4>, <0x500273f0 0x10>;`, OS không hiểu đang muốn nói tới trường hợp nào trong 4 trường hợp sau
        ```c
            reg = <0x50027000>, <0x4>, <0x500273f0 0x10>;
            reg = <0x50027000 0x4 0x500273f0>, <0x10>;
            reg = <0x50027000>, <0x4 0x500273f0 0x10>;
            reg = <0x50027000 0x4 0x500273f0 0x10>;
        ```
    + Vì vậy cần thuộc tính để định nghĩa đúng format mình muốn
        - `#address-cells = <x>`: chỉ thị số lượng cell để chứa địa chỉ, tức là địa chỉ sẽ rộng 1 cell (32bit)
        - `#size-cells = <y>`: chỉ thị số lượng cell để chứa kích thước của vùng, tức là kích thước vùng nhớ của node con là 1 cell
        - node cha sẽ khai báo 2 thuộc tính này, và node con sẽ tuân theo
        - Ví dụ: 
            ```
            #address-cells = <1>
            #size-cells = <1>
            reg = <0x15 0x4>
            ```
            + address-cells là 1 thì chỉ lấy 1 cell đầu tiên là 0x15
            + size-calls là 1 thì chỉ lấy 1 cell tiếp theo là 0x4
            ```
            #address-cells = <2>
            #size-cells = <1>
            reg = <0x15 0x34 0x4>
            ```
            + address-cells là 2 thì lấy 2 cell đầu tiên là 0x15 0x34
            + size-calls là 1 thì chỉ lấy 1 cell tiếp theo là 0x4
            ```
            spi@300000 {
                #address-cells = <1>;
                #size-cells = <0>;
                flash@1 {
                    reg = <1>;
                };
            };
            ```
- status property
    + chỉ định device có được dùng hay không
    + "okay", "ok", "disabled"
- Resource: interrupts, clocks, DMA, reset lines, ...
    + device tree mô tả bộ điều khiển như 1 node
    + node đó sẽ được dùng trong các node khác
    + ![alt text](images/image-11.png)
        - khi 1 node dùng node khác, cấu hình trong dấu `<>` phải tuân theo cell của node khác
        - ví dụ node khác yêu cầu cell là 3 thì trong `<>` cần có 3 tham số
- generic suffixes - các hậu tố chung
    + `xxx-gpios`   
        - dùng để phân biệt các chân của gpio
        - xxx là tên ví dụ: `power-gpios`, `reset-gpios`, ...
    + `xxx-names`:
        - đặt tên cho các item liên quan
        - cho phép driver tìm kiếm bằng tên thay vì ID
        - thứ tự của `xxx-names` khớp với item
        ```c
        uart0@4000c000 {
            dmas = <&edma 26 0>, <&edma 27 0>;
            dma-names = "tx", "rx";
        };
        ```
            - <&edma 26 0> là "tx"
            - <&edma 27 0> là "rx"
- how to validate Device tree content
    + Dựa vào spec của device tree: `https://www.devicetree.org/specifications/`
    + Dựa vào device tree binding - file mô tả phần cứng: `https://github.com/devicetree-org/dt-schema/tree/main/dtschema/schemas`
- device tree binding
    + các quy tắc về thông số kỹ thuật được viết dạng yaml, dễ dàng đọc, tool dễ phân tích
    + yaml quy định các viết phần cứng trong device tree
    + tức là khi viết device tree, thì file yaml định nghĩa các thuộc tính và format để khai báo
    + bootlin/linux-kernel/linux-kernel-bbb-labs/src/linux/Documentation/devicetree/bindings
- validate device trees
    + `dtc` chỉ verify syntax
    + yaml sẽ validate tính đúng đắn
    + Linux kernel cung cấp lệnh: 
        - `make dt_binding_check`: kiểm tra xem viết đúng cú pháp không
        - `make dtbs_check`: kiểm tra tính đúng đắn
    + 2 lệnh trên chạy cho toàn bộ hệ thống Linux sẽ mất thời gian, thay vào đó giới hạn phạm vi kiểm tra với file yaml cụ thể bằng lệnh: 
        - `make DT_SCHEMA_FILES=Documentation/devicetree/bindings/trivial-devices.yaml dt_binding_check`
        - `make DT_SCHEMA_FILES=Documentation/devicetree/bindings/trivial-devices.yaml dtbs_check`
- Binding syntax: base structure
    + Mỗi file YAML định nghĩa 1 cấp độ phân cấp device tree, tối đa là 2 cấp khi có node con
        - ![alt text](images/image-12.png)
        - `%YAML`: định nghĩa language version
        - `$id`: có thể không phải là URL thực, nhưng là mã định danh duy nhất
        - `$schema`: tham chiếu tới meta-schema mà file này cần được xác thực
        - `property`: nơi bắt đầu định nghĩa, tất cả thuộc tính cần được liệt kê, tên biến viết thường, theo sau là dấu `:`, có 1 dòng trống giữa các property
- Binding syntax: types
    + ![alt text](images/image-13.png)
    + các property cần phải xác định kiểu dữ liệu thông qua `type:` hoặc `ref:`
        - thuộc tính boolean không yêu cầu giá trị
        - giá trị số có thể có dấu hoặc không dấu, và luôn là số 32bit
        - String nên được define
        - Mảng và matric cũng cần thiết
    + Các binding chung đã set type cho nhiều property
        - giá trị/số lượng phần tử của chúng có thể bị giới hạn
        - type không cần phải lặp lại
    + `dt-schema` sẽ áp dụng 1 kiểu dữ liệu bựa vào hậu tố, ví dụ -hz, -ohms, -us
- Binding syntax: child nodes
    + ![alt text](images/image-14.png)
    + child node đơn giản là 1 property
    + type phải set là `type: object`
    + Nếu node con có tên cố định thì khai báo node con trong 1 node child-node
    + Nếu node con có tên không cố định thì khai báo 1 node là `patternProperties`
- Binding syntax: expressing constrains
    + bên cạnh việc định nghĩa chính xác các thuôc tính và type của chúng, giá trị của property cần được giới hạn:
        - tất cả property đều có thể có thêm tham số `description`, để con người đọc
        - tăng ràng buộc để giảm thiểu rủi ro
        - mỗi ràng buộc nằm ở 1 dòng
- Binding syntax: numerical constrains
    + ![alt text](images/image-15.png)
    + giới hạn bằng việc đặt minimum/maximum của giá trị
    + đặt giá trị default cho property
    + đặt minItems/maxItems cho mảng
- Binding syntax: lists and dictionaries
    + ![alt text](images/image-16.png)
    + biểu diễn các giá trị có thể xảy ra, giới hạn giá trị mà property có thể nhận
        - bắt buộc giá trị duy nhất: `const`
        - lấy giá trị từ list: `enum`
    + `const`/`item` có thể được nhóm lại trong 1 `items` list
    + có thể xây dựng điều kiện từ 
        - `oneOf`: biểu thị XOR
        - `anyOf`: biểu thị OR
        - `allOf`: biển thị AND
- Bindings syntax: referencing other bindings
    + ![alt text](images/image-17.png)
    + có thể viết constrains common rồi refer tới nó
    + file common 
        ```
        # File: xe-co-ban.yaml (Đây là file "common" - chứa ràng buộc chung)
        properties:
            so-banh-xe:
                maximum: 4  # Xe thông thường tối đa chỉ có 4 bánh
            co-dong-co:
                type: boolean # Giá trị bắt buộc là Đúng (True) hoặc Sai (False)
            bien-so-xe:
                type: string  # Phải là một chuỗi ký tự chữ và số
        ```
    + file refer tới common 
        ```
        # File: xe-may.yaml (File cấu hình cho một phần cứng cụ thể)
        allOf:
            - $ref: xe-co-ban.yaml  # <--- THAM CHIẾU: Lấy toàn bộ quy tắc của xe cơ bản vào đây

        properties:
            # 1. Ghi đè (Tune/Overwrite) lại thuộc tính chung cho đúng với thực tế của xe máy:
            so-banh-xe:
                const: 2              # Thiết bị chung bảo tối đa 4 bánh, nhưng xe máy bắt buộc phải là 2 bánh!

        # 2. Thêm thuộc tính đặc thù (Specific property) chỉ xe máy mới có:
        dung-tich-xi-nhan:
            type: integer
            maximum: 150          # Ví dụ xe máy thông thường tối đa 150cc
        ```
- Bindings syntax: altering on presence of properties
    + ![alt text](images/image-18.png)
    + đôi khi ta cần các mô tả linh hoạt hơn
        - phụ thuộc giữa các property
            + 1 property có thể cần có property khác
            + nếu cả 2 thuộc tính cùng xuất hiện hoặc cùng không xuất hiện, dependency cần biểu diễn 2 chiều
        - thay đổi constrain dựa theo property
            + biểu diễn bằng if/else dưới `allOf`
            + điển hình là mỗi compatible đi kèm với constrain khác nhau
- Bindings syntax: enforcing correct properties only
    + ![alt text](images/image-19.png)
    + các file YAML liệt kê các property và thêm constrai cho chúng
        - việc thêm các thuộc tính chưa định nghĩa có thể xảy ra
        - việc quên thuộc tính bắt buộc có thể xảy ra
    + cần thêm constrain để phát hiện lỗi chính tả và các property ngoài ý muốn
        - `required`: bắt buộc xuất hiện
        - `additionalProperties: false`: ngăn thuộc tính chưa được define trong file này xuất hiện
        - `unevaluatedProperties`: ngăn thuộc tính chưa được define trong file này xuất hiện hoặc không được refer đến thông qua `allOf`
- Bindings syntax: validating your own bindings
    + ![alt text](images/image-20.png)
    + để test YAML thì thêm `examples` ở cuối file
## Thực hành
- khi thêm file dts thì cần thêm vào Makefile
- build device tree: `make dtbs`
- `model`: đổi tên board
- `make DT_SCHEMA_FILES=misc/nintendo,nunchuk.yaml dt_binding_check`: kiểm tra các examples trong YAML
- Lệnh check cấu trúc device tree trong board
    ```
    dtc -I fs /sys/firmware/devicetree/base/ > /tmp/dts
    grep -C10 nunchuk /tmp/dts
    ```

# Introduction to pin muxing
## What is pin muxing
- Các SoC ngày càng tích hợp nhiều khối hardware, trong số đó có nhiều cái cần giao tiếp với ngoại vi
- Tuy nhiên, kích thước vật lý của chip nhỏ, vì vậy số lượng pin cũng bị giới hạn
- Vì vậy, không phải khối internal hardware nào cũng được đưa ra pin cùng lúc
- các pin được dồn kênh (multiplexed): chúng hiển thị chức năng của khối A hoặc B
- việc multiplexed được xử lý bằng phần mềm
## Pin muxing in the Linux kernel
- `pinctrl` subsystem đã được thêm vào source Linux từ Linux 3.2
- Subsystem này ở `drivers/pinctrl/` cung cấp hệ thống chung để xử lý pin muxing
    + 1 pin muxing driver interface dùng để triển khai driver cho từng loại SoC nhằm cấu hình pin muxing
    + 1 pin muxing consumer interface cho device drivers
- Phần lớn các pinctrl driver cung cấp 1 device tree binding và pin mux phải được mô tả trong device tree: `Documentation/devicetree/bindings/pinctrl`
- pinctrl subsystem diagram
    + ![alt text](images/image-21.png)
- Device Tree properties for consumer devices
    + device cần pin để mux thì dùng thuộc tính `pinctrl-<x>` và `pinctrl-names` trong device tree
        - `pinctrl-<x>`: link tới 1 cấu hình pin cụ thể ứng với state nhất định. `x` được đánh số theo thứ tự trái qua phải nếu `pinctrl-names` có nhiều tên
        - `pinctrl-names`: gán tên gọi cho mỗi trạng thái. Nếu để `default` thì pin mux được thiết lập bởi device driver
        - check `Documentation/devicetree/bindings/pinctrl/pinctrl-bindings.txt` để có thêm chi tiết
- Device Tree properties for consumer devices - Examples
    + ![alt text](images/image-22.png)
    + ![alt text](images/image-23.png)
        - `pinctrl-names` có 2 tên thì `pinctrl-<x>` cũng có 2 cái tương ứng cho từng name
- Defining pinctrl configurations
    + các cấu hình pinctrl khác nhau cần được định nghĩa như node con trong pinctrl device
    + cấu hình có thể được define ở 
        - `.dtsi`: cho cấu hình mà thường dùng chung trong nhiều board
        - `.dts`: dùng cho board cụ thể
    + `pinctrl-<x>` của device trỏ tới cấu hình pin mà nó cần thông qua 1 device tree phandle
    + Ví dụ
        - ![alt text](images/image-24.png)
            + trong AM33xx, driver `pinctrl-single` được sử dụng để cấu hình pin trong DT.
            + trong mỗi cấu hình pin, giá trị của `pinctrl-single,pins` cần đưa ra list của thanh ghi, value
        - để biết giá trị cấu hình chính xác, cần xem datasheet của board và SoC
## Thực hành
- i2c không có khả năng tự dò tìm
- i2cdetect sẽ bypass bằng cách đóng vai là master và gửi tín hiệu tới các device. i2cdetect quét từ 0x3 tới 0x77 rồi xem device nào phản hồi. Tuy nhiên i2cdetect có thể làm nhiễu loạn
- output của i2cdetect: 
    + UU: device đã được điều khiển bởi 1 driver
    + hiện số: device chưa có driver
- trong bảng P8, P9, cột Pin ở gần cuối, ký hiệu đó được dùng để kiểm tra pin assignment
- FIle check enum tương ứng với các mode, pin name, ...: `/home/as/Desktop/linuxEmbeddedBBB/BBB_docs/hw-docs/am33xx.h`

# Linux device and driver model
## Introduction
- The need for a device model
    + Linux kernel chay trên nhiều kiến trúc phần cứng, vì vậy cần tối ưu việc tái sử dụng code giữa các nền tảng
    + Ví dụ, ta muốn 1 USB device driver có thể dùng đươc ở x86, arm, ...
    + Điều này yêu cầu code được tổ chức rõ ràng, với device driver tách biệt khỏi controller driver, mô tả phần cứng tách biệt khỏi driver của chúng, ...
    + Đây là điều mà linux kernel device model cho phép.
- Kernel and device drivers
    + trong Linux, 1 driver luôn giao tiếp với
        - 1 framework mà cho phép driver đó expose tính năng phần cứng theo cách chung nhất
        - 1 kiến trúc bus để giao tiếp, phát hiện phần cứng
- Device model data structures
    + device model được tổ chức quanh 3 struct chính
        - `struct bus_type`: thể hiện loại bus (SPI, I2C, USB, ...)
        - `struct device_driver`: thể hiện 1 driver có khả năng điều khiển các device cụ thể trong 1 bus cụ thể
        - `struct device`: thể hiện 1 device kết nối tới 1 bus
    + kernel dùng tính chất kế thừa để tạo nhiều phiên bản khác nhau của `struct device_driver` và `struct device` cho mỗi bus subsystem
- Bus drivers
    + Component đầu tiên của device model là bus driver
        - mỗi bus driver đại diện cho 1 loại bus: USB, SPI, I2C, ...
    + Bus driver chịu trách nhiệm
        - đăng ký loại bus (`struct bus_type`)
        - cho phép đăng ký adapter driver (USB controller, I2C adapter, ...) tương thích để phát hiện được device đã kết nối và cung cấp cơ chế giao tiếp với các device
        - cho phép đăng kí device drivers (USB drivers, I2C devices, ...)
        - matching device driver với device bằng adapter driver
        - cung cấp API để implement adapter drivers và device driver
        - định nghĩa driver và struct của device thông qua `struct usb_driver` và `struct usb_interface`
- sysfs
    + bus, device, driver, ... là các struct nội bộ của kernel
    + sysfs virtual filesystem cung cấp cơ chế để export nhiều thông tin tới user space 
    + thường được mount vào `/sysfs`
        - `/sys/bus`: chứa danh sách các loại bus
        - `/sys/devices`: chứa danh sách các device
        - `/sys/class`: liệt kê các thiết bị theo framework mà chúng được kết nối vào
## Example of the USB bus
- Kiến trúc USB bus driver nằm ở 
    - chứa code cho kiến trúc bus USB
    - `drivers/usb/core`
    - `struct bus_type` được định nghĩa trong `drivers/usb/core/driver.c` và đăng ký trong `driver/usb/core/usb.c`
- Adapter drives
    - chứa các driver điều khiển phần cứng gắn vào bus USB
    - `drivers/usb/core`
- Device drivers
    - đặt nhiều nơi trong kernel source 
- Quan sát ví dụ về `drivers/net/usb/rtl8150.c`
- Device identifiers
    + Việc định nghĩa danh sách device mà driver này có thể quản lý, giúp cho USB core biết được với device nào thì driver nào nên được dùng
    + macro `MODULE_DEVICE_TABLE()` cho phép `depmod` (run bởi `make_modules_install`) trích xuất mối quan hệ giữa device identifiers và drivers, vì vậy driver có thể được load tự động bởi `udev`
- Instantiation of usb_driver
    + `struct usb_driver` là struct được định nghĩa bởi USB core. Mỗi USB device driver phải khởi tạo nó, và đăng kí nó với USB core sử dụng struct này
    + struct này kế thừa từ `struct device_driver`, cái mà được định nghĩa bởi device model
- Driver registration and unregistration 
    + khi driver được load/unload, nó phải được đăng ký/ hủy đăng ký nó khởi USB core
    + dùng `usb_register()`, `usb_deregister()` trong USB core `drivers/net/usb/rtl8150.c`
    + với code driver của drivers/net/usb/rtl8150.c, chỉ cần gọi `module_usb_driver()` là hàm init với exit được gọi
- Tại thời điểm khởi tạo, USB adapter driver liên quan tới USB controller đăng ký chính nó tới USB core
- `rtl8150` usb device driver đăng ký nó tới USB core
    + ![alt text](images/image-25.png)
    + USB core bây giờ biết sự kết hợp giữa vendor/productID của `rtl8150` và `struct usb_driver` của driver này
- Khi 1 device được phát hiện
    + USB core sẽ tìm kiếm IDs đã đăng ký và tìm driver tương ứng
    + sau đó USB core gọi hàm probe() để đăng ký driver `rtl8150`
- Probe method
    + được phát đi cho mỗi device kết nối với 1 driver
    + hàm `probe()` nhận tham số là 1 struct mô tả device, thường cụ thể bởi bus infrastructure (`struct pci_dev, struct usb_interface`, ...)
    + function này chịu trách nhiêm cho
        - khởi tạo device, mapping I/O memory, đăng ký interrupt. Bus infrasstructure cung cấp phương thức để lấy địa chỉ, số ngắt và thông tin khác của device
        - đăng ký device tới chính xác kernel framework (example: network infrastructure)
- Mô hình thiết bị của Linux mang tính đệ quy
    + bus này có thể kết nối và quản lý các bus khác, tạo thành cấu trúc phân tầng phức tạp 
    + ![alt text](images/image-26.png)

## Platform drivers
- trong số các device non-discoverable, có nhiều họ device là 1 phần trong SoC: UART controller, Ethernet controller, SPI hoặc I2C controller, graphic, ...
- Trong linux kernel, 1 bus cụ thể được gọi là `platform bus`, được tạo ra để handle những device này. Chúng được control qua `memory-mapped registers`, vì các device này không thể tự phát hiện được
- Platform bus hỗ trợ platform drivers mà handle platform devices
    + là trung gian kết nối platform driver và platform device
    + làm việc như các bus khác, ngoại trừ các device được liệt kê 1 cách cố định thay vì được phát hiện
- Implement of a platform driver
    + driver này cần implement 1 `struct platform_driver`
        - ![alt text](images/image-27.png)
    + và sau đó đăng ký struct đó trong hàm init, exit
        - ![alt text](images/image-28.png)
    + phần lớn các driver dùng hàm `module_platform_driver()` khi không cần làm gì đặc biệt trong hàm init và exit
        - ![alt text](images/image-29.png)
- Platform device instantiation
    + vì platform devices không thể được phát hiện tự động, chúng được định nghĩa cố định
        - cách cũ: khởi tạo trực tiếp từ `struct platform_device`
        - cách mới: mô tả qua device tree, từ đó các `struct platform_device` sẽ được tạo
- Using additional hardware resources - cách mà kernel sử dụng tài nguyên phần cứng bổ sung
    + Device tree thường chứa nhiều thông tin. Nó chứa các con trỏ trỏ tới các khối phần cứng không thể discoverable được   
    + Cách mà driver lấy thông tin phần cứng
        - Địa chỉ các thanh ghi I/O (để ghi/đọc dữ liệu phần cứng) và các đường tín hiệu ngắt (IRQ) được tự động chuyển đổi thành một mảng các `struct resource` gắn liền với `struct platform_device` đó. Driver chỉ cần gọi hàm chuẩn để lấy chúng ra.
        - Thông tin liên qua tới subsystem nào sẽ được phân tích bởi subsystem đó
        - Các thuộc tính được lấy trực tiếp bởi device driver, thông qua các hàm lookup của device tree
    + tất cả cách này cho phép cùng 1 driver được sử dụng với nhiều devices có chức năng tương đương, nhưng khác address
- Using resources
    + platform driver truy cập vào các tài nguyên được cung cấp bởi platform bus cũng như các thành phần khác do subsysmtem cung cấp thông qua API: `clk_get()`, `gpio_request()`, `dma_request_channel()`
        ```c
        res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        sport->rxirq = platform_get_irq(pdev, 0);
        ```
- Driver data
    + Ngoài thông tin và tài nguyên của device, driver có thể cần thêm các thông tin cụ thể khác của device để thay đổi hành vi khi driver điều khiển nhiều device cùng lúc
    + 1 con trỏ `const void *data` dùng để chứa thông tin đặc biệt này
        ```c
        static const struct of_device_id marvell_nfc_of_ids[] = {
            {
                .compatible = "marvell,armada-8k-nand-controller",
                .data = &marvell_armada_8k_nfc_caps,
            },
        };
        ```
    + Data này có thể lấy được trong hàm `probe` bằng cách
        ```c
            nfc->caps = of_device_get_match_data(&pdev->dev);
        ```

# Introduction to the I2C subsystem
## What is I2C?
+ Là bus tốc độ thấp sử dụng phổ biến để kết nối device onboard và ngoại vi tới vi xử lý
+ Sử dụng 2 dây: SDA cho data và SCL cho clock
+ I2C là 1 master/slave bus: chỉ master có thể khởi tạo việc truyền dữ liệu, và các slave chỉ có thể reply tới việc truyển dữ liệu của master
+ Ở linux system, I2C controller trong vi xử lý là master, điều khiển bus
+ Mỗi slave device được xác định bởi 1 I2C address (không thể có 2 device có cùng address trên cùng 1 bus). Mỗi giao tiếp được khởi tạo bởi master chứa địa chỉ này, cho phép các slave liên quan nhận biết rằng nó có nên rep lại giao tiếp này không
## An I2C bus example
+ ![alt text](images/image-30.png)
## The I2C bus driver
+ Như các bus subsystem khác, I2C bus driver chịu trách nhiệm
    - cung cấp API để implement I2C controller drivers
    - cung cấp API để implement I2C device driver trong kernel space
    - cung cấp API để implement I2C device driver trong user space
+ Code của I2C bus driver nằm ở `drivers/i2c/`
+ Code của I2C controller driver nằm ở `driver/i2c/busses/`
+ Code của I2C device driver nằm rải rác trong `drivers/`, phụ thuộc vào framework được dùng để expose devices
## Registering an I2C device driver
+ Như các bus subsystem khác, I2C subsystem định nghĩa `struct i2c_driver` kế thừa từ `struct device_driver` và cần phải khởi tạo và đăng ký bởi mỗi I2C device driver
    - thông thường, struct này trỏ tới hàm probe() và remove()
    - nó cũng chứa `id_table`, dùng cho việc phát hiện I2C devices mà không dựa vào device tree
+ `i2c_add_driver()` và `i2c_del_driver()` được dùng để register/inregister driver
+ nếu driver không làm gì trong init()/exit() thì có thể dùng `module_i2c_driver()` thay thế
## Registering an I2C device driver: example
+ ![alt text](images/image-31.png)
## Registering an I2C device: non-DT
+ Ở platform không dùng device tree, `struct i2c_board_info` cho phép mô tả cách mà I2C device kết nối tới board
+ Các struct này được định nghĩa với macro `I2C_BOARD_INFO()`, có param là device name và địa chỉ của slave device
+ mảng các struct này được đăng ký vào cho mỗi bus bằng việc dùng `i2c_register_board_info()`
## Registering an I2C device, non-DT example
+ ![alt text](images/image-32.png)
## Registering an I2C device, in the DT
+ trong device tree, I2C controller device được định nghĩa ở file .dtsi, và thường status = "disabled"
+ tại device tree của board:
    - I2C controller device được enabled
    - tần số bus I2C được khai báo, dùng `clock-frequency`
    - I2C device được mô tả là node con trong I2C controller bus, nơi mà `reg` được đăng ký là địa chỉ của slave trên bus
+ example
    - khai báo I2C controller
        + ![alt text](images/image-34.png)
        + mô tả các thông tin cần thiết cho bus I2C
    - khai báo I2C device
        + ![alt text](images/image-35.png)
        + mô tả các thông tin của device gắn vào bus I2C
## probe() and remove()
+ probe() có nhiệm vụ khởi tạo device và đăng ký nó vào kernel phù hợp, nó nhận các tham số
    - `struct i2c_client` pointer, cái mà đại diện cho chính I2C device mà device tree truyền vào, kế thừa từ `struct device`
    - với Linux kernel < 6.4, probe() có tham số thứ 2 không được sử dụng
    - ![alt text](images/image-36.png)
+ remove() có nhiệm vụ hủy đăng ký device khroi kernel và shut it down
    - ![alt text](images/image-37.png)
## Communicating with the I2C device: raw API
+ Gửi 1 `buf` tới I2C device
    - `int i2c_master_send(const struct i2c_client *client, const char *buf, int count);`
+ Nhận 1 `count` bytes từ I2C device và lưu vào `buf`
    - `int i2c_master_recv(const struct i2c_client *client, char *buf, int count);`
+ cả 2 function trên trả về số âm báo lỗi, hoặc là số byte truyền được
+ giữa quá trình ghi và đọc cần có khoảng delay để dữ liệu kịp ghi được vào
## Communicating with the I2C device: message transfer
+ API gửi message cho phép mô tả thông tin truyền đi bao gồm vài messages, với mỗi message được tryền đi theo 1 hướng
    - `int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num);`
        + `struct i2c_adapter` pointer được lấy thông qua `client->adapter`
        + `struct i2c_msg` định nghĩa length, location và direction của message
## I2C: message transfer example
+ ![alt text](images/image-38.png)
## I2C functionality
+ không phải tất cả I2C đều hỗ trợ tất cả tính năng 
+ I2C controller driver bảo với I2C core rằng chức năng nào chúng hỗ trợ
+ 1 I2C device driver phải kiểm tra chức năng nào chúng được cấp vởi I2C controller
+ `i2c_check_functionality()` dùng để check việc này
+ `include/uapi/linux/i2c.h` chứa define các chức năng tồn tại trong I2C
+ Tài liệu Linux support for I2C: https://www.kernel.org/doc/html/latest/i2c/
## Thực hành
- Trong folder /dev chứa 
    + terminal device ttyX: giao diện người dùng nhận input và xuất output 
    + mmcblk: bộ nhớ mmc và các phân vùng
    + sdX: device như usb
- Trong folder /sys chứa
    + /sys/class: expose các device được phân loại bởi kernel quản lý chúng. 
        - /sys/class/net chứa các network interface của hệ thống
        - /sys/class/thermal: chứa thông tin về nhiệt độ của system
    + /sys/bus: chứa tất cả bus của hệ thống
- COde với i2c thì #include <linux/i2c.h>
- `MODULE_DEVICE_TABLE`: 
    + export thông tin của device id ra user space để tự động load kernel module mỗi khi thiết bị cắm vào, k cần gọi modprobe hay insmod
    + Nếu k dùng macro này thì phải insmod module thủ công dù device đã được cắm vào, macro này sinh ra modalias trong .ko để udev nhận event rồi load module tương ứng (check bằng modinfo)
- sau khi probe() được gọi, driver nunchuk sẽ nằm trong `/sys/bus/i2c/driver/nunchuk`
- `device_property_read_bool`: kiểm tra giá trị 1 property trong device tree
- để khởi tạo được nunchuk, cần gửi `(các mã sau chỉ dành riêng cho nunchuk)`
    + lần đầu 0xf0 và 0x55 để tắt tính năng mã hóa của nunchuk, ghi 0x55 vào thanh ghi 0xf0 để tắt mã hóa
    + tiếp theo gửi 0xfb và 0x00 để reset dữ liệu của nunchuk về raw data
    + ghi 0x00 để yêu cầu nunchuk cập nhật giá trị hiện tại của các nút nhấn
- không được include các header trong asm/

# Kernel framework for device drivers
## Kernel and Device drivers
- Trong linux, 1 driver luôn giao tiếp với: 
    + 1 framework: cho phép driver expose tính năng phần cứng tới user space app
    + 1 bus: 1 phần của device model, để phát hiện và giao tiếp với hardware
- ![alt text](images/image-39.png)
- Section này cover về kernel framwork
## User space vision of devices
- Types of devices
    + Trong linux, có 4 loại device:    
        - Network devices: truy cập trong user space bằng `ip a`
        - Block devices: cung cấp cho user space khả năng truy cập vào bộ nhớ USB, hard disk, chúng được truy cập qua `/dev`
        - Character devices: cung cấp cho user space khả năng truy cập vào các device (input, sound, graphic, ...), chúng được truy cập qua `/dev`
        - Sysfs devices: nằm trong `/sys`
## Devices: everything is a file
- phần lớn các system objects đều là files
- cho phép app open, read, write, close, ...
- vì vậy device cần phải thể hiện qua file để app có thể truy cập -> device file
- tất cả device file được chứa trong `/dev`
- mỗi device file có 3 thông só:
    + type: char or block
    + major: class of device
    + minor: identifier in class
## Creating device files
- Trước Linux 2.6.32, device file cần được tạo thủ công bằng `mknod`
- `devtmpfs` virtual filesystem có thể mount vào `/dev` và chứa tất cả device đã đăng ký với kernel. Thuộc tính `CONFIG_DEVTMPFS_MOUNT` trong Kernel dùng để mount tự động vùng này lúc boot, ngoại trừ boot với `initramfs`
- `devtmpfs` có thể được hỗ trợ thêm bởi `udev` hoặc `mdev` để điều chỉnh permisssion/ownership, load kernel modules tự động và tạo symbol link tới devices
## Character drivers
- A character driver in the kernel
    + từ góc nhìn của app, character device là 1 file
    + Vì vậy character device implement các operations để khiến cho app nghĩ rằng device đó là 1 file
    + Để làm được vậy, character driver cần implement các hàm read, write, ... của `struct file_operations`
    + Linux filesystem layer sẽ đảm bảo operation của driver được gọi bởi user space app 
- From user space to the kernel: character devices
    + ![alt text](images/image-40.png)
- open() and release()
    + `int open(struct inode *i, struct file *f)`
        - gọi khi user space mở file
        - `struct inode`: đại diện cho 1 file trong filesystem (có thể là 1 file bình thường, 1 đường dẫn, symbol link, character device hoặc block device)
        - `struct file`: là struct được tạo mỗi lần mở file, nhiều struct file có thể cùng trỏ tới cùng inode structure
            + chứa thông tin như position, opening mode, ...
            + `void *private_data`
            + pointer trỏ vào file structure được pass vào tất cả operations
    + `int release(struct inode *i, struct file *f)`
        - gọi khi user space đóng file
- read() and write()
    + `ssize_t read(struct file *f, char __user *buf, size_t sz, loff_t *off)`
        - đọc data từ device, ghi tối đa `sz` bytes vào `buf` và cập nhật current position trong `off`
        - return về số bytes đọc được. Return 0 thường được user space hiểu là kết thúc file
        - trong UNIX, read() thường bị block khi không có đủ data để đọc từ device
    + `ssize_t write(struct file *f, const char __user *buf, size_t sz, loff_t *off)`
        - đọc `sz` bytes từ `buf` rồi ghi data đó vào file, cần cập nhật lại position `off`
- Exchanging data with user space 
    + Kernel không cho phép truy cập trực tiếp vào user space memory, không cho phép memcpy() hay trỏ trực tiếp vào
        - Việc trỏ vào user space bị vô hiệu quá để khiến việc khai thác lỗ hổng khó hơn
        - Nếu địa chỉ truyền vào bởi app không hợp lệ, kernel có thể bị lỗi segfault
        - Không bao giờ tin tưởng user space, user space app có thể truyển 1 kernel address mà kernel space đang muốn đọc hoặc ghi data
    + Để giữ code kernel có tính linh động (portable), bảo mật và xử lý lỗi chính xác, driver cần dùng function đặc biệt để trao đổi data với user space
    + A single value:
        - `get_user(v, p)`: biến `v` của kernel lấy value của user space pointer `p`
        - `put_user(v, p)`: giá trị của user space pointer `p` được set bằng giá trị của `v`
    + A buffer
        - `unsigned long copy_to_user(void __user *to, const void *from, unsigned long n);`
        - `unsigned long copy_from_user(void *to, const void __user *from, unsigned long n);`
        - return 0: success
        - return khác 0: error, thường `return -EFAULT`
    + ![alt text](images/image-41.png)
- Zero copy access to user memory
    + việc phải copy data tới hoặc từ 1 bộ đệm kernel trung gian có thể trở nên khó khăn nếu data quá lớn (như video chẳng hạn)
    + Zero copy là cơ chế giúp hệ thống truyền tải dữ liệu thẳng từ nơi này qua nơi khác mà không cần tốn thời gian copy qua lại bộ đệm trung gian của hệ điều hành
    + Các tùy chọn `zero copy`
        - `mmap()`: là system call cho phép user space truy cập trực tiếp vào memory mapped I/O space
        - `get_user_pages()` và các function liên quan khác để lấy mapping tới user pages mà không cần copy chúng
- unlocked_ioctl()
    + `long unlocked_ioctl(struct file *f, unsigned int cmd, unsigned long arg)`
    + đây là hàm để gửi lệnh điều khiển từ user space xuống kernel space thông qua device file rồi tới phần cứng 
    + liên quan tới `ioctl()` nhưng `ioctl()` có nhược điểm là block các tiến trình khác
    + mở rộng khả năng của driver ngoài các API đọc ghi
    + Ví dụ như: thay đổi tốc độ của serial port, cài đặt output format video, querying device serial number, ... được dùng rộng rãi trong các framework driver như ALSA(sound), V4L2(video)
    + `cmd` là 1 số định danh cho biết thao tác nào cần được thực hiện
    + `arg` là tùy chọn pass vào tham số thứ 3 của `ioctl(), có thể là số nguyên, địa chỉ, ...
    + `cmd` và `arg` tùy theo định nghĩa của từng driver
- `ioctl()` example
    + kernel space
        - ![alt text](images/image-42.png)
    + user space 
        - ![alt text](images/image-43.png)
## The concept of kernel frameworks
- Beyond character drivers: kernel framework - hơn cả character driver chính là kernel framework
    + nhiều device driver không được implement trực tiếp như character drivers
    + Thay vào đó chúng triển khai `struct file_operations` 1 lần và các driver cho cùng loại device truy cập vào để thực hiện tương tác với file
    + chúng được implement dưới dạng 1 framework, dành cho 1 loại device cụ thể (framwbuffer, V4L, ....)
        - driver kết nối vào 1 framework API hay vì vào `struct file_operations`
        - framework triển khai `struct file_operations` 1 lần để expose character devices tới user space
        - Việc triển khai này được chia sẻ cho tất cả device trong framework, tránh lặp lại code và nhất quán về user interface bất kể driver nào đang được dùng
- Example: Some kernel frameworks
    + ![alt text](images/image-44.png)
    + `struct file_operations` được implement trong các phần core
## Example: the input subsystem
- What is the input subsystem?
    + Có nhiệm vụ quản lý tất cả sự kiện input tới từ user
    + được chia thành 2 phần:
        - device drivers: trao đổi với hardware, cung cấp events tới các input
        - Event handlers: lấy events từ driver và pass vào nới cần qua các interface (hầu hết là qua `evdev`)
    + Trong user space, nó thường được dùng bởi graphic stack như X.Org, Wayland, ...
- Input subsystem diagram
    + ![alt text](images/image-45.png)
- Input subsystem overview
    + Kernel option `CONFIG_INPUT`
        - `menuconfig INPUT`
            + `tristate "Generic input layer (needed for keyboard, mouse, ...)"`
    + Implemented in `drivers/input`
        - input.c
        - input-poller.c
        - evdev.c
    + Defines the user/kernel API
        - `include/uapi/linux/input.h`
    + Danh sách các operation của 1 input driver cần được implement và các helper function cần cho driver
        - `struct input_dev` cho device driver part
        - `struct input_handler` cho event handler part
        - `include/linux/input.h`
- Input subsystem API
    + 1 input device được mô tả bởi `struct input_dev`
    + trước khi sử dụng, struct này cần được cấp phát và khởi tạo
        - `struct input_dev *devm_input_allocate_device(struct device *dev);`
        - return -ENOMEM nếu không cấp phát được bộ nhớ cho device
        - gọi hàm này để đăng ký device với system, nếu không thì system không thể tạo ra device file trong /dev/input
    + dựa vào loại event được tạo ra, input bit field `evbit` và `keybit` cần được cấu hình
        + `evbit`: loại sự kiện của nút nhấn (nhấn/thả/cảm ứng)
        + `keybit`: tên của nút bấm 
        ```c
        set_bit(EV_KEY, myinput_dev.evbit);
        set_bit(BTN_0, myinput_dev.keybit);
        ```
    + Khi input device đã được cấu hình, hãy đăng ký nó
        - `int input_register_device(struct input_dev *);`
        - đưa device vào danh sách quản lý của system
    + Những event này được gửi bởi driver tới event handler bằng 
        - `void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)`
        - các hàm này dùng để trả data từ input lên 
        - document của event types: `input/event-codes`
        - 1 event được hình thành từ sự thay đổi data đầu vào như trạng thái nút bấm, axis, ...
        - Input subsystem cung cấp các wrapper của `void input_event` như
            + `input_report_key()`
            + `input_report_abs()`
    + Sau khi đã submit các event, cần thông báo cho input core để sync data bằng cách gọi: 
        - `void input_sync(struct input_dev *dev)`
    + Example from drivers/hid/usbhid/usbmouse.c
        - ![alt text](images/image-46.png)
- Polling input devices
    + Input subsystem cung cấp 1 API để hỗ trợ các input devices không tự raise ngắt mà cần phải scan định kỳ để phát hiện thay đổi state. Tức là các input device này khi thay đổi state thì không tự tạo ngắt để thông báo mà ta cần phải tự cập nhật lại nó
    + `int input_setup_polling(struct input_dev *dev, void (*poll_fn)(struct input_dev *dev))`
        - `poll_fn`: là function mà sẽ được gọi định kì để check
    + chu kỳ polling được set bằng `input_set_poll_interval()` hoặc `input_set_min_poll_interval()` và `input_set_max_poll_interval()`
- evdev user space interface
    + main user space interface kết nối tới input devices là event interface
    + mỗi input devices được đại diện bằng 1 character device `/dev/input/event<X>`
    + 1 user space app có thể blocking read hoặc non-blocking read, cũng có thể dùng `select()` để nhận event sau khi mở device
    + Mỗi lần đọc sẽ return về `struct input_event` theo format sau:
        ```c
        struct input_event {
            struct timeval time;
            unsigned short type;
            unsigned short code;
            unsigned int value;
        };
        ```
    + Công cụ để test input device là `evtest`: `https://cgit.freedesktop.org/evtest/`
## Device-managed allocations
- Device managed allocations
    + hàm `probe()` chịu trách nhiệm cấp phát 1 số tài nguyên đặc biệt: memory, mapping I/O registers, registering interrupt handlers, ...
    + Những resource này cần được giải phóng chính xác trong `probe()` khi gặp lỗi và `remove()`
    + Việc giải phóng tài nguyên này cần nhiều code để xử lý lỗi, vì vậy `device managed allocations` ra đời
    + Ý tưởng là kết hợp việc cấp phát tài nguyên với `struct device` và tự động giải phóng khi device biến mất hoặc khi device hủy liên kết với driver
    + những function để tự động giải phóng tài nguyên có prefix là `devm_`: `devm_kmalloc()`, `devm_ioremap()`, xem thêm các hàm tại `https://www.kernel.org/doc/html/latest/driver-api/driver-model/devres.html`
- Device managed allocations: memory allocation example
    + ![alt text](images/image-47.png)
- Device managed allocations caveats
    + Việc dọn dẹp hoàn tất khi `struct device` được dọn xong, không còn reference gì cả
    + Không dùng các hàm `devm_` cho các vùng bộ nhớ nào mà các thành phần ngoài driver có thể chạm tới. Ví dụ, nếu user space vẫn đang mờ file mà device bị rút ra, remove() sẽ chạy và devm sẽ giải phóng vùng nhớ. User space không kịp đóng file và sẽ gây crash vì truy cập vùng bộ nhớ đã mất
    + Nếu dùng A reference B và B cũng nắm 1 reference của A và dùng `devm` thì hệ thống không biết cần giải phóng cái nào trước và tài nguyên sẽ bị kẹt lại, không bao giờ được xóa
    + Vì vậy khi dùng các hàm quản lý memory `devm_` thì cần chú ý vòng đời của object
## Driver data structures and links
- Driver data layout Three main data structures
    + Khi viết driver, cần tổ chức và quản lý dữ liệu của 1 device hợp lý. VÌ vây cần thiết kế mô hình gồm 3 tầng cấu trúc để giải quyết 3 bài toán: 
    + Bus-specific device structure (`struct i2c_client`, `struct usb_dev`, ...) - quản lý cách thiết bị giao tiếp với bus nào
        - struct đại diện cho device theo loại bus
        - những struct luôn có `struct device` bên trong
        - khi probe() chạy, con trỏ trỏ tới những struct này sẽ được truyền vào probe()
    + Framework-specific device struct (`struct input_dev`, `struct rtc_device`, ...) - để biết device này dùng để làm gì
        - có thể có `struct device` bên trong nếu framework muốn tạo ra device trong sysfs
        - với 1 device, có thể có đồng thời 1 `struct device` và 1 `struct device`
    + Driver **private data** - trạng thái riêng của device, driver
        - là struct tự mình định nghĩa, chứa các thông tin cần thiết
        - phụ thuộc vào driver cụ thể
        - nó chứa reference tới bus và framwork device 
        - lưu data riêng của driver
- Driver data allocation strategies
    + các chiến lược để tổ chức bộ nhớ RAM cho data của driver khi viết driver
    + Private data được nhúng vào framwork device vì vậy 1 quá trình cấp phát sẽ cấp phát cho cả framework device và private data
        - ![alt text](images/image-48.png)
        - framework device là các struct như `struct input_dev`, ... tức là nó đại diện chung cho 1 tập hợp các thiết bị có cùng kiểu chức năng input
    + framework cung cấp các hàm để cấp phát framework device, với 1 khoảng trống ở cuối để đặt private data vào đó
        - ![alt text](images/image-49.png)
    + framework device và private data được cấp phát riêng biệt
        - ![alt text](images/image-50.png)
- Phân biệt bus device và framework device
    + bus device: là thiết bị đứng ở góc độ giao tiếp phần cứng, được kết nối vào board bằng bus: i2c bus, usb bus, ...
    + framework device: là đại diện cho device ở góc độ chức năng đối với hệ điều hành. Tức là sau khi kết nối qua bus rồi thì device này làm được nhiệm vụ gì
    + 1 device khi cắm vào thì hệ điều hành sẽ tạo ra 1 bus device và 1 framework device cho nó
- Links between data structures
    + trong các hàm callback của bus, ta sẽ pass bus device
        - trong `struct device`, 1 trường có kích thước của con trỏ là `dev->driver_data` được dành cho driver sử dụng
        - dùng `dev_set_drvdata()` trong `probe()` để đưa 1 tham chiếu tới private data
        - từ các hàm callback của bus, ta lấy private data bằng `dev_get_drvdata()`
    + trong các hàm callback của framework, ta sẽ pass framework device
        - nếu framework device nhúng trong private data thì dùng `container_of()`, hoạt động dựa trên lệnh `offsetof()`
        - ngược lại, dùng framework device `dev->driver_data` và lấy private data reference
    + ![alt text](images/image-51.png)
## Thực hành
- `CONFIG_INPUT_EVDEV`: option kernel để tích hợp evdev vào kernel
- đầu tiên cần add 1 input device vào system trong nunchuk.c
    + thêm con trỏ `struct input_dev` trong probe(). Không để struct này global vì driver cần được dùng cho nhiều device nên mỗi khi probe được gọi, 1 input_dev mới dành cho 1 device mới được tạo
    + gọi `devm_input_allocate_device`
    + khi dùng `devm_input_allocate_device` thì không cần xử lý unregister device trong remove vi devm_ đã tự làm, thậm chí cũng không cần unregister cho `struct input_dev`
    + interval cần set sau khi đăng ký polling
    + `input_set_drvdata`: gắn con trỏ i2c dev cho input_dev (tạo struct trung gian chứa con trỏ để polling đoc được)
    + `input_register_device` cần được gọi cuối cùng sau khi đã thiết lập các thứ liên quan input
    + dùng evtest để test input

# Memory Management
## Physical and virtual memory
- ![alt text](images/image-52.png)
- trong không gian địa chỉ ảo, mỗi process luôn có 1 không gian ảo riêng
- Nhờ có địa chỉ ảo mà các process có thể dùng chung các dải địa chỉ ảo giống nhau mà không sợ bị ghi đè
- CPU luôn thấy địa chỉ ảo 
- MMU mapping địa chỉ ảo và địa chỉ thật
## Virtual memory organization
- ![alt text](images/image-53.png)
- `PAGE_OFFSET`: là mốc phân chia giữa kernel space và user space 
- Vùng kernel:
    + chứa kernel code
    + cấp phát vùng nhớ cho các kernel module
    + ánh xạ vật lý toàn bộ kernel
    + giống nhau ở mọi process: vùng kernel này của các process luôn trỏ về cùng 1 vị trí vật lý duy nhất của kernel để đảm bảo nhất quán
- Vùng Process n
    + chứa code và data của process
    + memory-mapped files
    + mỗi process có không gian địa chỉ riêng biệt
- Log của virtual mapping này được in ra từ đầu khi khởi động
## Physical/virtual memory mapping on 32-bit systems
- ![alt text](images/image-54.png)
- trong hệ thống 32bit, kernel space bị giới hạn 1GB memory. Nếu có nhiều bộ nhớ vật lý hơn, phần còn lại của memory sẽ không được truy cập bởi kernel space, chỉ có thể dùng trong user space
- Dù nhiều process nhưng chúng đều chỉ truy cập vào 1GB bộ nhớ kernel này
- Để cho phép kernel truy cập được nhiều hơn:
    + cấu hình `CONFIG_VMSPLIT_2G` hoặc `CONFIG_VMSPLIT_1G` -> giảm user space memory của mỗi process
    + bật `highmem` support nếu kiến trúc hỗ trợ
- Nhược điểm là kernel nếu mapping 1:1 địa chỉ ảo và vật lý thì chỉ map được khoảng 1GB trong RAM, phần còn lại kernel không thể đụng tới được, không có địa chỉ ảo nào map vào được vùng bộ nhớ còn lại
## Physical/virtual memory mapping on 64-bit systems (4kiB-pages)
- ![alt text](images/image-55.png)
- trong hệ thống 64bit, kernel có tới 256TiB memory để map địa chỉ, lớn hơn RAM thực tế dùng trên thiết bị nên kernel có thể nhìn thấy toàn bộ vùng nhớ RAM
- giữa kernel và process có 16M TiB trống để tách biệt 2 vùng này
## User space virtual address space
- Khi 1 process start, executable code được load vòa ram và được map vào không gian địa chỉ ảo
- Trong quá trình thực thi, việc mapping memory có thể được thực hiện thêm như:
    + memory allocations
    + memory mapped files
    + mmap areas
    + ...
## Userspace memory allocations
- Userspace mapping có thể nhắm tới toàn bộ memory, có nghĩa là userspace được cấp bộ nhớ từ bất kỳ vùng nào còn trống trong memory
- Khi cấp phát bộ nhớ, memory có thể chưa được cấp phát trên phần cứng thật
    + Kernel dùng tín hiệu Fault paging để cấp phát physical page. Physical page được cấp phát khi truy cập vào virtual address tạo ra page fault
    + hoặc có thể memory bị swap (cất đi tạm thời)
    + `mlock/mlockall` ra lệnh cho kernel cấp bộ nhớ RAM thực ngay lập tức 
- Việc cấp phát memory cho user space được cho phép cấp quá mức (lớn hơn RAM vật lý) -> có thể dẫn tới tràn memory
    + có thể cấu hình `/proc/sys/vm/overcommit_*` để quản lý việc cấp bộ nhớ này
- Khi tràn bộ nhớ, OOM killer sẽ kill 1 số process
## Kernel memory allocators
- ![alt text](images/image-56.png)
## Page allocator
- phù hợp cho cấp phát medium size
- Kernel thường quản lý bộ nhớ theo page, 1 page thường là 4K, nhưng có thể lớn hơn ở kiến trúc khác nhau
- Để giảm việc quản lý phúc tạp, kernel dùng thuật toán Buddy, ép việc cấp phát phải là lũy thừ của 2 (1 page, 2 page, 4 page, 8 page, ....)
- kích thước tối đa thường là 8192KB, nhưng có thể dựa vào cấu hình kernel
- việc cấp phát bộ nhớ này liên tục nhau về mặt vật lý
## Page allocator API
- dùng ở kernel space
- `unsigned long get_zeroed_page(gfp_t gfp_mask)`
    + return về địa chỉ ảo của free page (vùng nhớ đang trống), khởi tạo giá trị 0 cho vùng nhớ
- `unsigned long __get_free_page(gfp_t gfp_mask)`
    + tương tự nhưng không khởi tạo giá trị 0 
- `unsigned long __get_free_pages(gfp_t gfp_mask, unsigned int order)`
    + return về địa chỉ áo bắt đầu của vùng chứa các page có địa chỉ vật lý nối tiếp nhau -> cấp phát các page địa chỉ ảo liên tiếp nhau
    + `order`: là số page muốn cấp, lũy thừa của 2
- `void free_page(unsigned long addr)`
    + giải phóng 1 page
- `void free_pages(unsigned long addr, unsigned int order)`
    + giải phóng nhiều page, `order` là lũy thừa 2 page cần giải phóng
## Page allocator flags - gfp_t
- `GFP_KERNEL`: cấp bộ nhớ tiêu chuẩn, việc cấp phát có thể block tiến trình để tìm memory khả dụng. Trong các trường hợp thì đều ổn ngoại trừ interrupt
- `GFP_ATOMIC`: ram được cấp từ code sẽ không được phép chặn, cho phép truy cập khẩn cấp nhưng vẫn có thể fail nếu không có memory sẵn sàng. Flag này dùng cho ngắt hoặc các nhiệm vụ khẩn cấp
- các flag khác: `include/linux/gfp_types.h`
## SLAB allocator - bộ cấp phát SLAB
- cấp phát bộ nhớ nhỏ hơn 1 page size cho các driver cần ít memory hơn page size (4KB)
- cho phép tạo caches chứa các object có cùng kích thước
- size của object có thể nhỏ hoặc lớn hơn page size
- bộ cấp pháp SLAB đảm nhận nhiệm vụ tăng hoặc giảm size cache nếu cần dựa vào số lượng object được cấp phát. SLAB dùng page allocator để cấp phát và giải phóng pages
- SLAB cache được dùng cho cấu trúc dữ liệu mà xuất hiện nhiều trong kernel như: file object, process description, ...
- SLAB allocator hiếm khi dùng cho các driver
- ![alt text](images/image-57.png)
- Dễ hiểu thì SLAB như là người lấy 4KB đó đi chia nhỏ ra rồi bán lại cho các process, driver cần đúng như chúng muốn, chứ không cần thiết phải lấy cả 4KB hoặc 8KB, ... Giống như là muốn bao nhiêu thì lấy bấy nhiêu
- SLAB allocator giảm thời gian cấp phát thô từ RAM vật lý và chi phí khởi tạo lại các biến nội bộ của hệ điều hành
## Different SLAB allocators
- có nhiều phiên bản SLAB allocator khác nhau, nhưng đều tương thích về mặt API
- chọn phiên bản SLAB lúc cấu hình kernel
    + `CONFIG_SLUB`: allocator mặc định, lựa chọn tốt, ít phân mảnh
    + `CONFIG_SLUB_TINY`: chiếm dụng bộ nhớ tối thiểu, hi sinh khả năng mở rộng, debug. Chỉ dùng cho các hệ thống dưới 16MB ram 
## kmalloc allocator
- cấp phát bộ nhớ đa mục đích trong linux kernel
- với size nhỏ, nó dựa vào SLAB cache, đặt lên là `kmalloc-xxx` trong `/proc/slabinfo`
- với size lớn, nó dựa vào page allocator
- **vùng bộ nhớ được cấp là vùng vật lý liên tục**
- size của vùng được cấp được làm tròn lên bằng kích thước nhỏ nhất của SLAB cache mà nó có thể chứa vừa 
- dùng các flag như page allocator (GFP_KERNEL, GFP_ATOMIC, ...)
- Mỗi lần cấp phát tối đa 4MB
- Tổng dung lượng cấp phát tối đa 128MB
- Nên dùng để cấp phát trừ khi có lý do nào khác để dùng allocator khác
## kmalloc API
- include <linux/slab.h>
- `void *kmalloc(size_t size, gfp_t flags)`
    + cấp phát size bytes, trả về con trỏ virtual address trỏ tới vùng đó
    + flags: flags allocator
- `void kfree(const void *objp)`
    + giải phóng 1 vùng cấp phát từ `kmalloc`
- `void *kzalloc(size_t size, gfp_t flags)`
    + cấp phát vùng nhớ với khởi tạo là 0
- `void *kcalloc(size_t n, size_t size, gfp_t flags);`
    + cấp phát bộ nhớ cho mảng n phần tử với 1 phần tử có kích thước size byte, và set data vùng nhớ là 0 
- `void *krealloc(const void *p, size_t new_size, gfp_t flags)`
    + thay đổi kích thước của vùng nhớ là con trỏ p đang nắm, đổi thành new_size bằng việc cấp phát lại vùng mới rồi copy data qua nếu data mới không vừa với page alignment của vùng cũ. Còn nếu vẫn vừa với vùng cũ thì giữ nguyên vùng cũ và đổi size
## devm_kmalloc functions
- tự động giải phóng vùng nhớ khi device hoặc module bị gỡ bỏ
- `void *devm_kmalloc(struct device *dev, size_t size, gfp_t gfp);`
- `void *devm_kzalloc(struct device *dev, size_t size, gfp_t gfp);`
- `void *devm_kcalloc(struct device *dev, size_t n, size_t size, gfp_t flags);`
- `void *devm_kfree(struct device *dev, void *p);`
- trong probe() thì devm này dùng khi cấp phát bộ nhớ cho các struct device (i2c_client, spi_device, ...)
## vmalloc allocator
- có thể cấp phát bộ nhớ mà không cần chúng liên tục, tức là gom các mảnh bộ nhớ thành 1 cục rồi cấp phát
- size của bộ nhớ được yêu cầu được làm tròn lên page tiếp theo (không hiệu quả cho cấp phát nhỏ)
- có khả năng cấp phát được bộ nhớ khá lớn, vì phân mảnh bộ nhớ không phải là vấn đề. vmalloc sẽ đi gom các mảnh bộ nhớ rồi ghép lại cho đủ bộ nhớ yêu cầu
- không phù hợp cho DMA (Direct memory access)
- API trong `include/linux/vmalloc.h`
    + `void *vmalloc(unsigned long size);`
    + `void vfree(void *addr);`
## Kernel memory debugging
- KASAN: detect lỗi bộ nhớ động, tốn tài nguyên
- KFENCE: ít tốn tài nguyên hơn KASAN
- Kmemleak: check cho memory leak, tốn tài nguyên

# I/O memory
## Memory-mapped I/O