#ifndef CODEGEN_H

#include "common.h"
#include "misc.h"

typedef struct {
    uint8_t *items;
    size_t count;
    size_t capacity;
} Code;

extern Code *code;

#define emit_byte(byte) da_append(code, byte)

#define emit_bytes(...) do {                      \
    uint8_t bytes[] = { __VA_ARGS__ };            \
    for (size_t _i = 0; _i < sizeof(bytes); _i++) \
        emit_byte(bytes[_i]);                     \
} while (0)

#define emit_dword(dword) do {     \
    uint32_t _d = dword;           \
    emit_bytes((_d) & 0xff,        \
               (_d >> 8) & 0xff,   \
               (_d >> 16) & 0xff,  \
               (_d >> 24) & 0xff); \
} while (0)

#endif // CODEGEN_H
