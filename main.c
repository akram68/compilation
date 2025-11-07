#include "semantic.h"
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"
#include "lex.yy.h"

int yyparse(void);
int yylex_destroy(void);
extern FILE *yyin;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer);

int main()
{
    char input[2048];
    init_symbol_table();

    printf("=== Matrix Compiler (interactif) ===\nType 'exit' to quit.\n");
    while (1)
    {
        printf(">> ");
        if (!fgets(input, sizeof(input), stdin))
            break;
        input[strcspn(input, "\n")] = 0;
        if (strcmp(input, "exit") == 0)
            break;
        if (strlen(input) == 0)
            continue;
        YY_BUFFER_STATE buf = yy_scan_string(input);
        yy_switch_to_buffer(buf);
        yyparse();
        yylex_destroy();
    }
    return 0;
}
