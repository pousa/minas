/*=====================================================================================
  MAi is a interface to manage memory policy in NUMA architectures

  (C) Copyright 2010 INRIA 
  Projet: MESCAL / ANR NUMASIS

  Author: Christiane Pousa Ribeiro

  The author may be reached at pousa@imag.fr
 *====================================================================================*/

#include <sys/mman.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stddef.h>
#include <numa.h>
#include <math.h>


#include "common.h"
#include "numArch.h"

/*
Set informartion in place_info structure
parameters: filename-the name of the configuration file
return: none
*/
void set_place_info(char filename[])
{
  FILE *fp;
  int tmp_node,tcpu,i,cont,err,max_node=0;
  int nThreads;

  max_node = na_get_maxnodes();

  //getting system information - number of cpus/cores
  sinfo.ncpus = na_get_maxcpus();
  //getting system information - number of cpus/cores per node
  sinfo.ncoresNodes = na_get_cpusnode();

  sinfo.nodes = malloc(sinfo.nnodes*sizeof(unsigned long)); 

  //getting neighbors for all nodes of the machine 
  sinfo.nneighbors = na_get_maxneighbors();
  sinfo.neighbors = malloc(sinfo.nnodes*sizeof(int*));
  for(i=0;i<sinfo.nnodes;i++)
    sinfo.neighbors[i] = na_get_nneighbors(i);
  
  fp = fopen(filename,"r");

  /*Use programmer configurations*/
  if ( fp != NULL )
  {
    fscanf (fp,"%d",&(sinfo.nnodes));
    sinfo.nodes = malloc(sinfo.nnodes*sizeof(unsigned long)); 
    sinfo.lnodes = malloc(sinfo.nnodes*sizeof(unsigned long));
 
    for(i=0;i<sinfo.nnodes;i++)
     {
      fscanf( fp, "%d", &tmp_node);
      if(tmp_node > max_node)//node do not exists
	{
         printf("\n In these architecture the maximum number of a node is %d\n", 
		max_node);
	 exit(1);
	}  
      sinfo.nodes[i] = 1<<tmp_node;
      sinfo.lnodes[i] = tmp_node;
     }
     
    err = fscanf (fp,"%d",&(sinfo.nthreads));
    if(err != EOF)
      {
	sinfo.cpus = malloc(sinfo.nthreads*sizeof(unsigned long)); 
	sinfo.lcpus = malloc(sinfo.nthreads*sizeof(unsigned long)); 

	for(i=0;i<sinfo.nthreads && i<MAXTHREADS;i++)
	  {
	    fscanf( fp, "%d", &tcpu);

	    if(tcpu > sinfo.ncpus-1)//cpu|core do not exists
	      {
		printf("\n In these architecture the maximum number of a"
		       "cpu/core is %d\n", sinfo.ncpus-1);
		exit(1);
	      }  
	    sinfo.lcpus[i] = tcpu;	    
	    sinfo.cpus[i] = 1 << tcpu;
	  }
      }
         fclose(fp);
  }
  else /*Use mai automatic system configuration*/
  {
    
    sinfo.nnodes = max_node;
    sinfo.nodes = malloc(sinfo.nnodes*sizeof(unsigned long));
    sinfo.lnodes = malloc(sinfo.nnodes*sizeof(unsigned long));
   
    for(i=0;i<sinfo.nnodes;i++){
	 sinfo.nodes[i] = 1 << i; 
         sinfo.lnodes[i] = i;
	}   
  }

  if(sinfo.cpus == NULL) //if no cpu/core were configured to be used
    {
      int *cpu_node = NULL, round=0;
      char *ret=NULL;
      
      #ifdef OMP	
        ret = getenv("OMP_NUM_THREADS") ;
        if(ret != NULL){
         nThreads = atoi(ret);
         sinfo.nthreads = nThreads;  
	 omp_set_num_threads(nThreads);}
        else if(omp_get_num_threads()!=0)
	  sinfo.nthreads = omp_get_num_threads();
        else
          sinfo.nthreads = 0;
          #ifdef DEBUG 
          printf("\n %d\n",sinfo.nthreads);
         #endif
      #endif
	  int sharedCache = na_get_shared_cacheLevel();
	  if(sharedCache < 0)//no shared cache
	    { fp = fopen("/tmp/output/inter_placement.minas","r"); sinfo.threadPol = INTER;}
	  else
	    { fp = fopen("/tmp/output/intra_placement.minas","r"); sinfo.threadPol = INTRA;}

		sinfo.cpus = malloc(sinfo.nthreads*sizeof(unsigned long)); 
		sinfo.lcpus = malloc(sinfo.nthreads*sizeof(unsigned long)); 
	  
		if(fp == NULL){ printf("Error open minas files!\n"); exit(-1); }

		for(i=0;i<sinfo.nthreads && i<MAXTHREADS;i++)
		  {
		    fscanf( fp, "%d", &tcpu);
		    if(tcpu > sinfo.ncpus-1)//cpu|core do not exists
		      {
			printf("\n In these architecture the maximum number of a"
		       "cpu/core is %d\n", sinfo.ncpus-1);
			exit(1);
		      }  
		    sinfo.lcpus[i] = tcpu;
		    sinfo.cpus[i] = 1 << tcpu;
		  }
		fclose(fp);      
       }  
}

/*Initialize the structure
parameters: none
return: none
*/
void init_var_info()
{
  int i;
  for(i=0;i<MAI_HASH_SIZE;i++)
   arraysHash[i]=NULL;
}

/*Initialize the structure
parameters: none
return: none
*/
void init_place_info()
{
  sinfo.nodes = NULL;
  sinfo.lnodes = NULL;
  sinfo.nthreads = 0;
  sinfo.nnodes = 0;
  sinfo.ncpus = 0;
  sinfo.nneighbors = 0;
  sinfo.neighbors = NULL;  
  sinfo.threadPol = 0; 
  sinfo.cpus = NULL;
  sinfo.lcpus = NULL;

}

struct var_info* create_var_info()
{
  struct var_info * new_item;
  new_item = (struct var_info *) malloc(sizeof(struct var_info));
  pthread_mutex_init(&(new_item->lock_var), NULL);

  return new_item;
}

/*Set information about the application variables
parameters: ph-pointer to the first dimension of the array, pl-pointer to the lowest 
            dimension of the array, dimension[]=number of elements in each dimension,
	    size_item-size in bytes of an element, t-type of the array element
return: none
*/
void set_var_info(void *ph,void *pl,int ndim, int dimensions[],size_t size_item,size_t tsize,int type,struct var_info* new_item)
{
  int n=1,i;
  struct var_info *aux;
  void *p;

  //number of total elements 
  for(i=0;dimensions[i]!=0;i++)
    n *= dimensions[i];

  //alloc new variables information
  new_item->phigh=ph;
  new_item->plow=pl;
  new_item->nitems=n;
  new_item->block=0;
  new_item->block_size = 0;
  new_item->ndim=ndim;
  new_item->tsize=tsize;
  new_item->policy=-1;
  for(i=0;i<4;i++)
    new_item->dimensions[i] = dimensions[i];
  new_item->size=size_item;
  new_item->type=type;
  new_item->nodes = (int *) malloc(na_get_maxnodes()*sizeof(int));
  for(i=0;i<na_get_maxnodes();i++)
   new_item->nodes[i]=-1;
  new_item->child=NULL;
  new_item->parent=NULL;
  

  p = new_item->phigh;
  long key = MAI_HASH(p);

  
 #ifdef DEBUG
  printf("HASH key: %lu plhigh %p\n",key,new_item->phigh);
 #endif
 
  add_hash(key,new_item);

}

/*Set information about the application variables
parameters: ph-pointer to the first dimension of the array, pl-pointer to the lowest 
            dimension of the array, dimension[]=number of elements in each dimension,
	    size_item-size in bytes of an element, t-type of the array element
return: none
*/
void set_var_info_(void *ph,void *pl,int ndim, int dimensions[],size_t size_item,size_t tsize,int type)
{
  int n=1,i;
  struct var_info *aux,*new_item;
  void *p;

  //number of total elements 
  for(i=0;dimensions[i]!=0;i++)
    n *= dimensions[i];

   new_item = (struct var_info *) malloc(sizeof(struct var_info));

  //alloc new variables information
  new_item->phigh=ph;
  new_item->plow=pl;
  new_item->nitems=n;
  new_item->block=0;
  new_item->block_size = 0;
  new_item->ndim=ndim;
  new_item->tsize=tsize;
  new_item->policy=-1;
  for(i=0;i<4;i++)
    new_item->dimensions[i] = dimensions[i];
  new_item->size=size_item;
  new_item->type=type;
  new_item->nodes = (int *) malloc(na_get_maxnodes()*sizeof(int));
  for(i=0;i<na_get_maxnodes();i++)
   new_item->nodes[i]=-1;
  new_item->child=NULL;
  new_item->parent=NULL;
  pthread_mutex_init(&(new_item->lock_var), NULL);

  p = new_item->phigh;
  long key = MAI_HASH(p);

  
  #ifdef DEBUG
  printf("HASH key: %lu plow %p\n",key,new_item->plow);
  #endif
 
  add_hash(key,new_item);

}


/*Add variable information in MAI HASH table*/
void add_hash(long key, struct var_info *new_var)
{
 struct var_info *act;
 
 long hashval = key % MAI_HASH_SIZE;
 
  #ifdef DEBUG
  printf("HASH key: %lu  HASH entry: %lu \n",key,hashval);
 #endif
 
  if(arraysHash[hashval] == NULL) {
      arraysHash[hashval] = new_var;
      arraysHash[hashval]->parent = NULL;
      arraysHash[hashval]->child = NULL;
   #ifdef DEBUG
  printf("Ph %p \n",new_var->phigh);
  #endif
   }
   else {
      act=arraysHash[hashval];
      while(act->child != NULL) 
         act=act->child;
      act->child = new_var;
      act->child->child = NULL;
      act->child->parent = act;
   #ifdef DEBUG
  printf("Ph prim %p Ph sec %p \n",act->phigh,new_var->phigh);
  #endif
   }
}

void get_lock(void *ph)
{
  long key = MAI_HASH(ph);
  struct var_info *aux=NULL;
  long  hashval = key % MAI_HASH_SIZE;

#ifdef DEBUG 
  printf("HASH key: %lu  HASH entry: %lu \n",key,hashval);
#endif

  if(arraysHash[hashval] == NULL){
    printf("\n variable NOT managed by Minas");
    exit(-1);
  }
  else if(arraysHash[hashval]->child == NULL){
    aux = arraysHash[hashval];
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux == NULL)
   {
     printf("\n variable NOT managed by Minas");
     exit(-1);
    }
   }

  pthread_mutex_lock(&(aux->lock_var));
}

void release_lock(void *ph)
{
  long key = MAI_HASH(ph);
  struct var_info *aux=NULL;
  long  hashval = key % MAI_HASH_SIZE;
   
 

  if(arraysHash[hashval]==NULL){
    printf("\n variable NOT managed by Minas");
    exit(-1);
  }
  else if(arraysHash[hashval]->child == NULL){
    aux = arraysHash[hashval];
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux == NULL)
   {
     printf("\n variable NOT managed by Minas");
     exit(-1);
    }
   }


  pthread_mutex_unlock(&(aux->lock_var));
}

/*Set block for a variable
  parameters: ph-pointer to the first dimension of the array
  return: none
*/
void set_var_block(void *ph, int block)
{
 struct var_info *aux=NULL;
 long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
   printf("Not found!");
   exit(1);
  }
  else if(arraysHash[hashval]->child == NULL){
    arraysHash[hashval]->block=block;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
    aux->block=block; 
   }
  
}

/*Set block for a variable
  parameters: ph-pointer to the first dimension of the array
  return: none
*/
void set_var_block_size(void *ph, size_t block_size)
{
 struct var_info *aux=NULL;
 long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
   printf("Not found!");
   exit(1);
  }
  else if(arraysHash[hashval]->child == NULL){
    arraysHash[hashval]->block_size=block_size;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
    aux->block_size=block_size; 
   }
  
}

/*Set memory policy for a variable
  parameters: ph-pointer to the first dimension of the array
  return: none
*/
void set_var_policy(void *ph, int policy)
{

  long key = MAI_HASH(ph);
  struct var_info *aux=NULL;
  long  hashval = key % MAI_HASH_SIZE;

#ifdef DEBUG 
  printf("HASH key: %lu  HASH entry: %lu \n",key,hashval);
#endif

  if(arraysHash[hashval] == NULL){
    printf("\n variable NOT managed by Minas");
    exit(-1);
  }
  else if(arraysHash[hashval]->child == NULL){
    arraysHash[hashval]->policy=policy;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux == NULL)
   {
     printf("\n variable NOT managed by Minas");
     exit(-1);
    }
    else
	    aux->policy=policy;
   }

}

/*Set nodes for a variable
  parameters: ph-pointer to the first dimension of the array
  return: none
*/
void set_var_nodes(void *ph, int nnodes, int nodes[])
{
  struct var_info *aux=NULL;
 long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
   printf("Var nodes - Not found!");
   exit(1);
  }
  else if(arraysHash[hashval]->child == NULL){
    arraysHash[hashval]->nnodes=nnodes;
    arraysHash[hashval]->nodes=nodes;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    arraysHash[hashval]->nnodes=nnodes;
    arraysHash[hashval]->nodes=nodes;
    }
   }
  
}

/*Set node for a variable
  parameters: ph-pointer to the first dimension of the array
  return: none
*/
void set_var_node(void *ph, int nnodes, int nodes)
{
  struct var_info *aux=NULL;
 long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
   printf("Not found!");
   exit(1);
  }
  else if(arraysHash[hashval]->child == NULL){
    pthread_mutex_lock(&arraysHash[hashval]->lock_var);
    arraysHash[hashval]->nnodes=nnodes;
    arraysHash[hashval]->nodes[0]=nodes;
    pthread_mutex_unlock(&arraysHash[hashval]->lock_var);
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    pthread_mutex_lock(&aux->lock_var); 
    arraysHash[hashval]->nnodes=nnodes;
    arraysHash[hashval]->nodes[0]=nodes;
    pthread_mutex_unlock(&aux->lock_var); 
    }
   }
 

}


int get_true_pointer(void *ph)
{
 struct var_info *aux=NULL;
 long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
    return 0;
  }
  else if(arraysHash[hashval]->child == NULL){
    return 1;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    return 1;
    }
   }
}

void* get_true_pointer_handler(void *ph)
{
 struct var_info *aux=NULL,*rpointer=NULL;
 int i,find=0;

  for(i=0;i<MAI_HASH_SIZE && !find ;i++)
  {
    aux = arraysHash[i];
    //printf ("\n aux %p",aux);
    while(aux != NULL && !find) 
	{
		if(ph >= aux->phigh && ph < (aux->phigh+get_var_tsize(aux->phigh)))
			{ find = 1; rpointer = aux->phigh;}
        	aux=aux->child;
	}
    
  }
  return rpointer;
}

void* get_pointer_block(void *p,void *realp,size_t block)
{
   void *aux = realp,*p_block=NULL;
   int find=0;

   while (aux < (realp+get_var_tsize(realp)))
	{
		if (p>=aux && p<(aux+block))
		  { find = 1; p_block = aux;}
		aux = aux+block;
	}

  return p_block;
}

/*Get block for a variable
  parameters: ph-pointer to the first dimension of the array
  return: none
*/
int get_var_block(void *ph)
{
  struct var_info *aux=NULL;
 long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
    return 0;
  }
  else if(arraysHash[hashval]->child == NULL){
    return arraysHash[hashval]->block;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    return aux->block;
    }
   }
}

/*Get block for a variable
  parameters: ph-pointer to the first dimension of the array
  return: none
*/
size_t get_var_block_size(void *ph)
{
  struct var_info *aux=NULL;
 long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
    return 0;
  }
  else if(arraysHash[hashval]->child == NULL){
    return arraysHash[hashval]->block_size;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    return aux->block_size;
    }
   }
}


/*Get information about a application variable
  parameters: ph-pointer to the first dimension of the array
  return: memory policy
*/
int get_var_policy(void *ph)
{
    struct var_info *aux=NULL;
 long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
    return -1;
  }
  else if(arraysHash[hashval]->child == NULL){
    return arraysHash[hashval]->policy;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    return aux->policy;
    }
   }
}

/*Get nodes for a variable
  parameters: ph-pointer to the first dimension of the array
  return: none
*/
int* get_var_nodes(void *ph, int *nnodes)
{
  struct var_info *aux=NULL;
  long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
    *nnodes=0;
    return NULL;
  }
  else if(arraysHash[hashval]->child == NULL){
    *nnodes = arraysHash[hashval]->nnodes;
    return arraysHash[hashval]->nodes;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    *nnodes = aux->nnodes;
    return aux->nodes;
    }
   }

}

/*Get information about a application variable
  parameters: ph-pointer to the first dimension of the array
  return: a pointer for the lowest dimension of the array
*/
void* get_var_pl(void *ph)
{
  struct var_info *aux=NULL;
  long key = MAI_HASH(ph);
  long hashval = key % MAI_HASH_SIZE;

  #ifdef DEBUG
  printf("Ph %p HASH entry: %ld \n",ph,hashval);
  #endif

  if(arraysHash[hashval]==NULL)
  {
    #ifdef DEBUG
  printf("NULL:  \n");
  #endif
    return NULL;
  }
  else if(arraysHash[hashval]->child == NULL){
    #ifdef DEBUG
  printf("pl: %p \n",arraysHash[hashval]->plow);
  #endif
    return arraysHash[hashval]->plow;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    #ifdef DEBUG
  printf("pl: %l \n",aux->plow);
  #endif
    return aux->plow;
    }
   }

  
}

/*Get information about a application variable
  parameters: pl-pointer to the last dimension of the array
  return: a pointer for the highest dimension of the array
*/
void* get_var_ph(void *pl)
{
  struct var_info *aux;
  void *pointer=NULL;
  int i=0;

  //look for the right array
  while(i<MAI_HASH_SIZE)
  { 
   aux = arraysHash[i];
    while(aux!=NULL)
    {
      if( (pl>= aux->plow) && (pl <= (aux->plow+(aux->nitems*aux->size))))
       {
       pointer = aux->phigh;
       aux = NULL;
       i = MAI_HASH_SIZE;
      }
       else 
       aux=aux->child;
    }
   i++;
  }

  return pointer;
}

/*Get information about a application variable
  parameters: ph-pointer to the first dimension of the array
  return: number of array items 
*/
int get_var_itens(void *ph)
{
struct var_info *aux=NULL;
  long key = MAI_HASH(ph);

  long hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
    return 0;
  }
  else if(arraysHash[hashval]->child == NULL){
    return arraysHash[hashval]->nitems;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    return aux->nitems;
    }
   }
}

/* Get information about a application variable
parameters: ph-pointer to the first dimension of the array
return: size in bytes of an array item
*/
size_t get_var_size(void *ph)
{
struct var_info *aux=NULL;
  long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
    return 0;
  }
  else if(arraysHash[hashval]->child == NULL){
    return arraysHash[hashval]->size;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    return aux->size;
    }
   }
}

/*Get information about a application variable
parameters: ph-pointer to the first dimension of the array
return: type of an array item
*/
int get_var_type(void *ph)
{
struct var_info *aux=NULL;
  long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
    return 0;
  }
  else if(arraysHash[hashval]->child == NULL){
    return arraysHash[hashval]->type;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    return aux->type;
    }
   }
}

/*Get information about a application variable
parameters: ph-pointer to the first dimension of the array
return: number of dimensions of an array item
*/
int get_var_ndim(void *ph)
{
struct var_info *aux=NULL;
  long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
    return 0;
  }
  else if(arraysHash[hashval]->child == NULL){
    return arraysHash[hashval]->ndim;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    return aux->ndim;
    }
   }
}


/*Get information about a application variable
parameters: ph-pointer to the first dimension of the array
            dim-dimension number
return: nitems of a dimension
*/
int get_var_dim(void *ph,int dim)
{
 struct var_info *aux=NULL;
  long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
    return 0;
  }
  else if(arraysHash[hashval]->child == NULL){
    return arraysHash[hashval]->dimensions[dim-1];
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    return aux->dimensions[dim-1];
    }
   }
}

/* Get information about a application variable
parameters: ph-pointer to the first dimension of the array
return: total size of an array item
*/
size_t get_var_tsize(void *ph)
{
  struct var_info *aux=NULL;
  long key = MAI_HASH(ph);

  long  hashval = key % MAI_HASH_SIZE;

  if(arraysHash[hashval]==NULL)
  {
     printf("Var tsize - not found!");
     return 0;
  }
  else if(arraysHash[hashval]->child == NULL){
    return arraysHash[hashval]->tsize;
  }
  else
  {
    aux = arraysHash[hashval];
    //look for the right array
   while(aux->phigh != ph && aux!=NULL) 
     aux=aux->child;

   if(aux != NULL)
   {
    return aux->tsize;
    }
   }
}


/*align memory to be allocated
parameter=size
return=new size
*/
size_t alignmem(size_t mem) {
	unsigned long newmem;

	if (mem < PAGE_SIZE) {
		return PAGE_SIZE;
	}
	if (mem % PAGE_SIZE == 0) {
		return mem;
	}
	newmem = mem/PAGE_SIZE;
    	return (newmem+1) * PAGE_SIZE;
}

/*
Display the error for the memory policy setting
parameters: none
return: none
*/
void identify_error()
{
  if(errno == EFAULT)
    printf("\nerrno EFAULT");
    else if (errno == EINVAL)
      printf("\nerrno EINVAL");
      else if(errno==EPERM)
	printf("\nerro EPERM");
        else if (errno == EACCES)
          printf("\nerrno EACCES");
          else if (errno == EBADF)
            printf("\nerrno EBDAF");
            else if (errno == EIO)
              printf("\nerrno EIO");
              else if(errno==ENOMEM)
                printf("\nerro ENOMEM");
                else if(errno==ESRCH)
	          printf("\nerro ESRCH");
                  else  
		    printf("\nUnknow erro");
}

/*
Author: Fabrice Dupros
Get the current time
parameters: none
return: time in sec
*/
double mai_my_second()
{

        struct timeval tp;
        struct timezone tzp;
        int i;

        i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}


unsigned long* compute_elegible_nodes_band()
{
  int i,j,find,n1,n2;
  unsigned long *nodes;
  float *band,aux,high;

  band = malloc((sinfo.nnodes-1)*sizeof(double));
  nodes = malloc((sinfo.nnodes)*sizeof(unsigned long));

  n1=nodes[0]=sinfo.nodes[0];

  for(i=1;i<sinfo.nnodes;i++)
  { 
   n2 = sinfo.nodes[i];
   n2 = (log(n2)/log(2))+1;
   band[i-1]=mai_bandwidth(n1,n2);
  }

 for(j=1;j<sinfo.nnodes;j++) {
  high = band[0];
  find = 2;
  for(i=1;i<sinfo.nnodes-1;i++){
   if(high < band[i]){
    high = band[i];
    find = i+2;
   }
    band[find-2]=-1;
    nodes[j]=find;
  }}
  
  return nodes;
}

unsigned long* compute_elegible_nodes_lat()
{
  int i,j,find,n1,n2;
  unsigned long *nodes;
  float *lat,aux,low;

  lat = malloc((sinfo.nnodes-1)*sizeof(float));
  nodes = malloc((sinfo.nnodes)*sizeof(unsigned long));

  n1=nodes[0]=sinfo.nodes[0];

  for(i=1;i<sinfo.nnodes;i++)
  { 
   n2 = sinfo.nodes[i];
   n2 = (log(n2)/log(2))+1;
   lat[i-1]=mai_numafactor(n1,n2);
  }

 for(j=1;j<sinfo.nnodes;j++) {
  low = lat[0];
  find = 2;
  for(i=1;i<sinfo.nnodes-1;i++){
   if(low > lat[i]){
    low = lat[i];
    find = i+2;
   }
    lat[find-2]=+100;
    nodes[j]=find;
  }}
  
  return nodes;
}

int convert_node(unsigned long *node)
{
  int i,idx_node=-1;
 
  for(i = 0; i < sinfo.nnodes && idx_node == -1; i++) 
     if (mask_isset(node,MASK_SIZE,i)) 
	  idx_node = i;

  return idx_node;
}


int convert_cpu(unsigned long *cpu)
{
int i,idx_cpu=-1;
 int max_cpu = na_get_maxcpus();

  for(i = 0; i < max_cpu && idx_cpu == -1; i++) 
     if (mask_isset(cpu,CPU_MASK_SIZE,i)) 
	  idx_cpu = i;

  return idx_cpu;

}

