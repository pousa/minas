/*
 * include/heap_alloc.h
 *
 * Definition of memory allocator inside a predefined heap
 * working for  NUMA architecture
 * and multithread environements
 *
 * Authors: Martinasso Maxime
 *          Christiane Pousa Ribeiro
 *
 * (C) Copyright 2010 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 * 
 */

#ifndef __HEAP_ALLOC_H
#define __HEAP_ALLOC_H

#include<stddef.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>

/* System information*/
#define PAGE_SIZE getpagesize()
#define MASK_SIZE 8*sizeof(unsigned long)
#define NPAG (get_totalcache()*1024)/getpagesize()
#define CPU_MASK_SIZE sizeof(unsigned long)

/* manage heap */
#define HEAP_DYN_ALLOC 	0
#define HEAP_PAGE_ALLOC 1
#define HEAP_MINIMUM_PAGES 200000
#define HEAP_UNSPECIFIED_POLICY -1
#define HEAP_UNSPECIFIED_WEIGHT -1
#define HEAP_ITERATOR_ID_UNDEFINED -1
#define HEAP_NBMAXNODES 128
#define HEAP_OBJECTS 256

#define HEAP_INIT 1024

#define HEAP_SIZE_T 	(ha_memalign(sizeof(ha_heap_t)))
#define BLOCK_SIZE_T 	(ha_memalign(sizeof(ha_ub_t)))

#define IS_HEAP(h)			(h != NULL)
#define IS_HEAP_POLICY(h,p)		(h != NULL && h->alloc_policy == p)
#define HEAP_ADD_FREE_SIZE(sz,h) 	{ h->free_size += sz; h->used_size -= sz; }
#define HEAP_ADD_USED_SIZE(sz,h) 	{ h->free_size -= sz; h->used_size += sz; }
#define HEAP_GET_SIZE(h) 		( h->free_size + h->used_size )

/* mapping policies over numa nodes */
#define CYCLIC 0
#define LESS_LOADED 1
#define SMALL_ACCESSED 2
#define BIND 3 
#define PRIME 4
#define CYCLICN 5
#define SKEW 6

/* memory alignment */
#define SIZE_SIZE_T (sizeof(size_t))
#define HMALLOC_ALIGNMENT (2 * SIZE_SIZE_T)

/* zeroing memory */
#define MALLOC_ZERO(dest, nbytes)       memset(dest, (unsigned char)0, nbytes)
/* copying memory */
#define MALLOC_COPY(dest, src, nbytes)       memcpy(dest, src, nbytes)

#define set_bloc(b,s,ps,h,pv,nx) {\
	b->size = s; \
	b->prev_free_size = ps; \
	b->heap = h; \
	b->prev = pv; \
	b->next = nx; \
	b->data = NULL; \
	b->stat_size = 0; \
}

/* manage bitmap */
#define BITS_IN_BYTES 8
#define WORD_SIZE	(8*sizeof(unsigned long))
#define BITS_IN_WORDS (BITS_IN_BYTES*WORD_SIZE)
#define HEAP_BINMAP_SIZE 1048576
#define HEAP_BINMAP_MAX_PAGES (BITS_IN_WORDS*HEAP_BINMAP_SIZE)

/* Mark/Clear bits with given index */
#define markbit(M,i)       (M[i / BITS_IN_WORDS] |= 1 << (i % BITS_IN_WORDS))
#define clearbit(M,i)      (M[i / BITS_IN_WORDS] &= ~(1 << (i % BITS_IN_WORDS)))
#define bit_is_marked(M,i) (M[i / BITS_IN_WORDS] & (1 << (i % BITS_IN_WORDS)))


/* weights of pages */
enum pinfo_weight {
	HIGH_WEIGHT,
	MEDIUM_WEIGHT,
	LOW_WEIGHT
};



typedef unsigned int binmap_t;

/* heap and used block structures */
/* used memory bloc definition inside heap*/
struct ub {
	size_t size;
	size_t prev_free_size;
	void *data;
	size_t stat_size;
	struct heap *heap;
	struct ub *prev;
	struct ub *next;
};

/* heap definition */
struct heap {
	struct ub *used;
	pthread_mutex_t lock_heap; 
	struct heap *next_heap;
	struct heap *next_same_heap;
	size_t free_size;
	size_t used_size;
	size_t touch_size;
	size_t attached_size;
	int nb_attach;
	void* iterator;
	int iterator_num;
	int alloc_policy;
	int mempolicy;
	int weight;
	unsigned long nodemask[HEAP_NBMAXNODES];
	unsigned long maxnode;
	binmap_t bitmap[1]; /* avoid to be set as NULL, need to be the last field of the struct */
};

/* malloc statistics */
struct malloc_stats {
	size_t free_size;
	size_t used_size;
	size_t touch_size;
	size_t attached_size;
	int npinfo;
};

typedef struct ub ha_ub_t;
typedef struct heap ha_heap_t;


/* heap definition */
struct central_heap {
  ha_heap_t **node_heaps;
};

typedef struct central_heap ha_central_heap_t;
typedef struct malloc_stats ha_amalloc_stat_t;
pthread_mutex_t mai_mutex;


/* Manage node mask */
static inline int mask_equal(unsigned long *a, unsigned long sa, unsigned long *b, unsigned long sb) {
        int i;
	if (sa != sb) return 0;
        for (i = 0; i < sa/WORD_SIZE; i++)
                if (a[i] != b[i])
                        return 0;
        return 1;
}

static inline int mask_isset(unsigned long *a, unsigned long sa, int node) {
	if ((unsigned int)node >= sa*8) return 0; 
	return bit_is_marked(a,node);
}

static inline void mask_zero(unsigned long *a, unsigned long sa) {
        memset(a, 0, sa);
}

static inline void mask_set(unsigned long *a, int node) {
	markbit(a,node);
}

static inline void mask_clr(unsigned long *a, int node) {
	clearbit(a,node);
}

/* function prototypes */
size_t ha_memalign(size_t mem);

ha_heap_t *ha_acreatenuma(size_t size, int flag_heap, int policy, int weight, unsigned long *nodemask, unsigned long maxnode);
ha_heap_t *ha_acreate(size_t size, int flag_heap);
void  ha_adelete(ha_heap_t **heap);
int ha_is_empty_heap(ha_heap_t* heap);
int ha_amaparea(ha_heap_t* heap, int mempolicy, int weight, unsigned long *nodemask, unsigned long maxnode);

//static inline unsigned long ha_at_cmpchg(volatile void *ptr, unsigned long old, unsigned long new, int size);
void ha_aconcat_global_list(ha_heap_t *hsrc, ha_heap_t *htgt);
void ha_aconcat_local_list(ha_heap_t *hsrc, ha_heap_t *htgt);
ha_heap_t* ha_aget_heap_from_list(int mempolicy, int weight, unsigned long *nodemask, unsigned long maxnode, ha_heap_t* heap);


void *ha_apagealloc(int nb_pages, ha_heap_t *heap);
void *ha_amalloc(size_t size, ha_heap_t* heap);
void *ha_arealloc(void *ptr, size_t size, ha_heap_t* heap);
void *ha_acalloc(size_t nmemb, size_t size, ha_heap_t* heap);

/*Initializes all type of heaps*/
void mai_init(int mempol);

/*Allocates and place in the same node of the thread*/
void *mai_malloc(size_t size);
void *mai_calloc(size_t nmemb, size_t size);
void *mai_realloc(void *ptr, size_t size);

void  mai_free(void* ptr);

void  ha_afree_heap(void* ptr, ha_heap_t* heap);
void  ha_apagefree(void* ptr, int nb_pages, ha_heap_t *heap);

ha_amalloc_stat_t  ha_amallinfo(ha_heap_t* heap);

void ha_print_list(const char* str, ha_heap_t* heap);
void ha_print_heap(struct ub* root);


/* debug functions */

#ifdef HEAP_DEBUG

#define DEBUG_LIST(str,root) \
 		ha_print_list(str,root)
#else
#define	DEBUG_LIST(...) 
#endif

#ifdef HEAP_DEBUG
#define	DEBUG_PRINT(...) \
      fprintf(stderr,__VA_ARGS__)
#else
#define	DEBUG_PRINT(...) 
#endif

#endif
