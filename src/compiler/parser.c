#include <string.h>
#include <stdbool.h>
#include <wrench/wrench.h>
#include "parser.h"


static void copy_token_value(wchar_t **dest, const Token *token)
{
    size_t size = (token->value.length + 1) * sizeof(wchar_t);
    uint32_t length = token->value.length;

    *dest = wrench_allocate(size);

    wcsncpy(*dest, token->value.data, length);
    (*dest)[length] = '\0';
}

static inline FunctionCall *current_function_call(const ParsedFile *file)
{
    if (0 == file->function_stack.length) return NULL;

    return &vec_last(&file->function_stack);
}

static inline FunctionParam *current_param(const ParsedFile *file)
{
    const FunctionCall *call = current_function_call(file);

    if (!call) return NULL;

    return &vec_last(&call->params);
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

    vec_push(&file->function_stack, call);
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

static void add_new_function_call_param(ParsedFile *file, FunctionCall *call)
{
    if (0 != call->params.length) {
        FunctionParam *prev_param = &vec_last(&call->params);
        prev_param->type = PARAM_TYPE_FUNCTION_CALL;

        add_new_parsing_function(file, &file->prev);
        wrench_free(prev_param->val.str);

        FunctionCall *new = current_function_call(file);
        new->ctx.param_list = true;
        prev_param->val.fn_call = *new;
    }
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

static void add_new_closure_function_call(ParsedFile *file, const Token *token)
{
    FunctionParam *containing_param = current_param(file);

    add_new_parsing_function(file, token);

    vec_push(&containing_param->val.closure.fn_calls, *current_function_call(file));
}

static void add_new_closure_param(const ParsedFile *file, const Token *token, Closure *closure)
{
    ClosureParam param;
    copy_token_value(&param.name, token);

    const wchar_t *type_name = file->prev.value.data;

    if (0 == wcscmp(L"Int", type_name)) {
        param.type = PARAM_TYPE_INT;
    } else if (0 == wcscmp(L"String", type_name)) {
        param.type = PARAM_TYPE_STRING;
    } else {
        // TODO Custom type check
    }

    vec_push(&closure->params, param);
}

static void apply_token_to_param_list(ParsedFile *file, FunctionCall *call, const Token *token)
{
    FunctionParam new_param;

    switch (token->type) {
        case T_STRING:
            new_param.type = PARAM_TYPE_STRING;
            copy_token_value(&new_param.val.str, token);
            vec_push(&call->params, new_param);
            break;
        case T_SYMBOL:

            if (!call->ctx.closure_body && !call->ctx.closure_param_list) {
                copy_token_value(&new_param.val.str, token);
                vec_push(&call->params, new_param);
            } else if (call->ctx.closure_param_list && T_SYMBOL == file->prev.type) {
                Closure *closure = &vec_last(&call->params).val.closure;

                add_new_closure_param(file, token, closure);
            }
            break;
        case T_PAREN_LEFT:
            if (T_SYMBOL == file->prev.type) {
                if (call->ctx.closure_body) {
                    add_new_closure_function_call(file, &file->prev);
                } else {
                    add_new_function_call_param(file, call);
                }
            }
            break;
        case T_SQUARE_LEFT:
            new_param.type = PARAM_TYPE_CLOSURE;
            vec_init(&new_param.val.closure.params);
            vec_init(&new_param.val.closure.fn_calls);
            vec_push(&call->params, new_param);
            break;
    }
}

static void apply_token(ParsedFile *file, const Token *token)
{
    FunctionCall *call = current_function_call(file);
    TokenType type = token->type;

    if (T_PAREN_LEFT == type) {
        call->ctx.param_list = true;
    }

    if (T_PAREN_RIGHT == type) {
        call->ctx.param_list = false;

        FunctionCall top = vec_pop(&file->function_stack);

        if (0 == file->function_stack.length) {
            vec_push(&file->fn_calls, top);
        }
    }

    if (call->ctx.param_list) {
        apply_token_to_param_list(file, call, token);
    }
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
            if (call->ctx.closure_body) {
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
    FunctionCallVec *function_stack = &file->function_stack;

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

    vec_init(&file->function_stack);
    vec_init(&file->expectations);
    vec_init(&file->fn_calls);

    return file;
}

static void free_function_calls(FunctionCallVec *calls);

static void free_closure(Closure *closure)
{
    free_function_calls(&closure->fn_calls);
    vec_deinit(&closure->fn_calls);

    uint32_t i;
    ClosureParam current_param;
    vec_foreach(&closure->params, current_param, i) {
        wrench_free(current_param.name);
    }

    vec_deinit(&closure->params);
}

static void free_function_call(const FunctionCall *call)
{
    uint32_t i;
    FunctionParam current;

    vec_foreach(&call->params, current, i) {
        switch (current.type) {
            case PARAM_TYPE_FUNCTION_CALL:
                free_function_call(&current.val.fn_call);
                break;
            case PARAM_TYPE_STRING:
                wrench_free(current.val.str);
                break;
            case PARAM_TYPE_CLOSURE:
                free_closure(&current.val.closure);
                break;
        }
    }

    vec_deinit(&call->params);
    wrench_free(call->name);
}

static void free_function_calls(FunctionCallVec *calls)
{
    uint32_t i;
    FunctionCall call;

    vec_foreach(calls, call, i) {
        free_function_call(&call);
    }
}

void parsed_file_free(ParsedFile *file)
{
    token_free(file->prev);

    free_function_calls(&file->fn_calls);

    vec_deinit(&file->fn_calls);
    vec_deinit(&file->expectations);
    vec_deinit(&file->function_stack);

    wrench_free(file);
}

