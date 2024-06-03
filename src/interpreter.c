#include "interpreter.h"
#include "ast.h"
#include "builtin_functions.h"
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
    interpret_statement(vector_at(program, i), &state, return_code);
  }
  return return_code->value;
}

void interpret_statement(struct ast_node *stmt_node,
                         struct interpreter_state *state,
                         struct return_value *return_code) {
  switch (stmt_node->node_type) {
  case FN_DEF_STMT: {
    interpret_fn_def_statement(stmt_node, state);
    break;
  }
  case EXPR_STMT: {
    interpret_expr_statement(stmt_node, state, return_code);
    break;
  }
  case RETURN_STMT: {
    interpret_return_statement(stmt_node, state, return_code);
    break;
  }
  case VARIABLE_DECL_STMT: {
    interpret_variable_decl_statement(stmt_node, state, return_code);
    break;
  }
  case VARIABLE_ASSIGN_STMT: {
    interpret_variable_assignment_statement(stmt_node, state, return_code);
    break;
  }
  case BLOCK_STMT: {
    interpret_block_statement(stmt_node, state, return_code);
    break;
  }
  case IF_STMT: {
    interpret_if_statement(stmt_node, state, return_code);
    break;
  }
  case WHILE_STMT: {
    interpret_while_statement(stmt_node, state, return_code);
    break;
  }
  case FOR_STMT: {
    interpret_for_statement(stmt_node, state, return_code);
    break;
  }
  case BREAK_STMT: {
    interpret_break_statement(stmt_node, state, return_code);
    break;
  }
  default: {
    printf("Invalid statement\n");
    exit(1);
  }
  }
}

void interpret_fn_def_statement(struct ast_node *stmt_node,
                                struct interpreter_state *state) {
  /* Functions are local to environment. Child environments have access to
   * parent environment, but not vice versa. Works much like variable
   * declaration statements. */
  if (environment_lookup_function_current_env(
          state->env, stmt_node->fn_def_stmt_id->identifier_value)) {
    printf("Function '%s' already exists in current scope.\n",
           stmt_node->fn_def_stmt_id->identifier_value);
    exit(1);
  }
  struct function *fn_stmt = malloc(sizeof(struct function));
  fn_stmt->body = stmt_node->fn_def_stmt_block;
  fn_stmt->parameters = stmt_node->fn_def_stmt_parameters;
  environment_insert_function(
      state->env, stmt_node->fn_def_stmt_id->identifier_value, fn_stmt);
}

void interpret_expr_statement(struct ast_node *stmt_node,
                              struct interpreter_state *state,
                              struct return_value *return_code) {
  eval_expression(stmt_node->expr_stmt_expr, state, return_code);
}

void interpret_return_statement(struct ast_node *stmt_node,
                                struct interpreter_state *state,
                                struct return_value *return_code) {
  struct object *return_expr =
      eval_expression(stmt_node->return_stmt_expr, state, return_code);
  return_code->is_set = true;
  return_code->value = return_expr;
}

void interpret_variable_decl_statement(struct ast_node *stmt_node,
                                       struct interpreter_state *state,
                                       struct return_value *return_code) {
  /* Allow creating scope-local variable name of same name in a scope even if it
   * exists in previous scopes. */
  if (environment_lookup_variable_current_env(
          state->env, stmt_node->var_decl_stmt_id->identifier_value)) {
    printf("Variable '%s' already exists in current scope.\n",
           stmt_node->var_decl_stmt_id->identifier_value);
    exit(1);
  }
  struct object *variable_value =
      eval_expression(stmt_node->var_decl_stmt_expr, state, return_code);
  environment_insert_variable(state->env,
                              stmt_node->var_decl_stmt_id->identifier_value,
                              variable_value);
}

void interpret_variable_assignment_statement(struct ast_node *stmt_node,
                                             struct interpreter_state *state,
                                             struct return_value *return_code) {
  /* Check current scope, if not traverse to previous parent scope. */
  if (environment_lookup_variable(
          state->env, stmt_node->assign_stmt_id->identifier_value) == NULL) {
    printf("Variable '%s' does not exist\n",
           stmt_node->assign_stmt_id->identifier_value);
    exit(1);
  }
  struct object *variable_value =
      eval_expression(stmt_node->assign_stmt_expr, state, return_code);
  environment_update_variable(
      state->env, stmt_node->assign_stmt_id->identifier_value, variable_value);
}

void interpret_if_statement(struct ast_node *stmt_node,
                            struct interpreter_state *state,
                            struct return_value *return_code) {
  struct object *if_expr =
      eval_expression(stmt_node->if_stmt_expr, state, return_code);
  if (if_expr->data_type != BOOLEAN_VALUE) {
    printf("The result of the <expression> inside 'if' statement should result "
           "in a boolean value.\n");
    exit(1);
  }
  if (if_expr->bool_value) {
    interpret_block_statement(stmt_node->if_stmt_block, state, return_code);
  } else {
    if (stmt_node->if_else_stmt_block != NULL) {
      interpret_block_statement(stmt_node->if_else_stmt_block, state,
                                return_code);
    }
  }
}

void interpret_while_statement(struct ast_node *stmt_node,
                               struct interpreter_state *state,
                               struct return_value *return_code) {
  struct object *while_expr =
      eval_expression(stmt_node->while_stmt_expr, state, return_code);
  if (while_expr->data_type != BOOLEAN_VALUE) {
    printf("The <expression> of 'while' must return boolean.\n");
    exit(1);
  }
  while (while_expr->bool_value) {
    if (state->is_break) {
      state->is_break = false;
      break;
    }
    interpret_block_statement(stmt_node->while_stmt_block, state, return_code);
    while_expr =
        eval_expression(stmt_node->while_stmt_expr, state, return_code);
  }
}

void interpret_for_statement(struct ast_node *stmt_node,
                             struct interpreter_state *state,
                             struct return_value *return_code) {
  struct environment *parent_env = state->env;
  struct environment *block_env = environment_init_enclosed(state->env);
  state->env = block_env;
  interpret_variable_decl_statement(stmt_node->for_stmt_init_stmt, state,
                                    return_code);
  struct object *for_expr = eval_expression(
      stmt_node->for_stmt_expr->expr_stmt_expr, state, return_code);
  if (for_expr->data_type != BOOLEAN_VALUE) {
    printf("The expression of 'for' loop must result in a boolean value.\n");
    exit(1);
  }
  while (for_expr->bool_value) {
    if (state->is_break) {
      state->is_break = false;
      break;
    }
    interpret_block_statement(stmt_node->for_stmt_block, state, return_code);
    interpret_variable_assignment_statement(stmt_node->for_stmt_update_stmt,
                                            state, return_code);
    for_expr = eval_expression(stmt_node->for_stmt_expr->expr_stmt_expr, state,
                               return_code);
  }
}

void interpret_break_statement(struct ast_node *stmt_node,
                               struct interpreter_state *state,
                               struct return_value *return_code) {
  state->is_break = true;
}

void interpret_block_statement(struct ast_node *stmt_node,
                               struct interpreter_state *state,
                               struct return_value *return_code) {
  struct environment *parent_env = state->env;
  struct environment *block_env = environment_init_enclosed(state->env);
  state->env = block_env;
  for (size_t i = 0; i < stmt_node->block_stmt_stmts->size; i++) {
    if (return_code->is_set || state->is_break) {
      break;
    }
    interpret_statement(vector_at(stmt_node->block_stmt_stmts, i), state,
                        return_code);
  }
  state->env = parent_env;
}

struct object *eval_expression(struct ast_node *ast,
                               struct interpreter_state *state,
                               struct return_value *return_code) {
  struct object *returner = NULL;
  switch (ast->node_type) {
  case FN_CALL_NODE: {
    struct function *fn = environment_lookup_function(
        state->env, ast->fn_call_identifier->identifier_value);
    /* Check if function is defined. */
    if (!fn) {
      struct builtin_fn *builtin_fn_ = lookup_builtin_fns(
          state->builtin_fns, ast->fn_call_identifier->identifier_value);
      /* Check if it is a bultin function. */
      if (!builtin_fn_) {
        printf("Function name '%s' is not defined",
               ast->fn_call_identifier->identifier_value);
        exit(1);
      }
      /* Check builtin function's arity. */
      if (ast->fn_call_parameters->size != builtin_fn_->num_parameters) {
        printf(
            "Function call to '%s' with arity %ld does not match arity of %ld",
            ast->fn_call_identifier->identifier_value,
            ast->fn_call_parameters->size, builtin_fn_->num_parameters);
        exit(1);
      }
      /* Invoke the builtin function based on their arity. */
      if (builtin_fn_->num_parameters == 1) {
        void *(*fn_ptr)(void *) = builtin_fn_->fn_ptr;
        fn_ptr(eval_expression(vector_at(ast->fn_call_parameters, 0), state,
                               return_code));
      } else if (builtin_fn_->num_parameters == 2) {
        void *(*fn_ptr)(void *, void *) = builtin_fn_->fn_ptr;
        fn_ptr(eval_expression(vector_at(ast->fn_call_parameters, 0), state,
                               return_code),
               eval_expression(vector_at(ast->fn_call_parameters, 1), state,
                               return_code));
      } else {
        printf("Unsupported number of parameters for builtin functions.\n");
        exit(1);
      }
      return_code->is_set = false;
      return returner;
    }
    /* Check function arity. */
    if (fn->parameters->size != ast->fn_call_parameters->size) {
      printf("Function call to '%s' with arity %ld does not match arity of %ld",
             ast->fn_call_identifier->identifier_value,
             ast->fn_call_parameters->size, fn->parameters->size);
      exit(1);
    }
    /* Check function parameter types with types passed in and put it in the
     * environment. */
    struct environment *parent_env = state->env;
    struct environment *fn_call_env = environment_init_enclosed(state->env);
    for (size_t i = 0; i < ast->fn_call_parameters->size; i++) {
      struct object *parameter_eval = eval_expression(
          vector_at(ast->fn_call_parameters, i), state, return_code);
      char *fn_parameter = vector_at(fn->parameters, i);
      environment_insert_variable(fn_call_env, fn_parameter, parameter_eval);
    }
    /* Pass the parameters as environment to block statement and execute
     * statement blocks of the function. */
    state->env = fn_call_env;
    interpret_block_statement(fn->body, state, return_code);
    if (return_code->is_set) {
      if (ast->unary_op == BANG) {
        return_code->value->bool_value = !return_code->value->bool_value;
      } else if (ast->unary_op == MINUS) {
        return_code->value->int_value = -return_code->value->int_value;
      }
    }
    returner = return_code->value;
    return_code->is_set = false;
    state->env = parent_env;
    return returner;
  }
  case BINARY_NODE: {
    struct object *lhs = eval_expression(ast->left, state, return_code);
    struct object *rhs = eval_expression(ast->right, state, return_code);
    switch (ast->op) {
    case PLUS:
    case MINUS:
    case STAR:
    case SLASH: {
      returner = eval_additive_multiplicative_expression(ast->op, lhs, rhs);
      break;
    }
    case OR:
    case AND: {
      returner = eval_logical_expression(ast->op, lhs, rhs);
      break;
    }
    case EQUAL_EQUAL:
    case BANG_EQUAL: {
      returner = eval_equality_expression(ast->op, lhs, rhs);
      break;
    }
    case GREATER:
    case GREATER_EQUAL:
    case LESS:
    case LESS_EQUAL: {
      returner = eval_comparitive_expression(ast->op, lhs, rhs);
      break;
    }
    default: {
      printf("Invalid operation '%s' in binary node\n",
             get_string_from_token_atom(ast->op));
      exit(1);
    }
    }
    break;
  }
  case PRIMARY_NODE: {
    returner = eval_primary_expression(ast, state);
    break;
  }
  default: {
    printf("Invalid expression type in eval_expression\n");
    exit(1);
  }
  }
  return returner;
}

struct object *eval_logical_expression(enum token_type op, struct object *lhs,
                                       struct object *rhs) {
  struct object *returner = malloc(sizeof(struct object));
  returner->bool_value = (op == AND) ? (lhs->bool_value && rhs->bool_value)
                                     : (lhs->bool_value || rhs->bool_value);
  return returner;
}

struct object *eval_equality_expression(enum token_type op, struct object *lhs,
                                        struct object *rhs) {
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
    printf("Equality operator can only be performed between two operands of "
           "integers, strings, or booleans.\n");
    exit(1);
  }
  return returner;
}

struct object *eval_comparitive_expression(enum token_type op,
                                           struct object *lhs,
                                           struct object *rhs) {
  struct object *returner = malloc(sizeof(struct object));
  returner->data_type = BOOLEAN_VALUE;
  if (lhs->data_type != INT_VALUE || rhs->data_type != INT_VALUE) {
    printf(
        "Comparitive expression can only be performed between two integers.\n");
    exit(1);
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
  return returner;
}

struct object *eval_additive_multiplicative_expression(enum token_type op,
                                                       struct object *lhs,
                                                       struct object *rhs) {
  struct object *returner = malloc(sizeof(struct object));
  returner->data_type = INT_VALUE;
  if (lhs->data_type != INT_VALUE || rhs->data_type != INT_VALUE) {
    printf("For additive and multiplicative expressions, both operands must be "
           "of integer type.\n");
    exit(1);
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
    /* The `op' check is already performed by `eval_expression' parent function.
     */
  }
  }
  return returner;
}

struct object *eval_primary_expression(struct ast_node *ast,
                                       struct interpreter_state *state) {
  struct object *returner = malloc(sizeof(struct object));
  switch (ast->primary_node_type) {
  case NUMBER_PRIMARY_NODE: {
    returner->data_type = INT_VALUE;
    returner->int_value = ast->number_value;
    break;
  }
  case STRING_PRIMARY_NODE: {
    returner->data_type = STRING_VALUE;
    returner->string_value = ast->string_value;
    break;
  }
  case BOOLEAN_PRIMARY_NODE: {
    returner->data_type = BOOLEAN_VALUE;
    returner->bool_value = ast->boolean_value;
    break;
  }
  case IDENTIFIER_PRIMARY_NODE: {
    struct object *identifier_lookup =
        environment_lookup_variable(state->env, ast->identifier_value);
    if (!identifier_lookup) {
      printf("Identifier '%s' does not exist\n", ast->identifier_value);
      exit(1);
    }
    returner->data_type = identifier_lookup->data_type;
    switch (identifier_lookup->data_type) {
    case INT_VALUE: {
      returner->int_value = identifier_lookup->int_value;
      break;
    }
    case STRING_VALUE: {
      returner->string_value = identifier_lookup->string_value;
      break;
    }
    case BOOLEAN_VALUE: {
      returner->bool_value = identifier_lookup->bool_value;
      break;
    }
    default: {
      printf("Unsupported data type on identifier lookup\n");
    }
    }
    break;
  }
  case NIL_PRIMARY_NODE: {
    returner->data_type = NIL_VALUE;
    break;
  }
  }
  return returner;
}

struct environment *environment_init() {
  struct environment *env = malloc(sizeof(struct environment));
  env->current_env_variables = hash_table_init();
  env->current_env_functions = hash_table_init();
  env->enclosing_environment = NULL;
  return env;
}

struct environment *
environment_init_enclosed(struct environment *enclosed_env) {
  struct environment *env = malloc(sizeof(struct environment));
  env->current_env_variables = hash_table_init();
  env->current_env_functions = hash_table_init();
  env->enclosing_environment = enclosed_env;
  return env;
}

struct object *environment_lookup_variable(struct environment *env, char *key) {
  struct object *cur_env_value =
      hash_table_lookup(env->current_env_variables, key);
  if (cur_env_value != NULL) {
    return cur_env_value;
  }
  if (env->enclosing_environment != NULL) {
    return environment_lookup_variable(env->enclosing_environment, key);
  }
  return NULL;
}

struct object *environment_lookup_variable_current_env(struct environment *env,
                                                       char *key) {
  return hash_table_lookup(env->current_env_variables, key);
}

void environment_insert_variable(struct environment *env, char *key,
                                 struct object *value) {
  hash_table_insert(env->current_env_variables, key, value);
}

/* Check if the variable exists in the current scope environment. If it is,
 * update it. If it is not, recursively check the parent environment until we
 * reach the global scope. */
void environment_update_variable(struct environment *env, char *key,
                                 struct object *value) {
  struct object *scope = hash_table_lookup(env->current_env_variables, key);
  if (scope != NULL) {
    hash_table_update(env->current_env_variables, key, value);
  } else {
    if (env->enclosing_environment != NULL) {
      environment_update_variable(env->enclosing_environment, key, value);
    }
  }
}

struct function *environment_lookup_function(struct environment *env,
                                             char *key) {
  struct function *cur_env_value =
      hash_table_lookup(env->current_env_functions, key);
  if (cur_env_value != NULL) {
    return cur_env_value;
  }
  if (env->enclosing_environment != NULL) {
    return environment_lookup_function(env->enclosing_environment, key);
  }
  return NULL;
}

struct function *
environment_lookup_function_current_env(struct environment *env, char *key) {
  return hash_table_lookup(env->current_env_functions, key);
}

void environment_insert_function(struct environment *env, char *key,
                                 struct function *value) {
  hash_table_insert(env->current_env_functions, key, value);
}

void environment_free(struct environment *env) {
  hash_table_free(env->current_env_variables);
  free(env);
}
