/*
 * include/heap_alloc_numa.h
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

#ifndef __HEAP_NUMA_ALLOC_H
#define __HEAP_NUMA_ALLOC_H

#include <stddef.h>
#include <numa.h>
#include <numaif.h>
#include <sys/mman.h>

#include "heap_alloc.h"



/* structure used to describe the mapping of a set of pages over numa bloc */
struct pageinfo {
	int mempolicy;				/* mapping policy */
	int weight;				/* pages weights */
	unsigned long *nodemask;		/* numa nodes mask */
	unsigned long maxnode;			/* number of bits of mask */
	int nb_touched[HEAP_NBMAXNODES];	/* distribution of pages over numa nodes */
};

typedef struct pageinfo ha_pinfo_t;

/* function definitions */

/* Numa mapping */
int ma_maparea(void *ptr, size_t size, int mempolicy, unsigned long *nodemask, unsigned long maxnode);
long long ma_free_mem_node(int node);
long long ma_hits_mem_node(int node);

/* Memory allocators */
void *ha_hmalloc(size_t size, int mempolicy, int weight, unsigned long *nodemask, unsigned long maxnode,  ha_heap_t *heap);
void *ha_hrealloc(void *ptr, size_t size);
void *ha_hcalloc(size_t nmemb, size_t size, int policy, int weight, unsigned long *nodemask, unsigned long maxnode,  ha_heap_t *heap);
void ha_hfree(void *ptr);

/* Attach and detach of memory area to heap */
void ha_hattach_memory(void *ptr, size_t size, int mempolicy, int weight, unsigned long *nodemask, unsigned long maxnode, ha_heap_t *heap);
void ha_hdetach_memory(void *ptr, ha_heap_t *heap);

/* Memory migration */
int ha_hmove_memory(ha_pinfo_t *ppinfo, int mempolicy, int weight, unsigned long *nodemask, unsigned long maxnode,  ha_heap_t *heap);

/* Get pages information */
void ha_hget_pinfo(void *ptr, ha_pinfo_t* ppinfo, ha_heap_t *heap);

/* Get all pages information from heap */
int ha_hnext_pinfo(ha_pinfo_t **ppinfo, ha_heap_t *heap);

/* Check pages location */
#ifdef __NR_move_pages

void ha_hupdate_memory_nodes(ha_pinfo_t *ppinfo, ha_heap_t *heap);

#endif


/* Heap creation and destruction */
ha_heap_t* ha_hcreate_heap(void);
void ha_hdelete_heap(ha_heap_t *heap);

/* merge two heaps */
void ha_hfusion_heap(ha_heap_t *hacc, ha_heap_t *h);

#endif
