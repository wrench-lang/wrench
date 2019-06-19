#include <sys/stat.h>
#include "compiler.h"


static void create_build_dir(const char *dir)
{
    struct stat st;

    if (-1 == stat(dir, &st)) {
        mkdir(dir,  0777);
        chmod(dir, 0777);
    }
}

int main(int argc, char **argv)
{
    create_build_dir("./wrench-build");

    compile_entry(argv[1]);
}
