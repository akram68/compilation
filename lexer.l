%option noyywrap
%option header-file="lex.yy.h"

%{
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
extern YYSTYPE yylval;
%}

/* rules */
%%
[ \t\r\n]+                ;             /* ignorer espaces et retours ligne */

"print"                   { yylval.sval = strdup("print"); return PRINT; }
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
