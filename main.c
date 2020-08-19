#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "mzcc.h"

static char *outfile = NULL, *infile = NULL;
extern FILE *outfp;
static bool dump_ast;

static void usage()
{
    fprintf(stdout,
            "mzcc [options] filename\n"
            "OPTIONS\n"
            "  -o filename            Write output to the specified file.\n"
            "  --dump-ast             Dump abstract syntax tree(AST)\n");
}

static void print_usage_and_exit()
{
    usage();
    exit(1);
}

static void parse_args(int argc, char **argv)
{
    if (argc < 2) {
        print_usage_and_exit();
    }

    while (true) {
        argc--;
        argv++;
        if (!argc) {
            break;
        }

        if ((*argv)[0] == '-') {
            switch ((*argv)[1]) {
            case '\0':
                infile = "/dev/stdin";
                break;
            case 'o':
                argc--;
                argv++;
                outfile = *argv;
                break;
            case '-':
                if (!strcmp(*argv, "--dump-ast")) {
                    dump_ast = true;
                    break;
                }
            default:
                print_usage_and_exit();
            }
        } else {
            if (infile) {
                // The second non-option argument is not what we expect.
                print_usage_and_exit();
            }
            infile = argv[0];
        }
    }
}

static void open_output_file()
{
    if (outfile) {
        if (!(outfp = fopen(outfile, "w"))) {
            printf("Can not open file %s\n", outfile);
            exit(1);
        }
    } else {
        outfp = stdout;
    }
}

static void open_input_file()
{
    if (!infile) {
        printf("Input file is not specified\n\n");
        print_usage_and_exit();
    }

    if (!freopen(infile, "r", stdin)) {
        printf("Can not open file %s\n", infile);
        exit(1);
    }
}

int main(int argc, char **argv)
{
    parse_args(argc, argv);
    open_input_file();
    open_output_file();

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
