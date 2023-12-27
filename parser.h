#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "hash_table.h"
#include "tokens.h"
#include "utils.h"
#include "vector.h"

struct parser_state {
  size_t current_token_index;
  struct vector *tokens;
};

struct vector *parse_program(struct vector *tokens);
struct ast_node *parse_statement(struct parser_state *parser);
struct ast_node *
parse_function_definition_statement(struct parser_state *parser);
struct ast_node *parse_expression_statement(struct parser_state *parser);
struct ast_node *
parse_variable_declaration_statement(struct parser_state *parser);
struct ast_node *
parse_variable_assignment_statement(struct parser_state *parser);
struct ast_node *parse_if_else_statement(struct parser_state *parser);
struct ast_node *parse_while_statement(struct parser_state *parser);
struct ast_node *parse_block_statement(struct parser_state *parser);
struct ast_node *parse_expression(struct parser_state *parser);
struct ast_node *logical_or(struct parser_state *parser);
struct ast_node *logical_and(struct parser_state *parser);
struct ast_node *equality(struct parser_state *parser);
struct ast_node *comparitive(struct parser_state *parser);
struct ast_node *additive(struct parser_state *parser);
struct ast_node *multiplicative(struct parser_state *parser);
struct ast_node *unary(struct parser_state *parser);
struct ast_node *primary(struct parser_state *parser);
struct ast_node *fn_call(struct parser_state *parser);

struct token *get_current_token(struct parser_state *parser);
void increment_token_index(struct parser_state *parser);
bool check_index_bound(struct parser_state *parser);
bool invalid_data_type(enum token_type type);

#endif
