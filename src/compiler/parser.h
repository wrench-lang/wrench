#ifndef WRENCH_PARSER
#define WRENCH_PARSER 1


#include <stdbool.h>
#include <wrench/vector.h>
#include "token_stream.h"


typedef enum {
    FN_CALL_PARAM_TYPE_STRING,
    FN_CALL_PARAM_TYPE_INT,
    FN_CALL_PARAM_TYPE_SYMBOL,
    FN_CALL_PARAM_TYPE_FUNCTION_CALL,
    FN_CALL_PARAM_TYPE_CLOSURE
} FnCallParamType;

typedef vec_type(TokenType) TokenTypeVec;

typedef struct FunctionCall FunctionCall;

typedef vec_type(FunctionCall) FunctionCallVec;

typedef struct FunctionParam FunctionParam;

typedef vec_type(FunctionParam) FunctionParamVec;

struct FunctionCall {
    wchar_t *name;
    FunctionParamVec params;
    struct {
        bool param_list;
        bool closure_param_list;
        bool closure_body;
    } ctx;
};

typedef struct {
    FnCallParamType type;
    wchar_t *name;
} ClosureParam;

typedef vec_type(ClosureParam) ClosureParamVec;

typedef struct {
    ClosureParamVec params;
    FunctionCallVec fn_calls;
} Closure;

struct FunctionParam {
    FnCallParamType type;
    union {
        FunctionCall fn_call;
        wchar_t *str;
        Closure closure;
    } val;
};

typedef enum {
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN
} ParserStatus;


typedef struct {
    FunctionCallVec function_stack;
    Token prev;
    FunctionCallVec fn_calls;
    TokenTypeVec expectations;
} ParsedFile;


ParsedFile *parsed_file_new(void);

ParserStatus parsed_file_append(ParsedFile *file, const Token *token);

void parsed_file_free(ParsedFile *file);


#endif
