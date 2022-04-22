#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "derivatives.h"
#include "../writetex/writetex.h"

struct node_t *SubTreeCpy (const struct node_t *src)
{
    if (src == NULL)
        return NULL;
    struct node_t *dest = calloc (1, sizeof (struct node_t));
    dest->data.kind = src->data.kind;
    dest->data.lex = src->data.lex;
    
    if (src->left)
        dest->left  = SubTreeCpy (src->left);
    if (src->right)
        dest->right = SubTreeCpy (src->right);
    return dest;
}

int is_const (struct node_t *top)
{
    if (top == NULL)
        return 1;
    int res = 1;
    if (top->data.kind == OP)
    {
        res = is_const (top->left);
        if (res == 0)
            return 0;
        res = is_const(top->right);
        if (res == 0)
            return 0;
    }
    else if (top->data.kind == VAR)
        return 0;
    return 1;
}


struct node_t *NewNum (double val)
{
    struct node_t *newnode = calloc (1, sizeof(struct node_t));
    newnode->data.kind = NUM;
    newnode->data.lex.num  = val;
    return newnode;
}

struct node_t *NewOp (enum operation_t op)
{
    struct node_t *newnode = calloc (1, sizeof (struct node_t));
    newnode->data.kind = OP;
    newnode->data.lex.op = op;
    return newnode;
}


struct node_t *Derivate (const struct node_t *top)
{
    struct node_t *node = NULL;

    switch (top->data.kind)
    {
    case NUM:
        node = NewNum (0);
        return node;
    case VAR:
        node = NewNum (1);
        return node;
    case OP:
        switch (top->data.lex.op)
        {
        case ADD:
            node = NewOp (ADD);
            node->left  = Derivate (top->left);
            node->right = Derivate (top->right);
            return node;
        case SUB:
            node = NewOp (SUB);
            node->left  = Derivate (top->left);
            node->right = Derivate (top->right);
            return node;
        case MUL:
            node = NewOp (ADD);
            
            node->left  = NewOp (MUL);
            node->right = NewOp (MUL);

            node->left->left   = Derivate   (top->left);
            node->left->right  = SubTreeCpy (top->right);

            node->right->left  = SubTreeCpy (top->left);
            node->right->right = Derivate   (top->right);
            
            return node;
        case DIV:
            node = NewOp (DIV);
            
            node->left  = NewOp (SUB);
            node->right = NewOp (DEG);

            node->left->left  = NewOp (MUL);
            node->left->right = NewOp (MUL);

            node->right->left  = SubTreeCpy (top->right);
            node->right->right = NewNum (2);

            node->left->left->left   = Derivate   (top->left);
            node->left->left->right  = SubTreeCpy (top->right);

            node->left->right->left  = SubTreeCpy (top->left);
            node->left->right->right = Derivate   (top->right);

            return node;
        case SIN:
            node = NewOp (MUL);

            node->left  = NewOp (COS);
            node->right = Derivate (top->left);
            
            node->left->left = SubTreeCpy (top->left);
            
            return node;
        case COS:
            node = NewOp (SUB);

            node->left  = NewNum (0);
            node->right = NewOp  (MUL);
            
            node->right->left  = NewOp (SIN);
            node->right->right = Derivate (top->left);

            node->right->left->left = SubTreeCpy (top->left);
            
            return node;
        case LOG:
            node = NewOp (DIV);

            node->left  = Derivate (top->left);
            node->right = SubTreeCpy (top->left);
            
            return node;
        case EXP:
            node = NewOp (MUL);
            
            node->left  = SubTreeCpy (top);
            node->right = Derivate   (top->left);

            return node;
        case DEG:
            if (is_const(top->right))
            {
                if (is_const(top->left))
                {
                    return NewNum (0);
                }

                node = NewOp (MUL);

                node->right = SubTreeCpy (top->right);
                node->left  = NewOp (MUL);

                node->left->right = Derivate (top->left);
                node->left->left  = NewOp (DEG);

                node->left->left->left  = SubTreeCpy (top->left);
                node->left->left->right = NewOp (SUB);

                node->left->left->right->left  = SubTreeCpy (top->right);
                node->left->left->right->right = NewNum (1);
            }
            else
            {
                node = NewOp (MUL);

                node->left = NewOp (EXP);

                node->left->left = NewOp (MUL);
                node->left->left->left  = SubTreeCpy (top->right);
                node->left->left->right = NewOp (LOG);
                node->left->left->right->left = SubTreeCpy (top->left); 

                node->right = Derivate (node->left->left);
            }
            return node;
        default:
            fprintf (stderr, "ERROR: unexpected operator in differentiating\n");
            return NULL;
        }
    default:
        fprintf (stderr, "ERROR: unexpected operator in differentiating\n");
        return NULL;
    }
    
    return NULL;
}
