#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "hash_table.h"
#include "tokens.h"
#include "utils.h"
#include "vector.h"

typedef struct {
  size_t current_token_index;
  vector *tokens;
} parser_state;

vector *parse_program(vector *tokens);
ast_node *parse_statement(parser_state *parser);
ast_node *parse_variable_declaration_statement(parser_state *parser);
ast_node *parse_variable_assignment_statement(parser_state *parser);
ast_node *parse_if_statement(parser_state *parser);
ast_node *parse_block_statement(parser_state *parser);
ast_node *parse_expression(parser_state *parser);
ast_node *logical_or(parser_state *parser);
ast_node *logical_and(parser_state *parser);
ast_node *equality(parser_state *parser);
ast_node *comparitive(parser_state *parser);
ast_node *additive(parser_state *parser);
ast_node *multiplicative(parser_state *parser);
ast_node *primary(parser_state *parser);

token *get_current_token(parser_state *parser);
void increment_token_index(parser_state *parser);
bool check_index_bound(parser_state *parser);

#endif
