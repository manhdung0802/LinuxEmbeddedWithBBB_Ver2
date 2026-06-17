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
- Booting sequence
Power On
    │
    ▼
┌──────────┐      1. Đọc chân Boot Pins & Khởi tạo SRAM
│ ROM Code │ ──────────────────────────────────────────────┐
└────┬─────┘                                               │
     │ 2. Tải MLO/SPL vào SRAM                             ▼
     │──────────────────────────────────────────────> ┌─────────┐
     │                                                │  SRAM   │
     │ 3. Nhảy đến SRAM & nhường quyền                └────┬────┘
     │─────────────────────────────────────────────┐       ▲
     ▼                                             │       │
┌──────────┐                                       ▼       │ 4. Cấu hình &
│   SPL    │ ──────────────────────────────────────────────┘    Kích hoạt RAM
│  (MLO)   │
└────┬─────┘ 5. Tải file u-boot.img vào RAM (DDR)
     │──────────────────────────────────────────────> ┌─────────┐
     │                                                │   RAM   │
     │ 6. Nhảy đến RAM & nhường quyền                 │  (DDR)  │
     │─────────────────────────────────────────────┐  └────┬────┘
     ▼                                             │       ▲
┌──────────┐                                       ▼       │
│  U-Boot  │ ──────────────────────────────────────────────┘
└────┬─────┘ 7. Nạp Kernel (zImage) + DTB (.dtb) vào RAM
     │                                                     
     │ 8. Thực hiện lệnh 'bootz' để kích nổ Kernel          
     │─────────────────────────────────────────────┐
     ▼                                             │
┌──────────┐                                       ▼
│  Linux   │ <─────────────────────────────────────┘
│  Kernel  │ 9. Khởi tạo Driver, Mount Real Rootfs (eMMC/SD)
└────┬─────┘
     │ 10. Chạy tiến trình đầu tiên (PID 1)
     ▼
┌──────────┐
│  init /  │ 11. Chạy các dịch vụ nền (Services), Mở cổng Serial
│ systemd  │ ──────────────────────────────────────────────> [ Login Prompt ]
└──────────┘

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
- Concept of U-boot SPL
    + để đáp ứng được two-state boot process, U-boot có U-boot SPL (secondary program loader)
    + SPC là bản rút gọn của U-boot, đủ nhỏ để đáp ứng được size hạn chế của first stage bootloader
    + nó được hardcode trong C code
- Device tree in U-boot
    + là cấu trúc dữ liệu mô tả phần cứng
    + cho phép phần mềm biết được ngoại vi nào khả dụng và cách kết nối chúng tới system
    + U-boot defconfigs thường chỉ định rõ device tree, nhưng nó có thể được thay đổi bằng biến DEVICE_TREE
- Concept of U-boot environment
    + menuconfig được dùng để cấu hình U-boot tại thời điểm biên dịch
    + U-boot cũng có thể được cấu hình lúc run time nhờ các environment variables
    + Environment variables là cặp key/value
        - Một vài biến môi trường cụ thể ảnh hưởng tới hành vi của các câu lệnh U-boot
        - Có thể thêm biến môi trường và dùng chúng trong scripts
    + Biến môi trường U-boot được load và modify trong RAM
    + U-boot có 1 môi trường default được tích hợp sẵn trong binary của nó
        - được dùng khi không thấy môi trường nào khác
        - được xác định trong file cấu hình
        - môi trường default đôi khi quá phức tạp
- U-boot environment persistent storage
    + dựa vào việc cấu hình, biến môi trường U-boot có thể được lưu tại các vùng sau để không bị mất khi restart
        - offset cố định trong NAND flash
        - offset cố định ở MMC hoặc USB storage, trước điểm bắt đầu của partition đầu tiên
        - trong 1 file nằm ở partition FAT hoặc ext4
        - trong UBI volume
- U-boot environment commands
    + `printenv`: show all variable
    + `printenv <variable-name>` Shows the value of a variable
    + `setenv <variable-name> <variable-value>` Changes the value of a variable or defines a new one, only in RAM
    + `editenv <variable-name>` Interactively edits the value of a variable, only in RAM
    + After an `editenv` or `setenv`, changes in the environment are lost if they are not saved persistently
    + `saveenv`Saves the current state of the environment to storage for persistence.
    + env command, with many sub-commands: env default, env info, env erase, env set, env save, etc.
- U-boot memory allocation - phân bổ bộ nhớ U-boot
    + có nhiều lệnh trong U-boot khi ghi/đọc data từ bộ nhớ có thể cần địa chỉ RAM làm tham số
    + U-boot không có cơ chế phân phối bộ nhớ, người dùng phải tự tìm các vùng nhớ khả dụng để tải/dùng data
    + Dùng lệnh `bdinfo` để biết start address và size của RAM
    + Tránh dùng phần cuối của RAM, vì đó có thể đang được U-boot hoặc vùng nhớ động sử dụng
- U-Boot memory manipulation commands - lệnh thao tác bộ nhớ U-boot
    + các command này dùng để kiểm tra hoặc chỉnh sửa bộ nhớ, dùng cho debug, can thiệp vào thanh ghi
    + các địa chỉ được thao tác trong U-boot là địa chỉ vật lý 
    + hiển thị bộ nhớ: `mw [.b, .w, .l, .q] address [# of objects]`
    + ghi bộ nhớ: `mw [.b, .w, .l, .q] address value [count]`
    + chỉnh sửa bộ nhớ: `mm [.b, .w, .l, .q] address`
- U-boot raw storage commands - lệnh với bộ nhớ chưa qua định dạng
    + U-boot có thể thao tác với raw storage devices
    + Việc U-boot thao tác với raw storage là để trong quá trình khởi động, U-boot có thể nhảy tới các địa chỉ đã lưu biến môi trường trong `U-boot environment persistent storage`
    + Lệnh:
        - NAND flash
            + nand info
            + nand read <addr> <off|partition> <size>
            + nand erase [<off> [<size>]]
            + nand write <addr> <off|partition> <size>
            + ... 
        - MMC
            + mmc info
            + mmc read <addr> <blk#> <cnt>
            + mmc write <addr> <blk#> <cnt>
            + mmc part to show partition table
            + mmc dev to show/set current MMC device
            + ...
        - USB storage
            + usb info
            + usb read <addr> <blk#> <cnt>
            + usb write <addr> <blk#> <cnt>
            + usb part
            + usb dev
            + More: help usb
- U-Boot filesystem storage commands
    + U-boot support nhiều filesystems, danh sách hỗ trợ dựa vào việc cấu hình U-boot
    + Lệnh này giúp U-boot hiểu được cấu trúc và tệp tin trong các phân vùng đã định dạng (FAT, ext4,...)
        - Duyệt file: `ls` (ls mmc 0:1 - danh sách file tại bộ nhớ mmc, device 0, partition 1)
        - Xem thông tin phân vùng `size`
        - Nạp dữ liệu vào RAM để chuẩn bị boot: `load`
- U-boot networking
    - trang 149/522 embedded-linux-slides.pdf
    - load/đọc file từ network
    - TFTP - Trivial File Transfer Protocol
        - trang 150/522 embedded-linux-slides.pdf
- Script in environment varriables
    + biến môi trường có thể chứa 1 đoạn script nhỏ để thực thi 1 vài lệnh và test kết quả của command
        - hữu ích khi muốn tự động boot hoặc nâng cấp processed
        - có thể kết hợp nhiều lệnh bằng dấu `;` ở cuối mỗi lệnh
        - dùng `if`, `then`, `else`, `fi` để test 
        - chạy script bằng lệnh `run <variable-name>`
        - reference biến bằng `$variable-name`
        - ví dụ: `setenv bootcmd tftp 0x21000000 zImage; tftp 0x22000000 dtb; bootz 0x21000000 - 0x22000000'`
- U-boot booting commands
    + Lệnh để boot linux kernel image
        - `bootz`: boot file zImage
            + `bootz [kernel-addr] [initramfs] [device tree addr]`
                - addr: địa chỉ của kernel image trong RAM
                - initramfs: địa chỉ của initrd hoặc initramfs
                - fdt: địa chỉ của Devcei tree được pass vào Linux kernel
        - `booti`: boot file Image
        - `bootm`: boot kernel image với legacy U-boot header
        - `zboot`: boot file bzImage
    + Biến môi trường quan trọng
        - `bootcmd`: danh sách các lệnh được thực thi tự động bởi U-boot sau khi đếm ngược
            + `setenv bootcmd 'tftp 0x81000000 zImage; sleep 0.1; tftp 0x82000000 am335x-boneblack.dtb; bootz 0x81000000 - 0x82000000'`
        - `bootargs`: dòng lệnh Linux kernel
- FIT image
    + Flat Image Tree `image.itb`
    + xu hướng mới của việc boot bằng U-boot (hoặc các bootloader khác)
    + chứa format cho phép đóng gói nhiều image thành 1 cái
        - nhiều kernel image
        - nhiều device tree
        - nhiều initramfs
        - `.its`: file mô tả nội dung image
        - `.itb`: file binary của `.its`
- Generic Distro boot - cơ chế khởi động phân phối chung trong U-boot
    + mỗi board/platform có môi trường U-boot của riêng nó, với custom variables/commands -> lộn xộn -> cần phải tiêu chuẩn hóa hành vi của bootloader, nếu không thì mỗi board sẽ có 1 kiểu boot khác nhau
    + Khi kích hoạt Generic Distro boot, tại thời điểm boot, U-boot sẽ:
        - tìm kiếm vị trí của bootable partition để xem phân vùng nào được đánh dấu cờ khởi động - `part list`
        - với lệnh `sysboot`, U-boot sẽ tìm file /extlinux/extlinux.conf hoặc boot/extlinux/extlinux.conf mà mô tả boot như nào, và sau đó đưa ra lệnh để người dùng lựa chọn
        - khi cấu hình đã được chọn, U-boot load và boot kernel, device tree, và initramfs images
        - ví dụ: `part list mmc 0 -bootable bootpart; sysboot mmc 0:$bootpart any`
    + Một số biến môi trường cần được set
        - `kernel_addr_r`: địa chỉ trong RAM để load kernel image
        - `ramdisk_addr_r`: địa chỉ trong RAM để load initramfs image
        - `fdt_addr_r`: địa chỉ trong RAM để load DTB (Flattened device tree)
        - `pxefile_addr_r`: địa chỉ trong RAM để load file config, thường là extlinux.conf
        - `boootfile`: đường dẫn của file config (Ví dụ: `/boot/extlinux/extlinux.conf`)
## 7. TF-A: trusted firmware
- Concept of FIP
    + FIP là firmware image package, là concept do TF-A quản lý
    + Là 1 định dạng đóng gói dữ liệu được định nghĩa và sử dụng bởi TF-A
    + nhiệm vụ làm vỏ bọc để chứa BL33 (no-trusted firmware)
- TF-A
    + không dùng Kconfig để cấu hình
    + tất cả cấu hình được dựa vào biến mà pass vào cùng lệnh `make`
- Configure TF-A: important variables
    + `CROSS_COMPILE`
    + `ARCH`
    + `ARM_ARCH_MAJOR` = 7 cho ARMv7 hoặc 8 cho ARMv8  
    + `PLAT` SoC family, any directory name in plat that contains platform.mk
    + AARCH32_SP, the Secure Payload, specific to ARMv7. Either OP-TEE or the built-in SP-MIN provided by TF-A
    + `DTB_FILE_NAME`, path to the Device Tree describing our board
    + `BL33`, path to the second stage bootloader, usually U-Boot, to include in the FIP image
    + Specific to STM32MP1
        - `BL33_CFG`, path to the U-Boot Device Tree `u-boot.dtb`
        - `STM32MP_SDMMC=1`, enable support for SD card/eMMC in TF-A
## 8. Cách build U-boot
- export CROSS_COMPILE=arm-linux-gnueabihf-
- make am335x_evm_defconfig
- make menuconfig
- make DEVICE_TREE=am335x-boneblack
- truyền file qua U-boot bằng tftp thì đặt file ở Ubuntu /srv/tftp

# Linux kernel introduction
## 1. Linux kernel in the system
![alt text](image-8.png)
- Linux kernel đóng vai trò cầu nối giữa tầng app và phần cứng
## 2. Linux kernel main roles
- Linux kernel quản lý tất cả phần cứng (CPU, memory, I/O)
- cung cấp API để cho phép user space và libraries truy cập vào phần cứng
- quản lý truy cập đồng thời và sử dụng tài nguyên từ các ứng dụng
## 3. System calls
- Interface giữa kernel space và user space là các lệnh system calls
- có khoảng 400 lệnh system call: lệnh với file, network, process management, ...
- Các lệnh system call này được đóng gói trong C library và user space thường dùng các API trong trong C library để giao tiếp với kernel
## 4. Pseudo filesystems
- Linux thể hiện thông tin system và kernel ở userspace thông qua pseudo filesystems, hay còn gọi là virtual filesystems
- Pseudo filesystems cho phép app thấy được thư mục, file mà không tồn tại trong bộ nhớ thực
- Pseudo filesystems quan trọng nhất là:
    + /proc: chứa các process
    + /sys: mô tả hệ thống dạng device tree được kết nối bằng các bus
## 5. Linux kernel sources
- `https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/`
- Stable: `https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git`
- Linux kernel size and structure
    + Source code Linux tầm 80k file, 35M dồng, nặng 1.3Gb, nhưng Linux kernel chỉ nặng vài MB -> Lý do là bởi Linux source chứa nhiều device drivers, network protocol, filesystems, ..., còn phần lõi thì rất nhỏ
## 6. Kernel configuration
- Kernel chứa hàng ngàn device drivers, filesystem drivers, ...
- Có hàng ngàn tùy chọn cho kernel, được lựa chọn để build kernel
- Kernel configuration là quá trình set các tùy chọn mà user muốn kernel được build ra sao
- Tùy chọn phụ thuộc vào:
    + Kiến trúc của target, hardware
    + Khả năng mà mình muốn cấp cho kernel
- Kernel configuration và build system dựa vào nhiều Makefiles
    + người dùng chỉ tương tác với Makefile ở thư mục gốc của kernel source
    + `make <target>`
- Cấu hình và build: 
    + Initial configuration:
        - `make name_defconfig`: cấu hình linux kernel theo thiết lập sẵn của từng file defconfig, lệnh này sẽ tạo ra file `.config`
        - `make menuconfig`: cấu hình thêm tùy chỉnh
        - có thể dùng `make savedefconfig` để lưu lại tùy chỉnh dạng file `name_defconfig`
    + `make ARCH=arm CROSS_COMPILE=arm-linux-...` hoặc `export ARCH=arm, export CROSS_COMPILE=arm-linux-..., make`
    + Cần chỉ rõ kiến trúc cần build cho kernel: `ARCH=arm`, nếu không thì mặc định kernel build system sẽ build cho máy host
    + Lựa chọn compile: native compiler (gcc) hoặc cross compiler (arm-linux-)
        - Nếu build với Clang thì set LLVM thành 1
- Built-in or Module?
    + kernel image là 1 file đơn, là output của việc link tất cả object file liên quan tới tính năng được enable trong việc cấu hình. Đây là file được load vào RAM bởi bootloader. Tất cả các tính năng khả dụng ngay lập tức khi kernel start, tại thời điểm không có filesystem tồn tại 
    + 1 vài tính năng có thể được compile dạng module
        - load/unload lúc runtime
        - mỗi module được thể hiện qua device file
        - Không thể boot sớm module vì thời điểm boot không có filesystem nào
- Kernel option types - các kiểu dữ liệu trong Kconfig
    + có nhiều kiểu cấu hình kernel, được define trong file Kconfig, rất hữu ích để phân biệt giữa kernel build từ nhiều options
    + `bool` option: 
        - true: thêm tính năng vào kernel
        - false: loại bỏ tính năng khỏi kernel
    + `tristate` options:
        - true: thêm tính năng vào kernel image
        - module: thêm tính năng như 1 kernel module
        - false: loại bỏ tính năng
    + `int` options: chỉ định các giá trị số nguyên
    + `hex` options: chỉ định các giá trị số hex, ví dụ CONFIG_PAGE_OFFSET=0xC0000000
    + `string` options: chỉ định giá trị string, ví dụ CONFIG_LOCALVERSION=-no-network
- Kernel option dependencies
    + Giả sử việc bật driver network yêu cầu network stack phải được bật, vì vậy có 2 cách cấu hình trong file Kconfig
        ```
        B sẽ không hiện cho đến khi A được enable
        config B
            depends on A
        ```
        ```
        Khi A được enable, B sẽ enable theo
        config A
            select B
        ```
- xconfig
    + `make xconfig`
    + là 1 giao diện đồ họa để cấu hình kernel
    + yêu cầu phải có gói `qtbase5-dev`
- menuconfig
    + `make menuconfig`
- make oldconfig
    + `make oldconfig`
    + hữu ích khi nâng cấp .config từ bản kernel trước đó
    + Nếu bạn chỉnh sửa tệp .config bằng tay, việc chạy lệnh `make oldconfig` sau đó sẽ rất hữu ích, để thiết lập giá trị cho các tham số mới có thể xuất hiện do thay đổi phụ thuộc.
- Nếu muốn trở về bản .config cũ, dùng file `.config.old`

## 7. Compiling and installing the kernel
- make -jX (X là số nhân của CPU)
- recompile nhanh hơn -> dùng `export CROSS_COMPILE="ccache arm-linux"`
- Kernel compilation results - output của compile kernel
    + `arch/<arch>/boot/Image`: file kernel có thể boot
    + `arch/<arch>/boot/*Image`: file kernel nén có thể boot
        - bzImage: cho x86
        - zImage: cho arm
        - Image.gz: cho RISC-V
    + `arch/<arch>/boot/dts/<vendor>/*.dtb`: file device tree blob (file binary của device tree)
    + Các file .ko của kernel module, nằm rải rác trong source kernel
    + `vmlinux`: file raw kernel image, định dnagj ELF, hữu ích để debug
- Kernel installation: native case - máy host
    + `sudo make install`: install kernel cho máy host
    + Sau khi chạy lệnh trên, hệ thống sẽ: 
        - tạo file /boot/vmlinuz-<version>: file nén kernel image, giống với arch/<arch>/boot
        - tạo file /boot/System.map-<version>: chứa địa chỉ cho mục đích debug
        - tạo file /boot/config-<version>: cấu hình của kernel này
    + Ở GNU/Linux thường sẽ chạy 1 công cụ cấu hình bootloader để kernel mới có thể được load ở lần boot tiếp theo
- Kernel installation: embedded case
    + Không có quy chuẩn chung cho việc deploy kernel ở hệ thống nhúng
    + Vì vậy việc đưa kernel vào target thường được cấu hình qua các hệ thống build (yocto, buildroot,...)
- Module installation: native case
    + `sudo make modules_install`
    + Cài tất cả module vào /lib/modules/<version>/
        - `kernel`: module .ko file
        - `modules.alias`,`modules.alias.bin`: file chứa bí danh 
        - `modules.dep`, `modules.dep.bin`: file quản lý mức độ phụ thuộc giữa các module
        - `modules.symbols`, `modules.symbols.bin`: file khai báo ký hiệu thuộc module nào
        - `modules.builtin`: danh sách modules được build thẳng vào cùng kernel
- Module installation: embedded case
    + `make INSTALL_MOD_PATH=<dir>/ modules_install`: dir: đường dẫn chứa root file system trong target
- Kernel cleanup targets
    + make clean: xóa hết trừ .config và các file build hỗ trợ build externel modules
    + mrproper: xóa hết
    + distclean: xóa nhiều hơn mrproper
- Kernel building overview
    + ![Kernel building overview](image-9.png)
## 8. Booting the kernel
- Nhiều hệ thống nhúng có nhiều phần cứng non-discoverable (serial ethernet, I2C, USB controller, ...)
- Những phần cứng này cần được khai báo và pass vào Linux kernel, nếu không thì hardware đó coi như vô hình
- Bootloader sẽ cung cấp khai báo này khi start kernel thông qua dạng Device tree
- Customize your board device tree
    + Mỗi board/architecture có device tree riêng biệt , có sẵn tại `arch/<arch>/boot/dts/<vendor>/<board>.dtb`
    + Cần cấu hình device tree để:
        - mô tả ngoại vi được kết nối vào bus và cấu hình chúng
        - cấu hình pin muxing 
        - cấu hình vài thông số hệ thống: flash partitions, kernel command line
- Booting with U-boot
    + Load zImage tại địa chỉ X trong memory
    + Load .dtb tại địa chỉ Y trong memory
    + Start kernel: boot[z|i] X - Y (dấu `-` thể hiện không có `initramfs`)
- Kernel command line
    + kernel có thể thay đổi hành vi mà không cần recompile bằng cách dùng các kernel command line
    + kernel command line là 1 string mà định nghĩa nhiều tham số cho kernel
        - rất quan trọng để cấu hình hệ thống
        - `root=` cho root file system
        - `console=` hiển thị log kernel
        - ví dụ: `console=ttyS0 root=/dev/mmcblk0p2 rootwait`
        - xem thêm command line tại `https://www.kernel.org/doc/html/latest/admin-guide/kernel-parameters.html`
- Passing the kernel command line
    + U-boot chứa Linux kernel command line string trong biến môi trường `bootargs`
    + Ngay trước khi start kernel, Uboot sẽ chứa nội dung của `bootargs` vào `chosen` section của Device tree
    + Kernel sẽ hoạt động khác nhau dựa vào cấu hình
        - If CONFIG_CMDLINE_FROM_BOOTLOADER is set: The kernel will use only the string from the bootloader
        - If CONFIG_CMDLINE_FORCE is set: The kernel will only use the string received at configuration time in CONFIG_CMDLINE
        - If CONFIG_CMDLINE_EXTEND is set: The kernel will concatenate both strings

# Linux root filesystem
## 1. Filesystems
- filesystems được dùng để tổ chức dữ liệu và file trong bô nhớ hoặc network
- filesystems được mount vào địa chỉ đặc biệt trong cấu trúc thư mục phân cấp này
- điều này cho phép ứng dụng truy cập vào file và đường dẫn dễ dàng
- Tạo 1 mountpoint: `sudo mkdir /mnt/dung`
- `mount -t type device mountpoint`
    + type: kiểu của filesystem
    + device: bộ nhớ của device,network
    + mountpoint: vùng được mount
## 2. Root filesystem
- là filesystem được mount vào đường dẫn root (/) -> gọi là root filesystem
- root filesystem không được mount bằng lệnh `mount` mà nó được mount bởi kernel: `root=`
- nếu `root=` không được chỉ định, kernel bị panic
- root filesystem có thể được mount từ:
    + hard disk 
    + USB: `root=/dev/sdXY`
    + SD card: `root=/dev/mmcblkXpY`
    + NAND flash
    + ...
- Mounting rootfs over the network:
    + Khi hoạt động qua network, root filesystem có thể là 2 thư mục ở máy host, export bởi NFC (Network file system) -> chia sẻ thư mục chứa rootfs từ máy tính sang cho board
    + Thuận tiện cho cập nhật files trong root filesystem mà không cần khởi động lại
    + Có thể có hệ thống root filesystem lớn
    + Ở máy host:
        + Thêm cấu hình sau vào file /etc/exports:
            - `/home/tux/rootfs 192.168.1.111(rw,no_root_squash,no_subtree_check)`
                + /home/tux/rootfs: đường dẫn chứa rootfs ở máy host
                + 192.168.1.111: địa chỉ ip của board
                + (rw,no_root_squash,no_subtree_check): cho phép đọc/ghi, có quyền như root, tắt kiểm tra cây thư mục con để tăng tốc truyền file
            - sau đó chạy: `sudo exportfs -r`
    + Ở board:
        - kernel phải được compile với:
            + CONFIG_NFS_FS=y
            + CONFIG_ROOT_NFS=y
            + CONFIG_IP_PNP=y
        - cấu hình `root=/dev/nfs`
        - `ip=192.168.x.y`: IP của target
        - `nfsroot=192.168.a.b:/home/tux/rootfs/` (có thể cần thêm `,nfsvers=3,tcp` vì NFS ver 2 client và UDP có thể từ chối NFS server)
- Root filesystem in memory: initramfs
    + initramfs là phân vùng file system thu nhỏ được được nạp thẳng vào RAM khi boot. Kernel sẽ dùng nó để chạy các cấu hình cơ bản, nạp driver trước khi mount rootfs vào hệ thống
    + cũng có thể boot system với filesystem trong memory initramfs
    + Hoặc từ một kho lưu trữ CPIO được nén tích hợp vào kernel image
    + Lợi ích trong trường hợp:
        - khởi động nhanh các root filesystems nhỏ. Bởi vì filesystem đươc load hoàn tất tại thời điểm boot nên ứng dụng khởi động rất nhanh
    + Để tạo 1 initramfs,
        - Đóng gói rootfs thành định dạng `cpio`, sau đó nén lại bằng gzip
            ```
            cd rootfs/
            find . | cpio -H newc -o > ../initramfs.cpio
            cd ..
            gzip initramfs.cpio
            ```
        - Tiếp tục đóng gói để U-boot đọc được:
            ```
            mkimage -n 'Ramdisk Image' -A arm -O linux -T ramdisk -C gzip -d initramfs.cpio.gz uInitramfs
            ```
            -> rồi load uInitramfs vào địa chỉ trong RAM như 0x81000000 của zImage
        - Sau đó: `bootz kernel-addr initramfs-addr dtb-addr`
    + Để đóng gói initramfs cùng với kernel thì cần cấu hình
        - CONFIG_INITRAMFS_SOURCE= đường dẫn tới thư mục rootfs hoặc file cpio
## Root filesystem organization
- /bin: Basic programs
- /boot: Kernel images, configurations and initramfs (only when the kernel is loaded from a filesystem, not common on non-x86 architectures)
- /dev: Device files (covered later)
- /etc: System-wide configuration
- /home: Directory for the users home directories
- /lib: Basic libraries
- /media: Mount points for removable media
- /mnt: Mount point for a temporarily mounted filesystem
- /proc: Mount point for the proc virtual filesystem
- /root: Home directory of the root user
- /run: Run-time variable data (previously /var/run)
- /sbin: Basic system programs
- /sys: Mount point of the sysfs virtual filesystem
- /tmp: Temporary files
- /usr/bin: Non-basic programs
- /usr/lib: Non-basic libraries
- /usr/sbin: Non-basic system programs
- /var: Variable data files, for system services. This includes spool directories and files, administrative and logging data, and transient and temporary files








