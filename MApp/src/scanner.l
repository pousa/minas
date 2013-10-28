%option noyywrap
%option yylineno
%{
#include <stdio.h>
#include <string.h>
#include "symbol_table.h"
#include "grammar.tab.h"
#include "stack.h"

#define DEFINE 603
#define YY_USER_INIT yylloc.first_line=1;

void count();
void comment();
void comment2();
void comment3();
void trataInclude();
void trataDefine();
void trata_carac(char c);

extern char* TOKEN;
extern symbol_t table;
extern symbol_t typedefs;
extern symbol_t defines;
extern stack tipos[2];
extern symbol_t arquivos;
extern stack nomesArq;
extern stack parenteses;
extern YYLTYPE yylloc;

extern int line;
extern int column;

int ultimoToken = 0;
%}

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*
U                       _

%%
"/*"			{ count(); comment(); }
"#else"			{ count(); comment3(); }
"#include"		{ count(); trataInclude(); }
"#define"               { count(); trataDefine(); }
"#"                     { count(); comment2(); }
"__attribute__"         { count(); comment2(); unput(';'); column--; }
"__restrict"            { count(); }

"auto"			{ count(); ultimoToken = 0; return(AUTO); }
"break"			{ count(); ultimoToken = 0; return(BREAK); }
"case"			{ count(); ultimoToken = 0; return(CASE); }
"char"			{ count(); ultimoToken = 0; return(CHAR); }
"const"			{ count(); ultimoToken = 0; return(CONST); }
"continue"		{ count(); ultimoToken = 0; return(CONTINUE); }
"default"		{ count(); ultimoToken = 0; return(DEFAULT); }
"do"			{ count(); ultimoToken = 0; return(DO); }
"double"		{ count(); ultimoToken = 0; return(DOUBLE); }
"else"			{ count(); ultimoToken = 0; return(ELSE); }
"enum"			{ count(); ultimoToken = 0; return(ENUM); }
"extern"		{ count(); ultimoToken = 0; return(EXTERN); }
"float"			{ count(); ultimoToken = 0; return(FLOAT); }
"for"			{ count(); ultimoToken = 0; 
			  yylloc.first_line = line; 
			  yylloc.first_column = column - yyleng; 
		    	  return(FOR); }
"goto"			{ count(); ultimoToken = 0; return(GOTO); }
"if"			{ count(); ultimoToken = 0; return(IF); }
"int"			{ count(); ultimoToken = 0; return(INT); }
"long"			{ count(); ultimoToken = 0; return(LONG); }
"register"		{ count(); ultimoToken = 0; return(REGISTER); }
"return"		{ count(); ultimoToken = 0; return(RETURN); }
"short"			{ count(); ultimoToken = 0; return(SHORT); }
"signed"		{ count(); ultimoToken = 0; return(SIGNED); }
"sizeof"		{ count(); ultimoToken = 0; return(SIZEOF); }
"static"		{ count(); ultimoToken = 0; return(STATIC); }
"struct"		{ count(); ultimoToken = STRUCT; return(STRUCT); }
"switch"		{ count(); ultimoToken = 0; return(SWITCH); }
"typedef"		{ count(); ultimoToken = TYPEDEF; return(TYPEDEF); }
"union"			{ count(); ultimoToken = 0; return(UNION); }
"unsigned"		{ count(); ultimoToken = 0; return(UNSIGNED); }
"void"			{ count(); ultimoToken = 0; return(VOID); }
"volatile"		{ count(); ultimoToken = 0; return(VOLATILE); }
"while"			{ count(); ultimoToken = 0; return(WHILE); }
"__const"               { count(); }
"__THROW"		{ count(); }

{L}({L}|{D})*		{ count();
			  int tipo = check_type();
			  yylloc.first_line = line;
			  yylloc.first_column = column - yyleng; 
			  if (tipo != DEFINE)
			  {
			  	ultimoToken = 0; 
			  	yylval.geral.lexema = (char*)malloc(strlen(yytext)+1);
			  	strcpy(yylval.geral.lexema,yytext);
			  	yylval.geral.token = tipo;
			  	return(tipo);
			  }
			}
0[xX]{H}+{IS}?		{ count(); ultimoToken = 0; return(CONSTANT_INT); }
0{D}+{IS}?		{ count(); ultimoToken = 0; return(CONSTANT_INT); }

{D}+{IS}?		{ count(); ultimoToken = 0; 
			  yylval.geral.lexema = (char*)malloc(strlen(yytext)+1);
			  strcpy(yylval.geral.lexema,yytext);
			  yylval.geral.token = CONSTANT; 
			  return(CONSTANT_INT); }

L?'(\\.|[^\\'])+'	{ count(); ultimoToken = 0; return(CONSTANT); }

{D}+{E}{FS}?		{ count(); ultimoToken = 0; return(CONSTANT_FLOAT); }
{D}*"."{D}+({E})?{FS}?	{ count(); ultimoToken = 0; return(CONSTANT_FLOAT); }
{D}+"."{D}*({E})?{FS}?	{ count(); ultimoToken = 0; return(CONSTANT_FLOAT); }

L?\"(\\.|[^\\"])*\"	{ count(); if (ultimoToken == STRING_LITERAL) 
				   { }
				   else
				   { 
					ultimoToken = STRING_LITERAL; 
					return(STRING_LITERAL); 
				   }
			}

"..."			{ count(); ultimoToken = 0; return(ELLIPSIS); }
">>="			{ count(); ultimoToken = 0; return(RIGHT_ASSIGN);  }
"<<="			{ count(); ultimoToken = 0; return(LEFT_ASSIGN); }
"+="			{ count(); ultimoToken = 0; return(ADD_ASSIGN); }
"-="			{ count(); ultimoToken = 0; return(SUB_ASSIGN); }
"*="			{ count(); ultimoToken = 0; return(MUL_ASSIGN); }
"/="			{ count(); ultimoToken = 0; return(DIV_ASSIGN); }
"%="			{ count(); ultimoToken = 0; return(MOD_ASSIGN); }
"&="			{ count(); ultimoToken = 0; return(AND_ASSIGN); }
"^="			{ count(); ultimoToken = 0; return(XOR_ASSIGN); }
"|="			{ count(); ultimoToken = 0; return(OR_ASSIGN); }
">>"			{ count(); ultimoToken = 0; return(RIGHT_OP); }
"<<"			{ count(); ultimoToken = 0; return(LEFT_OP); }
"++"			{ count(); ultimoToken = 0; return(INC_OP); }
"--"			{ count(); ultimoToken = 0; return(DEC_OP); }
"->"			{ count(); ultimoToken = 0; return(PTR_OP); }
"&&"			{ count(); ultimoToken = 0; return(AND_OP); }
"||"			{ count(); ultimoToken = 0; return(OR_OP); }
"<="			{ count(); ultimoToken = 0; return(LE_OP); }
">="			{ count(); ultimoToken = 0; return(GE_OP); }
"=="			{ count(); ultimoToken = 0; return(EQ_OP); }
"!="			{ count(); ultimoToken = 0; return(NE_OP); }
";"			{ count(); ultimoToken = 0; return(';'); }
("{"|"<%")		{ count(); ultimoToken = 0; return('{'); }
("}"|"%>")		{ count(); ultimoToken = 0; return('}'); }
","			{ count(); ultimoToken = 0; return(','); }
":"			{ count(); ultimoToken = 0; return(':'); }
"="			{ count(); ultimoToken = 0; return('='); }
"("			{ count(); ultimoToken = '('; return('('); }
")"			{ count(); ultimoToken = 0; return(')'); }
("["|"<:")		{ count(); ultimoToken = 0; return('['); }
("]"|":>")		{ count(); ultimoToken = 0; return(']'); }
"."			{ count(); ultimoToken = 0; return('.'); }
"&"			{ count(); ultimoToken = 0; return('&'); }
"!"			{ count(); ultimoToken = 0; return('!'); }
"~"			{ count(); ultimoToken = 0; return('~'); }
"-"			{ count(); ultimoToken = 0; return('-'); }
"+"			{ count(); ultimoToken = 0; return('+'); }
"*"			{ count(); ultimoToken = 0; return('*'); }
"/"			{ count(); ultimoToken = 0; return('/'); }
"%"			{ count(); ultimoToken = 0; return('%'); }
"<"			{ count(); ultimoToken = 0; return('<'); }
">"			{ count(); ultimoToken = 0; return('>'); }
"^"			{ count(); ultimoToken = 0; return('^'); }
"|"			{ count(); ultimoToken = 0; return('|'); }
"?"			{ count(); ultimoToken = 0; return('?'); }

[ \t\v\n\f]		{ count(); }
.			{ count(); }

%%

void comment()
{
	char c, c1;

loop:
	while ((c = input()) != '*' && c != 0)
		trata_carac(c);
		
	trata_carac(c);

	if ((c1 = input()) != '/' && c != 0)
	{
		unput(c1);
		goto loop;
	}
	
	trata_carac(c1);
}

void comment2()
{
	char c;
	char antes;

loop2:	while ((c = input()) != '\n' && c != 0)
	{
		trata_carac(c);
		antes = c;
	}
	
	trata_carac(c);

	if (antes == '\\')
		goto loop2;	
		
}

void comment3()
{
	char c;
	char antes;
	char* palavraElse = "endif";
	int i = 0;
	char buf[5];

loop3:  while ((c = input()) != '#' && c != 0)
		trata_carac(c);
		
	trata_carac(c);

	i = 0;
	while (i < 5)
	{
		if ((buf[i] = input()) != palavraElse[i])
		{
			while (i >= 0)
			{
				unput(buf[i]);
				i--;
			}
			goto loop3;
		}
		i++;
	}		
	
	column += 5;
}

void trataInclude()
{
	char c;
	FILE *arq;
	char nomeArq[100];
	int i;
	
	while ((c = input()) != '\"' && c != 0 && c != '<')
		trata_carac(c);
		
	if (c == '<')
	{
		while ((c = input()) != '>' && c!= 0)
			trata_carac(c);
		return;
	}

	i = 0;

	while ((c = input()) != '\"' && c != 0)
		nomeArq[i++] = c;

	nomeArq[i] = '\0';
	
	if (!lookup(arquivos,nomeArq))
	{
		entry_t* entrada = (entry_t*)malloc(sizeof(entry_t));
		entrada->name = nomeArq;
		insert(&arquivos,entrada);
		char* p = malloc(strlen(nomeArq));
		strcpy(p,nomeArq);
		push(&nomesArq,p);
		yyrestart(yyin);
		yyin = fopen(nomeArq,"r");
		line = 1;
		column = 1;
	}
	
	column += i;
}

void trataDefine()
{
	char c,antes;
	FILE *arq;
	char nome[100];
	char valor[100];
	int i;
	
	while ((c = input()) == ' ' || c == '\t')
		trata_carac(c);
		
	trata_carac(c);

	nome[0] = c;
	i = 1;

loop4:	while ((c = input()) != ' ' && c != '\t' && c != '\n')
	{
		if (c == '(')
			push(&parenteses,"(");
		if (c == ')')
			pop(&parenteses);
		nome[i++] = c;
		antes = c;
		trata_carac(c);
	}
	
	trata_carac(c);

	if ((c == '\n') && (antes == '\\'))
	{
		i--;
		goto loop4;
	}

	if (!empty(parenteses))
		goto loop4;

	nome[i] = '\0';

	while ((c = input()) == ' ' || c == '\t')
		trata_carac(c);

	valor[0] = c;
	i = 1;
loop5:	while ((c = input()) != '\n')
	{
		valor[i++] = c;
		antes = c;
		trata_carac(c);
	}
	
	trata_carac(c);

	if ((c == '\n') && (antes == '\\'))
	{
		i--;
		goto loop5;
	}

	valor[i] = '\0';

	entry_t* entrada = (entry_t*)malloc(sizeof(entry_t));
	entrada->name = malloc(strlen(nome));
	strcpy(entrada->name,nome);
	entrada->tipo_str = malloc(strlen(valor));
	strcpy(entrada->tipo_str,valor);
	insert(&defines,entrada);
}

void count()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
		{
			column = 1;
			line++;
		}
		else if (yytext[i] == '\t')
			column += 8 - (column % 8) + 1;
		else
			column++; 
}

void trata_carac(char c)
{
	if (c == '\n')
	{
		column = 1;
		line++;
	}
	else if (c == '\t')
		column += 8 - (column % 8);
	else
		column++;
}

int check_type()
{
	entry_t* entrada;
	int tam,j;

	if ((entrada = lookup(defines,yytext)) != NULL)
	{
		tam = strlen(entrada->tipo_str);
		while (tam != 0)
		{
			unput(entrada->tipo_str[tam-1]);
			tam--;
		}		
		return(DEFINE);
	}

	if ((ultimoToken == STRUCT) || (ultimoToken == '('))
	{
		if (ultimoToken == '(')
		  while (!empty(tipos[0]))
			pop(&tipos[0]);
		return IDENTIFIER;
	}

	if ((lookup(typedefs,yytext)) || (ultimoToken == TYPEDEF))
		return(TYPE_NAME); 

	return(IDENTIFIER);
}
