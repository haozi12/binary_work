#include "binary_work.h"

int vswap_endianf(const char* format, va_list args) {
	return bw_vswap_endianf(format, args);
}