cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE arm-toolchain.cmake)
project(main)

add_executable(${PROJECT_NAME} main.cpp)

