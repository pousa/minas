/** @file symbol_table.h
 * @version 1.1
 */
#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include <stdio.h>

#define NAO_USADO 0
#define READ 1
#define WRITE 2
#define READ_WRITE 3

/** 
 * Tipo abstrato para guardar lista de dimensões de array
 */
typedef struct listaDim
{
    int dim; /* tamanho da dimensão */
    struct listaDim* prox;
} lista_dim;

/** 
 * Tipo abstrato para guardar informações de arrays 
 */
typedef struct campoArray
{
    int numDim; /* número de dimensões do array */
    lista_dim* dimensoes;
} infoArray;

/** 
 * Tipo abstrato para guardar lista de elementos de leitura/escrita das variáveis
 */
typedef struct listaRW
{
    int tipoAcesso;  /* tipo de acesso (leitura,escrita,leitura/escrita) */
    int linhaFor;    /* linha do FOR que engloba o escopo direto da variável */
    int colunaFor;   /* coluna do FOR que engloba o escopo direto da variável */
    char* arquivo;   /* arquivo em que a variável está sendo usada */
    struct listaRW* prox;
} lista_RW;


/**
 * Tipo abstrato das entradas na tabela de Hash.
 * Sempre vao ser inseridos na tabela, e recuperado dela, ponteiros sobre tais
 * estruturas de dados abstratas.
 */
typedef struct {
   char* name;      		/** um string que representa o nome de uma variavel. */ 
   char* tipo_str;  		/** tipo da variavel **/
   char* escopo;    		/** escopo da variavel **/
   char* arquivo;   		/** arquivo em que se encontra a variável **/
   int numPonteiros;  		/** número de ponteiros **/
   infoArray* dimArray; 	/** dimensoes se for array */
   int isArray;  		/** diz se é array */
   lista_RW* listaAcessos; 	/** lista de acessos a variavel em questão **/
   void* extra; 		/** qualquer informacao extra. */
   int linha;   		/** linha em que aparece a variavel **/
} entry_t ;


/**
 * Tipo abstrato dos nodos das listas encadeados.
 */

typedef struct entryList
{
       entry_t* entry;	/** Estrutura contendo as informações do símbolo. */
       struct entryList* next;	/** Ponteiro para o próximo elemento da lista. */
} entry_list; 

/** \brief Encapsulacao de um tipo abstrato que se chamara 'symbol_t'
 *
 * Voce deve inserir, entre o 'typedef' e o 'symbol_t', a estrutura de dados
 * abstrata que voce ira implementar.
 *
 */ 
typedef entry_list** symbol_t;

/** \brief Alocar memória para novo elemento na lista de elementos de leitura/escrita das variáveis
 *
 * @param linha um inteiro correspondente a linha do FOR escopo
 * @param coluna um inteiro correspondente a coluna do FOR escopo
 * @param entrada uma referencia para uma entrada a ser inserido o elemento
 * @param arquivo o nome do arquivo em que o escopo se encontra
 * @return o valor 1 se deu certo.
 */
int new_access_element(entry_t* entrada, int linha, int coluna, char* arquivo);

/** \brief Atualiza ultimo elemento de acesso com valor especificado como parâmetro
 *
 * @param linha um inteiro correspondente a linha do FOR escopo
 * @param coluna um inteiro correspondente a coluna do FOR escopo
 * @param entrada uma referencia para uma entrada a ser atualizada o último elemento
 * @param valor o novo valor a ser incorporado no último elemento da lista de acessos da entrada
 * @param arquivo o nome do arquivo em que o escopo se encontra
 * @return o valor 1 se deu certo, 0 caso contrário
 */
int update_element(entry_t* entrada, int valor, int linha, int coluna, char *arquivo);

/** \brief Verifica se uma variável já foi incluída no escopo de um determinado FOR
 * 
 * @param linha um inteiro correspondente a linha do FOR escopo
 * @param coluna um inteiro correspondente a coluna do FOR escopo
 * @param entrada uma referencia para uma entrada a ser verificada no escopo
 * @param arquivo o nome do arquivo em que o escopo se encontra
 * @return o valor 1 se variável já existe no escopo. 0 caso contrário.
 */
int scope_already_has_variable(entry_t* entrada, int linha, int coluna, char* arquivo);

/** \brief Inicializar a tabela de Hash.
 *
 * @param table uma referencia sobre uma tabela de simbolos.
 * @return o valor 0 se deu certo.
 */
int init_table(symbol_t* table) ;

/** \brief Destruir a tabela de Hash.
 *
 * 'free_table' eh o destrutor da estrutura de dados. Deve ser chamado pelo 
 * usuario no fim de seu uso de uma tabela de simbolos.
 * @param table uma referencia sobre uma tabela de simbolos.
 */
void free_table(symbol_t* table) ;

/** \brief Retornar um ponteiro sobre a entrada associada a 'name'.
 *
 * Essa funcao deve consultar a tabela de simbolos para verificar se se encontra
 * nela uma entrada associada a um char* (string) fornecido em entrada. Para
 * a implementacao, sera necessario usar uma funcao que mapeia um char* a
 * um numero inteiro. Aconselha-se, por exemplo, consultar o livro do dragao
 * (Aho/Sethi/Ulman), Fig. 7.35 e a funcao HPJW.
 *
 * @param table uma tabela de simbolos.
 * @param name um char* (string).
 * @return um ponteiro sobre a entrada associada a 'name', ou NULL se 'name'
 *         nao se encontrou na tabela.
 */
entry_t* lookup(symbol_t table, char* name) ;

/** \brief Inserir uma entrada em uma tabela.
 *
 * @param table uma tabela de simbolos.
 * @param entry uma entrada.
 * @return um numero negativo se nao se conseguiu efetuar a insercao, zero se
 *   deu certo.
 */
int insert(symbol_t* table, entry_t* entry) ;

/** \brief Imprimir o conteudo de uma tabela.
 *
 * A formatacao exata e deixada a carga do programador. Deve-se listar todas
 * as entradas contidas na tabela atraves de seu nome (char*). Deve retornar
 * o numero de entradas na tabela.
 *
 * @param table uma tabela de simbolos.
 * @return o numero de entradas na tabela.
 */
int print_table(symbol_t table);

/** \brief Imprimir o conteudo de uma tabela em um arquivo.
 *
 * A formatacao exata e deixada a carga do programador. Deve-se listar todas
 * as entradas contidas na tabela atraves de seu nome (char*). Deve retornar
 * o numero de entradas na tabela. A saida deve ser dirigida para um arquivo,
 * cujo descritor e passado em parametro.
 *
 * @param out um descritor de arquivo (FILE*).
 * @param table uma tabela de simbolos.
 * @return o numero de entradas na tabela.
 */
int print_file_table(FILE* out, symbol_t table);

#endif
