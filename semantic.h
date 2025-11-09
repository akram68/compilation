#ifndef SEMANTIC_H
#define SEMANTIC_H

typedef struct {
    float *data;
    int rows;
    int cols;
} Matrix;

typedef struct {
    Matrix *matrix;
} MatrixWrapper;

void init_symbol_table();
void add_matrix_variable(const char *name, Matrix *mat);
Matrix* get_matrix_variable(const char *name);
void print_matrix_variable(const char *name);

Matrix* create_matrix(int rows, int cols);
void free_matrix(Matrix *mat);
Matrix* copy_matrix(Matrix *mat);
void print_matrix(Matrix *mat);

Matrix* matrix_add(Matrix *a, Matrix *b);
Matrix* matrix_subtract(Matrix *a, Matrix *b);
Matrix* matrix_multiply(Matrix *a, Matrix *b);
Matrix* matrix_transpose(Matrix *a);

void start_new_matrix();
void start_new_row();
void add_number_to_row(float num);
Matrix* finalize_matrix();
int get_last_row_size();

#endif
