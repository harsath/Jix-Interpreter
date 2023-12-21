#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,
  BANG,
  EQUAL,
  EQUAL_EQUAL,
  BANG_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,
  AND,
  OR,
  IDENTIFIER,
  STRING,
  NUMBER,
  FUNCTION,
  IF,
  ELSE,
  TRUE,
  FALSE,
  NIL,
  VAR,
  RETURN,
  WHILE,
  FOR,
  STRING_DATATYPE,
  INT_DATATYPE,
  BOOL_DATATYPE,
  EOF_TOKEN,
  INVALID_TOKEN
} token_type;

typedef struct {
  token_type type;
  const char *token_char;
  size_t token_char_len;
} token;

token_type get_token_atom_from_string(const char *key);
token_type get_keyword_token_from_string(const char *key);
const char *get_string_from_token_atom(token_type type);
token *create_token(token_type type, const char *token_char,
                    size_t token_char_len);

#endif
