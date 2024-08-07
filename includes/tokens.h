#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum token_type {
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  LEFT_BRACKET,
  RIGHT_BRACKET,
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
  LET,
  IF,
  ELSE,
  TRUE,
  FALSE,
  NIL,
  VAR,
  RETURN,
  WHILE,
  FOR,
  BREAK,
  EOF_TOKEN,
  INVALID_TOKEN
};

struct token {
  enum token_type type;
  const char *token_char;
  size_t token_char_len;
  size_t token_line;
};

enum token_type get_token_atom_from_string(const char *key);
enum token_type get_keyword_token_from_string(const char *key);
const char *get_string_from_token_atom(enum token_type type);
struct token *create_token(enum token_type type, const char *token_char,
                           size_t token_char_len, size_t token_line);
char *number_to_char(long number);

#endif
