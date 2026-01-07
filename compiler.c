#include "compiler.h"
#include "scanner.h"

#define INS_POPQ_RAX 0x58
#define INS_POPQ_RDI 0x5f
#define INS_PUSH_RAX 0x50
#define INS_RET      0xc3

typedef enum {
  PREC_NONE,
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
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

static void emit_byte(uint8_t byte)
{
    da_append(code, byte);
}

static void emit_bytes(uint8_t byte1, uint8_t byte2)
{
    emit_byte(byte1);
    emit_byte(byte2);
}

static void emit_dword(int32_t dword)
{
    emit_byte(0xff & dword);
    emit_byte(0xff & (dword >> 8));
    emit_byte(0xff & (dword >> 16));
    emit_byte(0xff & (dword >> 24));
}

Token previous;
Token current;

static Token advance() 
{
    previous = current;
    current = scan_token();
    return current;
}

static void integer()
{
    int value = strtol(previous.start, NULL, 10);
    emit_byte(0x68);
    emit_dword(value);
}

static void binary()
{
    Token_Type optype = previous.type;
    Parse_Rule* rule = get_rule(optype);
    parse_precedence((Precedence)(rule->precedence + 1));

    emit_bytes(INS_POPQ_RDI, INS_POPQ_RAX);
    switch (optype) {
    case TOKEN_PLUS:  emit_bytes(0x01, 0xf8); break;
    case TOKEN_MINUS: emit_bytes(0x29, 0xf8); break;
    case TOKEN_STAR:  emit_bytes(0xf7, 0xef); break;
    case TOKEN_SLASH: emit_byte(0x99); emit_bytes(0xf7, 0xff); break;
    default: break;
    }
    emit_byte(INS_PUSH_RAX);
}

Parse_Rule rules[] = {
    [TOKEN_PLUS]    = {NULL,    binary, PREC_TERM},
    [TOKEN_MINUS]   = {NULL,    binary, PREC_TERM},
    [TOKEN_STAR]    = {NULL,    binary, PREC_FACTOR},
    [TOKEN_SLASH]   = {NULL,    binary, PREC_FACTOR},
    [TOKEN_INTEGER] = {integer, NULL,   PREC_NONE},
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
    emit_bytes(INS_POPQ_RAX, INS_RET);
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