#include "test_helper.h"
#include "utils.h"

int main(int argc, const char *argv[]) {

	const char *test_files[] = {
		"var_decl.jix",
		"var_assign.jix",
		"if_else_stmt.jix",
		"while_stmt.jix",
		"for_stmt.jix",
		"break_stmt.jix",
		"functions.jix",
		"array_test1.jix",
	};

	long expected_results[] = {
		10,
		40,
		50,
		10,
		10,
		20,
		10,
		7,
	};

	const char *test_name[] = {
		"Variable declaration",
		"Variable assignment",
		"If-Else statement",
		"While statement",
		"For statement",
		"Break statement",
		"Functions",
		"Array test 1",
	};

	size_t total_tests = sizeof(test_files) / sizeof(test_files[0]);

	for (size_t i = 0; i < total_tests; i++) {
		struct object *return_value = interpreter_pipeline(test_files[i]);
		JIX_ASSERT_TRUE(expected_results[i], return_value->int_value, test_name[i]);
	}

	JIX_TEST_STATS();

	if (total_fail_count_ > 0) {
		return -1;
	}

	return 0;
}
