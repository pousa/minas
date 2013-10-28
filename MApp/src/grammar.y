%{
  #define __extension__ 
  #define __attribute__(x) /*Nothing */

  #include <stdio.h>
  #include "symbol_table.h"
  #include "stack.h"
  #include <stdlib.h>
  #include "string.h"
  
  symbol_t table;
  symbol_t global;
  stack pilha_ids;
  symbol_t typedefs;
  symbol_t defines;
  symbol_t arquivos;
  stack nomesArq;
  stack tipos[2];
  stack parenteses;
  stack linhasFor;
  stack colunasFor;
  stack linhasArq;
  stack colunasArq;
  int atual = 0;
  char* nomeFuncao = NULL;
  char* NOMEGLOBAL = NULL;
  int numPonteiros = 0;
  int line = 1;
  int column = 1;

  #define INDEFINIDO -1
  #define LISTA_IDS 600
  #define NADA 601
  #define ARRAY 602
%}

%union
{
	char* lexema;
	struct Tgeral { int token; char* lexema; entry_t* entrada; } geral;
	int num;
	struct Tgeral2 { int num; int token; entry_t* entrada; } geral2;
}

%token<geral> IDENTIFIER 
%token<geral> TYPE_NAME
%token<geral> CONSTANT CONSTANT_INT CONSTANT_FLOAT
%token STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type<geral> declaration_specifiers
%type<geral> enumerator
%type<geral> declaration
%type<geral> direct_declarator
%type<geral> declarator
%type<geral> init_declarator_list
%type<geral> storage_class_specifier
%type<geral> type_specifier
%type<geral> type_qualifier
%type<geral> init_declarator struct_or_union struct_or_union_specifier
%type<geral2> primary_expression expression assignment_expression postfix_expression unary_expression
%type<geral2> multiplicative_expression additive_expression shift_expression relational_expression
%type<geral2> equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression
%type<geral2> logical_or_expression conditional_expression constant_expression cast_expression
%type<geral2> expression_statement

%start init_unit
%locations
%%

primary_expression
	: IDENTIFIER 

	  { $$.num = -1; $$.token = IDENTIFIER;
  	    entry_t* entrada = lookup(table,$1.lexema);
 	    if (entrada == NULL)
  	    { 
		entrada = lookup(global,$1.lexema);
		if (entrada == NULL)
		{
			/* situação em que a variável ou função está definida em algum typedef */
			$$.entrada = NULL;
		}
  	    } 
  	    $$.entrada = entrada;

	    if ($$.entrada != NULL)
	    {	
 	    	// tratamento de escopos se limita APENAS para arrays QUANDO presentes em escopos de FOR
  	    	if (!empty(linhasFor) && !empty(colunasFor) && ($$.entrada->isArray || $$.entrada->numPonteiros != 0 )
			&& !scope_already_has_variable($$.entrada,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq)))
		{
  		 	new_access_element($$.entrada,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
		}
	    }
	 }

 	| CONSTANT  		{ $$.num = -1; $$.token = CONSTANT; $$.entrada = NULL; }
	| CONSTANT_INT 		{ $$.num = atoi($1.lexema); $$.token = INT; $$.entrada = NULL; }
	| CONSTANT_FLOAT	{ $$.num = -1; $$.token = FLOAT; $$.entrada = NULL; }
	| STRING_LITERAL 	{ $$.num = -1; $$.entrada = NULL; }
	| '(' expression ')'    { $$ = $2; }
	;

postfix_expression
	: primary_expression 					{ $$ = $1; }
	| postfix_expression '[' expression ']' 		{ $$ = $1; }
	| postfix_expression '(' ')' 				{ $$ = $1; }
	| postfix_expression '(' argument_expression_list ')'   { $$ = $1; }
	| postfix_expression '.' IDENTIFIER 			{ $$ = $1; }
	| postfix_expression PTR_OP IDENTIFIER 			{ $$ = $1; }
	| postfix_expression INC_OP 

	  { if ($1.entrada != NULL && !empty(linhasFor) && !empty(colunasFor))
		 update_element($1.entrada,WRITE,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
	   $$ = $1; 
	  }

	| postfix_expression DEC_OP 
	
	  { if ($1.entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
		update_element($1.entrada,WRITE,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq)); 
	    $$ = $1; 
	  }

	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression 			{ $$ = $1; }
	| INC_OP unary_expression 

	  { if ($2.entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
		  update_element($2.entrada,WRITE,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq)); 
	    $$ = $2; 
	  }

	| DEC_OP unary_expression 

	  { if ($2.entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
		  update_element($2.entrada,WRITE,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
	    $$ = $2;
	  }

	| unary_operator cast_expression        { $$ = $2; }
	| SIZEOF unary_expression 		{ $$ = $2; }
	| SIZEOF '(' type_name ')' 		{ $$.num = -1; $$.entrada = NULL; $$.token = NADA; }
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression
	: unary_expression 			{ $$ = $1; }
	| '(' type_name ')' cast_expression 	{ $$ = $4; }
	;

multiplicative_expression
	: cast_expression 

	  { if ($1.entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
		update_element($1.entrada,READ,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
	    $$ = $1;
	  }

	| multiplicative_expression '*' cast_expression 

	  { if ($1.token == INT && $3.token == INT)
	      { $$.num = $1.num * $3.num; $$.token = INT; } 
	    else 
	      { $$.token = -1; $$.num = -1; }
	      
	   if ($3.entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
		update_element($3.entrada,READ,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
		
	    $$.entrada = NULL;
	  }

	| multiplicative_expression '/' cast_expression 

	{ if ($1.token == INT && $3.token == INT) 
	    { $$.num = $1.num / $3.num; $$.token = INT; }
	 else 
	    { $$.token = -1; $$.num = -1; } 
	    	      
	 if ($3.entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
	     update_element($3.entrada,READ,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
		
         $$.entrada = NULL; 
        }

	| multiplicative_expression '%' cast_expression 

	{ if ($1.token == INT && $3.token == INT)
	    { $$.num = $1.num % $3.num; $$.token = INT; } 
	  else { $$.token = -1; $$.num = -1; } 
	  	      
	  if ($3.entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
	  	update_element($3.entrada,READ,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
		
	  $$.entrada = NULL; 
	}

	;

additive_expression	
	: multiplicative_expression			{ $$ = $1; }
	| additive_expression '+' multiplicative_expression

	 { if ($1.token == INT && $3.token == INT)
	     { $$.num = $1.num + $3.num; $$.token = INT; }
	   else { $$.token = -1; $$.num = -1; } 
	   $$.entrada = NULL; 
	 }

	| additive_expression '-' multiplicative_expression 

	  { if ($1.token == INT && $3.token == INT)
	      { $$.num = $1.num - $3.num; $$.token = INT; } 
	    else { $$.token = -1; $$.num = -1; } 
	    $$.entrada = NULL; 
	  }

	;

shift_expression
	: additive_expression					{ $$ = $1; }
	| shift_expression LEFT_OP additive_expression 		{ $$.num = -1; $$.entrada = NULL; }
	| shift_expression RIGHT_OP additive_expression 	{ $$.num = -1; $$.entrada = NULL; }
	;

relational_expression
	: shift_expression					{ $$ = $1; }
	| relational_expression '<' shift_expression 		{ $$.num = -1; $$.entrada = NULL; }
	| relational_expression '>' shift_expression 		{ $$.num = -1; $$.entrada = NULL; }
	| relational_expression LE_OP shift_expression 		{ $$.num = -1; $$.entrada = NULL; }
	| relational_expression GE_OP shift_expression 		{ $$.num = -1; $$.entrada = NULL; }
	;

equality_expression
	: relational_expression					{ $$ = $1; }
	| equality_expression EQ_OP relational_expression 	{ $$.num = -1; $$.entrada = NULL; }
	| equality_expression NE_OP relational_expression 	{ $$.num = -1; $$.entrada = NULL; }
	;

and_expression
	: equality_expression					{ $$ = $1; }
	| and_expression '&' equality_expression 		{ $$.num = -1; $$.entrada = NULL; }
	;

exclusive_or_expression
	: and_expression					{ $$ = $1; }
	| exclusive_or_expression '^' and_expression 

	{ if ($1.token == INT && $3.token == INT)
	   { $$.num = $1.num ^ $3.num; $$.token = INT; } 
	  else 
	   { $$.token = -1; $$.num = -1; } 
	  $$.entrada = NULL;
	}

	;

inclusive_or_expression
	: exclusive_or_expression				{ $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression 

	  { if ($1.token == INT && $3.token == INT) 
	      { $$.num = $1.num | $3.num; $$.token = INT; }
	    else { $$.token = -1; $$.num = -1; }
	    $$.entrada = NULL; 
	  }

	;

logical_and_expression
	: inclusive_or_expression						{ $$ = $1; }
	| logical_and_expression AND_OP inclusive_or_expression 		{ $$.num = -1; $$.entrada = NULL; }
	;

logical_or_expression
	: logical_and_expression						{ $$ = $1; }
	| logical_or_expression OR_OP logical_and_expression 			{ $$.num = -1;  $$.entrada = NULL;}
	;

conditional_expression
	: logical_or_expression							{ $$ = $1; }
	| logical_or_expression '?' expression ':' conditional_expression 	{ $$.num = -1; $$.entrada = NULL; }
	;

assignment_expression
	: conditional_expression						{ $$ = $1; }
	| unary_expression assignment_operator assignment_expression 

	  { if (($1.token == IDENTIFIER) && ($1.entrada != NULL) && !empty(linhasFor) && !empty(colunasFor)
		&& ($1.entrada->isArray || $1.entrada->numPonteiros != 0 ))
	  {
		update_element($1.entrada,WRITE,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
	  }
  	    $$ = $3;
	  }

	;

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';' 

	  { if ($2.token == LISTA_IDS)
  	    {  
  		int tamanho_tipo = 0;

  		while (!empty(tipos[0]))
  		{
			push(&tipos[1],pop(&tipos[0]));
			tamanho_tipo += strlen((char*)top(tipos[1])) + 1;
  		}
				
  		char* tipo_atual = (char*)malloc(tamanho_tipo);
  		tipo_atual[0] = '\0';
							
  		while (!empty(tipos[1]))
  		{
			strcat(tipo_atual,(char*)pop(&tipos[1]));
			if (!empty(tipos[1])) 
				strcat(tipo_atual," ");
  		}								

  		while (!empty(pilha_ids))
		{ 
			char* nomeID = (char*)pop(&pilha_ids);
			entry_t* entrada = lookup(table,nomeID);
			if (entrada == NULL) 
				entrada = lookup(global,nomeID);

			if (entrada == NULL)
			{
				printf("ERRO - IDENTIFICADOR NÃO ENCONTRADO!\n");
				exit(1);
			}

                	entrada->tipo_str = (void*)tipo_atual;	
			entrada->listaAcessos = NULL;

			if ($1.token == TYPEDEF)
			{
				insert(&typedefs,entrada);			
			}
		} // do while
  	    } // do if											
 	  }

	; 

declaration_specifiers
	: storage_class_specifier 				{ $$.token = NADA; }
	| storage_class_specifier declaration_specifiers 
		
	{ if (($2.token == IDENTIFIER) || ($2.token == STRUCT))
	     $$.token = NADA; 
	  else 
	     $$.token = $1.token;
	}

	| type_specifier 					{ $$.token = $1.token; }
	| type_specifier declaration_specifiers 		{ $$.token = $1.token; }
	| type_qualifier 					{ $$.token = $1.token; }
	| type_qualifier declaration_specifiers 		{ $$.token = $1.token; }
	;

init_declarator_list
	: init_declarator 

	  { if ($1.token == IDENTIFIER)
    	      $$.token = LISTA_IDS;  
	  }

	| init_declarator_list ',' init_declarator 

	  { if (($3.token == IDENTIFIER) || ($1.token == LISTA_IDS))
	      $$.token = LISTA_IDS; 
	  }

	;

init_declarator
	: declarator 						{ $$ = $1; }
	| declarator '=' initializer 				{ $$ = $1; }
	;

storage_class_specifier
	: TYPEDEF 			{ $$.token = TYPEDEF;  }
	| EXTERN 			{ $$.token = EXTERN; push(&tipos[0],(void*)"extern"); }
	| STATIC 			{ $$.token = STATIC; push(&tipos[0],(void*)"static"); }
	| AUTO 				{ $$.token = AUTO; push(&tipos[0],(void*)"auto"); }
	| REGISTER 			{ $$.token = REGISTER; push(&tipos[0],(void*)"register"); }
	;

type_specifier
	: VOID 				{ $$.token = VOID; push(&tipos[0],(void*)"void"); }
	| CHAR 				{ $$.token = CHAR; push(&tipos[0],(void*)"char"); }
	| SHORT 			{ $$.token = SHORT; push(&tipos[0],(void*)"short"); }
	| INT 				{ $$.token = INT; push(&tipos[0],(void*)"int"); }
	| LONG 				{ $$.token = LONG; push(&tipos[0],(void*)"long"); }
	| FLOAT 			{ $$.token = FLOAT; push(&tipos[0],(void*)"float"); }
	| DOUBLE 			{ $$.token = DOUBLE; push(&tipos[0],(void*)"double"); }
	| SIGNED 			{ $$.token = SIGNED; push(&tipos[0],(void*)"signed"); }
	| UNSIGNED 			{ $$.token = UNSIGNED; push(&tipos[0],(void*)"unsigned"); }
	| struct_or_union_specifier 	{ $$.token = $1.token; }
	| enum_specifier 		{ $$.token = ENUM; }
	| TYPE_NAME 			{ $$.token = $1.token; push(&tipos[0],$1.lexema); }
	;
 
struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}' 		{ push(&tipos[0],(void*)$2.lexema); $$.token = $1.token; }
	| struct_or_union '{' struct_declaration_list '}' 			{ $$.token = NADA; }
	| struct_or_union IDENTIFIER 						{ push(&tipos[0],(void*)$2.lexema); $$.token = $1.token; }
	;

struct_or_union
	: STRUCT 			{ $$.token = STRUCT; push(&tipos[0],(void*)"struct"); }
	| UNION 			{ $$.token = UNION; push(&tipos[0],(void*)"union"); }
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;

type_qualifier
	: CONST 			{ $$.token = CONST; }
	| VOLATILE 			{ $$.token = VOLATILE; }
	;

declarator
	: pointer direct_declarator 	{ $$ = $2; }
	| direct_declarator 		{ $$ = $1; }
	;

direct_declarator
	: IDENTIFIER 

	  { $$.token = IDENTIFIER; 
	    push(&pilha_ids,$1.lexema); 
	    entry_t* entrada = (entry_t*)malloc(sizeof(entry_t));
	    entrada->isArray = 0;
	    entrada->name = $1.lexema;
            entrada->dimArray = NULL;
 	    entrada->escopo = (char*)malloc(strlen(nomeFuncao)+1);
	    entrada->numPonteiros = numPonteiros;
	    entrada->arquivo = top(nomesArq);
	    entrada->listaAcessos = NULL;
	    numPonteiros = 0;
	    strcpy(entrada->escopo,nomeFuncao);
	    if (nomeFuncao == NOMEGLOBAL)
	      insert(&global,entrada);
	    else
              insert(&table,entrada);
	    $$.entrada = entrada; 
	  } 

	| '(' declarator ')' 						{ $$ = $2; }
	| direct_declarator '[' constant_expression ']'

 	 { if (!$1.entrada->isArray)
	   {
     		 $1.entrada->isArray = 1;
      		 $1.entrada->dimArray = (infoArray*)malloc(sizeof(infoArray));
      		 $1.entrada->dimArray->numDim = 1;
      		 $1.entrada->dimArray->dimensoes = malloc(sizeof(lista_dim));
      		 $1.entrada->dimArray->dimensoes->dim = $3.num;
      		 $1.entrada->dimArray->dimensoes->prox = NULL;
   	   }
   	   else
   	   {
		lista_dim* p = $1.entrada->dimArray->dimensoes;
		while (p->prox != NULL)
			p = p->prox;
		p->prox = malloc(sizeof(lista_dim));
		p->prox->dim = $3.num;
		p->prox->prox = NULL;
        	$1.entrada->dimArray->numDim++;
   	   }						
   	   $$.token = $1.token;
   	   $$.entrada = $1.entrada;
        }

	| direct_declarator '[' ']'
 
	 { if (!$1.entrada->isArray)
   	   {
      		$1.entrada->isArray = 1;
     		$1.entrada->dimArray = (infoArray*)malloc(sizeof(infoArray));
      		$1.entrada->dimArray->numDim = 1;
      		$1.entrada->dimArray->dimensoes = malloc(sizeof(lista_dim));
      		$1.entrada->dimArray->dimensoes->dim = INDEFINIDO;
      		$1.entrada->dimArray->dimensoes->prox = NULL;
  	   }
   	   else
   	   {
		lista_dim* p = $1.entrada->dimArray->dimensoes;
		while (p->prox != NULL)
			p = p->prox;
		p->prox = malloc(sizeof(lista_dim));
		p->prox->dim = INDEFINIDO;
		p->prox->prox = NULL;
        	$1.entrada->dimArray->numDim++;
   	   }						
   	   $$.token = $1.token; 
   	   $$.entrada = $1.entrada; 
	 }

	| direct_declarator '(' 

	  { if ($1.token == IDENTIFIER) 
  	    { 
     		nomeFuncao = malloc(strlen($1.lexema)+1); 
     		strcpy(nomeFuncao,$1.lexema); 

  		int tamanho_tipo = 0;

  		while (!empty(tipos[0]))
  		{
			push(&tipos[1],pop(&tipos[0]));
			tamanho_tipo += strlen((char*)top(tipos[1])) + 1;
  		}
				
  		char* tipo_atual = (char*)malloc(tamanho_tipo);
  		tipo_atual[0] = '\0';
							
  		while (!empty(tipos[1]))
  		{
			strcat(tipo_atual,(char*)pop(&tipos[1]));
			if (!empty(tipos[1])) 
				strcat(tipo_atual," ");
  		}								

  		while (!empty(pilha_ids))
		{ 
			char* nomeID = (char*)pop(&pilha_ids);
			entry_t* entrada = lookup(table,nomeID);
			if (entrada == NULL) 
				entrada = lookup(global,nomeID);

			if (entrada == NULL)
			{
				printf("ERRO - IDENTIFICADOR NÃO ENCONTRADO!\n");
				exit(1);
			}

                	entrada->tipo_str = (void*)tipo_atual;
	   		entrada->listaAcessos = NULL;	 

			if ($1.token == TYPEDEF)
			{
				insert(&typedefs,entrada);			
			}
		}
  	    }
	  } 
	    parameter_type_list  
	  { free_stack(&tipos[0]); } 
	    ')' 
	  { $$.token = $1.token; 
	    $$.lexema = $1.lexema; 
	    $$.entrada = NULL; 
	  }

	| direct_declarator '(' identifier_list ')' 	

	  { $$.token = $1.token;
	    $$.lexema = $1.lexema; 
	    $$.entrada = NULL; 
	  }

	| direct_declarator '(' 

	  { if ($1.token == IDENTIFIER) 
  	    { 
     		nomeFuncao = malloc(strlen($1.lexema)+1); 
     		strcpy(nomeFuncao,$1.lexema); 

  		int tamanho_tipo = 0;

  		while (!empty(tipos[0]))
  		{
			push(&tipos[1],pop(&tipos[0]));
			tamanho_tipo += strlen((char*)top(tipos[1])) + 1;
  		}
				
  		char* tipo_atual = (char*)malloc(tamanho_tipo);
  		tipo_atual[0] = '\0';
							
  		while (!empty(tipos[1]))
  		{
			strcat(tipo_atual,(char*)pop(&tipos[1]));
			if (!empty(tipos[1])) 
				strcat(tipo_atual," ");
  		}								

  		while (!empty(pilha_ids))
		{ 
			char* nomeID = (char*)pop(&pilha_ids);
			entry_t* entrada = lookup(table,nomeID);
			if (entrada == NULL) 
				entrada = lookup(global,nomeID);

			if (entrada == NULL)
			{
				printf("ERRO - IDENTIFICADOR NÃO ENCONTRADO!\n");
				exit(1);
			}

                	entrada->tipo_str = (void*)tipo_atual;
			entrada->listaAcessos = NULL;	 

			if ($1.token == TYPEDEF)
			{
				insert(&typedefs,entrada);			
			}
		}
  	    }
	 }
 	  ')' 
	 { $$.token = $1.token; 
	   $$.lexema = $1.lexema; 
	   $$.entrada = NULL;
	 }

	;

pointer
	: '*' 						{ numPonteiros++; }
	| '*' type_qualifier_list  			{ numPonteiros++; }
	| '*' pointer  					{ numPonteiros++; }
	| '*' type_qualifier_list pointer  		{ numPonteiros++; }
	;

type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;


parameter_type_list
	: parameter_list
	| parameter_list ',' ELLIPSIS
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator 
	  { if ($2.token == IDENTIFIER)
  	    {  
  		int tamanho_tipo = 0;

  		while (!empty(tipos[0]))
  		{
			push(&tipos[1],pop(&tipos[0]));
			tamanho_tipo += strlen((char*)top(tipos[1])) + 1;
  		}
					
  		char* tipo_atual = (char*)malloc(tamanho_tipo);
  		tipo_atual[0] = '\0';
									
  		while (!empty(tipos[1]))
  		{
			strcat(tipo_atual,(char*)pop(&tipos[1]));
			if (!empty(tipos[1])) 
				strcat(tipo_atual," ");
  		}								

  		while (!empty(pilha_ids))
		{ 
			char* nomeID = (char*)pop(&pilha_ids);
			entry_t* entrada = lookup(table,nomeID);
			if (entrada == NULL) 
				entrada = lookup(global,nomeID);

			if (entrada == NULL)
			{
				printf("ERRO - IDENTIFICADOR NÃO ENCONTRADO!\n");
				exit(1);
			}

               		entrada->tipo_str = (void*)tipo_atual;	
			entrada->listaAcessos = NULL; 

			if ($1.token == TYPEDEF)
				insert(&typedefs,entrada);
		}
  	    } // do if											
 	  }
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER 
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer 					{ numPonteiros--; }
	| direct_abstract_declarator
	| pointer direct_abstract_declarator 		{ numPonteiros--; }
	;

direct_abstract_declarator
	: '(' abstract_declarator ')' 
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')' 
	;

initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

initializer_list
	: initializer
	| initializer_list ',' initializer
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

compound_statement
	: '{' '}'
	| '{' statement_list '}'
	| '{' declaration_list '}'
	| '{' declaration_list statement_list '}'
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement
	| statement_list statement
	;

expression_statement
	: ';'                 			{ $$.token = NADA; $$.entrada = NULL; $$.num = -1; }
	| expression ';'			{ $$ = $1; }
	;
 
selection_statement
	: IF '(' expression ')' statement
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' 
	
	  { int *i = (int*)malloc(sizeof(int));
  	    *i = @1.first_line;
  	    push(&linhasFor,(void*)i);
  	    i = (int*)malloc(sizeof(int));
            *i = @1.first_column;
            push(&colunasFor,(void*)i);
	  } 
	  
	  for_statement 

	;
	
for_statement 
	: expression_statement expression_statement ')' statement 
	    
	  { int *i = (int*)pop(&linhasFor);
  	    free(i);
  	    i = pop(&colunasFor);
  	    free(i);
 	  } 
 	  
 	  |
 	  
	  expression_statement expression_statement expression ')' statement 
	    
	  { int *i = (int*)pop(&linhasFor);
  	    free(i);
  	    i = pop(&colunasFor);
  	    free(i);
 	  }
 	  
	  ;   

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition 			{ print_table(table); free_table(&table); }
	| declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list  compound_statement 		{ nomeFuncao = NOMEGLOBAL; }
	| declaration_specifiers declarator compound_statement 					{ nomeFuncao = NOMEGLOBAL; }
	| declarator declaration_list compound_statement 					{ nomeFuncao = NOMEGLOBAL; }
	| declarator compound_statement 							{ nomeFuncao = NOMEGLOBAL; }
	;

init_unit
	: translation_unit
	|
	;

%%
#include <stdio.h>

extern char yytext[];
extern int column;
extern FILE* yyin;
extern int yylineno;

int main(int argc, char* argv[]) 
{  
   init_stack(&pilha_ids);
   init_stack(&tipos[0]);
   init_stack(&tipos[1]);
   init_stack(&nomesArq);
   init_stack(&parenteses);
   init_stack(&linhasFor);
   init_stack(&colunasFor);
   init_table(&table);
   init_table(&arquivos);
   init_table(&typedefs);
   init_table(&defines);
   init_table(&global);

   nomeFuncao = (char*)malloc(100);
   NOMEGLOBAL = (char*)malloc(10);
   NOMEGLOBAL[0] = '\0';
   strcat(NOMEGLOBAL,"global");

   nomeFuncao = NOMEGLOBAL;

   if (argc != 2) 
   {
     printf("uso: %s <input_file>. Try again!\n", argv[0]);
     exit(-1);
   }

   yyin = fopen(argv[1], "r");

   if (!yyin) 
   {
     printf("Uso: %s <input_file>. Could not find %s. Try again!\n", argv[0], argv[1]);
     exit(-1);
   }

   push(&nomesArq,argv[1]);

   while (1) 
   {
	if (!yyparse()) 
	{ }
  	else 
      		printf("ERROR em arquivo %s.\n",(char*)top(nomesArq));

	pop(&nomesArq);

	if (empty(nomesArq))
		break;

	yyrestart(yyin);
	yyin = fopen(top(nomesArq),"r");
	nomeFuncao = NOMEGLOBAL;
	line = 1;	
        column = 1;
   }

   print_table(global);

   return(0);
}

yyerror(s)
char *s;
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}

