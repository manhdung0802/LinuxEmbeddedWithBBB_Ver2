# Cross-compiling toolchains
## 1. Toolchain definition
- Cross compiling sinh ra để:
    + giải quyết phần cứng giới hạn ở target
    + cấu hình yếu của target
    + không muốn cài tất cả tool ở target
## 2. Architecture tuple and toolchain prefix
- tên của kiến trúc thường có 3 hoặc 4 phần: arm-linux-gnueabihf
    + tên kiến trúc: arm, riscv, ...
    + tên vendor
    + tên operating system
    + ABI/C library
- Các component của gcc toolchain
    + Binutils: bộ tool để tạo ra các file binary cho kiến trúc CPU, nó có các lệnh: as, ld, ar, ...
    + kernel header: kernel header dùng để compile cần thấp hơn hoặc bằng kernel header đang chạy ở target
    + C/C++ library: inteface giữa tầng app và tầng kernel, cung cấp các thư viện C cơ bản: glibc, uClibc, musl,...
    + C/C++ compiler: bộ compiler GNU (gcc compiler)
    + GDB debugger (optional)
## 3. Linux vs bare-metal toochain
- Linux toolchain: 
    + là toolchain đã bao gồm C standard library, sử dụng Linux system call để gọi system service
    + có thể dùng để build linux user space app và kernel space code
    + được nhận diện bởi `linux` trong cụm arm-`linux`, arm-none-`linux`-gnueabihf
- Bare metal toolchain:
    + là toolchain không bao gồm C standard library
    + sử dụng để build bare-metal code (firmware, bootloader, linux kernel)
    + được nhận diện bởi `none` trong cụm arm-`none`-eabi, ...

## 4. Toolchain options
### 4.1 ABI - Application Binary Interface
- ABI định nghĩa cách mà file binary giao tiếp với nhau và với hiệu điều hành 
    + calling conventions
    + system calls
    + cấu trúc dữ liệu
- Có 2 ABI chính
    + EABI (soft-float): phép toán số thực sẽ được xử lý bằng phần mềm, chậm
    + EDBIhf (hard-float): thực hiện phép toán số thực bằng thanh ghi số thực chuyên dụng, kiến trúc đó cần có FPU (floating point unit) thì mới dùng được EABIhf (tất cả ARMv7-A và ARMv8-A đều có FPU)

## 5. Obtaining a Toolchain
- Có nhiều các để có được toolchain:
    + Build toolchain thủ công: khó, phức tạp
    + Tìm 1 toolchain đã được build sẵn
    + Dùng các công cụ để hỗ trợ build toolchain:
        - crosstool-ng
        - buildroot
        - yocto / openEmbedded

---

# Bootloaders and firmware
## 1. Bootloader role
- Bootloader là 1 phần code chịu trách nhiệm cho:
    + khởi tạo phần cứng cơ bản
    + Load file binary, thường là OS kernel từ bộ nhớ, network
    + Giải nén binary
    + Thực thi application
- Bên cạnh đó, bootloader còn cung cấp shell hoặc menu:
    + menu: chọn OS để load
    + shell: lệnh để load data từ bộ nhớ, network, kiểm tra memory, test phần cứng
- Bootloader là đoạn code đầu tiên được bộ xử lý thực thi và có thể được chỉnh sửa
## 2. Booting on embedded platforms
- Phần lớn các bộ xử lý có chứa ROM code thực thi các bước khửoi tạo cho quá quá trình boot
- ROM code được viết bởi vendor và được nạp sẵn vào processor
    + ROM code không thể thay đổi
    + Hành vi của ROM code được mô tả trong datasheet
- ROM code có nhiệm vụ:
    + tìm bootloader phù hợp
    + tải nó và chạy nó từ NAND/NOR flash, usb, sd card, emmc, ...
- Thông thường, chương trình chạy với RAM chưa được khởi tạo, do đó bootloader chỉ có thể được load vào SRAM
    + Kích thước bootloader nhỏ do SRAM hạn chế
    + Quá trình khởi động được chia thành 2 bước:
        - first stage bootloader: nhỏ, chạy ở SRAM, khởi tạo external DRAM
        - second stage bootloader: lớn, chạy ở external DRAM
- Booting on AM335x:
    + ![alt text](image.png)
- Two stage booting sequence:
    + ![alt text](image-1.png)
    + 1. ROM code load first stage bootloader từ bộ nhớ vào SRAM
    + 2. first stage bootloader chạy ở SRAM, khởi tạo DRAM
    + 3. first stage bootload load second stage bootloader từ bộ nhớ vào DRAM
- ROM code recovery mechanism:
    + ROM code cung cấp cơ chế phục hồi, cho phép nạp firmware cho bo mạch không có bootloader hoặc bootloader bị hỏng, thường bằng giao thức riêng cho nhà sản xuất thông qua UART hoặc USB
    + Nó thường cho phép đẩy 1 bootloader mới vào RAM, giúp việc nạp bootloader có thể thực hiện tại được
    + 1 số tool của các vendor: STM32 cube, SAM-BA, Snagboot `https://github.com/bootlin/snagboot`,...
## 3. Bootloaders - giới thiệu 1 số bootloader phổ biến
- GRUB - Grand Unified Bootloader:
    + là bootloader của GNU project
    + chủ yếu dành cho x86
- Syslinux
    + dùng cho việc boot từ network và usb, sdcard
    + https://mirrors.edge.kernel.org/pub/linux/utils/boot/syslinux/
    + syslinux: boot từ FAT filesystem
    + pxelinux: boot từ network
    + isolinux: boot từ CD-ROM
    + extlinux: boot từ phần lớn kiểu filesystem
- systemd-boot
    + trình quản lý boot UEFI đơn giản
    + việc cấu hình được chứa trong EFI system partition
    + https://www.freedesktop.org/wiki/Software/systemd/systemd-boot/
- shim:
    + là bootloader UEFI nhỏ
    + dùng chủ yếu trong việc secure boot
    + https://github.com/rhboot/shim
- **U-boot:**
    + dùng rộng rãi trong phần lớn thiết bị nhúng: arm, risc-v, ....
    + support x86
    + https://docs.u-boot-project.org/en/latest/
- Barebox: 
    + 1 bootloader khác cho cpu nhúng
    + ít được sử dụng hơn U-boot
## 4. Trusted firmware
- Thông thường, bootloader chỉ được dùng trong quá trình booting
- Soc ngày nay có cơ chế bảo mật nâng cao yêu cầu chạy trusted firmware
- Trusted firmware được load bởi bootloader và nó vẫn tồn tại ngay cả sau khi quyền điều khiển đã được giao cho OS
    + trusted firmware chứa trong phần riêng biệt của DDR hoặc SRAM cụ thể, không thể truy cập được từ OS
    + nó cung cấp các service cho OS nhưng OS không thể thực hiện trực tiếp được
    + nó chịu trách nhiệm chạy 1 OS bảo mật bên cạnh OS thường
- Trusted firmware nằm ở mức đặc quyền EL3 (Exception level 3), kiểm soát tuyệt đối toàn bộ phần cứng
- Inteface with secure firmware - nhiệm vụ của trusted firmware:
    + được chuẩn hoá bởi ARM
    + ngăn chặn hệ điều hành đang hoạt động trong môi trường truy cập được vào các phần cứng quan trọng
    + Linux (EL1) giao tiêp với Secure firmware (EL3) qua interface:
        - PSCI - Power state coordination interface: quản lý nguồn điện, on/off cpu, ...
        - SCMI - System control and management interface: quản lý nguồn điện cho các vùng khác, điều chỉnh clock, sensor, hiệu năng
    + Đối với các dòng chip 64-bit hiện đại ngày nay (như dòng i.MX 8, i.MX 9 hay Raspberry Pi 4/5), Trusted Firmware là bắt buộc phải có. Nếu không có firmware này ở tầng EL3 để cung cấp dịch vụ PSCI/SCMI, Linux Kernel sẽ không thể boot được hoặc không thể điều khiển được CPU.
- TF-A - Trusted firmwate-A
    + là phần mềm mẫu mà ARM thiết kế cho các dòng Cotex-A
    + Nhiệm vụ của nó là hiện thực hóa tầng Secure Monitor chạy ở mức đặc quyền tối cao EL3
    + nó có các interface mà OS cần từ secure firmware
    + nó có driver cho khối hardware mà Linux không thể truy cập trực tiếp được
    + nó cần được port riêng cho từng dòng SoC
    + dựa theo platform, có thể nó cần port cho từng board
- Trusted OS, OP-TEE
    + 1 trusted operating system có thể chạy ở secure world được gọi là trusted Execution Environment (TEE)
    + Phân vùng phần cứng giữa secure world và normal world: 1 vài tài nguyên phần cứng chỉ khả dụng trong secure world, bởi trusted OS
    + cho phép chạy app/service tách biệt khỏi linux và có thể cung cấp các service cho Linux app
    + OP-TEE là OS mã nguồn mở bảo mật - secure OS
- Sơ đồ tổng quát trusted firmware: ![alt text](image-2.png)
## 5. Example boot sequence on ARM
![alt text](image-3.png)
![alt text](image-4.png)
![alt text](image-5.png)
![alt text](image-6.png)
![alt text](image-7.png)

## 6. The U-boot bootloader
- U-boot cấu hình hệ thống dựa vào kconfig từ Linux kernel
- folder configs chứa file cấu hình hỗ trợ nhiều board hoặc platform
    + file cấu hình đó có thể là 1 cấu hình mà hỗ trợ nhiều board có cùng processor
    + file cấu hình định nghĩa tất cả tuỳ chọn liên quan: loại CPU, drivers cần thiết, tính năng U-boot được compile cùng
- U-boot configuration:
    + U-boot cần được cấu hình trước khi compile
    + cấu hình được chứa trong file .config
    + Load cấu hình sẵn có: `make BOARDNAME_defconfig`
    + Mở menu của U-boot: `make menuconfig`
- U-boot compilation:
    + cần phải chỉ định CROSS_COMPILE khi build: `make CROSS_COMPILE=arm-linux-`
    + kết quả là `u-boot.bin` hoặc `u-boot.img`, ...
- Concept off U-boot SPL
    + để đáp ứng được two-state boot process, U-boot có U-boot SPL (secondary program loader)
    + SPC là bản rút gọn của U-boot, đủ nhỏ để đáp ứng được size hạn chế của first stage bootloader
    + nó được hardcode trong C code
- Device tree in U-boot
    + là cấu trúc dữ liệu mô tả phần cứng
    + cho phép phần mềm biết được ngoại vi nào khả dụng và cách kết nối chúng tới system
    + U-boot defconfigs thường chỉ định rõ device tree, nhưng nó có thể được thay đổi bằng biến DEVICE_TREE
- Concept of U-boot environment
    + 