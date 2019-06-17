#ifndef MAZUCC_H
#define MAZUCC_H

#include <stdbool.h>
#include <stdint.h>
#include "dict.h"
#include "list.h"
#include "util.h"

enum TokenType {
    TTYPE_NULL,
    TTYPE_IDENT,
    TTYPE_PUNCT,
    TTYPE_NUMBER,
    TTYPE_CHAR,
    TTYPE_STRING,
};

typedef struct {
    int type;
    uintptr_t priv;
} Token;

enum {
    AST_LITERAL = 256,
    AST_STRING,
    AST_LVAR,
    AST_GVAR,
    AST_FUNCALL,
    AST_FUNC,
    AST_DECL,
    AST_ARRAY_INIT,
    AST_ADDR,
    AST_DEREF,
    AST_IF,
    AST_TERNARY,
    AST_FOR,
    AST_RETURN,
    AST_COMPOUND_STMT,
    AST_STRUCT_REF,
    PUNCT_EQ,
    PUNCT_INC,
    PUNCT_DEC,
    PUNCT_LOGAND,
    PUNCT_LOGOR,
    PUNCT_ARROW,
    PUNCT_LSHIFT,
    PUNCT_RSHIFT,
};

enum {
    CTYPE_VOID,
    CTYPE_CHAR,
    CTYPE_INT,
    CTYPE_LONG,
    CTYPE_FLOAT,
    CTYPE_DOUBLE,
    CTYPE_ARRAY,
    CTYPE_PTR,
    CTYPE_STRUCT,
};

typedef struct __Ctype {
    int type;
    int size;
    struct __Ctype *ptr; /* pointer or array */
    int len;             /* array length */
    /* struct */
    Dict *fields;
    int offset;
} Ctype;

typedef struct __Ast {
    int type;
    Ctype *ctype;
    union {
        /* char, int, or long */
        long ival;

        /* float or double */
        struct {
            union {
                double fval;
                int lval[2];
            };
            char *flabel;
        };

        /* string literal */
        struct {
            char *sval;
            char *slabel;
        };

        /* Local/global variable */
        struct {
            char *varname;
            struct {
                int loff;
                char *glabel;
            };
        };

        /* Binary operator */
        struct {
            struct __Ast *left;
            struct __Ast *right;
        };

        /* Unary operator */
        struct {
            struct __Ast *operand;
        };

        /* Function call or function declaration */
        struct {
            char *fname;
            struct {
                List *args;
                struct {
                    List *params;
                    List *localvars;
                    struct __Ast *body;
                };
            };
        };

        /* Declaration */
        struct {
            struct __Ast *declvar;
            struct __Ast *declinit;
        };

        /* Array initializer */
        List *arrayinit;

        /* if statement or ternary operator */
        struct {
            struct __Ast *cond;
            struct __Ast *then;
            struct __Ast *els;
        };

        /* for statement */
        struct {
            struct __Ast *forinit;
            struct __Ast *forcond;
            struct __Ast *forstep;
            struct __Ast *forbody;
        };

        /* return statement */
        struct __Ast *retval;

        /* Compound statement */
        List *stmts;

        /* Struct reference */
        struct {
            struct __Ast *struc;
            char *field; /* specific to ast_to_string only */
        };
    };
} Ast;

/* verbose.c */
extern char *token_to_string(const Token tok);
extern char *ast_to_string(Ast *ast);
extern char *ctype_to_string(Ctype *ctype);

/* lexer.c */
extern bool is_punct(const Token tok, int c);
extern void unget_token(const Token tok);
extern Token peek_token(void);
extern Token read_token(void);

#define get_priv(tok, type)                                       \
    ({                                                            \
        assert(__builtin_types_compatible_p(typeof(tok), Token)); \
        ((type) tok.priv);                                        \
    })

#define get_ttype(tok)                                            \
    ({                                                            \
        assert(__builtin_types_compatible_p(typeof(tok), Token)); \
        (tok.type);                                               \
    })

#define get_token(tok, ttype, priv_type) \
    ({                                   \
        assert(get_ttype(tok) == ttype); \
        get_priv(tok, priv_type);        \
    })

#define get_char(tok) get_token(tok, TTYPE_CHAR, char)
#define get_strtok(tok) get_token(tok, TTYPE_STRING, char *)
#define get_ident(tok) get_token(tok, TTYPE_IDENT, char *)
#define get_number(tok) get_token(tok, TTYPE_NUMBER, char *)
#define get_punct(tok) get_token(tok, TTYPE_PUNCT, int)

/* parser.c */
extern List *strings;
extern List *flonums;
extern List *ctypes;
extern char *make_label(void);
extern List *read_toplevels(void);
extern bool is_inttype(Ctype *ctype);
extern bool is_flotype(Ctype *ctype);

/* codegen_x64.c */
extern void emit_data_section(void);
extern void emit_toplevel(Ast *v);

#endif /* MAZUCC_H */
