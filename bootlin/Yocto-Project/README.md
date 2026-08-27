# Menu

# Introduction to Embedded Linux
## Simplified Linux system architecture 
- Kiến trúc chung của hệ thống Linux
- ![alt text](images/image.png)
## Overall Linux boot sequence
- ![alt text](images/image-1.png)
## Embedded Linux work - công việc về embedded linux
- BSP work: port bootloader và Linux kernel cho nhiều kiến trúc, phát triển Linux device driver
- system integration work: tích hợp tất cả thành phần thuộc user space cần cho hệ thống, cấu hình chúng, ...
- application development: viết các ứng dụng và thư hiện cụ thể
## Embedded Linux build system: principle
- nguyên lý của hệ thống build Linux embedded:
    + ![alt text](images/image-2.png)
    + biên dịch từ source code nên rất linh hoạt
    + tận dụng được sức mạnh CPU từ việc cross compile
    + sử dụng các recipe để việc build dễ dàng hơn
## Embedded Linux build system: tools
- có nhiều giải pháp để build hệ thống linux: Yocto/OpenEmbedded, buildroot, ...

# Yocto Project and Poky reference system overview
## The Yocto project overview
### Yocto: principle
- ![alt text](images/image-3.png)
- Yocto luôn build ra các gói binary
- Cuối cùng yocto sẽ build ra rootfs
### Lexicon: bitbake - thuật ngũ: bitbake
- trong Yocto/OpenEmbedded, bitbake đóng vai trò là build engine
- bitbake tương tự như make, nó phân tích các text file để biết build như nào
### Lexicon: recipes
- Loại file chính mà bitbake phân tích là `recipes`, mỗi `recipes` mô tả 1 component software cụ thể
- Mỗi `recipes` mô tả cách fetch và build 1 software component như: 1 program, 1 library, 1 image
- `recipes` có cú pháp cụ thể
- `bitbake` có thể build bất cứ recipe nào, nó sẽ tự build các thành phần phụ thuộc
- ![alt text](images/image-4.png)
### Lexicon: tasks
- quá trình build được thực hiện bởi 1 `recipe` được chia thành nhiều `tasks`
- mỗi task thực hiện 1 bước build cụ thể: fetch, configure, compile, package
- các task có thể phụ thuộc lẫn nhau
- ![alt text](images/image-5.png)
### Lexicon: metadata and layers
- input của `bitbake` được gọi là `metadata`
- `metadata` bao gồm các file config, recipes, classes và include files
- `metadata` được bố trí trong `layers`, các layers này có thể kết hợp với nhau để tạo ra các thành phần khác nhau
    + 1 `layer` là tập hợp của các `recipes`, `config files` và `classes` phục vụ cho 1 mục đích chung
        - ví dụ cho board của TI, cần layer `meta-ti-bsp` 
    + nhiều `layer` có thể được sử dụng, tùy thuộc vào mục đích và sự cần thiết
### `openembedded-core` là core layer của Yocto project
+ là khung xương của Yocto
+ tất cả các layer khác được build dựa trên layer `openembedded-core`
+ nó hỗ trợ nhiều kiến trúc như ARM, MIPS, ...
+ nó cũng hỗ trợ QEMU để mô phỏng những kiến trúc trên
### Lexicon: Poky
- Poky có nhiều nghĩa:
    + Là git repo được tập hợp từ: bitbake, openembedded-core, yocto-docs và meta-yocto
    + Hoặc có thể là 1 bản phân phối được cung cấp bởi Yocto project
- `meta-poky` là layer được cung cấp bởi các bản phân phối poky
### The Yocto Project lexicon
- ![alt text](images/image-6.png)
- Yocto có thể được thêm bớt các layers tùy thuộc target
### Example of a Yocto Project based BSP
- để build 1 image cho BBB, ta cần
    + Poky reference system, gồm toàn bộ các recipes và tools
    + layer `meta-ti-bsp`: tập hợp các recipes của TI
- nếu muốn sửa layer, cần tạo 1 custom layer để sửa, không được tự ý sửa trong các layer của bên thứ 3 hoặc vendor
## The Poky reference system overview
### Getting the Poky reference system
- `https://git.yoctoproject.org/`
- `git clone -b scarthgap https://git.yoctoproject.org/git/poky`
- cứ 6 tháng thì có 1 bản mới được release và được bảo trì trong 7 tháng
- Bản LTS được bảo trì 4 năm
- Mỗi bản release có 1 code name, ví dụ: `kirkstone` hoặc `scarthgap`
- các bản release ở `https://wiki.yoctoproject.org/wiki/Releases`
### Poky
- cấu trúc của Poky trong Yocto project
- ![alt text](images/image-7.png)
### Poky source tree
- bitbake: chứa các script mà lệnh bitbake cần
- meta: chứa metadata của Openembedded-core
- meta-skeleton: chứa các recipe template cho BSP và kernel development
- meta-poky: chứa các cấu hình cho bản phân phối Poky
- meta-yocto-bsp: là layer phần cứng mẫu đi kèm, chứa các cấu hình để giúp linux chạy được trên device nhúng mà Yocto hỗ trợ
- oe-init-build-env: script để set up môi trường build, nó sẽ tạo folder build
- scripts: chứa script dùng để cấu hình môi trường, development tools và các tool để flash image vào target
### Documentation
- `https://docs.yoctoproject.org/singleindex.html`
- các thuật ngũ: `https://docs.yoctoproject.org/genindex.html`

# Using Yocto Project - basics
## Environment setup
### Environment setup
- Tất cả các file gốc của Poky không được phép sửa đổi khi muốn build 1 custom image, ta cần tạo layer riêng để sửa
- Các file cấu hình cụ thể và các folder trong lúc build được gom vào folder `build`
- `oe-init-build-env`: thiết lập thư mục build và các biến môi trường
### oe-init-build-env
- script này làm thay đổi môi trường làm việc của terminal hiện tại nên cần dùng thêm lệnh `source`
- script này thêm các biến môi trường để bitbake sử dụng
- giúp ta có thể dùng các lệnh của Poky
- `source oe-init-build-env <build-folder-name>`
### The initial build/ directory
- `oe-init-build-env` tạo 1 folder build và 1 sub-folder `conf` (chứa cấu hình cho layer và các cấu hình cho image)
### Exported environment variables
- Yocto project xuất ra các biến môi trường, trong đó:
    + BUILDDIR: đường dẫn tuyệt đối của thư mục build
    + PATH: chứa đường dẫn tới các executable programs, đường dẫn tới `scripts/` và `bitbake/bin/` sẽ được chèn vào
### Available commands
- `bitbake`: lệnh build chính, dùng để thực hiện các task của recipe (download, configure, compile, ...)
- `bitbake-*`: các lệnh cụ thể liên quan tới bitbake build engine
## Configuring the build system
### The build/conf/ directory
- thư mục `build/conf/ chứa 2 file cấu hình
    + bblayers.conf: danh sách các layers cần dùng
    + local.conf: cấu hình các biến liên quan tới việc build
    + site.conf: (optional) cấu hình các biến dùng cho network, CPU/memory usage
### Configuring the build
- cấu hình `conf/local.conf`:
    + `BB_NUMBER_THREADS`: số lượng task thực hiện đồng thời, mặc định là bằng số thread của CPU đang có
    + `PARALLEL_MAKE`: số lượng process được dùng khi compile, mặc định là bằng số thread của CPU đang có
    + `MACHINE`: target mình muốn build (ví dụ `beaglebone`)
## Building an image
### Compilation
- Việc biên dịch được thực hiện bởi bitbake build engine
- `bitbake [options] [recipename/target ...]`
- để build 1 target: `bitbake [target]
- build image tối giản: `bitbake core-image-minimal`, lệnh này thực hiện build toàn bộ cho target được chọn
### The build/ directory after the build
- ý nghĩa các folder 
    - conf/: chứa các file cấu hình
    - downloads/: các file mã nguồn, gói cài đặt được tải về từ internet
    - sstate-cache/: bộ đệm để bitbake tái sử dụng
    - tmp/: chứa toàn bộ output của build system
    - tmp/work/: các đường dẫn được phân loại theo kiến trúc, được dùng để giải nén, cấu hình, buid package. Nó chứa patch source, các file object, log
    - tmp/sysroots/: chứa các shared libraries và header để compile app cho target và host
    - tmp/deploy/: output cuối cùng của việc build
    - tmp/deploy/images/: chứa image được build bởi OpenEmbedded build system, image này dùng để flash vào target
    - tmp/buildstats/: file log phân tích quá trình build (CPU usage, time, ....)
## Thực hành
- chạy lệnh source poky/oe-init-build-env trước
- cấu hình trong các file `bblayers.conf`, `local.conf`
- `bitbake core-image-minimal`
- output nằm ở `$BUILDDIR/tmp/deploy/images/beaglebone`
- flash vào sdcard: `xz -dc /home/as/Desktop/linuxEmbeddedBBB/bootlin/Yocto-Project/yocto-bbb-labs/build/tmp/deploy/images/beaglebone/core-image-minimal-beaglebone.rootfs-20260823132757.wic.xz | sudo dd of=/dev/sda conv=fdatasync bs=4M status=progress`
- output của Yocto đã bao gồm: bootloader, kernel, dtb, rootfs

# Using Yocto Project - advanced usage
## Advanced build usage and configuration
- 4 chủ đề nâng cao trong việc build Yocto
    + Variable operators and overrides. các toán tử gán, nối chuỗi, cơ chế đè dữ liệu
    + Select package variants. cách lựa chọn phiên bản của từng gói phần mềm
    + Manually add packages to the generated image. cách thêm thủ công các gói phần mềm mong muốn
    + Run specific tasks with BitBake. điều khiển Bitbake build tác vụ theo ý muốn
## A little reminder
- Recipe: mô tả các fetch, cấu hình, compile và cài đặt các software component (app, lib, ...)
- Các task có thể chạy độc lập (nếu dependencies của chúng đáp ứng được)
- các package có sẵn trong layer mặc định không được chọn để build, cần cấu hình để chọn cái gì được build cùng image
- 1 số recipe cung cấp cùng 1 chức năng, vì vậy có thể cần cấu hình để chỉ định dùng cái nào
## Variables
### Overview
- OpenEmbedded build system dùng các biến để lưu thông tin
- tên biến thường đươc viết in hoa
- giá trị là string
- Biến được định nghĩa trong `Configuration Files (*.conf)` có scope là global
- Biến được định nghĩa trong `Recipes (*.bb, .bbappend, .bbclass)` có scope là local
- Các recipe có thể truy cập biến global
- **Expansion time**: là thời điểm mà hệ thống thực sự đọc giá trị cuối cùng của biến, thay vì thời điểm khai báo biến đó
### Operators: 
- basic assignment
    - `VAR = "value"`
    - xuống dòng thì dùng `\`: 
        ```c
        LIST = "this \ 
                and that"
        ```
    - gán tên biến làm giá trị trong 1 biến khác
        ```c
        COLOUR = "blue"
        SKY = "the sky is ${COLOUR}"
        ```
- immediate expansion
    + phép toán `:=` ngay lập tức gán giá trị, không delay
    ```c
    COLOUR = "blue"
    SKY := "the sky is ${COLOUR}"
    COLOUR = "grey"
    PHRASE = "Look, ${SKY}"
    ```
        - COLOUR ở dòng thứ 2 ngay lập tức lấy "blue" gán luôn, không bị trì hoãn tới dòng thứ 4
    + phép toán `=` không tính ngay
    ```c
    COLOUR = "blue"
    SKY = "the sky is ${COLOUR}"
    COLOUR = "grey"
    PHRASE = "Look, ${SKY}"
    ```
        - COLOUR ở dòng 2 chưa tính, mà đến dòng thứ 4 dùng thì mới gán COLOUR là "grey"
    + chỉ nên dùng `:=` khi cần, còn đa số cứ dùng `=`
- appending and prepending: thêm vào cuối hoặc đầu
    + `+=`: thêm vào cuối cùng khoảng trắng
    + `=+`: thêm vào đầu cùng khoảng trắng
    + `.=`: thêm vào cuối không có khoảng trắng
    + `=.`: thêm vào đầu không có khoảng trắng
- default and weak default values: `?=` và `??=`
    + `?=`: kiểm tra xem biến đã có giá trị từ trước chưa, nếu rồi thì dùng giá trị trước đó, không thì dùng giá trị được gán vào
        - ![alt text](images/image-8.png)
    + `??=`: yếu hơn `?=` và `=`, nếu biến đã được gán giá trị từ `?=` hoặc `=` thì giá trị của `??=` không còn tác dụng, kể cả khi `??=` đặt trước hay sau lệnh `?=`/`=`
- caveats - lưu ý
    + Kết hợp toán từ `?=` và `+=` (...) có thể gây sai lệch 
        - ![alt text](images/image-9.png)
    + trong 1 project có nhiều file cấu hình (conf, bb, ..) không thể biết được thứ tự các toán tử. **Vì vậy cần tránh dùng `+=`, `=+`, `.=`, `=.` trong file `conf/local.conf`. Ta nên cùng cơ chế `override`**
### bitbake-getvar
- bitbake-getvar được dùng để hiểu và debug biến
- nó liệt kê được các file có cấu hình giá trị biến, kể cả giá trị trước và cuối cùng
- `bitbake-getvar <VARIABLE>`: hiển thị giá trị của biến global scope
- `bitbake-getvar -r <recipe> <VARIABLE>`: hiển thị giá trị của biến local scope trong recipe
### overrides
- Cơ chế override của Bitbake cho phép append, prepend hoặc modify giá trị của 1 biến tại thời điểm Expansion time, khi giá trị của biến thực sự được đọc
- Cú pháp override: `<VARIABLE>:<override> = "value"`
- Override để modify giá trị của biến
    + thêm giá trị vào cuối biến (không có space)
        - `IMAGE_INSTALL:append = " dropbear"`
    + thêm giá trị vào đầu biến (không có space)
        - `PATH:prepend = "home/as/:"`
    + xóa giá trị khỏi biến
        - `IMAGE_INSTALL:remove = "i2c-tools"`
### Order of variable assignment
- ![alt text](images/image-10.png)
- Thứ tự khi gán giá trị cho biến
    + các toán tử sẽ gán giá trị trước
    + các toán tử append, prepend, remove sẽ thay đổi giá trị của biến theo thứ tự append đổi trước, prepend tiếp theo, cuối cùng là remove bất kể thứ tự dòng code là gì
### Overrides for conditional asignment
- Khi khai báo 1 biến `OVERRIDES="dung:manh:mai" thì nếu biến chứa 1 trong các string này, có thể gán giá trị dựa theo string đó
- Ví dụ ![alt text](images/image-11.png)
    + `OVERRIDES` định nghĩa các giá trị cho phép
    + Biến KERNEL_DEVICETREE sẽ thay đổi giá trị tùy vào chuỗi trong `OVERRIDES`
    + giá trị của `OVERRIDES` do hệ thống sinh ra dựa theo kiến trúc cpu
- precedence: độ ưu tiên của các lệnh gán giá trị
    + lệnh nào càng chi tiết cụ thể về target, lệnh đó sẽ được ưu tiên hơn
    + ví dụ nếu khai báo:
    ```c
    IMAGE_INSTALL:beaglebone = "busybox mtd-utils i2c-tools"
    IMAGE_INSTALL = "busybox mtd-utils
    ```
        - hệ thống nhận thấy lệnh `IMAGE_INSTALL:beaglebone` cụ thể hơn cho target nên nó sẽ lấy giá trị của lệnh này và bỏ qua lệnh dưới
- combining overrides: kết hợp các phương thức override
    + nếu ra có lệnh 
    ```c
    IMAGE_INSTALL = "busybox mtd-utils"
    IMAGE_INSTALL:append = " dropbear"
    IMAGE_INSTALL:append:beaglebone = " i2c-tools"
    ```
        - kết quả sẽ là `IMAGE_INSTALL = "busybox mtd-utils dropbear i2c-tools"` nếu machine được chỉ động là `beaglebone`
        - nếu không thì kết quả là IMAGE_INSTALL = "busybox mtd-utils dropbear"
## Virtual provides
- đặt vấn đề: có thể có nhiều recipe có chung chức năng nhưng mỗi thời điểm chỉ có thể dùng được 1
- để giải quyết, bitbake sử dụng cơ chế `virtual providers` để tạo 1 tên gọi chung cho chức năng đó
- Khi build, chỉ có 1 recipe được lựa chọn để đáp ứng nhu cầu và tích hợp vào image
### Variant examples
- virtual provider thường có tên theo dạng `virtual/<name>`
- Ví dụ:
    + virtual/bootloader: u-boot, u-boot-ti-staging
    + virtual/kernel: linux-yocto, linux-yocto-tiny, linux-yocto-rt
    + virtual/libc: glibc, musl, newlib
    + virtual/xserver: xserver-xorg
### Provider selection
- virtual provider được cấu hình qua biến `PREFERRED_PROVIDER` (thêm suffix này vào trước)
- Ví dụ:
    + `PREFERRED_PROVIDER_virtual/kernel ?= "linux-ti-staging"`
    + `PREFERRED_PROVIDER_virtual/libgl = "mesa"`
### Version selection
- mặc định, bitbake sẽ build recipe có phiên bản cao nhất từ layer có độ ưu tiên cao nhất trừ khi recipe có set `DEFAULT_PREFERENCE = "-1"`
- khi có nhiều phiên bản recipe khả dụng, ta có thể chọn đích danh 1 phiên bản với `PREFERRED_VERSION`
- ví dụ:
    + `PREFERRED_VERSION_nginx = "1.20.1"`
    + `PREFERRED_VERSION_linux-yocto = "5.14%"` (%: chỉ định nhóm version 5.14.x)
## Selection of packages to install
- Việc build recipe sẽ tạo ra các gói binary
- số lượng pkg được cài vào image được phụ thuộc vào target mình chọn (core-image-minimal, ...)
- ta có thể tự custom các pkg muốn cài vào image
- Trong quá trình debug hoặc phát triển, có thể thêm pkg mà không cần sửa recipe
- packages được quyết định bởi biến `IMAGE_INSTALL`
## The power of BitBake
### Common BitBake options
- Bitbake có thể dùng để build toàn bộ cho 1 target với lệnh `bitbake [target]`
    + target: là tên của recipe
    + ví dụ: `bitbake ncurses`, `bitbake ncurses-native`
- Có thể chèn thêm các optione
    + `-c <task>`: thực thi task
    + `-s`: liệt kê các recipe khả dụng và version của nó
    + `-f`: buộc task phải chạy
    +`world`: build tất cả recipes
- ví dụ:
    + `bitbake -c listtasks virtual/kernel`: liệt kê các task khả dụng cho gói `virtual/kernel`
    + `bitbake -c menuconfig virtual/kernel`: thực thi task `menuconfig` cho recipe `virtual/kernel`
    + `bitbake -f dropbear`: buộc recipe dropbear phải chạy tất cả các task của nó
    + `bitbake --runall=fetch core-image-minimal`: tải tất cả recipe source và dependence của chúng
    + `bitbake --help`
### shared state cache
- bitbake chứa output của mỗi task vào 1 đường dẫn, gọi là shared state cache
- shared state cache dùng để tăng tốc biên dịch
- đường dẫn của shared state cache được define bởi biến `SSTATE_DIR` và mặc định ở `build/sstate-cache`
- có thể xóa state cache bằng cách `find sstate-cache/ -type f -atime +30 -delete` (xóa các file không được truy cập quá 30 ngày)

## Thực hành 
- `bitbake -vn virtual/kernel`: xem phiên bản kernel nào đang được dùng
- Sau khi sửa file conf, chạy lệnh trên để check lại xem yocto load đúng mã nguồn mình chọn cho kernel hay chưa
- `bitbake -c listtasks virtual/kernel`: các task tồn tại trong virtual/kernel
- `bitbake -c <task> virtual/kernel`: task là tên sau chữ `do_` trong listtasks của virtual/kernel
- `bitbake --runall=fetch world`: tải tất cả source của các package và dependencies mà có trong các layer trong Yocto trên máy
- `bitbake -s`: liệt kê tất cả package local và phiên bản
- Các layer dùng có thể có recipe kernel với phiên bản khác nhau, vì vậy cần chọn 1 phiên bản qua virtual/kernel. linux-bb.org là 1 recipe nằm trong bootlin/Yocto-Project/yocto-bbb-labs/meta-ti/meta-ti-bsp/recipes-kernel/linux/linux-bb.org
- `PREFERRED_VERSION_linux-bb.org:beaglebone = "6.6%"` chọn phiên bản của recipe kernel. Chọn xong thì kiểm tra đúng bản hay chưa `bitbake -e linux-bb.org | grep "^PV="`
- `bitbake virtual/kernel`: build riêng kernel và dtb