project(src)

add_library(${PROJECT_NAME} car.cpp)

add_subdirectory(internal)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include PRIVATE internal)

target_link_libraries(${PROJECT_NAME} PRIVATE internal)
