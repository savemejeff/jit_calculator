#ifndef SCANNER_H

typedef enum {
    // single char tokens
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_CARET,

    // literals
    TOKEN_NUMBER,

    TOKEN_EOF
} Token_Type;

typedef struct {
    Token_Type type;
    const char *start;
    int length;
    int line;
} Token;

void init_scanner(const char *source);
Token scan_token();

#endif // SCANNER_H
