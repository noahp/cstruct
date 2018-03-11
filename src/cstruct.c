#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <stdio.h>

int cstruct_packs(char *buffer, size_t buffersize, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);

    while (*fmt != '\0') {
        switch (*fmt) {
            case '<':
            case '>':
                break;

            case 'h':
                printf("short! %x\n", va_arg(args, int));
                break;

            case 'i':
                printf("int! %x\n", va_arg(args, uint32_t));
                break;

            default:
                return -1;
        }
        fmt++;
    }

    va_end(args);

    return 0;
}
