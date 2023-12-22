#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "ast.h"
#include "vector.h"

void print_ast_program_var_decl_stmt(ast_node *var_decl_stmt);
void print_ast_program_var_assign_stmt(ast_node *var_decl_stmt);
void print_ast_program(vector *program);
void print_ast(ast_node *node, size_t level);


#endif
