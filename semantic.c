#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

#define MAX_VARS 100
#define MAX_MATRIX_ROWS 100
#define MAX_MATRIX_COLS 100

typedef struct {
    char name[50];
    Matrix *matrix;
} MatrixVariable;

MatrixVariable var_table[MAX_VARS];
int var_count = 0;

float temp_matrix_data[MAX_MATRIX_ROWS][MAX_MATRIX_COLS];
int temp_row_count = 0;
int temp_col_count = 0;
int current_row_size = 0;

void init_symbol_table() {
    var_count = 0;
    printf("Table des symboles initialisee.\n");
}

Matrix* create_matrix(int rows, int cols) {
    Matrix *mat = (Matrix*)malloc(sizeof(Matrix));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (float*)malloc(rows * cols * sizeof(float));
    return mat;
}

void free_matrix(Matrix *mat) {
    if (mat) {
        if (mat->data) free(mat->data);
        free(mat);
    }
}

Matrix* copy_matrix(Matrix *mat) {
    if (!mat) return NULL;
    Matrix *copy = create_matrix(mat->rows, mat->cols);
    memcpy(copy->data, mat->data, mat->rows * mat->cols * sizeof(float));
    return copy;
}

void print_matrix(Matrix *mat) {
    if (!mat) {
        printf("(matrice vide)\n");
        return;
    }
    printf("Matrice %dx%d:\n", mat->rows, mat->cols);
    for (int i = 0; i < mat->rows; i++) {
        printf("  [ ");
        for (int j = 0; j < mat->cols; j++) {
            printf("%.2f ", mat->data[i * mat->cols + j]);
        }
        printf("]\n");
    }
}

void add_matrix_variable(const char *name, Matrix *mat) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_table[i].name, name) == 0) {
            free_matrix(var_table[i].matrix);
            var_table[i].matrix = copy_matrix(mat);
            printf("Variable '%s' mise a jour\n", name);
            return;
        }
    }
    strcpy(var_table[var_count].name, name);
    var_table[var_count].matrix = copy_matrix(mat);
    var_count++;
    printf("Variable '%s' ajoutee\n", name);
}

Matrix* get_matrix_variable(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_table[i].name, name) == 0) {
            return var_table[i].matrix;
        }
    }
    printf("Erreur semantique: variable '%s' non declaree.\n", name);
    return NULL;
}

void print_matrix_variable(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_table[i].name, name) == 0) {
            printf("\n=== Resultat: %s ===\n", name);
            print_matrix(var_table[i].matrix);
            return;
        }
    }
    printf("Erreur: variable '%s' non trouvee.\n", name);
}

Matrix* matrix_add(Matrix *a, Matrix *b) {
    if (!a || !b) return NULL;
    if (a->rows != b->rows || a->cols != b->cols) {
        printf("Erreur: dimensions incompatibles pour l'addition (%dx%d + %dx%d)\n",
               a->rows, a->cols, b->rows, b->cols);
        return NULL;
    }
    Matrix *result = create_matrix(a->rows, a->cols);
    for (int i = 0; i < a->rows * a->cols; i++) {
        result->data[i] = a->data[i] + b->data[i];
    }
    return result;
}

Matrix* matrix_subtract(Matrix *a, Matrix *b) {
    if (!a || !b) return NULL;
    if (a->rows != b->rows || a->cols != b->cols) {
        printf("Erreur: dimensions incompatibles pour la soustraction (%dx%d - %dx%d)\n",
               a->rows, a->cols, b->rows, b->cols);
        return NULL;
    }
    Matrix *result = create_matrix(a->rows, a->cols);
    for (int i = 0; i < a->rows * a->cols; i++) {
        result->data[i] = a->data[i] - b->data[i];
    }
    return result;
}

Matrix* matrix_multiply(Matrix *a, Matrix *b) {
    if (!a || !b) return NULL;
    if (a->cols != b->rows) {
        printf("Erreur: dimensions incompatibles pour la multiplication (%dx%d * %dx%d)\n",
               a->rows, a->cols, b->rows, b->cols);
        return NULL;
    }
    Matrix *result = create_matrix(a->rows, b->cols);
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < b->cols; j++) {
            float sum = 0;
            for (int k = 0; k < a->cols; k++) {
                sum += a->data[i * a->cols + k] * b->data[k * b->cols + j];
            }
            result->data[i * b->cols + j] = sum;
        }
    }
    return result;
}

Matrix* matrix_transpose(Matrix *a) {
    if (!a) return NULL;
    Matrix *result = create_matrix(a->cols, a->rows);
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < a->cols; j++) {
            result->data[j * a->rows + i] = a->data[i * a->cols + j];
        }
    }
    return result;
}

void start_new_matrix() {
    temp_row_count = 0;
    temp_col_count = 0;
}

void start_new_row() {
    current_row_size = 0;
}

void add_number_to_row(float num) {
    if (temp_row_count < MAX_MATRIX_ROWS && current_row_size < MAX_MATRIX_COLS) {
        temp_matrix_data[temp_row_count][current_row_size] = num;
        current_row_size++;
    }
}

Matrix* finalize_matrix() {
    if (temp_row_count == 0) return NULL;
    Matrix *mat = create_matrix(temp_row_count, temp_col_count);
    for (int i = 0; i < temp_row_count; i++) {
        for (int j = 0; j < temp_col_count; j++) {
            mat->data[i * temp_col_count + j] = temp_matrix_data[i][j];
        }
    }
    return mat;
}

int get_last_row_size() {
    return current_row_size;
}
