cmake_minimum_required(VERSION 3.20.0)
project(telemetry VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    string(REGEX REPLACE "/W[1-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_compile_options(-Wall)

file(GLOB_RECURSE SOURCES "src/*.c" "src/**/*.c" "src/*.cpp" "src/**/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/threads
    ${CMAKE_SOURCE_DIR}/src/utils
)

target_link_libraries(${PROJECT_NAME} PRIVATE gpiod)
