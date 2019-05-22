#include <stdio.h>
#include <wrench/wrench.h>
#include "compiler.h"
#include "token_stream.h"
#include "parser.h"


static void compile_file(const char *path)
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
            // TODO validate EOF
            break;
        }

        parsed_file_append(parsed, &token);
    }

    fclose(file);
}

void compile(const char* path)
{
    wrench_init();

    compile_file(path);

    wrench_deinit();
}
