#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "lexer.h"


double number_input (const char *buf, int *i)
{
    double val = 0;
    double weight = 1;


    while (isdigit (buf[*i]))
    {
        val = val * 10 + buf[*i] - '0';
        *i += 1;
    }

    if (buf[*i] == '.')
    {
        *i += 1;
        while (isdigit (buf[*i]))
        {
            weight /= 10;
            val += (buf[*i] - '0') * weight;
            *i += 1;
        }
    }
    return val;
}

int is_brace (char c)
{
    if (c != ')' && c != '(')
        return 0;
    return 1;
}

int skip_spaces (const char* buf, int ip)
{
    while (buf[ip] == ' ' || buf[ip] == '\t' || buf[ip] == '\n' || buf[ip] == '\r')
        ip += 1;
    return ip;
}

#define MAX_VAR_LEN 32
char *ReadVar (const char *buf, int i)
{
    int j = 0;
    char *str = calloc (MAX_VAR_LEN, sizeof(char));
    assert (str);
    char c = buf[i];

    if (!isalpha(c))
    {
        fprintf (stderr, "ERROR: wrong usage of \'%c\' token\n", c);
        free (str);
        return NULL;
    }
    while (isalpha(c))
    {
        str[j] = c;
        j += 1;
        i += 1;
        c = buf[i];
    }
    return str;
}
#undef MAX_VAR_LEN

int Input (char **buf)
{
    char c = 0;
    int len = 0;
    *buf = calloc (MAXLEN, sizeof (char));
    assert (*buf);

    c = (char)getchar ();
    *buf[len] = c;
    len += 1;
    while (c != '\0' && c != EOF && c != '\n')
    {
        c = (char)getchar();
        (*buf)[len] = c;
        len += 1;
    }
    

    return len;
}



void End (char *buf, struct lex_array_t *lex)
{
    if (lex)
    {
        for (int i = 0; i < lex->size; i++)
        {
            if (lex->lexems[i].kind == VAR)
                free (lex->lexems[i].lex.name);
        }
        free (lex->lexems);
        free (lex);
    }
    free (buf);
}


void print_lex (struct lex_array_t *lex)
{
    for (int i = 0; i < lex->size; i++)
    {
        switch (lex->lexems[i].kind )
        {
        case BRACE:
            switch (lex->lexems[i].lex.b)
            {
                case LBRAC:
                    printf ("LBRAC ");
                    break;
                case RBRAC:
                    printf ("RBRAC ");
                    break;
                default:
                    fprintf (stderr, "ERROR: unknown brace type\n");
                    exit(0);
                    break;
            }
            break;
        case OP:
            switch (lex->lexems[i].lex.op)
            {
                case ADD:
                    printf ("PLUS ");
                    break;
                case SUB:
                    printf ("MINUS ");
                    break;
                case MUL:
                    printf ("MUL ");
                    break;
                case DIV:
                    printf ("DIV ");
                    break;
                case SIN:
                    printf ("SIN ");
                    break;
                case COS:
                    printf ("COS ");
                    break;
                case EXP:
                    printf ("EXP ");
                    break;
                case LOG:
                    printf ("LOG ");
                    break;
                case DEG:
                    printf ("DEG ");
                    break;
                default:
                    fprintf (stderr, "ERROR: unknown operation type\n");
                    exit(0);
            }
            break;
        case NUM:
            printf ("NUMBER:%g ", lex->lexems[i].lex.num);
            break;
        case VAR:
            printf ("VAR:%s ", lex->lexems[i].lex.name);
            break;
        default:
            fprintf (stderr, "ERROR: unknow lexem type\n");
            exit(0);
            break;
        }   
    }
    printf ("\n");

}

int lex_insert (struct lex_array_t *lex, const char *buf, int i, int ip)
{
    while (buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\n')
        i += 1;
    if (buf[i] == '\0')
        return i;

    if (buf[i] == '(')
    {
        lex->size += 1;
        lex->lexems[ip].kind  = BRACE;
        lex->lexems[ip].lex.b = LBRAC;
    }
    else if (buf[i] == ')')
    {
        lex->size += 1;
        lex->lexems[ip].kind  = BRACE;
        lex->lexems[ip].lex.b = RBRAC;
    }
    else if (buf[i] == '*')
    {
        lex->size += 1;
        lex->lexems[ip].kind   = OP;
        lex->lexems[ip].lex.op = MUL;
    }
    else if (buf[i] == '/')
    {
        lex->size += 1;
        lex->lexems[ip].kind   = OP;
        lex->lexems[ip].lex.op = DIV;
    }
    else if (buf[i] == '+')
    {
        lex->size += 1;
        lex->lexems[ip].kind   = OP;
        lex->lexems[ip].lex.op = ADD;
    }
    else if (buf[i] == '-')
    {
        lex->size += 1;
        lex->lexems[ip].kind   = OP;
        lex->lexems[ip].lex.op = SUB;
    }
    else if (buf[i] == '^')
    {
        lex->size += 1;
        lex->lexems[ip].kind   = OP;
        lex->lexems[ip].lex.op = DEG;
    }
    else if (strncmp (buf + i, "sin", 3) == 0)
    {

        lex->size += 1;
        lex->lexems[ip].kind = OP;
        lex->lexems[ip].lex.op = SIN;
        i += 2;
    }
    else if (strncmp (buf + i, "cos", 3) == 0)
    {
        lex->size += 1;
        lex->lexems[ip].kind = OP;
        lex->lexems[ip].lex.op = COS;
        i += 2;
    }
    else if (strncmp (buf + i, "exp", 3) == 0)
    {
        lex->size += 1;
        lex->lexems[ip].kind = OP;
        lex->lexems[ip].lex.op = EXP;
        i += 2;
    }
    else if (strncmp (buf + i, "ln", 2) == 0)
    {
        lex->size += 1;
        lex->lexems[ip].kind = OP;
        lex->lexems[ip].lex.op = LOG;
        i += 1;
    }
    else if (isdigit (buf[i]))
    {
        lex->size += 1;
        lex->lexems[ip].kind = NUM;
        lex->lexems[ip].lex.num = number_input (buf, &i);
        i -= 1;
    }
    else
    {
        lex->size += 1;
        lex->lexems[ip].kind = VAR;
        lex->lexems[ip].lex.name = ReadVar (buf, i);
        if (lex->lexems[ip].lex.name == NULL)
            return -1;
        i += strlen(lex->lexems[ip].lex.name) - 1;
    }
    return i + 1;
} 


struct lex_array_t *lex_resize (struct lex_array_t *lex)
{
    lex->capacity = 2 * lex->capacity;
    lex->lexems = realloc (lex->lexems, (size_t)lex->capacity * sizeof (struct lexem_t));
    assert (lex->lexems);
    return lex;
}

struct lex_array_t *lex_asurence (struct lex_array_t *lex)
{
    struct lex_array_t *new_lex = calloc (1, sizeof (struct lex_array_t));
    new_lex->capacity = lex->capacity;
    new_lex->lexems = calloc (new_lex->capacity, sizeof (struct lexem_t));

    int j = 0;
    for (int i = 0; i < lex->size; i++)
    {
        if (new_lex->size == new_lex->capacity)
            lex_resize (new_lex);

        new_lex->lexems[j].kind = lex->lexems[i].kind;
        new_lex->lexems[j].lex  = lex->lexems[i].lex;

        if ((i < lex->size - 1 ) && (lex->lexems[i].kind == NUM) && (lex->lexems[i + 1].kind == VAR))
        {
            j++;
            new_lex->lexems[j].kind = OP;
            new_lex->lexems[j].lex.op = MUL;
            new_lex->size += 1;
        }
        j++;
        new_lex->size += 1;
    }

    free (lex->lexems);
    free (lex);
    
    return new_lex;
}


#define START_CAPACITY 16
struct lex_array_t *lex_string (const char *buf)
{
    int ip = 0;
    int size = 0;
    struct lex_array_t *lex = calloc (1, sizeof (struct lex_array_t));
    lex->lexems   = calloc (START_CAPACITY, sizeof (struct lexem_t));
    assert (lex);
    assert (lex->lexems);
    lex->capacity = START_CAPACITY;
    size = 0;
    ip = 0;
    for (int i = 0; buf[i] != '\0' && buf[i] != EOF && buf[i] != '\n'; )
    {
        if (lex->size == lex->capacity)
            lex_resize (lex);
        i = lex_insert (lex, buf, i, ip);
        if (i == -1)
        {
            free (lex->lexems);
            free (lex);
            return NULL;
        }
        size += 1;
        ip += 1;
    }
    lex->size = size;

    lex = lex_asurence(lex);

    return lex;
}
#undef START_CAPACITY