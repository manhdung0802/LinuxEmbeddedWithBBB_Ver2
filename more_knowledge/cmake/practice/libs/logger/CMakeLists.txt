cmake_minimum_required(VERSION 3.16)

project(logger)

add_library(${PROJECT_NAME} logger.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER logger.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION lib PUBLIC_HEADER DESTINATION include)
