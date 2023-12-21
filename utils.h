#ifndef UTILS_H
#define UTILS_H

#include "ast.h"
#include "tokens.h"
#include "vector.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 10240
#define IDENTIFIER_BUFFER_SIZE 50

char *read_file(const char *file_path);
char *create_token_string_copy(const char *char_ptr, size_t start_index,
                               size_t current_index);
void print_ast_program_var_decl_stmt(ast_node *var_decl_stmt);
void print_ast_program(vector *program);
void print_ast(ast_node *node, size_t level);

#endif
