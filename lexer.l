%option noyywrap 
/* Désactive la fonction yywrap (simplifie l'utilisation) */


%{
#include "semantic.h"
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
extern YYSTYPE yylval; /* Variable globale pour communiquer avec Bison*/
%}

/* rules */
%%
[ \t\r\n]+                ;             /* ignorer espaces et retours ligne */

"print"                   { yylval.sval = strdup("print"); return PRINT; } /* Quand "print" est trouvé, stocke la chaîne et retourne le token PRINT */
"["                       { return LBRACKET; }
"]"                       { return RBRACKET; }
";"                       { return SEMICOLON; }
","                       { return COMMA; }
"="                       { return ASSIGN; }
"+"                       { return PLUS; }
"-"                       { return MINUS; }
"*"                       { return MULT; }
"'"                       { return TRANSPOSE; }
"("                       { return '('; }
")"                       { return ')'; }

[0-9]+(\.[0-9]+)?         {
                            yylval.fval = (float)atof(yytext);
                            return NUMBER;
                          }

[A-Za-z_][A-Za-z0-9_]*    {
                            yylval.sval = strdup(yytext);
                            return ID;
                          }

.                         { fprintf(stderr, "Erreur lexicale: symbole inconnu '%s'\n", yytext); return yytext[0]; }
%%
