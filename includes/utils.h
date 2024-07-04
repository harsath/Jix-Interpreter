#ifndef UTILS_H
#define UTILS_H

#include "ast.h"
#include "interpreter.h"
#include "tokens.h"
#include "vector.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 10240
#define IDENTIFIER_BUFFER_SIZE 50

char *read_file(const char *file_path);
char *create_token_string_copy(const char *char_ptr, size_t start_index,
                               size_t current_index);
struct object *interpreter_pipeline(const char *file_name);
void print_ast_pipeline(const char *file_name);
const char *convert_object_to_string(struct object *obj);
char *format_string(const char *format, ...);
void print_parser_errors(struct vector *parser_errors);
void print_interpreter_error(struct runtime_error *runtime_error);

#endif
