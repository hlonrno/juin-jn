#pragma once

#include "vec.h"

typedef enum {
  KEYWORD,
  IDENTIFIER,
  OPERATOR,
  STR_LITERAL,
  CHR_LITERAL,
  INT_LITERAL,
  FLT_LITERAL,
} TokenType;

typedef struct {
  TokenType type;
  union {
    double flt;
    long long num;
    Vector *arr;
    char *str;
  } value;
} Token;
