#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mzcc.h"

char *outfile = NULL, *infile = NULL;
extern FILE *outfp;
bool dump_ast;

static void usage()
{
    fprintf(stdout,
            "\n"
            "Usage : mzcc [ input file | - ]\n"
            "\n"

            "  -o filename            Output to the specified file\n"
            "  -dump-ast              Print Abstract Syntax Tree\n"
            "  [ input file | - ]     Specifying a file or use standard input\n\n");
}

void print_usage_and_exit()
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
            case 'd':
                if (!strcmp(*argv, "-dump-ast")) {
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

static FILE *open_output_file()
{
    FILE *tmp = stdout;
    if (outfile && !(tmp = fopen(outfile, "w"))) {
        printf("Can not open file %s\n", outfile);
        exit(1);
    }
    return tmp;
}

static FILE *open_input_file()
{
    if (!infile) {
        printf("Input file is not specified\n\n");
        print_usage_and_exit();
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
    parse_args(argc, argv);
    stdin = open_input_file();
    outfp = open_output_file();

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
