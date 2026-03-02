#include "compiler.h"
#include "scanner.h"

typedef enum {
  PREC_NONE,
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_EXPONENT,    // ^
  PREC_PRIMARY
} Precedence;

typedef enum {
    ASSOC_LEFT,
    ASSOC_RIGHT
} Associativity;

typedef void (*parse_fn)();

typedef struct {
    parse_fn prefix;
    parse_fn infix;
    Precedence precedence;
    Associativity associativity;
} Parse_Rule;

Code *code;

static void parse_precedence(Precedence precedence);
static Parse_Rule *get_rule(Token_Type type);

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
    immediate(*(uint64_t *)&value);
    push();
}

static void binary()
{
    Token_Type optype = previous.type;
    Parse_Rule* rule = get_rule(optype);
    parse_precedence((Precedence)(rule->precedence + 1));

    pop2();
    pop();
    switch (optype) {
    case TOKEN_PLUS:  add(); break;
    case TOKEN_MINUS: sub(); break;
    case TOKEN_STAR:  mul(); break;
    case TOKEN_SLASH: idiv(); break;
    case TOKEN_CARET: exponent(); break;
    default: break;
    }
    push();
}

static void unary()
{
    Token_Type optype = previous.type;
    Parse_Rule* rule = get_rule(optype);
    parse_precedence((Precedence)(rule->precedence) + 1);

    pop();
    switch (optype) {
    case TOKEN_PLUS:  break;
    case TOKEN_MINUS: neg(); break;
    default: break;
    }
    push();
}

static void group()
{
    parse_precedence(PREC_TERM);
    consume(TOKEN_RPAREN);
}

Parse_Rule rules[] = {
    [TOKEN_PLUS]   = {unary,  binary, PREC_TERM,     ASSOC_LEFT},
    [TOKEN_MINUS]  = {unary,  binary, PREC_TERM,     ASSOC_LEFT},
    [TOKEN_STAR]   = {NULL,   binary, PREC_FACTOR,   ASSOC_LEFT},
    [TOKEN_SLASH]  = {NULL,   binary, PREC_FACTOR,   ASSOC_LEFT},
    [TOKEN_LPAREN] = {group,  NULL,   PREC_NONE,     ASSOC_LEFT},
    [TOKEN_RPAREN] = {NULL,   NULL,   PREC_NONE,     ASSOC_LEFT},
    [TOKEN_CARET]  = {NULL,   binary, PREC_EXPONENT, ASSOC_RIGHT},
    [TOKEN_NUMBER] = {number, NULL,   PREC_NONE,     ASSOC_LEFT},
    [TOKEN_EOF]    = {NULL,   NULL,   PREC_NONE,     ASSOC_LEFT},
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

    Parse_Rule *rule = get_rule(current.type);
    while (precedence <= rule->precedence ||
    (precedence == rule->precedence + 1 &&
    rule->associativity == ASSOC_RIGHT)) {
        advance();
        parse_fn infix_rule = get_rule(previous.type)->infix;
        infix_rule();
        rule = get_rule(current.type);
    }
}

static void expression()
{
    preamble();
    parse_precedence(PREC_TERM);
    postamble();
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
