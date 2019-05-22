#include "test.h"
#include "../src/compiler/parser.h"
#include <stdlib.h>


CTEST(parser, function_call)
{
    ParsedFile *file = parsed_file_new();

    WrenchWcharVec val_print;
    wrench_vec_init(&val_print);
    wrench_vec_push(&val_print, L'p');
    wrench_vec_push(&val_print, L'r');
    wrench_vec_push(&val_print, L'i');
    wrench_vec_push(&val_print, L'n');
    wrench_vec_push(&val_print, L't');

    WrenchWcharVec val_paren_left;
    wrench_vec_init(&val_paren_left);
    wrench_vec_push(&val_paren_left, CHR_PAREN_LEFT);

    WrenchWcharVec val_str;
    wrench_vec_init(&val_str);
    wrench_vec_push(&val_str, L'\'');
    wrench_vec_push(&val_str, L'H');
    wrench_vec_push(&val_str, L'e');
    wrench_vec_push(&val_str, L'l');
    wrench_vec_push(&val_str, L'l');
    wrench_vec_push(&val_str, L'o');
    wrench_vec_push(&val_str, L'\'');

    WrenchWcharVec val_comma;
    wrench_vec_init(&val_comma);
    wrench_vec_push(&val_comma, CHR_COMMA);

    WrenchWcharVec val_sym;
    wrench_vec_init(&val_sym);
    wrench_vec_push(&val_sym, L't');
    wrench_vec_push(&val_sym, L'e');
    wrench_vec_push(&val_sym, L's');
    wrench_vec_push(&val_sym, L't');

    WrenchWcharVec val_paren_right;
    wrench_vec_init(&val_paren_right);
    wrench_vec_push(&val_paren_right, CHR_PAREN_RIGHT);

    WrenchWcharVec val_paren_right2;
    wrench_vec_init(&val_paren_right2);
    wrench_vec_push(&val_paren_right2, CHR_PAREN_RIGHT);

    Token print = {.type = T_SYMBOL, .value = val_print};
    Token paren_left = {.type = T_PAREN_LEFT, .value = val_paren_left};
    Token str = {.type = T_STRING, .value = val_str};
    Token comma = {.type = T_COMMA, .value = val_comma};
    Token symbol = {.type = T_SYMBOL, .value = val_sym};
    Token paren_right = {.type = T_PAREN_RIGHT, .value = val_paren_right};
    Token paren_right2 = {.type = T_PAREN_RIGHT, .value = val_paren_right2};

    // print('Hello', test())
    parsed_file_append(file, &print);
    parsed_file_append(file, &paren_left);
    parsed_file_append(file, &str);
    parsed_file_append(file, &comma);
    parsed_file_append(file, &symbol);
    parsed_file_append(file, &paren_left);
    parsed_file_append(file, &paren_right);
    parsed_file_append(file, &paren_right2);

    const FunctionCallVec *calls = parsed_file_get_fn_calls(file);
    FunctionCall call = calls->data[0];

    ASSERT_WSTR(L"print", call.name);
    ASSERT_EQUAL(2, call.params.length);

    ASSERT_EQUAL(FN_PARAM_STRING, call.params.data[0].type);
    ASSERT_WSTR(L"'Hello'", call.params.data[0].val.str);

    ASSERT_EQUAL(FN_PARAM_FUNCTION_CALL, call.params.data[1].type);
    ASSERT_WSTR(L"test", call.params.data[1].val.str);

    token_free(print);
    token_free(paren_left);
    token_free(str);
    token_free(comma);
    token_free(symbol);
    token_free(paren_right);
    token_free(paren_right2);

    parsed_file_free(file);
}
