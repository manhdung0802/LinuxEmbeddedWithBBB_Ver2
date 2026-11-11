cmake_minimum_required(VERSION 3.16)

project(mathUtils)

add_library(${PROJECT_NAME} SHARED math_utils.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER math_utils.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include)
