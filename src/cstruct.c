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
    int array_len = 0;

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
                array_len = (array_len) ? (array_len) : (1);
                CHECK_SIZE(array_len);
                memset(tempbuf, 0, array_len);
                tempbuf += array_len;
                array_len = 0;
                break;

            case 'c':
                CHECK_ARGC();
                if (array_len) {
                    CHECK_SIZE(array_len);
                    uint8_t *cval = va_arg(args, uint8_t *);
                    memcpy(tempbuf, cval, array_len);
                    tempbuf += array_len;
                } else {
                    CHECK_SIZE(1);
                    int cval = va_arg(args, int);
                    printf(" char: %x\n", cval);
                    *tempbuf = (uint8_t)cval;
                    tempbuf++;
                }
                array_len = 0;
                break;

            case 'h':
                CHECK_ARGC();
                if (array_len) {
                    uint16_t *hval = va_arg(args, uint16_t *);
                    for (size_t i = 0; i < array_len; i++) {
                        CHECK_SIZE(2);
                        ENDIAN_MEMCPY(tempbuf, &hval[i], 4);
                        tempbuf += 2;
                    }
                } else {
                    CHECK_SIZE(1);
                    int hval = va_arg(args, int);
                    ENDIAN_MEMCPY(tempbuf, &hval, 2);
                    tempbuf += 2;
                }
                array_len = 0;
                break;

            case 'i':
                CHECK_ARGC();
                if (array_len) {
                    int *ival = va_arg(args, int *);
                    for (size_t i = 0; i < array_len; i++) {
                        CHECK_SIZE(4);
                        ENDIAN_MEMCPY(tempbuf, &ival[i], 4);
                        tempbuf += 4;
                    }
                } else {
                    CHECK_SIZE(4);
                    int ival = va_arg(args, int);
                    ENDIAN_MEMCPY(tempbuf, &ival, 4);
                    tempbuf += 4;
                }
                array_len = 0;
                break;

            case 'l':
                CHECK_ARGC();
                if (array_len) {
                    long long *llval = va_arg(args, long long *);
                    for (size_t i = 0; i < array_len; i++) {
                        CHECK_SIZE(8);
                        ENDIAN_MEMCPY(tempbuf, &llval[i], 8);
                        tempbuf += 8;
                    }
                } else {
                    CHECK_SIZE(8);
                    long long llval = va_arg(args, long long);
                    ENDIAN_MEMCPY(tempbuf, &llval, 8);
                    tempbuf += 8;
                }
                array_len = 0;
                break;

            default:
                // handle array length prefix
                if (*fmt >= '0' && *fmt <= '9') {
                    // cheesy atoi https://stackoverflow.com/a/868508
                    array_len = array_len * 10 + (*fmt - '0');
                } else {
                    return -1;
                }
                break;
        }
        fmt++;
    }

    va_end(args);

done:
    return tempbuf - buffer;
}
