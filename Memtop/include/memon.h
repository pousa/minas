/*
 * include/memon.h
 *
 * Author: Martinasso Maxime
 *
 * (C) Copyright 2007 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 * 
 */

#ifndef __MEMON_H
#define __MEMON_H

//#include<stdio.h>
#include<pthread.h>

#include "memdata.h" 

#define SLEEP_MONITOR 2

typedef struct memaddr {
	int pid;
	void* ptr;
	size_t size;
	char name[MAX_ADDR_NAME];
} memaddr_t;

static int working = 0;
static int hw_fifo;
static pthread_t thread_mon;
static pthread_mutex_t idx_mutex;

static memaddr_t addr[MAX_ADDR_PROCESS];
static int idx_addr = 0;
static int id_count = 0;

/* user interface */
void init_memon();
void finalize_memon();
void attach_memory(int pid, char *name, void *ptr, size_t size);
void detach_memory(int pid, char *name, void *ptr);

/* monirtoring */
void* monitor(void* p);
void touched_pages(int* nodes, void* ptr, size_t size);
void signal_handler(int sig);

#endif
