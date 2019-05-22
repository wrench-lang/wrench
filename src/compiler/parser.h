#ifndef WRENCH_PARSER
#define WRENCH_PARSER 1


#include <stdbool.h>
#include <wrench/vector.h>
#include "token_stream.h"


typedef enum {
    FN_PARAM_STRING,
    FN_PARAM_SYMBOL,
    FN_PARAM_FUNCTION_CALL
} FunctionParamType;

typedef wrench_vec_type(TokenType) TokenTypeVec;

typedef struct FunctionCall FunctionCall;

typedef struct FunctionParam FunctionParam;

typedef wrench_vec_type(FunctionParam) FunctionParamVec;

struct FunctionCall {
    wchar_t *name;
    FunctionParamVec params;
    bool in_param_list;
    TokenTypeVec expectations;
};

struct FunctionParam {
    FunctionParamType type;
    union {
        FunctionCall fn_call;
        wchar_t *str;
    } val;
};

typedef wrench_vec_type(FunctionCall) FunctionCallVec;

typedef struct ParsedFile ParsedFile;


ParsedFile *parsed_file_new(void);

void parsed_file_append(ParsedFile *file, const Token *token);

const FunctionCallVec *parsed_file_get_fn_calls(ParsedFile *file);

void parsed_file_free(ParsedFile *file);


#endif
