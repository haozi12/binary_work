#ifndef BINARY_WORK_LOADED
#define BINARY_WORK_LOADED

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
namespace bi_work {
	extern "C" {
#endif
		enum ErrorCode {
			NULL_BUFFER_OR_FORMAT = -1,
			NULL_FILE = -2,
			MALLOC_FAILURE = -3
		};

		/*
		 * This header file contains the _read_ and _write_ functions.
		 * Note: These functions do not handle endianness, nor do they address the
		 * issue that the sizes of int and long vary across different platforms.
		 * _vread_,_vwrite_ for customize working
		 * use bw_bfreadf,bw_bfwritef instead of _read_,_write_, they are the same, but bw_bf prefix is more readable
		 * bw_freadf,bw_fwritef for file reading or writing
		 * Format string (const char*). Supports: %d, %u, %f, %lf, %hd, %hu, %hhd, %hhu,
		 * %ld, %lu, %lld, %llu, %s, %c, %n.
		 * The format string controls how data is parsed and written into the destination buffer.
		 * %n records the number of bytes advanced so far.
		 */


		 /*
		 *此头文件包含_read_,_write_函数
		 *注意_write_,_read_函数不处理大小端序，不同平台上int、long字长不同的问题
		 *提供_vwrite_，_vread_函数用于自定义封装
		 *现在可以使用统一命名后的函数名称
		 *_read_ -> bw_bfreadf
		 *_write_ -> bw_bfwritef
		 *bw_freadf,bw_fwritef可以直接从文件按格式化字符串读取或者写入二进制数据
		 格式化字符串const char* format 支持%d，%u，%f，%lf，%hd，%hu，%hhd，%hhu，%ld，%lu，%lld，%llu，%s，%c，%n
		 *现在可以使用bw_vreadf,bw_vwritef来调用_vread_和_vwrite_
		*/

		int _write_(void* dest, size_t buffer_size, const char* format, ...);
		int _read_(const void* raw_buffer, size_t buffer_size, const char* format, ...);


		int _vwrite_(void* dest, size_t buffer_size, const char* format, va_list args);
		int _vread_(const void* raw_buffer, size_t buffer_size, const char* format, va_list args);

		static inline int bw_vreadf(const void* raw_buffer, size_t buffer_size, const char* format, va_list args) {
			return _vread_(raw_buffer, buffer_size, format, args);
		}

		static inline int bw_vwritef(void* dest, size_t buffer_size, const char* format, va_list args) {
			return _vwrite_(dest, buffer_size, format, args);
		}


		int bw_bfreadf(const void* raw_buffer, size_t buffer_size, const char* format, ...);
		int bw_bfwritef(void* dest, size_t buffer_size, const char* format, ...);


		int bw_vfreadf(FILE* _Stream, size_t read_length, const char* format, va_list args);
		int bw_vfwritef(FILE* _Stream, size_t write_length, const char* format, va_list args);

		int bw_fwritef(FILE* _Stream, size_t write_length, const char* format, ...);
		int bw_freadf(FILE* _Stream, size_t read_length, const char* format, ...);

#ifdef __cplusplus
		static inline int vreadf(const void* raw_buffer, size_t buffer_size, const char* format, va_list args) {
			return _vread_(raw_buffer, buffer_size, format, args);
		}

		static inline int vwritef(void* dest, size_t buffer_size, const char* format, va_list args) {
			return _vwrite_(dest, buffer_size, format, args);
		}


		static inline int bfreadf(const void* raw_buffer, size_t buffer_size, const char* format, ...) {
			va_list args;
			va_start(args, format);
			int count = _vread_(raw_buffer, buffer_size, format, args);
			va_end(args);
			return count;
		}
		static inline int bfwritef(void* dest, size_t buffer_size, const char* format, ...) {
			va_list args;
			va_start(args, format);
			int count = _vwrite_(dest, buffer_size, format, args);
			va_end(args);
			return count;
		}


		static inline int vfreadf(FILE* _Stream, size_t read_length, const char* format, va_list args) {
			if (_Stream == NULL) {
				return NULL_FILE;
			}
			unsigned char* buffer = (unsigned char*)calloc(read_length, sizeof(unsigned char));
			if (buffer == NULL) {
				perror("malloc failed");
				return MALLOC_FAILURE;
			}
			size_t bytes = fread(buffer, sizeof(unsigned char), read_length, _Stream);
			if (bytes < read_length) {
				fprintf(stderr, "Warning: File does not have enough bytes, data might be incomplete.\n");
			}
			int count = _vread_(buffer, bytes, format, args);
			free(buffer);
			return count;

		}
		static inline int vfwritef(FILE* _Stream, size_t write_length, const char* format, va_list args) {
			if (_Stream == NULL) {
				return NULL_FILE;
			}
			unsigned char* buffer = (unsigned char*)calloc(write_length, sizeof(unsigned char));
			if (buffer == NULL) {
				perror("malloc failed");
				return MALLOC_FAILURE;
			}
			int count = _vwrite_(buffer, write_length, format, args);
			size_t bytes = fwrite(buffer, sizeof(unsigned char), write_length, _Stream);
			if (bytes < write_length) {
				fprintf(stderr, "Warning: bytes written: %zu, but expected: %zu\n", bytes, write_length);
			}
			free(buffer);
			return count;
		}

		static inline int fwritef(FILE* _Stream, size_t write_length, const char* format, ...) {
			va_list args;
			va_start(args, format);
			int count = bw_vfwritef(_Stream, write_length, format, args);
			va_end(args);
			return count;
		}
		static inline int freadf(FILE* _Stream, size_t read_length, const char* format, ...) {
			va_list args;
			va_start(args, format);
			int count = bw_vfreadf(_Stream, read_length, format, args);
			va_end(args);
			return count;
		}
	}
}
#endif

#endif