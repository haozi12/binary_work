#include "binary_work.h"

/*
 * _read_ function
 * Reads buffer_size bytes from raw_buffer and writes them to the target
 * addresses. The pointer advances according to the data type during the
 * read process.
 *
 * @param raw_buffer  Pointer to the source buffer.
 * @param buffer_size Size of the source buffer in bytes.
 * @param format      Format string (const char*). Supports:
 *                    %d, %u, %f, %lf, %hd, %hu, %hhd, %hhu,
 *                    %ld, %lu, %lld, %llu, %s, %c, %n.
 *                    The format string determines the pointer types in
 *                    the variable argument list. %n records the number of
 *                    bytes advanced so far.
 * @param ...         Variable arguments (pointers to target variables
 *                    where read data will be stored).
 *
 * @note For %s, a size_t parameter specifying the buffer size must follow
 *       immediately after the destination pointer. The %s buffer is
 *       automatically null-terminated ('\0' is appended at the end).
 *       Unsupported format specifiers are skipped.
 */

/*
* _read_函数
* 从raw_buffer读取buffer_size个字节写入目标地址，写入过程会根据数据类型推进指针
* 参数raw_buffer原始缓冲区地址
* 参数buffer_size缓冲区大小
* 格式化字符串const char* format 支持%d，%u，%f，%lf，%hd，%hu，%hhd，%hhu，%ld，%lu，%lld，%llu，%s，%c，%n
* 格式化字符串负责解析地址列表的指针类型，%n记录当前前进的字节数
* 注意%s后必须紧跟一个缓冲区大小size_t参数，不支持的格式化字符会跳过
* %s缓存区末尾自动添加\0
* 可变参数列表（待写入的地址）
*/

int _vread_(const void* raw_buffer, size_t buffer_size, const char* format, va_list args) {
	if (raw_buffer == NULL || format == NULL) {
		return -1;
	}
	const unsigned char* forward_ptr = (const unsigned char*)raw_buffer;
	int count = 0;
	const char* ptr = format;
	while (*ptr != '\0' && forward_ptr < (const unsigned char*)raw_buffer + buffer_size) {
		if (*ptr == '%') {
			ptr++;
			if (*ptr == '\0')
				break;
			switch (*ptr) {
			case 'd':
				if (forward_ptr + sizeof(int) > (const unsigned char*)raw_buffer + buffer_size) {
					goto end;
				}
				memmove(va_arg(args, int*), forward_ptr, sizeof(int));
				count++;
				forward_ptr += sizeof(int);
				break;
			case 's':
			{
				char* dest = va_arg(args, char*);
				size_t size = va_arg(args, size_t);
				if (size == 0) {
					break;
				}
				if (forward_ptr + size > (const unsigned char*)raw_buffer + buffer_size) {
					goto end;
				}
				memmove(dest, forward_ptr, size);
				dest[size - 1] = '\0';//Ensure null-termination
				count++;
				forward_ptr += size;
				break;
			}
			case 'n':
			{
				int* n_ptr = va_arg(args, int*);
				if (n_ptr != NULL) {
					*n_ptr = (int)(forward_ptr - (const unsigned char*)raw_buffer);
				}//write the number of bytes read so far into the provided pointer
				break;
			}
			case 'c':
				if (forward_ptr + sizeof(char) > (const unsigned char*)raw_buffer + buffer_size) {
					goto end;
				}
				memmove(va_arg(args, char*), forward_ptr, sizeof(char));
				count++;
				forward_ptr += sizeof(char);
				break;
			case 'f':
				if (forward_ptr + sizeof(float) > (const unsigned char*)raw_buffer + buffer_size) {
					goto end;
				}
				memmove(va_arg(args, float*), forward_ptr, sizeof(float));
				count++;
				forward_ptr += sizeof(float);
				break;
			case 'h':
				ptr++;
				if (*ptr == '\0') {
					goto end;
				}
				if (*ptr == 'd') {
					if (forward_ptr + sizeof(short) > (const unsigned char*)raw_buffer + buffer_size) {
						goto end;
					}
					memmove(va_arg(args, short*), forward_ptr, sizeof(short));
					count++;
					forward_ptr += sizeof(short);
					break;
				}
				if (*ptr == 'u') {
					if (forward_ptr + sizeof(unsigned short) > (const unsigned char*)raw_buffer + buffer_size) {
						goto end;
					}
					memmove(va_arg(args, unsigned short*), forward_ptr, sizeof(unsigned short));
					count++;
					forward_ptr += sizeof(unsigned short);
					break;
				}
				if (*ptr == 'h') {
					ptr++;
					if (*ptr == 'd') {
						if (forward_ptr + sizeof(signed char) > (const unsigned char*)raw_buffer + buffer_size) {
							goto end;
						}
						memmove(va_arg(args, signed char*), forward_ptr, sizeof(signed char));
						count++;
						forward_ptr += sizeof(signed char);
						break;
					}
					if (*ptr == 'u') {
						if (forward_ptr + sizeof(unsigned char) > (const unsigned char*)raw_buffer + buffer_size) {
							goto end;
						}
						memmove(va_arg(args, unsigned char*), forward_ptr, sizeof(unsigned char));
						count++;
						forward_ptr += sizeof(unsigned char);
						break;
					}
					if (*ptr == '\0')
					{
						goto end;
					}
					else {
						break;
					}
				}
				break;
			case 'l':
				ptr++;
				if (*ptr == '\0') {
					goto end;
				}
				if (*ptr == 'f') {
					if (forward_ptr + sizeof(double) > (const unsigned char*)raw_buffer + buffer_size) {
						goto end;
					}
					memmove(va_arg(args, double*), forward_ptr, sizeof(double));
					count++;
					forward_ptr += sizeof(double);
					break;
				}
				if (*ptr == 'd') {
					if (forward_ptr + sizeof(long) > (const unsigned char*)raw_buffer + buffer_size) {
						goto end;
					}
					memmove(va_arg(args, long*), forward_ptr, sizeof(long));
					count++;
					forward_ptr += sizeof(long);
					break;
				}
				if (*ptr == 'u') {
					if (forward_ptr + sizeof(unsigned long) > (const unsigned char*)raw_buffer + buffer_size) {
						goto end;
					}
					memmove(va_arg(args, unsigned long*), forward_ptr, sizeof(unsigned long));
					count++;
					forward_ptr += sizeof(unsigned long);
					break;
				}
				if (*ptr == 'l')
				{
					ptr++;
					if (*ptr == 'd') {
						if (forward_ptr + sizeof(long long) > (const unsigned char*)raw_buffer + buffer_size) {
							goto end;
						}
						memmove(va_arg(args, long long*), forward_ptr, sizeof(long long));
						count++;
						forward_ptr += sizeof(long long);
						break;
					}
					if (*ptr == 'u') {
						if (forward_ptr + sizeof(unsigned long long) > (const unsigned char*)raw_buffer + buffer_size) {
							goto end;
						}
						memmove(va_arg(args, unsigned long long*), forward_ptr, sizeof(unsigned long long));
						count++;
						forward_ptr += sizeof(unsigned long long);
						break;
					}
					if (*ptr == '\0')
					{
						goto end;
					}
					else {
						break;
					}
				}
				break;
			case 'u':
				if (forward_ptr + sizeof(unsigned int) > (const unsigned char*)raw_buffer + buffer_size) {
					goto end;
				}
				memmove(va_arg(args, unsigned int*), forward_ptr, sizeof(unsigned int));
				count++;
				forward_ptr += sizeof(unsigned int);
				break;
			default:
				break;
			}
		}
		ptr++;
	}
end:
	return count;
}