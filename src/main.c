#include "../include/opcp.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* CLI entry point: opcp [-b size] <src> <dst> (default buffer: 4 KiB). */
int main(int argc, char *argv[])
{
	int copy_result;

	if (argc == 3) {
		copy_result = copy(argv[1], argv[2], 4096);

		if (copy_result != 0) {
			fprintf(stderr,"Operation failed!\n");
		}

		return copy_result;
	}
	else if (argc == 5) {
		if (strcmp(argv[1], "-b") == 0) {
			size_t buff_size;
			buff_size = parse_size(argv[2]);
			/* 0 indicates either a parse failure or an invalid zero-byte buffer. */
			if (buff_size == 0) {
				fprintf(stderr,"Invalid buffer size!\n");
				return 1;
			}
			copy_result = copy(argv[3], argv[4], buff_size);
			if (copy_result != 0) {
				fprintf(stderr,"Operation failed!\n");
			}
			return copy_result;
		}
		else {
			fprintf(stderr,"Invalid argument!\n");
			return 1;
		}
	}
	else {
		fprintf(stderr,"Invalid format!\nExample usage: opcp [-b size] <src> <dst>\n");
		return 1;
	}

}
