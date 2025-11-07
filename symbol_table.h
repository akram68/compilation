#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct
{
    char *name;
    int rows;
    int cols;
    /* we don't store values here to keep it simple (could be added) */
    int initialized;
} Symbol;

void init_symbol_table(void);
void add_symbol(const char *name, int rows, int cols);
Symbol *get_symbol(const char *name);
void update_symbol(const char *name, int rows, int cols);
void print_symbol_table(void);

#endif
