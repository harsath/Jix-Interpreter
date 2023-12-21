cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(jix_interpreter C)

add_executable(
	jix_interpreter 
	main.c
	utils.c
	scanner.c
	tokens.c
	vector.c
	parser.c
	interpreter.c
	hash_table.c
	ast_printer.c
)
