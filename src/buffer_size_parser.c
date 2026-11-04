#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include "../include/opcp.h"

long parse_size(const char *str) 
{
	int value = 0; 
	long multiplier;
	long total;

	if (str == NULL || *str == '\0') {
		return 0; // Empty buffer size 
	}
	if (!(isdigit(*str))) {
		return 0; // Not digit argument
	}
	while (isdigit(*str)) { // taking the number
		value = (value * 10) + (*str - '0');
		str++;
	}
	switch (*str) { //determining the multiplier
		case 'K': case 'k': 
			multiplier = 1024;
			break;
		case 'M': case 'm':
			multiplier = 1024 * 1024;
			break;
		case 'G': case 'g': 
			multiplier = 1024 * 1024 * 1024;
			break;
		case 'B': case 'b': 
		case '\0': // In case of Null byte
			multiplier = 1;
			break;
		default: 
			return 0; // Unknown unit suffix
	}
	if (*str != '\0' && *(str + 1) != '\0') {
		return 0;
	}
	total = value * multiplier;
	if (total == 0) {
		return 0; // Buffer size cannot be 0
	}
	return total;
}


