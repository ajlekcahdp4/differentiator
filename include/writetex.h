#ifndef WRITETEX_INCLUDED
#define WRITETEX_INCLUDED

#include "parser.h"

void PrintStart (FILE* f);
void DumpDerivate (FILE *f, struct node_t *top);
void PrintEnd (FILE* f);

#endif