#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED

#include "../lexer/lexer.h"


struct lexer_state {
    int cur;
    struct lex_array_t lexarr;
};



struct node_t {
  struct node_t *left, *right;
  struct lexem_t data;
};



struct lexem_t current (struct lexer_state *pstate);
int is_add        (struct lexer_state *pstate);
int is_mul_div    (struct lexer_state *pstate);
int is_plus_minus (struct lexer_state *pstate);
int is_mul        (struct lexer_state *pstate);
int is_l_brace    (struct lexer_state *pstate);
int is_r_brace    (struct lexer_state *pstate);
int is_number     (struct lexer_state *pstate);

struct node_t *parse_term   (struct lexer_state *pstate);
struct node_t *parse_expr   (struct lexer_state *pstate);
struct node_t *parse_factor (struct lexer_state *pstate);

struct node_t *build_syntax_tree(struct lex_array_t lexarr);
int  calc_result      (struct node_t *top);
void tree_dump        (struct node_t *top);  
void free_syntax_tree (struct node_t * top);




#endif