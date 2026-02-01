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

#define emit_qword(x) do {                   \
    uint64_t _x = x;                          \
    for (int _i = 0; _i < 8; _i++) {         \
        emit_byte((_x >> (_i * 8)) & 0xff);  \
    }                                        \
} while (0)

void ret();
void pop();
void pop2();
void push();
void neg();
void add();
void sub();
void mul();
void idiv();
void exponent();
void immediate(uint64_t imm);

#endif // CODEGEN_H
