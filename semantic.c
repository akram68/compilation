#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "semantic.h"

#define MAX_VARS 100
#define MAX_MATRIX_ROWS 100
#define MAX_MATRIX_COLS 100

/* table pour stocker une variable matrice */
typedef struct {
    char name[50];
    Matrix *matrix;
} MatrixVariable;

static MatrixVariable var_table[MAX_VARS];
static int var_count = 0; /* Compteur de var dans la table */

static float temp_matrix_data[MAX_MATRIX_ROWS][MAX_MATRIX_COLS];
int temp_row_count = 0;
int temp_col_count = 0;
int current_row_size = 0;

void init_symbol_table() {
    var_count = 0;
    printf("Table des symboles initialisee.\n");
}

/* création d'une matrice et initialisation du champ name */
Matrix* create_matrix(int rows, int cols) {
    Matrix *mat = (Matrix*)malloc(sizeof(Matrix));
    if (!mat) {
        fprintf(stderr, "Erreur allocation create_matrix\n");
        exit(1);
    }
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (float*)malloc(rows * cols * sizeof(float));
    if (!mat->data) {
        fprintf(stderr, "Erreur allocation data\n");
        free(mat);
        exit(1);
    }
    mat->name = NULL;
    return mat;
}

void free_matrix(Matrix *mat) {
    if (mat) {
        if (mat->data) free(mat->data);
        if (mat->name) free(mat->name);
        free(mat);
    }
}

Matrix* copy_matrix(Matrix *mat) {
    if (!mat) return NULL;
    Matrix *copy = create_matrix(mat->rows, mat->cols);
    memcpy(copy->data, mat->data, mat->rows * mat->cols * sizeof(float));
    /* copie du nom si existant */
    if (mat->name) copy->name = strdup(mat->name);
    else copy->name = NULL;
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

/* Ajoute ou met à jour une variable dans la table des symboles */
void add_matrix_variable(const char *name, Matrix *mat) {
    if (!name || !mat) return;

    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_table[i].name, name) == 0) {
            /* suppression ancienne matrice */
            if (var_table[i].matrix) free_matrix(var_table[i].matrix);
            /* stocke une copie de la nouvelle matrice */
            var_table[i].matrix = copy_matrix(mat);
            /* affecte le nom à la matrice stockée */
            if (var_table[i].matrix) {
                if (var_table[i].matrix->name) free(var_table[i].matrix->name);
                var_table[i].matrix->name = strdup(name);
            }
            printf("Variable '%s' mise a jour\n", name);
            return;
        }
    }
    /* nouvelle entrée */
    if (var_count < MAX_VARS) {
        strncpy(var_table[var_count].name, name, sizeof(var_table[var_count].name)-1);
        var_table[var_count].name[sizeof(var_table[var_count].name)-1] = '\0';
        var_table[var_count].matrix = copy_matrix(mat);
        if (var_table[var_count].matrix) {
            if (var_table[var_count].matrix->name) free(var_table[var_count].matrix->name);
            var_table[var_count].matrix->name = strdup(name);
        }
        var_count++;
        printf("Variable '%s' ajoutee\n", name);
    } else {
        printf("Erreur: table de variables pleine\n");
    }
}

Matrix* get_matrix_variable(const char *name) {
    if (!name) return NULL;
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

/* opérations : on met name = NULL pour que le parser/codegen fixe le nom dest */
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
    result->name = NULL;
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
    result->name = NULL;
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
    result->name = NULL;
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
    result->name = NULL;
    return result;
}

Matrix* matrix_inverse(Matrix *a) {
    if (!a) return NULL;

    if (a->rows != a->cols) {
        printf("Erreur: seules les matrices carrees peuvent etre inversees (%dx%d)\n",
               a->rows, a->cols);
        return NULL;
    }

    int n = a->rows;
    Matrix *result = create_matrix(n, n);

    float *augmented = (float*)malloc(n * 2 * n * sizeof(float));
    if (!augmented) {
        fprintf(stderr, "Erreur allocation pour inversion\n");
        free_matrix(result);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            augmented[i * 2 * n + j] = a->data[i * n + j];
        }
        for (int j = 0; j < n; j++) {
            augmented[i * 2 * n + n + j] = (i == j) ? 1.0f : 0.0f;
        }
    }

    for (int i = 0; i < n; i++) {
        int pivot_row = i;
        float max_val = fabs(augmented[i * 2 * n + i]);
        for (int k = i + 1; k < n; k++) {
            float val = fabs(augmented[k * 2 * n + i]);
            if (val > max_val) {
                max_val = val;
                pivot_row = k;
            }
        }

        if (max_val < 1e-10) {
            printf("Erreur: matrice singuliere, inversion impossible\n");
            free(augmented);
            free_matrix(result);
            return NULL;
        }

        if (pivot_row != i) {
            for (int j = 0; j < 2 * n; j++) {
                float temp = augmented[i * 2 * n + j];
                augmented[i * 2 * n + j] = augmented[pivot_row * 2 * n + j];
                augmented[pivot_row * 2 * n + j] = temp;
            }
        }

        float pivot = augmented[i * 2 * n + i];
        for (int j = 0; j < 2 * n; j++) {
            augmented[i * 2 * n + j] /= pivot;
        }

        for (int k = 0; k < n; k++) {
            if (k != i) {
                float factor = augmented[k * 2 * n + i];
                for (int j = 0; j < 2 * n; j++) {
                    augmented[k * 2 * n + j] -= factor * augmented[i * 2 * n + j];
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            result->data[i * n + j] = augmented[i * 2 * n + n + j];
        }
    }

    free(augmented);
    result->name = NULL;
    return result;
}

void reset_matrix_builder() {
    temp_row_count = 0;
    temp_col_count = 0;
    current_row_size = 0;
    for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
        for (int j = 0; j < MAX_MATRIX_COLS; j++) {
            temp_matrix_data[i][j] = 0.0f;
        }
    }
}

void start_new_matrix() {
    reset_matrix_builder();
}

void start_new_row() {
    current_row_size = 0;
}

void add_number_to_row(float num) {
    if (temp_row_count < MAX_MATRIX_ROWS && current_row_size < MAX_MATRIX_COLS) {
        temp_matrix_data[temp_row_count][current_row_size] = num;
        current_row_size++;
    } else {
        printf("Erreur: depassement dimensions temporaires\n");
    }
}

Matrix* finalize_matrix() {
    int rows = temp_row_count;
    if (rows <= 0) rows = 1;
    int cols = temp_col_count;
    if (cols <= 0) cols = current_row_size;

    Matrix *mat = create_matrix(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mat->data[i * cols + j] = temp_matrix_data[i][j];
        }
    }
    mat->name = NULL;
    return mat;
}

int get_last_row_size() {
    return current_row_size;
}
