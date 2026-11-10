cmake_minimum_required(VERSION 3.10)

project(dungmath)

add_library(${PROJECT_NAME} add.cpp)

# Khai báo add.h là PUBLIC_HEADER để lệnh install cài add.h vào include
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER add.h)

# cài file bin vào lib và header vào include để các file khác include được header add.h
# output: -- Installing: /usr/local/lib/libdungmath.a
#         -- Installing: /usr/local/include/add.h
install(TARGETS dungmath LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include)
