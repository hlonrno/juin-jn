#include "tokens.h"
#include <stdlib.h>

token_t *token_init(token_type_t type, token_value_t value) {
  token_t *token = malloc(sizeof(token_t));
  token->type = type;
  token->value = value;
  return token;
}
