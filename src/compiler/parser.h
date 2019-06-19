#ifndef WRENCH_PARSER
#define WRENCH_PARSER 1


#include <stdbool.h>
#include <wrench/vector.h>
#include "token_stream.h"


typedef enum {
    FN_PARAM_STRING,
    FN_PARAM_SYMBOL,
    FN_PARAM_FUNCTION_CALL,
    FN_PARAM_CLOSURE
} FunctionParamType;

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
    FunctionCallVec fn_calls;
} Closure;

struct FunctionParam {
    FunctionParamType type;
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
    FunctionCallVec *function_stack;
    Token prev;
    FunctionCallVec *fn_calls;
    TokenTypeVec expectations;
} ParsedFile;


ParsedFile *parsed_file_new(void);

ParserStatus parsed_file_append(ParsedFile *file, const Token *token);

void parsed_file_free(ParsedFile *file);


#endif
