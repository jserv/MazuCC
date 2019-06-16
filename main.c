#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "mzcc.h"

int main(int argc, char **argv)
{
    bool dump_ast = ((argc > 1) && !strcmp(argv[1], "--dump-ast"));
    List *toplevels = read_toplevels();
    if (!dump_ast)
        emit_data_section();

    for (Iter i = list_iter(toplevels); !iter_end(i);) {
        Ast *v = iter_next(&i);
        if (dump_ast)
            printf("%s", ast_to_string(v));
        else
            emit_toplevel(v);
    }
    list_free(cstrings, free);
    list_free(ctypes, free);
    return 0;
}
