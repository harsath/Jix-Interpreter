#include "test-helper.h"
#include "utils.h"

int main(int argc, const char *argv[]) {

	const char *test_files[] = {
		"var_decl.jix",
		"var_assign.jix"
	};

	long expected_results[] = {
		10,
		40
	};

	const char *test_name[] = {
		"Variable declaration",
		"Variable assignment"
	};

	size_t total_tests = sizeof(test_files) / sizeof(test_files[0]);

	for (size_t i = 0; i < total_tests; i++) {
		struct object *return_value = interpreter_pipeline(test_files[i]);
		JIX_ASSERT_TRUE(return_value->int_value == expected_results[i], test_name[i]);
	}

	JIX_TEST_STATS();

	return 0;
}
