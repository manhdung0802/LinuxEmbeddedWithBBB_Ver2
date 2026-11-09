cmake_minimum_required(VERSION 3.13)

project(dung)

add_executable(${PROJECT_NAME} main.cpp)

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../libs/build/)
target_link_libraries(${PROJECT_NAME} dungmath)
