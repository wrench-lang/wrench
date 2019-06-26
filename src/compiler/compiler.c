#include <stdio.h>
#include <wrench/wrench.h>
#include "compiler.h"
#include "token_stream.h"
#include "parser.h"
#include "generator.h"


void compile_file(const char *path)
{
    FILE *file = fopen(path, "r");
    if (!file) {
        wrench_fatalf("Unable to open file %s", path);
    }

    TokenStream *stream = token_stream_new(path);
    ParsedFile *parsed = parsed_file_new();
    Token token;

    while (1) {
        token = token_stream_next(stream);

        if (T_EOF == token.type) {
            break;
        }

        parsed_file_append(parsed, &token);
    }
    generate_file(parsed, "./wrench-build/test.c", true);

    token_stream_free(stream);
    parsed_file_free(parsed);

    fclose(file);
}

void compile_entry(const char* entry)
{
    wrench_init();

    compile_file(entry);

    wrench_deinit();
}
