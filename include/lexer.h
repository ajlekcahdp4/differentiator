#ifndef LEXER_INCLUDED
#define LEXER_INCLUDED

#define MAXLEN 1024

enum lexem_kind_t { OP, BRACE, VAR, NUM };
enum operation_t { ADD, SUB, MUL, DIV, SIN, COS, EXP, LOG, DEG };
enum braces_t { LBRAC, RBRAC };

struct lexem_t {
    enum lexem_kind_t kind;
    union lexem_val{
        char *name;
        double num;
        enum  operation_t op;
        enum  braces_t b;
    } lex;
};

struct lex_array_t {
    struct lexem_t *lexems;
    int size;
    int capacity;
};


void print_lex (struct lex_array_t *lex);
int skip_spaces (const char* buf, int ip);
int Input (char **buf);
void End (char *buf, struct lex_array_t *lex);
int lex_insert (struct lex_array_t *lex, const char *buf, int i, int ip);
struct lex_array_t *lex_string (const char *buf);
struct lex_array_t *lex_resize (struct lex_array_t *lex);

#endif