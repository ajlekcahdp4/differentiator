#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "derivatives.h"
#include "writetex.h"
#include "optimization.h"
#include "tree.h"





int main ()
{
    char *buf = 0;
    int res = 0;
    size_t deg = 0;
    struct lex_array_t *lex = 0;
    struct node_t *top1  = NULL;
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
    top1 = build_syntax_tree (*lex);
    if (top1 == NULL)
        return 0;
    //========Derivating and tex writing ==========
    printf ("Please, enter the name for output file: ");
    char *filename = calloc (128, sizeof(char));
    scanf ("%s", filename);
    FILE *f = fopen (filename, "w");
    free (filename);
    printf ("Please, enter degree of the derivate you need: ");
    res = scanf ("%lu", &deg);
    assert (res);

    PrintStart (f, top1, deg);
    for (size_t i = 0; i < deg; i++)
    {
        top2 = Derivate (top1);
        top2 = Optimize (f, top2);
        DeleteTree (top1);
        top1 = top2;
    }

    tree_dump (top1);
    PrintEnd (f, top1);
    fclose (f);
    //========End===========
    DeleteTree(top1);
    End (buf, lex);
    
    return 0;
}