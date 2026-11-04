#include "../include/opcp.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int copy_result;

	if (argc == 3) {
		copy_result = copy(argv[1], argv[2], 4096);
	
		if (copy_result != 0) {
			printf("Operation failed!"); 
		}

		return copy_result;
	}
	else if (argc == 5) {
		if (strcmp(argv[1], "-b") == 0) {
			long buff_size;
			buff_size = parse_size(argv[2]);
			if (buff_size == 0) {
				printf("Invalid buffer size!");
				return 1;
			}
			copy_result = copy(argv[3], argv[4], buff_size);
			if (copy_result != 0) {
				printf("Operation failed!");
			}
			return copy_result;
		}
		else {
			printf("Invalid argument!");
			return 1;
		}
	}
	else {
		printf("Invalid format!\nExample usage: opcp [-b size] <src> <dst>");
		return 1;
	}
	
}
