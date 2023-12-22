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
  interpreter_state state = {.variables = hash_table_init()};
  object *return_code = malloc(sizeof(object));
  for (size_t i = 0; i < program->size; i++) {
    interpret_statement(vector_at(program, i), &state, return_code);
  }
  object *xx = hash_table_lookup(state.variables, "bar");
  printf("Ret: %li\n", xx->int_value);
  return return_code;
}

void interpret_statement(ast_node *stmt_node, interpreter_state *state,
                         object *return_code) {
  switch (stmt_node->node_type) {
  case VARIABLE_DECL_STMT: {
    if (hash_table_lookup(state->variables,
                          stmt_node->var_decl_stmt_id->identifier_value)) {
      printf("Variable '%s' already exists\n",
             stmt_node->var_decl_stmt_id->identifier_value);
      exit(1);
    }
    object *variable_value =
        eval_expression(stmt_node->var_decl_stmt_expr, state);
    hash_table_insert(state->variables,
                      stmt_node->var_decl_stmt_id->identifier_value,
                      variable_value);
    break;
  }
  case VARIABLE_ASSIGN_STMT: {
    if (hash_table_lookup(state->variables,
                          stmt_node->assign_stmt_id->identifier_value) ==
        NULL) {
      printf("Variable '%s' does not exist\n",
             stmt_node->assign_stmt_id->identifier_value);
      exit(1);
    }
    object *variable_value =
        eval_expression(stmt_node->assign_stmt_expr, state);
    hash_table_update(state->variables,
                      stmt_node->assign_stmt_id->identifier_value,
                      variable_value);
    break;
  }
  default: {
    printf("Invalid statement\n");
    exit(1);
  }
  }
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
          hash_table_lookup(state->variables, ast->identifier_value);
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
