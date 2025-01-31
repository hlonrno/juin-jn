#pragma once

#include "vec.h"
#include <stdbool.h>
#include <stdio.h>
#define BUF_SIZE 16

typedef struct {
  FILE *fptr;
  char *error_message;
  Vector *tokens;
  size_t line_number;
  size_t index;
  char current_char;
  bool error;
  bool eof;
  char buf[BUF_SIZE];
} Lexer;

Lexer *tokenize(FILE *fptr);
