#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <argparse.h>
#include "cli.h"

#if defined(__APPLE__)

#include <sys/syslimits.h>

#elif defined(__linux__)

#include <linux/limits.h>

#endif


static char *CC = "cc";
static char BUILD_DIR[PATH_MAX] = "./wrench-build";
static char *ENTRY_FILE  = NULL;
static int NO_DEBUG = 0;


static struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('c', "cc", &CC, "C compiler. Supported: gcc, clang", NULL, 0, 0),
    OPT_STRING('d', "build-dir", &BUILD_DIR, "Build directory", NULL, 0, 0),
    OPT_BOOLEAN('\0', "no-debug", &NO_DEBUG, "Disable debug mode", NULL, 0, 0),
    OPT_END(),
};

static const char *usage[] = {
    "wrench file [options] [[--] args]",
    NULL
};


void cli_load(int argc, const char **argv)
{
    struct argparse argparse;

    argparse_init(&argparse, options, usage, 0);
    argparse_parse(&argparse, argc, argv);

    ENTRY_FILE = (char *) argv[0];
}

const char *cli_get_cc(void)
{
    return CC;
}

const char *cli_get_build_dir(void)
{
    return BUILD_DIR;
}

const char *cli_get_file(void)
{
    return ENTRY_FILE;
}

bool cli_is_debug(void)
{
    return !(bool) NO_DEBUG;
}
