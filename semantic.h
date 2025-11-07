#ifndef SEMANTIC_H
#define SEMANTIC_H

void add_variable(const char *name, float value);
float get_variable_value(const char *name);
void print_variable(const char *name);
void init_symbol_table();
void start_new_row();
void add_number_to_row(float num);
int get_last_row_size();

#endif
