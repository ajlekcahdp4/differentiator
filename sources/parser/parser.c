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
int is_div (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != OP)
        return 0;
    if (cur.lex.op != DIV)
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
int is_op (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != OP)
        return 0;
    return 1;
}

int is_sin (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != OP)
        return 0;
    if (cur.lex.op != SIN)
        return 0;
    return 1;
}

int is_cos (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != OP)
        return 0;
    if (cur.lex.op != COS)
        return 0;
    return 1;
}

int is_log (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != OP)
        return 0;
    if (cur.lex.op != LOG)
        return 0;
    return 1;
}

int is_deg (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != OP)
        return 0;
    if (cur.lex.op != DEG)
        return 0;
    return 1;
}


int is_exp (struct lexer_state *pstate)
{
    struct lexem_t cur = current (pstate);
    if (cur.kind != OP)
        return 0;
    if (cur.lex.op != EXP)
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

    while (pstate->cur < pstate->lexarr.size && is_plus_minus (pstate))
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



// term ::= deg {*, /, ^} deg | deg |
struct node_t *parse_term  (struct lexer_state *pstate)
{
    struct node_t *lhs  = NULL;
    struct node_t *rhs  = NULL;
    struct node_t *term = NULL;
    struct node_t *temp = NULL;
    lhs = parse_deg (pstate);
    term = lhs;

    while (pstate->cur < pstate->lexarr.size && is_mul_div (pstate))
    {
        temp = calloc (1, sizeof (struct node_t));
        temp->data.kind   = OP;
        temp->data.lex.op = DIV;
        if (is_mul (pstate))
            temp->data.lex.op = MUL;
        pstate->cur += 1;

        rhs = parse_deg (pstate);
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

// deg ::= factor ^ factor | factor
struct node_t *parse_deg (struct lexer_state *pstate)
{
    struct node_t *deg  = NULL;
    struct node_t *lhs  = NULL;
    struct node_t *rhs  = NULL;
    struct node_t *temp = NULL;

    lhs = parse_factor (pstate);
    deg = lhs;

    while (pstate->cur < pstate->lexarr.size && is_deg (pstate))
    {
        temp = calloc (1, sizeof (struct node_t));
        temp->data.kind   = OP;
        temp->data.lex.op = DEG;
        pstate->cur += 1;

        rhs = parse_factor (pstate);
        if (rhs == NULL)
            return NULL;
        temp->left = deg;
        temp->right = rhs;
        deg = temp;
    }
    return deg;
}


// factor ::= ( expr ) | number | {sin(expr), cos(expr), ln (expr), exp (expr)}
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
    if (is_op(pstate))
    {
        if (is_sin (pstate))
        {
            struct node_t *sin = calloc (1, sizeof(struct node_t));
            sin->data.kind = OP;
            sin->data.lex.op = current(pstate).lex.op;
            pstate->cur += 1;
            sin->left = parse_factor (pstate);
            return sin;
        }
        else if (is_cos (pstate))
        {
            struct node_t *cos = calloc (1, sizeof(struct node_t));
            cos->data.kind = OP;
            cos->data.lex.op = current(pstate).lex.op;
            pstate->cur += 1;
            cos->left = parse_factor (pstate);
            return cos;
        }
        else if (is_log (pstate))
        {
            
            struct node_t *log = calloc (1, sizeof(struct node_t));
            log->data.kind = OP;
            log->data.lex.op = current(pstate).lex.op;
            pstate->cur += 1;
            log->left = parse_factor (pstate);
            return log;
        }
        else if (is_exp (pstate))
        {
            
            struct node_t *exp = calloc (1, sizeof(struct node_t));
            exp->data.kind = OP;
            exp->data.lex.op = current(pstate).lex.op;
            pstate->cur += 1;
            exp->left = parse_factor (pstate);
            return exp;
        }
        else
        {
            fprintf (stderr, "ERROR: unexpected command in parsing factor\n");
            exit (0);
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
        printf ("ERROR: error while building syntax tree\n");
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