# Menu

# Linux Kernel Introduction
## Linux kernel in the system
- ![alt text](image.png)
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
    + make modules hoặc make -j4 (nếu build cùng kernel)
    + make INSTALL_MOD_PATH=<dir>/ modules_install
- tổng quan việc compile và install kernel
    + ![alt text](image-1.png)
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
    + ![alt text](image-2.png)
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
    + ![alt text](image-3.png)
- DT overall structure: simplified example
    + ![alt text](image-4.png)
    + ![alt text](image-5.png)
    + ![alt text](image-6.png)
    + ![alt text](image-7.png)
    + ![alt text](image-8.png)
    + ![alt text](image-9.png)
- Device tree inheritance
    + device tree có thể được chia thành nhiều file
    + `.dtsi`: là file dùng để include vào file khác
        - chứa định nghĩa Soc-level
        - định nghĩa chung cho 1 số lượng board
    + `.dts`: là file device tree cuối cùng
    + việc ghi đè (overlay) device tree được tuân theo thứ tự include, cho phép file sau ghi đè giá trị của file trước
- DT inheritance in Bone Black support
    + ![alt text](image-10.png)
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
    + ![alt text](image-11.png)
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
        - ![alt text](image-12.png)
        - `%YAML`: định nghĩa language version
        - `$id`: có thể không phải là URL thực, nhưng là mã định danh duy nhất
        - `$schema`: tham chiếu tới meta-schema mà file này cần được xác thực
        - `property`: nơi bắt đầu định nghĩa, tất cả thuộc tính cần được liệt kê, tên biến viết thường, theo sau là dấu `:`, có 1 dòng trống giữa các property
- Binding syntax: types
    + ![alt text](image-13.png)
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
    + ![alt text](image-14.png)
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
    + ![alt text](image-15.png)
    + giới hạn bằng việc đặt minimum/maximum của giá trị
    + đặt giá trị default cho property
    + đặt minItems/maxItems cho mảng
- Binding syntax: lists and dictionaries
    + ![alt text](image-16.png)
    + biểu diễn các giá trị có thể xảy ra, giới hạn giá trị mà property có thể nhận
        - bắt buộc giá trị duy nhất: `const`
        - lấy giá trị từ list: `enum`
    + `const`/`item` có thể được nhóm lại trong 1 `items` list
    + có thể xây dựng điều kiện từ 
        - `oneOf`: biểu thị XOR
        - `anyOf`: biểu thị OR
        - `allOf`: biển thị AND
- Bindings syntax: referencing other bindings
    + ![alt text](image-17.png)
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
    + ![alt text](image-18.png)
    + đôi khi ta cần các mô tả linh hoạt hơn
        - phụ thuộc giữa các property
            + 1 property có thể cần có property khác
            + nếu cả 2 thuộc tính cùng xuất hiện hoặc cùng không xuất hiện, dependency cần biểu diễn 2 chiều
        - thay đổi constrain dựa theo property
            + biểu diễn bằng if/else dưới `allOf`
            + điển hình là mỗi compatible đi kèm với constrain khác nhau
- Bindings syntax: enforcing correct properties only
    + ![alt text](image-19.png)
    + các file YAML liệt kê các property và thêm constrai cho chúng
        - việc thêm các thuộc tính chưa định nghĩa có thể xảy ra
        - việc quên thuộc tính bắt buộc có thể xảy ra
    + cần thêm constrain để phát hiện lỗi chính tả và các property ngoài ý muốn
        - `required`: bắt buộc xuất hiện
        - `additionalProperties: false`: ngăn thuộc tính chưa được define trong file này xuất hiện
        - `unevaluatedProperties`: ngăn thuộc tính chưa được define trong file này xuất hiện hoặc không được refer đến thông qua `allOf`
- Bindings syntax: validating your own bindings
    + ![alt text](image-20.png)
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
    + ![alt text](image-21.png)
- Device Tree properties for consumer devices
    + device cần pin để mux thì dùng thuộc tính `pinctrl-<x>` và `pinctrl-names` trong device tree
        - `pinctrl-<x>`: link tới 1 cấu hình pin cụ thể ứng với state nhất định. `x` được đánh số theo thứ tự trái qua phải nếu `pinctrl-names` có nhiều tên
        - `pinctrl-names`: gán tên gọi cho mỗi trạng thái. Nếu để `default` thì pin mux được thiết lập bởi device driver
        - check `Documentation/devicetree/bindings/pinctrl/pinctrl-bindings.txt` để có thêm chi tiết
- Device Tree properties for consumer devices - Examples
    + ![alt text](image-22.png)
    + ![alt text](image-23.png)
        - `pinctrl-names` có 2 tên thì `pinctrl-<x>` cũng có 2 cái tương ứng cho từng name
- Defining pinctrl configurations
    + các cấu hình pinctrl khác nhau cần được định nghĩa như node con trong pinctrl device
    + cấu hình có thể được define ở 
        - `.dtsi`: cho cấu hình mà thường dùng chung trong nhiều board
        - `.dts`: dùng cho board cụ thể
    + `pinctrl-<x>` của device trỏ tới cấu hình pin mà nó cần thông qua 1 device tree phandle
    + Ví dụ
        - ![alt text](image-24.png)
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