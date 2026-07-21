#include "binary_work.h"

int freadf(FILE* _Stream, size_t read_length, const char* format, ...) {
	va_list args;
	va_start(args, format);
	int count = bw_vfreadf(_Stream, read_length, format, args);
	va_end(args);
	return count;
}