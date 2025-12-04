#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

#define MAX_SYMBOLS 256

static Symbol table[MAX_SYMBOLS];
static int symbol_count = 0;

void init_symbol_table(void)
{
    symbol_count = 0;
}

void add_symbol(const char *name, int rows, int cols)
{
    if (get_symbol(name))
        return;
    if (symbol_count >= MAX_SYMBOLS)
        return;
    table[symbol_count].name = strdup(name);
    table[symbol_count].rows = rows;
    table[symbol_count].cols = cols;
    table[symbol_count].initialized = 1;
    symbol_count++;
}

Symbol *get_symbol(const char *name)
{
    for (int i = 0; i < symbol_count; ++i)
    {
        if (strcmp(table[i].name, name) == 0)
            return &table[i];
    }
    return NULL;
}

void update_symbol(const char *name, int rows, int cols)
{
    Symbol *s = get_symbol(name);
    if (s)
    {
        s->rows = rows;
        s->cols = cols;
        s->initialized = 1;
    }
    else
    {
        add_symbol(name, rows, cols);
    }
}

void print_symbol_table(void)
{
    printf("Table des symboles:\n");
    for (int i = 0; i < symbol_count; ++i)
    {
        printf(" - %s : %dx%d\n", table[i].name, table[i].rows, table[i].cols);
    }
}
