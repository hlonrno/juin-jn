#pragma once
#include "token.c"
#include "vector.c"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define IS_NUMBER(n) (n > 47 && n < 58)
#define IS_SPECIAL_CHARACTER(n) (strchr("{}()[]:.,;=*/%+-", n) != NULL)
#define IS_IDENTIFIER_BEGIN(n) ((n > 96 && n < 123) || (n > 64 && n < 91) || n == '_' || n == '#')
#define IS_IDENTIFIER(n) (IS_IDENTIFIER_BEGIN(n) || IS_NUMBER(n))
#define DO_IGNORE(n) (strchr(" \n\t\b\r", n) != NULL)
#define BUF_SIZE 16 

static const char *const KEYWORDS[] = { "fn", "const", "var", "enum", "struct", "fnstruct" };
static const char *const OPERATORS[] = { "(", ")", "{", "}", ".", ",", "[", "]", ":", "=", "<", ">" };
static const char *const TYPES[] = { "Signed", "Unsigned", "Float", "Array" };
static const size_t KEYWORDS_SIZE = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);
static const size_t OPERATORS_SIZE = sizeof(OPERATORS) / sizeof(OPERATORS[0]);
static const size_t TYPES_SIZE = sizeof(TYPES) / sizeof(TYPES[0]);

typedef struct {
  FILE *fptr;
  char *error;
  size_t current_line;
  size_t index;
  char current_char;
  bool eof;
  char buf[BUF_SIZE];
} Lexer;

void advance(Lexer *const restrict stream, size_t ammount) {
  if (ammount > 1) {
    for (size_t i = 0; i < ammount; ++i)
      advance(stream, 1);
  } else {
    ++stream->index;
    if (stream->index == BUF_SIZE - 1 || stream->buf[stream->index - 1] == '\n') {
      if (!fgets(stream->buf, BUF_SIZE, stream->fptr))
        stream->eof = true;
      stream->index = 0;
    }
  }

  stream->current_char = stream->buf[stream->index];
  stream->current_line += stream->current_char == '\n';
}

// PERF: O(n), can be improved.
bool isIn(Token *const restrict token, const char *const array[], size_t length) {
  for (size_t i = 0; i < length; ++i)
    if (strcmp(array[i], token->value.data) == 0)
      return true;
  return false;
}

void getIdentifier(Lexer *const restrict stream, Token *const restrict token) {
  while (IS_IDENTIFIER(stream->current_char)) {
    vectorAdd(&token->value, &stream->current_char);
    advance(stream, 1);
  }

  token->type = IDENTIFIER;
  vectorTrim(&token->value);
}

void getOperator(Lexer *const restrict stream, Token *const restrict token) {
  do {
    vectorAdd(&token->value, &stream->current_char);
    if (isIn(token, OPERATORS, OPERATORS_SIZE))
      advance(stream, 1);
    else
      break;
  } while (true);

  token->type = OPERATOR;
  vectorPop(&token->value);
  vectorTrim(&token->value);
}

void getNumber(Lexer *const restrict stream, Token *const restrict token) {
  char dotcount = 0;
  while (IS_NUMBER(stream->current_char) || stream->current_char == '.') {
    if (stream->current_char == '.')
      if (++dotcount > 1)
        break;

    vectorAdd(&token->value, &stream->current_char);
    advance(stream, 1);
  }

  token->type = dotcount ? FLOAT_LITERAL : INT_LITERAL;
  vectorTrim(&token->value);
}

void getString(Lexer *const restrict stream, Token *const restrict token) {
  advance(stream, 1);
  while (stream->current_char != '"' && !stream->eof) {
    vectorAdd(&token->value, &stream->current_char);
    advance(stream, 1);
  }
  advance(stream, 1);

  token->type = STRING_LITERAL;
  vectorTrim(&token->value);
}

void getCharacter(Lexer *const restrict stream, Token *const restrict token) {
  advance(stream, 1);
  vectorAdd(&token->value, &stream->current_char);
  advance(stream, 1);

  if (stream->current_char != '\'') {
    char *buffer = calloc(64, sizeof(char));
    snprintf(buffer, 128, "Expected closing quote ('). [line %zu]", stream->current_line);
    stream->error = buffer;
    stream->eof = true;
  }
  advance(stream, 1);

  token->type = CHAR_LITERAL;
  vectorTrim(&token->value);
}

void lexerInit(Lexer *const restrict stream, char *const restrict file_name) {
  stream->fptr = fopen(file_name, "r");
  stream->error = NULL;
  stream->current_line = 1;
  stream->index = 0;

  if (!fgets(stream->buf, BUF_SIZE, stream->fptr))
    stream->eof = true;
  stream->current_char = stream->buf[0];
  stream->eof = stream->eof ? true : stream->current_char == '\0';
}

void lexerDeinit(Lexer *const restrict stream) {
  free(stream->error);
  fclose(stream->fptr);
  stream->error = NULL;
  stream->fptr = NULL;
}

Token *lexerTokenize(Lexer *const restrict stream, Token *const restrict buf, size_t bufferSize) {
  if (stream->eof)
    return NULL;

  Token token;
  for (size_t token_index = 0; token_index < bufferSize && !stream->eof; ++token_index) {
skip:
    if (DO_IGNORE(stream->current_char)) {
      advance(stream, 1);
      if (stream->eof)
        break;
      goto skip;
    }

    tokenInit(&token);
    if (IS_IDENTIFIER_BEGIN(stream->current_char)) {
      getIdentifier(stream, &token);
      if (isIn(&token, KEYWORDS, KEYWORDS_SIZE))
        token.type = KEYWORD;
      else if (isIn(&token, TYPES, TYPES_SIZE))
        token.type = TYPE;
    } else if (IS_SPECIAL_CHARACTER(stream->current_char)) {
      getOperator(stream, &token);
    } else if (IS_NUMBER(stream->current_char)) {
      getNumber(stream, &token);
    } else if (stream->current_char == '"') {
      getString(stream, &token);
    } else if (stream->current_char == '\'') {
      getCharacter(stream, &token);
    } else {
      tokenDeinit(&token);
      char *buffer = calloc(64, sizeof(char));
      snprintf(buffer, sizeof(char) * 64, "Couldn't recognize token. [line %zu]", stream->current_line);
      stream->error = buffer;
      return NULL;
    }

    buf[token_index] = token;
  }

  return buf;
}

