#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

static FILE *out = NULL;
static int tmp_counter = 0;
char *new_temp()
{
    char *buf = malloc(16);
    sprintf(buf, "t%d", tmp_counter++);
    return buf;
}


void init_codegen()
{
    out = fopen("out.txt", "w");
    if (!out)
    {
        fprintf(stderr, "Erreur: impossible d'ouvrir out.asm\n");
        exit(1);
    }
    fprintf(out, "; --- CODE GENERE ---\n");
}

void generate_assignment(const char *varname, Matrix *mat)
{
    if (!out)
        return;
    fprintf(out, "STORE %s %d %d\n", varname, mat->rows, mat->cols);
}

void generate_operation(const char *op, const char *dest, const char *a, const char *b)
{
    if (!out)
        return;
    fprintf(out, "%s %s %s %s\n", op, dest ? dest : "-", a ? a : "-", b ? b : "-");
}

void generate_transpose(const char *dest, const char *src)
{
    if (!out)
        return;
    fprintf(out, "TRANS %s %s\n", dest, src);
}

void generate_print(const char *varname)
{
    if (!out)
        return;
    fprintf(out, "PRINT %s\n", varname);
}

void generate_finish()
{
    if (!out)
        return;
    fprintf(out, "; --- FIN ---\n");
    fclose(out);
    out = NULL;
}