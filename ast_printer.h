#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "ast.h"
#include "vector.h"

void print_ast_program(vector *program);
void print_ast_statement(ast_node *statement);
void print_ast_program_var_decl_stmt(ast_node *var_decl_stmt);
void print_ast_program_var_assign_stmt(ast_node *var_decl_stmt);
void print_ast_program_if_stmt(ast_node *if_stmt);
void print_ast_program_block_stmt(ast_node *var_decl_stmt);
void print_ast_expr(ast_node *node, size_t level);


#endif
