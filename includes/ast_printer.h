#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "ast.h"
#include "vector.h"

void print_ast_program(struct vector *program);
void print_ast_statement(struct ast_node *statement, size_t indent_level);
void print_ast_program_fn_def_stmt(struct ast_node *fn_def_stmt,
                                   size_t indent_level);
void print_ast_program_expr_stmt(struct ast_node *expr_stmt);
void print_ast_program_return_stmt(struct ast_node *return_stmt);
void print_ast_program_var_decl_stmt(struct ast_node *var_decl_stmt);
void print_ast_program_var_assign_stmt(struct ast_node *var_assign_stmt);
void print_ast_program_if_stmt(struct ast_node *if_stmt, size_t indent_level);
void print_ast_program_while_stmt(struct ast_node *while_stmt,
                                  size_t indent_level);
void print_ast_program_for_stmt(struct ast_node *for_stmt,
                                  size_t indent_level);
void print_ast_program_break_stmt(struct ast_node *break_stmt,
                                  size_t indent_level);
void print_ast_program_block_stmt(struct ast_node *var_decl_stmt,
                                  size_t indent_level);

void print_ast_expr(struct ast_node *node);

#endif
