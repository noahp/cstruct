#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// copy, reversed
static void *prv_memcpy_r(void *dest, const void *src, size_t count) {
    size_t i;
    for (i = 0; count > 0; i++, count--) {
        ((uint8_t *)dest)[i] = ((uint8_t *)src)[count - 1];
    }
    ((uint8_t *)dest)[i] = ((uint8_t *)src)[0];

    return dest;
}

int v_cstruct_packs(char *buffer, size_t buffersize, const char *fmt, size_t argc, ...) {
    bool little_endian = true;
    uint8_t *tempbuf = (uint8_t*)buffer;
    int array_len = 0;

    va_list args;
    va_start(args, argc);

// check if args are left
#define CHECK_ARGC()   \
    if (argc-- == 0) { \
        return -1;     \
    }
#define CHECK_SIZE(count)                        \
    if (tempbuf + count > (uint8_t*)buffer + buffersize) { \
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
                        CHECK_SIZE(sizeof(uint16_t));
                        ENDIAN_MEMCPY(tempbuf, &hval[i], sizeof(*hval));
                        tempbuf += sizeof(*hval);
                    }
                } else {
                    CHECK_SIZE(1);
                    uint16_t hval = (uint16_t)va_arg(args, int);
                    ENDIAN_MEMCPY(tempbuf, &hval, sizeof(hval));
                    tempbuf += sizeof(hval);
                }
                array_len = 0;
                break;

            case 'i':
                CHECK_ARGC();
                if (array_len) {
                    uint32_t *ival = va_arg(args, uint32_t *);
                    for (size_t i = 0; i < array_len; i++) {
                        CHECK_SIZE(sizeof(uint32_t));
                        ENDIAN_MEMCPY(tempbuf, &ival[i], sizeof(*ival));
                        tempbuf += sizeof(*ival);
                    }
                } else {
                    CHECK_SIZE(sizeof(uint32_t));
                    uint32_t ival = va_arg(args, uint32_t);
                    ENDIAN_MEMCPY(tempbuf, &ival, sizeof(ival));
                    tempbuf += sizeof(ival);
                }
                array_len = 0;
                break;

            case 'l':
                CHECK_ARGC();
                if (array_len) {
                    uint64_t *llval = va_arg(args, uint64_t *);
                    for (size_t i = 0; i < array_len; i++) {
                        CHECK_SIZE(sizeof(uint64_t));
                        ENDIAN_MEMCPY(tempbuf, &llval[i], sizeof(*llval));
                        tempbuf += sizeof(*llval);
                    }
                } else {
                    CHECK_SIZE(sizeof(uint64_t));
                    uint64_t llval = va_arg(args, uint64_t);
                    ENDIAN_MEMCPY(tempbuf, &llval, sizeof(llval));
                    tempbuf += sizeof(llval);
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
    return tempbuf - (uint8_t*)buffer;
}
