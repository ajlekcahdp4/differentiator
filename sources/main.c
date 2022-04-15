#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "derivatives/derivatives.h"
#include "writetex/writetex.h"

int main ()
{
    char *buf = 0;
    struct lex_array_t *lex = 0;
    struct node_t *top = NULL;
    struct node_t *top2 = NULL;

    Input (&buf);
    
    //===========lexer=============
    lex = lex_string (buf);
    if (lex == NULL || lex->size < 0)
    {
        End (buf, lex);
        return 1;
    }
    //==========parsing============
    top = build_syntax_tree (*lex);
    if (top == NULL)
        return 0;
    //========Derivating and tex writing ==========
    printf ("Please, enter the name for output file\n");
    char *filename = calloc (128, sizeof(char));
    scanf ("%s", filename);
    FILE *f = fopen (filename, "w");
    PrintStart (f);
    top2 = Derivate (top);
    DumpDerivate (f, top2);
    tree_dump (top2);
    PrintEnd (f);
    fclose (f);
    //========End===========
    End (buf, lex);
    return 0;
}