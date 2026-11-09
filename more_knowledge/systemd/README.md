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

-------- video 2