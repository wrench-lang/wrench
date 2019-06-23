#include "../../test.h"
#include "../src/compiler/parser.h"

#include <stdlib.h>

CTEST(parser, function_call)
{
    ParsedFile *file = parsed_file_new();

    WcharVec val_print;
    vec_init(&val_print);

    vec_push(&val_print, L'p');
    vec_push(&val_print, L'r');
    vec_push(&val_print, L'i');
    vec_push(&val_print, L'n');
    vec_push(&val_print, L't');

    WcharVec val_paren_left;
    vec_init(&val_paren_left);
    vec_push(&val_paren_left, CHR_PAREN_LEFT);

    WcharVec val_str;
    vec_init(&val_str);
    vec_push(&val_str, L'\'');
    vec_push(&val_str, L'H');
    vec_push(&val_str, L'e');
    vec_push(&val_str, L'l');
    vec_push(&val_str, L'l');
    vec_push(&val_str, L'o');
    vec_push(&val_str, L'\'');

    WcharVec val_comma;
    vec_init(&val_comma);
    vec_push(&val_comma, CHR_COMMA);

    WcharVec val_sym;
    vec_init(&val_sym);
    vec_push(&val_sym, L't');
    vec_push(&val_sym, L'e');
    vec_push(&val_sym, L's');
    vec_push(&val_sym, L't');

    WcharVec val_paren_left_2;
    vec_init(&val_paren_left_2);
    vec_push(&val_paren_left_2, CHR_PAREN_LEFT);

    WcharVec val_paren_right;
    vec_init(&val_paren_right);
    vec_push(&val_paren_right, CHR_PAREN_RIGHT);

    WcharVec val_paren_right2;
    vec_init(&val_paren_right2);
    vec_push(&val_paren_right2, CHR_PAREN_RIGHT);

    Token print = {.type = T_SYMBOL, .value = val_print};
    Token paren_left = {.type = T_PAREN_LEFT, .value = val_paren_left};
    Token str = {.type = T_STRING, .value = val_str};
    Token comma = {.type = T_COMMA, .value = val_comma};
    Token symbol = {.type = T_SYMBOL, .value = val_sym};
    Token paren_left_2 = {.type = T_PAREN_LEFT, .value = val_paren_left_2};
    Token paren_right = {.type = T_PAREN_RIGHT, .value = val_paren_right};
    Token paren_right2 = {.type = T_PAREN_RIGHT, .value = val_paren_right2};

    /* // print('Hello', test()) */
    ASSERT_EQUAL(PARSER_OK, parsed_file_append(file, &print));
    ASSERT_EQUAL(PARSER_OK, parsed_file_append(file, &paren_left));
    ASSERT_EQUAL(PARSER_OK, parsed_file_append(file, &str));
    ASSERT_EQUAL(PARSER_OK, parsed_file_append(file, &comma));
    ASSERT_EQUAL(PARSER_OK, parsed_file_append(file, &symbol));
    ASSERT_EQUAL(PARSER_OK, parsed_file_append(file, &paren_left_2));
    ASSERT_EQUAL(PARSER_OK, parsed_file_append(file, &paren_right));
    ASSERT_EQUAL(PARSER_OK, parsed_file_append(file, &paren_right2));

    const FunctionCallVec *calls = &file->fn_calls;
    FunctionCall call = calls->data[0];

    ASSERT_WSTR(L"print", call.name);
    ASSERT_EQUAL(2, call.params.length);

    ASSERT_EQUAL(PARAM_TYPE_STRING, call.params.data[0].type);
    ASSERT_WSTR(L"'Hello'", call.params.data[0].val.str);

    ASSERT_EQUAL(PARAM_TYPE_FUNCTION_CALL, call.params.data[1].type);
    ASSERT_WSTR(L"test", call.params.data[1].val.str);

    parsed_file_free(file);
}
