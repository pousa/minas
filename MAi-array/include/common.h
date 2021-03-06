/*=====================================================================================
  MAi is a interface to manage memory policy in NUMA architectures

  (C) Copyright 2010 INRIA 
  Projet: MESCAL / ANR NUMASIS

  Author: Christiane Pousa Ribeiro

  The author may be reached at pousa@imag.fr
 *====================================================================================*/

#ifndef __COMMUNS_H
#define __COMMUNS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Data types*/
#define CHAR    1
#define DOUBLE  2
#define FLOAT   3
#define INT     4
#define STRUCT 5

/* System information*/
#define PAGE_SIZE getpagesize()
#define MASK_SIZE 8*sizeof(unsigned long)
#define CPU_MASK_SIZE sizeof(unsigned long)
#define MAXNODES 256
#define MAXTHREADS 512
#define NPAG (na_get_tcache()*1024)/getpagesize()
#define KBYTE 1024

/*Manage memory number of pages*/
#define REST(x)  (((x)%PAGE_SIZE)==0) ? 1:0

/* manage bitmap */
#define BITS_IN_BYTES 8
#define WORD_SIZE	(8*sizeof(unsigned long))
#define BITS_IN_WORDS (BITS_IN_BYTES*WORD_SIZE)

/* Mark/Clear bits with given index */
#define markbit(M,i)       (M[i / BITS_IN_WORDS] |= 1 << (i % BITS_IN_WORDS))
#define clearbit(M,i)      (M[i / BITS_IN_WORDS] &= ~(1 << (i % BITS_IN_WORDS)))
#define bit_is_marked(M,i) (M[i / BITS_IN_WORDS] & (1 << (i % BITS_IN_WORDS)))

#define MAI_HASH(x)     ({ \
                                    unsigned long y = (unsigned long ) x; \
                                    y = (y>>11) * 654435761UL; \
                                    y; \
                                 })
#define MAI_HASH_SIZE 128

/*Maximun support*/
#define MAXTHREADS 512
#define MAXPAGES 8192
#define INTRA 1
#define INTER 0

/*Memory policies*/
#define CYCLIC 0
#define CYCLIC_B 1
#define SKEW 2
#define SKEW_B 3
#define PRIME 4
#define PRIME_B 5
#define BIND 6
#define BIND_B 7
#define RANDOM 8
#define RANDOM_B 9
#define OWN 10
#define NEXTOUCH 11
#define NEXTOUCHN 12

/*Get threads id*/
#define tid() ((pid_t) syscall(SYS_gettid))


#ifdef OMP
#include "omp.h"
#endif 

#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

/*Information about variables*/
struct var_info
{

  int type;
  int policy;
  int nitems;
  int ndim;  
  int dimensions[4];
  int nnodes;
  int block;  //for own memory policies
  size_t tsize;
  size_t size;
  size_t block_size; //block size for bind_block memory policies
  pthread_mutex_t lock_var;
  int *nodes;
  void *phigh,*plow;
  struct var_info *parent,*child;
};

/*Information to place memory/threads*/
struct place_info
{
  int nthreads;
  int nnodes;
  int ncpus;
  int ncoresNodes;
  int threadPol;
  int nneighbors;
  int **neighbors;
  unsigned long *cpus,*lcpus;
  unsigned long *nodes,*lnodes;
  pthread_mutex_t lock_place; 
};

/* malloc statistics */
struct mai_malloc {
	size_t used_size;
	size_t nmmaps;
	size_t block_h;
	int npinfo;
};

typedef struct mai_malloc mai_stats;

/*Common variables*/
pthread_mutex_t mai_mutex;
struct place_info sinfo;
struct var_info *arraysHash[MAI_HASH_SIZE];
double time_last_pmig,time_last_tmig;
pid_t main_pid;

void add_hash(long  key, struct var_info *var);
struct var_info* create_var_info();

/*Configuration structures - function prototype*/
void set_place_info(char filename[]);

/*Initialize the structure*/
void init_var_info();
void init_place_info();

/*set information about the application variables*/
void set_var_info(void *ph,void *pl,int ndim, int dimensions[], 
               size_t size_item,size_t tsize,int type,struct var_info* new_item);
void set_var_info_(void *ph,void *pl,int ndim, int dimensions[], 
               size_t size_item,size_t tsize,int type);
void set_var_policy(void *ph, int policy);
void set_var_nodes(void *ph, int nnodes, int nodes[]);
void set_var_block(void *ph, int block);

/*get information about a application variable*/
int get_var_block(void *ph);
int get_var_policy(void *ph);
void* get_var_pl(void *ph);
int get_var_itens(void *ph);
size_t get_var_size(void *ph);
int get_var_type(void *ph);
int get_var_dim(void *ph,int dim);
int* get_var_nodes(void *ph,int *nnodes);
size_t get_var_tsize(void *ph);
int get_var_ndim(void *ph);
int get_true_pointer(void *ph);
void* get_true_pointer_handler(void *ph);
void* get_pointer_block(void *p,void *realp,size_t block);
void set_var_block_size(void *ph, size_t block_size);
size_t get_var_block_size(void *ph);

/*Display error*/
void identify_error(); 

/*Get time*/
double mai_my_second();

/*Compute elegible nodes*/
unsigned long* compute_elegible_nodes_band();
unsigned long* compute_elegible_nodes_lat();

void release_lock(void *ph);
void get_lock(void *ph);

int convert_node(unsigned long *node);
int convert_cpu(unsigned long *cpu);

static inline int mask_isset(unsigned long *a, unsigned long sa, int node) {
	if ((unsigned int)node >= sa*8) return 0; 
	return bit_is_marked(a,node);
}


#ifdef __cplusplus
}
#endif

#endif


