cmake_minimum_required(VERSION 3.20.0)
project(telemetry VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT DEFINED CONFIG_GPS_115200 AND NOT DEFINED CONFIG_GPS_9600)
    set(CONFIG_GPS_115200 1)
endif()

add_compile_options(-Wall -Wextra -pedantic -Werror -O2)

file(GLOB_RECURSE SOURCES "src/*.c" "src/**/*.c" "src/*.cpp" "src/**/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/threads
    ${CMAKE_SOURCE_DIR}/src/utils
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    gpiodcxx
    capnp
    kj
)