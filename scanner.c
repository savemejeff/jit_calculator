#include "common.h"
#include "misc.h"
#include "scanner.h"

typedef struct {
    const char *start;
    const char *current;
    int line;
} Scanner;

Scanner scanner;

bool is_at_end()
{
    return *(scanner.current) == '\0';
}

char peek()
{
    return *(scanner.current);
}

char advance()
{
    (scanner.current) += 1;
    return (scanner.current)[-1];
}

Token make_token(Token_Type type)
{
    Token token;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    token.type = type;
    return token;
}

void skip_whitespaces()
{
    for (;;) {
        char c = peek();
        switch (c) {
        case '\n':
            scanner.line += 1;
            FALLTHROUGH;
        case '\r':
        case ' ':
        case '\t':
            advance();
            break;
        default:
            return;
        }
    }
}

Token number()
{
    while (!is_at_end() && isdigit(peek())) {
        advance();
    }
    if (!is_at_end() && peek() == '.') {
        advance();
        while (!is_at_end() && isdigit(peek())) {
            advance();
        }
    }
    return make_token(TOKEN_NUMBER);
}

void init_scanner(const char *source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

Token scan_token()
{
    skip_whitespaces();
    scanner.start = scanner.current;

    if (is_at_end()) {
        return make_token(TOKEN_EOF);
    }

    char c = advance();
    if (isdigit(c)) {
        return number();
    }
    switch (c) {
    case '+': return make_token(TOKEN_PLUS);
    case '-': return make_token(TOKEN_MINUS);
    case '*': return make_token(TOKEN_STAR);
    case '/': return make_token(TOKEN_SLASH);
    case '(': return make_token(TOKEN_LPAREN);
    case ')': return make_token(TOKEN_RPAREN);
    }
    ASSERT("unexpected character");
    return make_token(TOKEN_EOF); // make compiler happy
}