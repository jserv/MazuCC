#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "mzcc.h"

#define make_strtok(x) make_token(TTYPE_STRING, (uintptr_t) get_cstring(x))
#define make_ident(x) make_token(TTYPE_IDENT, (uintptr_t) get_cstring(x))
#define make_punct(x) make_token(TTYPE_PUNCT, (uintptr_t) (x))
#define make_number(x) make_token(TTYPE_NUMBER, (uintptr_t) (x))
#define make_char(x) make_token(TTYPE_CHAR, (uintptr_t) (x))

static Token *ungotten = NULL;

static Token *make_token(enum TokenType type, uintptr_t data)
{
    Token *r = malloc(sizeof(Token));
    r->type = type;
    r->priv = data;
    return r;
}

static int getc_nonspace(void)
{
    int c;
    while ((c = getc(stdin)) != EOF) {
        if (isspace(c) || c == '\n' || c == '\r')
            continue;
        return c;
    }
    return EOF;
}

static Token *read_number(char c)
{
    String *s = make_string();
    string_append(s, c);
    while (1) {
        int c = getc(stdin);
        if (!isdigit(c) && !isalpha(c) && c != '.') {
            ungetc(c, stdin);
            return make_number(get_cstring(s));
        }
        string_append(s, c);
    }
}

static Token *read_char(void)
{
    char c = getc(stdin);
    if (c == EOF)
        goto err;
    if (c == '\\') {
        c = getc(stdin);
        if (c == EOF)
            goto err;
    }
    char c2 = getc(stdin);
    if (c2 == EOF)
        goto err;
    if (c2 != '\'')
        error("Malformed char literal");
    return make_char(c);
err:
    error("Unterminated char");
    return NULL; /* non-reachable */
}

static Token *read_string(void)
{
    String *s = make_string();
    while (1) {
        int c = getc(stdin);
        if (c == EOF)
            error("Unterminated string");
        if (c == '"')
            break;
        if (c == '\\') {
            c = getc(stdin);
            switch (c) {
            case EOF:
                error("Unterminated \\");
            case '\"':
                break;
            case 'n':
                c = '\n';
                break;
            default:
                error("Unknown quote: %c", c);
            }
        }
        string_append(s, c);
    }
    return make_strtok(s);
}

static Token *read_ident(char c)
{
    String *s = make_string();
    string_append(s, c);
    while (1) {
        int c2 = getc(stdin);
        if (isalnum(c2) || c2 == '_') {
            string_append(s, c2);
        } else {
            ungetc(c2, stdin);
            return make_ident(s);
        }
    }
}

static void skip_line_comment(void)
{
    while (1) {
        int c = getc(stdin);
        if (c == '\n' || c == EOF)
            return;
    }
}

static void skip_block_comment(void)
{
    enum { in_comment, asterisk_read } state = in_comment;
    while (1) {
        int c = getc(stdin);
        if (state == in_comment) {
            if (c == '*')
                state = asterisk_read;
        } else if (c == '/') {
            return;
        }
    }
}

static Token *read_rep(int expect, int t1, int t2)
{
    int c = getc(stdin);
    if (c == expect)
        return make_punct(t2);
    ungetc(c, stdin);
    return make_punct(t1);
}

static Token *read_token_int(void)
{
    int c = getc_nonspace();
    switch (c) {
    case '0' ... '9':
        return read_number(c);
    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '_':
        return read_ident(c);
    case '/': {
        c = getc(stdin);
        if (c == '/') {
            skip_line_comment();
            return read_token_int();
        }
        if (c == '*') {
            skip_block_comment();
            return read_token_int();
        }
        ungetc(c, stdin);
        return make_punct('/');
    }
    case '*':
    case '(':
    case ')':
    case ',':
    case ';':
    case '.':
    case '[':
    case ']':
    case '{':
    case '}':
    case '!':
    case '?':
    case ':':
        return make_punct(c);
    case '-':
        c = getc(stdin);
        if (c == '-')
            return make_punct(PUNCT_DEC);
        if (c == '>')
            return make_punct(PUNCT_ARROW);
        ungetc(c, stdin);
        return make_punct('-');
    case '=':
        return read_rep('=', '=', PUNCT_EQ);
    case '+':
        return read_rep('+', '+', PUNCT_INC);
    case '&':
        return read_rep('&', '&', PUNCT_LOGAND);
    case '|':
        return read_rep('|', '|', PUNCT_LOGOR);
    case '<':
        return read_rep('<', '<', PUNCT_LSHIFT);
    case '>':
        return read_rep('>', '>', PUNCT_RSHIFT);
    case '"':
        return read_string();
    case '\'':
        return read_char();
    case EOF:
        return NULL;
    default:
        error("Unexpected character: '%c'", c);
        return NULL; /* non-reachable */
    }
}

bool is_punct(Token *tok, int c)
{
    return tok && (get_ttype(tok) == TTYPE_PUNCT) && (get_punct(tok) == c);
}

void unget_token(Token *tok)
{
    if (!tok)
        return;
    if (ungotten)
        error("Push back buffer is already full");
    ungotten = tok;
}

Token *peek_token(void)
{
    Token *tok = read_token();
    unget_token(tok);
    return tok;
}

Token *read_token(void)
{
    if (ungotten) {
        Token *tok = ungotten;
        ungotten = NULL;
        return tok;
    }
    return read_token_int();
}
