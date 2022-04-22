#include "parser.h"
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


int TreeDepth (struct node_t *top)
{
    int depth = 1;
    if (top->left)
        depth += TreeDepth (top->left);
    if (top->right)
        depth += TreeDepth (top->right);
    return depth;
}