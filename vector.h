#ifndef vector_h
#define vector_h


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#define DEFAULT_INITIAL_SIZE 10
#define GENERIC_TYPE unsigned char
#define GENERIC_TYPE_PTR GENERIC_TYPE *

typedef struct {
    GENERIC_TYPE_PTR *_internal_buffer;
    size_t size;
    size_t capacity;
} vector;

static vector *vector_init();
static bool vector_push_back(vector *vector_, void *item);
static bool vector_remove_at(vector *vector_, size_t index);
static void *vector_at(vector *vector_, size_t index);
static void vector_free(vector *vector_);

static bool _vector_increase_capacity(vector *vector_);

static vector *vector_init() {
    vector *vector_ = calloc(1, sizeof(vector));
    if (!vector_) {
	perror("Vector initilization error");
	return NULL;
    }
    vector_->_internal_buffer = calloc(DEFAULT_INITIAL_SIZE, sizeof(GENERIC_TYPE_PTR));
    if (!vector_->_internal_buffer) {
	perror("Vector initilization error");
	free(vector_);
	return NULL;
    }
    vector_->size = 0;
    vector_->capacity = DEFAULT_INITIAL_SIZE;
    return vector_;
}

static bool vector_push_back(vector *vector_, void *item) {
    assert(vector_ != NULL && item != NULL);
    if (vector_->capacity <= vector_->size) {
	if(!_vector_increase_capacity(vector_)) {
	    return false;
	}
    }
    vector_->_internal_buffer[vector_->size++] = item;
    return true;
}
static bool vector_remove_at(vector *vector_, size_t index) {
    assert(vector_ != NULL && index >= 0);
    if (vector_->size <= index) { return false; }
    size_t current_index = index;
    do {
	void *next_value_ptr = vector_->_internal_buffer[current_index+1];
	vector_->_internal_buffer[current_index] = next_value_ptr;
	current_index++;
    } while (current_index < vector_->size);
    vector_->size--;
    return true;
}
static void *vector_at(vector *vector_, size_t index) {
    assert(vector_ != NULL && index >= 0);
    if (vector_->size <= index) { return NULL; }
    return vector_->_internal_buffer[index];
}
static void vector_free(vector *vector_) {
    free(vector_->_internal_buffer);
    free(vector_);
}

static bool _vector_increase_capacity(vector *vector_) {
    GENERIC_TYPE_PTR *temp_ptr = realloc(vector_->_internal_buffer, 
	    sizeof(GENERIC_TYPE_PTR) * vector_->capacity * 2);
    if (!temp_ptr) { return false; }
    vector_->_internal_buffer = temp_ptr;
    vector_->capacity = vector_->capacity * 2;
    return true;
}

#endif
