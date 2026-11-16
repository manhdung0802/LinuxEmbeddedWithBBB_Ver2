cmake_minimum_required(VERSION 3.16)

project(main)

add_executable(${PROJECT_NAME} app/main.cpp)
add_subdirectory(car/src)
add_library(fast_math INTERFACE)

target_include_directories(fast_math INTERFACE fast_math/include)

target_link_libraries(${PROJECT_NAME}
    PRIVATE src fast_math
)
