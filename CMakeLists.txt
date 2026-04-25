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

if(CONFIG_GPS_115200)
    add_compile_definitions(CONFIG_GPS_115200)
elseif(CONFIG_GPS_9600)
    add_compile_definitions(CONFIG_GPS_9600)
else()
    add_compile_definitions(CONFIG_GPS_115200)
endif()

if(CONFIG_USE_MASTER)
    add_compile_definitions(CONFIG_USE_MASTER)
endif()

# RTC configuration
add_compile_definitions(USE_DEV_LIB)
add_compile_definitions(DEV_I2C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -pedantic -Werror -O2)

file(GLOB_RECURSE SOURCES "src/*.c" "src/**/*.c" "src/*.cpp" "src/**/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/threads
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/utils/rtc_watchdog
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    gpiodcxx
    capnp
    kj
)