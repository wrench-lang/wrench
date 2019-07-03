#include "../../test.h"
#include "../src/compiler/parser.h"


CTEST_SKIP(parser, complex_closure)
{
}

CTEST(parser, closure)
{
    TokenStream *stream = token_stream_new(fixture("closure_parameter.wrnc"));
    ParsedFile *file = parsed_file_new();
    Token token = token_stream_next(stream);

    while (token.value.data) {
        ASSERT_EQUAL(PARSER_OK, parsed_file_append(file, &token));
        token = token_stream_next(stream);
    }

    ASSERT_EQUAL(1, file->fn_calls.length);
    const FunctionCall call = file->fn_calls.data[0];

    ASSERT_EQUAL(1, call.params.length);
    const FunctionParam param = call.params.data[0];

    ASSERT_EQUAL(FN_CALL_PARAM_TYPE_CLOSURE, param.type);

    const Closure closure = param.val.closure;
    ASSERT_EQUAL(1, closure.fn_calls.length);
    ASSERT_EQUAL(1, closure.params.length);

    ClosureParam closure_param = closure.params.data[0];
    ASSERT_WSTR(L"argc", closure_param.name);
    ASSERT_EQUAL(FN_CALL_PARAM_TYPE_INT, closure_param.type);

    const FunctionCall inner_call = closure.fn_calls.data[0];
    ASSERT_EQUAL(0, inner_call.params.length);
    ASSERT_WSTR(L"hello", inner_call.name);

    parsed_file_free(file);
    token_stream_free(stream);
}
