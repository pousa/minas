all: lex.yy.c grammar.tab.c symbol_table.o stack.o
	gcc -o cuia lex.yy.c grammar.tab.c -ll symbol_table.o stack.o

lex.yy.c: scanner.l
	lex scanner.l

grammar.tab.c: grammar.y
	bison -d grammar.y

symbol_table.o: symbol_table.c
	gcc -c symbol_table.c

stack.o: stack.c
	gcc -c stack.c

clean:
	rm -f lex.yy.c grammar.tab.c symbol_table.o stack.o parser


