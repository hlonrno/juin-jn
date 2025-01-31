#pragma once

#include <stddef.h>

typedef struct {
  size_t capacity;
  size_t length;
  size_t element_size;
  void *data;
} Vector;

Vector *vec_init(size_t initial_capacity, size_t element_size);
void vec_destroy(Vector *vec);
void vec_ensure_capacity(Vector *vec, size_t capacity);
void vec_add(Vector *vec, void *element);
void vec_insert(Vector *vec, void *element, size_t index);
void *vec_pop(Vector *vec, size_t index);
void vec_trim(Vector *vec);
