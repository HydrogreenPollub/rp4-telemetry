cmake_minimum_required(VERSION 3.20.0)
project(rp4-telemetry VERSION 1.0.0)

add_compile_options(-Wall)

file(GLOB SOURCES "src/*.c")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} gpiod)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)