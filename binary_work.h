#ifndef __BINARY_WORK_H__
#define __BINARY_WORK_H__

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

/*
 * This header file contains the _read_ and _write_ functions.
 * Note: These functions do not handle endianness, nor do they address the
 * issue that the sizes of int and long vary across different platforms.
 */


 /*
 *此头文件包含_read_,_write_函数
 *注意_write_,_read_函数不处理大小端序，不同平台上int、long字长不同的问题
 *提供_vwrite_，_vread_函数用于自定义封装
*/


int _write_(void* dest, size_t buffer_size, const char* format, ...);
int _read_(const void* raw_buffer, size_t buffer_size, const char* format, ...);
int _vwrite_(void* dest,size_t buffer_size,const char* format,va_list args);
int _vread_(const void* raw_buffer, size_t buffer_size, const char* format, va_list args);
#endif