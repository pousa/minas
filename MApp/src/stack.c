#include "stack.h"
#include <stdlib.h>

/**  \brief Inicializar a pilha.
 *          Uso tipico: init_stack(&minha_pilha);
 *
 * Inicializa a pilha, allocando qualquer espaço na memoria que seja
 * necessario. Nao se deve efetuar nenhuma hipotese restritiva quanto ao numero
 * total de entradas que podera conter a pilha num dado instante.
 *
 * @param s um ponteiro sobre uma pilha (stack*).
 * @return nada (void).
 */
void init_stack(stack* s) 
{
     *s = NULL;
}

/** \brief Testar se a pilha esta vazia.
 *
 * Testa se a pilha esta vazia.
 * @param s uma pilha
 * @return 
 *    - 0 se a pilha nao esta vazia, 
 *    - um valor diferente de zero se a pilha esta vazia.
 */
int empty(stack s)
{
    if (s == NULL)
      return 1;
    else
      return 0;
}   

/** \brief Empilhar um elemento na pilha.
 *         (O tipo do elemento é void *.)
 *
 * Empilha um elemento na pilha.
 * @param s, uma referencia sobre a pilha onde se deve inserir o elemento.
 * @param item, uma referencia sobre o elemento a ser inserido.
 * @return 0 se a insercao deu certo.
 */
int push(stack* s, void* item) 
{
    stack newNode = (stack)malloc(sizeof(struct Tstack));
    if (newNode == NULL)
       return 1;
    newNode->value = item;
    newNode->next = *s;
    *s = newNode;
    return 0;
}

/** \brief Desempilhar o elemento no topo da pilha.
 *
 *  Desempilha o elemento no topo da pilha, e retorna-o. Remove este elemento 
 *  da pilha.
 *  @param s, um ponteiro sobre a pilha de onde se deve tirar um elemento.
 *  @return o elemento que foi desempilhado, ou NULL se nao tinha como 
 *  desempilhar alguma coisa. 
 */
void* pop(stack* s) 
{
   if (empty(*s))
      return NULL;
   
   void* item = (*s)->value;
   stack temp = *s;
   *s = (*s)->next;
 //  free(temp);
   
   return item;
}

/** \brief Consultar o elemento no topo da pilha.
 *
 *  Retorna um ponteiro sobre o elemento no topo da pilha. Nao remove este 
 *  elemento da pilha.
 *  @param s, a pilha de que se deve consultar o topo.
 *  @return o elemento, ou NULL se nao tinha nada no topo.
 */
void* top(stack s)
{
      if (empty(s))
         return NULL;
         
      return s->value;      
}

/**  \brief Liberar a memoria usada pela pilha.
 *          Uso tipico: free_stack(&minha_pilha);
 *
 * 'free_stack' eh o destrutor da estrutura de dados de pilha. Deve liberar
 * qualquer espaco na memoria que tenha sido alocado para a implementacao
 * interna da pilha passada em argumento. Um acesso a uma pilha, depois da
 * chamada a 'free_stack' levara a um erro na execucao.
 *
 * @param s um ponteiro sobre uma pilha (stack*).
 * @return nada (void).
 */
void free_stack (stack* s)
{
     stack temp;
          
     while (!empty(*s))
     {
           temp = *s;
           *s = (*s)->next;
           free(temp);
     }    
     *s = NULL;
}
