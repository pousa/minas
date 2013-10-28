/*
 * src/memon.c
 *
 * Author: Martinasso Maxime
 *
 * (C) Copyright 2007 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 * 
 */

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
#include<pthread.h>
#include<numaif.h>
#include<sys/syscall.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/file.h>
#include<signal.h>

#include "memdata.h"
#include "memon.h"

void signal_handler(int sig) {
	working=0;
	finalize_memon();
	if (sig == SIGINT) {
		exit(0);
	}
}

void finalize_memon() {
	close(hw_fifo);
	if (working) working = 0;
}

void init_memon() {
	int pid,i;
      	pthread_attr_t attr;
	struct sigaction sig_action;

	if (!working) {
		working=1;
		/* init monitoring thread */
		pid = getpid();
		if (pthread_mutex_init(&idx_mutex, NULL)) {
			printf ("Error initializing monitoring mutex\n");
		}
          	pthread_attr_init (&attr);
		if (pthread_create(&thread_mon, &attr, (void*)monitor, (void*)pid)) {
			perror ("Error creating monitoring thread: ");
		}
		/* handling signals */
		sig_action.sa_handler = signal_handler;
		sig_action.sa_flags = SA_RESTART;
		sigaction (SIGINT, &sig_action, NULL);
		sigaction (SIGPIPE, &sig_action, NULL);
	}
	for (i = 0; i < MAX_ADDR_PROCESS; ++i) {
		addr[i].ptr = NULL;
	}
}

void attach_memory(int pid, char *name, void *ptr, size_t size) {
	int i = 0;
	
	if (size > 0) {
		pthread_mutex_lock(&idx_mutex);

		while(addr[i].ptr != NULL && i < MAX_ADDR_PROCESS) i++;

		if (i < MAX_ADDR_PROCESS) {
			strcpy(addr[i].name,name);
			addr[i].pid = pid;
			addr[i].ptr = ptr;
			addr[i].size = size;
			idx_addr++;
		}

		pthread_mutex_unlock(&idx_mutex);
	}
}

void detach_memory(int pid, char *name, void *ptr) {
	int i;
	memdata_t *pdata;
	void *data;
	int *size_data;
	
	pthread_mutex_lock(&idx_mutex);
	for (i = 0; i < MAX_ADDR_PROCESS; ++i) {
		if (addr[i].ptr == ptr && strcmp(addr[i].name,name) == 0 && addr[i].pid == pid) {
			break;
		}
	}
	if (i < MAX_ADDR_PROCESS) {
		addr[i].ptr = NULL;
		idx_addr--;
	}
	pthread_mutex_unlock(&idx_mutex);
}

void* monitor(void* p) {
	int i, j, count;
	int pid = (int)p;
	unsigned long len = 0;
	struct stat stat_buf;
	void *data;
	int *size;
	memdata_t *pdata;
	char msg[255];

	sprintf(msg,"%s%d",FIFO_PATH_NAME,getuid());
	if (stat(msg, &stat_buf) == 0) {
		if ((hw_fifo = open(msg,O_WRONLY|O_NONBLOCK)) == -1) {
			perror("open");
		}
	} else {
		perror("stat");
		if(mkfifo(msg, 0666) == -1) {
			perror ("Error creating fifo pipe:");
		} else {
			if ((hw_fifo = open(msg,O_WRONLY|O_NONBLOCK)) == -1) {
				perror("open");
			}
		}
	}

	while(working) {
		/* update pages of addr */
		pthread_mutex_lock(&idx_mutex);
		if (idx_addr > 0) {
			count = 0;
			data = malloc(sizeof(int)+idx_addr*sizeof(memdata_t));
			size = (int*)data;
			*size = idx_addr;
			pdata = (memdata_t*)(data+sizeof(int));
			j = 0;
			for (i = 0; i < MAX_ADDR_PROCESS && count < idx_addr; ++i) {
				if (addr[i].ptr != NULL) {
					pdata[j].pid = addr[i].pid;
					strcpy(pdata[j].name, addr[i].name);
					touched_pages(pdata[j].pages_node, addr[i].ptr, addr[i].size);
					pdata[j].size = addr[i].size;
					count++;
					j++;
				}
			}
			if (count > 0) {
				/* send data through a pipe */
				flock(hw_fifo,LOCK_EX);
				len = write(hw_fifo,data,sizeof(int)+idx_addr*sizeof(memdata_t));
				flock(hw_fifo,LOCK_UN);
			}
			free(data);
		}
		pthread_mutex_unlock(&idx_mutex);

		/* sleep */
		sleep(SLEEP_MONITOR);
	}
}

void touched_pages(int* nodes, void* ptr, size_t size) {
	int i, ret, pagesize, nb_pages, node;
	void **addr;
	int *node_pages;
        #ifdef __NR_move_pages
	pagesize = getpagesize();
	nb_pages = size/pagesize;
	addr = malloc(nb_pages*sizeof(void*));
	node_pages = malloc(nb_pages*sizeof(int));

	for(i = 0; i < MAX_NUMA_NODE; ++i) {
		nodes[i]=0;
	}

	for(i = 0; i < nb_pages; ++i) {
		addr[i] = (void*)(ptr+i*pagesize);
		node_pages[i] = -1;
	}

	ret = syscall(__NR_move_pages, 0, nb_pages, addr, NULL, node_pages, MPOL_MF_MOVE);
	if (ret != 0) {
		if (errno == ENOSYS)
			fprintf(stderr,"upgrade to kernel >= 2.6.18\n");
		else
			fprintf(stderr,"errno syscall = %d\n", errno);
	}
	
	for(i = 0; i < nb_pages; ++i) {
		nodes[node_pages[i]]++;
	}
	free(addr);
	free(node_pages);
        #else
        printf("upgrade to kernel >= 2.6.18\n");
        #endif
}

