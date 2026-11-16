project(sensor)

find_library(MATH_LIB NAMES m)

add_library(${PROJECT_NAME} src/sensor.cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/internal)

target_link_directories(${PROJECT_NAME}
    PRIVATE 
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE ${MATH_LIB} internal
)