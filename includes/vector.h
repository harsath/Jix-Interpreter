#ifndef VECTOR_H
#define VECTOR_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_INITIAL_SIZE 10
#define GENERIC_TYPE unsigned char
#define GENERIC_TYPE_PTR GENERIC_TYPE *

struct vector {
  GENERIC_TYPE_PTR *_internal_buffer;
  size_t size;
  size_t capacity;
};

struct vector *vector_init();
bool vector_push_back(struct vector *vector_, void *item);
void *vector_remove_at(struct vector *vector_, size_t index);
void *vector_at(struct vector *vector_, size_t index);
void vector_free(struct vector *vector_);

bool _vector_increase_capacity(struct vector *vector_);

#endif
