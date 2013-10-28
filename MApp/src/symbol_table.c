#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO 211

/** \brief Alocar memória para novo elemento na lista de elementos de leitura/escrita das variáveis
 *
 * @param linha um inteiro correspondente a linha do FOR escopo
 * @param coluna um inteiro correspondente a coluna do FOR escopo
 * @param entrada uma referencia para uma entrada a ser inserido o elemento
 * @param arquivo o nome do arquivo em que o escopo se encontra
 * @return o valor 1 se deu certo.
 */
int new_access_element(entry_t* entrada, int linha, int coluna, char* arquivo)
{
	if (entrada->listaAcessos == NULL)
	{
		entrada->listaAcessos = (lista_RW*)malloc(sizeof(lista_RW));
		entrada->listaAcessos->tipoAcesso = 0;
 		entrada->listaAcessos->linhaFor = linha;
		entrada->listaAcessos->colunaFor = coluna;
		entrada->listaAcessos->arquivo = arquivo;
		entrada->listaAcessos->prox = NULL;
	}
	else
	{
		lista_RW* p = entrada->listaAcessos;
		while (p->prox != NULL)
		{
			p = p->prox;
		}
		p->prox = (lista_RW*)malloc(sizeof(lista_RW));	
		p->prox->tipoAcesso = 0;
                p->prox->linhaFor = linha;
                p->prox->colunaFor = coluna;
                p->prox->arquivo = arquivo;
		p->prox->prox = NULL;
	}
	return 1;
}

/** \brief Atualiza ultimo elemento de acesso com valor especificado como parâmetro
 *
 * @param linha um inteiro correspondente a linha do FOR escopo
 * @param coluna um inteiro correspondente a coluna do FOR escopo
 * @param entrada uma referencia para uma entrada a ser atualizada o último elemento
 * @param valor o novo valor a ser incorporado no último elemento da lista de acessos da entrada
 * @param arquivo o nome do arquivo em que o escopo se encontra
 * @return o valor 1 se deu certo, 0 caso contrário
 */
int update_element(entry_t* entrada, int valor, int linha, int coluna, char *arquivo)
{
	lista_RW* p = entrada->listaAcessos;

	while (p != NULL)
	{
		if ((p->linhaFor == linha) && (p->colunaFor == coluna) && (!strcmp(p->arquivo,arquivo)) )
		{
			p->tipoAcesso |= valor;
			return 1;
		}
		p = p->prox;
	}
	
	return 0;
}

/** \brief Verifica se uma variável já foi incluída no escopo de um determinado FOR
 * 
 * @param linha um inteiro correspondente a linha do FOR escopo
 * @param coluna um inteiro correspondente a coluna do FOR escopo
 * @param entrada uma referencia para uma entrada a ser verificada no escopo
 * @param arquivo o nome do arquivo em que o escopo se encontra
 * @return o valor 1 se variável já existe no escopo. 0 caso contrário.
 */
int scope_already_has_variable(entry_t* entrada, int linha, int coluna, char* arquivo)
{
	lista_RW* p = entrada->listaAcessos;

	while (p != NULL)
	{
		if ((p->linhaFor == linha) && (p->colunaFor == coluna) && (!strcmp(p->arquivo,arquivo)) )
			return 1;
		p = p->prox;
	}
	return 0;
}

/** \brief Inicializar a tabela de Hash.
 *
 * @param table, uma referencia sobre uma tabela de simbolos.
 * @return o valor 0 se deu certo.
 */
int init_table(symbol_t* table)
{
	int i;
    *table = (symbol_t)malloc(sizeof(entry_list)*TAMANHO);
    
    /* se não obteve memória para a tabela, retorna 1 */
    if (*table == NULL)
       return 1;
    
    /* inicializa as listas de nodos da tabela com NULL */
    for (i = 0; i < TAMANHO ; i++)
      (*table)[i] = NULL;
  
    return 0;  
}

/** \brief Função de Hash hashpjw (livro do Dragao)
 *
 * @param nome, um string contendo o nome do simbolo.
 * @return o valor do calculo da Função de Hash.
 */
int hashpjw(char* nome)
{
    char* p;
    unsigned h = 0, g;
    for (p = nome; *p != '\0'; p++)
    {
        h = (h << 4) + (*p);
        if (g = h&0xf0000000)
        {
              h = h ^ (g >> 24);
              h = h ^ g;
        }
    }
    return h % TAMANHO;
}

/** \brief Retornar um ponteiro sobre a entrada associada a 'name'.
 *
 * Essa funcao deve consultar a tabela de simbolos para verificar se se encontra
 * nela uma entrada associada a um char* (string) fornecido em entrada. Para
 * a implementacao, sera necessario usar uma funcao que mapeia um char* a
 * um numero inteiro. Aconselha-se, por exemplo, consultar o livro do dragao
 * (Aho/Sethi/Ulman), Fig. 7.35 e a funcao HPJW.
 *
 * @param table, uma tabela de simbolos.
 * @param name, um char* (string).
 * @return um ponteiro sobre a entrada associada a 'name', ou NULL se 'name'
 *         nao se encontrou na tabela.
 */
entry_t* lookup(symbol_t table, char* name)
{
    int hashValue = hashpjw(name);
    
    if (table[hashValue] == NULL)
      return NULL;
    else
    {
        entry_list* p = table[hashValue];
        while (p != NULL)
        {
            if (!strcmp(p->entry->name,name))
                return p->entry;
            else
                p = p->next;
        }
        return NULL;
    }  
}     

/** \brief Inserir uma entrada em uma tabela.
 *
 * @param table, uma tabela de simbolos.
 * @param entry, uma entrada.
 * @return um numero negativo se nao se conseguiu efetuar a insercao, zero se
 *   deu certo.
 */
int insert(symbol_t* table, entry_t* entry)
{
    int hashValue = hashpjw(entry->name);
    
    if ((*table)[hashValue] == NULL) /* se lista de entradas na posicao esta vazia, cria o 1° nodo da lista */
    {
          (*table)[hashValue] = (entry_list*)malloc(sizeof(entry_list));
          if ((*table)[hashValue] != NULL)
          {
              ((*table)[hashValue])->entry = entry;
              ((*table)[hashValue])->next = NULL;
              return 0;
          }
          else
            return -1;          
    }
    else /* se lista de entradas na posicao não está vazia, insere no inicio da lista */
    {
        entry_list* p = (entry_list*)malloc(sizeof(entry_list));
        if (p != NULL)
        {
        	entry_list* lista = (*table)[hashValue];
        	
        	/* verifica se o elemento já existe na lista, se existir, retorna -1 */
        	while (lista != NULL)
        	{
        		if (!strcmp(lista->entry->name,entry->name))
        		  return -1;
        		lista = lista->next;
        	}
        	
        	/* se chegou aqui, elemento não existe na lista e faz a inserção no início da lista */
            p->entry = entry;
            p->next = (*table)[hashValue];
            (*table)[hashValue] = p; 
            return 0;   
        }
        else
          return -1;
    }
}

/** \brief Destruir a tabela de Hash.
 *
 * 'free_table' eh o destrutor da estrutura de dados. Deve ser chamado pelo 
 * usuario no fim de seu uso de uma tabela de simbolos.
 * @param table, uma referencia sobre uma tabela de simbolos.
 */
void free_table(symbol_t* table)
{
	int i;
	for (i = 0; i < TAMANHO ; i++)
	{
		entry_list* p = (*table)[i];
		entry_list* temp;
		while (p != NULL)
		{
			temp = p;
			p = p->next;
			free(temp);
		}
		(*table)[i] = NULL;
	}
}

/** \brief Imprimir o conteudo de uma tabela.
 *
 * A formatacao exata e deixada a carga do programador. Deve-se listar todas
 * as entradas contidas na tabela atraves de seu nome (char*). Deve retornar
 * o numero de entradas na tabela.
 *
 * @param table, uma tabela de simbolos.
 * @return o numero de entradas na tabela.
 */
int print_table(symbol_t table)
{
	int numEntradas = 0, i;
	int numPonteiros;
	
	for (i = 0; i < TAMANHO ; i++)
	{
		entry_list* p = table[i];
		if (p == NULL)
		  continue;
		while (p != NULL)
		{
			numEntradas++;
			printf("'%s' ",p->entry->name);
			if (p->entry->numPonteiros != 0)
				printf("ARRAY_MAL ");
			else if (p->entry->isArray)
				printf("ARRAY_DIM ");
			else
				printf("NOT_ARRAY ");
			printf("'%s",p->entry->tipo_str);
			numPonteiros = p->entry->numPonteiros;
			while (numPonteiros != 0)
			{
				printf("*");
				numPonteiros--;
			}
			printf("' ");
 			if (p->entry->isArray)
			{
				printf("%d ",p->entry->dimArray->numDim);
				lista_dim* q = p->entry->dimArray->dimensoes;
				while (q != NULL)
				{
					printf("%d ",q->dim);
					q = q->prox;
				}
			}
			printf("'%s' ",p->entry->escopo);
			printf("'%s' ",p->entry->arquivo);
			
			lista_RW* aux = p->entry->listaAcessos;
			if (aux == NULL)
				printf("- ");
			else
			{
				while (aux != NULL)
				{
					if (aux->tipoAcesso == READ)
						printf("(R,%d,%d,'%s') ",aux->linhaFor,aux->colunaFor,aux->arquivo);
					else if (aux->tipoAcesso == WRITE)
						printf("(W,%d,%d,'%s') ",aux->linhaFor,aux->colunaFor,aux->arquivo);
					else if (aux->tipoAcesso == READ_WRITE)
						printf("(RW,%d,%d,'%s') ",aux->linhaFor,aux->colunaFor,aux->arquivo);
					else	
						printf("(I,-1) ");
					aux = aux->prox;
				}
			}
			
			p = p->next;
   		  	printf("\n");
		}		
	}
	
	return numEntradas;
}

/** \brief Imprimir o conteudo de uma tabela em um arquivo.
 *
 * A formatacao exata e deixada a carga do programador. Deve-se listar todas
 * as entradas contidas na tabela atraves de seu nome (char*). Deve retornar
 * o numero de entradas na tabela. A saida deve ser dirigida para um arquivo,
 * cujo descritor e passado em parametro.
 *
 * @param out, um descritor de arquivo (FILE*).
 * @param table, uma tabela de simbolos.
 * @return o numero de entradas na tabela.
 */
int print_file_table(FILE* out, symbol_t table)
{
        out = fopen("symbol_table.txt","w");
    
	int numEntradas = 0, i;
	
	for (i = 0; i < TAMANHO ; i++)
	{
		entry_list* p = table[i];
		if (p == NULL)
		  continue;
		while (p != NULL)
		{
			numEntradas++;
			fwrite(p->entry->name,1,strlen(p->entry->name),out);
			p = p->next;
			if (p != NULL)
			  fwrite(" - ",1,strlen(" - "),out);
		}		
		fwrite("\n",1,strlen("\n"),out);
	}	
	fclose(out);
	
	return numEntradas;
}
