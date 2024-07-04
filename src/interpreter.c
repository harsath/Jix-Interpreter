#include "interpreter.h"
#include "ast.h"
#include "builtin_functions.h"
#include "errors.h"
#include "hash_table.h"
#include "parser.h"
#include "tokens.h"
#include "utils.h"
#include "vector.h"

struct object *interpret(struct vector *program) {
  if (!program) {
    return NULL;
  }
  struct interpreter_state state = {.env = environment_init(),
                                    .builtin_fns =
                                        init_and_register_builtin_fns()};
  struct return_value *return_code = malloc(sizeof(struct object));
  return_code->is_set = false;
  return_code->value = NULL;
  for (size_t i = 0; i < program->size; i++) {
    struct ast_node *stmt = vector_at(program, i);
    state.current_stmt_lines.start_line = stmt->source_position.start_line;
    state.current_stmt_lines.end_line = stmt->source_position.end_line;
    struct result *ret = interpret_statement(stmt, &state, return_code);
    if (ret->type == RESULT_ERROR) {
      print_interpreter_error(ret->error.runtime);
      exit(1);
    }
  }
  return return_code->value->object;
}

struct result *interpret_statement(struct ast_node *statement,
                                   struct interpreter_state *state,
                                   struct return_value *return_code) {
  if (!return_code->is_set) {
    switch (statement->node_type) {
    case FN_DEF_STMT:
      return interpret_fn_def_statement(statement, state);
    case VARIABLE_DECL_STMT:
      return interpret_variable_decl_statement(statement, state, return_code);
    case VARIABLE_ASSIGN_STMT:
      return interpret_variable_assignment_statement(statement, state,
                                                     return_code);
    case IF_STMT:
      return interpret_if_statement(statement, state, return_code);
    case WHILE_STMT:
      return interpret_while_statement(statement, state, return_code);
    case FOR_STMT:
      return interpret_for_statement(statement, state, return_code);
    case BREAK_STMT:
      return interpret_break_statement(statement, state, return_code);
    case RETURN_STMT:
      return interpret_return_statement(statement, state, return_code);
    case BLOCK_STMT:
      return interpret_block_statement(statement, state, return_code);
    case EXPR_STMT:
      return interpret_expr_statement(statement, state, return_code);
    default: {
      char *error_message = strdup("Invalid statement");
      return result_error_runtime(runtime_error_init(
          error_message, statement->source_position.start_line,
          statement->source_position.end_line));
    }
    }
  }
  return result_ok_object(NULL);
}

struct result *interpret_fn_def_statement(struct ast_node *stmt_node,
                                          struct interpreter_state *state) {
  /* Functions are local to environment. Child environments have access to
   * parent environment, but not vice versa. Works much like variable
   * declaration statements. */
  if (environment_lookup_symbol_current_env(state->env,
                                            stmt_node->fn_def_stmt.id)) {
    char *error_message =
        format_string("Function '%s' already exists in current scope",
                      stmt_node->fn_def_stmt.id);
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  struct function *fn_stmt = malloc(sizeof(struct function));
  fn_stmt->body = stmt_node->fn_def_stmt.block->node;
  fn_stmt->parameters = stmt_node->fn_def_stmt.parameters;
  struct object *fn_stmt_value = malloc(sizeof(struct object));
  fn_stmt_value->data_type = FUNCTION_VALUE;
  fn_stmt_value->function_value.is_builtin = false;
  fn_stmt_value->function_value.function_value = fn_stmt;
  environment_insert_symbol(state->env, stmt_node->fn_def_stmt.id,
                            fn_stmt_value);
  return result_ok_object(NULL);
}

struct result *
interpret_variable_decl_statement(struct ast_node *stmt_node,
                                  struct interpreter_state *state,
                                  struct return_value *return_code) {
  /* Allow creating scope-local variable name of same name in a scope even
   * if it exists in previous scopes. */
  if (environment_lookup_symbol_current_env(state->env,
                                            stmt_node->var_decl_stmt.id)) {
    char *error_message =
        format_string("Variable '%s' already exists in current scope",
                      stmt_node->var_decl_stmt.id);
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  struct result *variable_value =
      eval_expression(stmt_node->var_decl_stmt.expr->node, state, return_code);
  RETURN_RESULT_IF_ERROR(variable_value);
  environment_insert_symbol(state->env, stmt_node->var_decl_stmt.id,
                            variable_value->object);
  return result_ok_object(NULL);
}

struct result *
interpret_variable_assignment_statement(struct ast_node *stmt_node,
                                        struct interpreter_state *state,
                                        struct return_value *return_code) {
  /* Check current scope, if not traverse to previous parent scope. */
  char *error_message;
  if (stmt_node->var_assign_stmt.primary->node->primary_node_type ==
      IDENTIFIER_PRIMARY_NODE) {
    if (environment_lookup_symbol(
            state->env, stmt_node->var_assign_stmt.primary->node->id) == NULL) {
      error_message =
          format_string("Variable '%s' does not exist",
                        stmt_node->var_assign_stmt.primary->node->id);
      return result_error_runtime(runtime_error_init(
          error_message, state->current_stmt_lines.start_line,
          state->current_stmt_lines.end_line));
    }
    struct result *variable_value = eval_expression(
        stmt_node->var_assign_stmt.expr->node, state, return_code);
    RETURN_RESULT_IF_ERROR(variable_value);
    environment_reassign_symbol(state->env,
                                stmt_node->var_assign_stmt.primary->node->id,
                                variable_value->object);
  } else {
    struct result *array_obj = eval_primary_expression(
        stmt_node->var_assign_stmt.primary->node->array_access.primary->node,
        state, return_code);
    RETURN_RESULT_IF_ERROR(array_obj);
    if (array_obj->object->data_type != ARRAY_VALUE) {
      error_message =
          strdup("Variable array assignment can only be used for arrays");
      return result_error_runtime(runtime_error_init(
          error_message, state->current_stmt_lines.start_line,
          state->current_stmt_lines.end_line));
    }
    struct result *array_index = eval_expression(
        stmt_node->var_assign_stmt.primary->node->array_access.index->node,
        state, return_code);
    RETURN_RESULT_IF_ERROR(array_index);
    if (array_index->object->data_type != INT_VALUE) {
      error_message =
          strdup("Variable array assignment index must be an integer");
      return result_error_runtime(runtime_error_init(
          error_message, state->current_stmt_lines.start_line,
          state->current_stmt_lines.end_line));
    }
    if (array_index->object->int_value >=
        array_obj->object->array_value->size) {
      error_message = strdup("Index out of bound");
      return result_error_runtime(runtime_error_init(
          error_message, state->current_stmt_lines.start_line,
          state->current_stmt_lines.end_line));
    }
    struct result *expr = eval_expression(stmt_node->var_assign_stmt.expr->node,
                                          state, return_code);
    RETURN_RESULT_IF_ERROR(expr);
    vector_replace_at(array_obj->object->array_value,
                      array_index->object->int_value, expr->object);
  }
  return result_ok_object(NULL);
}

struct result *interpret_if_statement(struct ast_node *stmt_node,
                                      struct interpreter_state *state,
                                      struct return_value *return_code) {
  struct result *if_expr =
      eval_expression(stmt_node->if_else_stmt.expr->node, state, return_code);
  RETURN_RESULT_IF_ERROR(if_expr);
  if (if_expr->object->data_type != BOOLEAN_VALUE) {
    char *error_message = strdup("The result of the <expression> inside 'if' "
                                 "statement should result in a boolean value");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  if (if_expr->object->bool_value) {
    struct result *ret = interpret_block_statement(
        stmt_node->if_else_stmt.if_block->node, state, return_code);
    RETURN_RESULT_IF_ERROR(ret);
  } else {
    if (stmt_node->if_else_stmt.else_block != NULL) {
      struct result *ret = interpret_block_statement(
          stmt_node->if_else_stmt.else_block->node, state, return_code);
      RETURN_RESULT_IF_ERROR(ret);
    }
  }
  return result_ok_object(NULL);
}

struct result *interpret_while_statement(struct ast_node *stmt_node,
                                         struct interpreter_state *state,
                                         struct return_value *return_code) {
  struct result *while_expr =
      eval_expression(stmt_node->while_stmt.expr->node, state, return_code);
  RETURN_RESULT_IF_ERROR(while_expr);
  if (while_expr->object->data_type != BOOLEAN_VALUE) {
    char *error_message =
        strdup("The <expression> of 'while' must return boolean");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  while (while_expr->object->bool_value) {
    if (state->is_break) {
      state->is_break = false;
      break;
    }
    struct result *ret = interpret_block_statement(
        stmt_node->while_stmt.block->node, state, return_code);
    RETURN_RESULT_IF_ERROR(ret);
    while_expr =
        eval_expression(stmt_node->while_stmt.expr->node, state, return_code);
    RETURN_RESULT_IF_ERROR(while_expr);
  }
  return result_ok_object(NULL);
}

struct result *interpret_for_statement(struct ast_node *stmt_node,
                                       struct interpreter_state *state,
                                       struct return_value *return_code) {
  struct environment *parent_env = state->env;
  struct environment *block_env = environment_init_enclosed(state->env);
  state->env = block_env;
  struct result *ret = interpret_variable_decl_statement(
      stmt_node->for_stmt.init_stmt->node, state, return_code);
  RETURN_RESULT_IF_ERROR(ret);
  struct result *for_expr =
      eval_expression(stmt_node->for_stmt.expr_stmt->node->expr_stmt_expr->node,
                      state, return_code);
  RETURN_RESULT_IF_ERROR(for_expr);
  if (for_expr->object->data_type != BOOLEAN_VALUE) {
    char *error_message =
        strdup("The expression of 'for' loop must result in a boolean value");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  while (for_expr->object->bool_value) {
    if (state->is_break) {
      state->is_break = false;
      break;
    }
    struct result *ret = interpret_block_statement(
        stmt_node->for_stmt.block->node, state, return_code);
    RETURN_RESULT_IF_ERROR(ret);
    ret = interpret_variable_assignment_statement(
        stmt_node->for_stmt.update_stmt->node, state, return_code);
    RETURN_RESULT_IF_ERROR(ret);
    for_expr = eval_expression(
        stmt_node->for_stmt.expr_stmt->node->expr_stmt_expr->node, state,
        return_code);
  }
  return result_ok_object(NULL);
}

struct result *interpret_break_statement(struct ast_node *stmt_node,
                                         struct interpreter_state *state,
                                         struct return_value *return_code) {
  state->is_break = true;
  return result_ok_object(NULL);
}

struct result *interpret_return_statement(struct ast_node *stmt_node,
                                          struct interpreter_state *state,
                                          struct return_value *return_code) {
  struct result *return_expr =
      eval_expression(stmt_node->return_stmt_expr->node, state, return_code);
  RETURN_RESULT_IF_ERROR(return_expr);
  return_code->is_set = true;
  return_code->value = return_expr;
  return result_ok_object(NULL);
}

struct result *interpret_block_statement(struct ast_node *stmt_node,
                                         struct interpreter_state *state,
                                         struct return_value *return_code) {
  struct environment *parent_env = state->env;
  struct environment *block_env = environment_init_enclosed(state->env);
  state->env = block_env;
  for (size_t i = 0; i < stmt_node->block_stmt_stmts->size; i++) {
    if (return_code->is_set || state->is_break) {
      break;
    }
    struct result *ret = interpret_statement(
        vector_at(stmt_node->block_stmt_stmts, i), state, return_code);
    RETURN_RESULT_IF_ERROR(ret);
  }
  state->env = parent_env;
  return result_ok_object(NULL);
}

struct result *interpret_expr_statement(struct ast_node *stmt_node,
                                        struct interpreter_state *state,
                                        struct return_value *return_code) {
  struct result *ret =
      eval_expression(stmt_node->expr_stmt_expr->node, state, return_code);
  RETURN_RESULT_IF_ERROR(ret);
  return result_ok_object(NULL);
}

struct result *eval_expression(struct ast_node *ast,
                               struct interpreter_state *state,
                               struct return_value *return_code) {
  switch (ast->node_type) {
  case BINARY_NODE:
    return eval_binary_expression(ast, state, return_code);
  case UNARY_NODE:
    return eval_unary_expression(ast, state, return_code);
  case PRIMARY_NODE:
    return eval_primary_expression(ast, state, return_code);
  default: {
    char *error_message =
        strdup("Invalid expression type inside `eval_expression`");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  }
}

struct result *eval_binary_expression(struct ast_node *ast,
                                      struct interpreter_state *state,
                                      struct return_value *return_code) {
  struct result *lhs =
      eval_expression(ast->binary.left->node, state, return_code);
  RETURN_RESULT_IF_ERROR(lhs);
  struct result *rhs =
      eval_expression(ast->binary.right->node, state, return_code);
  RETURN_RESULT_IF_ERROR(rhs);
  switch (ast->binary.op) {
  case OR:
  case AND:
    return eval_logical_expression(ast->binary.op, lhs->object, rhs->object);
  case EQUAL_EQUAL:
  case BANG_EQUAL:
    return eval_equality_expression(ast->binary.op, lhs->object, rhs->object,
                                    state);
  case GREATER:
  case GREATER_EQUAL:
  case LESS:
  case LESS_EQUAL:
    return eval_comparitive_expression(ast->binary.op, lhs->object, rhs->object,
                                       state);
  case PLUS:
  case MINUS:
  case STAR:
  case SLASH:
    return eval_additive_multiplicative_expression(ast->binary.op, lhs->object,
                                                   rhs->object, state);
  default: {
    char *error_message =
        format_string("Invalid operation '%s' in binary node",
                      get_string_from_token_atom(ast->binary.op));
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  }
}

struct result *eval_unary_expression(struct ast_node *ast,
                                     struct interpreter_state *state,
                                     struct return_value *return_code) {
  struct result *primary_expr =
      eval_primary_expression(ast->unary.primary->node, state, return_code);
  RETURN_RESULT_IF_ERROR(primary_expr);
  char *error_message;
  if (ast->unary.op != NIL) {
    switch (ast->unary.op) {
    case MINUS: {
      if (primary_expr->object->data_type != INT_VALUE) {
        error_message = strdup("Unary '-' can only be applied to integers");
        return result_error_runtime(runtime_error_init(
            error_message, state->current_stmt_lines.start_line,
            state->current_stmt_lines.end_line));
      }
      primary_expr->object->int_value = -primary_expr->object->int_value;
      break;
    }
    case BANG: {
      if (primary_expr->object->data_type != BOOLEAN_VALUE) {
        char *error_message =
            strdup("Unary '!' can only be applied to booleans");
        return result_error_runtime(runtime_error_init(
            error_message, state->current_stmt_lines.start_line,
            state->current_stmt_lines.end_line));
      }
      primary_expr->object->bool_value = !primary_expr->object->bool_value;
      break;
    }
    default: {
      char *error_message = strdup("Invalid unary operation");
      return result_error_runtime(runtime_error_init(
          error_message, state->current_stmt_lines.start_line,
          state->current_stmt_lines.end_line));
    }
    }
  }
  return primary_expr;
}

struct result *eval_logical_expression(enum token_type op, struct object *lhs,
                                       struct object *rhs) {
  struct object *returner = malloc(sizeof(struct object));
  returner->data_type = BOOLEAN_VALUE;
  returner->bool_value = (op == AND) ? (lhs->bool_value && rhs->bool_value)
                                     : (lhs->bool_value || rhs->bool_value);
  return result_ok_object(returner);
}

struct result *eval_equality_expression(enum token_type op, struct object *lhs,
                                        struct object *rhs,
                                        struct interpreter_state *state) {
  struct object *returner = malloc(sizeof(struct object));
  returner->data_type = BOOLEAN_VALUE;
  if (lhs->data_type == INT_VALUE && rhs->data_type == INT_VALUE) {
    returner->bool_value = (op == EQUAL_EQUAL)
                               ? (lhs->int_value == rhs->int_value)
                               : (lhs->int_value != rhs->int_value);
  } else if (lhs->data_type == BOOLEAN_VALUE &&
             rhs->data_type == BOOLEAN_VALUE) {
    returner->bool_value = (op == EQUAL_EQUAL)
                               ? (lhs->bool_value == rhs->bool_value)
                               : (lhs->bool_value != rhs->bool_value);
  } else if (lhs->data_type == STRING_VALUE && rhs->data_type == STRING_VALUE) {
    returner->bool_value =
        (op == EQUAL_EQUAL)
            ? (strcmp(lhs->string_value, rhs->string_value) == 0)
            : false;
  } else {

    char *error_message =
        strdup("Equality operator can only be performed between two operands "
               "of integers, strings, or booleans");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  return result_ok_object(returner);
}

struct result *eval_comparitive_expression(enum token_type op,
                                           struct object *lhs,
                                           struct object *rhs,
                                           struct interpreter_state *state) {
  struct object *returner = malloc(sizeof(struct object));
  returner->data_type = BOOLEAN_VALUE;
  if (lhs->data_type != INT_VALUE || rhs->data_type != INT_VALUE) {
    char *error_message = strdup("Comparitive expression can only be "
                                 "performed between two integers.\n");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  long lhs_value = lhs->int_value;
  long rhs_value = rhs->int_value;
  switch (op) {
  case GREATER: {
    returner->bool_value = (lhs_value > rhs_value);
    break;
  }
  case GREATER_EQUAL: {
    returner->bool_value = (lhs_value >= rhs_value);
    break;
  }
  case LESS: {
    returner->bool_value = (lhs_value < rhs_value);
    break;
  }
  case LESS_EQUAL: {
    returner->bool_value = (lhs_value <= rhs_value);
    break;
  }
  default: {
  }
  }
  return result_ok_object(returner);
}

struct result *
eval_additive_multiplicative_expression(enum token_type op, struct object *lhs,
                                        struct object *rhs,
                                        struct interpreter_state *state) {
  struct object *returner = malloc(sizeof(struct object));
  if (lhs->data_type == STRING_VALUE || rhs->data_type == STRING_VALUE) {
    if (op != PLUS) {
      char *error_message = strdup("Only '+' can be performed on strings");
      return result_error_runtime(runtime_error_init(
          error_message, state->current_stmt_lines.start_line,
          state->current_stmt_lines.end_line));
    }
    const char *lhs_string;
    const char *rhs_string;
    if (lhs->data_type != STRING_VALUE) {
      lhs_string = convert_object_to_string(lhs);
      rhs_string = rhs->string_value;
    } else {
      lhs_string = lhs->string_value;
      rhs_string = convert_object_to_string(rhs);
    }
    size_t lhs_length = strlen(lhs_string);
    size_t rhs_length = strlen(rhs_string);
    returner->data_type = STRING_VALUE;
    returner->string_value = format_string("%s%s", lhs_string, rhs_string);
    return result_ok_object(returner);
  }
  returner->data_type = INT_VALUE;
  if (lhs->data_type != INT_VALUE || rhs->data_type != INT_VALUE) {
    char *error_message =
        strdup("For additive and multiplicative expressions, both operands "
               "must be of integer type or strings");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  switch (op) {
  case PLUS: {
    returner->int_value = lhs->int_value + rhs->int_value;
    break;
  }
  case MINUS: {
    returner->int_value = lhs->int_value - rhs->int_value;
    break;
  }
  case STAR: {
    returner->int_value = lhs->int_value * rhs->int_value;
    break;
  }
  case SLASH: {
    returner->int_value = lhs->int_value / rhs->int_value;
    break;
  }
  default: {
    /* The 'op' check is already performed by `eval_expression' parent
     * function.
     */
  }
  }
  return result_ok_object(returner);
}

struct result *eval_primary_expression(struct ast_node *ast,
                                       struct interpreter_state *state,
                                       struct return_value *return_code) {
  struct object *returner = malloc(sizeof(struct object));
  switch (ast->primary_node_type) {
  case NUMBER_PRIMARY_NODE: {
    returner->data_type = INT_VALUE;
    returner->int_value = ast->number;
    break;
  }
  case STRING_PRIMARY_NODE: {
    returner->data_type = STRING_VALUE;
    returner->string_value = ast->string;
    break;
  }
  case BOOLEAN_PRIMARY_NODE: {
    returner->data_type = BOOLEAN_VALUE;
    returner->bool_value = ast->boolean;
    break;
  }
  case IDENTIFIER_PRIMARY_NODE: {
    struct object *symbol_lookup =
        environment_lookup_symbol(state->env, ast->id);
    if (!symbol_lookup) {
      struct builtin_fn *builtin_function =
          lookup_builtin_fns(state->builtin_fns, ast->id);
      if (builtin_function != NULL) {
        returner->data_type = FUNCTION_VALUE;
        returner->function_value.is_builtin = true;
        returner->function_value.builtin_function = builtin_function;
        break;
      } else {
        char *error_message =
            format_string("Identifier '%s' does not exist", ast->id);
        return result_error_runtime(runtime_error_init(
            error_message, state->current_stmt_lines.start_line,
            state->current_stmt_lines.end_line));
      }
    }
    returner = symbol_lookup;
    break;
  }
  case NIL_PRIMARY_NODE: {
    returner->data_type = NIL_VALUE;
    break;
  }
  case FN_CALL_PRIMARY_NODE: {
    struct result *ret =
        eval_fn_call_primary_expression(ast, state, return_code);
    RETURN_RESULT_IF_ERROR(ret);
    returner = ret->object;
    break;
  }
  case METHOD_CALL_PRIMARY_NODE: {
    struct result *ret =
        eval_method_call_primary_expression(ast, state, return_code);
    RETURN_RESULT_IF_ERROR(ret);
    returner = ret->object;
    break;
  }
  case ARRAY_CREATION_PRIMARY_NODE: {
    struct result *ret =
        eval_array_creation_primary_expression(ast, state, return_code);
    RETURN_RESULT_IF_ERROR(ret);
    returner = ret->object;
    break;
  }
  case ARRAY_ACCESS_PRIMARY_NODE: {
    struct result *ret =
        eval_array_access_primary_expression(ast, state, return_code);
    RETURN_RESULT_IF_ERROR(ret);
    returner = ret->object;
    break;
  }
  default: {
    char *error_message = strdup("Unimplemented primary expression");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  }
  return result_ok_object(returner);
}

struct result *
eval_fn_call_primary_expression(struct ast_node *ast,
                                struct interpreter_state *state,
                                struct return_value *return_code) {
  struct result *primary_eval =
      eval_primary_expression(ast->fn_call.primary->node, state, return_code);
  RETURN_RESULT_IF_ERROR(primary_eval);
  struct object *fn_call_primary_eval = primary_eval->object;
  char *error_message;
  if (fn_call_primary_eval->data_type != FUNCTION_VALUE) {
    error_message = strdup("Function calls can only be performed on callable");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }

  /* Handle builtin functions */
  if (fn_call_primary_eval->function_value.is_builtin) {
    return eval_builtin_fn_call_primary_expression(ast, fn_call_primary_eval,
                                                   state, return_code);
  }

  /* Handle user-define functions */
  struct environment *parent_env = state->env;
  struct environment *fn_call_env = environment_init_enclosed(parent_env);
  for (size_t i = 0; i < ast->fn_call.parameters->size; i++) {
    struct result *val = vector_at(ast->fn_call.parameters, i);
    struct result *parameter_eval =
        eval_expression(val->node, state, return_code);
    RETURN_RESULT_IF_ERROR(parameter_eval);
    char *parameter_id = vector_at(
        fn_call_primary_eval->function_value.function_value->parameters, i);
    environment_insert_symbol(fn_call_env, parameter_id,
                              parameter_eval->object);
  }
  state->env = fn_call_env;
  struct result *ret = interpret_block_statement(
      fn_call_primary_eval->function_value.function_value->body, state,
      return_code);
  RETURN_RESULT_IF_ERROR(ret);
  struct object *returner = NULL;
  if (return_code->is_set) {
    return_code->is_set = false;
    returner = return_code->value->object;
  }
  state->env = parent_env;
  return result_ok_object(returner);
}

struct result *eval_builtin_fn_call_primary_expression(
    struct ast_node *ast, struct object *fn_call_primary,
    struct interpreter_state *state, struct return_value *return_code) {
  /* Check builtin function's arity */
  size_t builtin_fn_arity =
      fn_call_primary->function_value.builtin_function->num_parameters;
  if (ast->fn_call.parameters->size != builtin_fn_arity) {
    char *error_message =
        format_string("Function '%s' takes %ld, gut given %ld",
                      fn_call_primary->function_value.builtin_function->fn_name,
                      builtin_fn_arity, ast->fn_call.parameters->size);
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  /* Invoke the builtin function based on arity */
  if (builtin_fn_arity == 1) {
    void *(*fn_ptr)(void *) =
        fn_call_primary->function_value.builtin_function->fn_ptr;
    struct result *val_1 = vector_at(ast->fn_call.parameters, 0);
    struct result *expr_eval = eval_expression(val_1->node, state, return_code);
    RETURN_RESULT_IF_ERROR(expr_eval);
    fn_ptr(expr_eval->object);
  } else if (builtin_fn_arity == 2) {
    void *(*fn_ptr)(void *, void *) =
        fn_call_primary->function_value.builtin_function->fn_ptr;
    struct result *val_1 = vector_at(ast->fn_call.parameters, 0);
    struct result *val_2 = vector_at(ast->fn_call.parameters, 1);
    struct result *expr_eval_1 =
        eval_expression(val_1->node, state, return_code);
    RETURN_RESULT_IF_ERROR(expr_eval_1);
    struct result *expr_eval_2 =
        eval_expression(val_2->node, state, return_code);
    RETURN_RESULT_IF_ERROR(expr_eval_2);
    fn_ptr(expr_eval_1, expr_eval_2);
  } else {
    char *error_message =
        strdup("Unsupported number of parameters to bulitn function");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  return_code->is_set = false;
  return result_ok_object(NULL);
}

struct result *
eval_method_call_primary_expression(struct ast_node *ast,
                                    struct interpreter_state *state,
                                    struct return_value *return_code) {
  /* Method calls are currently only supported for arrays. This will
   * change once we add support for user-defined types. */
  struct object *returner = NULL;
  struct result *array_obj = eval_primary_expression(
      ast->array_access.primary->node, state, return_code);
  RETURN_RESULT_IF_ERROR(array_obj);
  if (array_obj->object->data_type == ARRAY_VALUE) {
    if (ast->method_call.member->node->primary_node_type !=
        FN_CALL_PRIMARY_NODE) {
      char *error_message = strdup("Array methods can only be function calls");
      return result_error_runtime(runtime_error_init(
          error_message, state->current_stmt_lines.start_line,
          state->current_stmt_lines.end_line));
    }
    struct result *array_method_call_primary =
        ast->method_call.member->node->fn_call.primary;
    if (array_method_call_primary->node->primary_node_type !=
        IDENTIFIER_PRIMARY_NODE) {
      char *error_message =
          strdup("Method calls to array should must be an identifier type");
      return result_error_runtime(runtime_error_init(
          error_message, state->current_stmt_lines.start_line,
          state->current_stmt_lines.end_line));
    }
    if (strcmp(array_method_call_primary->node->id, "add") == 0) {
      for (size_t i = 0;
           i < ast->method_call.member->node->fn_call.parameters->size; i++) {
        struct result *val =
            vector_at(ast->method_call.member->node->fn_call.parameters, i);
        struct result *ret = eval_expression(val->node, state, return_code);
        RETURN_RESULT_IF_ERROR(ret);
        vector_push_back(array_obj->object->array_value, ret->object);
      }
    } else if (strcmp(array_method_call_primary->node->id, "len") == 0) {
      returner = malloc(sizeof(struct object));
      returner->data_type = INT_VALUE;
      returner->int_value = array_obj->object->array_value->size;
    } else if (strcmp(array_method_call_primary->node->id, "pop") == 0) {
      if (array_obj->object->array_value->size <= 0) {
        char *error_message = strdup("Calling .pop() on an empty array");
        return result_error_runtime(runtime_error_init(
            error_message, state->current_stmt_lines.start_line,
            state->current_stmt_lines.end_line));
      }
      struct vector *member_parameter =
          ast->method_call.member->node->fn_call.parameters;
      if (member_parameter->size > 1) {
        char *error_message =
            strdup(".pop() only supports one optional argument");
        return result_error_runtime(runtime_error_init(
            error_message, state->current_stmt_lines.start_line,
            state->current_stmt_lines.end_line));
      }
      if (member_parameter->size == 1) {
        struct result *val = vector_at(member_parameter, 0);
        struct result *ret = eval_expression(val->node, state, return_code);
        RETURN_RESULT_IF_ERROR(ret);
        struct object *index = ret->object;
        if (index->data_type != INT_VALUE) {
          char *error_message =
              strdup("The `pos` in .pop(pos) must be an integer");
          return result_error_runtime(runtime_error_init(
              error_message, state->current_stmt_lines.start_line,
              state->current_stmt_lines.end_line));
        }
        /* Positive index */
        if (index->int_value >= 0) {
          if (index->int_value >= array_obj->object->array_value->size) {
            char *error_message = strdup("Index out of bound in .pop(pos)");
            return result_error_runtime(runtime_error_init(
                error_message, state->current_stmt_lines.start_line,
                state->current_stmt_lines.end_line));
          }
          returner = vector_remove_at(array_obj->object->array_value,
                                      index->int_value);
        } else {
          /* Negative index */
          long index_calc =
              array_obj->object->array_value->size + index->int_value;
          if (index_calc < 0) {
            char *error_message = strdup("Index out of bound in .pop(pos)");
            return result_error_runtime(runtime_error_init(
                error_message, state->current_stmt_lines.start_line,
                state->current_stmt_lines.end_line));
          }
          returner =
              vector_remove_at(array_obj->object->array_value, index_calc);
        }
      } else {
        /* the `pos` in .pop(pos) is optional. If `pos` is not given, we
         * remove the last item */
        returner = vector_remove_at(array_obj->object->array_value,
                                    array_obj->object->array_value->size - 1);
      }
    } else {
      char *error_message =
          format_string("Invalid method '%s' for array operation",
                        array_method_call_primary->node->id);
      return result_error_runtime(runtime_error_init(
          error_message, state->current_stmt_lines.start_line,
          state->current_stmt_lines.end_line));
    }
  } else {
    char *error_message =
        strdup("Method calls are only supported for arrays for now");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  return result_ok_object(returner);
}

struct result *
eval_array_creation_primary_expression(struct ast_node *ast,
                                       struct interpreter_state *state,
                                       struct return_value *return_code) {
  struct object *array_obj = malloc(sizeof(struct object));
  array_obj->data_type = ARRAY_VALUE;
  array_obj->array_value = vector_init();
  for (size_t i = 0; i < ast->array->size; i++) {
    struct result *val = vector_at(ast->array, i);
    struct result *ret = eval_expression(val->node, state, return_code);
    RETURN_RESULT_IF_ERROR(ret);
    vector_push_back(array_obj->array_value, ret->object);
  }
  return result_ok_object(array_obj);
}

struct result *
eval_array_access_primary_expression(struct ast_node *ast,
                                     struct interpreter_state *state,
                                     struct return_value *return_code) {
  struct result *primary_eval = eval_primary_expression(
      ast->array_access.primary->node, state, return_code);
  RETURN_RESULT_IF_ERROR(primary_eval);
  struct object *array_obj = primary_eval->object;
  if (array_obj->data_type != ARRAY_VALUE) {
    char *error_message = strdup("Array access can only be used for arrays");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  struct result *index_eval =
      eval_expression(ast->array_access.index->node, state, return_code);
  RETURN_RESULT_IF_ERROR(index_eval);
  struct object *array_index = index_eval->object;
  if (array_index->data_type != INT_VALUE) {
    char *error_message = strdup("Array index must be an integer");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  if (array_index->int_value >= array_obj->array_value->size) {
    char *error_message = strdup("Index out of bound");
    return result_error_runtime(
        runtime_error_init(error_message, state->current_stmt_lines.start_line,
                           state->current_stmt_lines.end_line));
  }
  return result_ok_object(
      vector_at(array_obj->array_value, array_index->int_value));
}

struct environment *environment_init() {
  struct environment *env = malloc(sizeof(struct environment));
  env->symbols = hash_table_init();
  env->parent_environment = NULL;
  return env;
}

struct environment *
environment_init_enclosed(struct environment *enclosed_env) {
  struct environment *env = malloc(sizeof(struct environment));
  env->symbols = hash_table_init();
  env->parent_environment = enclosed_env;
  return env;
}

struct object *environment_lookup_symbol(struct environment *env, char *key) {
  struct object *cur_env_value = hash_table_lookup(env->symbols, key);
  if (cur_env_value != NULL) {
    return cur_env_value;
  }
  if (env->parent_environment != NULL) {
    return environment_lookup_symbol(env->parent_environment, key);
  }
  return NULL;
}

struct object *environment_lookup_symbol_current_env(struct environment *env,
                                                     char *key) {
  return hash_table_lookup(env->symbols, key);
}

void environment_insert_symbol(struct environment *env, char *key,
                               struct object *value) {
  hash_table_insert(env->symbols, key, value);
}

void environment_reassign_symbol(struct environment *env, char *key,
                                 struct object *value) {
  struct object *scope = hash_table_lookup(env->symbols, key);
  if (scope != NULL) {
    hash_table_update(env->symbols, key, value);
  } else {
    if (env->parent_environment != NULL) {
      environment_reassign_symbol(env->parent_environment, key, value);
    }
  }
}

void environment_free(struct environment *env) {
  if (env->parent_environment != NULL) {
    environment_free(env->parent_environment);
  }
  hash_table_free(env->symbols);
  free(env);
}
