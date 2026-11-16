# 1. Basic
- file CMakeLists.txt cơ bản gồm có:
    + `cmake_minimum_required`
    + `project`: các lệnh set phải đặt trước lệnh project
    + `add_executable`
- Lệnh build:
    - `cmake [options] <path-to-source>`
    - `cmake [options] <path-to-existing-build>`
    - `cmake [options] -S <path-to-source> -B <path-to-build>`
- cmake: tạo ra Makefile
- make: thực thi file Makefile đó
# 2. Libraries, Installing, Paring with execution
- `install(TARGETS trim DESTINATION bin)`: cài trim vào folder bin, thường là `/usr/local/bin`
    + `/usr/local/bin` chứa các gói cho người dùng tự code, tự cài
    + `/usr/bin` chứa các gói từ apt, pacman, ...
    + dùng lệnh `make install`
    + cài vào /usr/local/bin rồi thì có thể gọi ở mọi nơi
- `add_library(${PROJECT_NAME} add.cpp)`: tạo library để dùng trong main, chỉ cần thêm file cpp chứ k cần thêm header
    + khi build sẽ tạo ra static library `lib${PROJECT_NAME}.a` (đuôi .a)
- ở cmake của main, cần khai báo đường dẫn của library 
    + `target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../libs/build/)`: khai báo đường dẫn chứa lib trong folder build
    + `target_link_libraries(${PROJECT_NAME} dungmath)`: tên của lib
    + `target_include_directories()`: khai báo đường dẫn của header để include được trong các file ngoài folder mà không cần đường dẫn `../`
- `set_target_properties`: set các thuộc tính cho target
- `add_subdirectory(folder)`: thêm các folder con vào để cmake tổng khi build sẽ build luôn cả phần con

# 3. Cmake với Git submodule
- cmake cho phép tích hợp thêm các library từ github
- clone submodule tùy ý về rồi `add_subdirectory` để build cùng cmake tổng
    + `add_subdirectory(folder)`: thêm các folder con vào để cmake tổng khi build sẽ build luôn cả phần con
        - khi đã dùng cái này thì không cần gọi `target_link_directories` cho các folder đó nữa

# 4. Versioning source code
- dùng `configure_file()` để cấu hình file `.h.in`. Trong file `.h.in`:
    + #define a @b@
    + PROJECT_VERSION = 1.0.0 là ver trong `project(main VERSION 1.0.0)`, phải là `#define PROJECT_VERSION_STRING "@PROJECT_VERSION@"`
    + PROJECT_VERSION_MAJOR = 1
    + PROJECT_VERSION_MINOR = 0
    + PROJECT_VERSION_PATCH = 0
- dùng #cmakedefine để define các cờ cho cmake

# 5. Making Libs Optional
- tùy chọn biên dịch lib nào
- `option()`: tạo tùy chọn trong lệnh cmake
    + tên của option được cấu hình trong lệnh `cmake -DOPTION=value ..`
- `list(APPEND EXTRA_LIBS Adder)`: thêm Adder vào danh sách EXTRA_LIBS (tên tự đặt), sau đó EXTRA_LIBS sẽ được truyền vào target_link_libraries chẳng hạn

# 6. Installing your software
- dùng `install`

# 7. CPack
- không quan trọng

# 8. find_library()
- dùng để tìm tệp binary (.so, .a) của thư viện mình cần trong máy build, chưa có thì phải cài lib đó trước
- `find_library(myglewlib NAMES glew glew2 GLEW GLEW2 DOC "glew library")`
    + `myglewlib`: tên bất kì
    + `glew`: tên của thư viện
    + `NAMES glew2 GLEW GLEW2`: các tên mà thư viện này có thể dùng (có thể không cần ghi)
    + `DOC "glew library"`: mô tả

# 9. find_package() và PkgConfig 
- dùng để tìm tất cả lib, header, flag, sub-library của gói chứ không phải mỗi lib như find_library()
- `find_package(<TênThưViện> [phiên_bản] [EXACT] [REQUIRED] [COMPONENTS comp1 comp2...])`
- nó tìm thư viện theo cơ chế: tìm file `Find<Package>.cmake` do cmake viết sẵn hoặc `<Package>Config.cmake` của các thư viện thứ 3 cài vào hệ thống
    + tuy nhiên không phải library nào cũng có sẵn file `.cmake`

# 10. cờ biên dịch & Build Type
- Compiler flags: hiện các cảnh báo khi build code
    + `target_compile_options`: thêm các cờ cảnh báo biên dịch
        + -Wall: bật các cảnh báo cơ bản
        + -Wextra: bật thêm cảnh báo nâng cao
        + -Werror: biến các warning thành error
        + -O0: tắt tối ưu để build nhanh
        + -O2, -O3: tối ưu tốc độ cho bản release, nhưng không thể debug được, file binary rất nặng, có thể gây ra các lỗi khó đoán do logic bị tối ưu quá mức
        + -Os: tối ưu dung lượng
        + -g hoặc -g3: phục vụ debug cho gdb hoặc valgrind
        + ...
    + `target_compile_definitions`: thêm macro định nghĩa 
        - khi khai báo biến trong hàm này thì code có thể dùng hàm này để đặt trong `ifdef`, giống như tạo ra 1 biến #define vậy
- Build Type:
    - `CMAKE_BUILD_TYPE`: cờ dùng trong lệnh `cmake -DCMAKE_BUILD_TYPE=xx`
        + Debug: dùng khi đang phát triển
        + Release: dùng khi release
        + RelWithDebInfo: release nhưng vẫn có debug
        + MinSizeRel: tối ưu dung lượng file binary
        + nếu muốn đặt trong CMakeLists.txt thì dùng set(CMAKE_BUILD_TYPE Release)
    - `CMAKE_CXX_STANDARD`: chuẩn C++
    - `CMAKE_CXX_STANDARD_REQUIRED`: buộc hỗ trợ chuẩn C++ nào

# 11. Toolchain
- Toolchain: chuỗi các công cụ (gcc, g++, ld, as, gdb, glibc) để biên dịch code thành file binary.
- Cross-compilation (Biên dịch chéo): dùng máy tính mạnh (Host x86_64) để build ra file chạy cho vi xử lý khác (Target ARM Cortex-A8 của BeagleBone Black).
- Toolchain file (`*.cmake`): file cấu hình riêng cho CMake để chuyển đổi trình biên dịch mà KHÔNG cần sửa file `CMakeLists.txt`.
- Các biến cốt lõi trong Toolchain file:
    + `set(CMAKE_SYSTEM_NAME Linux)`: báo cho CMake biết OS trên mạch đích.
    + `set(CMAKE_SYSTEM_PROCESSOR arm)`: kiến trúc chip đích.
    + `set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)`: đường dẫn gcc chéo.
    + `set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)`: đường dẫn g++ chéo.
    + `set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)`: chỉ tìm thư viện cho chip đích.
- Lệnh build:
    + `cmake -B build-arm -DCMAKE_TOOLCHAIN_FILE=path/to/toolchain.cmake`
- Kiểm tra file chạy cho kiến trúc nào:
    + Dùng lệnh `file <tên_file_binary>`

# 12. FetchContent

# 13. PUBLIC / PRIVATE / INTERFACE
Quy định tính kế thừa (transitivity) của thuộc tính target (include_directories, compile_options, compile_definitions, link_libraries):
- PRIVATE:
    + Áp dụng: CHỈ cho bản thân target đó.
    + Bên ngoài link tới: KHÔNG được kế thừa.
    + Thường dùng cho: mã nguồn .cpp nội bộ, header bí mật nội bộ, thư viện phụ trợ chỉ dùng trong .cpp.
- INTERFACE:
    + Áp dụng: CHỈ cho những ai link tới target đó.
    + Bản thân target: KHÔNG dùng (thường là target header-only không có file .cpp để biên dịch).
    + Thường dùng cho: Header-only library.
    + ví dụ về folder 13/fast_math/include
    ```c
    add_library(fast_math INTERFACE)

    target_include_directories(fast_math INTERFACE fast_math/include)

    target_link_libraries(${PROJECT_NAME}
        PRIVATE src fast_math
    )
    ```
- PUBLIC:
    + Áp dụng: CẢ HAI (bản thân target dùng VÀ ai link tới nó cũng tự động được kế thừa).
    + Thường dùng cho: thư mục chứa header công khai (include/), cờ chuẩn C++ chung của API.

# 14. Tạo compile_commands.json