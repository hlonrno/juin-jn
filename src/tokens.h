#pragma once

typedef enum {
  KEYWORD,
  IDENTIFIER,
  STR_LITERAL,
  NUM_LITERAL,
  CHR_LITERAL,
  SPECIAL_CHAR
} token_type_t;

typedef union {
  unsigned long long number;
  void *arr;
  char chr;
} token_value_t;

typedef struct {
  token_type_t type;
  token_value_t value;
} token_t;

token_t *token_init(token_type_t type, token_value_t value);
void token_destroy(token_t *token);

