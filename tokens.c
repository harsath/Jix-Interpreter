#include "tokens.h"

token_type get_token_atom_from_string(const char *key) {
  if (strcmp(key, "(") == 0) {
    return LEFT_PAREN;
  }
  if (strcmp(key, ")") == 0) {
    return RIGHT_PAREN;
  }
  if (strcmp(key, "{") == 0) {
    return LEFT_BRACE;
  }
  if (strcmp(key, "}") == 0) {
    return RIGHT_BRACE;
  }
  if (strcmp(key, ",") == 0) {
    return COMMA;
  }
  if (strcmp(key, ".") == 0) {
    return DOT;
  }
  if (strcmp(key, "-") == 0) {
    return MINUS;
  }
  if (strcmp(key, "+") == 0) {
    return PLUS;
  }
  if (strcmp(key, ";") == 0) {
    return SEMICOLON;
  }
  if (strcmp(key, "/") == 0) {
    return SLASH;
  }
  if (strcmp(key, "*") == 0) {
    return STAR;
  }
  if (strcmp(key, "!") == 0) {
    return BANG;
  }
  if (strcmp(key, "=") == 0) {
    return EQUAL;
  }
  if (strcmp(key, "==") == 0) {
    return EQUAL_EQUAL;
  }
  if (strcmp(key, "!=") == 0) {
    return BANG_EQUAL;
  }
  if (strcmp(key, ">") == 0) {
    return GREATER;
  }
  if (strcmp(key, ">=") == 0) {
    return GREATER_EQUAL;
  }
  if (strcmp(key, "<") == 0) {
    return LESS;
  }
  if (strcmp(key, "<=") == 0) {
    return LESS_EQUAL;
  }
  if (strcmp(key, "&&") == 0) {
    return AND;
  }
  if (strcmp(key, "||") == 0) {
    return OR;
  }
  if (strcmp(key, "function") == 0) {
    return FUNCTION;
  }
  if (strcmp(key, "if") == 0) {
    return IF;
  }
  if (strcmp(key, "else") == 0) {
    return ELSE;
  }
  if (strcmp(key, "true") == 0) {
    return TRUE;
  }
  if (strcmp(key, "false") == 0) {
    return FALSE;
  }
  if (strcmp(key, "nil") == 0) {
    return NIL;
  }
  if (strcmp(key, "var") == 0) {
    return VAR;
  }
  if (strcmp(key, "return") == 0) {
    return RETURN;
  }
  if (strcmp(key, "while") == 0) {
    return WHILE;
  }
  if (strcmp(key, "for") == 0) {
    return FOR;
  }
  if (strcmp(key, "int") == 0) {
    return INT_DATATYPE;
  }
  if (strcmp(key, "string") == 0) {
    return STRING_DATATYPE;
  }
  if (strcmp(key, "bool") == 0) {
    return BOOL_DATATYPE;
  }
  return INVALID_TOKEN;
}

token_type get_keyword_token_from_string(const char *key) {
  if (strcmp(key, "function") == 0) {
    return FUNCTION;
  }
  if (strcmp(key, "if") == 0) {
    return IF;
  }
  if (strcmp(key, "else") == 0) {
    return ELSE;
  }
  if (strcmp(key, "true") == 0) {
    return TRUE;
  }
  if (strcmp(key, "false") == 0) {
    return FALSE;
  }
  if (strcmp(key, "nil") == 0) {
    return NIL;
  }
  if (strcmp(key, "var") == 0) {
    return VAR;
  }
  if (strcmp(key, "return") == 0) {
    return RETURN;
  }
  if (strcmp(key, "while") == 0) {
    return WHILE;
  }
  if (strcmp(key, "for") == 0) {
    return FOR;
  }
  if (strcmp(key, "int") == 0) {
    return INT_DATATYPE;
  }
  if (strcmp(key, "string") == 0) {
    return STRING_DATATYPE;
  }
  if (strcmp(key, "bool") == 0) {
    return BOOL_DATATYPE;
  }
  return IDENTIFIER;
}

const char *get_string_from_token_atom(token_type type) {
  switch (type) {
  case LEFT_PAREN:
    return "(";
  case RIGHT_PAREN:
    return ")";
  case LEFT_BRACE:
    return "{";
  case RIGHT_BRACE:
    return "}";
  case COMMA:
    return ",";
  case DOT:
    return ".";
  case MINUS:
    return "-";
  case PLUS:
    return "+";
  case SEMICOLON:
    return ";";
  case SLASH:
    return "/";
  case STAR:
    return "*";
  case EQUAL:
    return "=";
  case EQUAL_EQUAL:
    return "==";
  case BANG:
    return "!";
  case BANG_EQUAL:
    return "!=";
  case GREATER:
    return ">";
  case GREATER_EQUAL:
    return ">=";
  case LESS:
    return "<";
  case LESS_EQUAL:
    return "<=";
  case AND:
    return "&&";
  case OR:
    return "||";
  case IDENTIFIER:
    return "<identifier>";
  case STRING:
    return "<string>";
  case NUMBER:
    return "<number>";
  case FUNCTION:
    return "function";
  case IF:
    return "if";
  case ELSE:
    return "else";
  case TRUE:
    return "true";
  case FALSE:
    return "false";
  case NIL:
    return "nil";
  case VAR:
    return "var";
  case RETURN:
    return "return";
  case WHILE:
    return "while";
  case FOR:
    return "for";
  case INT_DATATYPE:
    return "int";
  case STRING_DATATYPE:
    return "string";
  case BOOL_DATATYPE:
    return "bool";
  default:
    return "INVALID";
  }
}

token *create_token(token_type type, const char *token_char,
                    size_t token_char_len) {
  token *token_ = malloc(sizeof(token));
  if (!token_) {
    perror("Memory allocation error for token");
    return NULL;
  }
  token_->type = type;
  token_->token_char = token_char;
  token_->token_char_len = token_char_len;
  return token_;
}
