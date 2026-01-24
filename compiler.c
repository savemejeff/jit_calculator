#include "compiler.h"
#include "scanner.h"

typedef enum {
  PREC_NONE,
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_EXPONENT,    // ^
  PREC_PRIMARY
} Precedence;

typedef void (*parse_fn)();

typedef struct {
    parse_fn prefix;
    parse_fn infix;
    Precedence precedence;
} Parse_Rule;

Code *code;

static void parse_precedence(Precedence precedence);
static Parse_Rule *get_rule(Token_Type type);

typedef union {
  uint64_t u64[1];
  double   f64[1];
  uint32_t u32[2];
  uint8_t  u8[8];
} Raw;

static void emit_byte(uint8_t byte)
{
    da_append(code, byte);
}

#define emit_bytes(...) do {			\
  uint8_t bytes[] = { __VA_ARGS__ };		\
  for (size_t _i = 0; _i < sizeof(bytes); _i++) \
    emit_byte(bytes[_i]);			\
} while (0)

static void emit_dword(uint32_t dword)
{
  Raw raw = {0};
  raw.u32[0] = dword;
  emit_byte(raw.u8[0]);
  emit_byte(raw.u8[1]);
  emit_byte(raw.u8[2]);
  emit_byte(raw.u8[3]);
}

static void ret()
{
    emit_byte(0xc3);
}

// XXX: the `movd` part is same, maybe refactor this
static void mov_rax_xmm0()
{
    emit_bytes(0x66, 0x48);
    emit_bytes(0x0f, 0x6e);
    emit_byte(0xc0);
}

static void mov_rax_xmm1()
{
    emit_bytes(0x66, 0x48);
    emit_bytes(0x0f, 0x6e);
    emit_byte(0xc8);
}

static void mov_xmm0_rax()
{
    emit_bytes(0x66, 0x48);
    emit_bytes(0x0f, 0x7e);
    emit_byte(0xc0);
}

static void push_eax()
{
    // sub $0x4, %rsp
    emit_bytes(0x48, 0x83);
    emit_bytes(0xec, 0x04);
    // mov %eax, (%rsp)
    emit_bytes(0x89, 0x04);
    emit_byte(0x24);
}

static void push_rax()
{
    emit_byte(0x50);
}

static void pop_rax()
{
    emit_byte(0x58);
}

static void push_u64(uint64_t u64)
{
  Raw raw = {0};
  raw.u64[0] = u64;
  // mov $0xXXXX, %eax
  emit_byte(0xb8);
  emit_dword(raw.u32[1]);
  push_eax();
  emit_byte(0xb8);
  emit_dword(raw.u32[0]);
  push_eax();
}

Token previous;
Token current;

static Token advance()
{
    previous = current;
    current = scan_token();
    return current;
}

static void consume(Token_Type type)
{
    ASSERT(current.type == type && "wrong token");
    advance();
}

static void number()
{
    double value = strtod(previous.start, NULL);
    Raw raw = {0};
    raw.f64[0] = value;
    push_u64(raw.u64[0]);
}

static void exponent()
{
  push_u64((uint64_t)pow);
  pop_rax();
  emit_bytes(0xff, 0xd0);
}

static void binary()
{
    Token_Type optype = previous.type;
    Parse_Rule* rule = get_rule(optype);
    parse_precedence((Precedence)(rule->precedence + 1));

    pop_rax();
    mov_rax_xmm1();
    pop_rax();
    mov_rax_xmm0();
    switch (optype) {
    case TOKEN_PLUS:  emit_bytes(0xf2, 0x0f, 0x58, 0xc1); break;
    case TOKEN_MINUS: emit_bytes(0xf2, 0x0f, 0x5c, 0xc1); break;
    case TOKEN_STAR:  emit_bytes(0xf2, 0x0f, 0x59, 0xc1); break;
    case TOKEN_SLASH: emit_bytes(0xf2, 0x0f, 0x5e, 0xc1); break;
    case TOKEN_CARET: exponent(); break;
    default: break;
    }
    mov_xmm0_rax();
    push_rax();
}

static void neg() 
{
  Raw raw = {0};
  raw.f64[0] = -1;
  push_u64(raw.u64[0]);
  pop_rax();
  mov_rax_xmm1();
  emit_bytes(0xf2, 0x0f, 0x59, 0xc1);
}

static void unary() 
{
    Token_Type optype = previous.type;
    Parse_Rule* rule = get_rule(optype);
    parse_precedence((Precedence)(rule->precedence) + 1);

    pop_rax();
    mov_rax_xmm0();
    switch (optype) {
    case TOKEN_PLUS:  break;
    case TOKEN_MINUS: neg(); break;
    default: break;
    }
    mov_xmm0_rax();
    push_rax();
}

static void group()
{
    parse_precedence(PREC_TERM);
    consume(TOKEN_RPAREN);
}

Parse_Rule rules[] = {
    [TOKEN_PLUS]    = {unary,   binary, PREC_TERM},
    [TOKEN_MINUS]   = {unary,   binary, PREC_TERM},
    [TOKEN_STAR]    = {NULL,    binary, PREC_FACTOR},
    [TOKEN_SLASH]   = {NULL,    binary, PREC_FACTOR},
    [TOKEN_LPAREN]  = {group,   NULL,   PREC_NONE},
    [TOKEN_RPAREN]  = {NULL,    NULL,   PREC_NONE},
    [TOKEN_CARET]   = {NULL,    binary, PREC_EXPONENT},
    [TOKEN_NUMBER]  = {number,  NULL,   PREC_NONE},
    [TOKEN_EOF]     = {NULL,    NULL,   PREC_NONE},
};

static Parse_Rule *get_rule(Token_Type type)
{
    return &rules[type];
}

static void parse_precedence(Precedence precedence)
{
    advance();
    parse_fn prefix_rule = get_rule(previous.type)->prefix;
    ASSERT(prefix_rule != NULL && "Expect expression");

    prefix_rule();
    while (precedence <= get_rule(current.type)->precedence) {
        advance();
        parse_fn infix_rule = get_rule(previous.type)->infix;
        infix_rule();
    }
}

static void expression()
{
    parse_precedence(PREC_TERM);
    pop_rax();
    mov_rax_xmm0();
    ret();
}

Code *compile(const char *source)
{
    code = malloc(sizeof(Code));
    code->items = NULL;
    code->count = 0;
    code->capacity = 0;
    init_scanner(source);
    current = scan_token();
    expression();
    return code;
}
