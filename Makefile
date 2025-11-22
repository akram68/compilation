CC = gcc
FLEX = D:/win_flex_bison-latest/win_flex.exe
BISON = D:/win_flex_bison-latest/win_bison.exe
CFLAGS = -Wall -Wextra
OUTPUT = matrix_compiler.exe

all: $(OUTPUT)

$(OUTPUT): lex.yy.c parser.tab.c semantic.c main.c codegen.c
	$(CC) parser.tab.c lex.yy.c semantic.c main.c codegen.c -o $(OUTPUT) $(CFLAGS)

lex.yy.c: lexer.l parser.tab.h
	$(FLEX) --header-file=lex.yy.h lexer.l

parser.tab.c parser.tab.h: parser.y
	$(BISON) -d parser.y

clean:
	del lex.yy.c lex.yy.h parser.tab.c parser.tab.h $(OUTPUT) 2>nul || true

.PHONY: all clean