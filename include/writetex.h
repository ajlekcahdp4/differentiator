#ifndef WRITETEX_INCLUDED
#define WRITETEX_INCLUDED

#include "parser.h"

void PrintStart (FILE* f, struct node_t *top, size_t deg);
void DumpDerivate (FILE *f, struct node_t *top);
void PrintEnd (FILE* f, struct node_t *top);

#endif