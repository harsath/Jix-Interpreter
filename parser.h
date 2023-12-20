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

vector *parse_program(vector *tokens);

ast_node *parse_statement(parser_state *parser);
ast_node *parse_variable_decl_statement(parser_state *parser);
ast_node *parse_expression(parser_state *parser);
ast_node *additive(parser_state *parser);
ast_node *multiplicative(parser_state *parser);
ast_node *primary(parser_state *parser);

token *get_current_token(parser_state *parser);
void increment_token_index(parser_state *parser);
bool check_index_bound(parser_state *parser);

vector *parse_program(vector *tokens) {
  parser_state parser = {.tokens = tokens, .current_token_index = 0};
  vector *program = vector_init();
  while (parser.current_token_index < tokens->size) {
    vector_push_back(program, parse_statement(&parser));
  }
  return program;
}

ast_node *parse_statement(parser_state *parser) {
  return parse_variable_decl_statement(parser);
}
ast_node *parse_variable_decl_statement(parser_state *parser) {
  token *dtype = get_current_token(parser);
  switch (dtype->type) {
  case INT_DATATYPE:
  case STRING_DATATYPE:
  case BOOL_DATATYPE: {
    increment_token_index(parser);
    ast_node *var_decl_stmt = malloc(sizeof(ast_node));
    var_decl_stmt->node_type = VARIABLE_DECL_STMT;
    var_decl_stmt->var_decl_stmt_dtype = dtype->type;
    var_decl_stmt->var_decl_stmt_id = primary(parser);
    if (get_current_token(parser)->type != EQUAL) {
      printf("Identifier must have '=' next\n");
      exit(1);
    }
    increment_token_index(parser);
    var_decl_stmt->var_decl_stmt_expr = parse_expression(parser);
    if (!check_index_bound(parser) ||
        get_current_token(parser)->type != SEMICOLON) {
      printf("Statement must end with ';'\n");
      exit(1);
    }
    increment_token_index(parser);
    return var_decl_stmt;
    break;
  }
  default: {
    printf("Unsupported datatype in variable declaration statement");
    exit(1);
  }
  }
}

ast_node *parse_expression(parser_state *parser) { return additive(parser); }

ast_node *additive(parser_state *parser) {
  ast_node *left = multiplicative(parser);
  token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL &&
         (op->type == PLUS || op->type == MINUS)) {
    increment_token_index(parser);
    ast_node *right = multiplicative(parser);
    ast_node *new_left = malloc(sizeof(ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->left = left;
    new_left->right = right;
    new_left->op = op->type;
    left = new_left;
    op = get_current_token(parser);
  }
  return left;
}

ast_node *multiplicative(parser_state *parser) {
  ast_node *left = primary(parser);
  token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL &&
         (op->type == STAR || op->type == SLASH)) {
    increment_token_index(parser);
    ast_node *right = primary(parser);
    ast_node *new_left = malloc(sizeof(ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->left = left;
    new_left->right = right;
    new_left->op = op->type;
    left = new_left;
    op = get_current_token(parser);
  }
  return left;
}

ast_node *primary(parser_state *parser) {
  token *cur_tok = get_current_token(parser);
  switch (cur_tok->type) {
  case NUMBER: {
    ast_node *num_node = malloc(sizeof(ast_node));
    num_node->node_type = PRIMARY_NODE;
    num_node->primary_node_type = NUMBER_PRIMARY_NODE;
    char temp_value[100] = {0};
    memcpy(temp_value, cur_tok->token_char, cur_tok->token_char_len);
    char *end_ptr;
    num_node->number_value = strtol(temp_value, &end_ptr, 10);
    increment_token_index(parser);
    return num_node;
  }
  case STRING: {
    ast_node *string_node = malloc(sizeof(ast_node));
    string_node->node_type = PRIMARY_NODE;
    string_node->primary_node_type = STRING_PRIMARY_NODE;
    string_node->string_value = cur_tok;
    increment_token_index(parser);
    return string_node;
  }
  case IDENTIFIER: {
    ast_node *identifier_node = malloc(sizeof(ast_node));
    identifier_node->node_type = PRIMARY_NODE;
    identifier_node->primary_node_type = IDENTIFIER_PRIMARY_NODE;
    identifier_node->identifier_value = cur_tok;
    increment_token_index(parser);
    return identifier_node;
  }
  case TRUE:
  case FALSE: {
    ast_node *bool_node = malloc(sizeof(ast_node));
    bool_node->node_type = PRIMARY_NODE;
    bool_node->primary_node_type = BOOLEAN_PRIMARY_NODE;
    bool_node->boolean_value = cur_tok->type == TRUE ? true : false;
    increment_token_index(parser);
    return bool_node;
  }
  case NIL: {
    ast_node *nil_node = malloc(sizeof(ast_node));
    nil_node->node_type = PRIMARY_NODE;
    nil_node->primary_node_type = NIL_PRIMARY_NODE;
    increment_token_index(parser);
    return nil_node;
  }
  case LEFT_PAREN: {
    increment_token_index(parser);
    ast_node *expr = parse_expression(parser);
    cur_tok = get_current_token(parser);
    if (cur_tok->type != RIGHT_PAREN) {
      printf("Expecting ')' after the expression\n");
    }
    increment_token_index(parser);
    return expr;
  }
  default: {
    printf("Unsupported primary '%s'\n",
           get_string_from_token_atom(cur_tok->type));
  }
  }
  return NULL;
}

token *get_current_token(parser_state *parser) {
  return vector_at(parser->tokens, parser->current_token_index);
}

void increment_token_index(parser_state *parser) {
  parser->current_token_index++;
}

bool check_index_bound(parser_state *parser) {
  return parser->current_token_index < parser->tokens->size;
}

#endif
