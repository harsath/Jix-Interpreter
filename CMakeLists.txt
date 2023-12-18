cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(jix_interpreter C)

add_executable(
	jix_interpreter 
	main.c
	utils.h
	scanner.h
	tokens.h
	vector.h
)

