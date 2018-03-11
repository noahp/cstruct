#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <stdio.h>

// copy, reversed
static void *prv_memcpy_r(void *dest, const void *src, size_t count) {
    size_t i;
    for (i = 0; count > 0; i++, count--) {
        ((char *)dest)[i] = ((char *)src)[count - 1];
    }
    ((char *)dest)[i] = ((char *)src)[0];

    return dest;
}

int v_cstruct_packs(char *buffer, size_t buffersize, const char *fmt, size_t argc, ...) {
    bool little_endian = true;
    char *tempbuf = buffer;

    va_list args;
    va_start(args, argc);

// check if args are left
#define CHECK_ARGC()   \
    if (argc-- == 0) { \
        return -1;     \
    }
#define CHECK_SIZE(count)                        \
    if (tempbuf + count > buffer + buffersize) { \
        goto done;                               \
    }

// endian copy
#define ENDIAN_MEMCPY(dest, src, count) \
    (little_endian) ? (memcpy(dest, src, count)) : (prv_memcpy_r(dest, src, count))

    while (*fmt != '\0') {
        switch (*fmt) {
            case '<':
                little_endian = true;
                break;

            case '>':
                little_endian = false;
                break;

            case 'x':
                CHECK_SIZE(1);
                *tempbuf = '\0';
                tempbuf++;
                break;

            case 'c': {
                CHECK_ARGC();
                CHECK_SIZE(1);
                uint8_t cval = (uint8_t)va_arg(args, int);
                *tempbuf = cval;
                tempbuf++;
            } break;

            case 'h': {
                CHECK_ARGC();
                CHECK_SIZE(1);
                int hval = va_arg(args, int);
                ENDIAN_MEMCPY(tempbuf, &hval, 2);
                tempbuf += 2;
            } break;

            case 'i': {
                CHECK_ARGC();
                CHECK_SIZE(4);
                int ival = va_arg(args, int);
                ENDIAN_MEMCPY(tempbuf, &ival, 4);
                tempbuf += 4;
            }
            break;

            default:
                return -1;
        }
        fmt++;
    }

    va_end(args);

done:
    return tempbuf - buffer;
}
