#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "optimization.h"
#include "../derivatives/derivatives.h"
#include "../writetex/writetex.h"

#include "../dump_tree/dump_tree.h"

int MergeConstants (struct node_t *top);
int MulZero (struct node_t *top);

unsigned long long HashNode (struct node_t *top)
{
    unsigned long long hash = (unsigned long long)(top->data.lex.num * top->data.lex.num);
    return hash;
}

unsigned long long HashTree (struct node_t *top)
{
    if (top == NULL)
        return 0;
    unsigned long long hash = 0;
    if (top->left)
        hash += HashTree (top->left);
    if (top->right)
        hash += HashTree (top->right);
    hash += HashNode (top);
    return hash;
}


void Optimize (FILE *f, struct node_t *top)
{
    unsigned long long oldhash = HashTree (top);
    unsigned long long newhash = 0;

    while (oldhash != newhash)
    {
        oldhash = HashTree (top);
        DumpDerivate (f, top);
        MergeConstants (top);
        MulZero (top);
        newhash = HashTree (top);
    }
}


int MulZero (struct node_t *top)
{
    if (top == NULL)
        return 0;
    if (top->left && top->left->left)
        MulZero (top->left);
    if (top->right && top->right->left)
        MulZero (top->right);
    if (top->data.kind != OP)
    {
        fprintf (stderr, "ERROR: no operation in optimization (no mul zero)\n");
        return 1;
    }
    if ((top->left->data.kind == NUM && top->left->data.lex.num < 1e-7) || (top->right->data.kind == NUM && top->right->data.lex.num < 1e-7))
    {
        top->data.kind = NUM;
        top->data.lex.num = 0;
        free (top->left);
        free (top->right);
        top->left  = NULL;
        top->right = NULL;
    }
    return 0;
}


int MergeConstants (struct node_t *top)
{
    if (top == NULL)
        return 0;
    if (top->left && top->left->left)
        MergeConstants (top->left);
    if (top->right && top->right->left)
        MergeConstants (top->right);
    if (top->data.kind != OP)
    {
        fprintf (stderr, "ERROR: no operation in optimization (merging constants)\n");
        return 1;
    }
    if (is_const (top))
    {
        switch (top->data.lex.op)
        {
        case ADD:
            top->data.kind = NUM;
            top->data.lex.num = top->left->data.lex.num + top->right->data.lex.num;
            free (top->left);
            free (top->right);
            top->left = NULL;
            top->right = NULL;
            return 0;
        case SUB:
            top->data.kind = NUM;
            top->data.lex.num = top->left->data.lex.num - top->right->data.lex.num;
            free (top->left);
            free (top->right);
            top->left = NULL;
            top->right = NULL;
            return 0;
        case MUL:
            top->data.kind = NUM;
            top->data.lex.num = top->left->data.lex.num * top->right->data.lex.num;
            free (top->left);
            free (top->right);
            top->left = NULL;
            top->right = NULL;
            return 0;
        case DIV:
            top->data.kind = NUM;
            top->data.lex.num = top->left->data.lex.num / top->right->data.lex.num;
            free (top->left);
            free (top->right);
            top->left = NULL;
            top->right = NULL;
            return 0;
        case DEG:
            top->data.kind = NUM;
            top->data.lex.num = pow (top->left->data.lex.num, top->right->data.lex.num);
            free (top->left);
            free (top->right);
            top->left = NULL;
            top->right = NULL;
            return 0;
        default:
            return 0;
        }
    }
    return 0;   
}