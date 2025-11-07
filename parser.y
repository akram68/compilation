%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.yy.h"
#include "semantic.h"

void yyerror(const char *s);
int yylex(void);

// compteur global pour vérifier la cohérence des matrices
int current_row_size = -1;
%}

%union {
    float fval;
    char* sval;
}

%token <sval> ID
%token <fval> NUMBER
%token PRINT LBRACKET RBRACKET SEMICOLON COMMA ASSIGN PLUS MINUS MULT TRANSPOSE

%type <fval> expr term factor
%type <fval> matrix row

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
            add_variable($1, $3);  // analyse sémantique : enregistrer la variable
            free($1);
        }
    | PRINT '(' ID ')' SEMICOLON
        {
            printf("Affichage de la variable: %s\n", $3);
            print_variable($3);     // vérifie si la variable existe
            free($3);
        }
    | error SEMICOLON
        {
            printf("Erreur syntaxique: Erreur de syntaxe detectee !\n");
            yyclearin;
            yyerrok;
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
            $$ = $1 + $3;
        }
    | expr MINUS term
        {
            printf("-> Soustraction detectee\n");
            $$ = $1 - $3;
        }
    | term
        { $$ = $1; }
    ;

term:
      term MULT factor
        {
            printf("-> Multiplication détectee\n");
            $$ = $1 * $3;
        }
    | factor
        { $$ = $1; }
    ;

factor:
      ID
        {
            $$ = get_variable_value($1); // analyse sémantique : vérifier l’existence
            free($1);
        }
    | ID TRANSPOSE
        {
            printf("-> Transposition détectee\n");
            $$ = get_variable_value($1); // symbolique, on pourrait faire transpose plus tard
            free($1);
        }
    | NUMBER
        { $$ = $1; }
    | LBRACKET matrix RBRACKET
        { $$ = $2; }
    ;

matrix:
      row
        { $$ = 0; current_row_size = get_last_row_size(); }
    | matrix SEMICOLON row
        {
            if (get_last_row_size() != current_row_size)
                printf(" Erreur sémantique: lignes de tailles differentes dans la matrice.\n");
            $$ = 0;
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
