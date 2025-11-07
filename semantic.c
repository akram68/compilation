#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

#define MAX_VARS 100
#define MAX_ROW 100

typedef struct
{
    char name[50];
    float value;
} Variable;

Variable var_table[MAX_VARS];
int var_count = 0;

float last_row[MAX_ROW];
int last_row_size = 0;

void add_variable(const char *name, float value)
{
    for (int i = 0; i < var_count; i++)
    {
        if (strcmp(var_table[i].name, name) == 0)
        {
            var_table[i].value = value;
            printf("Variable '%s' mise à jour à %.2f\n", name, value);
            return;
        }
    }
    strcpy(var_table[var_count].name, name);
    var_table[var_count].value = value;
    var_count++;
    printf("Variable '%s' ajoutée avec valeur %.2f\n", name, value);
}

void init_symbol_table()
{
    var_count = 0;
    printf("Table des symboles initialisée.\n");
}

float get_variable_value(const char *name)
{
    for (int i = 0; i < var_count; i++)
    {
        if (strcmp(var_table[i].name, name) == 0)
            return var_table[i].value;
    }
    printf("⚠️ Erreur sémantique: variable '%s' non déclarée.\n", name);
    return 0;
}

void print_variable(const char *name)
{
    for (int i = 0; i < var_count; i++)
    {
        if (strcmp(var_table[i].name, name) == 0)
        {
            printf("%s = %.2f\n", name, var_table[i].value);
            return;
        }
    }
    printf("⚠️ Erreur: variable '%s' non trouvée.\n", name);
}

// Gestion de matrices
void start_new_row() { last_row_size = 0; }
void add_number_to_row(float num)
{
    if (last_row_size < MAX_ROW)
        last_row[last_row_size++] = num;
}
int get_last_row_size() { return last_row_size; }
