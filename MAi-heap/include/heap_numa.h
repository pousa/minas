/*
 * include/heap_numa.h
 *
 * Reservation of memory adresses 
 * and NUMA binding of physical pages
 *
 * Authors: Martinasso Maxime
 *          Christiane Pousa Ribeiro
 *
 * (C) Copyright 2010 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 * 
 */
#ifndef __HEAP_NUMA_H
#define __HEAP_NUMA_H

#include <signal.h>

/* --- function prototypes --- */
int ha_maparea(void *ptr, size_t size, int mempolicy, unsigned long *nodemask, unsigned long maxnode);
long long ha_free_mem_node(int node);
long long ha_hits_mem_node(int node);

/* --- MAi memory policies --- */
int mai_cyclic(void *ptr, size_t size, int *nodemask, int nnodes);
int mai_cyclic_block(void *ptr,size_t size, int *nodemask, int nnodes);
int mai_bind(void *ptr, size_t size, int node);
int mai_bind_block(void *ptr, size_t size,int *nodemask, int nnodes);
int mai_prime(void *ptr, size_t size, unsigned long *nodemask, int nnodes, int prime);
int mai_prime_block(void *ptr, size_t size, int *nodemask, int nnodes, int prime);
int mai_skew(void *ptr, size_t size, int *nodemask, int nnodes);
int mai_skew_block(void *ptr, size_t size, int *nodemask, int nnodes);
int mai_random(void *ptr, size_t size, int *nodemask, int nnodes);
int mai_random_block(void *ptr, size_t size, int *nodemask, int nnodes);

#endif
