cmake_minimum_required(VERSION 3.16)

project(dungadder)

find_library(myglewlib NAMES glew glew2 GLEW GLEW2)
if(myglewlib)
    message(STATUS "Found glew")
else()
    message(FATAL_ERROR "Not found glew")
endif()

find_library(myglulib NAMES glu GLU)
if(myglulib)
    message(STATUS "Found glu")
else()
    message(FATAL_ERROR "Not found glu")
endif()

find_library(mygllib NAMES gl GL)
if(mygllib)
    message(STATUS "Found gl")
else()
    message(FATAL_ERROR "Not found gl")
endif()

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
    ${myglewlib}
    ${mygllib}
    ${myglulib}
    glfw)
