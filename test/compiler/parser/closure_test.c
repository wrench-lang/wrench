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
        printf("%ls\n", token.value.data);
        ASSERT_EQUAL(PARSER_OK, parsed_file_append(file, &token));
        token = token_stream_next(stream);
    }

    ASSERT_EQUAL(1, file->fn_calls->length);
    const FunctionCall call = file->fn_calls->data[0];

    /* ASSERT_EQUAL(FN_PARAM_CLOSURE); */

    parsed_file_free(file);
    token_stream_free(stream);
}
