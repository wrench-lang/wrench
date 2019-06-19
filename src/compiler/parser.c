#include <string.h>
#include <stdbool.h>
#include <wrench/wrench.h>
#include "parser.h"


static void copy_token_value(wchar_t **dest, const Token *token)
{
    size_t size = token->value.length * sizeof(wchar_t);

    *dest = wrench_allocate(size + sizeof(wchar_t));

    wcsncpy(*dest, token->value.data, token->value.length);
}

static inline FunctionCall *current_function_call(const ParsedFile *file)
{
    if (0 == file->function_stack->length) return NULL;

    return &vec_last(file->function_stack);
}

static void add_new_parsing_function(ParsedFile *file, const Token *token)
{
    FunctionCall call = {
        .ctx = {
            .param_list = false,
            .closure_param_list = false,
            .closure_body = false
        }
    };
    vec_init(&call.params);

    copy_token_value(&call.name, token);

    vec_push(file->function_stack, call);
}

static bool meets_expectations(const ParsedFile *file, const Token *token)
{
    TokenType type;
    uint32_t i;

    vec_foreach(&file->expectations, type, i) {
        if (type == token->type) {
            return true;
        }
    }

    return false;
}

static void create_param_function_call(ParsedFile *file, FunctionCall *call)
{
    FunctionParam *prev_param = &vec_last(&call->params);
    prev_param->type = FN_PARAM_FUNCTION_CALL;

    add_new_parsing_function(file, &file->prev);
    wrench_free(prev_param->val.str);

    FunctionCall *new = current_function_call(file);
    new->ctx.param_list = true;
    prev_param->val.fn_call = *new;
}

static void change_closure_context(ParsedFile *file, const Token *token)
{
    FunctionCall *call = current_function_call(file);
    TokenType type = token->type, prev_type = file->prev.type;

    if (T_POINTY_LEFT == type) {
        call->ctx.closure_param_list = true;
    } else if (T_COLON == prev_type && T_SYMBOL == type) {
        call->ctx.closure_param_list = false;
        call->ctx.closure_body = true;
    } else if (T_SQUARE_LEFT == type) {
        call->ctx.closure_body = false;
    }
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
                if (call->ctx.closure_body) {
                    add_new_parsing_function(file, &file->prev);
                } else {
                    create_param_function_call(file, call);
                }
            }
            param.type = FN_PARAM_SYMBOL;
            break;
        case T_SQUARE_LEFT:
            param.type = FN_PARAM_CLOSURE;
            break;
    }

    switch (token->type) {
        case T_STRING:
        case T_SYMBOL:
            if (!call->ctx.closure_body && !call->ctx.closure_param_list) {
                copy_token_value(&param.val.str, token);
                vec_push(&call->params, param);
            }
             // TODO add fn call to closure
            break;
        case T_SQUARE_LEFT:
            /* vec_init(&param.val.closure.fn_calls); */
            break;
    }
}

static void apply_token(ParsedFile *file, const Token *token)
{
    FunctionCall *call = current_function_call(file);
    TokenType type = token->type;

    if (T_PAREN_LEFT == type && !call->ctx.param_list) {
        call->ctx.param_list = true;
        return;
    }

    if (T_PAREN_RIGHT == type && call->ctx.param_list) {
        call->ctx.param_list = false;

        FunctionCall top = vec_pop(file->function_stack);

        if (0 == file->function_stack->length) {
            vec_push(file->fn_calls, top);
        }
        return;
    }

    if (call->ctx.param_list) {
        apply_token_to_param_list(file, call, token);
    }
}


static bool in_closure_param(const ParsedFile *file)
{
    const FunctionCallVec *stack = file->function_stack;

    if (2 > stack->length) return false;

    const FunctionCall *prev_call = &stack->data[stack->length -2];

    return prev_call->ctx.closure_body;
}

#define add_expectations(vec, count, ...) \
    vec_push_array(vec, ((TokenType [count])__VA_ARGS__), count)


static void add_symbol_expectations(TokenTypeVec *expectations, const FunctionCall *call, TokenType prev_type)
{
    if (call->ctx.closure_param_list) {
        if (T_SYMBOL == prev_type) {
            add_expectations(expectations, 2, {T_COMMA, T_POINTY_RIGHT});
        } else if (T_COLON == prev_type) {
            add_expectations(expectations, 1, {T_SYMBOL});
        } else {
            add_expectations(expectations, 1, {T_SYMBOL});
        }
    } else {
        add_expectations(expectations, 3, {T_COMMA, T_PAREN_RIGHT, T_PAREN_LEFT});
    }
}

static void set_expectation(ParsedFile *file, const Token *token)
{
    FunctionCall *call = current_function_call(file);

    if (!call) return;

    TokenTypeVec *expectations = &file->expectations;
    vec_reset(expectations);

    TokenType prev_type = file->prev.type;

    switch (token->type) {
        case T_PAREN_LEFT:
            add_expectations(expectations, 4, {T_STRING, T_PAREN_RIGHT, T_STRING, T_SQUARE_LEFT});
            break;
        case T_STRING:
            add_expectations(expectations, 2, {T_COMMA, T_PAREN_RIGHT});
            break;
        case T_SYMBOL:
            add_symbol_expectations(expectations, call, prev_type);
            break;
        case T_COMMA:
            add_expectations(expectations, 2, {T_STRING, T_SYMBOL});
            break;
        case T_PAREN_RIGHT:
            if (in_closure_param(file)) {
                /* exit(0); */
                add_expectations(expectations, 2, {T_SYMBOL, T_SQUARE_RIGHT});
            } else {
                add_expectations(expectations, 2, {T_PAREN_RIGHT, T_COMMA});
            }
            break;
        case T_SQUARE_LEFT:
            add_expectations(expectations, 2, {T_POINTY_LEFT});
            break;
        case T_SQUARE_RIGHT:
            add_expectations(expectations, 2, {T_COMMA, T_PAREN_RIGHT});
            break;
        case T_POINTY_LEFT:
            add_expectations(expectations, 2, {T_SYMBOL, T_PAREN_RIGHT});
            break;
        case T_POINTY_RIGHT:
            add_expectations(expectations, 1, {T_COLON});
            break;
        case T_COLON:
            add_expectations(expectations, 1, {T_SYMBOL});
            break;
    }
}

static void add_first_call(ParsedFile *file, const Token *token)
{
    add_new_parsing_function(file, token);
    vec_reset(&file->expectations);
    vec_push(&file->expectations, T_PAREN_LEFT);
}

ParserStatus parsed_file_append(ParsedFile *file, const Token *token)
{
    FunctionCallVec *function_stack = file->function_stack;

    if (0 != function_stack->length) {
         if (!meets_expectations(file, token)) {
             return PARSER_UNEXPECTED_TOKEN;
         }

         apply_token(file, token);
         set_expectation(file, token);
         change_closure_context(file, token);
    } else {
        if (T_SYMBOL != token->type) {
            return PARSER_UNEXPECTED_TOKEN;
        }
        add_first_call(file, token);
    }
    token_free(file->prev);
    file->prev = *token;

    return PARSER_OK;
}

ParsedFile *parsed_file_new(void)
{
    ParsedFile *file = wrench_allocate(sizeof(ParsedFile));
    file->prev = (Token) {.type = T_EOF};
    file->function_stack = wrench_allocate(sizeof(FunctionCallVec));

    vec_init(file->function_stack);
    vec_init(&file->expectations);

    file->fn_calls = wrench_allocate(sizeof(FunctionCallVec));
    vec_init(file->fn_calls);

    return file;
}

static void free_function_call(const FunctionCall *call)
{
    uint32_t i;
    FunctionParam param;

    vec_foreach(&call->params, param, i) {
        switch (param.type) {
            case FN_PARAM_FUNCTION_CALL:
                free_function_call(&param.val.fn_call);
                break;
            case FN_PARAM_STRING:
                wrench_free(param.val.str);
                break;
        }
    }

    vec_deinit(&call->params);
    wrench_free(call->name);
}

void parsed_file_free(ParsedFile *file)
{
    token_free(file->prev);

    uint32_t i;
    FunctionCall call;

    vec_foreach(file->fn_calls, call, i) {
        free_function_call(&call);
    }

    vec_deinit(file->fn_calls);
    wrench_free(file->fn_calls);

    vec_foreach(file->function_stack, call, i) {
        free_function_call(&call);
    }

    vec_deinit(&file->expectations);

    vec_deinit(file->function_stack);
    wrench_free(file->function_stack);

    wrench_free(file);
}

