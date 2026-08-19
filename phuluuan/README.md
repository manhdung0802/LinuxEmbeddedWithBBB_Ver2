# Process trong userspace
- Process là 1 thực thể được khởi tạo từ 1 file chương trình trên ổ cứng, process sẽ quản lý các thread của nó.
- 1 process có thể có nhiều thread
- Process sẽ sử dụng các tài nguyên của hệ thống như RAM, ngoại vi, ...
## Start chương trình
- 1 chương trình userspace trong linux thì hàm main là điểm đầu tiên đi vào chương trình
- `int main(int argc, char *argv[])` hoặc `void main()`
    + argc: số tham số truyền vào
    + argv[]: mảng chứa các chuỗi ký tự truyền vào (argv[0]), argv[1], ...)
- trước khi vào hàm main, kernel sẽ chạy 1 số code ẩn để khởi tạo tài nguyên cho process
## Kết thúc chương trình
- Chủ động: trong code, gọi các hàm để kết thúc như return, kill(), exit(), ...
- Bị động: bị crash, ctrl + c, kill -9 PID
- Chương trình con khi kết thúc sẽ gửi giá trị trả về cho chương trình cha, dựa vào giá trị này có thể debug được chương trình con kết thúc có bình thường hay không
## Các tham số môi trường ảnh hưởng tới process
- Khi 1 process chạy trong Linux, nó sẽ bị ảnh hưởng bởi các process khác
- Cần quan tâm môi trường mà process chạy vì trong hệ điều hành Linux, 1 process có thể cho ra nhiều kết quả khác nhau phụ thuôc vào môi trường chạy nó
### tham số argc và argv[]
- Việc truyền tham số argc, argv[] sẽ ảnh hưởng tới quá trình chạy của process. CHương trình khi chạy với tham số đầu vào này sẽ làm chương trình rất linh hoạt
```c
#include <stdio.h>

int main(int argc, char *argv[]){
    int i = 0;
    for (i = 0; i < argc; i++){
        printf("%s\n", argv[i]);
    }
    return 0;
}
```
### tham số environment list
- mỗi khi chương trình start, hệ điều hành sẽ cấp cho chương trình 1 danh sách biến môi trường
- `printenv`: in ra các biến môi trường
- để truy câp được biến môi trường từ chương trình
    + khai báo `extern char **environ`: mảng 2 chiều, mỗi phần từ trỏ vào 1 biến môi trường
    + dùng `char *getenv(const char *name)` để truy xuất biến môi trường
## Cấu trúc bộ nhớ của 1 process
- ![alt text](images/image.png)
- Vùng text: lưu các hằng số, instructions, thường có thuộc tính read-only
- Vùng initialized data: chứa các biến đã được khởi tạo như biến global, biến static
- Vùng uninitialized data: chứa các biến chưa được khởi tạo giá trị, mặc định hệ điều hình gắn là 0 hoặc NULL
- Vùng heap: dùng cho việc cấp phát bộ nhớ động
- Vùng stack: chưa các biến cục bộ, địa chỉ trả về của hàm
- Vùng Command-line argument và environment variable: chứa csac tham số argc, argv[]
- Khi tổ chức trên RAM, các vùng nhớ này rời rạc, không liền mạch. Các process cần địa chỉ virtual memory liền mạch là được
- Khi bị leak memory do kiểm soát cấp phát động yếu kém, nếu process gây leak đó kết thúc thì toàn bộ vùng leak của process đó sẽ được giải phóng
## Shared library và static library
- Shared library: là 1 file, được build ra như chương trình c bình thường nhưng nó không có hàm main, không thể tự chạy được
    + Khi code gọi 1 function, library sẽ load hàm đó vào RAM, và trả về địa chỉ hàm cho chương trình
    + Ưu: 
        - tốn ít bộ nhớ
        - function cần dùng chỉ cần load 1 lần vào RAM, sau đó chương trình nào dùng thì chỉ cần lấy địa chỉ trong RAM
        - function được giải phóng khỏi RAM khi không còn chương trình nào dùng nữa
    + Nhược: mất thời gian load hàm
- Static library: 
    + Khi code gọi 1 function trong library thì khi biên dịch, function đó được chèn luôn vào nơi gọi nó
    + Ưu: nhanh khi gọi hàm
    + Nhược: tốn tài nguyên
## Cách tạo ra process
- PID: mỗi process có 1 số PID, hệ điều hành dùng số này để quản lý các process
- Init process có PID là 0
- Process trong hệ điều hành khi bị crash thì không ảnh hưởng process khác
- Tạo process: 
    + fork(): tạo bản sao của chương trình gọi nó
        ```c
        #include <stdio.h>

        int main(int argc, char *argv[]){
            int pid;
            pid = fork();

            if(pid == 0) {
                printf("CHild"); // Tiến trình con sẽ được tạo ra với giá trị trả về của fork là 0, đây là return của hàm, không phải pid trong hệ thống
            } else {
                printf("Parent");
            }

            return 0;
        }
        ```
        - fork() tạo ra chương trình thứ 2 chạy đoạn code từ sau điểm gọi fork()
    + exec(), execl():
        - tạo ra 1 process mới hoàn toàn theo ý mình muốn, không phải bản copy
        - cần truyền vào chương trình mới (có thể là file binary của 1 file c, và các tham số, ...)
        - Khi đã gọi exec, các code dưới hàm exec của cùng PID không được gọi nữa
        - Nên fork ra 1 process mới trước rồi dùng exec để thay chương trình chạy trong process mới. Ta sẽ có 2 process riêng biệt
        ```c
        #include <stdio.h>

        int main(int argc, char *argv[]){
            int pid;
            pid = fork();

            if(pid == 0) {
                printf("CHild"); // Tiến trình con sẽ được tạo ra với giá trị trả về của fork là 0, đây là return của hàm, không phải pid trong hệ thống
                execl("./child", "child" ,...);
                do_something(); // không chạy được nữa do exec đã thay thế bằng chương trình child
            } else {
                printf("Parent");
            }

            return 0;
        }
        ```
## Cách kết thúc process
+ giá trị trong return hoặc exit sẽ được trả về cho process cha
+ nếu process cha kết thúc trước process con, thì process con sẽ được đổi cha sang init process
+ Khi process con kết thúc, nó gửi SIGCHLD về cho process cha. Process cha nhận SIGCHLD này để xác nhận con kết thúc. Nếu không thì process con sẽ trở thành zombie process, tài nguyên của nó không được giải phóng
    - Dùng wait(..) trong process cha để chờ process con được gửi tín hiệu kết thúc và để hệ điều hành giải phóng tài nguyên của process con
- `ps -aux`: check trạng thái của process, cột STAT (S, R+, Z, Ss, Sl, ...)
- Khi process ở trạng thái Zombie, không thể kill được nó. Vì vậy trong process cha cần gọi 
    + wait(&status): nếu chỉ có 1 process, vì wait chỉ nhận được 1 status của 1 process thôi
    + waitpid(...): nhận status kết thúc theo từng PID của từng process
## User id và group id
- Chương trình được start thì user nào thì có quyền hạn của user đó (ví dụ: admin, user, group, ...)
- Ví dụ: chương trình muốn sửa file hệ thống thì cần start từ root
- Hệ điều hành có thể check được quyền của chương trình thông qua user id và group id
- Có thể thay đổi user id và group id
    + setgid(...)
    + setuid(...)