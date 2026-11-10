# 1. Basic
- file CMakeLists.txt cơ bản gồm có:
    + `cmake_minimum_required`
    + `project`
    + `add_executable`
- Lệnh build:
    - `cmake [options] <path-to-source>`
    - `cmake [options] <path-to-existing-build>`
    - `cmake [options] -S <path-to-source> -B <path-to-build>`
- cmake: tạo ra Makefile
- make: thực thi file Makefile đó
# 2. Libraries, Installing, Paring with execution
- `install(TARGERS trim DESTINATION bin)`: cài trim vào folder bin, thường là /usr/local/bin
    + dùng lệnh `make install`
    + cài vào /usr/local/bin rồi thì có thể gọi ở mọi nơi
- `add_library(${PROJECT_NAME} add.cpp)`: tạo library để dùng trong main, chỉ cần thêm file cpp chứ k cần thêm header
    + khi build sẽ tạo ra static library `lib${PROJECT_NAME}.a` (đuôi .a)
- ở cmake của main, cần khai báo đường dẫn của library 
    + `target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../libs/build/)`: khai báo đường dẫn chứa lib
    + `target_link_libraries(${PROJECT_NAME} dungmath)`: khai báo đường dẫn chứa lib để link với project, là folder trong thư mục build
    + `target_include_directories()`: khai báo đường dẫn của header để include được trong các file ngoài folder mà không cần đường dẫn `../`
- `set_target_properties`: set các thuộc tính cho target
- `add_subdirectory(folder)`: thêm các folder con vào để cmake tổng khi build sẽ build luôn cả phần con

# 3. Cmake với Git submodule
- cmake cho phép tích hợp thêm các library từ github
- clone submodule tùy ý về rồi `add_subdirectory` để build cùng cmake tổng
    + `add_subdirectory(folder)`: thêm các folder con vào để cmake tổng khi build sẽ build luôn cả phần con

# 4. Versioning source code
- `https://www.youtube.com/watch?v=K3bx7NYSXVk&list=PLalVdRk2RC6o5GHu618ARWh0VO0bFlif4&index=4`
- dùng `configure_file()` để cấu hình file `.h.in`

# 5. Making Libs Optional
- tùy chọn biên dịch lib nào
- `option()`: tạo tùy chọn trong lệnh cmake
    + tên của option được cấu hình trong lệnh `cmake -DOPTION=value ..`
- `list(APPEND EXTRA_LIBS Adder)`: thêm Adder vào danh sách EXTRA_LIBS (tên tự đặt), sau đó EXTRA_LIBS sẽ được truyền vào target_link_libraries chẳng hạn
