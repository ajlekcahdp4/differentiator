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
int MulOne (struct node_t *top);
int PlusZero (struct node_t *top);

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

//macros
#define res_check(arg)  \
if (arg == 0)           \
    break;


void Optimize (FILE *f, struct node_t *top)
{
    unsigned long long oldhash = HashTree (top);
    unsigned long long newhash = 0;
    int res = 0;

    while (oldhash != newhash)
    {
        oldhash = HashTree (top);
        DumpDerivate (f, top);
        res = 1;
        res = MergeConstants (top);
        DumpDerivate (f, top);
        res_check (res);
        res = MulZero (top);
        DumpDerivate (f, top);
        res_check (res);
        //res = MulOne (top);
        //DumpDerivate (f, top);
        res_check (res);
        //res = PlusZero (top);
        //DumpDerivate (f, top);
        res_check (res);

        newhash = HashTree (top);
    }
    //DumpDerivate (f, top);
}
#undef res_check



int MulOne (struct node_t *top)
{
    struct node_t *temp = NULL;
    if (top == NULL)
        return 0;
    if (top->left && top->left->left)
    {
        MulOne (top->left);
        if (top->left->left == NULL)
        {
            temp = top->left;
            top->left = top->left->right;
            free (temp);
        }
        else if (top->left->right == NULL)
        {
            temp = top->left;
            top->left= top->left->left;
            free (temp);
        }
    }
    if (top->right && top->right->left)
    {
        MulOne (top->right);
        if (top->right->left == NULL)
        {
            temp = top->right;
            top->right = top->right->right;
            free (temp);
        }
        else if (top->right->right == NULL)
        {
            temp = top->right;
            top->right= top->right->left;
            free (temp);
        }
    }
    if (top->data.kind != OP)
    {
        fprintf (stderr, "ERROR: no operation in optimization (in mul one)\n");
        return -1;
    }
    if (top->data.lex.op != MUL)
        return 1;
    if (top->left->data.kind == NUM && fabs (top->left->data.lex.num - 1.0) < 1e-7)//is_one
    {
        top->data.kind = top->right->data.kind;
        top->data.lex = top->right->data.lex;
        free (top->left);
        top->left  = NULL;
        return 0;
    }
    if (top->right->data.kind == NUM && fabs (top->right->data.lex.num - 1.0) < 1e-7)//is_one
    {
        top->data.kind = top->left->data.kind;
        top->data.lex = top->left->data.lex;
        free (top->right);
        top->right = NULL;
        return 0;
    }
    return 1;
}



int PlusZero (struct node_t *top)
{
    struct node_t *temp = NULL;
    if (top == NULL)
        return 0;
    if (top->left && top->left->left)
    {
        PlusZero (top->left);
        if (top->left->left == NULL)
        {
            temp = top->left;
            top->left = top->left->right;
            free (temp);
        }
        else if (top->left->right == NULL)
        {
            temp = top->left;
            top->left= top->left->left;
            free (temp);
        }
    }
    if (top->right && top->right->left)
    {
        PlusZero (top->right);
        if (top->right->left == NULL)
        {
            temp = top->right;
            top->right = top->right->right;
            free (temp);
        }
        else if (top->right->right == NULL)
        {
            temp = top->right;
            top->right= top->right->left;
            free (temp);
        }
    }
    if (top->data.kind != OP)
    {
        fprintf (stderr, "ERROR: no operation in optimization (in Plus Zero)\n");
        return -1;
    }
    if (top->data.lex.op != ADD)
        return 1;
    if (top->left->data.kind == NUM && fabs (top->left->data.lex.num) < 1e-7)//is_null
    {
        top->data.kind = top->right->data.kind;
        top->data.lex = top->right->data.lex;
        free (top->left);
        top->left  = NULL;
        return 0;
    }
    if (top->right->data.kind == NUM && fabs (top->right->data.lex.num) < 1e-7)//is_null
    {
        top->data.kind = top->left->data.kind;
        top->data.lex = top->left->data.lex;
        free (top->right);
        top->right = NULL;
        return 0;
    }
    return 1;
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
        fprintf (stderr, "ERROR: no operation in optimization (in mul zero)\n");
        return -1;
    }
    if (top->data.lex.op != MUL)
        return 1;
    if ((top->left->data.kind == NUM && top->left->data.lex.num < 1e-7) || (top->right->data.kind == NUM && top->right->data.lex.num < 1e-7))
    {
        top->data.kind = NUM;
        top->data.lex.num = 0;
        free (top->left);
        free (top->right);
        top->left  = NULL;
        top->right = NULL;
        return 0;
    }
    return 1;
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
        return -1;
    }
    if (top->left->data.kind == NUM && top->right->data.kind == NUM)
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
    return 1;   
}