cmake_minimum_required(VERSION 3.10)

project(jix_interpreter C)

add_executable(
	jix_interpreter 
	main.c
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
