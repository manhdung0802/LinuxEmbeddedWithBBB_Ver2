cmake_minimum_required(VERSION 3.16)

project(dungadder)


option(USER_ADDER "Simple library for adding." ON)

if(USER_ADDER)
    add_subdirectory(Adder)
    list(APPEND EXTRA_DIR Adder)
    list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/Adder")
    list(APPEND EXTRA_LIBS adder)
endif()

add_executable(${PROJECT_NAME} main.cpp)
configure_file(config.h.in config.h)

add_subdirectory(external/glfw)

target_include_directories(${PROJECT_NAME}
    PUBLIC external/glfw/include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${EXTRA_INCLUDES})
target_link_directories(${PROJECT_NAME}
    PRIVATE external/glfw/src/
    ${EXTRA_DIR})
target_link_libraries(${PROJECT_NAME}
    ${EXTRA_LIBS}
    glfw)
