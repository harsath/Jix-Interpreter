#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "ast.h"
#include "string_builder.h"

struct string_builder *print_ast(struct vector *program);
void print_statement(struct ast_node *node, struct string_builder *str, size_t indent_level);
void print_fn_def_stmt(struct ast_node *node, struct string_builder *str, size_t indent_level);
void print_expr_stmt(struct ast_node *node, struct string_builder *str, size_t indent_level);
void print_return_stmt(struct ast_node *node, struct string_builder *str, size_t indent_level);
void print_var_decl_stmt(struct ast_node *node, struct string_builder *str, size_t indent_level);
void print_var_assign_stmt(struct ast_node *node, struct string_builder *str, size_t indent_level);
void print_if_stmt(struct ast_node *node, struct string_builder *str, size_t indent_level);
void print_while_stmt(struct ast_node *node, struct string_builder *str, size_t indent_level);
void print_for_stmt(struct ast_node *node, struct string_builder *str, size_t indent_level);
void print_break_stmt(struct ast_node *node, struct string_builder *str, size_t indent_level);
void print_block_stmt(struct ast_node *node, struct string_builder *str, size_t indent_level);

void print_expression(struct ast_node *node, struct string_builder *str);
void print_binary_expression(struct ast_node *node, struct string_builder *str);
void print_unary_expression(struct ast_node *node, struct string_builder *str);
void print_primary_expression(struct ast_node *node, struct string_builder *str);

char *get_indent_str(size_t indent_level);

#endif
