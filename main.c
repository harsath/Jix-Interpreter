#include "utils.h"

int main(int argc, const char *argv[]){
	const char *file_name = "foo.jix";
	char *input = read_file(file_name);
	if (!input) {
		return -1;
	}
	printf("%s",input);
	
	return 0;
}
