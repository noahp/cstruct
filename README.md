[![Build Status](https://img.shields.io/travis/noahp/cstruct.svg?style=for-the-badge)](https://travis-ci.org/noahp/cstruct)
[![Codecov](https://img.shields.io/codecov/c/github/noahp/cstruct.svg?style=for-the-badge)](https://codecov.io/gh/noahp/cstruct)
[![C99](https://img.shields.io/badge/language-C99-blue.svg?style=for-the-badge)](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf)
# 🚚 cstruct
Manually pack structured data in c just like you always wanted to!

```c
int result = cstruct_packs(buf, sizeof(buf), '<hh10ci', 12, 45,
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 0x12345678);
// `buf` is now:
// { 0x12, 0x00,
//   0x45, 0x00,
//   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
//   0x78, 0x56, 0x34, 0x12 }
```

# use
Packing syntax is loosely a subset of python struct:
> https://docs.python.org/3.6/library/struct.html

Each **field** in the format string corresponds to a matching positional arg.

Arrays + endian specifiers are supported.

## endian specifiers
Alignment is always packed (none).

Default endianness is native.

These characters in the format string can be used to specify packed endianness. You can use them multiple times to switch endianess, if that's something you like:
```c
">hi<hi" // [big-endian short][big-endian int][little-endian short][little-endian int]
```

| Character | Byte order   |
|-----------|--------------|
|<          |Little endian |
|>          |Big endian    |

## format string
| Format | C type    | Size (octets)             |
|--------|-----------|---------------------------|
|x       |*pad*      |1                          |
|c       |`char`     |1<sup>[1](#weirdChar)</sup>|
|h       |`short`    |2                          |
|i       |`int`      |4                          |
|q       |`long long`|8                          |

Note that signedness is irrelevant, so use the appropriate width for your type.

*<a name="weirdChar">1</a>: Apologies to architectures with non-standard `char`, you'll need to select an appropriate width type if you want to support unpacking by casting.*

## repeat count -> array count
Instead of effecting `4h` -> `hhhh` (requiring 4 positional args), `4h` instead indicates that the matching positional arg is an array of 4 `short` values.

# build
Should work with any c99 compiler.

Option:
1. Drop the c module into your build system, or
2. `make` builds the lib

# test
`make test` to build and run the tests.

# why though
Yeah, it's pretty dumb. Enjoy!

Inspired by: https://github.com/bluekitchen/btstack/blob/master/src/hci_cmd.c#L58
