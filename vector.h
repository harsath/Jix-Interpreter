#ifndef VECTOR_H
#define VECTOR_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_INITIAL_SIZE 10
#define GENERIC_TYPE unsigned char
#define GENERIC_TYPE_PTR GENERIC_TYPE *

typedef struct {
  GENERIC_TYPE_PTR *_internal_buffer;
  size_t size;
  size_t capacity;
} vector;

vector *vector_init();
bool vector_push_back(vector *vector_, void *item);
bool vector_remove_at(vector *vector_, size_t index);
void *vector_at(vector *vector_, size_t index);
void vector_free(vector *vector_);

bool _vector_increase_capacity(vector *vector_);

#endif
