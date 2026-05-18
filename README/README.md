# Menu

- [I. Linux booting sequence](#i-linux-booting-sequence)
- [II. Build Uboot và kernel](#ii-build-uboot-va-kernel)
- [III. Build root file system (rootfs)](#iii-build-root-file-system-rootfs)

# I. Linux booting sequence
Quá trình boot của linux có 3 giai đoạn chính
![alt text](LinuxBootingSequence.png)
## 1. First stage loader
- Khi cấp nguồn, Core sẽ thực thi các lệnh trong Rom Code: khởi tại clock cơ bản, khởi tạo 1 số data, load và chạy file thực thi của phase 2 lên bộ nhớ ram
## 2. Second program loader (Phase MLO của Uboot)
- MLO (SPL) nhiệm vụ chính: khởi tạo phần cứng tối thiểu cần thiết để load DRAM (DRAM controller), thiết lập UART/serial, I2C/SPI nếu cần, và sau đó load U-Boot chính (u-boot.img/u-boot-dtb.img) từ cùng hoặc thiết bị khác.
- Tại sao cần SPL: ROM nhỏ không đủ chức năng để khởi tạo DRAM phức tạp nên SPL chịu trách nhiệm chuẩn bị RAM để U-Boot lớn hơn có thể chạy.
- Hành vi: SPL nằm ở vùng mà ROM có thể đọc; sau khi DRAM sẵn sàng, SPL copy u-boot từ SD/eMMC vào RAM và nhảy tới nó.
## 3. Uboot phase (phase 2 của Uboot)
- Xác định kernel image (zImage/vmlinuz) và DTBs.
- Thiết lập bootargs (kernel command line) bao gồm root=, console=, rw/ro, v.v.
- Load kernel (zImage/Image) và DTB vào RAM; nếu dùng initramfs thì load initramfs (initrd).
- Nhảy sang kernel (bootm/bootz/kexec tuỳ U-Boot và format).

> **Notes**: Vì sao không load thẳng các file ở phase 2 vào bộ nhớ Rom code của SoC mà lại chia thành 2 phase?
> - Rom code của SoC thường có size rất nhỏ chỉ đủ để thực thi vài lệnh nên việc load cả chương trình vào là không thể.
> - Phase từ MLO tới Uboot là nằm trong code của uboot, mà code uboot là code mã nguồn mở, có tính linh động cao, support được nhiều dòng chip và không phải các chip đều có ngoại vi giống nhau. **Vì vậy cần cách li phần cấu hình uboot sang 1 phase riêng (phase 2) để cho phù hợp với SoC đang dùng, đảm bảo sự linh động của uboot**
***
# II. Build Uboot và kernel
<a id="ii-build-uboot-va-kernel"></a>
[Digikey Uboot](https://www.youtube.com/redirect?event=video_description&redir_token=QUFFLUhqbFVQTDVTS242WEVjUVdUYV8yZUxJY3lzNE1mQXxBQ3Jtc0trTm9KdkJJcFlib3kyUzZWLXlzUjZFZGFibDhIbE1IeUFEYTVIeERzSDV1UlZuckE4SFUtZU5mX3dBZW1mR2hTNFJBdGNZdXNka25ZVnJVLXQyNlRFOTNfNWFvTk1qWTltQ05iMHlZOUN4SVdKWWJ0TQ&q=https%3A%2F%2Fforum.digikey.com%2Ft%2Fdebian-getting-started-with-the-beaglebone-black%2F12967&v=RfEtorHpThU)
## 1. Tải compile gcc
```wget -c https://mirrors.edge.kernel.org/pub/tools/crosstool/files/bin/x86_64/11.5.0/x86_64-gcc-11.5.0-nolibc-arm-linux-gnueabi.tar.xz```

- Compile này để build kernel 
## 2. Export ra để dễ gọi compile
```export CC32=`pwd`/gcc-11.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-```
## 3. Build code Uboot
- Tải uboot về rồi build (xem link đầu chapter)
## 4. Build kernel
- Tải kernel về rồi build (xem link đầu chapter)
***
# III. Build root file system (rootfs)
**Notes:**
- Là các ứng dụng trong OS

```wget -c https://rcn-ee.com/rootfs/eewiki/minfs/debian-13-minimal-armhf-2025-04-02.tar.xz```

## 1. Copy uboot, kernel, rootfs vào thẻ nhớ
- Cắm thẻ nhớ vào Ubuntu
- Export usb ra biến môi trường: ```export DISK=/dev/sdb```
- Xóa phân vùng thẻ nhớ: ```sudo dd if=/dev/zero of=${DISK} bs=1M count=10``` - ghi 0 vào DISK với 10 sector, mỗi sector 1MB
- Copy binary của Uboot vào thẻ nhớ: ```sudo dd if=MLO of=${DISK} count=2 seek=1 bs=128k``` - với sector là 128K và offset(seek) là 1 là dịch đầu ghi lên 128K, sau đó mới chép MLO vào. 128K = 131072Byte, mà block sector của thẻ thường là 512bytes. Vậy thì ta đã lưu MLO ở sector thứ 131072/512=256 để Rom code có thể đọc (Rom code luôn đọc ở địa chỉ nhất định)

    >**Notes:** Trong bbb, nó phân vùng SoC, trong SoC có chứa 1 đoạn Rom code. Trong Rom code, khi nó cấp nguồn, thì bbb sẽ boot từ địa chỉ default. Vì boot bằng sdcard nên trong Rom code quy định: khi mở nguồn, bbb sẽ copy lệnh từ địa chỉ nhất định của sdcard: đây là địa chỉ để mapping khối MLO để chip có thể thực thi lệnh MLO
- Copy code bootloader: `sudo dd if=./u-boot/u-boot-dtb.img of=${DISK} count=4 seek=1 bs=384k`
- Phân vùng thẻ nhớ: thẻ sẽ chia ra 2 phân vùng chính: boot(fat32 - chứa MLO và uboot) và ext4. 
    ```
    sudo sfdisk ${DISK} <<-__EOF__
    4M,7475M,L,*
    __EOF__
    ```
## 2. Copy thành phần khác vào sdcard
- `cd kernelbuildscripts`
- Check kernel version: `cat kernel_version`
- Export nó: export kernel_version=số kernel từ lệnh trên
- Mount thẻ nhớ vào ubuntu: 
    ```
    - sudo mkdir /media/rootfs
    - sudo mkfs.ext4 -L rootfs -O ^metadata_csum,^64bit ${DISK}1
    - sudo mount ${DISK}1 /media/rootfs
    ```
- Copy rootfs:
    ```
    sudo tar xfvp ./debian-*-*-armhf-*/armhf-rootfs-*.tar -C /media/rootfs/
    ```
- Ghi kernel_version vào boot/uEnv.txt: để chương trình boot lên biết đang chạy ver nào
    ```
    sudo sh -c "echo 'uname_r=${kernel_version}' >> /media/rootfs/boot/uEnv.txt"
    ```
- Copy kernel image vào sdcard
    ```
    sudo cp -v ./deploy/${kernel_version}.zImage /media/rootfs/boot/vmlinuz-${kernel_version}
    ```
- Copy device tree
    ```
    - sudo mkdir -p /media/rootfs/boot/dtbs/${kernel_version}/
    - sudo tar xfv ./deploy/${kernel_version}-dtbs.tar.gz -C /media/rootfs/boot/dtbs/${kernel_version}/
    ```
- Copy file system table
    ```
    sudo sh -c "echo '/dev/sdb1 / auto errors=remount-ro 0 1' >> /media/rootfs/etc/fstab"
    ```
- Umount sdcard
    `sudo umount /media/rootfs`
***
# IV. Uboot Basic Concept & Uboot architecture
## 1. Uboot
- [I. Linux booting sequence](#i-linux-booting-sequence)
- Vai trò chính của uboot: load nhân của hệ điều hành lên DRam
### 1.1 Phase 1: Rom code - first stage loader
![alt text](image.png)
- Là mã nguồn được nhà sản xuất chip ghi sẵn vào flash của chip, khi cấp nguồn, chip đầu tiên sẽ chạy rom code
- Nhiệm vụ: init các clock cơ bản, load phase 2 lên bộ nhớ SRAM của chip (SPL ở bộ nhớ).
### 1.2 Phase 2: SPL - second program loader
![alt text](image-1.png)
- Sau khi rom code đã load được SPL lên ram rồi, CPU sẽ thực thi code ở vùng SRAM (SRAM rất bé).
- SPL sẽ khởi tạo các ngoại vi cao hơn: DDR controller (mở rộng bộ nhớ RAM để load phase 3 - TPL)
### 1.3 Phase 3: TPL - third program loader
![alt text](image-2.png)
- DRAM đã được khởi tạo 
- Loader code kernal vào DRAM, trao quyền cho kernel để phân chia tài nguyên hệ thống, khởi chạy hệ điều hành

### 1.4 Đặt vấn đề
- **Làm sao Uboot biết địa chỉ để load SPL?**
    + Địa chỉ này được lập trình trong Rom code, phụ thuộc hãng sản xuất và từng SoC.
    + spruh73q.pdf - rom code có 2 chế độ boot
        - 26.1.8.5.5 Raw mode: có ghi địa chỉ mà Rom code sẽ lấy lệnh từ sdcard/mmd. Rom code sẽ nhảy tới 4 địa chỉ mặc định và quét các địa chỉ đó, và kiểm tra xem vùng địa chỉ đó tồn tại CHSETTINGS hay không? CÓ thì rom thực thi lệnh. Dùng lệnh `sudo dd if=MLO of=${DISK} count=2 seek=1 bs=128k` để đặt MLO vào sdcard tại offset 128k, chiều dài tối đa là 2*128 và ROM Code chỉ đọc vài byte đầu tiên (tối đa là 1 sector 512 byte)
        - 26.1.8.5.6 FAT mode: phân vùng thẻ thành FAT, lưu file MLO vào, sau đó rom code sẽ tìm file có tên là MLO và thực thi lệnh.
        - Dùng lệnh `sudo hexdump -C -s 0x20000 -n 100 /dev/sdb` với 0x20000 là  thể hiện cho offset 128KB
- **Làm sao SPL biết địa chỉ để load TPL?**
    + Cpu cần biết địa chỉ để đọc TPL và địa chỉ đó được ghi trong u-boot/.config
    + u-boot/.config: là file chứa config chung cho phase SPL
        - CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_USE_SECTOR=y: dùng Raw mode
        - CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR=0x300: địa chỉ để load code TPL, vì vậy cần lưu code TPL vào 0x300
        - `sudo dd if=./u-boot/u-boot-dtb.img of=${DISK} count=4 seek=1 bs=384k`: lệnh để ghi TPL vào 0x300 
            + 384 * 1 * 1024 = 0x60000 - là địa chỉ ghi TPL xuống thẻ nhớ byte thứ 60000
            + 0x300: TPL đặt lại sector 0x300
            + Hoặc có thể hiểu 0x300(hex) = 768(dec), mà 1 sector của sdcard thường là 512 nên 768 * 512 = 60000
                > Cpu sẽ thực thi code ở byte 0x60000
- **Làm sao TPL biết địa chỉ để load kernel?**
    + Cpu cần biết địa chỉ để TPL load kernel và địa chỉ đó được ghi trong u-boot/.config
    + u-boot/.config:
        - CONFIG_SYS_LOAD_ADDR=0x82000000: cần đặt sẵn file kernel vào địa chỉ này để load được kernel. 
        - Có thể thay đổi địa chỉ load kernel thông qua uEnv.txt hoặc có thể sửa code uBoot

    > Những câu hỏi trên sẽ cần để port Uboot lên 1 board khác ngoài BBB

## 2. Modify Uboot
### 2.1 Tại sao cần modify Uboot
> Code Uboot rất linh động, có thể chạy trên nhiều nền tảng:x86, arm,... Mỗi dòng board đều gắn các linh kiện khác nhau: có/không sdcard, có/không emmc, ..., việc cần làm là thay đổi code Uboot để boot kernel dưới các điều kiện khác nhau.

### 2.2 uEnv.txt - custom Uboot
- thường được lưu ở /boot/uEnv.txt
- Khi uboot chay, nó đọc file uEnv.txt
- viết script để chỉnh sửa hành vi uboot
- Command uboot
    - `Loadaddr <destination address> <file to load>`: load file tới đích
    - `Setenv <variable> <value>`: set value cho 1 biến trong uboot
    - `Printenv <variable>`: in biến môi trường trong uboot ra
    - `Run`: chạy bất cứ biến môi trường nào của uboot

### 2.3 Script uboot hoạt động như nào
![alt text](image-3.png)
- Uboot là tập hợp nhiều biến môi trường. Nó sẽ setup các varible sau đó sắp các biến đó thành List variable và thiết lập mối liên kết giứa các biến môi trường. Sau đó dùng run để thực thi biến môi trường
- Nhấn space liên tục lúc boot để vào uboot ở bbb
- uEnv.txt example:
    > bootargs=console=tty00,115200 root=/dev/mmcblk0p1 rw
bootcmd=echo "Running bootcmd ..."; load mmc 0:1 0x82000000 /boot/vmlinuz-5.4.288-bone69; load mmc 0:1 0x880000 /boot/dtbs/5.4.288-bone69/am335x-boneblack.dtb; bootz 0x82000000 - 0x88000000;
boot=echo "Running boot script use /boot/uEnv.txt"; run bootcmd;

# V. Linux OS structure
## 1. Cấu trúc của Linux OS
![alt text](image-4.png)
- Bootloader: có chức năng load kernel vào bộ nhớ. U-boot là 1 trong các phiên bản bootloader thường được dùng, nó support nhiều kiến trúc, size nhỏ, dùng để load kernel 
- Kernel: quản lý tài nguyên, task, process, thread, quản lý bộ lập lịch
- Root file system: là hệ thống file được public tới người dùng, để người dùng tương tác thông qua file

## 2. Build Root basic concept
- Buildroot là dự án mã nguổn mở gồm các script và makefiles để tự động hóa build hệ thống
### 2.1 Buildroot bao gồm:
+ Toolchain: là các compile, hỗ trợ build chéo từ máy host ra image cho target
+ Kernel: buildroot có thể tự động tải kernel source, apply patches, cấu hình tùy chọn kernel dựa vào target platform
+ Root filesystem: chứa các thư viện, binary cần thiết
+ Bootloaders: hỗ trợ nhiều BL bao gồm Uboot
- Output của buildroot: output nằm trong sdcard và boot từ sdcard
    + Uboot image
    + kernel image
    + root file system
    + device tree binary
### 2.2 Cách build bằng buildroot
- Vào folder buildroot-2026.02
- chạy các lệnh như trong file: buildroot-2026.02/board/beagleboard/beaglebone/readme.txt
- cầu hình trong menucondfig: Xem `Linux Embedded #22 Build Root build and generate package` - lưu ý TTY port cần set là ttyS0

### 2.3. Output buildroot
- /buildroot-2023.08/output/images/
- sdcard.img: gồm tất cả mọi thứ (bootloader, kernel, ...)
- To copy the image file to the sdcard use dd:
    + umount /dev/sdx
    + dd if=output/images/sdcard.img of=/dev/XXX
    + thẻ nhớ sẽ được chia thành 2 vùng 16M (để boot) và 512M (rootfs)

# VI. Linux kernel
## 1. Kernel
- Hầu hết các hệ thống có ứng dụng hệ điều hành (window, linux, macos) đều có khái niệm nhân
- Nhân là trái tim hệ điều hành, có nhiệm vụ:
    - quản lý tài nguyên hệ thống, process, task 
    - nó giống như 1 cầu nối giữa tầng người dùng với phần cứng.

## 2. Kernel module
### 2.1 Khái niệm
- là những module được viết để thực thi trong kernel
- nó có khả năng load và unload vào kernel
- có khả năng mở rộng tính năng của kernel mà không cần reboot lại cả hệ thống linux (window thì cần khởi động lại, chính vì thể nhiều hệ thống server dùng linux để tránh việc khởi động lại làm mất dữ liệu, tốn năng lượng khởi động)
- Ưu điểm: 
    + bản thân kernel module không cần build với nhân, vì vậy giảm được size của kernel, giúp code kernel linh động hơn
    + không cần build lại kernel khi thay đổi driver, tiết kiệm thời gian, chi phí
    + Không cần reboot khi có update kernel module

> ### Bản chất tại sao có thể load và unload kernel runtime được? Tới 2 phần linking tiếp theo
### 2.2 Static linking
- Link tất cả file obj thành 1 file thống nhất
- Tất cả code thành 1 file linking duy nhất
- Linking time: xảy ra ở lúc compile
- Flexible: ít linh động, sửa 1 thì phải build lại cả chương trình
- Hiệu năng: khởi chạy nhanh hơn dynamic linking 
### 2.3 Dynamic linking
- Link các thư viện bên ngoài
- FIle thực thi khi build ra khá nhỏ vì lúc nào cần dùng thư viện nào thì nó mới link vào chương trình
- Linking time: xảy ra ở lúc runtime
- Flexible: linh động hơn, dễ maintain, sửa thư viện nào thì build lại thư viện đó thôi
- Hiệu năng: khởi chạy chậm hơn static linking
- Code example: codeExamples/dynamicLinking
> ### Cơ chế loadable của kernel module tương tự như Dynamic linking. 
> ### File kernel module đã build rồi load vào kernel bản chất là link code vào kernel
> ### Người ta ưa thích dynamic linking cho kernel vì kernel nhỏ thì hệ thống load nhanh

### 2.4 Kernel module example
- codeExamples/kernelModule
- 2 thư viện quan trọng
    - linux/module.h: cung cấp API (module_init, module_exit)
    - linux/init.h: 
- Kernel dùng kbuild để build code
- Chỉ có quyền sudo mới load, unload, ... kernel (sudo -s)
- Load kernel: `insmod kernel.ko`
- Unload kernel: `rmmod kernel.ko`

### 2.5 Tương tác file trong linux
- codeExamples/file_handling
- tất cả đối tượng của linux đều là file
- Khi load code của kernel module vào kernel thì linux sẽ tạo ra 1 file để đại diện cho kernel module đó 
- Application sẽ tương tác với kernel module bằng device file đó
- API để tương tác với file: check lại `man7.org`
- Linux quản lý file qua file table, linux quản lý các file qua trường index và tương tác qua file nào thì cần dùng index của file đó và tìm index qua hàm open()

### 2.6 Device file concept
![alt text](image-5.png)
- Khi link kernel module vào kernel, linux sẽ tạo ra 1 device file
- Device file là file trong linux, đại diện cho 1 module
- Module đó đại diện cho 1 hardware tương ứng
- Hiểu đơn giản là hardware đó cần 1 trình điều khiển để user tương tác được thông qua software
- Flow: 
    + user tương tác với kernel module thông qua device file này
    + device file này gửi dữ liệu xuống kernel module 
    + kernel module tương tác với phần cứng

#### 2.6.1 Tạo device file
- codeExamples/device_file
- Có 3 cách tạo device file
    - dùng command mknod
    - dùng thư viện udev
    - dùng Misc module: build 1 kernel module dạng misc module

### 2.7. Cross Compile
- codeExamples/cross_compile
- Để các file execute chạy được trên BBB thì file đó phải được build với tập lệnh của arm
- Tiết kiệm hiệu năng cho target machine vì code đã được build ở máy khác
- `Khi cross compile kernel, phiên bản header của bbb và pc phải giống nhau`
- `Khi cross compile app, phiên bản header để build nhỏ hơn hoặc bằng header kernel của bbb`

# VII. BeagleBone Black
## 1. Configue pin mux
- Để biết pin có mux gì thì cần biết thanh ghi nào
- Xem `spruh73q.pdf - 9. Control module` ghi địa chỉ các thanh ghi để cấu hình pin mux
- Lệnh check nhanh pin mux trên board
    - `sudo cat /sys/kernel/debug/pinctrl/44e10800.pinmux-pinctrl-single/pins`
    - pin 0 (PIN0) 0:gpio-0-31 44e10800(offset) 00000031(mode) pinctrl-single
- Code sáng led khi load kernel: `codeExamples/led_init`
## 2. Build code với kernel header 
- Khi build cross compile kernel, cần trỏ tới đường dẫn KERNEL với số phiên bản đúng như ở BBB
    - Sửa các dòng sau trong KERNEL/Makefile về phiên bản như ở BBB:
        - VERSION = 6
        - PATCHLEVEL = 18
        - SUBLEVEL = 23
        - EXTRAVERSION = -bone28
        - chạy `make ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabihf- LOCALVERSION= modules_prepare`
    - Nếu bị lỗi hiện dấu '+' ở sau phiên bản kernel, chạy lệnh:
        - `make ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabihf- LOCALVERSION= modules_prepare`

## 3. Code với kernel
- con trỏ ở tầng use và kernel không tương thích với nhau nên khi trao đổi dữ liệu, cần dùng `copy_from_user` để lấy data từ user
- Khi write trong kernel, cần read từ pin ra trước rồi mới ghi để keep những giá trị bit khác và chỉ thay đổi bit mình muốn

# VIII. Device tree
## 1. Device tree là gì
- Sửu dụng để mô hình hóa lại các cấu hình phần cứng
- Ví dụ stm32 có 2 ngoại vi timer, 4 ngoại vi uart, ... Mỗi ngoại vi có cấu hình đi kèm: baud rate, config,... Bình thường, mình sẽ define cấu hình của các ngoại vi ở 1 file riêng dạng struct, array, ... và nạp xuống vdk.
    - Nhược điểm: 
        - Muốn đổi cấu hình baudrate chẳng hạn -> cần đổi cấu hình và build lại source -> tốn thời gian update từ board này qua board khác
        - Phức tạp, có thể cấu hình sai vì không có 1 chuẩn chung
- Từ khó khăn trên, người ra định nghĩa ra device tree
- Device tree là cây chứa cấu hình phần cứng, nó nằm trong bộ nhớ thiết bị dưới dạng file nhị phân (binary).
    - File nhị phân này nằm ở 1 vùng bộ nhớ
    - Khi update chỉ cần update file nhị phân này
    - Tránh build lại toàn project
    - Dễ phát triển driver mà không cần phụ thuộc phần cứng nhiều

## 2. Platform driver
- Là 1 driver được khởi chạy trong thời gian khởi động của OS
- Nó phân tích device tree, biết được cấu hình hardware -> thực hiện action để khởi tạo hệ thống
- Ví dụ: khi booting, mình cần ethernet, platform driver sẽ kiểm tra device tree để tìm cấu hình thích hợp cho ethernet -> nó sẽ khởi tạo ngoại vi tương ứng
- Khi viết platform driver, nó có 1 số API đặc trưng:
    - `struct resource *platform_get_resource(struct platform_device *pdev, unsigned int type unsigned int n);` - đọc device tree và lấy ra 1 node trong device tree, biết được thông tin
    - `struct resource *platform_get_resource_byname(struct platform_device *pdev, unsigned int type, const char *name);` - lấy resource theo tên
    - `int platform_get_irq(struct platform_device *pdev, unsigned int n);` - get thông tin của interrupt của 1 node trong device tree

## 3. Layout của device tree
- Bản chất nó là cấu trúc dữ liệu được build và nạp xuống bộ nhớ
- Tại thời điểm khởi động os, khối data này được os phân tích: các hệ thống liên quan sẽ được khởi tạo
- Ví dụ: 
    - thời điểm khởi động, cần khởi tạo cache, cpu, ngoại vi thì cần định nghĩa trong device tree
    - Khi port từ board A qua B, thì thay đổi config trong device tree
- File device tree có ký hiệu: 
    - .dts: file device tree gốc
    - .dtsi: file device tree có thể include vào file khác
    - .dtb: file binary output sau khi buld dts
    - .dtbo
- Cấu trúc của file device tree: `https://github.com/beagleboard/devicetree-source/blob/master/arch/arm/boot/dts/am33xx.dtsi`
    - `/ {` - Start bằng ký hiệu 
    -  `compatible = "ti,am33xx";` - mapping giữa device tree với driver điều khiển, nếu string này match với string được khai báo ở driver thì hàm proc sẽ được gọi để khởi tạo hệ thống
    - `ti,...`: define custom của riêng Ti, không có trong cú pháp device tree
    - `interrupt-parent = <&intc>;` - chỉ định hệ thống dùng interrupt controller nào
    - `#address-cells = <1>` - 1(chip 32bit) hoặc 2(chip 64bit), đặc trưng cho viết địa chỉ của register, ngoại vi
    - `#size-cells = <1>` - 1(32bit) dải địa chỉ của chip là 1 số 32bit hoặc 2(64bit) giải địa chỉ là 2 số 32bit
    - `chosen { };` 
        - để rỗng như này tức là trường này được define ở nơi khác, các trường thông tin có thể được ghi đè lẫn nhau
        - `base_dtb = "am335x-boneblack.dts`: device tree này build ra file device tree binary .dtb nào
        - `base_dtb_timestamp = __TIMESTAMP`: chỉ thị của compiler, record lại thời gian build để biết thông tin ngày giờ build, phiên bản build
    - `aliases {i2c0 = &i2c0, v.v`: là dạng define ngoại vi i2c, uart, spi, ...
    - `cpus {`: khai báo thông tin cpu (bao nhiêu nhân, compatible, ...)
    - `ocp:` - on chip peripherals - trường thuộc tính cho ngoại vi
        - node `edma`:
            - `edma@49000000`: sau tên là @ và đia chỉ base address
            - `reg =`: địa chỉ baseaddress và size
            - `reg-names`: tên của thanh ghi
            - `interrupts = <12 13 14>`: các line interrupt 0x12 0x13 0x14 mà edma có thể dùng
            - `interrupt-names`: tên interrupt tương ứng với line
            - `dma-requets = <64>`: số lượng requets dma mà driver có thể hỗ trợ 
            - `#dma-cells = <2>`: support cho cấu hình dma, 2 cell thì có 1 cell cho dma controller và 1 cell cho dma channel. 
    - `status = "okay"`: ngoại vi này có nên enable hay không

## 4. Ví dụ mẫu cho code device tree
- `codeExamples/device_tree`
### 4.1. Thêm cấu hình trong file dts
- file dts có thể compile từ file dtb lấy từ BBB
    - `dtc -I dtb -O dts -o am335x-boneblack.dts am335x-boneblack.dtb`
- check `user-data` trong `codeExamples/device_tree/am335x-boneblack.dts`
### 4.2. Viết driver để parse cấu hình node device tree vừa thêm
- `codeExamples/device_tree/device_tree_BBB_kernel_module.c`
- Hàm `probe`: gọi ra khi device tree và driver matching với nhau, thực hiện các đoạn code để tải 
- Hàm `remove`: gọi khi driver được unload khỏi kernel, deinit các tài nguyên trong hàm `probe`
- `module_platform_driver(device_tree_driver)`: khởi tạo struct của driver

### 4.3. Build lại devicetree
- Chạy script `make build_dtb` để build lại .dtb
- Load lại file .dtb vào BBB:/boot/dtbs/$(uname -r)/
- Khởi động lại BBB
- Load kerner device_tree_BBB_kernel_module.ko vào parse device tree
- Kiểm tra /proc/device-tree xem đã có node mới thêm chưa
### Phần này chưa hoàn thiện, device tree đưa vào nhưng hàm probe không start

# IX. PWM driver
## 1. Ứng dụng của PWM
- Điều khiển động cơ
- Điều khiển điện áp
## 2. TỔng quan 
![alt text](image-7.png)
- Để viết và điều khiển được PWM ngoại vi cần:
    - Device tree cho PWM: define thông số hoạt động của PWM bằng device-tree
    - Viết PWM driver: viết và build file .ko rồi load .ko vào linux
    - Sau khi load pwm driver ko vào, os tiến hành quết danh sách device trong device-tree và tìm ra device và drive match nhau qua trường compatible -> hàm probe được gọi dể khởi tạo PWM

-- 43