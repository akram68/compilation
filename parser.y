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
%token PRINT INV LBRACKET RBRACKET SEMICOLON COMMA ASSIGN PLUS MINUS MULT TRANSPOSE

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
            if ($3) {
                add_matrix_variable($1, $3);
                generate_assignment($1, $3);
                free_matrix($3);
            } else {
                printf("Erreur: expression invalide\n");
            }
            free($1);
            reset_matrix_builder();
            first_row_size = -1;
        }
    | PRINT '(' ID ')' SEMICOLON
        {
            printf("Affichage de la variable: %s\n", $3);
            print_matrix_variable($3);
            generate_print($3);
            free($3);
            reset_matrix_builder();
        }
    | error SEMICOLON
        {
            printf("Erreur syntaxique: Erreur de syntaxe detectee !\n");
            yyclearin;
            yyerrok;
            reset_matrix_builder();
        }
    | error '\n'
        {
            printf("Erreur syntaxique: instruction incomplete ou point-virgule manquant.\n");
            yyclearin;
            yyerrok;
            reset_matrix_builder();
        }
    ;

expr:
      expr PLUS term
        {
            printf("-> Addition detectee\n");
            if (!$1 || !$3) {
                $$ = NULL;
            } else {
                Matrix *result = matrix_add($1, $3);
                if (result) {
                    char *dest = new_temp();
                    char *name1 = $1->name ? $1->name : "tmp";
                    char *name3 = $3->name ? $3->name : "tmp";
                    generate_operation("ADD", dest, name1, name3);
                    result->name = dest;
                }
                $$ = result;
            }
            free_matrix($1);
            free_matrix($3);
        }
    | expr MINUS term
        {
            printf("-> Soustraction detectee\n");
            if (!$1 || !$3) {
                $$ = NULL;
            } else {
                Matrix *result = matrix_subtract($1, $3);
                if (result) {
                    char *dest = new_temp();
                    char *name1 = $1->name ? $1->name : "tmp";
                    char *name3 = $3->name ? $3->name : "tmp";
                    generate_operation("SUB", dest, name1, name3);
                    result->name = dest;
                }
                $$ = result;
            }
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
            if (!$1 || !$3) {
                $$ = NULL;
            } else {
                Matrix *result = matrix_multiply($1, $3);
                if (result) {
                    char *dest = new_temp();
                    char *name1 = $1->name ? $1->name : "tmp";
                    char *name3 = $3->name ? $3->name : "tmp";
                    generate_operation("MUL", dest, name1, name3);
                    result->name = dest;
                }
                $$ = result;
            }
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
            if (m) {
                $$ = copy_matrix(m);
                if ($$) $$->name = strdup($1);
            } else {
                $$ = NULL;
            }
            free($1);
        }
    | ID TRANSPOSE
        {
            printf("-> Transposition detectee\n");
            Matrix *m = get_matrix_variable($1);
            if (!m) {
                $$ = NULL;
            } else {
                Matrix *result = matrix_transpose(m);
                char *dest = new_temp();
                generate_transpose(dest, $1);
                if (result) result->name = dest;
                $$ = result;
            }
            free($1);
        }
    | INV '(' ID ')'
        {
            printf("-> Inversion detectee\n");
            Matrix *m = get_matrix_variable($3);
            if (!m) {
                $$ = NULL;
            } else {
                Matrix *result = matrix_inverse(m);
                if (result) {
                    char *dest = new_temp();
                    generate_inverse(dest, $3);
                    result->name = dest;
                }
                $$ = result;
            }
            free($3);
        }
    | NUMBER
        {
            $$ = create_matrix(1, 1);
            $$->data[0] = $1;
        }
    | LBRACKET matrix RBRACKET
        { $$ = $2; }
    | '(' expr ')'
        { $$ = $2; }
    ;

matrix:
      row
        {
            first_row_size = get_last_row_size();
            temp_col_count = first_row_size;
            temp_row_count = 1;
            $$ = finalize_matrix();
        }
    | matrix SEMICOLON row
        {
            int current_size = get_last_row_size();
            if (current_size != first_row_size) {
                printf("Erreur semantique: lignes de tailles differentes dans la matrice.\n");
                $$ = NULL;
            } else {
                temp_row_count++;
                $$ = finalize_matrix();
            }
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
