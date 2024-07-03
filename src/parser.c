#include "parser.h"
#include "ast.h"
#include "errors.h"
#include "hash_table.h"
#include "interpreter.h"
#include "tokens.h"
#include "utils.h"
#include "vector.h"

struct parser *parse_program(struct vector *tokens) {
  struct parser_state state = {.tokens = tokens, .current_token_index = 0};
  struct parser *parser = malloc(sizeof(struct parser));
  parser->parser_errors = false;
  parser->program = vector_init();
  while (state.current_token_index < tokens->size) {
    struct result *stmt = parse_statement(&state);
    if (stmt->type == RESULT_ERROR) {
      parser->parser_errors = true;
      // TODO: Handle error (skipping to next stmt)
      printf("PARSER ERROR: %s\n", stmt->error->message);
      exit(1);
    }
    vector_push_back(parser->program, stmt);
  }
  return parser;
}

struct result *parse_statement(struct parser_state *parser) {
  struct token *stmt = get_current_token(parser);
  switch (stmt->type) {
  case FUNCTION:
    return parse_function_definition_statement(parser);
  case LET:
    return parse_variable_declaration_statement(parser);
  case IDENTIFIER:
    return parse_expression_statement(parser);
  case IF:
    return parse_if_else_statement(parser);
  case WHILE:
    return parse_while_statement(parser);
  case FOR:
    return parse_for_statement(parser);
  case BREAK:
    return parse_break_statement(parser);
  case RETURN:
    return parse_return_statement(parser);
  case LEFT_BRACE:
    return parse_block_statement(parser);
  default: {
    char *error_message = format_string("Unsupported statement type '%s'\n",
                                        get_string_from_token_atom(stmt->type));
    return result_error(error_init(ERROR_SYNTAX, error_message, 0));
  }
  }
}

struct result *
parse_function_definition_statement(struct parser_state *parser) {
  struct ast_node *fn_def_stmt = malloc(sizeof(struct ast_node));
  fn_def_stmt->source_position.start_line =
      get_current_token(parser)->token_line;
  increment_token_index(parser);
  fn_def_stmt->node_type = FN_DEF_STMT;
  struct token *current_token = get_current_token(parser);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(
      check_current_token_type(IDENTIFIER, parser));
  fn_def_stmt->fn_def_stmt.id = create_token_string_copy(
      current_token->token_char, 0, current_token->token_char_len);
  increment_token_index(parser);
  fn_def_stmt->fn_def_stmt.parameters = vector_init();
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(LEFT_PAREN, parser));
  if (get_current_token(parser)->type != RIGHT_PAREN) {
    struct result *err = parse_function_definition_parameters(
        fn_def_stmt->fn_def_stmt.parameters, parser);
    CHECK_AND_RETURN_IF_ERROR_EXISTS(err);
  }
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(RIGHT_PAREN, parser));
  fn_def_stmt->fn_def_stmt.block = parse_block_statement(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(fn_def_stmt->fn_def_stmt.block);
  fn_def_stmt->source_position.end_line =
      get_previous_token(parser)->token_line;
  return result_ok_node(fn_def_stmt);
}

struct result *
parse_function_definition_parameters(struct vector *parameters,
                                     struct parser_state *parser) {
  struct token *current_token = get_current_token(parser);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(
      check_current_token_type(IDENTIFIER, parser));
  char *parameter_id = create_token_string_copy(current_token->token_char, 0,
                                                current_token->token_char_len);
  vector_push_back(parameters, parameter_id);
  increment_token_index(parser);
  while (get_current_token(parser)->type == COMMA) {
    increment_token_index(parser);
    current_token = get_current_token(parser);
    CHECK_AND_RETURN_IF_ERROR_EXISTS(
        check_current_token_type(IDENTIFIER, parser));
    char *parameter_id = create_token_string_copy(
        current_token->token_char, 0, current_token->token_char_len);
    vector_push_back(parameters, parameter_id);
    increment_token_index(parser);
  }
  return NULL;
}

struct result *
parse_variable_declaration_statement(struct parser_state *parser) {
  struct ast_node *var_decl_stmt = malloc(sizeof(struct ast_node));
  var_decl_stmt->source_position.start_line =
      get_current_token(parser)->token_line;
  increment_token_index(parser);
  var_decl_stmt->node_type = VARIABLE_DECL_STMT;
  struct token *current_token = get_current_token(parser);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(
      check_current_token_type(IDENTIFIER, parser));
  char *id = create_token_string_copy(current_token->token_char, 0,
                                      current_token->token_char_len);
  var_decl_stmt->var_decl_stmt.id = id;
  increment_token_index(parser);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(EQUAL, parser));
  var_decl_stmt->var_decl_stmt.expr = parse_expression(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(var_decl_stmt->var_decl_stmt.expr);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(SEMICOLON, parser));
  var_decl_stmt->source_position.end_line =
      get_previous_token(parser)->token_line;
  return result_ok_node(var_decl_stmt);
}

struct result *parse_variable_assignment_statement(struct parser_state *parser,
                                                   size_t start_line,
                                                   struct result *primary) {
  if (primary->node->primary_node_type != IDENTIFIER_PRIMARY_NODE &&
      primary->node->primary_node_type != ARRAY_ACCESS_PRIMARY_NODE) {
    char *error_message = strdup(
        "Variable assignments can only be performed on identifiers and arrays");
    struct error *err = error_init(ERROR_SYNTAX, error_message,
                                   get_current_token(parser)->token_line);
    return result_error(err);
  }
  struct ast_node *var_assign_stmt = malloc(sizeof(struct ast_node));
  var_assign_stmt->source_position.start_line = start_line;
  var_assign_stmt->node_type = VARIABLE_ASSIGN_STMT;
  var_assign_stmt->var_assign_stmt.primary = primary;
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(EQUAL, parser));
  var_assign_stmt->var_assign_stmt.expr = parse_expression(parser);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(SEMICOLON, parser));
  var_assign_stmt->source_position.end_line =
      get_previous_token(parser)->token_line;
  return result_ok_node(var_assign_stmt);
}

struct result *parse_if_else_statement(struct parser_state *parser) {
  struct ast_node *if_stmt = malloc(sizeof(struct ast_node));
  if_stmt->source_position.start_line = get_current_token(parser)->token_line;
  increment_token_index(parser);
  if_stmt->node_type = IF_STMT;
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(LEFT_PAREN, parser));
  if_stmt->if_else_stmt.expr = parse_expression(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(if_stmt->if_else_stmt.expr);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(RIGHT_PAREN, parser));
  if_stmt->if_else_stmt.if_block = parse_block_statement(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(if_stmt->if_else_stmt.if_block);
  if_stmt->if_else_stmt.else_block = NULL;
  if (check_index_bound(parser) && get_current_token(parser)->type == ELSE) {
    increment_token_index(parser);
    if_stmt->if_else_stmt.else_block = parse_block_statement(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(if_stmt->if_else_stmt.else_block);
  }
  if_stmt->source_position.end_line = get_previous_token(parser)->token_line;
  return result_ok_node(if_stmt);
}

struct result *parse_while_statement(struct parser_state *parser) {
  struct ast_node *while_stmt = malloc(sizeof(struct ast_node));
  while_stmt->source_position.start_line =
      get_current_token(parser)->token_line;
  increment_token_index(parser);
  while_stmt->node_type = WHILE_STMT;
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(LEFT_PAREN, parser));
  while_stmt->while_stmt.expr = parse_expression(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(while_stmt->while_stmt.expr);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(RIGHT_PAREN, parser));
  while_stmt->while_stmt.block = parse_block_statement(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(while_stmt->while_stmt.block);
  while_stmt->source_position.end_line = get_previous_token(parser)->token_line;
  return result_ok_node(while_stmt);
}

struct result *parse_for_statement(struct parser_state *parser) {
  struct ast_node *for_stmt = malloc(sizeof(struct ast_node));
  for_stmt->source_position.start_line = get_current_token(parser)->token_line;
  increment_token_index(parser);
  for_stmt->node_type = FOR_STMT;
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(LEFT_PAREN, parser));
  for_stmt->for_stmt.init_stmt = parse_statement(parser);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(check_ast_node_type(
      for_stmt->for_stmt.init_stmt->node, VARIABLE_DECL_STMT, parser));
  for_stmt->for_stmt.expr_stmt = parse_statement(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(for_stmt->for_stmt.expr_stmt);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(check_ast_node_type(
      for_stmt->for_stmt.expr_stmt->node, EXPR_STMT, parser));
  for_stmt->for_stmt.update_stmt = parse_statement(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(for_stmt->for_stmt.update_stmt);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(check_ast_node_type(
      for_stmt->for_stmt.update_stmt->node, VARIABLE_ASSIGN_STMT, parser));
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(RIGHT_PAREN, parser));
  for_stmt->for_stmt.block = parse_block_statement(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(for_stmt->for_stmt.block);
  for_stmt->source_position.end_line = get_previous_token(parser)->token_line;
  return result_ok_node(for_stmt);
}

struct result *parse_break_statement(struct parser_state *parser) {
  struct ast_node *break_stmt = malloc(sizeof(struct ast_node));
  break_stmt->source_position.start_line =
      get_current_token(parser)->token_line;
  increment_token_index(parser);
  break_stmt->node_type = BREAK_STMT;
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(SEMICOLON, parser));
  break_stmt->source_position.end_line = get_previous_token(parser)->token_line;
  return result_ok_node(break_stmt);
}

struct result *parse_return_statement(struct parser_state *parser) {
  struct ast_node *return_stmt = malloc(sizeof(struct ast_node));
  return_stmt->source_position.start_line =
      get_current_token(parser)->token_line;
  increment_token_index(parser);
  return_stmt->node_type = RETURN_STMT;
  return_stmt->return_stmt_expr = parse_expression(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(return_stmt->return_stmt_expr);
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(SEMICOLON, parser));
  return_stmt->source_position.end_line =
      get_previous_token(parser)->token_line;
  return result_ok_node(return_stmt);
}

struct result *parse_expression_statement(struct parser_state *parser) {
  size_t start_line = get_current_token(parser)->token_line;
  struct result *primary = parse_expression(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(primary);
  if (get_current_token(parser)->type == EQUAL) {
    return parse_variable_assignment_statement(parser, start_line, primary);
  }
  struct ast_node *expr_stmt = malloc(sizeof(struct ast_node));
  expr_stmt->source_position.start_line = start_line;
  expr_stmt->node_type = EXPR_STMT;
  expr_stmt->expr_stmt_expr = primary;
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(SEMICOLON, parser));
  expr_stmt->source_position.end_line = get_previous_token(parser)->token_line;
  return result_ok_node(expr_stmt);
}

struct result *parse_block_statement(struct parser_state *parser) {
  increment_token_index(parser);
  struct ast_node *block_stmt = malloc(sizeof(struct ast_node));
  block_stmt->node_type = BLOCK_STMT;
  block_stmt->block_stmt_stmts = vector_init();
  while (check_index_bound(parser) &&
         get_current_token(parser)->type != RIGHT_BRACE) {
    struct result *stmt = parse_statement(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(stmt);
    vector_push_back(block_stmt->block_stmt_stmts, stmt);
  }
  CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(RIGHT_BRACE, parser));
  return result_ok_node(block_stmt);
}

struct result *parse_expression(struct parser_state *parser) {
  return logical_or(parser);
}

struct result *logical_or(struct parser_state *parser) {
  struct result *left = logical_and(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(left);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL && op->type == OR) {
    increment_token_index(parser);
    struct result *right = logical_and(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(right);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
    struct result *new_left_result = result_ok_node(new_left);
    left = new_left_result;
    op = get_current_token(parser);
  }
  return left;
}

struct result *logical_and(struct parser_state *parser) {
  struct result *left = equality(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(left);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL && op->type == AND) {
    increment_token_index(parser);
    struct result *right = equality(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(right);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
    struct result *new_left_result = result_ok_node(new_left);
    left = new_left_result;
    op = get_current_token(parser);
  }
  return left;
}

struct result *equality(struct parser_state *parser) {
  struct result *left = comparitive(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(left);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL &&
         (op->type == EQUAL_EQUAL || op->type == BANG_EQUAL)) {
    increment_token_index(parser);
    struct result *right = comparitive(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(right);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
    struct result *new_left_result = result_ok_node(new_left);
    left = new_left_result;
    op = get_current_token(parser);
  }
  return left;
}

struct result *comparitive(struct parser_state *parser) {
  struct result *left = additive(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(left);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL &&
         (op->type == GREATER || op->type == GREATER_EQUAL ||
          op->type == LESS || op->type == LESS_EQUAL)) {
    increment_token_index(parser);
    struct result *right = additive(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(right);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
    struct result *new_left_result = result_ok_node(new_left);
    left = new_left_result;
    op = get_current_token(parser);
  }
  return left;
}

struct result *additive(struct parser_state *parser) {
  struct result *left = multiplicative(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(left);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL &&
         (op->type == PLUS || op->type == MINUS)) {
    increment_token_index(parser);
    struct result *right = multiplicative(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(right);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
    struct result *new_left_result = result_ok_node(new_left);
    left = new_left_result;
    op = get_current_token(parser);
  }
  return left;
}

struct result *multiplicative(struct parser_state *parser) {
  struct result *left = parse_unary(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(left);
  struct token *op = get_current_token(parser);
  while (check_index_bound(parser) && op != NULL &&
         (op->type == STAR || op->type == SLASH)) {
    increment_token_index(parser);
    struct result *right = parse_unary(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(right);
    struct ast_node *new_left = malloc(sizeof(struct ast_node));
    new_left->node_type = BINARY_NODE;
    new_left->binary.left = left;
    new_left->binary.right = right;
    new_left->binary.op = op->type;
    struct result *new_left_result = result_ok_node(new_left);
    left = new_left_result;
    op = get_current_token(parser);
  }
  return left;
}

struct result *parse_unary(struct parser_state *parser) {
  enum token_type current_token_type = get_current_token(parser)->type;
  if (current_token_type == MINUS || current_token_type == BANG) {
    increment_token_index(parser);
    struct ast_node *unary = malloc(sizeof(struct ast_node));
    unary->unary.op = current_token_type;
    unary->node_type = UNARY_NODE;
    unary->unary.op = current_token_type;
    unary->unary.primary = parse_extended_primary(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(unary->unary.primary);
    return result_ok_node(unary);
  }
  return parse_extended_primary(parser);
}

struct result *parse_extended_primary(struct parser_state *parser) {
  struct result *primary = parse_primary(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(primary);
  while (get_current_token(parser)->type == LEFT_PAREN ||
         get_current_token(parser)->type == LEFT_BRACKET) {
    if (get_current_token(parser)->type == LEFT_BRACKET) {
      increment_token_index(parser);
      struct result *index = parse_expression(parser);
      CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(index);
      CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(RIGHT_BRACKET, parser));
      struct ast_node *array_access_primary = malloc(sizeof(struct ast_node));
      array_access_primary->node_type = PRIMARY_NODE;
      array_access_primary->primary_node_type = ARRAY_ACCESS_PRIMARY_NODE;
      array_access_primary->array_access.primary = primary;
      array_access_primary->array_access.index = index;
      struct result *new_left_result = result_ok_node(array_access_primary);
      primary = new_left_result;
    } else {
      increment_token_index(parser);
      struct ast_node *fn_call = malloc(sizeof(struct ast_node));
      fn_call->node_type = PRIMARY_NODE;
      fn_call->primary_node_type = FN_CALL_PRIMARY_NODE;
      fn_call->fn_call.primary = primary;
      fn_call->fn_call.parameters = vector_init();
      if (get_current_token(parser)->type != RIGHT_PAREN) {
        CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(
            parse_parameters(parser, fn_call->fn_call.parameters));
      }
      CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(RIGHT_PAREN, parser));
      struct result *new_left_result = result_ok_node(fn_call);
      primary = new_left_result;
    }
  }
  if (get_current_token(parser)->type == DOT) {
    increment_token_index(parser);
    struct ast_node *method_call_primary = malloc(sizeof(struct ast_node));
    method_call_primary->node_type = PRIMARY_NODE;
    method_call_primary->primary_node_type = METHOD_CALL_PRIMARY_NODE;
    method_call_primary->method_call.object = primary;
    struct result *method_call_member = parse_extended_primary(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(method_call_member);
    method_call_primary->method_call.member = method_call_member;
    struct result *new_left_result = result_ok_node(method_call_primary);
    primary = new_left_result;
  }
  return primary;
}

struct result *parse_primary(struct parser_state *parser) {
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
    return result_ok_node(num_node);
  }
  case STRING: {
    struct ast_node *string_node = malloc(sizeof(struct ast_node));
    string_node->node_type = PRIMARY_NODE;
    string_node->primary_node_type = STRING_PRIMARY_NODE;
    string_node->string = create_token_string_copy(cur_tok->token_char, 0,
                                                   cur_tok->token_char_len);
    increment_token_index(parser);
    return result_ok_node(string_node);
  }
  case IDENTIFIER: {
    struct ast_node *identifier_node = malloc(sizeof(struct ast_node));
    identifier_node->node_type = PRIMARY_NODE;
    identifier_node->primary_node_type = IDENTIFIER_PRIMARY_NODE;
    identifier_node->id = create_token_string_copy(cur_tok->token_char, 0,
                                                   cur_tok->token_char_len);
    increment_token_index(parser);
    return result_ok_node(identifier_node);
  }
  case TRUE:
  case FALSE: {
    struct ast_node *bool_node = malloc(sizeof(struct ast_node));
    bool_node->node_type = PRIMARY_NODE;
    bool_node->primary_node_type = BOOLEAN_PRIMARY_NODE;
    bool_node->boolean = cur_tok->type == TRUE ? true : false;
    increment_token_index(parser);
    return result_ok_node(bool_node);
  }
  case NIL: {
    struct ast_node *nil_node = malloc(sizeof(struct ast_node));
    nil_node->node_type = PRIMARY_NODE;
    nil_node->primary_node_type = NIL_PRIMARY_NODE;
    increment_token_index(parser);
    return result_ok_node(nil_node);
  }
  case LEFT_BRACKET: {
    return parse_array_creation(parser);
  }
  case LEFT_PAREN: {
    increment_token_index(parser);
    struct result *expr = parse_expression(parser);
    cur_tok = get_current_token(parser);
    CHECK_AND_RETURN_IF_ERROR_EXISTS(consume_token(RIGHT_PAREN, parser));
    return expr;
  }
  default: {
    struct error *error =
        error_init(ERROR_SYNTAX, "Unsupported primary", cur_tok->token_line);
    return result_error(error);
  }
  }
}

struct result *parse_parameters(struct parser_state *parser,
                                struct vector *parameters) {
  struct result *expr = parse_expression(parser);
  CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(expr);
  vector_push_back(parameters, expr);
  while (get_current_token(parser)->type == COMMA) {
    increment_token_index(parser);
    expr = parse_expression(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(expr);
    vector_push_back(parameters, expr);
  }
  return result_ok_node(NULL);
}

struct result *parse_array_creation(struct parser_state *parser) {
  increment_token_index(parser);
  struct vector *array = vector_init();
  if (get_current_token(parser)->type != RIGHT_BRACKET) {
    struct result *expr = parse_expression(parser);
    CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(expr);
    vector_push_back(array, expr);
    while (get_current_token(parser)->type == COMMA) {
      increment_token_index(parser);
      expr = parse_expression(parser);
      CHECK_AND_RETURN_IF_ERROR_RESULT_NODE(expr);
      vector_push_back(array, expr);
    }
  }
  increment_token_index(parser);
  struct ast_node *array_node = malloc(sizeof(struct ast_node));
  array_node->node_type = PRIMARY_NODE;
  array_node->primary_node_type = ARRAY_CREATION_PRIMARY_NODE;
  array_node->array = array;
  return result_ok_node(array_node);
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
  parser->current_token_index++;
}

bool check_index_bound(struct parser_state *parser) {
  return parser->current_token_index < parser->tokens->size;
}

struct result *consume_token(enum token_type expected_token,
                             struct parser_state *parser) {
  if (!check_index_bound(parser)) {
    return result_error(error_init(ERROR_SYNTAX,
                                   "Parser terminated prematurely",
                                   get_previous_token(parser)->token_line));
  }
  struct token *current_token = get_current_token(parser);
  if (current_token->type != expected_token) {
    char *error_message = format_string(
        "Expected %s, but got %s", get_string_from_token_atom(expected_token),
        get_string_from_token_atom(current_token->type));
    struct error *error =
        error_init(ERROR_SYNTAX, error_message, current_token->token_line);
    return result_error(error);
  }
  increment_token_index(parser);
  return NULL;
}

struct result *check_current_token_type(enum token_type expected_token,
                                        struct parser_state *parser) {
  struct token *current_token = get_current_token(parser);
  if (current_token->type != expected_token) {
    char *error_message = format_string(
        "Expected %s, but got %s", get_string_from_token_atom(expected_token),
        get_string_from_token_atom(current_token->type));
    struct error *error =
        error_init(ERROR_SYNTAX, error_message, current_token->token_line);
    return result_error(error);
  }
  return NULL;
}

struct result *check_ast_node_type(struct ast_node *node,
                                   enum ast_node_type expected_node_type,
                                   struct parser_state *parser) {
  if (node->node_type != expected_node_type) {
    char *error_message =
        format_string("Expected %s, but got %s",
                      get_string_from_ast_node_type(expected_node_type),
                      get_string_from_ast_node_type(node->node_type));
    struct error *error = error_init(ERROR_SYNTAX, error_message,
                                     get_current_token(parser)->token_line);
    return result_error(error);
  }
  return NULL;
}

struct result *
check_primary_ast_node_type(struct ast_node *node,
                            enum ast_primary_node_type expected_node_type,
                            struct parser_state *parser) {
  if (node->primary_node_type != expected_node_type) {
    char *error_message = format_string(
        "Expected %s, but got %s",
        get_string_from_primary_ast_node_type(expected_node_type),
        get_string_from_primary_ast_node_type(node->primary_node_type));
    struct error *error = error_init(ERROR_SYNTAX, error_message,
                                     get_current_token(parser)->token_line);
    return result_error(error);
  }
  return NULL;
}
