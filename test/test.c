
#include "../src/cstruct.h"

int main(int argc, const char **argv) {
    char outbuf[512];
    return cstruct_packs(outbuf, sizeof(outbuf), "<hi", 0x1234, 0x12345678);
}
