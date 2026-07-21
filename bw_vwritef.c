#include "binary_work.h"

int bw_vwritef(void* dest, size_t buffer_size, const char* format, va_list args) {
	return _vwrite_(dest, buffer_size, format, args);
}