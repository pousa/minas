/*
 * include/memdata.h
 *
 * Author: Martinasso Maxime
 *
 * (C) Copyright 2007 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 * 
 */

#ifndef __MEMDATA_H
#define __MEMDATA_H


/*
 * Becareful that the size of memdata * number of variable < 64Ko
 * due to limitiation on write to the pipe 
 */

#define MAX_NUMA_NODE 128
#define MAX_ADDR_PROCESS 96
#define MAX_ADDR_NAME 16

static char FIFO_PATH_NAME[] = "/tmp/memtop_fifo_name_";

typedef struct memdata {
	int pid;
	char name[MAX_ADDR_NAME];
	int pages_node[MAX_NUMA_NODE];
	size_t size;
} memdata_t;

#endif
