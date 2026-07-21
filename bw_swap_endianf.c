#include "binary_work.h"

int bw_swap_endianf(const char* format, ...) {
	va_list args;
	va_start(args, format);
	int count = bw_vswap_endianf(format, args);
	va_end(args);
	return count;
}