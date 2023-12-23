#include "interpreter.h"
#include "ast.h"
#include "hash_table.h"
#include "tokens.h"
#include "utils.h"
#include "vector.h"

object *interpret(vector *program) {
  if (!program) {
    return NULL;
  }
  interpreter_state state = {.env = environment_init()};
  object *return_code = malloc(sizeof(object));
  for (size_t i = 0; i < program->size; i++) {
    interpret_statement(vector_at(program, i), &state, return_code);
  }
  object *xx = environment_lookup(state.env, "bar");
  printf("Ret: %li\n", xx->int_value);
  return return_code;
}

void interpret_statement(ast_node *stmt_node, interpreter_state *state,
                         object *return_code) {
  switch (stmt_node->node_type) {
  case VARIABLE_DECL_STMT: {
    interpret_variable_decl_statement(stmt_node, state);
    break;
  }
  case VARIABLE_ASSIGN_STMT: {
    interpret_variable_assignment_statement(stmt_node, state);
    break;
  }
  case BLOCK_STMT: {
    interpret_block_statement(stmt_node, state, return_code);
    break;
  }
  default: {
    printf("Invalid statement\n");
    exit(1);
  }
  }
}

void interpret_variable_decl_statement(ast_node *stmt_node,
                                       interpreter_state *state) {
  /* Allow creating scope-local variable name of same name in a scope even if it
   * exists in previous scopes. */
  if (environment_lookup_current_env(
          state->env, stmt_node->var_decl_stmt_id->identifier_value)) {
    printf("Variable '%s' already exists\n",
           stmt_node->var_decl_stmt_id->identifier_value);
    exit(1);
  }
  object *variable_value =
      eval_expression(stmt_node->var_decl_stmt_expr, state);
  environment_insert(state->env, stmt_node->var_decl_stmt_id->identifier_value,
                     variable_value);
}

void interpret_variable_assignment_statement(ast_node *stmt_node,
                                             interpreter_state *state) {
  /* Check current scope, if not traverse to previous parent scope. */
  if (environment_lookup(state->env,
                         stmt_node->assign_stmt_id->identifier_value) == NULL) {
    printf("Variable '%s' does not exist\n",
           stmt_node->assign_stmt_id->identifier_value);
    exit(1);
  }
  object *variable_value = eval_expression(stmt_node->assign_stmt_expr, state);
  environment_update(state->env, stmt_node->assign_stmt_id->identifier_value,
                     variable_value);
}

void interpret_block_statement(ast_node *stmt_node, interpreter_state *state,
                               object *return_code) {
  environment *parent_env = state->env;
  environment *block_env = environment_init_enclosed(state->env);
  state->env = block_env;
  for (size_t i = 0; i < stmt_node->block_stmt_stmts->size; i++) {
    interpret_statement(vector_at(stmt_node->block_stmt_stmts, i), state,
                        return_code);
  }
  environment_free(state->env);
  state->env = parent_env;
}

object *eval_expression(ast_node *ast, interpreter_state *state) {
  object *returner = malloc(sizeof(object));
  switch (ast->node_type) {
  case BINARY_NODE: {
    object *lhs = eval_expression(ast->left, state);
    object *rhs = eval_expression(ast->right, state);
    switch (ast->op) {
    case PLUS: {
      if (lhs->data_type == INT_DATATYPE && rhs->data_type == INT_DATATYPE) {
        returner->data_type = INT_DATATYPE;
        returner->int_value = lhs->int_value + rhs->int_value;
      } else {
        printf("PLUS operator not defined for 'string' and 'bool', or mix of "
               "data types");
      }
      break;
    }
    case MINUS: {
      if (lhs->data_type == INT_DATATYPE && rhs->data_type == INT_DATATYPE) {
        returner->data_type = INT_DATATYPE;
        returner->int_value = lhs->int_value - rhs->int_value;
      } else {
        printf("MINUS operator not defined for 'string' and 'bool', or mix of "
               "data types");
      }
      break;
    }
    case STAR: {
      if (lhs->data_type == INT_DATATYPE && rhs->data_type == INT_DATATYPE) {
        returner->data_type = INT_DATATYPE;
        returner->int_value = lhs->int_value * rhs->int_value;
      } else {
        printf("STAR operator not defined for 'string' and 'bool', or mix of "
               "data types");
      }
      break;
    }
    case SLASH: {
      if (lhs->data_type == INT_DATATYPE && rhs->data_type == INT_DATATYPE) {
        returner->data_type = INT_DATATYPE;
        returner->int_value = lhs->int_value / rhs->int_value;
      } else {

        printf("SLASH operator not defined for 'string' and 'bool', or mix of "
               "data types");
      }
      break;
    }
    default:
      printf("Invalid operation '%s' in binary node\n",
             get_string_from_token_atom(ast->op));
      exit(1);
    }
    break;
  }
  case PRIMARY_NODE: {
    switch (ast->primary_node_type) {
    case NUMBER_PRIMARY_NODE: {
      returner->data_type = INT_DATATYPE;
      returner->int_value = ast->number_value;
      break;
    }
    case STRING_PRIMARY_NODE: {
      returner->data_type = STRING_DATATYPE;
      returner->string_value = ast->string_value;
      break;
    }
    case BOOLEAN_PRIMARY_NODE: {
      returner->data_type = BOOL_DATATYPE;
      returner->bool_value = ast->boolean_value;
      break;
    }
    case IDENTIFIER_PRIMARY_NODE: {
      object *identifier_lookup =
          environment_lookup(state->env, ast->identifier_value);
      if (!identifier_lookup) {
        printf("Identifier '%s' does not exist\n", ast->identifier_value);
        exit(1);
      }
      returner->data_type = identifier_lookup->data_type;
      switch (identifier_lookup->data_type) {
      case INT_DATATYPE: {
        returner->int_value = identifier_lookup->int_value;
        break;
      }
      case STRING_DATATYPE: {
        returner->string_value = identifier_lookup->string_value;
        break;
      }
      case BOOL_DATATYPE: {
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
      returner->data_type = NIL;
      break;
    }
    }
    break;
  }
  default: {
    printf("Invalid expression type in eval_expression\n");
    exit(1);
  }
  }
  return returner;
}

environment *environment_init() {

  environment *env = malloc(sizeof(environment));
  env->current_env_variables = hash_table_init();
  env->enclosing_environment = NULL;
  return env;
}

environment *environment_init_enclosed(environment *enclosed_env) {
  environment *env = malloc(sizeof(environment));
  env->current_env_variables = hash_table_init();
  env->enclosing_environment = enclosed_env;
  return env;
}

object *environment_lookup(environment *env, char *key) {
  object *cur_env_value = hash_table_lookup(env->current_env_variables, key);
  if (cur_env_value != NULL) {
    return cur_env_value;
  }
  if (env->enclosing_environment != NULL) {
    return environment_lookup(env->enclosing_environment, key);
  }
  return NULL;
}

object *environment_lookup_current_env(environment *env, char *key) {
  return hash_table_lookup(env->current_env_variables, key);
}

void environment_insert(environment *env, char *key, object *value) {
  hash_table_insert(env->current_env_variables, key, value);
}

/* Check if the variable exists in the current scope environment. If it is,
 * update it. If it is not, recursively check the parent environment until we
 * reach the global scope. */
void environment_update(environment *env, char *key, object *value) {
  object *scope = hash_table_lookup(env->current_env_variables, key);
  if (scope != NULL) {
    hash_table_update(env->current_env_variables, key, value);
  } else {
    if (env->enclosing_environment != NULL) {
      environment_update(env->enclosing_environment, key, value);
    }
  }
}

void environment_free(environment *env) {
  hash_table_free(env->current_env_variables);
  free(env);
}
