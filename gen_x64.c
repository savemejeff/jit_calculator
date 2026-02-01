#include "codegen.h"

void ret()
{
    pop();
    emit_bytes(0xc3);
}

void pop()
{
    // popq %rax
    emit_bytes(0x58);
    // movq %rax, %xmm0
    emit_bytes(0x66, 0x48, 0x0f, 0x6e, 0xc0);
}

void pop2()
{
    // popq %rax
    emit_bytes(0x58);
    // movq %rax, %xmm1
    emit_bytes(0x66, 0x48, 0x0f, 0x6e, 0xc8);
}

void push()
{
    // movq %xmm0, %rax
    emit_bytes(0x66, 0x48, 0x0f, 0x7e, 0xc0);
    // push %rax
    emit_bytes(0x50);
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
    emit_bytes(0xf2, 0x0f, 0x58, 0xc1);
}

void sub()
{
    emit_bytes(0xf2, 0x0f, 0x5c, 0xc1);
}

void mul()
{
    emit_bytes(0xf2, 0x0f, 0x59, 0xc1);
}

void idiv()
{
    emit_bytes(0xf2, 0x0f, 0x5e, 0xc1);
}

void exponent()
{
    emit_bytes(0x48, 0xb8);
    emit_qword((uint64_t)pow);
    // call *(%rax)
    emit_bytes(0xff, 0xd0);
}

void immediate(uint64_t imm)
{
    // movabs $imm, %rax
    emit_bytes(0x48, 0xb8);
    emit_qword(imm);
    // movq %rax, %xmm0
    emit_bytes(0x66, 0x48, 0x0f, 0x6e, 0xc0);
}
