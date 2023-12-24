#include "parser.h"
#include "ast.h"
#include "hash_table.h"
#include "tokens.h"
#include "utils.h"

struct vector *parse_program(struct vector *tokens) {
  struct parser_state parser = {.tokens = tokens, .current_token_index = 0};
  struct vector *program = vector_init();
  while (parser.current_token_index < tokens->size) {
    vector_push_back(program, parse_statement(&parser));
  }
  return program;
}

struct ast_node *parse_statement(struct parser_state *parser) {
  struct token *stmt = get_current_token(parser);
  switch (stmt->type) {
  case INT_DATATYPE:
  case STRING_DATATYPE:
  case BOOL_DATATYPE: {
    return parse_variable_declaration_statement(parser);
  }
  case IDENTIFIER: {
    return parse_variable_assignment_statement(parser);
  }
  case IF: {
    return parse_if_else_statement(parser);
  }
  case WHILE: {
    return parse_while_statement(parser);
  }
  case LEFT_BRACE: {
    return parse_block_statement(parser);
  }
  default: {
    printf("Unsupported statement type '%s'\n",
           get_string_from_token_atom(stmt->type));
    exit(1);
  }
  }
}

struct ast_node *
parse_variable_declaration_statement(struct parser_state *parser) {
  struct token *dtype = get_current_token(parser);
  switch (dtype->type) {
  case INT_DATATYPE:
  case STRING_DATATYPE:
  case BOOL_DATATYPE: {
    increment_token_index(parser);
    struct ast_node *var_decl_stmt = malloc(sizeof(struct ast_node));
    var_decl_stmt->node_type = VARIABLE_DECL_STMT;
    var_decl_stmt->var_decl_stmt_dtype = dtype->type;
    var_decl_stmt->var_decl_stmt_id = primary(parser);
    if (!check_index_bound(parser) ||
        get_current_token(parser)->type != EQUAL) {
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

struct ast_node *
parse_variable_assignment_statement(struct parser_state *parser) {
  struct ast_node *var_assign_stmt = malloc(sizeof(struct ast_node));
  var_assign_stmt->node_type = VARIABLE_ASSIGN_STMT;
  var_assign_stmt->assign_stmt_id = primary(parser);
  if (!check_index_bound(parser) || get_current_token(parser)->type != EQUAL) {
    printf("Variable assignment must have '='\n");
    exit(1);
  }
  increment_token_index(parser);
  var_assign_stmt->assign_stmt_expr = parse_expression(parser);
  if (!check_index_bound(parser) ||
      get_current_token(parser)->type != SEMICOLON) {
    printf("Statement must end with ';'\n");
    exit(1);
  }
  increment_token_index(parser);
  return var_assign_stmt;
}

struct ast_node *parse_if_else_statement(struct parser_state *parser) {
  increment_token_index(parser);
  struct ast_node *if_stmt = calloc(1, sizeof(struct ast_node));
  if_stmt->node_type = IF_STMT;
  if (get_current_token(parser)->type != LEFT_PAREN) {
    printf("Keyword 'if' must be followed by '(' <expression> ')'.\n");
    exit(1);
  }
  increment_token_index(parser);
  if_stmt->if_stmt_expr = parse_expression(parser);
  if (get_current_token(parser)->type != RIGHT_PAREN) {
    printf("The <expression> in 'if' must be followed by ')'.\n");
    exit(1);
  }
  increment_token_index(parser);
  if_stmt->if_stmt_block = parse_block_statement(parser);
  if (check_index_bound(parser) && get_current_token(parser)->type == ELSE) {
    increment_token_index(parser);
    if_stmt->if_else_stmt_block = parse_block_statement(parser);
  }
  return if_stmt;
}

struct ast_node *parse_while_statement(struct parser_state *parser) {
  increment_token_index(parser);
  struct ast_node *while_stmt = malloc(sizeof(struct ast_node));
  while_stmt->node_type = WHILE_STMT;
  if (get_current_token(parser)->type != LEFT_PAREN) {
    printf("Keyword 'while' must be followed by '(' <expression> ')'.\n");
    exit(1);
  }
  increment_token_index(parser);
  while_stmt->while_stmt_expr = parse_expression(parser);
  if (get_current_token(parser)->type != RIGHT_PAREN) {
    printf("The <expression> in 'while' must be followed by ')'.\n");
    exit(1);
  }
  increment_token_index(parser);
  while_stmt->while_stmt_block = parse_block_statement(parser);
  return while_stmt;
}

struct ast_node *parse_block_statement(struct parser_state *parser) {
  increment_token_index(parser);
  struct ast_node *block_stmt = malloc(sizeof(struct ast_node));
  block_stmt->node_type = BLOCK_STMT;
  block_stmt->block_stmt_stmts = vector_init();
  while (check_index_bound(parser) &&
         get_current_token(parser)->type != RIGHT_BRACE) {
    vector_push_back(block_stmt->block_stmt_stmts, parse_statement(parser));
  }
  if (get_current_token(parser)->type != RIGHT_BRACE) {
    printf("Block statement must end with '}'.\n");
    exit(1);
  }
  increment_token_index(parser);
  return block_stmt;
}

struct ast_node *parse_expression(struct parser_state *parser) {
  return logical_or(parser);
}

struct ast_node *logical_or(struct parser_state *parser) {
  struct ast_node *left = logical_and(parser);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL && op->type == OR) {
    increment_token_index(parser);
    struct ast_node *right = logical_and(parser);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->left = left;
    new_left->right = right;
    new_left->op = op->type;
    left = new_left;
    op = get_current_token(parser);
  }
  return left;
}

struct ast_node *logical_and(struct parser_state *parser) {
  struct ast_node *left = equality(parser);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL && op->type == AND) {
    increment_token_index(parser);
    struct ast_node *right = equality(parser);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->left = left;
    new_left->right = right;
    new_left->op = op->type;
    left = new_left;
    op = get_current_token(parser);
  }
  return left;
}

struct ast_node *equality(struct parser_state *parser) {
  struct ast_node *left = comparitive(parser);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL &&
         (op->type == EQUAL_EQUAL || op->type == BANG_EQUAL)) {
    increment_token_index(parser);
    struct ast_node *right = comparitive(parser);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->left = left;
    new_left->right = right;
    new_left->op = op->type;
    left = new_left;
    op = get_current_token(parser);
  }
  return left;
}

struct ast_node *comparitive(struct parser_state *parser) {
  struct ast_node *left = additive(parser);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL &&
         (op->type == GREATER || op->type == GREATER_EQUAL ||
          op->type == LESS || op->type == LESS_EQUAL)) {
    increment_token_index(parser);
    struct ast_node *right = additive(parser);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->left = left;
    new_left->right = right;
    new_left->op = op->type;
    left = new_left;
    op = get_current_token(parser);
  }
  return left;
}

struct ast_node *additive(struct parser_state *parser) {
  struct ast_node *left = multiplicative(parser);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL &&
         (op->type == PLUS || op->type == MINUS)) {
    increment_token_index(parser);
    struct ast_node *right = multiplicative(parser);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->left = left;
    new_left->right = right;
    new_left->op = op->type;
    left = new_left;
    op = get_current_token(parser);
  }
  return left;
}

struct ast_node *multiplicative(struct parser_state *parser) {
  struct ast_node *left = primary(parser);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL &&
         (op->type == STAR || op->type == SLASH)) {
    increment_token_index(parser);
    struct ast_node *right = primary(parser);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->left = left;
    new_left->right = right;
    new_left->op = op->type;
    left = new_left;
    op = get_current_token(parser);
  }
  return left;
}

struct ast_node *primary(struct parser_state *parser) {
  struct token *cur_tok = get_current_token(parser);
  switch (cur_tok->type) {
  case NUMBER: {
    struct ast_node *num_node = malloc(sizeof(struct ast_node));
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
    struct ast_node *string_node = malloc(sizeof(struct ast_node));
    string_node->node_type = PRIMARY_NODE;
    string_node->primary_node_type = STRING_PRIMARY_NODE;
    string_node->string_value = create_token_string_copy(
        cur_tok->token_char, 0, cur_tok->token_char_len);
    increment_token_index(parser);
    return string_node;
  }
  case IDENTIFIER: {
    struct ast_node *identifier_node = malloc(sizeof(struct ast_node));
    identifier_node->node_type = PRIMARY_NODE;
    identifier_node->primary_node_type = IDENTIFIER_PRIMARY_NODE;
    identifier_node->identifier_value = create_token_string_copy(
        cur_tok->token_char, 0, cur_tok->token_char_len);
    increment_token_index(parser);
    return identifier_node;
  }
  case TRUE:
  case FALSE: {
    struct ast_node *bool_node = malloc(sizeof(struct ast_node));
    bool_node->node_type = PRIMARY_NODE;
    bool_node->primary_node_type = BOOLEAN_PRIMARY_NODE;
    bool_node->boolean_value = cur_tok->type == TRUE ? true : false;
    increment_token_index(parser);
    return bool_node;
  }
  case NIL: {
    struct ast_node *nil_node = malloc(sizeof(struct ast_node));
    nil_node->node_type = PRIMARY_NODE;
    nil_node->primary_node_type = NIL_PRIMARY_NODE;
    increment_token_index(parser);
    return nil_node;
  }
  case LEFT_PAREN: {
    increment_token_index(parser);
    struct ast_node *expr = parse_expression(parser);
    cur_tok = get_current_token(parser);
    if (cur_tok->type != RIGHT_PAREN) {
      printf("Expecting ')' after the expression, got '%s'\n",
             get_string_from_token_atom(cur_tok->type));
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

struct token *get_current_token(struct parser_state *parser) {
  return vector_at(parser->tokens, parser->current_token_index);
}

void increment_token_index(struct parser_state *parser) {
  parser->current_token_index++;
}

bool check_index_bound(struct parser_state *parser) {
  return parser->current_token_index < parser->tokens->size;
}
