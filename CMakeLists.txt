cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(jix_interpreter C)

include_directories(includes)

# Source files for main program
file(GLOB MAIN_SOURCES "src/*.c")
list(REMOVE_ITEM TEST_SRC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/ast_printer.c")

# Source files for test runner
file(GLOB TEST_SRC_SOURCES "src/*.c")
list(REMOVE_ITEM TEST_SRC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")
file(GLOB TEST_C_FILES "test/*.c")
file(GLOB TEST_H_FILES "test/*.h")
set(TEST_SOURCES ${TEST_SRC_SOURCES} ${TEST_C_FILES} ${TEST_H_FILES})

# Executable for main interpreter
add_executable(jix ${MAIN_SOURCES})

# Executable for test runner
add_executable(jix_tests ${TEST_SOURCES})
