#include "binary_work.h"
int bw_vreadf(const void* raw_buffer, size_t buffer_size, const char* format, va_list args) {
	return _vread_(raw_buffer, buffer_size, format, args);
}