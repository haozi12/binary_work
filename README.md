# binary_work

使用C标准库风格的格式化标记符来处理二进制数据

`_write_` 和 `_read_`函数，它们使用类似 printf/scanf 的格式字符串，将原始值序列化到原始字节缓冲区或从缓冲区反序列化。格式字符串控制消耗或产生哪些值以及它们的顺序。注意，普通字符和下面未列出的说明符会被自动跳过。

支持的格式化标记符
- %d       - int
- %u       - unsigned int
- %f       - float
- %lf      - double
- %hd      - short
- %hu      - unsigned short
- %hhd     - signed char
- %hhu     - unsigned char
- %ld      - long
- %lu      - unsigned long
- %lld     - long long
- %llu     - unsigned long long
- %s       - char* buffer (requires a size_t length argument immediately after the pointer)
- %c       - char
- %n       - write the number of bytes processed so far into an `int*` argument

Behavior summary
- `_write_(dest, buffer_size, format, ...)` 把va_list的参数写入到 `dest`缓冲区直到格式化字符串结束或者缓冲区写满。
  -  `%s`期望接收 `char*` 缓冲区和 `size_t` 缓冲区大小。`_write_` 不会给 `%s` 填充末尾的'\0'。
- `_read_(raw_buffer, buffer_size, format, ...)` 从 `raw_buffer` 读取数据写入到va_list中的目标地址。
  - `%s`期望接收 `char*` 缓冲区和 `size_t` 缓冲区大小。 `_read_` 把数据写入整个`char*`缓冲区并把缓冲区末尾(`dest[size - 1]`)的字符置`\0`确保始终写入一个合法的C字符串。
- 返回值: 两个函数都返回成功写入的元素个数，注意%n不被包含在内。对format检查是否为NULL和buffer_size是否为0，任一条件为真返回-1。
- `%n` 期望一个`int*`参数，写入当前已处理的字节数。`%n` 不影成功写入的项数统计。

注意事项
- `_read_` 函数不检查va_list可变参数列表里的指针是否为NULL。
- `_read_` ， `_write_` 在格式化字符串结束或者缓冲区读/写满时结束，忽略剩下的标记符和参数列表余下的参数。
- 函数不检查大小端，从小端序写入的数据在大端序读取时会错误解释导致乱码和数据错误。
- 函数使用原生的整数大小，注意int，long在不同的实现中长度可能不同。
- 函数复制原始二进制字节，注意使用正确的格式化字符来读取二进制数据以恢复原数据。
- `%s`: 注意传入合法的char* 指针并且紧跟一个size_t参数指定缓冲区大小。如果`current byte + size > buffer_size` 函数会在复制数据前结束，不会向char*缓冲区写入任何数据。
- `%n` 使用 `int*` 存储当前读取/写入字节数。处理大型缓冲区时int可能会溢出，此时可修改函数定义中case 'n'的部分来接收更大的整数。

示例代码
```c
#include <stdio.h>
#include "binary_work.h"
//always use a fixed size instead of strlen() when handling a string to ensure _read_ can read it properly
int main(void) {
    unsigned char buffer[128] = {0};
    int bytes_processed;
    int ret;

    int a = -42;
    unsigned int b = 1234;
    float f = 3.14f;
    char str[16] = "Hello world!"; // please use sizeof(str) do not use strlen()

    /* Write: pass pointer and size for %s; pass &bytes_processed for %n.
       The returned value excludes %n (it only counts %d, %u, %f, %s here). */
    ret = _write_(buffer, sizeof(buffer), "%d,%u,%f,%s,%n",
        a, b, f, str, sizeof(str), &bytes_processed);

    /* ret == 4 (d,u,f,str). bytes_processed contains the total bytes advanced. */
    printf("items written: %d, bytes written (from %%n): %d\n", ret, bytes_processed);
    printf("buffer:");
    for (int i = 0; i < bytes_processed; i++) {
        printf(" %02x", (unsigned char)buffer[i]);
    }
    printf("\nlast byte processed: %02x\n",(unsigned char)*(buffer + bytes_processed - 1));
    /* Read back: again %n does not increment the return value. */
    int ra;
    unsigned int rb;
    float rf;
    char rs[16];// please use sizeof(rs) do not use strlen()
    int read_items, read_bytes;
    read_items = _read_(buffer, sizeof(buffer), "%d,%u,%f,%s,%n",
        &ra, &rb, &rf, rs, sizeof(rs), &read_bytes);// please use sizeof(rs) do not use strlen()

    /* read_items == 4 (d,u,f,s). read_bytes contains number of bytes consumed. */
    printf("items read: %d, bytes read (from %%n): %d\n", read_items, read_bytes);
    printf("values: %d %u %f %s\n", ra, rb, rf, rs);
    printf("last byte processed: %02x\n", (unsigned char)*(buffer + read_bytes - 1));
    return 0;
}
```



输出 (MSVC x86-64):
```
-items written: 4, bytes written (from %n): 28
-buffer: d6 ff ff ff d2 04 00 00 c3 f5 48 40 48 65 6c 6c 6f 20 77 6f 72 6c 64 21 00 00 00 00
-last byte processed: 00
-items read: 4, bytes read (from %n): 28
-values: -42 1234 3.140000 Hello world!
-last byte processed: 00
```
添加了`_vread_`，`_vwrite_`函数用于自定义封装
示例代码
```c
#include <stdio.h>
#include "binary_work.h"

typedef struct {
	float score;
	char id[16];
	char name[32];
}Student;

#define Student_size sizeof(float) + 16 * sizeof(char) + 32 * sizeof(char)
const size_t Student_Properties = 3;
int write_student_info_to_buffer(void* buffer, size_t buffer_size, const char* format, ...);
int load_student_info_from_buffer(const void* buffer, size_t buffer_size, const char* format, ...);

int main() {
	Student stu_1 = {98.0,"CK20192013","John Valantine"};
	unsigned char buffer[Student_size] = {0};

	write_student_info_to_buffer(NULL, sizeof(buffer), "%f,%s,%s", stu_1.score,
		stu_1.name, sizeof(stu_1.name), stu_1.id, sizeof(stu_1.id));

	write_student_info_to_buffer(buffer, sizeof(buffer) - 1, "%f,%s,%s", stu_1.score,
		stu_1.name, sizeof(stu_1.name), stu_1.id, sizeof(stu_1.id));

	write_student_info_to_buffer(buffer, sizeof(buffer),"%f,%s,%s",stu_1.score,
		stu_1.name,sizeof(stu_1.name),stu_1.id,sizeof(stu_1.id));

	Student stu_1_backup;

	load_student_info_from_buffer(NULL, sizeof(buffer), "%d,%s,%s", &stu_1_backup.score, stu_1_backup.name,
		sizeof(stu_1_backup.name), stu_1_backup.id, sizeof(stu_1_backup.id));

	load_student_info_from_buffer(buffer, sizeof(buffer)-1, "%d,%s,%s", &stu_1_backup.score, stu_1_backup.name,
		sizeof(stu_1_backup.name), stu_1_backup.id, sizeof(stu_1_backup.id));

	load_student_info_from_buffer(buffer, sizeof(buffer), "%d,%s,%s", &stu_1_backup.score, stu_1_backup.name,
		sizeof(stu_1_backup.name), stu_1_backup.id, sizeof(stu_1_backup.id));

	printf("Student info:\nid: %s\nname: %s\n\score: %.1f\n",stu_1_backup.id,stu_1_backup.name,stu_1_backup.score);
	return 0;
}




int load_student_info_from_buffer(const void* buffer,size_t buffer_size, const char* format, ...) {
	va_list args;
	va_start(args,format);
	int count = _vread_(buffer,buffer_size,format,args);
	va_end(args);
	if (count == -1) {
		fprintf(stderr,"Load Error: Invalid buffer.\n");
		return -1;
	}
	if (count != Student_Properties) {
		fprintf(stderr, "Load Error: Buffer size is not enough for information loading.\n");
		return -2;
	}
	return count;
}

int write_student_info_to_buffer(void* buffer, size_t buffer_size, const char* format, ...) {
	va_list args;
	va_start(args, format);
	int count = _vwrite_(buffer, buffer_size, format, args);
	va_end(args);
	if (count == -1) {
		fprintf(stderr, "Write Error: Invalid buffer.\n");
		return -1;
	}
	if (count != Student_Properties) {
		fprintf(stderr, "Write Error: Buffer size is not enough for information writing.\n");
		return -2;
	}
	return count;
}
```
输出（MSVC x86-64）:
```
-Write Error: Invalid buffer.
-Write Error: Buffer size is not enough for information writing.
-Load Error: Invalid buffer.
-Load Error: Buffer size is not enough for information loading.
-Student info:
-id: CK20192013
-name: John Valantine
-score: 98.0
```

