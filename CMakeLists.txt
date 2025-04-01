cmake_minimum_required(VERSION 3.20.0)
project(telemetry VERSION 1.0.0)

set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-Wall)

file(GLOB_RECURSE SOURCES "src/*.c" "src/**/*.c")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/threads
    ${CMAKE_SOURCE_DIR}/src/utils
)

target_link_libraries(${PROJECT_NAME} PRIVATE gpiod)