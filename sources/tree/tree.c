#include "../parser/parser.h"
#include <stdlib.h>

int DeleteTree (struct node_t *top)
{
    if (top == NULL)
        return 0;
    if (top->left)
        DeleteTree (top->left);
    if (top->right)
        DeleteTree (top->right);
    free (top);
    return 0;
}