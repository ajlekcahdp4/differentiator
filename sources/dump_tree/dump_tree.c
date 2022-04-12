#include <stdio.h>
#include <stdlib.h>
#include "dump_tree.h"
#include "../lexer/lexer.h"
#include "../parser/parser.h"



void DtStart (FILE* dotfile)
{
    fprintf (dotfile, "digraph G {\n");
    fprintf (dotfile, "rankdir = TB\n");
}

void DtSetNode (FILE* dotfile, struct node_t *node)
{
    fprintf (dotfile, "Node%p [style=filled, fillcolor = \"palegreen\" ,label=\"", node);
    switch (node->data.kind)
    {
        case OP:
            switch (node->data.lex.op)
            {
                case ADD:
                    fprintf (dotfile, "+");
                    break;
                case SUB:
                    fprintf (dotfile, "-");
                    break;
                case MUL:
                    fprintf (dotfile, "*");
                    break;
                case DIV:
                    fprintf (dotfile, "/");
                    break;
                case SIN:
                    fprintf (dotfile, "sin");
                    break;
                case COS:
                    fprintf (dotfile, "cos");
                    break;
                case LOG:
                    fprintf (dotfile, "log");
                    break;
                case EXP:
                    fprintf (dotfile, "exp");
                    break;
                case DEG:
                    fprintf (dotfile, "^");
                    break;
                default:
                    printf ("ERROR: unknown operators type\n");
                    break;
            }
            break;
        case NUM:
            fprintf (dotfile, "%d", node->data.lex.num);
            break;
        case VAR:
            fprintf (dotfile, "%s", node->data.lex.name);
            break;
        case BRACE:
            break;
        default:
            printf ("ERROR: unknown lexem type %d\n", node->data.kind);
            break;
    }
    fprintf (dotfile, "\"];\n");
}

void DtSetNodes (FILE *dotfile, struct node_t *top)
{
    DtSetNode (dotfile, top);
    if (top->left)
        DtSetNodes (dotfile, top->left);
    if (top->right)
        DtSetNodes (dotfile, top->right);
}

void DtSetDependencies (FILE *dotfile, struct node_t *top)
{
    if (top->left)
    {
        fprintf (dotfile, "Node%p->Node%p[color = \"red\"]\n", top, top->left);
        DtSetDependencies (dotfile, top->left);
    }
    if (top->right)
    {
        fprintf (dotfile, "Node%p->Node%p[color = \"red\"]\n", top, top->right);
        DtSetDependencies (dotfile, top->right);
    }
    fprintf (dotfile, "\n");
}

void DtEnd (FILE* dotfile)
{
    fprintf (dotfile, "}\n");
} 

