#include "parser.h"
#include "ast.h"
#include "hash_table.h"
#include "interpreter.h"
#include "tokens.h"
#include "utils.h"
#include "vector.h"

struct vector *parse_program(struct vector *tokens) {
  struct parser_state parser = {.tokens = tokens, .current_token_index = 0};
  parser.current_line = get_current_token(&parser)->token_line;
  struct vector *program = vector_init();
  while (parser.current_token_index < tokens->size) {
    vector_push_back(program, parse_statement(&parser));
  }
  return program;
}

struct ast_node *parse_statement(struct parser_state *parser) {
  struct token *stmt = get_current_token(parser);
  switch (stmt->type) {
  case FUNCTION:
    return parse_function_definition_statement(parser);
  case LET:
    return parse_variable_declaration_statement(parser);
  case IDENTIFIER: {
    return parse_expression_statement(parser);
  }
  case IF: {
    return parse_if_else_statement(parser);
  }
  case WHILE: {
    return parse_while_statement(parser);
  }
  case FOR: {
    return parse_for_statement(parser);
  }
  case BREAK: {
    return parse_break_statement(parser);
  }
  case RETURN: {
    return parse_return_statement(parser);
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
parse_function_definition_statement(struct parser_state *parser) {
  increment_token_index(parser);
  struct ast_node *fn_def_stmt = malloc(sizeof(struct ast_node));
  fn_def_stmt->node_type = FN_DEF_STMT;
  struct token *current_token = get_current_token(parser);
  check_current_token_type(IDENTIFIER, "Function name must be identifier",
                           parser);
  fn_def_stmt->fn_def_stmt.id = create_token_string_copy(
      current_token->token_char, 0, current_token->token_char_len);
  increment_token_index(parser);
  fn_def_stmt->fn_def_stmt.parameters = vector_init();
  consume_token(LEFT_PAREN, parser);
  if (get_current_token(parser)->type != RIGHT_PAREN) {
    parse_function_definition_parameters(fn_def_stmt->fn_def_stmt.parameters,
                                         parser);
  }
  consume_token(RIGHT_PAREN, parser);
  fn_def_stmt->fn_def_stmt.block = parse_block_statement(parser);
  return fn_def_stmt;
}

void parse_function_definition_parameters(struct vector *parameters,
                                          struct parser_state *parser) {
  struct token *current_token = get_current_token(parser);
  check_current_token_type(IDENTIFIER,
                           "Parameter name must be identifier string", parser);
  char *parameter_id = create_token_string_copy(current_token->token_char, 0,
                                                current_token->token_char_len);
  vector_push_back(parameters, parameter_id);
  increment_token_index(parser);
  while (get_current_token(parser)->type == COMMA) {
    increment_token_index(parser);
    current_token = get_current_token(parser);
    check_current_token_type(
        IDENTIFIER, "Parameter name must be identifier string", parser);
    char *parameter_id = create_token_string_copy(
        current_token->token_char, 0, current_token->token_char_len);
    vector_push_back(parameters, parameter_id);
    increment_token_index(parser);
  }
}

struct ast_node *
parse_variable_declaration_statement(struct parser_state *parser) {
  increment_token_index(parser);
  struct ast_node *var_decl_stmt = malloc(sizeof(struct ast_node));
  var_decl_stmt->node_type = VARIABLE_DECL_STMT;
  struct token *current_token = get_current_token(parser);
  check_current_token_type(
      IDENTIFIER, "Variable declaration name must be identifier string",
      parser);
  char *id = create_token_string_copy(current_token->token_char, 0,
                                      current_token->token_char_len);
  var_decl_stmt->var_decl_stmt.id = id;
  increment_token_index(parser);
  consume_token(EQUAL, parser);
  var_decl_stmt->var_decl_stmt.expr = parse_expression(parser);
  consume_token(SEMICOLON, parser);
  return var_decl_stmt;
}

struct ast_node *
parse_variable_assignment_statement(struct parser_state *parser,
                                    struct ast_node *primary) {
  if (primary->primary_node_type != IDENTIFIER_PRIMARY_NODE &&
      primary->primary_node_type != ARRAY_ACCESS_PRIMARY_NODE) {
    printf("Variable assignments can only be performed on identifiers and "
           "arrays.\n");
    exit(1);
  }
  struct ast_node *var_assign_stmt = malloc(sizeof(struct ast_node));
  var_assign_stmt->node_type = VARIABLE_ASSIGN_STMT;
  var_assign_stmt->var_assign_stmt.primary = primary;
  consume_token(EQUAL, parser);
  var_assign_stmt->var_assign_stmt.expr = parse_expression(parser);
  consume_token(SEMICOLON, parser);
  return var_assign_stmt;
}

struct ast_node *parse_if_else_statement(struct parser_state *parser) {
  increment_token_index(parser);
  struct ast_node *if_stmt = malloc(sizeof(struct ast_node));
  if_stmt->node_type = IF_STMT;
  consume_token(LEFT_PAREN, parser);
  if_stmt->if_else_stmt.expr = parse_expression(parser);
  consume_token(RIGHT_PAREN, parser);
  if_stmt->if_else_stmt.if_block = parse_block_statement(parser);
  if_stmt->if_else_stmt.else_block = NULL;
  if (check_index_bound(parser) && get_current_token(parser)->type == ELSE) {
    increment_token_index(parser);
    if_stmt->if_else_stmt.else_block = parse_block_statement(parser);
  }
  return if_stmt;
}

struct ast_node *parse_while_statement(struct parser_state *parser) {
  increment_token_index(parser);
  struct ast_node *while_stmt = malloc(sizeof(struct ast_node));
  while_stmt->node_type = WHILE_STMT;
  consume_token(LEFT_PAREN, parser);
  while_stmt->while_stmt.expr = parse_expression(parser);
  consume_token(RIGHT_PAREN, parser);
  while_stmt->while_stmt.block = parse_block_statement(parser);
  return while_stmt;
}

struct ast_node *parse_for_statement(struct parser_state *parser) {
  increment_token_index(parser);
  struct ast_node *for_stmt = malloc(sizeof(struct ast_node));
  for_stmt->node_type = FOR_STMT;
  consume_token(LEFT_PAREN, parser);
  for_stmt->for_stmt.init_stmt = parse_statement(parser);
  check_ast_node_type(
      for_stmt->for_stmt.init_stmt, VARIABLE_DECL_STMT,
      "First statement in 'for' loop must be variable declaration statement",
      parser);
  for_stmt->for_stmt.expr_stmt = parse_statement(parser);
  check_ast_node_type(
      for_stmt->for_stmt.expr_stmt, EXPR_STMT,
      "Second statement in 'for' loop must be an expression statement", parser);
  for_stmt->for_stmt.update_stmt = parse_statement(parser);
  check_ast_node_type(
      for_stmt->for_stmt.update_stmt, VARIABLE_ASSIGN_STMT,
      "Third statement in 'for' loop must be variable assignment statement",
      parser);
  consume_token(RIGHT_PAREN, parser);
  for_stmt->for_stmt.block = parse_block_statement(parser);
  return for_stmt;
}

struct ast_node *parse_break_statement(struct parser_state *parser) {
  increment_token_index(parser);
  struct ast_node *break_stmt = malloc(sizeof(struct ast_node));
  break_stmt->node_type = BREAK_STMT;
  consume_token(SEMICOLON, parser);
  return break_stmt;
}

struct ast_node *parse_return_statement(struct parser_state *parser) {
  increment_token_index(parser);
  struct ast_node *return_stmt = malloc(sizeof(struct ast_node));
  return_stmt->node_type = RETURN_STMT;
  return_stmt->return_stmt_expr = parse_expression(parser);
  consume_token(SEMICOLON, parser);
  return return_stmt;
}

struct ast_node *parse_expression_statement(struct parser_state *parser) {
  struct ast_node *primary = parse_expression(parser);
  if (get_current_token(parser)->type == EQUAL) {
    return parse_variable_assignment_statement(parser, primary);
  }
  struct ast_node *expr_stmt = malloc(sizeof(struct ast_node));
  expr_stmt->node_type = EXPR_STMT;
  expr_stmt->expr_stmt_expr = primary;
  consume_token(SEMICOLON, parser);
  return expr_stmt;
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
  consume_token(RIGHT_BRACE, parser);
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
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
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
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
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
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
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
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
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
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
    left = new_left;
    op = get_current_token(parser);
  }
  return left;
}

struct ast_node *multiplicative(struct parser_state *parser) {
  struct ast_node *left = parse_unary(parser);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL &&
         (op->type == STAR || op->type == SLASH)) {
    increment_token_index(parser);
    struct ast_node *right = parse_unary(parser);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
    left = new_left;
    op = get_current_token(parser);
  }
  return left;
}

struct ast_node *parse_unary(struct parser_state *parser) {
  enum token_type current_token_type = get_current_token(parser)->type;
  if (current_token_type == MINUS || current_token_type == BANG) {
    increment_token_index(parser);
    struct ast_node *unary = malloc(sizeof(struct ast_node));
    unary->unary.op = current_token_type;
    unary->node_type = UNARY_NODE;
    unary->unary.op = current_token_type;
    unary->unary.primary = parse_extended_primary(parser);
    return unary;
  }
  return parse_extended_primary(parser);
}

struct ast_node *parse_extended_primary(struct parser_state *parser) {
  struct ast_node *primary = parse_primary(parser);
  while (get_current_token(parser)->type == LEFT_PAREN ||
         get_current_token(parser)->type == LEFT_BRACKET) {
    if (get_current_token(parser)->type == LEFT_BRACKET) {
      increment_token_index(parser);
      struct ast_node *index = parse_expression(parser);
      consume_token(RIGHT_BRACKET, parser);
      struct ast_node *array_access_primary = malloc(sizeof(struct ast_node));
      array_access_primary->node_type = PRIMARY_NODE;
      array_access_primary->primary_node_type = ARRAY_ACCESS_PRIMARY_NODE;
      array_access_primary->array_access.primary = primary;
      array_access_primary->array_access.index = index;
      primary = array_access_primary;
    } else {
      increment_token_index(parser);
      struct ast_node *fn_call = malloc(sizeof(struct ast_node));
      fn_call->node_type = PRIMARY_NODE;
      fn_call->primary_node_type = FN_CALL_PRIMARY_NODE;
      fn_call->fn_call.primary = primary;
      fn_call->fn_call.parameters = vector_init();
      if (get_current_token(parser)->type != RIGHT_PAREN) {
        parse_parameters(parser, fn_call->fn_call.parameters);
      }
      consume_token(RIGHT_PAREN, parser);
      primary = fn_call;
    }
  }
  if (get_current_token(parser)->type == DOT) {
    increment_token_index(parser);
    struct ast_node *method_call_primary = malloc(sizeof(struct ast_node));
    method_call_primary->node_type = PRIMARY_NODE;
    method_call_primary->primary_node_type = METHOD_CALL_PRIMARY_NODE;
    method_call_primary->method_call.object = primary;
    struct ast_node *method_call_member = parse_extended_primary(parser);
    method_call_primary->method_call.member = method_call_member;
    primary = method_call_primary;
  }
  return primary;
}

struct ast_node *parse_primary(struct parser_state *parser) {
  struct token *cur_tok = get_current_token(parser);
  switch (cur_tok->type) {
  case NUMBER: {
    struct ast_node *num_node = malloc(sizeof(struct ast_node));
    num_node->node_type = PRIMARY_NODE;
    num_node->primary_node_type = NUMBER_PRIMARY_NODE;
    char temp_value[100] = {0};
    memcpy(temp_value, cur_tok->token_char, cur_tok->token_char_len);
    char *end_ptr;
    num_node->number = strtol(temp_value, &end_ptr, 10);
    increment_token_index(parser);
    return num_node;
  }
  case STRING: {
    struct ast_node *string_node = malloc(sizeof(struct ast_node));
    string_node->node_type = PRIMARY_NODE;
    string_node->primary_node_type = STRING_PRIMARY_NODE;
    string_node->string = create_token_string_copy(cur_tok->token_char, 0,
                                                   cur_tok->token_char_len);
    increment_token_index(parser);
    return string_node;
  }
  case IDENTIFIER: {
    struct ast_node *identifier_node = malloc(sizeof(struct ast_node));
    identifier_node->node_type = PRIMARY_NODE;
    identifier_node->primary_node_type = IDENTIFIER_PRIMARY_NODE;
    identifier_node->id = create_token_string_copy(cur_tok->token_char, 0,
                                                   cur_tok->token_char_len);
    increment_token_index(parser);
    return identifier_node;
  }
  case TRUE:
  case FALSE: {
    struct ast_node *bool_node = malloc(sizeof(struct ast_node));
    bool_node->node_type = PRIMARY_NODE;
    bool_node->primary_node_type = BOOLEAN_PRIMARY_NODE;
    bool_node->boolean = cur_tok->type == TRUE ? true : false;
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
  case LEFT_BRACKET: {
    return parse_array_creation(parser);
  }
  case LEFT_PAREN: {
    increment_token_index(parser);
    struct ast_node *expr = parse_expression(parser);
    cur_tok = get_current_token(parser);
    consume_token(RIGHT_PAREN, parser);
    return expr;
  }
  default: {
    printf("Unsupported primary '%s'\n",
           get_string_from_token_atom(cur_tok->type));
    exit(1);
  }
  }
  return NULL;
}

void parse_parameters(struct parser_state *parser, struct vector *parameters) {
  vector_push_back(parameters, parse_expression(parser));
  while (get_current_token(parser)->type == COMMA) {
    increment_token_index(parser);
    vector_push_back(parameters, parse_expression(parser));
  }
}

struct ast_node *parse_array_creation(struct parser_state *parser) {
  increment_token_index(parser);
  struct vector *array = vector_init();
  if (get_current_token(parser)->type != RIGHT_BRACKET) {
    vector_push_back(array, parse_expression(parser));
    while (get_current_token(parser)->type == COMMA) {
      increment_token_index(parser);
      vector_push_back(array, parse_expression(parser));
    }
  }
  increment_token_index(parser);
  struct ast_node *array_node = malloc(sizeof(struct ast_node));
  array_node->node_type = PRIMARY_NODE;
  array_node->primary_node_type = ARRAY_CREATION_PRIMARY_NODE;
  array_node->array = array;
  return array_node;
}

struct token *get_previous_token(struct parser_state *parser) {
  return vector_at(parser->tokens, parser->current_token_index - 1);
}

struct token *get_current_token(struct parser_state *parser) {
  return vector_at(parser->tokens, parser->current_token_index);
}

struct token *get_next_token(struct parser_state *parser) {
  return vector_at(parser->tokens, parser->current_token_index + 1);
}

void increment_token_index(struct parser_state *parser) {
  if (check_index_bound(parser)) {
    parser->current_line = get_current_token(parser)->token_line;
  }
  parser->current_token_index++;
}

bool check_index_bound(struct parser_state *parser) {
  return parser->current_token_index < parser->tokens->size;
}

void consume_token(enum token_type expected_token,
                   struct parser_state *parser) {
  if (!check_index_bound(parser)) {
    printf("Parser terminated prematurely.\n");
    exit(1);
  }
  struct token *current_token = get_current_token(parser);
  if (current_token->type != expected_token) {
    printf("Line %li, Char %li:\tExpected %s, got %s (%.*s)\n",
           parser->current_line, current_token->token_char_start_index,
           get_string_from_token_atom(expected_token),
           get_string_from_token_atom(current_token->type),
           (int)current_token->token_char_len, current_token->token_char);
    exit(1);
  }
  increment_token_index(parser);
}

void check_current_token_type(enum token_type expected_token,
                              const char *error_message,
                              struct parser_state *parser) {
  struct token *current_token = get_current_token(parser);
  if (current_token->type != expected_token) {
    printf("Line %li, Char %li:\tExpected %s, got %s (%.*s). %s\n",
           parser->current_line, current_token->token_char_start_index,
           get_string_from_token_atom(expected_token),
           get_string_from_token_atom(current_token->type),
           (int)current_token->token_char_len, current_token->token_char,
           error_message);
    exit(1);
  }
}

void check_ast_node_type(struct ast_node *node,
                         enum ast_node_type expected_node_type,
                         const char *error_message,
                         struct parser_state *parser) {
  if (node->node_type != expected_node_type) {
    printf("Line %li:\t%s\n", parser->current_line, error_message);
    exit(1);
  }
}

void check_primary_ast_node_type(struct ast_node *node,
                                 enum ast_primary_node_type expected_node_type,
                                 const char *error_message,
                                 struct parser_state *parser) {
  if (node->primary_node_type != expected_node_type) {
    printf("Line %li:\t%s", parser->current_line, error_message);
    exit(1);
  }
}
