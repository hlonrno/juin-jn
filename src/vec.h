#pragma once

#include <stddef.h>

typedef struct {
  size_t capacity;
  size_t length;
  size_t element_size;
  void *data;
} vec_t;

vec_t *vec_init(size_t initial_size, size_t element_size);
void vec_destroy(vec_t *arr);
void vec_ensure_capacity(vec_t *arr, size_t capacity);
void vec_add(vec_t *arr, void *elem);
void vec_insert(vec_t *arr, void *elem, size_t index);
void *vec_pop(vec_t *arr, size_t index);
void vec_trim(vec_t *arr);
