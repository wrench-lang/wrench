#include <string.h>
#include <stdbool.h>
#include <wrench/wrench.h>
#include "parser.h"


struct ParsedFile {
    FunctionCallVec *function_stack;
    Token prev;
    FunctionCallVec *parsed;
};

static void copy_token_value(wchar_t **dest, const Token *token)
{
    size_t size = token->value.length * sizeof(wchar_t);

    *dest = wrench_allocate(size + sizeof(wchar_t));

    wcsncpy(*dest, token->value.data, token->value.length);
}

static inline FunctionCall *current_function_call(const ParsedFile *file)
{
    if (0 == file->function_stack->length) return NULL;

    return &wrench_vec_last(file->function_stack);
}

static void add_new_parsing_function(ParsedFile *file, const Token *token)
{
    FunctionCall call = {.in_param_list = false};
    wrench_vec_init(&call.params);
    wrench_vec_init(&call.expectations);
    wrench_vec_push(&call.expectations, T_PAREN_LEFT);

    copy_token_value(&call.name, token);

    wrench_vec_push(file->function_stack, call);
}

static void check_expectations(const ParsedFile *file, const Token *token)
{
    const FunctionCall *call = current_function_call(file);
    TokenType type;
    uint32_t i;

    wrench_vec_foreach(&call->expectations, type, i) {
        if (type == token->type) {
            return;
        }
    }

    wrench_fatalf("Unexpected token: %ls\n", token->value.data);
}

static void convert_symbol_to_function_call(ParsedFile *file, FunctionCall *call)
{
    FunctionParam *prev_param = &wrench_vec_last(&call->params);
    prev_param->type = FN_PARAM_FUNCTION_CALL;

    add_new_parsing_function(file, &file->prev);
    wrench_deallocate(prev_param->val.str);

    FunctionCall *new = current_function_call(file);
    new->in_param_list = true;
    prev_param->val.fn_call = *new;
}

static void apply_token_to_param_list(ParsedFile *file, FunctionCall *call, const Token *token)
{
    FunctionParam param;

    switch (token->type) {
        case T_STRING:
            param.type = FN_PARAM_STRING;
            break;
        case T_SYMBOL:
            param.type = FN_PARAM_SYMBOL;
            break;
        case T_PAREN_LEFT:
            if (T_SYMBOL == file->prev.type) {
                convert_symbol_to_function_call(file, call);
                return;
            }
            param.type = FN_PARAM_SYMBOL;
    }
    switch (token->type) {
        case T_STRING:
        case T_SYMBOL:
            copy_token_value(&param.val.str, token);
            wrench_vec_push(&call->params, param);
            break;
    }
}

static void apply_token(ParsedFile *file, const Token *token)
{
    FunctionCall *call = current_function_call(file);

    if (T_PAREN_LEFT == token->type && !call->in_param_list) {
        call->in_param_list = true;
        return;
    }

    if (T_PAREN_RIGHT == token->type && call->in_param_list) {
        call->in_param_list = false;

        FunctionCall top = wrench_vec_pop(file->function_stack);

        if (0 == file->function_stack->length) {
            wrench_vec_push(file->parsed, top);
        }
        return;
    }

    if (call->in_param_list) {
        apply_token_to_param_list(file, call, token);
    }
}


#define add_expectations(vec, count, ...) \
    wrench_vec_push_array(vec, ((TokenType [count])__VA_ARGS__), count)


static void set_expectation(ParsedFile *file, const Token *token)
{
    FunctionCall *call = current_function_call(file);

    if (!call) return;

    TokenTypeVec *expectations = &call->expectations;
    wrench_vec_reset(expectations);

    if (call->in_param_list) {
        switch (token->type) {
            case T_PAREN_LEFT:
            case T_STRING:
            case T_SYMBOL:
                add_expectations(expectations, 4, {T_STRING, T_COMMA, T_PAREN_RIGHT, T_PAREN_LEFT});
                break;
            case T_COMMA:
                add_expectations(expectations, 3, {T_STRING, T_SYMBOL});
                break;
            case T_PAREN_RIGHT:
                add_expectations(expectations, 2, {T_PAREN_RIGHT, T_COMMA});
                break;
        }
    } else {
        switch (token->type) {
            case T_PAREN_RIGHT:
                add_expectations(expectations, 2, {T_SYMBOL, T_PAREN_RIGHT});
                break;
            }
    }
}

void parsed_file_append(ParsedFile *file, const Token *token)
{
    FunctionCallVec *function_stack = file->function_stack;

    if (0 != function_stack->length) {
         check_expectations(file, token);
         apply_token(file, token);
         set_expectation(file, token);
    } else {
        if (T_SYMBOL != token->type) {
            wrench_fatal("Expected T_SYMBOL");
        }
        add_new_parsing_function(file, token);
    }
    file->prev = *token;
}

ParsedFile *parsed_file_new(void)
{
    ParsedFile *file = wrench_allocate(sizeof(ParsedFile));
    file->prev = (Token) {.type = T_EOF};
    file->function_stack = wrench_allocate(sizeof(FunctionCallVec));
    wrench_vec_init(file->function_stack);

    file->parsed = wrench_allocate(sizeof(FunctionCallVec));
    wrench_vec_init(file->parsed);

    return file;
}

const FunctionCallVec *parsed_file_get_fn_calls(ParsedFile *file)
{
    return file->parsed;
}

static void free_function_call(const FunctionCall *call)
{
    wrench_vec_deinit(&call->expectations);

    uint32_t i;
    FunctionParam param;

    wrench_vec_foreach(&call->params, param, i) {
        switch (param.type) {
            case FN_PARAM_FUNCTION_CALL:
                free_function_call(&param.val.fn_call);
                break;
            case FN_PARAM_STRING:
                wrench_deallocate(param.val.str);
                break;
        }
    }

    wrench_vec_deinit(&call->params);
    wrench_deallocate(call->name);
}

void parsed_file_free(ParsedFile *file)
{
    wrench_vec_deinit(file->function_stack);
    wrench_deallocate(file->function_stack);

    uint32_t i;
    FunctionCall call;
    wrench_vec_foreach(file->parsed, call, i) {
        free_function_call(&call);
    }

    wrench_vec_deinit(file->parsed);
    wrench_deallocate(file->parsed);

    wrench_deallocate(file);
}

