#include "binary_work.h"

int bfwritef(void* dest, size_t buffer_size, const char* format, ...) {
	va_list args;
	va_start(args, format);
	int count = _vwrite_(dest, buffer_size, format, args);
	va_end(args);
	return count;
}