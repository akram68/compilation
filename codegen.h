#ifndef CODEGEN_H
#define CODEGEN_H

#include "semantic.h"
char *new_temp();

void init_codegen();
void generate_assignment(const char *varname, Matrix *mat);
void generate_operation(const char *op, const char *dest, const char *a, const char *b);
void generate_transpose(const char *dest, const char *src);
void generate_inverse(const char *dest, const char *src);
void generate_print(const char *varname);
void generate_finish();

#endif