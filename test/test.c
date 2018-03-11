//
// Basic test coverage for cstruct.
//
#include <stdio.h>
#include <string.h>
#include "../src/cstruct.h"

// World's cheesiest UT framework
#define TEXT_RED_START "\033[31;1m"
#define TEXT_GREEN_START "\033[32;1m"
#define TEXT_END "\033[0m"
#define TEXT_RED(text) "\033[31;1m" text "\033[0m\n"
#define TEXT_GREEN(text) "\033[32;1m" text "\033[0m\n"
#define TEXT_BLUE(text) "\033[34;1m" text "\033[0m\n"

#define UT_INT_EQUAL(expected, actual)                                                        \
    if (expected != actual) {                                                                 \
        printf(TEXT_RED("%s:%d  %d doesn't match %d"), __FILE__, __LINE__, expected, actual); \
        return -1;                                                                            \
    } else {                                                                                  \
        printf(TEXT_GREEN("%s:%d  int_equal passed"), __FILE__, __LINE__);                    \
    }

static void prv_print_hex(const char *prefix, const char *suffix, const char *buf, size_t len) {
    printf("%s [", prefix);
    for (size_t i = 0; i < len; i++) {
        printf(" %02x", buf[i]);
    }
    printf(" ] %s", suffix);
}

static int prv_ut_mem_equal(const char *expected, size_t expected_len, const char *actual,
                            size_t actual_len, int line, const char *filename) {
    if (expected_len != actual_len) {
        return -1;
    }

    size_t i;
    for (i = 0; i < expected_len; i++) {
        if (expected[i] != actual[i]) {
            break;
        }
    }

    if (i < expected_len) {
        printf(TEXT_RED("%s:%d  memcmp mismatch"), filename, line);
        prv_print_hex(TEXT_RED_START "Expected: ", TEXT_END, expected, expected_len);
        prv_print_hex(TEXT_RED_START "Actual: ", TEXT_END "\n", actual, actual_len);
        return -1;
    }
    printf(TEXT_GREEN("%s:%d  memcmp passed"), filename, line);
    return 0;
}

#define UT_MEM_EQUAL(expected, expected_len, actual, actual_len)                                 \
    if (prv_ut_mem_equal(expected, expected_len, actual, actual_len, __LINE__, __FILE__) != 0) { \
        return -1;                                                                               \
    }

int main(int argc, const char **argv) {
    char outbuf[512];
    int packed = 0;

#define test_case_0 "\x34\x12\x78\x56\x34\x12"
    packed = cstruct_packs(outbuf, sizeof(outbuf), "<hi", 0x1234, 0x12345678);
    UT_INT_EQUAL(6, packed);
    UT_MEM_EQUAL(test_case_0, sizeof(test_case_0) - 1, outbuf, packed);

#define test_case_1 "\x34\x00\x78\x56\x34\x12"
    packed = cstruct_packs(outbuf, sizeof(outbuf), "<cxi", 0x34, 0x12345678);
    UT_INT_EQUAL(6, packed);
    UT_MEM_EQUAL(test_case_1, sizeof(test_case_1) - 1, outbuf, packed);

#define test_case_2 "\x34\x12\x78\x56\x34\x12"
    packed = cstruct_packs(outbuf, sizeof(outbuf), "<hi", 0x1234, 0x12345678);
    UT_INT_EQUAL(6, packed);
    UT_MEM_EQUAL(test_case_0, sizeof(test_case_2) - 1, outbuf, packed);

    return 0;
}
