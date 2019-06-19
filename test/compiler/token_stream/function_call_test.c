#include "../../test.h"
#include "../src/compiler/token_stream.h"


CTEST(token_stream, empty_file)
{
    TokenStream *stream = token_stream_new(fixture("empty_file.wrnc"));

    Token token;
    token = token_stream_next(stream);

    ASSERT_EQUAL(T_EOF, token.type);
    ASSERT_WSTR(NULL, token.value.data);

    token_stream_free(stream);
}

CTEST(token_stream, function_call)
{
    TokenStream *stream = token_stream_new(fixture("fn_call.wrnc"));

    Token token;

    token = token_stream_next(stream);

    ASSERT_EQUAL(T_SYMBOL, token.type);
    ASSERT_WSTR(L"print", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_PAREN_LEFT, token.type);
    ASSERT_WSTR(L"(", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_STRING, token.type);
    ASSERT_WSTR(L"'Hello World'", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_COMMA, token.type);
    ASSERT_WSTR(L",", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_SYMBOL, token.type);
    ASSERT_WSTR(L"Test", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_PAREN_RIGHT, token.type);
    ASSERT_WSTR(L")", token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_EOF, token.type);
    ASSERT_WSTR(NULL, token.value.data);
    token_free(token);

    token = token_stream_next(stream);
    ASSERT_EQUAL(T_EOF, token.type);
    ASSERT_WSTR(NULL, token.value.data);
    token_free(token);

    token_stream_free(stream);
}

