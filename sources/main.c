#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "derivatives/derivatives.h"

int main ()
{
    char *buf = 0;
    struct lex_array_t *lex = 0;
    struct node_t *top = NULL;
    struct node_t *top2 = NULL;

    Input (&buf);
    
    lex = lex_string (buf);
    if (lex->size < 0)
    {
        End (buf, lex);
        return 1;
    }
    top = build_syntax_tree (*lex);
    if (top == NULL)
        return 0;
    tree_dump (top);
    top2 = Derivate (top);
    tree_dump (top2);

    return 0;
}