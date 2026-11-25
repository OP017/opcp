#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include "../include/opcp.h"

/* Parse a size string with optional B/K/M/G suffix; returns 0 on error. */
size_t parse_size(const char *str)
{
	size_t value = 0;
	size_t multiplier;
	size_t total;

	if (str == NULL || *str == '\0') {
		return 0;
	}
	if (!(isdigit(*str))) {
		return 0;
	}
	while (isdigit(*str)) {
		value = (value * 10) + (size_t)(*str - '0');
		str++;
	}
	switch (*str) {
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
		case '\0':
			multiplier = 1;
			break;
		default:
			return 0;
	}
	/* Reject trailing characters after the unit suffix (e.g. "4KB"). */
	if (*str != '\0' && *(str + 1) != '\0') {
		return 0;
	}
	total = value * multiplier;
	if (total == 0) {
		return 0;
	}
	return total;
}
