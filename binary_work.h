#ifndef __BINARY_WORK_H__
#define __BINARY_WORK_H__

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#define INVALID_SIZE (size_t)-1

/*
 *此头文件包含_read_,_write_函数
*/
inline size_t count_element(size_t memory_size,size_t elememt_size){
	if (elememt_size == 0){
		return INVALID_SIZE;
	}
	return memory_size / elememt_size;
}

int _write_(const void* raw_buffer, size_t buffer_size, const char* format, ...);
int _read_(const void* raw_buffer, size_t buffer_size, const char* format, ...);
#endif