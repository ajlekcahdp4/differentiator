#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "parser.h"
#include "../dump_tree/dump_tree.h"



struct lexem_t current (struct lexer_state *pstate)
{
    return pstate->lexarr.lexems[pstate->cur];
}


int is_plus_minus (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != OP)
        return 0;
    if (cur.lex.op != SUB && cur.lex.op != ADD)
        return 0;
    return 1;
}
int is_add  (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != OP)
        return 0;
    if (cur.lex.op != ADD)
        return 0;
    return 1;
}

int is_mul_div (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != OP)
        return 0;
    if (cur.lex.op != MUL && cur.lex.op != DIV)
        return 0;
    return 1;
}

int is_mul ( struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != OP)
        return 0;
    if (cur.lex.op != MUL)
        return 0;
    return 1;
}

int is_l_brace ( struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != BRACE)
        return 0;
    if (cur.lex.b != LBRAC)
        return 0;
    return 1;
}

int is_r_brace ( struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != BRACE)
        return 0;
    if (cur.lex.b != RBRAC)
        return 0;
    return 1;
}

int is_number (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != NUM)
        return 0;
    return 1;
}

int is_var (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != VAR)
        return 0;
    return 1;
}


// expr ::= term {+, -} term | term
struct node_t *parse_expr (struct lexer_state *pstate)
{
    struct node_t *lhs  = NULL;
    struct node_t *rhs  = NULL;
    struct node_t *expr = NULL;
    struct node_t *temp = NULL;

    lhs = parse_term (pstate);
    expr = lhs;

    while (pstate->cur < pstate->lexarr.size && current(pstate).kind == OP && is_plus_minus (pstate))
    {
        temp = calloc (1, sizeof (struct node_t));
        temp->data.kind   = OP;
        temp->data.lex.op = SUB;
        if (is_add (pstate))
            temp->data.lex.op = ADD;
        pstate->cur += 1;

        rhs = parse_term (pstate);
        #if 1
        if (rhs == NULL)
            return NULL;
        #endif
        
        temp->left  = expr;
        temp->right = rhs;
        expr = temp;
    }

    return expr;
}



// term ::= factor {*, /} factor | factor
struct node_t *parse_term  (struct lexer_state *pstate)
{
    struct node_t *lhs  = NULL;
    struct node_t *rhs  = NULL;
    struct node_t *term = NULL;
    struct node_t *temp = NULL;
    lhs = parse_factor (pstate);
    term = lhs;

    while (pstate->cur < pstate->lexarr.size && current(pstate).kind == OP && is_mul_div (pstate))
    {
        temp = calloc (1, sizeof (struct node_t));
        temp->data.kind   = OP;
        temp->data.lex.op = DIV;
        if (is_mul (pstate))
            temp->data.lex.op = MUL;
        pstate->cur += 1;

        rhs = parse_factor (pstate);
        #if 1
        if (rhs == NULL)
            return NULL;
        #endif
        temp->left  = term;
        temp->right = rhs;
        term = temp;
    }

    return term;
}

// factor ::= ( expr ) | number
struct node_t *parse_factor (struct lexer_state *pstate)
{
    struct node_t *expr = NULL;
    if (is_l_brace (pstate))
    {
        pstate->cur += 1;
        expr = parse_expr (pstate);
        if (expr == NULL)
            return 0;
        if (!is_r_brace (pstate))
        {
            printf ("ERROR\n");
            return 0;
        }
        pstate->cur += 1;   
        return expr;
    }
    if (is_number (pstate) || is_var (pstate))//div to 2 ways
    {
        if (is_number (pstate))
        {
            struct node_t *num = calloc (1, sizeof (struct node_t));
            num->data.kind = NUM;
            num->data.lex.num = current(pstate).lex.num;
            pstate->cur += 1;
            return num;
        }
        else if (is_var (pstate))
        {
            struct node_t *var = calloc (1, sizeof (struct node_t));
            var->data.kind = VAR;
            var->data.lex.name = (current(pstate)).lex.name;
            pstate->cur += 1;
            return var;
        }
    }
    return NULL;
}

struct node_t *build_syntax_tree(struct lex_array_t lexarr)
{
    int cur = 0;
    struct node_t *res = NULL;
    struct lexer_state ls = {cur, lexarr};
    
    res = parse_expr (&ls);
    if (ls.cur != ls.lexarr.size && res != 0)
    {
        printf ("ERROR\n");
        return 0;
    }
    return res;
}

int calc_result(struct node_t *top) //inorder calculation
{
    struct node_t *cur = top;
    int val_l = 0;
    int val_r = 0;

    if (cur->left)
        val_l = calc_result (cur->left);
    if (cur->right)
        val_r = calc_result (cur->right);

    if (top->data.kind == OP)
    {
        if (top->data.lex.op == ADD)
            return val_l + val_r;
        else if (top->data.lex.op == SUB)
            return val_l - val_r;
        else if (top->data.lex.op == MUL)
            return val_l * val_r;
        else if (top->data.lex.op == DIV)
            return val_l / val_r;
    }
    return top->data.lex.num;
}

void tree_dump (struct node_t *top)
{
    FILE *dotfile = fopen ("dump.dot", "w");
    DtStart (dotfile);
    DtSetNodes (dotfile, top);
    DtSetDependencies (dotfile, top);
    DtEnd (dotfile);
    fclose (dotfile);
    system ("dot dump.dot -T png -o dump.png");
}



void free_syntax_tree(struct node_t * top) 
{
    if (top->left)
        free_syntax_tree (top->left);
    if (top->right)
        free_syntax_tree (top->right);
    free (top);
}