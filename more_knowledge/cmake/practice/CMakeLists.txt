cmake_minimum_required(VERSION 3.16)

project(main VERSION 1.0.0)

add_executable(${PROJECT_NAME} src/main.cpp)

option(USE_LOGGER "Use logger" OFF)
option(USE_FMT "Use fmt" OFF)

configure_file(config.h.in config.h)

if(USE_LOGGER)
    add_subdirectory(libs/logger)
    list(APPEND EXTRA_LIB logger)
endif()
if(USE_FMT)
    add_subdirectory(external/fmt)
    list(APPEND EXTRA_LIB fmt)
endif()

add_subdirectory(libs/math_utils)


target_link_libraries(${PROJECT_NAME}
    PRIVATE ${EXTRA_LIB}
    PRIVATE mathUtils)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/math_utils
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
