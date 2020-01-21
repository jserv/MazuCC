#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mzcc.h"

char *outfile = NULL, *infile = NULL;
extern FILE *outputfp;
bool dump_ast;

static void usage()
{
    fprintf(stdout,
            "Usage : mzcc <input file>\n"
            "\n"

            "-o filename     Output to the specified file\n"
            "-dump-ast       Print Abstract Syntax Tree\n");
    exit(1);
}
static void parseopt(int argc, char **argv)
{
    if (argc == 1) {
        usage();
    } else {
        int opt;
        while ((opt = getopt(argc, argv, "o:d:")) != -1) {
            switch (opt) {
            case 'o':
                outfile = optarg;
                break;
            case 'd':
                dump_ast = true;
                break;
            }
        }
    }
    if (optind == argc - 1) {  // input file name is specified
        infile = argv[optind];
    }
}
static FILE *open_output_file()
{
    FILE *tmp = stdout;
    if (outfile != NULL && ((tmp = fopen(outfile, "w")) == NULL)) {
        printf("Can not open file %s\n", outfile);
        exit(1);
    }
    return tmp;
}
static FILE *open_input_file()
{
    if (infile == NULL) {
        printf("Input file is not specified\n\n");
        usage();
    }

    FILE *tmp = fopen(infile, "r");
    if (tmp == NULL) {
        printf("Can not open file %s\n", infile);
        exit(1);
    }
    return tmp;
}
int main(int argc, char **argv)
{
    parseopt(argc, argv);
    stdin = open_input_file();
    outputfp = open_output_file();

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
    list_free(cstrings);
    list_free(ctypes);
    return 0;
}
