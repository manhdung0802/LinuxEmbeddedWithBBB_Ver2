cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

project(main VERSION 1.0.0)

add_executable(${PROJECT_NAME} app/main.cpp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sensor)
add_library(utils INTERFACE)
target_include_directories(utils INTERFACE utils/include)

configure_file(config.h.in config.h)

find_package(Threads REQUIRED)

option(USE_DISPLAY "enable display" OFF)

if(USE_DISPLAY)
    message(STATUS "added display")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/display)
    list(APPEND EXTRA_LIBS display)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)
target_compile_definitions(${PROJECT_NAME} PRIVATE SENSOR_CALIBRATED)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${CMAKE_BINARY_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE utils sensor ${EXTRA_LIBS} Threads::Threads
)
