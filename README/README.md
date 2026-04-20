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
- **Làm sao TPL biết địa chỉ để load kernel?**
    > Những câu hỏi trên sẽ cần để port Uboot lên 1 board khác ngoài BBB