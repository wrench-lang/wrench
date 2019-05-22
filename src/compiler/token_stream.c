#include <stdio.h>
#include <wrench/wrench.h>
#include "token_stream.h"


struct TokenStream {
    FILE *file;
    bool ended;
};


TokenStream *token_stream_new(const char *path)
{
    TokenStream *stream = wrench_allocate(sizeof(TokenStream));
    stream->file = fopen(path, "r");
    stream->ended = false;

    if (!stream->file) {
        wrench_fatalf("Unable to open file '%s'", path);
    }

    return stream;
}

static TokenType get_single_char_token_type(wchar_t chr, const WrenchWcharVec *val)
{
    switch(chr) {
        case CHR_PAREN_LEFT:
            return T_PAREN_LEFT;
        case CHR_PAREN_RIGHT:
            return T_PAREN_RIGHT;
        case CHR_POINTY_LEFT:
            return T_POINTY_LEFT;
        case CHR_POINTY_RIGHT:
            return T_PAREN_RIGHT;
        case CHR_SQUARE_LEFT:
            return T_SQUARE_LEFT;
        case CHR_SQUARE_RIGHT:
            return T_SQUARE_RIGHT;
        case CHR_COMMA:
            return T_COMMA;
        case CHR_APOSTHROPE:
            return T_STRING;
    }

    if (0 != val->length && CHR_APOSTHROPE == val->data[0]) {
        return T_STRING;
    }

    return T_SYMBOL;
}

static TokenType get_multi_char_token_type(const WrenchWcharVec *val)
{
    //@todo Integer, Float

    int last_offs = (int) val->length - 1;

    if (CHR_APOSTHROPE == val->data[0] && CHR_APOSTHROPE == val->data[last_offs]) {
        return T_STRING;
    }

    if (CHR_APOSTHROPE != val->data[0] && CHR_APOSTHROPE != val->data[last_offs]) {
        return T_SYMBOL;
    }

    wrench_fatalf("Invalid string %ls", val->data);
}

Token token_stream_next(TokenStream *stream)
{
    WrenchWcharVec val;
    wrench_vec_init(&val);

    wchar_t chr;
    TokenType type;

    while (WEOF != (wint_t) (chr = (wchar_t) fgetwc(stream->file))) {
        type = get_single_char_token_type(chr, &val);

        if (T_STRING != type) {
            if (CHR_NEW_LINE == chr || CHR_SPACE == chr)  {
                continue;
            }
        }

        if (T_STRING == type || T_SYMBOL == type) {
            wrench_vec_push(&val, chr);
        } else {
            if (0 != val.length) {
                ungetwc((wint_t) chr, stream->file);
                type = get_multi_char_token_type(&val);

                if (T_SYMBOL == type || T_STRING == type) {
                    wrench_vec_push(&val, '\0');
                }
                return (Token) {.type = type, .value = val};
            } else {
                wrench_vec_push(&val, chr);
                wrench_vec_push(&val, '\0');

                return (Token) {.type = type, .value = val};
            }
        }
    }

    return (Token) {.type = T_EOF, .value = val};
}

void token_free(Token token)
{
    wrench_vec_deinit(&token.value);
}

void token_stream_free(TokenStream *stream)
{
    fclose(stream->file);
    wrench_deallocate((void *) stream);
}
