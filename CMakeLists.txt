cmake_minimum_required(VERSION 3.20.0)
project(telemetry VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT DEFINED TELEMETRY_VERSION)
    execute_process(
        COMMAND git describe --always --tags --dirty
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE TELEMETRY_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(NOT TELEMETRY_VERSION)
        set(TELEMETRY_VERSION "unknown")
    endif()
endif()
add_compile_definitions(TELEMETRY_VERSION="${TELEMETRY_VERSION}")

if(CONFIG_USE_MASTER)
    add_compile_definitions(CONFIG_USE_MASTER)
endif()

# RTC configuration
add_compile_definitions(USE_DEV_LIB)
add_compile_definitions(DEV_I2C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -pedantic -Werror -O2)

find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED)

pkg_check_modules(CAPNP REQUIRED capnp)
pkg_check_modules(KJ REQUIRED kj)
pkg_check_modules(GPIODCXX REQUIRED gpiodcxx)

file(GLOB_RECURSE SOURCES "src/*.c" "src/**/*.c" "src/*.cpp" "src/**/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/threads
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/utils/rtc_watchdog
    ${CAPNP_INCLUDE_DIRS}
    ${KJ_INCLUDE_DIRS}
    ${GPIODCXX_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${GPIODCXX_LIBRARIES}
    ${CAPNP_LIBRARIES}
    ${KJ_LIBRARIES}
)

find_program(CLANG_FORMAT NAMES clang-format clang-format-18 clang-format-17)
if(CLANG_FORMAT)
    file(GLOB_RECURSE FORMAT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/main.cpp"
        "${CMAKE_SOURCE_DIR}/src/threads/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/threads/*.hpp"
        "${CMAKE_SOURCE_DIR}/src/utils/log.*"
        "${CMAKE_SOURCE_DIR}/src/utils/status.*"
        "${CMAKE_SOURCE_DIR}/src/utils/log_paths.*"
        "${CMAKE_SOURCE_DIR}/src/utils/data.*"
    )
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${FORMAT_SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-format"
    )
endif()