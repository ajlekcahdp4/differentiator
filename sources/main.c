#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "lexer/lexer.h"

int main ()
{
    char *buf = 0;
    struct lex_array_t *lex = 0;

    Input (&buf);
    
    lex = lex_string (buf);
    if (lex->size > 0)
    {
        print_lex (lex);
        End (buf, lex);
    }
    return 0;
}