cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(jix_interpreter C)

include_directories(includes)

file(GLOB SOURCES "src/*.c")

add_executable(jix_interpreter ${SOURCES})
