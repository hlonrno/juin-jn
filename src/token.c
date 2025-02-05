#pragma once
#include "vector.c"

typedef enum {
  KEYWORD,
  TYPE,
  IDENTIFIER,
  OPERATOR,
  STRING_LITERAL,
  CHAR_LITERAL,
  INT_LITERAL,
  FLOAT_LITERAL,
} TokenType;

typedef struct {
  size_t line, begin, end;
  TokenType type;
  Vector value;
} Token;

void tokenInit(Token *const restrict token) {
  vectorInit(&token->value, 16, sizeof(char));
}

void tokenDeinit(Token *const restrict token) {
  vectorDeinit(&token->value);
}
