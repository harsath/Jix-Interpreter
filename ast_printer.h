#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "ast.h"
#include "vector.h"

void print_ast_program(struct vector *program);
void print_ast_statement(struct ast_node *statement);
void print_ast_program_fn_def_stmt(struct ast_node *fn_def_stmt);
void print_ast_program_var_decl_stmt(struct ast_node *var_decl_stmt);
void print_ast_program_var_assign_stmt(struct ast_node *var_assign_stmt);
void print_ast_program_if_stmt(struct ast_node *if_stmt);
void print_ast_program_while_stmt(struct ast_node *while_stmt);
void print_ast_program_block_stmt(struct ast_node *var_decl_stmt);
void print_ast_expr(struct ast_node *node, size_t level);

#endif
