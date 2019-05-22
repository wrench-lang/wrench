#ifndef WRENCH_TOKEN_STREAM
#define WRENCH_TOKEN_STREAM 1


#include <stdio.h>
#include <stdbool.h>
#include <wrench/value.h>


typedef enum {
    T_PAREN_LEFT,
    T_PAREN_RIGHT,
    T_SQUARE_LEFT,
    T_SQUARE_RIGHT,
    T_POINTY_LEFT,
    T_POINTY_RIGHT,
    T_COMMA,
    T_STRING,
    T_SYMBOL,
    T_EOF
} TokenType;

#define CHR_PAREN_LEFT      L'('
#define CHR_PAREN_RIGHT     L')'
#define CHR_SQUARE_LEFT     L'['
#define CHR_SQUARE_RIGHT    L']'
#define CHR_POINTY_LEFT     L'<'
#define CHR_POINTY_RIGHT    L'>'
#define CHR_COMMA           L','
#define CHR_APOSTHROPE      L'\''
#define CHR_NEW_LINE        L'\n'
#define CHR_SPACE           L' '

typedef struct {
    TokenType type;
    WrenchWcharVec value;
} Token;

typedef struct TokenStream TokenStream;


TokenStream *token_stream_new(const char *path);

Token token_stream_next(TokenStream *stream);

void token_stream_free(TokenStream *stream);

void token_free(Token token);


#endif
