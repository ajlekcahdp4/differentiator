#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "derivatives.h"

struct node_t *SubTreeCpy (struct node_t *src)
{
    struct node_t *dest = calloc (1, sizeof (struct node_t));
    dest->data.kind = src->data.kind;
    dest->data.lex = src->data.lex;
    
    if (src->left)
        dest->left  = SubTreeCpy (src->left);
    if (src->right)
        dest->right = SubTreeCpy (src->right);
    return dest;
}

struct node_t *NewNode (enum lexem_kind_t kind, union lexem_val lex, struct node_t *left, struct node_t *right)
{
    struct node_t *newnode = calloc (1, sizeof(struct node_t));
    newnode->data.kind = kind;
    newnode->data.lex  = lex;
    newnode->left = left;
    newnode->right = right;
    return newnode;
}

struct node_t * Add (struct node_t *left, struct node_t *right)
{
    struct node_t *newnode = calloc (1, sizeof(struct node_t));
    newnode->data.kind   = OP;
    newnode->data.lex.op = ADD;
    newnode->left  = left;
    newnode->right = right;
    return newnode;
}

struct node_t * Sub (struct node_t *left, struct node_t *right)
{
    struct node_t *newnode = calloc (1, sizeof(struct node_t));
    newnode->data.kind   = OP;
    newnode->data.lex.op = SUB;
    newnode->left  = left;
    newnode->right = right;
    return newnode;
}

struct node_t *Derivate (struct node_t *top)
{
    union lexem_val lex;
    struct node_t *left  = NULL;
    struct node_t *right = NULL;
    struct node_t *temp1 = NULL;
    struct node_t *temp2 = NULL;

    switch (top->data.kind)
    {
    case NUM:
        lex.num = 0;
        return NewNode (NUM, lex, NULL, NULL);
    case VAR:
        lex.num = 1;
        return NewNode (NUM, lex, NULL, NULL);
    case OP:
        switch (top->data.lex.op)
        {
        case ADD:
            lex.op = ADD;
            return NewNode (OP, lex, Derivate (top->left), Derivate (top->right));
        case SUB:
            lex.op = SUB;
            return NewNode (OP, lex, Derivate (top->left), Derivate (top->right));
        case MUL:
            lex.op = MUL;
            temp1  = SubTreeCpy (top->right);
            left   = NewNode    (OP, lex, Derivate(top->left), temp1);
            temp1  = SubTreeCpy (top->left);
            right  = NewNode    (OP, lex, temp1, Derivate (top->right));
            return   Add (left, right);
        case DIV:
            lex.op = MUL;
            temp1  = SubTreeCpy (top->left);
            right  = NewNode (OP, lex, temp1, Derivate (top->right));
            temp1  = SubTreeCpy (top->right);
            left   = NewNode (OP, lex, Derivate (top->left), temp1);
            
            lex.num = 2;
            temp2   = NewNode (NUM, lex, NULL, NULL);
            lex.op  = DEG;
            temp1   = SubTreeCpy (top->right);
            temp2   = NewNode (OP, lex, temp1, temp2);

            temp1   = Sub (left, right);
            lex.op  = DIV;
            return NewNode (OP, lex, temp1, temp2);
        case SIN:
            lex.op = COS;
            temp1  = SubTreeCpy (top->left);
            temp1  = NewNode (OP, lex, temp1, NULL);
            lex.op = MUL;
            return NewNode (OP, lex, temp1, Derivate (top->left));
        case COS:
            lex.op  = SIN;
            temp1   = SubTreeCpy (top->left);
            temp1   = NewNode (OP, lex, temp1, NULL);
            lex.num = 0;
            temp2   = NewNode (NUM, lex, NULL, NULL);
            lex.op  = SUB;
            temp1   = NewNode (OP, lex, temp2, temp1);
            lex.op  = MUL;
            return    NewNode (OP, lex, temp1, Derivate (top->left));
        case LOG:
            lex.op = LOG;
            temp1 = SubTreeCpy (top->left);
            temp2 = Derivate (top->left);
            lex.op = DIV;
            return NewNode (OP, lex, temp2, temp1);
        case EXP:
            lex.op = EXP;
            temp1 = SubTreeCpy (top->left);
            temp1 = NewNode (OP, lex, temp1, NULL);
            lex.op = MUL;
            return (OP, lex, temp1, Derivate (top->left));
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
