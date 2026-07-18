#include "binary_work.h"

/*
 * _write_ function
 * Writes buffer_size bytes to dest. The pointer advances according to the
 * data type during the write process.
 *
 * @param dest        Pointer to the destination buffer.
 * @param buffer_size Number of bytes to be written.
 * @param format      Format string (const char*). Supports:
 *                    %d, %u, %f, %lf, %hd, %hu, %hhd, %hhu,
 *                    %ld, %lu, %lld, %llu, %s, %c, %n.
 *                    The format string controls how data is parsed and
 *                    written into the destination buffer. %n records the
 *                    number of bytes advanced so far.
 * @param ...         Variable arguments (data to be written).
 *
 * @note For %s, a size_t parameter specifying the buffer size must follow
 *       immediately after the source pointer. Unsupported format specifiers
 *       are skipped.
 */

/*
* _write_函数
* 向dest写入buffer_size个字节，写入过程会根据数据类型推进指针
* 参数dest目标缓冲区地址
* 参数buffer_size待写入的字节数
* 格式化字符串const char* format 支持%d，%u，%f，%lf，%hd，%hu，%hhd，%hhu，%ld，%lu，%lld，%llu，%s，%c，%n
* 格式化字符串负责解析数据并且写入目标缓冲区，%n记录当前前进字节数
* 注意%s后必须紧跟一个缓冲区大小size_t参数，不支持的格式化字符会跳过
* 可变参数列表（待写入的数据）
*/


int _vwrite_(void* dest, size_t buffer_size, const char* format, va_list args) {
	if (dest == NULL || format == NULL) {
		return -1;
	}
	unsigned char* forward_ptr = (unsigned char*)dest;
	int count = 0;
	const char* ptr = format;
	while (*ptr != '\0' && forward_ptr < (unsigned char*)dest + buffer_size) {
		if (*ptr == '%') {
			ptr++;
			if (*ptr == '\0')
				break;
			switch (*ptr) {
			case 'd':
				if (forward_ptr + sizeof(int) > (unsigned char*)dest + buffer_size) {
					goto end;
				}
				int value = va_arg(args, int);
				memmove(forward_ptr, &value, sizeof(int));
				count++;
				forward_ptr += sizeof(int);
				break;
			case 's':
			{
				char* source = va_arg(args, char*);
				size_t size = va_arg(args, size_t);
				if (source == NULL) {
					goto end;
				}
				if (size == 0) {
					break;
				}
				if (forward_ptr + size > (unsigned char*)dest + buffer_size) {
					goto end;
				}
				memmove(forward_ptr, source, size);
				count++;
				forward_ptr += size;
				break;
			}
			case 'n':
			{
				int* n_ptr = va_arg(args, int*);
				if (n_ptr != NULL) {
					*n_ptr = (int)(forward_ptr - (const unsigned char*)dest);
				}//write the number of bytes written so far into the provided pointer
				break;
			}
			case 'c':
				if (forward_ptr + sizeof(char) > (unsigned char*)dest + buffer_size) {
					goto end;
				}
				char c = (char)va_arg(args, int);// Promote char to int when passing through '...'
				memmove(forward_ptr, &c, sizeof(char));
				count++;
				forward_ptr += sizeof(char);
				break;
			case 'f':
				if (forward_ptr + sizeof(float) > (unsigned char*)dest + buffer_size) {
					goto end;
				}
				float f = (float)va_arg(args, double); // Promote float to double when passing through '...'
				memmove(forward_ptr, &f, sizeof(float));
				count++;
				forward_ptr += sizeof(float);
				break;
			case 'h':
				ptr++;
				if (*ptr == '\0') {
					goto end;
				}
				if (*ptr == 'd') {
					if (forward_ptr + sizeof(short) > (unsigned char*)dest + buffer_size) {
						goto end;
					}
					short s = (short)va_arg(args, int);// Promote short to int when passing through '...'
					memmove(forward_ptr, &s, sizeof(short));
					count++;
					forward_ptr += sizeof(short);
					break;
				}
				if (*ptr == 'u') {
					if (forward_ptr + sizeof(unsigned short) > (unsigned char*)dest + buffer_size) {
						goto end;
					}
					unsigned short us = (unsigned short)va_arg(args, unsigned int); // Promote unsigned short to unsigned int when passing through '...'
					memmove(forward_ptr, &us, sizeof(unsigned short));
					count++;
					forward_ptr += sizeof(unsigned short);
					break;
				}
				if (*ptr == 'h') {
					ptr++;
					if (*ptr == 'd') {
						if (forward_ptr + sizeof(signed char) > (unsigned char*)dest + buffer_size) {
							goto end;
						}
						signed char sc = (signed char)va_arg(args, int); // Promote signed char to int when passing through '...'
						memmove(forward_ptr, &sc, sizeof(signed char));
						count++;
						forward_ptr += sizeof(signed char);
						break;
					}
					if (*ptr == 'u') {
						if (forward_ptr + sizeof(unsigned char) > (unsigned char*)dest + buffer_size) {
							goto end;
						}
						unsigned char uc = (unsigned char)va_arg(args, unsigned int); // Promote unsigned char to unsigned int when passing through '...'
						memmove(forward_ptr, &uc, sizeof(unsigned char));
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
					if (forward_ptr + sizeof(double) > (unsigned char*)dest + buffer_size) {
						goto end;
					}
					double d = va_arg(args, double);
					memmove(forward_ptr, &d, sizeof(double));
					count++;
					forward_ptr += sizeof(double);
					break;
				}
				if (*ptr == 'd') {
					if (forward_ptr + sizeof(long) > (unsigned char*)dest + buffer_size) {
						goto end;
					}
					long l = va_arg(args, long);
					memmove(forward_ptr, &l, sizeof(long));
					count++;
					forward_ptr += sizeof(long);
					break;
				}
				if (*ptr == 'u') {
					if (forward_ptr + sizeof(unsigned long) > (unsigned char*)dest + buffer_size) {
						goto end;
					}
					unsigned long ul = va_arg(args, unsigned long);
					memmove(forward_ptr, &ul, sizeof(unsigned long));
					count++;
					forward_ptr += sizeof(unsigned long);
					break;
				}
				if (*ptr == 'l')
				{
					ptr++;
					if (*ptr == 'd') {
						if (forward_ptr + sizeof(long long) > (unsigned char*)dest + buffer_size) {
							goto end;
						}
						long long ll = va_arg(args, long long);
						memmove(forward_ptr, &ll, sizeof(long long));
						count++;
						forward_ptr += sizeof(long long);
						break;
					}
					if (*ptr == 'u') {
						if (forward_ptr + sizeof(unsigned long long) > (unsigned char*)dest + buffer_size) {
							goto end;
						}
						unsigned long long ull = va_arg(args, unsigned long long);
						memmove(forward_ptr, &ull, sizeof(unsigned long long));
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
				if (forward_ptr + sizeof(unsigned int) > (unsigned char*)dest + buffer_size) {
					goto end;
				}
				unsigned int ui = va_arg(args, unsigned int);
				memmove(forward_ptr, &ui, sizeof(unsigned int));
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
