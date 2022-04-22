#ifndef DERIVATIVE_INCLUDED
#define DERIVATIVE_INCLUDED

#include "../parser/parser.h"

struct node_t *SubTreeCpy (const struct node_t *src);
struct node_t *Derivate (const struct node_t *top);
int is_const (struct node_t *top);

#endif