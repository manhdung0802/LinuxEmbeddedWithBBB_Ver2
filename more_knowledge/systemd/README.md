# Khái niệm systemd
- systemd đóng vai trò là Init System, và có PID = 1
- công việc cuối cùng trong quá trình kernel start là gọi tiến trình `/sbin/init` trỏ tới `systemd` để khởi động `systemd`
- Lợi ích dùng systemd:
    + khởi động nhanh hơn nhờ start deamons song song
    + không cần quan tâm thứ tự khởi động của deamon
    + không cần script `/etc/init.d` phức tạp nữa, giờ chỉ cần file cấu hình cho mỗi deamon
    + có thể cấu hình deamon A tự restart khi deamon B crash hoặc deamon có thể start stop nếu cần thiết
    + Tính bảo mật cao: 
        - deamon không thể bị stop hoặc restart bởi bất kỳ user nào kể cả root
        - mỗi process được chạy trong `cgroup` của nó, có thể điều khiển mức tài nguyên mà process được dùng
        - các process được tách biệt nhau khiến việc hack hệ thống khó hơn
    + các câu lệnh cho systemd được thống nhất trên toàn bộ các bản phân phối Linux có dùng systemd, tạo thuận lợi khi sử dụng

# unit files
- unit là cách gọi của 1 service được quản lý bới systemd
- 1 systemd unit có thể quản lý 1 trong các unit type sau:
    + service
    + socket
    + device
    + mount point
    + swap file
    + partition
    + startup target
    + watched filesystem path
    + tập hợp các process được khởi tạo external
- vị trí của Unit file trong linux
    + `/lib/systemd/system/`: đường dẫn tiêu chuẩn, chứa các unit file của distro
    + `/usr/lib/systemd/system`: chứa unit file của các package apt
    + `/run/systemd/system`: chứa unit file tạm thời
    + `/etc/systemd/system`: chứa custom unit file mình tự tạo
- cấu trúc tệp unit file:
    ```c
    [Unit]
    Description=A simple unit file created by Dung -> #description
    After=network-up.target #chỉ chạy service sau khi network-up.target đã sẵn sàng

    [Service]
    ExecStart=/usr/local/bin/dungprogram -> #bin location

    [Install] -> # thời điểm nào thì service này nên chạy, ai muốn nó
    WantedBy=multi-user.target
    ```
    + After: service của tôi luôn chạy sau target của After
    + WantedBy: khi hệ thống đạt tới mốc này, service của tôi sẽ được chạy
- sau khi tạo 1 service mới, cần cập nhật lại danh sách unit file: `systemctl daemon-reload`

# systemd command
- systemctl
    + list-units : liệt kê các unit đang chạy
    + list-unit-files: liệt kê các file unit
    + status a: xem trạng thái của service a
    + enable/disable service: cho phép/không cho phép service này chạy lúc khởi động hệ thống, lệnh này không chạy/ngừng chạy service khi hệ thống đang chạy bình thường, nó chỉ có tác dụng khi reboot
    + start : chạy lệnh trong ExecStart, tương tự cho stop, reload. 
        - ExecStartPre sẽ luôn tự chạy trước ExecStart
        - ExecStartPost thì chạy sau
    + kill: 

# systemd target
- target dùng để quản lý mối quan hệ giữa các unit trong hệ thống
- nó giống như là từng phase trong quá trình khởi động của hệ thống, và target sẽ là nơi mà cung cấp thông tin của phase đó để service của mình có thể start sau khi 1 phase trong hệ thống đã hoàn tất
- target giống như là điểm để báo rằng 1 phase nào đó trong hệ thống đã hoàn tất và service của tôi có thể bắt đầu chạy 
- `multi-user.target`: là target gần như cuối cùng thông báo hệ thống đã khởi động ok rồi. Nếu hệ thống có màn hình thì cuối cùng là `graphical.target`
- `systemctl list-units --type=target`: liệt kê các target có trong hệ thống
- `systemctl isolate name.target`: chuyển trạng thái của linux về target mình muốn, nó sẽ dừng tất cả service khác không liên quan. Ví dụ như `systemctl isolate rescue.target` sẽ đưa hệ thống về trạng thái cứu hộ. Lệnh này ít khi dùng
- `systemctl get-default`: check xem hệ thống đang ở target nào