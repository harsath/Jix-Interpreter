#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#define RESET_COLOR "\033[0m"
#define RED_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"

static size_t total_test_count_ = 0;
static size_t total_fail_count_ = 0;
static size_t total_pass_count_ = 0;

#define JIX_ASSERT_TRUE(expression_result, test_name)                          \
  do {                                                                         \
    total_test_count_++;                                                       \
    if (expression_result) {                                                   \
      total_pass_count_++;                                                     \
      printf("TEST: %s, STATUS: %s[ OK ]%s, LINE: %d\n", test_name,            \
             GREEN_COLOR, RESET_COLOR, __LINE__);                              \
    } else {                                                                   \
      total_fail_count_++;                                                     \
      printf("TEST: %s, STATUS: %s[ FAIL ]%s, LINE: %d\n", test_name,          \
             RED_COLOR, RESET_COLOR, __LINE__);                                \
    }                                                                          \
  } while (0)

#define JIX_TEST_STATS()                                                       \
  do {                                                                         \
    printf("\nTotal tests: %lu\nTotal tests passed: %lu\nTotal tests failed: " \
           "%lu\n",                                                            \
           total_test_count_, total_pass_count_, total_fail_count_);           \
  } while (0)

#endif