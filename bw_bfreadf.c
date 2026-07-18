#include "binary_work.h"

//another name of _read_

int bw_bfreadf(const void* raw_buffer, size_t buffer_size, const char* format, ...) {
	va_list args;
	va_start(args, format);
	int count = _vread_(raw_buffer, buffer_size, format, args);
	va_end(args);
	return count;
}