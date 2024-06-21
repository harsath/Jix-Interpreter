#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "hash_table.h"
#include "tokens.h"
#include "utils.h"
#include "vector.h"

struct parser_state {
  size_t current_token_index;
  size_t current_line; /* Current line in source file */
  struct vector *tokens;
};

struct vector *parse_program(struct vector *tokens);
struct ast_node *parse_statement(struct parser_state *parser);
struct ast_node *
parse_function_definition_statement(struct parser_state *parser);
void parse_function_definition_parameters(struct vector *parameters,
                                          struct parser_state *parser);
struct ast_node *parse_return_statement(struct parser_state *parser);
struct ast_node *parse_expression_statement(struct parser_state *parser);
struct ast_node *
parse_variable_declaration_statement(struct parser_state *parser);
struct ast_node *
parse_variable_assignment_statement(struct parser_state *parser);
struct ast_node *parse_if_else_statement(struct parser_state *parser);
struct ast_node *parse_while_statement(struct parser_state *parser);
struct ast_node *parse_for_statement(struct parser_state *parser);
struct ast_node *parse_break_statement(struct parser_state *parser);
struct ast_node *parse_block_statement(struct parser_state *parser);
struct ast_node *parse_expression(struct parser_state *parser);
struct ast_node *logical_or(struct parser_state *parser);
struct ast_node *logical_and(struct parser_state *parser);
struct ast_node *equality(struct parser_state *parser);
struct ast_node *comparitive(struct parser_state *parser);
struct ast_node *additive(struct parser_state *parser);
struct ast_node *multiplicative(struct parser_state *parser);
struct ast_node *parse_unary(struct parser_state *parser);
struct ast_node *parse_extended_primary(struct parser_state *parser);
struct ast_node *parse_primary(struct parser_state *parser);
struct ast_node *parse_fn_call(struct parser_state *parser);
void parse_parameters(struct parser_state *parser, struct vector *parameters);
struct ast_node *parse_array_creation(struct parser_state *parser);

struct token *get_previous_token(struct parser_state *parser);
struct token *get_current_token(struct parser_state *parser);
struct token *get_next_token(struct parser_state *parser);
void increment_token_index(struct parser_state *parser);
bool check_index_bound(struct parser_state *parser);
bool invalid_data_type(enum token_type type);
void consume_token(enum token_type expected_token, struct parser_state *parser);
void check_current_token_type(enum token_type expected_token,
                              const char *error_message,
                              struct parser_state *parser);
void check_ast_node_type(struct ast_node *node,
                         enum ast_node_type expected_node_type,
                         const char *error_message,
                         struct parser_state *parser);
void check_primary_ast_node_type(struct ast_node *node,
                                 enum ast_primary_node_type expected_node_type,
                                 const char *error_message,
                                 struct parser_state *parser);

#endif
