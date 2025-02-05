#pragma once
#include "ast.c"
#include "token.c"
#include "lexer.c"
#include <stdlib.h>

#define TOKEN_BUF_SIZE 16 

typedef struct {
  size_t index;
  size_t token_count;
  Token current_token;
  Node program;
  Lexer token_stream;
  Token buf[TOKEN_BUF_SIZE];
  char *error;
  bool eof;
} Parser;

void nextToken(Parser *const restrict parser, size_t ammount) {
  tokenDeinit(&parser->buf[parser->index]);
  if (ammount > 1) {
    for (size_t i = 0; i < ammount; ++i)
      nextToken(parser, 1);
  
  } else {
    ++parser->index;
    if (parser->index == TOKEN_BUF_SIZE) {
      parser->eof = (parser->token_count = lexerTokenize(&parser->token_stream, parser->buf, TOKEN_BUF_SIZE)) == 0;
      parser->index = 0;
    }
  }

  parser->current_token = parser->buf[parser->index];
}

void parserInit(Parser *restrict const parser, char *restrict const file_name) {
  lexerInit(&parser->token_stream, file_name);
  parser->eof = (parser->token_count = lexerTokenize(&parser->token_stream, parser->buf, TOKEN_BUF_SIZE)) == 0;
  parser->index = 0;
  parser->current_token = parser->buf[parser->index];
  nodeInit(&parser->program, sizeof(Node));

}

Node *parseStatement(Parser *restrict const parser) {
  switch (parser->current_token.type) {
    case KEYWORD:
      return getKeyword();
    case TYPE:
      return getCurrent(parser, TYPE_SIGNATURE);
    case IDENTIFIER:
      return getCurrent(parser, FLoAT);
    case OPERATOR:
      return getCurrent(parser, FLoAT);
    case STRING_LITERAL:
      return getCurrent(parser, FLoAT);
    case CHAR_LITERAL:
      return getCurrent(parser, FLoAT);
    case INT_LITERAL:
      return getCurrent(parser, FLoAT);
    case FLOAT_LITERAL:
      return getCurrent(parser, FLoAT);

    default:
      char buf[64];
      snprintf(buf, sizeof(buf) / sizeof(char),
          "Couldn't recognise pattern. [line %zu char %zu]",
          parser->current_token.line,
          parser->current_token.begin);
      parser->error = buf;
  }
  return NULL;
}

void parserParse(Parser *parser, Node *program) {
  while (!parser->eof) {
    vectorAdd(&parser->program.children, (char *)parseStatement(parser));
  }
}

