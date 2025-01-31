#include "lexer.h"
#include "token.h"
#include "vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IS_NUMBER(n) (n > 47 && n < 58)
#define IS_SPECIAL_CHAR(n) strchr("#{}()[]:.,;=*/%+-", n)
#define IS_LETTER(n) ((n > 96 && n < 123) || (n > 64 && n < 91) || n == '_' || n == '#')
#define IS_IDENTIFIER(n) (IS_LETTER(n) || IS_NUMBER(n))
#define DO_IGNORE(n) strchr(" \n\t\b\r\0", n)
#define KEYWORDS "else,if,for,var,while,fn,fnstruct"

static void advance(Lexer *lexer, size_t count);
static bool is_keyword(char *identifier);
static Vector *format(const char fmt[], size_t things);

static Vector *get_identifier(Lexer *lexer);
static Vector *get_string(Lexer *lexer);
static char *get_char(Lexer *lexer);
static long long get_integer(Lexer *lexer);

Lexer *tokenize(FILE *fptr) {
  Lexer *lexer = malloc(sizeof(Lexer));
  Vector *tokens = vec_init(512, sizeof(Token));
  lexer->fptr = fptr;
  lexer->line_number = 1;
  lexer->tokens = tokens;
  lexer->eof = 0;
  advance(lexer, 0);

  while (!lexer->eof) {
    if (DO_IGNORE(lexer->current_char)) {
      advance(lexer, 1);
      continue;
    } else if (IS_LETTER(lexer->current_char)) {
      Vector *identifier = get_identifier(lexer);
      Token *token = malloc(sizeof(Token));
      token->type = is_keyword(identifier->data) ? KEYWORD : IDENTIFIER;
      token->value.arr = identifier;
      vec_add(tokens, token);
      // free(token);
    } else if (IS_SPECIAL_CHAR(lexer->current_char)) {
      char operator = lexer->current_char;
      Token *token = malloc(sizeof(Token));
      token->type = OPERATOR;
      memcpy(token->value.str, &operator, sizeof(char));
      vec_add(tokens, token);
      // free(token);
    } else if (lexer->current_char == '"') {
      Vector *str = get_string(lexer);
      Token *token = malloc(sizeof(Token));
      token->type = STR_LITERAL;
      token->value.arr = str;
      vec_add(tokens, token);
      // free(token);
    } else if (IS_NUMBER(lexer->current_char)) {
      Token *token = malloc(sizeof(Token));
      long long integer = get_integer(lexer);
      token->type = INT_LITERAL;
      token->value.num = integer;

      if (lexer->current_char == '.') {
        long long decimal = get_integer(lexer);
        long long radix = 1;
        while (decimal > radix) radix *= 10;
        double flt = integer + decimal / (double)radix;
        token->type = FLT_LITERAL;
        token->value.flt = flt;
      }
      vec_add(tokens, token);
      // free(token);
    } else {
      Vector *message = format("Couldn't find Token. [line %]", lexer->line_number);
      lexer->error = 1;
      lexer->error_message = message->data;
      // free(message);
      break;
    }
    
    advance(lexer, 1);
  }

  printf("# Tokens: %zu;\n", tokens->length);
  vec_trim(tokens);
  return lexer;
}

static void advance(Lexer *lexer, size_t count) {
  lexer->index += count;

  if (lexer->index > BUF_SIZE || count == 0) {
    lexer->index %= BUF_SIZE;
    lexer->current_char = '\0';
    if (!fgets(lexer->buf, BUF_SIZE, lexer->fptr))
      lexer->eof = 1;
    else
      lexer->current_char = lexer->buf[lexer->index];
  }
}

static bool is_keyword(char *identifier) {
  for (size_t i = 0; KEYWORDS[i] != '\0'; i++) {
    if (identifier[0] == KEYWORDS[i]) {
      for (size_t j = 0;; j++)
        if (identifier[j] == '\0' && !IS_IDENTIFIER(KEYWORDS[i + j]))
          return 1;
        else if (identifier[j] != KEYWORDS[i + j])
          break;
    }

    while (IS_IDENTIFIER(KEYWORDS[i]))
      i++;
  }

  return 0;
}

static Vector *format(const char fmt[], size_t number) {
  Vector *message = vec_init(32, sizeof(char));

  char count = 0;
  for (size_t i = 0; fmt[i] != '\0'; i++) {
    if (count || fmt[i] != '%') {
      vec_add(message, (void *)(fmt + i));
      continue;
    }

    count++;
    size_t radix = 1;
    while ((radix *= 10) <= number);
    radix /= 10;

    while (radix > 0) {
      char chr = (number / radix) % 10 + '0';
      vec_add(message, &chr);
      number %= radix;
      radix /= 10;
    }
  }

  vec_trim(message);
  return message;
}

static Vector *get_identifier(Lexer *lexer) {
  Vector *identifier = vec_init(16, sizeof(char));

  while (IS_IDENTIFIER(lexer->current_char)) {
    vec_add(identifier, &lexer->current_char);
    advance(lexer, 1);
  }

  vec_trim(identifier);
  return identifier;
}

static Vector *get_string(Lexer *lexer) {
  Vector *str = vec_init(32, sizeof(char));

  advance(lexer, 1);
  while (lexer->current_char != '"' && !lexer->eof) {
    vec_add(str, &lexer->current_char);
    advance(lexer, 1); 
  }
  advance(lexer, 1);

  vec_trim(str);
  return str;
}

static char *get_char(Lexer *lexer) {
  char *chr = malloc(1);

  advance(lexer, 1);
  *chr = lexer->current_char;
  advance(lexer, 2);

  return chr;
}

static long long get_integer(Lexer *lexer) {
  long long num = 0;
  Vector *str = vec_init(8, sizeof(char));

  while (IS_NUMBER(lexer->current_char)) {
    vec_add(str, &lexer->current_char);
    advance(lexer, 1);
  }

  while (str->length > 0) {
    num += (*(char *)vec_pop(str, str->length - 1) - '0');
    num *= 10;
  }

  vec_destroy(str);
  return num;
}

