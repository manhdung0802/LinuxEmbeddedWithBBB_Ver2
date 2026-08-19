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
- Static library: 



- fork(): copy bản sao của chương trình đang chạy
- exec(...), execl(...): tạo 1 process mới chạy binary mình muốn
- wait(..): chờ process con được tạo ra gửi tín hiệu kết thúc 
- setgid()
- setuid()