#include "scanner.h"
#include "tokens.h"
#include "utils.h"

#define INCREMENT_INDEXES()                                                    \
  do {                                                                         \
    current_index++;                                                           \
    current_line_start_index++;                                                \
  } while (0)

#define INCREMENT_CURRENT_LINE_RESET_START_INDEX()                             \
  do {                                                                         \
    current_line++;                                                            \
    current_line_start_index = 0;                                              \
  } while (0)

struct vector *scan_tokens(const char *source_code) {
  if (!source_code) {
    return NULL;
  }
  struct vector *tokens = vector_init();
  size_t current_index = 0;
  size_t current_line = 1;
  size_t start_index = 0;
  size_t current_line_start_index = 0;
  size_t source_code_len = strlen(source_code);
  while (current_index != source_code_len) {
    if (isdigit(source_code[current_index])) {
      INCREMENT_INDEXES();
      while (isdigit(source_code[current_index])) {
        INCREMENT_INDEXES();
      }
      struct token *token_ = create_token(
          NUMBER, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '(') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          LEFT_PAREN, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == ')') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          RIGHT_PAREN, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '{') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          LEFT_BRACE, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '}') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          RIGHT_BRACE, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '[') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          LEFT_BRACKET, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == ']') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          RIGHT_BRACKET, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == ',') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          COMMA, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '.') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          DOT, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '-') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          MINUS, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '+') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          PLUS, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == ';') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          SEMICOLON, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '/') {
      INCREMENT_INDEXES();
      if (source_code[current_index] == '/') {
        INCREMENT_INDEXES();
        while (source_code[current_index] != '\n') {
          INCREMENT_INDEXES();
        }
        INCREMENT_CURRENT_LINE_RESET_START_INDEX();
      } else {
        struct token *token_ = create_token(
            SLASH, current_line_start_index, (source_code + start_index),
            (current_index - start_index), current_line);
        vector_push_back(tokens, token_);
      }
    } else if (source_code[current_index] == '*') {
      INCREMENT_INDEXES();
      struct token *token_ = create_token(
          STAR, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '=') {
      INCREMENT_INDEXES();
      if (source_code[current_index] == '=') {
        INCREMENT_INDEXES();
        struct token *token_ = create_token(
            EQUAL_EQUAL, current_line_start_index, (source_code + start_index),
            (current_index - start_index), current_line);
        vector_push_back(tokens, token_);
      } else {
        struct token *token_ = create_token(
            EQUAL, current_line_start_index, (source_code + start_index),
            (current_index - start_index), current_line);
        vector_push_back(tokens, token_);
      }

    } else if (source_code[current_index] == '!') {
      INCREMENT_INDEXES();
      if (source_code[current_index] == '=') {
        INCREMENT_INDEXES();
        struct token *token_ = create_token(
            BANG_EQUAL, current_line_start_index, (source_code + start_index),
            (current_index - start_index), current_line);
        vector_push_back(tokens, token_);
      } else {
        struct token *token_ = create_token(
            BANG, current_line_start_index, (source_code + start_index),
            (current_index - start_index), current_line);
        vector_push_back(tokens, token_);
      }
    } else if (source_code[current_index] == '>') {
      INCREMENT_INDEXES();
      if (source_code[current_index] == '=') {
        INCREMENT_INDEXES();
        struct token *token_ =
            create_token(GREATER_EQUAL, current_line_start_index,
                         (source_code + start_index),
                         (current_index - start_index), current_line);
        vector_push_back(tokens, token_);
      } else {
        struct token *token_ = create_token(
            GREATER, current_line_start_index, (source_code + start_index),
            (current_index - start_index), current_line);
        vector_push_back(tokens, token_);
      }
    } else if (source_code[current_index] == '<') {
      INCREMENT_INDEXES();
      if (source_code[current_index] == '=') {
        INCREMENT_INDEXES();
        struct token *token_ = create_token(
            LESS_EQUAL, current_line_start_index, (source_code + start_index),
            (current_index - start_index), current_line);
        vector_push_back(tokens, token_);
      } else {
        struct token *token_ = create_token(
            LESS, current_line_start_index, (source_code + start_index),
            (current_index - start_index), current_line);
        vector_push_back(tokens, token_);
      }
    } else if (source_code[current_index] == '&') {
      INCREMENT_INDEXES();
      if (source_code[current_index] == '&') {
        INCREMENT_INDEXES();
        struct token *token_ = create_token(
            AND, current_line_start_index, (source_code + start_index),
            (current_index - start_index), current_line);
        vector_push_back(tokens, token_);
      } else {
        perror("Invalid token '&'\n");
        INCREMENT_INDEXES();
      }
    } else if (source_code[current_index] == '|') {
      INCREMENT_INDEXES();
      if (source_code[current_index] == '|') {
        INCREMENT_INDEXES();
        struct token *token_ = create_token(
            OR, current_line_start_index, (source_code + start_index),
            (current_index - start_index), current_line);
        vector_push_back(tokens, token_);
      } else {
        perror("Invalid token '|'\n");
        INCREMENT_INDEXES();
      }
    } else if (isalpha(source_code[current_index]) ||
               source_code[current_index] == '_') {
      INCREMENT_INDEXES();
      while (isalpha(source_code[current_index]) ||
             isdigit(source_code[current_index]) ||
             source_code[current_index] == '_') {
        INCREMENT_INDEXES();
      }
      char *iden_buffer =
          create_token_string_copy(source_code, start_index, current_index);
      enum token_type tok_type = get_keyword_token_from_string(iden_buffer);
      free(iden_buffer);
      struct token *token_ = create_token(
          tok_type, current_line_start_index, (source_code + start_index),
          (current_index - start_index), current_line);
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '"') {
      INCREMENT_INDEXES();
      while (source_code[current_index] != '"' &&
             current_index < source_code_len) {
        INCREMENT_INDEXES();
      }
      if (source_code[current_index] == '"') {
        INCREMENT_INDEXES();
        char *string_literal = create_token_string_copy(
            source_code, start_index + 1, current_index - 1);
        char *parsed_string = parse_escape_sequences(string_literal);
        free(string_literal);
        struct token *token_ =
            create_token(STRING, current_line_start_index, parsed_string,
                         strlen(parsed_string), current_line);
        vector_push_back(tokens, token_);
      } else {
        perror("Non-terminating string");
      }
    } else if (source_code[current_index] == ' ' ||
               source_code[current_index] == '\n' ||
               source_code[current_index] == '\t' ||
               source_code[current_index] == '\r') {
      if (source_code[current_index] == '\n') {
        INCREMENT_CURRENT_LINE_RESET_START_INDEX();
      }
      INCREMENT_INDEXES();
    }

    else {
      printf("Unsupported token '%c'\n", source_code[current_index]);
      INCREMENT_INDEXES();
    }
    start_index = current_index;
  }
  return tokens;
}

char *parse_escape_sequences(const char *input) {
  size_t len = strlen(input);
  char *result = malloc(len + 1);
  size_t j = 0;
  for (size_t i = 0; i < len; i++) {
    if (input[i] == '\\' && i + 1 < len) {
      switch (input[i + 1]) {
      case 'n':
        result[j++] = '\n';
        i++; // skipping the 'n'
        break;
      case 't':
        result[j++] = '\t';
        i++; // skipping the 't'
        break;
      case '\\':
        result[j++] = '\\';
        i++; // skipping the '\'
        break;
      default:
        result[j++] = input[i];
        break;
      }
    } else {
      result[j++] = input[i];
    }
  }
  result[j] = '\0';
  return result;
}
