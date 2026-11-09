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
    + `target_link_libraries(${PROJECT_NAME} dungmath)`: khai báo lib để link với project

video 2: 29:10