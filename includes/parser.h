#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "errors.h"
#include "hash_table.h"
#include "tokens.h"
#include "utils.h"
#include "vector.h"

#define CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(result_ptr)                      \
  do {                                                                         \
    if (result_ptr->type == RESULT_ERROR) {                                    \
      return result_ptr;                                                       \
    }                                                                          \
  } while (0)

#define CHECK_AND_RETURN_IF_ERROR_EXISTS(result_ptr)                           \
  do {                                                                         \
    if (result_ptr) {                                                          \
      return result_ptr;                                                       \
    }                                                                          \
  } while (0)

struct parser_state {
  size_t current_token_index;
  size_t current_line; /* Current line in source file */
  struct vector *tokens;
};

struct vector *parse_program(struct vector *tokens);
struct result *parse_statement(struct parser_state *parser);
struct result *parse_function_definition_statement(struct parser_state *parser);
struct result *
parse_function_definition_parameters(struct vector *parameters,
                                     struct parser_state *parser);
struct result *parse_return_statement(struct parser_state *parser);
struct result *parse_expression_statement(struct parser_state *parser);
struct result *
parse_variable_declaration_statement(struct parser_state *parser);
struct result *parse_variable_assignment_statement(struct parser_state *parser,
                                                   struct result *primary);
struct result *parse_if_else_statement(struct parser_state *parser);
struct result *parse_while_statement(struct parser_state *parser);
struct result *parse_for_statement(struct parser_state *parser);
struct result *parse_break_statement(struct parser_state *parser);
struct result *parse_block_statement(struct parser_state *parser);
struct result *parse_expression(struct parser_state *parser);
struct result *logical_or(struct parser_state *parser);
struct result *logical_and(struct parser_state *parser);
struct result *equality(struct parser_state *parser);
struct result *comparitive(struct parser_state *parser);
struct result *additive(struct parser_state *parser);
struct result *multiplicative(struct parser_state *parser);
struct result *parse_unary(struct parser_state *parser);
struct result *parse_extended_primary(struct parser_state *parser);
struct result *parse_primary(struct parser_state *parser);
struct result *parse_parameters(struct parser_state *parser,
                                struct vector *parameters);
struct result *parse_array_creation(struct parser_state *parser);

struct token *get_previous_token(struct parser_state *parser);
struct token *get_current_token(struct parser_state *parser);
struct token *get_next_token(struct parser_state *parser);
void increment_token_index(struct parser_state *parser);
bool check_index_bound(struct parser_state *parser);
struct result *consume_token(enum token_type expected_token,
                             struct parser_state *parser);
struct result *check_current_token_type(enum token_type expected_token,
                                        struct parser_state *parser);
struct result *check_ast_node_type(struct ast_node *node,
                                   enum ast_node_type expected_node_type,
                                   struct parser_state *parser);
struct result *
check_primary_ast_node_type(struct ast_node *node,
                            enum ast_primary_node_type expected_node_type,
                            struct parser_state *parser);

#endif
