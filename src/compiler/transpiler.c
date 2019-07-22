#include <stdio.h>
#include <wrench/wrench.h>
#include "compiler.h"
#include "token_stream.h"
#include "parser.h"
#include "cli.h"


static uint32_t ENTITY_ID = 0;

static const char *TEMPLATE_FN_CALL = "wrench_function_call(\"%ls\", NULL);\n";


static void add_call(const FunctionCall *call, FILE *file)
{
    fprintf(file, TEMPLATE_FN_CALL, call->name);
}

void transpile_file(const ParsedFile *parsed, const char *target, bool entry)
{
    FILE *file = fopen(target, "w+");

    if (!file) wrench_fatalf("Unable to write file '%s'", target);

    FunctionCall fn_call;
    uint32_t i = 0;

    fprintf(file, "%s", "#include <wrench/wrench.h>\n");

    if (entry) {
        fprintf(file, "int main(int argc, char **argv) {\n");
    } else {
        fprintf(file, "void entry_%d(void) {\n", ENTITY_ID++);
    }

    vec_foreach(&parsed->fn_calls, fn_call, i) {
        add_call(&fn_call, file);
    };

    if (entry) {
        fprintf(file, "%s", "\n return 0; \n }\n");
    } else {
        fprintf(file, "%s", "\n}\n");
    }

    fclose(file);
}

void transpile(const char *path)
{
    TokenStream *stream = token_stream_new(path);
    ParsedFile *parsed = parsed_file_new();
    Token token = token_stream_next(stream);

    while (T_EOF != token.type) {
        parsed_file_append(parsed, &token);
        token = token_stream_next(stream);
    }

    // TODO: cli_get_build_dir()
    transpile_file(parsed, "./wrench-build/test.c", true);

    token_stream_free(stream);
    parsed_file_free(parsed);
}
