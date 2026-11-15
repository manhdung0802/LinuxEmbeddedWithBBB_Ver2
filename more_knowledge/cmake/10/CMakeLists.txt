cmake_minimum_required(VERSION 3.16)

project(main)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_CUSTOM_LOG)

target_compile_options(${PROJECT_NAME} PRIVATE -Os)
