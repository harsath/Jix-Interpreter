#ifndef utils_h
#define utils_h

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 10240

char *read_file(const char *file_path) {
	if (!file_path) {
		perror("Filepath is NULL");
		return NULL;
	}
	int fd = open(file_path, O_RDONLY);
	if (fd < 0) {
		perror("Error opening file");
		return NULL;
	}

	char *buffer = calloc(BUFFER_SIZE, sizeof(char));
	if (!buffer) {
		perror("Memory allocation error on for input file buffer");
		close(fd);
		return NULL;
	}

	ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
	if (buffer < 0) {
		perror("Error reading file");
		close(fd);
		free(buffer);
		return NULL;
	}

	buffer[bytes_read] = '\0';
	close(fd);

	return buffer;

}


#endif
