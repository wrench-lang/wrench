#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <stdbool.h>
#include <stdio.h>
#include "cli.h"
#include "compiler.h"

#define COMMAND_FORMAT "%s %s %s -o %s"
#define COMMAND_FORMAT_DEBUG "%s %s %s %s -o %s"
#define DEFAULT_CFLAGS "-std=c11 -Wall -Wextra -Wconversion -pedantic -ftrapv -Wshadow -Wundef -Wcast-align -Wunreachable-code -Wno-switch -fstack-protector -fno-common -D_FORTIFY_SOURCE=2 -l:"WRENCH_RUNTIME_VERSION
#define OBJECT_CFLAGS DEFAULT_CFLAGS" -c -fpic"
#define DEBUG_FLAGS "-ggdb -fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-omit-frame-pointer -lasan"


static FILE *objects;


static int execute_compiler_command(
    const char *source,
    const char *target,
    const char *cflags
)
{
    const char *cc = cli_get_cc();
    bool is_debug = cli_is_debug();

    char command[
         strlen(COMMAND_FORMAT)
        + strlen(cc)
        + strlen(source)
        + strlen(cflags)
        + strlen(target)
        + (is_debug ? strlen(DEBUG_FLAGS) : 0)
        + 1
    ];

    if (is_debug) {
        sprintf(command, COMMAND_FORMAT_DEBUG, cc, source, cflags, DEBUG_FLAGS, target);
    } else {
        sprintf(command, COMMAND_FORMAT, cc, source, cflags, target);
    }

    if (0 != system(command)) {
        printf("Command failed: %s", command);

        return -1;
    }

    fprintf(objects, "%s ", target);

    return 0;
}

static bool is_c_source(const char *path, int type)
{
    size_t len = strlen(path);

    return FTW_F == type
        && 'c' == path[len - 1]
        && '.' == path[len - 2];
}

static int compile_file(const char *source,const struct stat* stat, int type)
{
    (void) stat;

    if (!is_c_source(source, type)) return 0;

    const size_t source_len = strlen(source) + 1;
    char target[source_len];

    memcpy(target, source, source_len);
    target[source_len - 2] = 'o';
    target[source_len - 1] = '\0';

    return execute_compiler_command(source, target, OBJECT_CFLAGS);
}

void compile(void)
{
    char *objects_buf;
    size_t objects_len;
    objects = open_memstream(&objects_buf, &objects_len);

    const char *build_dir = cli_get_build_dir();
    ftw(build_dir, compile_file, 10);
    fflush(objects);

    execute_compiler_command(objects_buf, "wrench.out", DEFAULT_CFLAGS);

    fclose(objects);
    free(objects_buf);
}
