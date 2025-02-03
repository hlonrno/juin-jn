#pragma once
#include "vector.c"

typedef enum {
  PROGRAM,
  VARIABLE_DECLARATION,
  VARIABLE_DEFINITION,
  BINARY_OPERATION,
  TERNARY_OPERATION,
  // a lot more...
} NodeType;

typedef struct Node {
  NodeType type;
  Vector children;
} Node;

void nodeInit(Node *const restrict node, size_t child_size) {
  vectorInit(&node->children, 2, child_size);
}

void nodeDeinit(Node *const restrict node) {
  vectorDeinit(&node->children);
}

