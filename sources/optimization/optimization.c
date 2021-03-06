#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "optimization.h"
#include "derivatives.h"
#include "writetex.h"
#include "tree.h"
#include "dump_tree.h"

int MergeConstants (struct node_t *top);
int MergeConstants2 (struct node_t *top);
int MulZero (struct node_t *top);
struct node_t *MulOne (struct node_t *top);
struct node_t *PlusZero (struct node_t *top);


int is_one  (struct node_t *node);
int is_zero (struct node_t *node);
int is_consts_mergeable (struct node_t *top);


//=====================================hash=functions========================================

unsigned long long HashNode (struct node_t *top)
{
    unsigned long long hash = (unsigned long long)((top->data.lex.num * top->data.lex.num) / (top->data.lex.num * 1e-5));
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

//========================================Optimize============================================

struct node_t *Optimize (FILE *f, struct node_t *top)
{
    unsigned long long oldhash = HashTree (top);
    unsigned long long newhash = 0;

    while (oldhash != newhash)
    {
        DumpDerivate (f, top);
        oldhash = HashTree (top);
        MergeConstants (top);
        MulZero (top);
        top = MulOne (top);
        top = PlusZero (top);
        MergeConstants2 (top);
        newhash = HashTree (top);
    }
    return top;
}


//=======================================MulOne==============================================
struct node_t *MulOne (struct node_t *top)
{
    struct node_t *temp = NULL;
    if (top == NULL)
        return NULL;
    if (top->left  && top->left->left)
        top->left  = MulOne (top->left);
    if (top->right && top->right->left)
        top->right = MulOne (top->right);
    if (top->data.kind != OP)
    {
        fprintf (stderr, "ERROR: no operation in optimization (in mul one)\n");
        return top;
    }
    if (top->data.lex.op != MUL)
        return top;
    if (is_one (top->left))
    {
        free (top->left);
        temp = top;
        top  = top->right;
        free (temp);
        return top;
    }
    if (is_one (top->right))
    {
        free (top->right);
        temp = top;
        top  = top->left;
        free (temp);
        return top;
    }
    return top;
}


//=======================================PlusZero=============================================
struct node_t *PlusZero (struct node_t *top)
{
    struct node_t *temp = NULL;
    if (top == NULL)
        return NULL;
    if (top->left  && top->left->left)
        top->left  = PlusZero (top->left);
    if (top->right && top->right->left)
        top->right = PlusZero(top->right);
    if (top->data.kind != OP)
    {
        fprintf (stderr, "ERROR: no operation in optimization (in Plus Zero)\n");
        return NULL;
    }
    if (top->data.lex.op != ADD)
        return top;
    if (is_zero (top->left))
    {
        free (top->left);
        temp = top;
        top = top->right;
        free (temp);
        return top;
    }
    if (is_zero (top->right))
    {
        free (top->right);
        temp = top;
        top = top->left;
        free (temp);
        return top;
    }
    return top;
}

//========================================MulZero=============================================

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
        DeleteTree (top->left);
        DeleteTree (top->right);
        top->left  = NULL;
        top->right = NULL;
        return 0;
    }
    return 1;
}

//=====================================Merge=constants=========================================

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


//=====================================Merge=constants=2=========================================

int MergeConstants2 (struct node_t *top)
{
    struct node_t *temp = NULL;
    if (top == NULL)
        return 0;
    if (top->left && top->left->left)
        MergeConstants2 (top->left);
    if (top->right && top->right->left)
        MergeConstants2 (top->right);
    if (top->data.kind != OP)
    {
        fprintf (stderr, "ERROR: no operation in optimization (merging constants)\n");
        return -1;
    }
    if (top->data.lex.op == SIN || top->data.lex.op == COS || top->data.lex.op == EXP || top->data.lex.op == LOG || top->data.lex.op == DEG)
        return 1;
    if (top->left->data.kind == NUM)
    {
        if (top->right->data.kind != OP)
            return 1;
        if (top->right->data.lex.op == SIN || top->right->data.lex.op == COS || top->right->data.lex.op == EXP || top->right->data.lex.op == LOG || top->right->data.lex.op == DEG)
            return 1;
        switch (top->data.lex.op)
        {
        case MUL:
            if (top->right->left->data.kind == NUM)
            {
                switch (top->right->data.lex.op)
                {
                case MUL:
                    top->left->data.lex.num = top->left->data.lex.num * top->right->left->data.lex.op;
                    free (top->right->left);
                    temp = top->right;
                    top->right = temp->right;
                    free (temp);
                    return 0;
                default:
                    return 1;
                }
            }
            if (top->right->right->data.kind == NUM)
            {
                switch (top->right->data.lex.op)
                {
                case MUL:
                    top->left->data.lex.num = top->left->data.lex.num * top->right->right->data.lex.num;
                    free (top->right->right);
                    temp = top->right;
                    top->right = temp->left;
                    free (temp);
                    return 0;
                default:
                    return 1;
                }
            }
            break;
        default:
            return 1;
        }
    }
    if (top->right->data.kind == NUM)
    {
        ;
    }
    return 0;
}

//=======================================checking=============================================

#define EPSILON ((double)1e-5)

int is_one (struct node_t *node)
{
    if (node->data.kind != NUM)
        return 0;
    if (fabs (node->data.lex.num - 1) > EPSILON)
        return 0;
    return 1;
}

int is_zero (struct node_t *node)
{
    if (node->data.kind != NUM)
        return 0;
    if (fabs (node->data.lex.num) > EPSILON)
        return 0;
    return 1;
}



#undef EPSILON