#include "interpreter.h"
#include "ast.h"
#include "tokens.h"
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
  object *xx = hash_table_lookup(state.variables, "xxx");
  return return_code;
}

void interpret_statement(ast_node *stmt_node, interpreter_state *state,
                         object *return_code) {
  switch (stmt_node->node_type) {
  case VARIABLE_DECL_STMT: {
    char variable_key[100] = {0};
    ast_node *identifier = stmt_node->var_decl_stmt_id;
    memcpy(variable_key, identifier->identifier_value->token_char,
           identifier->identifier_value->token_char_len);
    if (hash_table_lookup(state->variables, variable_key)) {
      printf("Variable '%s' already exists\n", variable_key);
      exit(1);
    }
    object *variable_value =
        eval_expression(stmt_node->var_decl_stmt_expr, state);
    hash_table_insert(state->variables, variable_key, variable_value);
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
      printf("String primary node unimplemented\n");
      exit(1);
    }
    case BOOLEAN_PRIMARY_NODE: {
      printf("Boolean primary node unimplemented\n");
      exit(1);
    }
    case IDENTIFIER_PRIMARY_NODE: {
      printf("Identifier primary node unimplemented\n");
      exit(1);
    }
    case NIL_PRIMARY_NODE: {
      printf("Nil primary node unimplemented\n");
      exit(1);
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
