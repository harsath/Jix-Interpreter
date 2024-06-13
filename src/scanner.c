#include "scanner.h"
#include "tokens.h"
#include "utils.h"

struct vector *scan_tokens(const char *source_code) {
  if (!source_code) {
    return NULL;
  }
  struct vector *tokens = vector_init();
  size_t current_index = 0;
  size_t start_index = 0;
  size_t source_code_len = strlen(source_code);
  while (current_index != source_code_len) {
    if (isdigit(source_code[current_index])) {
      current_index++;
      while (isdigit(source_code[current_index])) {
        current_index++;
      }
      struct token *token_ = create_token(NUMBER, (source_code + start_index),
                                          (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '(') {
      current_index++;
      struct token *token_ =
          create_token(LEFT_PAREN, (source_code + start_index),
                       (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == ')') {
      current_index++;
      struct token *token_ =
          create_token(RIGHT_PAREN, (source_code + start_index),
                       (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '{') {
      current_index++;
      struct token *token_ =
          create_token(LEFT_BRACE, (source_code + start_index),
                       (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '}') {
      current_index++;
      struct token *token_ =
          create_token(RIGHT_BRACE, (source_code + start_index),
                       (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '[') {
      current_index++;
      struct token *token_ =
          create_token(LEFT_BRACKET, (source_code + start_index),
                       (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == ']') {
      current_index++;
      struct token *token_ =
          create_token(RIGHT_BRACKET, (source_code + start_index),
                       (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == ',') {
      current_index++;
      struct token *token_ = create_token(COMMA, (source_code + start_index),
                                          (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '.') {
      current_index++;
      struct token *token_ = create_token(DOT, (source_code + start_index),
                                          (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '-') {
      current_index++;

      if (isdigit(source_code[current_index])) {
        current_index++;
        while (isdigit(source_code[current_index])) {
          current_index++;
        }
        struct token *token_ = create_token(NUMBER, (source_code + start_index),
                                            (current_index - start_index));
        vector_push_back(tokens, token_);
      } else {
        struct token *token_ = create_token(MINUS, (source_code + start_index),
                                            (current_index - start_index));
        vector_push_back(tokens, token_);
      }
    } else if (source_code[current_index] == '+') {
      current_index++;
      struct token *token_ = create_token(PLUS, (source_code + start_index),
                                          (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == ';') {
      current_index++;
      struct token *token_ =
          create_token(SEMICOLON, (source_code + start_index),
                       (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '/') {
      current_index++;
      if (source_code[current_index] == '/') {
        current_index++;
        while (source_code[current_index] != '\n') {
          current_index++;
        }
      } else {
        struct token *token_ = create_token(SLASH, (source_code + start_index),
                                            (current_index - start_index));
        vector_push_back(tokens, token_);
      }
    } else if (source_code[current_index] == '*') {
      current_index++;
      struct token *token_ = create_token(STAR, (source_code + start_index),
                                          (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '=') {
      current_index++;
      if (source_code[current_index] == '=') {
        current_index++;
        struct token *token_ =
            create_token(EQUAL_EQUAL, (source_code + start_index),
                         (current_index - start_index));
        vector_push_back(tokens, token_);
      } else {
        struct token *token_ = create_token(EQUAL, (source_code + start_index),
                                            (current_index - start_index));
        vector_push_back(tokens, token_);
      }

    } else if (source_code[current_index] == '!') {
      current_index++;
      if (source_code[current_index] == '=') {
        current_index++;
        struct token *token_ =
            create_token(BANG_EQUAL, (source_code + start_index),
                         (current_index - start_index));
        vector_push_back(tokens, token_);
      } else {
        struct token *token_ = create_token(BANG, (source_code + start_index),
                                            (current_index - start_index));
        vector_push_back(tokens, token_);
      }
    } else if (source_code[current_index] == '>') {
      current_index++;
      if (source_code[current_index] == '=') {
        current_index++;
        struct token *token_ =
            create_token(GREATER_EQUAL, (source_code + start_index),
                         (current_index - start_index));
        vector_push_back(tokens, token_);
      } else {
        struct token *token_ =
            create_token(GREATER, (source_code + start_index),
                         (current_index - start_index));
        vector_push_back(tokens, token_);
      }
    } else if (source_code[current_index] == '<') {
      current_index++;
      if (source_code[current_index] == '=') {
        current_index++;
        struct token *token_ =
            create_token(LESS_EQUAL, (source_code + start_index),
                         (current_index - start_index));
        vector_push_back(tokens, token_);
      } else {
        struct token *token_ = create_token(LESS, (source_code + start_index),
                                            (current_index - start_index));
        vector_push_back(tokens, token_);
      }
    } else if (source_code[current_index] == '&') {
      current_index++;
      if (source_code[current_index] == '&') {
        current_index++;
        struct token *token_ = create_token(AND, (source_code + start_index),
                                            (current_index - start_index));
        vector_push_back(tokens, token_);
      } else {
        perror("Invalid token '&'\n");
        current_index++;
      }
    } else if (source_code[current_index] == '|') {
      current_index++;
      if (source_code[current_index] == '|') {
        current_index++;
        struct token *token_ = create_token(OR, (source_code + start_index),
                                            (current_index - start_index));
        vector_push_back(tokens, token_);
      } else {
        perror("Invalid token '|'\n");
        current_index++;
      }
    } else if (isalpha(source_code[current_index]) ||
               source_code[current_index] == '_') {
      current_index++;
      while (isalpha(source_code[current_index]) ||
             isdigit(source_code[current_index]) ||
             source_code[current_index] == '_') {
        current_index++;
      }
      char *iden_buffer =
          create_token_string_copy(source_code, start_index, current_index);
      enum token_type tok_type = get_keyword_token_from_string(iden_buffer);
      free(iden_buffer);
      struct token *token_ = create_token(tok_type, (source_code + start_index),
                                          (current_index - start_index));
      vector_push_back(tokens, token_);
    } else if (source_code[current_index] == '"') {
      current_index++;
      while (source_code[current_index] != '"' &&
             current_index < source_code_len) {
        current_index++;
      }
      if (source_code[current_index] == '"') {
        current_index++;
        char *string_literal = create_token_string_copy(source_code, start_index + 1, current_index - 1);
        char *parsed_string = parse_escape_sequences(string_literal);
        free(string_literal);
        struct token *token_ = create_token(STRING, parsed_string, strlen(parsed_string));
        vector_push_back(tokens, token_);
      } else {
        perror("Non-terminating string");
      }
    } else if (source_code[current_index] == ' ' ||
               source_code[current_index] == '\n' ||
               source_code[current_index] == '\t' ||
               source_code[current_index] == '\r') {
      current_index++;
    }

    else {
      printf("Unsupported token '%c'\n", source_code[current_index]);
      current_index++;
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
