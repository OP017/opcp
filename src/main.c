#include "../include/opcp.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		return 1;
	}

	int copy_result = copy(argv[1], argv[2], 1024);
	
	if (copy_result != 0) {
		printf("Operation failed!"); 
	}

	return copy_result;
}
