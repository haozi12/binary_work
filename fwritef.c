#include "binary_work.h"

int fwritef(FILE* _Stream, size_t write_length, const char* format, ...) {
	va_list args;
	va_start(args, format);
	int count = bw_vfwritef(_Stream, write_length, format, args);
	va_end(args);
	return count;
}