#include <stdio.h>
#include <string.h>
#include <wrench/wrench.h>
#include "generator.h"


static uint32_t ENTITY_ID = 0;

/* static const char *TEMPLATE_ENTRY_FN_CALL = "wrench_function_call(\"%ls\", NULL);"; */


static void add_call(const FunctionCall *call, FILE *file)
{
    /* size_t size = strlen(TEMPLATE_ENTRY_FN_CALL) + (wcslen(call->name) * sizeof(wchar_t)); */
    /* char *call_str = wrench_allocate(size); */

    /* sprintf(call_str, TEMPLATE_ENTRY_FN_CALL, call->name); */
    /* fprintf(file, "%s", call_str); */

    /* return call_str; */
}

void generate_file(const ParsedFile *parsed, const char *target, bool entry)
{
    FILE *file = fopen(target, "w+");

    if (!file) wrench_fatalf("Unable to write file '%s'", target);

    FunctionCall fn_call;
    uint32_t i = 0;

    fprintf(file, "%s", "#include <wrench/wrench.h>\n");

    if (!entry) {
        fprintf(file, "int main(int argc, char **argv) {\n");
    } else {
        fprintf(file, "void entry_%d(void) {\n", ENTITY_ID++);
    }

    vec_foreach(&parsed->fn_calls, fn_call, i) {
        add_call(&fn_call, file);
    };

    if (!entry) {
        fprintf(file, "%s", "\n}\n");
    } else {
        fprintf(file, "%s", "\n return 0; \n }\n");
    }


    fclose(file);
}
