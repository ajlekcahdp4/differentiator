#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <assert.h>
#include "writetex.h"
#include "tree.h"

int PrintDerivate (FILE *f, struct node_t *top);

#if 0
char* phrase[20] = {0};

phrase[0] = "Несложно заметить, что:\n";
phrase[1] = "Очевидно, что:\n";
phrase[2] = "Читателю уже должно быть очевидно, что:\n";
#endif

void PrintStart (FILE* f)
{   
    fprintf (f, "\\documentclass{article}\n\\usepackage[T2A]{fontenc}\n\\usepackage[utf8]{inputenc}\n\\usepackage[russian]{babel}\n");
    fprintf (f, "\\usepackage{graphicx}\n\\graphicspath{ {./images/} }\n");
    fprintf (f, "\\usepackage{wrapfig}\n\\usepackage{amsmath}\n");
    fprintf (f, "\\usepackage[left = 3cm, right = 2cm, top = 2cm]{geometry}\n");
    fprintf (f, "\\title{Нахождение криволинейных производных в замкнутом множестве колец псевдодействительных чисел}\n");
    fprintf (f, "\\author{Александр Романов Б01-107}\n\n");
    fprintf (f, "\\begin{document}\n");
    fprintf (f, "\\maketitle\n\n");
}


void DumpDerivate (FILE *f, struct node_t *top)
{
    fprintf (f, "\\[ ");
    PrintDerivate (f, top);
    fprintf (f, "\\] \n");
}


int NeedBraces (struct node_t *top)
{
    if (TreeDepth (top) == 1)
        return 0;
    if (top->data.lex.op == MUL)
        return 0;
    if (top->data.lex.op == DIV)
        return 0;
    if (top->data.lex.op == DEG)
        return 0;
    if (top->data.lex.op == SIN)
        return 0;
    if (top->data.lex.op == COS)
        return 0;
    if (top->data.lex.op == LOG)
        return 0;
    if (top->data.lex.op == EXP)
        return 0;
    return 1;
}


int PrintDerivate (FILE *f, struct node_t *top)
{
    if (top == NULL)
        return 0;
    
    switch (top->data.kind)
    {
    case NUM:
        fprintf (f, "%.4g ", top->data.lex.num);
        return 0;
    case VAR:
        fprintf (f, "%s ", top->data.lex.name);
        return 0;
    case OP:
    {
        switch (top->data.lex.op)
        {
        case MUL:
            if (NeedBraces (top->left))
            {
                fprintf (f, "\\left( ");
                PrintDerivate (f, top->left);
                fprintf (f, "\\right) ");
            }
            else
                PrintDerivate (f, top->left);

            fprintf (f, "\\cdot ");

            if (NeedBraces (top->right))
            {
                fprintf (f, "\\left( ");
                PrintDerivate (f, top->right);
                fprintf (f, "\\right) ");
            }
            else
                PrintDerivate (f, top->right);
            return 0;
        case DIV:
            fprintf (f, "\\frac{ ");
            PrintDerivate (f, top->left);
            fprintf (f, "}{ ");
            PrintDerivate (f, top->right);
            fprintf (f, "} ");
            return 0;
        case ADD:
            PrintDerivate (f, top->left);
            fprintf (f, "+ ");
            PrintDerivate (f, top->right);
            return 0;
        case SUB:
            PrintDerivate (f, top->left);
            fprintf (f, "- ");
            PrintDerivate (f, top->right);
            return 0;
        case SIN:
            fprintf (f, "\\sin");
            if (TreeDepth (top->left) > 1)
            {
                fprintf (f, "\\left( ");
                PrintDerivate (f, top->left);
                fprintf (f, "\\right) ");
            }
            else
                PrintDerivate (f, top->left);
            return 0;
        case COS:
            fprintf (f, "\\cos");
            if (TreeDepth (top->left) > 1)
            {
                fprintf (f, "\\left( ");
                PrintDerivate (f, top->left);
                fprintf (f, "\\right) ");
            }
            else
                PrintDerivate (f, top->left);
            return 0;
            return 0;
        case LOG:
            fprintf (f, "ln");
            if (TreeDepth (top->left) > 1)
            {
                fprintf (f, "\\left( ");
                PrintDerivate (f, top->left);
                fprintf (f, "\\right) ");
            }
            else
                PrintDerivate (f, top->left);
            return 0;
        case EXP:
            fprintf (f, "e^{ ");
            PrintDerivate (f, top->left);
            fprintf (f, "} ");
            return 0;
        case DEG:
            if (TreeDepth (top->left) > 1)
            {
                fprintf (f, "\\left( ");
                PrintDerivate (f, top->left);
                fprintf (f, "\\right) ");
            }
            else
                PrintDerivate (f, top->left);
            fprintf (f, "^{ ");
            PrintDerivate (f, top->right);
            fprintf (f, "} ");
            return 0;
        default:
            fprintf (stderr, "ERROR: unexpected operator in writing tex\n");
            return 1;
        }
    }
    default:
        fprintf (stderr, "ERROR: unexpected node type in writing tex\n");
        return 1;
    }
}

void PrintEnd (FILE* f)
{
    fprintf (f, "\\section{Краткие выводы:}\n\n");
    fprintf (f, "\\end{document}");
}