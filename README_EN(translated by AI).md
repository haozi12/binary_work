# Binary work

Serialize and deserialize binary data using C standard library-style format specifiers.

The `_write_` and `_read_` functions use format strings similar to printf/scanf to serialize primitive values to and deserialize from raw byte buffers. The format string controls which values are consumed or produced and in what order. Note that normal characters and specifiers not listed below are automatically skipped.

The functions `_read_`, `_write_`, `_vread_`, and `_vwrite_` have been renamed (the original function names are still usable). You can now use `bw_vreadf` and `bw_vwritef` to call `_vread_` and `_vwrite_` respectively. File operation variants have also been added, so you can now read and write binary data directly from/to files using `bw_bfwritef`, `bw_bfreadf`, `bw_freadf`, or `bw_fwritef`. C++ support has been added, and functions reside under the `bi_work` namespace. Prefix-free versions (without the `bw_` prefix) are provided for C++. The `%r` specifier has been added—it is similar to `%s`, but `_read_` and `_vread_` will not write a `'\0'` at the end of the buffer.

**July 21, 2026**  
Added the byte-order swap function `bw_swap_endianf` and its `va_list` counterpart `bw_vswap_endianf`. These can process multiple variables in a single call using a format string, returning the number of successfully converted items. Note: `%n` is not supported by these functions.  
Optimized the header file by moving `static inline` implementations into separate `.c` files.

Function declarations:
```c
int bw_swap_endianf(const char* format, ...);
int bw_vswap_endianf(const char* format, va_list args);
```

Function list: `_write_` (same as `bw_bfwritef`) | `_read_` (same as `bw_bfreadf`) | `_vread_` (same as `bw_vreadf`) | `_vwrite_` (same as `bw_vwritef`)  
| `bw_vfreadf` | `bw_vfwritef` | `bw_freadf` | `bw_fwritef`

## Supported format specifiers
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
- %r       - similar to %s, but `_read_` and `_vread_` will not put a '\0' at the end of the buffer
- %c       - char
- %n       - write the number of bytes processed so far into an `int*` argument

## Function behavior
- `_write_(dest, buffer_size, format, ...)` writes values from the va_list arguments into the `dest` buffer until the format string ends or the buffer is full.
  - For `%s`, it expects a `char*` buffer and a `size_t` buffer size. `_write_` does **not** append a trailing `'\0'` for `%s`.
- `_read_(raw_buffer, buffer_size, format, ...)` reads data from `raw_buffer` and writes it to the destination addresses in the va_list.
  - For `%s`, it expects a `char*` buffer and a `size_t` buffer size. `_read_` copies data into the entire `char*` buffer and sets the last byte (`dest[size - 1]`) to `'\0'` to ensure a valid C string is always produced.
- Return value: Both functions return the number of elements successfully written/read. Note that `%n` is not included in this count. If `format` is NULL or `buffer_size` is 0, the function returns -1.
- `%n` expects an `int*` argument and writes the number of bytes processed so far. `%n` does not affect the count of successfully written/read items.

## Important notes
- The `_read_` function does not check whether pointers in the va_list are NULL.
- `_read_` and `_write_` stop when the format string ends or the buffer is full (read/written to capacity), ignoring any remaining specifiers and arguments in the parameter list.
- These functions do not handle endianness. Data written in little-endian order will be misinterpreted when read in big-endian order, leading to garbled data and errors.
- The functions use native integer sizes. Be aware that the sizes of `int`, `long`, etc. may differ across implementations.
- The functions copy raw binary bytes. Make sure to use the correct format specifiers when reading binary data to recover the original values.
- `%s`: Always pass a valid `char*` pointer followed immediately by a `size_t` argument specifying the buffer size. If `current byte + size > buffer_size`, the function will stop before copying any data, and nothing will be written to the `char*` buffer.
- `%n` uses an `int*` to store the current number of read/written bytes. When processing large buffers, `int` may overflow. In such cases, you can modify the `case 'n'` section of the function definition to use a larger integer type.
- Make sure the `size` used when writing via `%s` is consistent with the `size` used when reading; otherwise, garbled data and errors will occur.

## Example code
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

Output (MSVC x86-64):
```
items written: 4, bytes written (from %n): 28
buffer: d6 ff ff ff d2 04 00 00 c3 f5 48 40 48 65 6c 6c 6f 20 77 6f 72 6c 64 21 00 00 00 00
last byte processed: 00
items read: 4, bytes read (from %n): 28
values: -42 1234 3.140000 Hello world!
last byte processed: 00
```

The `_vread_` and `_vwrite_` functions are provided for custom wrappers.

### Example code
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
	Student stu_1 = { 98.0,"CK20192013","John Valantine" };
	unsigned char buffer[Student_size] = { 0 };

	write_student_info_to_buffer(buffer, sizeof(buffer), NULL, stu_1.score,
		stu_1.name, sizeof(stu_1.name), stu_1.id, sizeof(stu_1.id));

	write_student_info_to_buffer(NULL, sizeof(buffer), "%f,%s,%s", stu_1.score,
		stu_1.name, sizeof(stu_1.name), stu_1.id, sizeof(stu_1.id));

	write_student_info_to_buffer(buffer, sizeof(buffer) - 1, "%f,%s,%s", stu_1.score,
		stu_1.name, sizeof(stu_1.name), stu_1.id, sizeof(stu_1.id));

	write_student_info_to_buffer(buffer, sizeof(buffer), "%f,%s,%s", stu_1.score,
		stu_1.name, sizeof(stu_1.name), stu_1.id, sizeof(stu_1.id));

	Student stu_1_backup;

	load_student_info_from_buffer(buffer, sizeof(buffer), NULL, &stu_1_backup.score, stu_1_backup.name,
		sizeof(stu_1_backup.name), stu_1_backup.id, sizeof(stu_1_backup.id));

	load_student_info_from_buffer(NULL, sizeof(buffer), "%f,%s,%s", &stu_1_backup.score, stu_1_backup.name,
		sizeof(stu_1_backup.name), stu_1_backup.id, sizeof(stu_1_backup.id));

	load_student_info_from_buffer(buffer, sizeof(buffer) - 1, "%f,%s,%s", &stu_1_backup.score, stu_1_backup.name,
		sizeof(stu_1_backup.name), stu_1_backup.id, sizeof(stu_1_backup.id));

	load_student_info_from_buffer(buffer, sizeof(buffer), "%f,%s,%s", &stu_1_backup.score, stu_1_backup.name,
		sizeof(stu_1_backup.name), stu_1_backup.id, sizeof(stu_1_backup.id));

	printf("Student info:\nid: %s\nname: %s\n\score: %.1f\n", stu_1_backup.id, stu_1_backup.name, stu_1_backup.score);
	return 0;
}



int load_student_info_from_buffer(const void* buffer, size_t buffer_size, const char* format, ...) {
	va_list args;
	va_start(args, format);
	int count = _vread_(buffer, buffer_size, format, args);
	va_end(args);
	if (count == -1) {
		fprintf(stderr, "Load Error: Invalid buffer or format string.\n");
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
		fprintf(stderr, "Write Error: Invalid buffer or format string.\n");
		return -1;
	}
	if (count != Student_Properties) {
		fprintf(stderr, "Write Error: Buffer size is not enough for information writing.\n");
		return -2;
	}
	return count;
}

```
Output (MSVC x86-64):
```
Write Error: Invalid buffer or format string.
Write Error: Invalid buffer or format string.
Write Error: Buffer size is not enough for information writing.
Load Error: Invalid buffer or format string.
Load Error: Invalid buffer or format string.
Load Error: Buffer size is not enough for information loading.
Student info:
id: CK20192013
name: John Valantine
score: 98.0
```

### `bw_freadf` and `bw_fwritef` example code:
```c
#include <stdio.h>
#include "binary_work.h"

void write_file() {
	FILE* file = fopen("test", "wb");
	if (file == NULL) {
		perror("fail to open file");
		return;
	}
	int a = -18841;
	float b = 0.213f;
	char str[16] = "Hello world!";
	size_t write_size = sizeof(int) + sizeof(float) + sizeof(str);
	int count = bw_fwritef(file,write_size,"%d,%f,%s", a, b, str, sizeof(str));
	printf("Elements written: %d\nType: int,float,char[16]\n", count);
	fclose(file);
}

void read_file() {
	FILE* file = fopen("test", "rb");
	if (file == NULL) {
		perror("fail to open file");
		return;
	}
	int a;
	float b;
	char str[16] = {0};
	size_t read_size = sizeof(int) + sizeof(float) + sizeof(str);
	int count = bw_freadf(file, read_size, "%d,%f,%s", &a, &b, str, sizeof(str));
	printf("Elements read: %d\nType: int: %d\nType: float: %f\nType: char[16]: %s\n", count,a,b,str);
	fclose(file);
}

int main(void) {
	write_file();
	read_file();
	return 0;
}
```
Output (MSVC x86-64):
```
Elements written: 3
Type: int,float,char[16]
Elements read: 3
Type: int: -18841
Type: float: 0.213000
Type: char[16]: Hello world!
```

### `bw_swap_endianf` usage example
```c
#include <stdio.h>
#include "binary_work.h"

int main() {
	int a = 42;
	double b = 3.14;
	int count = bw_swap_endianf("%d %lf", &a, &b);
	printf("converted count:%d, %d,%lf\n", count,a, b);
	count = bw_swap_endianf("%d %lf", &a, &b);
	printf("converted count:%d, %d,%lf\n", count, a, b);
	return 0;
}
```
Output:
```
converted count:2, 704643072,0.000000
converted count:2, 42,3.140000