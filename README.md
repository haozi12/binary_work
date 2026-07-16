# binary_work

Small helpers for writing and reading binary data using C-style format specifiers.

This project provides two functions, `_write_` and `_read_`, that serialize and deserialize primitive values into/from a raw byte buffer using a format string similar to printf/scanf. The format string controls which values are consumed/produced and in what order. Notice that normal characters and the specifiers not listed below will be automatically skipped.

Supported format specifiers
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
- `_write_(dest, buffer_size, format, ...)` writes values from the varargs into `dest` until the format is consumed or the buffer limit is reached.
  - For `%s`, the caller must pass both a `char*` and a `size_t` length; that many bytes are written. `_write_` does not add a null terminator for `%s`.
- `_read_(raw_buffer, buffer_size, format, ...)` reads values from `raw_buffer` into pointers supplied in the varargs.
  - For `%s`, the caller must pass a `char*` buffer and a `size_t` length; `_read_` copies that many bytes from the source and ensures `dest[size-1] = '\0'` (it null-terminates the last byte).
- Return value: Both functions return the count of format items successfully written/read. The count excludes `%n` specifiers (i.e., `%n` does not increment the returned item count). On invalid parameters (for example, NULL buffer or format), the function returns `-1`.
- `%n` behavior: When `%n` is present in the format, it stores the number of bytes processed so far into the provided `int*`. The action of `%n` does not change the returned success count.

Important portability and safety notes
- Endianness: These functions do NOT handle endianness. Data written on a little-endian machine may be misinterpreted on a big-endian machine.
- Type sizes: The functions use the platform's native sizes for `int`, `long`, etc. For portable formats prefer fixed-width types (e.g., `int32_t`, `uint64_t`).
- Alignment: The code copies raw bytes; consumers must interpret the bytes with the correct alignment/type size.
- `%s`: Always pass a valid pointer and correct size; otherwise reads/writes may overflow.
- `%n` uses `int*` for storing the byte count. If you expect buffers > INT_MAX, consider changing the API to use a larger integer type for `%n`.

Example
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
    char str[16] = "hello"; // please use sizeof(str) do not use strlen()

    /* Write: pass pointer and size for %s; pass &bytes_processed for %n.
       The returned value excludes %n (it only counts %d, %u, %f, %s here). */
    ret = _write_(buffer, sizeof(buffer), "%d,%u,%f,%s,%n",
        a, b, f, str, sizeof(str), &bytes_processed);// please use sizeof(str) do not use strlen()

    /* ret == 4 (d,u,f,str). bytes_processed contains the total bytes advanced. */
    printf("items written: %d, bytes written (from %%n): %d\n", ret, bytes_processed);

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

    return 0;
}
```



Outputs:
```
-items written: 4, bytes written (from %n): 28
-items read: 4, bytes read (from %n): 28
-values: -42 1234 3.140000 hello
```
