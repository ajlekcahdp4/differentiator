#ifndef DUMP_INCLUDED
#define DUMP_INCLUDED

struct node_t;

void DtStart    (FILE* dotfile);
void DtSetNode  (FILE* dotfile, struct node_t *node);
void DtSetDependencies (FILE *dotfile, struct node_t *top);
void DtEnd      (FILE* dotfile);
void DtSetNodes (FILE *dotfile, struct node_t *top);

#endif