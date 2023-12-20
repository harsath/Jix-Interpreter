#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "tokens.h"
#include "utils.h"
#include "vector.h"

typedef struct {
  size_t current_token_index;
  vector *tokens;
} parser_state;

ast_node *parse_expression(parser_state *parser);
ast_node *additive(parser_state *parser);
ast_node *multiplicative(parser_state *parser);
ast_node *primary(parser_state *parser);

ast_node *build_ast(vector *tokens) {
  parser_state parser = {.tokens = tokens, .current_token_index = 0};
  return parse_expression(&parser);
}

ast_node *parse_expression(parser_state *parser) { return additive(parser); }

ast_node *additive(parser_state *parser) {
  ast_node *left = multiplicative(parser);
  token *op = vector_at(parser->tokens, parser->current_token_index);
  while ((parser->current_token_index < parser->tokens->size) && op != NULL &&
         (op->type == PLUS || op->type == MINUS)) {
    parser->current_token_index++;
    ast_node *right = multiplicative(parser);
    ast_node *new_left = malloc(sizeof(ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->left = left;
    new_left->right = right;
    new_left->op = op->type;
    left = new_left;
    op = vector_at(parser->tokens, parser->current_token_index);
  }
  return left;
}

ast_node *multiplicative(parser_state *parser) {
  ast_node *left = primary(parser);
  token *op = vector_at(parser->tokens, parser->current_token_index);
  while ((parser->current_token_index < parser->tokens->size) && op != NULL &&
         (op->type == STAR || op->type == SLASH)) {
    parser->current_token_index++;
    ast_node *right = primary(parser);
    ast_node *new_left = malloc(sizeof(ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->left = left;
    new_left->right = right;
    new_left->op = op->type;
    left = new_left;
    op = vector_at(parser->tokens, parser->current_token_index);
  }
  return left;
}

ast_node *primary(parser_state *parser) {
  token *cur_tok = vector_at(parser->tokens, parser->current_token_index);
  switch (cur_tok->type) {
  case NUMBER: {
    ast_node *num_node = malloc(sizeof(ast_node));
    num_node->node_type = PRIMARY_NODE;
    num_node->primary_node_type = NUMBER_PRIMARY_NODE;
    char temp_value[100] = {0};
    memcpy(temp_value, cur_tok->token_char, cur_tok->token_char_len);
    char *end_ptr;
    num_node->number_value = strtol(temp_value, &end_ptr, 10);
    parser->current_token_index++;
    return num_node;
  }
  default: {
    printf("Unsupported primary '%s'\n",
           get_string_from_token_atom(cur_tok->type));
  }
  }
  return NULL;
}

#endif
