%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.yy.h"
#include "semantic.h"
#include "parser.tab.h"
#include "codegen.h"

void yyerror(const char *s);/* Déclaration fonction d'erreur */
int yylex(void);/* Déclaration fonction lexicale */

int first_row_size = -1;
extern int temp_row_count;/* Compteur temporaire de lignes et l'autre de colonnes*/
extern int temp_col_count;
%}
/* Définition du type union pour stocker différentes valeurs */
%union {
    float fval;/* Pour les nombres */
    char* sval;/* Pour les identifiants */
    Matrix* matrix;/* Pour les matrices */
}
/* Déclaration des tokens avec leurs types */
%token <sval> ID        /* Token ID de type chaîne */
%token <fval> NUMBER    /* Token ID de type float */
%token PRINT LBRACKET RBRACKET SEMICOLON COMMA ASSIGN PLUS MINUS MULT TRANSPOSE

%type <matrix> expr term factor matrix
%type <fval> row

%start program

%%

program:
      /* vide */
    | program statement
    ;

statement:
      ID ASSIGN expr SEMICOLON
        {
            printf("Affectation: %s = (expression)\n", $1);
            add_matrix_variable($1, $3);
             generate_assignment($1, $3); 
            free_matrix($3);
            free($1);
        }
    | PRINT '(' ID ')' SEMICOLON
        {
            printf("Affichage de la variable: %s\n", $3);
            print_matrix_variable($3);
            generate_print($3);
            free($3);
        }
    | error SEMICOLON
        {
            printf("Erreur syntaxique: Erreur de syntaxe detectee !\n");
            yyclearin;  /* Efface le token en erreur */
            yyerrok;    /* Reprend l'analyse normale */
        }
    | error '\n'
        {
            printf("Erreur syntaxique: instruction incomplete ou point-virgule manquant.\n");
            yyclearin;
            yyerrok;
        }
    ;

expr:
      expr PLUS term
    {
        printf("-> Addition detectee\n");
        Matrix *result = matrix_add($1, $3);
        char *dest = new_temp();

    generate_operation("ADD", dest, $1->name, $3->name);

    result->name = dest;
    $$ = result;

    free_matrix($1);
    free_matrix($3);
}

    | expr MINUS term
{
    printf("-> Soustraction detectee\n");
    Matrix *result = matrix_subtract($1, $3);
    char *dest = new_temp();

    generate_operation("SUB", dest, $1->name, $3->name);

    result->name = dest;
    $$ = result;

    free_matrix($1);
    free_matrix($3);
}

    | term
        { $$ = $1; }
    ;

term:
     term MULT factor
{
    printf("-> Multiplication detectee\n");
    Matrix *result = matrix_multiply($1, $3);
    char *dest = new_temp();

    generate_operation("MUL", dest, $1->name, $3->name);

    result->name = dest;
    $$ = result;

    free_matrix($1);
    free_matrix($3);
}

    | factor
        { $$ = $1; }
    ;

factor:
      ID
        {
            Matrix *m = get_matrix_variable($1);
            $$ = copy_matrix(m);
            free($1);
        }
    | ID TRANSPOSE
{
    printf("-> Transposition detectee\n");
    Matrix *m = get_matrix_variable($1);
    Matrix *result = matrix_transpose(m);

    char *dest = new_temp();
    generate_transpose(dest, $1);

    result->name = dest;
    $$ = result;

    free($1);
}

    | NUMBER
        {
            $$ = create_matrix(1, 1);
            $$->data[0] = $1; /* Affecte la valeur */
        }
    | LBRACKET matrix RBRACKET
        { $$ = $2; }
    ;

matrix:
      row
        {
            first_row_size = get_last_row_size();
            temp_col_count = first_row_size;
            temp_row_count = 1;
            $$ = finalize_matrix(); /* Crée la matrice finale */
        }
    | matrix SEMICOLON row
        {
            int current_size = get_last_row_size();/* Vérification de cohérence des dimensions */
            if (current_size != first_row_size) {
                printf("Erreur semantique: lignes de tailles differentes dans la matrice.\n");
            }
            temp_row_count++;
            $$ = finalize_matrix();/* Recrée la matrice avec la nouvelle ligne */
        }
    ;

row:
      NUMBER
        {
            start_new_row();
            add_number_to_row($1);
            $$ = 0;
        }
    | row COMMA NUMBER
        {
            add_number_to_row($3);
            $$ = 0;
        }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erreur syntaxique: %s\n", s);
}
