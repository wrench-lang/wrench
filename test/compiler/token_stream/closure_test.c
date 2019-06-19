#include "../../test.h"
#include "../src/compiler/token_stream.h"


CTEST(token_stream, closure)
{
    TokenStream *stream = token_stream_new(fixture("closure_parameter.wrnc"));

    Token token;
    token = token_stream_next(stream);

    ASSERT_EQUAL(T_SYMBOL, token.type);
    ASSERT_WSTR(L"main", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_PAREN_LEFT, token.type);
    ASSERT_WSTR(L"(", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_SQUARE_LEFT, token.type);
    ASSERT_WSTR(L"[", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_POINTY_LEFT, token.type);
    ASSERT_WSTR(L"<", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_SYMBOL, token.type);
    ASSERT_WSTR(L"Int", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_SYMBOL, token.type);
    ASSERT_WSTR(L"argc", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_POINTY_RIGHT, token.type);
    ASSERT_WSTR(L">", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_COLON, token.type);
    ASSERT_WSTR(L":", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_SYMBOL, token.type);
    ASSERT_WSTR(L"Void", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_SYMBOL, token.type);
    ASSERT_WSTR(L"hello", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_PAREN_LEFT, token.type);
    ASSERT_WSTR(L"(", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_PAREN_RIGHT, token.type);
    ASSERT_WSTR(L")", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_SQUARE_RIGHT, token.type);
    ASSERT_WSTR(L"]", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_PAREN_RIGHT, token.type);
    ASSERT_WSTR(L")", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_EOF, token.type);
    ASSERT_WSTR(NULL, token.value.data);
    token_free(token);

    token_stream_free(stream);
}
