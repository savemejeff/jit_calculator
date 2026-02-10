#include "codegen.h"

#define MOV(xd, imm, sh) \
((1 << 31) | (0xa5 << 23) | (sh << 17) | (imm << 5) | (xd))
#define MOVK(xd, imm, sh) \
((1 << 31) | (0xe5 << 23) | (sh << 17) | (imm << 5) | (xd))

#define DTOX 6
#define XTOD 7
#define FMOV(op, rn, rd) \
((1 << 31) | (0x1e << 24) | (3 << 21) | (op << 16) | (rn << 5) | (rd))

#define SP 31
#define STR(rt, rn, imm) \
((0x03f4 << 22) | ((imm / 8) << 10) | (rn << 5) | (rt))
#define LDR(rt, rn, imm) \
((0x03f5 << 22) | ((imm / 8) << 10) | (rn << 5) | (rt))


void ret()
{
    pop();
    // ldp x29, x30, [sp], #0x1f0
    emit_dword(0xa8df7bfd);
    // ret
    emit_dword(0xd65f03c0);
}

int stack_top = 0x10;
void pop()
{
    stack_top -= 8;
    emit_dword(LDR(0, 29, stack_top));
}

void pop2()
{
    stack_top -= 8;
    emit_dword(LDR(1, 29, stack_top));
}

void push()
{
    emit_dword(STR(0, 29, stack_top));
    stack_top += 8;
}

void neg()
{
    push();
    pop2();
    immediate(0);
    push();
    pop();
    sub();
}

void add()
{
    emit_dword(0x1e612800);
}

void sub()
{
    emit_dword(0x1e613800);
}

void mul()
{
    emit_dword(0x1e610800);
}

void idiv()
{
    emit_dword(0x1e611800);
}

void exponent()
{
    uint64_t imm = (uint64_t)pow;
    emit_dword(MOV(0, (imm & 0xffff), 0));
    emit_dword(MOVK(0, ((imm >> 16) & 0xffff), 16));
    emit_dword(MOVK(0, ((imm >> 32) & 0xffff), 32));
    emit_dword(MOVK(0, ((imm >> 48) & 0xffff), 48));
    emit_dword(0xd63f0000);
}

void immediate(uint64_t imm)
{
    emit_dword(MOV(0, (imm & 0xffff), 0));
    emit_dword(MOVK(0, ((imm >> 16) & 0xffff), 16));
    emit_dword(MOVK(0, ((imm >> 32) & 0xffff), 32));
    emit_dword(MOVK(0, ((imm >> 48) & 0xffff), 48));
    emit_dword(FMOV(XTOD, 0, 0));
}

// XXX: stack has a fixed capacity now
void preamble()
{
    // stp x29, x30, [sp, #-0x1f0]!
    emit_dword(0xa9a17bfd);
    emit_dword(0x910003fd);
}

void postamble()
{
}
