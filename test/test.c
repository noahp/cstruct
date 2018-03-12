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
        printf(" %02x", (unsigned char)buf[i]);
    }
    printf(" ] %s", suffix);
}

static int prv_ut_mem_equal(const char *expected, size_t expected_len, const char *actual,
                            size_t actual_len, int line, const char *filename) {
    size_t i = ~expected_len;
    if (expected_len == actual_len) {
        for (i = 0; i < expected_len; i++) {
            if (expected[i] != actual[i]) {
                break;
            }
        }
    }
    if (i != expected_len) {
        printf(TEXT_RED("%s:%d  memcmp mismatch"), filename, line);
        prv_print_hex(TEXT_RED_START "Expected: ", TEXT_END "\n", expected, expected_len);
        prv_print_hex(TEXT_RED_START "Actual:   ", TEXT_END "\n", actual, actual_len);
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

// basic little-endian
#define test_case_0 "\x34\x12\x78\x56\x34\x12"
    packed = cstruct_packs(outbuf, sizeof(outbuf), "<hi", 0x1234, 0x12345678);
    UT_INT_EQUAL(6, packed);
    UT_MEM_EQUAL(test_case_0, sizeof(test_case_0) - 1, outbuf, packed);

// pad
#define test_case_1 "\x34\x00\x78\x56\x34\x12"
    packed = cstruct_packs(outbuf, sizeof(outbuf), "<cxi", 0x34, 0x12345678);
    UT_INT_EQUAL(6, packed);
    UT_MEM_EQUAL(test_case_1, sizeof(test_case_1) - 1, outbuf, packed);

// basic big-endian
#define test_case_2 "\x12\x34\x12\x34\x56\x78"
    packed = cstruct_packs(outbuf, sizeof(outbuf), ">hi", 0x1234, 0x12345678);
    UT_INT_EQUAL(6, packed);
    UT_MEM_EQUAL(test_case_2, sizeof(test_case_2) - 1, outbuf, packed);

// illegal character
    packed = cstruct_packs(outbuf, sizeof(outbuf), "^>hi", 0x1234, 0x12345678);
    UT_INT_EQUAL(-1, packed);

// wrong number of input arguments
    packed = cstruct_packs(outbuf, sizeof(outbuf), "<hii", 0x1234, 0x12345678);
    UT_INT_EQUAL(-1, packed);

// switch endian
#define test_case_3 "\x12\x34\x56\x78\x9a\xbc\xde\xf0\x0f\xed\xcb\xa9\x87\x65\x43\x21"
    packed =
        cstruct_packs(outbuf, sizeof(outbuf), ">l<l", 0x123456789abcdef0ULL, 0x21436587a9cbed0fULL);
    UT_INT_EQUAL(16, packed);
    UT_MEM_EQUAL(test_case_3, sizeof(test_case_3) - 1, outbuf, packed);

// arrays
#define test_case_4                                                                                \
    "\x31\x32\x33\x34\x35\x36\x01\x00\x02\x00\x03\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00" \
    "\x00\x04\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00" \
    "\x00\x00"
    short shortarray[3] = {1, 2, 3};
    int intarray[5] = {1, 2, 3, 4, 5};
    long long qarray[2] = {1, 2};
    packed = cstruct_packs(outbuf, sizeof(outbuf), "2c4c3h5i2l", "12", "3456", shortarray, intarray,
                           qarray);
    UT_INT_EQUAL(48, packed);
    UT_MEM_EQUAL(test_case_4, sizeof(test_case_4) - 1, outbuf, packed);

// array only applies to attached element
#define test_case_5 "\x12\x34\x54"
    packed = cstruct_packs(outbuf, sizeof(outbuf), "2cc", "\x12\x34", 0x54);
    UT_INT_EQUAL(3, packed);
    UT_MEM_EQUAL(test_case_5, sizeof(test_case_5) - 1, outbuf, packed);

    return 0;
}
