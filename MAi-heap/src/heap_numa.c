/*
 * src/heap_numa.c
 *
 * Reservation of memory adresses 
 * and NUMA binding of physical pages
 *
 * Authors: Martinasso Maximevoid 
 *          Christiane Pousa Ribeiro
 *
 * (C) Copyright 2010 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 *
 */

#include <errno.h>
#include <stdio.h>
#include <numaif.h>
#include <numa.h>
#include <stddef.h>
#include <sys/mman.h>
#include <ctype.h>
#include <utmpx.h>
#include <sys/syscall.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <pthread.h>

#include "heap_numa.h"
#include "heap_alloc_numa.h"
#include "heap_alloc.h"

/* --- ha_maparea ---
 * This function bind the physical pages following the distribution
 * passed in arguments.
 * The binding can be done in two ways:
 *      - CYCLIC : memory is mapped by bloc following a cyclic placement
 *      - CYCLIC_NEIGHBORS : memory is mapped by bloc following a cyclic placement using neighbos memory banks
 *      - NEXT_TOUCH : memory is mapped following the memory access patterns
 *      - LESS_LOADED : map to the node with the most free memory from nodemask
 *      - SMALL_ACCESSED : map to the node with the lowest number of hits from nodemask
 *	- BIND : memory is mapped by bloc following a bind placement
 *	- PRIME : memory is mapped by bloc following a prime mapp placement
 *      - SKEW : memory is mapped by bloc following a skew mapp placement 
 * Note that for each mapping the size of bloc are aligned to page size.
 * If flags is not correctly set, by default CYCLIC is used.
 */

int ha_maparea(void *ptr, size_t size, int mempolicy, unsigned long *nodemask, unsigned long maxnode) {
        int err, i, idx_node, max_nodes;
        long long node_free_size, max_free_size = 0;
        long long node_hits, min_hits = LONG_MAX;
        unsigned long newnodemask[HEAP_NBMAXNODES];
        unsigned long newmaxnode;
        void *t;
	int *node_neighbors;
	int nneighbors,great_int;

	int mem_node,npagi,pm_no,prime,find=0;
  	unsigned long node,nodes=0;
  	unsigned long block,np,npag=NPAG;

	max_nodes = numa_max_node() + 1;

        DEBUG_PRINT("ha_maparea size %d, start address %p mask=%ld ", (int)size, ptr,*nodemask);
		  switch(mempolicy) {
			  case SMALL_ACCESSED:
				  for(i = 0; i < max_nodes; ++i) {
					  if (mask_isset(nodemask,maxnode,i)) {
						  node_hits = ha_hits_mem_node(i);
						  if (min_hits > node_hits) {
							  min_hits = node_hits;
							  idx_node = i;
						  }
					  }
				  }
				  newmaxnode = maxnode;
				  mask_zero(newnodemask,newmaxnode);
				  mask_set(newnodemask,idx_node);
				  err = mbind (ptr, size, MPOL_BIND , newnodemask, newmaxnode, MPOL_MF_MOVE);
				  break;
			  case LESS_LOADED:
				  for(i = 0; i < max_nodes; ++i) {
					  if (mask_isset(nodemask,maxnode,i)) {
						  node_free_size = ha_free_mem_node(i);
						  if (node_free_size > max_free_size) {
							  max_free_size = node_free_size;
							  idx_node = i;
						  }
					  }
				  }
				  newmaxnode = maxnode;
				  mask_zero(newnodemask,newmaxnode);
				  mask_set(newnodemask,idx_node);
				  err = mbind (ptr, size, MPOL_BIND , newnodemask, newmaxnode, MPOL_MF_MOVE);
				  break;
			  case CYCLIC:
				 err = mbind (ptr, size, MPOL_INTERLEAVE , nodemask, maxnode, MPOL_MF_MOVE);
				  if (err) {
				  	perror("mbind error:");
				  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_INTERLEAVE|MPOL_MF_MOVE, nodemask, maxnode);
				  } else {
					  DEBUG_PRINT("mbind returns %d : Success\n", err);
				  }
				  break;
				  break;
			  case CYCLICN:
			          for(i = 0; i < max_nodes && !find; ++i) {
					  if (mask_isset(nodemask,maxnode,i)) 
					    { find=1;  idx_node = i;}	  
				  }
				  newmaxnode = maxnode;
				  mask_zero(newnodemask,newmaxnode);
				  nneighbors = na_get_maxneighbors();
				  node_neighbors = na_get_nneighbors(idx_node);
				  for(i = 0; i < nneighbors; i++)
				     mask_set(newnodemask,node_neighbors[i]);
				 err = mbind (ptr, size, MPOL_INTERLEAVE , newnodemask, maxnode, MPOL_MF_MOVE);
				  if (err) {
				  	perror("mbind error:");
				  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_INTERLEAVE|MPOL_MF_MOVE, nodemask, maxnode);
				  } else {
					  DEBUG_PRINT("mbind returns %d : Success\n", err);
				  }
				  break;
			  case BIND:
				  err = mbind (ptr, size, MPOL_BIND , nodemask, maxnode, MPOL_MF_MOVE);
				  if (err) {
				  	perror("mbind error:");
				  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, nodemask, maxnode);
				  } else {
					  DEBUG_PRINT("mbind returns %d : Success\n", err);
				  }
  				  break;
		          case SKEW:
				    t = ptr;
  				    block = (size/PAGE_SIZE)/npag;

				for(i=0;i<block;i++)
 				{
				        great_int = i/maxnode;
				        mem_node = (i+great_int+1)%maxnode;
  					newmaxnode = maxnode;
  					mask_zero(newnodemask,newmaxnode);
				        mask_set(newnodemask,mem_node);
				        err = mbind (t, PAGE_SIZE*npag, MPOL_BIND , newnodemask, MASK_SIZE,MPOL_MF_MOVE);
        			        t = (npag*PAGE_SIZE)+ t;
 				   }
				  if (err) {
				  	perror("mbind error:");
				  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, nodemask, maxnode);
				  } else {
					  DEBUG_PRINT("mbind returns %d : Success\n", err);
				  }
  				
  				  break;
			  case PRIME:
				    t = ptr;
  				    block = (size/PAGE_SIZE)/npag;
  				    prime = 11;
 
				for(i=0;i<block;i++)
 				{
        				npagi = i*npag;
        				if((npagi%prime) > max_nodes)
        				{
         					pm_no = npagi % prime;
         					while(pm_no > max_nodes)
         					{
           						pm_no = (npagi/prime)*(prime-max_nodes)+((npagi%prime)-max_nodes);
           						npagi = pm_no/prime;
           						pm_no = pm_no % prime;
         					}
         					mem_node = pm_no;
        				}
        				else
         					mem_node = npagi % prime;
  					newmaxnode = maxnode;
  					mask_zero(newnodemask,newmaxnode);
				        mask_set(newnodemask,mem_node);
				        err = mbind (t, PAGE_SIZE*npag, MPOL_BIND , newnodemask, MASK_SIZE,MPOL_MF_MOVE);
        			        t = (npag*PAGE_SIZE)+ t;
 				   }
				  if (err) {
				  	perror("mbind error:");
				  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, nodemask, maxnode);
				  } else {
					  DEBUG_PRINT("mbind returns %d : Success\n", err);
				  }
  				  break;
			  default:
				  err = mbind (ptr, size, MPOL_INTERLEAVE , nodemask, maxnode, MPOL_MF_MOVE);
				  if (err) {
				  	perror("mbind error:");
				  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_INTERLEAVE|MPOL_MF_MOVE, nodemask, maxnode);
				  } else {
					  DEBUG_PRINT("mbind returns %d : Success\n", err);
				  }
				  break;
        }
        return err;
}

long long ha_free_mem_node(int node) {
        char fn[64];
        long long size = -1;
        size_t len = 0;
        char *line = NULL;
	char *s, *end;
        FILE *f;

        sprintf(fn,"/sys/devices/system/node/node%d/meminfo", node);
        f = fopen(fn, "r");
        if (!f)
                return -1;
        while (getdelim(&line, &len, '\n', f) > 0) {
                s = strcasestr(line, "kB");
                if (!s)
                        continue;
                --s;
                while (s > line && isspace(*s))
                        --s;
                while (s > line && isdigit(*s))
                        --s;
                if (strstr(line, "MemFree")) {
                        size = strtoull(s,&end,0) << 10;
                        if (end == s) size= -1;
                }
        }
        fclose(f);
        free(line);
        return size;
}

long long ha_hits_mem_node(int node) {
        char fn[64];
        long long hits = -1;
        size_t len = 0;
        char *line = NULL;
        FILE *f;

        sprintf(fn,"/sys/devices/system/node/node%d/numastat", node);
        f = fopen(fn, "r");
        if (!f)
                return -1;
        while (getdelim(&line, &len, '\n', f) > 0) {
                if (strstr(line, "numa_hit")) {
                        hits = strtoull(line+8,NULL,0);
                        break;
                }
        }
        fclose(f);
        free(line);
        return hits;
}

int mai_cyclic(void *ptr, size_t size, int *nodemask, int nnodes)
{
        int err,i;
        unsigned long newnode;

        size = ha_memalign(size);

	for(i = 0; i < nnodes; ++i) 
	 newnode = 1 << nodemask[i];

	err = mbind (ptr, size, MPOL_INTERLEAVE , &newnode, MASK_SIZE, MPOL_MF_MOVE);
	 if (err) {
	  	perror("mbind error:");
	  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_INTERLEAVE|MPOL_MF_MOVE, nodemask, MASK_SIZE);
	  } else {
	         DEBUG_PRINT("mbind returns %d : Success\n", err);
		 }
        return err;
}

int mai_cyclic_block(void *ptr,size_t block, int *nodemask, int nnodes)
{
        int err,num_pages=0,i;
        unsigned long newnode;
        void *t;
          
        t=ptr;

 	block = ha_memalign(block);

        num_pages = (block/PAGE_SIZE)/nnodes;	   
 
      for(i=0;i<nnodes;i++){
        newnode = 1 << (nodemask[i%nnodes]);
	err = mbind (t, (num_pages*PAGE_SIZE), MPOL_BIND , &newnode, MASK_SIZE, MPOL_MF_MOVE);
	 if (err) {
	  	perror("mbind error:");
	  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, nodemask, MASK_SIZE);
	  } else {
	         DEBUG_PRINT("mbind returns %d : Success\n", err);
		 }

        t = t + (num_pages*PAGE_SIZE);	
      }
      return err;
}


int mai_bind(void *ptr, size_t size, int node)
{
        int err,i;
        unsigned long newnode;
        void *t;
          
	newnode = 1 << node;
	size = ha_memalign(size);
	
        err = mbind (ptr, size, MPOL_BIND , &newnode, MASK_SIZE, MPOL_MF_MOVE);
	 if (err) {
	  	perror("mbind error:");
	  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, nodemask, MASK_SIZE);
	  } else {
	         DEBUG_PRINT("mbind returns %d : Success\n", err);
		 }
         return err;
}

int mai_bind_block(void *ptr, size_t block, int *nodemask,int nnodes)
{
        int err,num_pages,i;
        unsigned long newnode;
        void *t;
          
        t=ptr;
	block = ha_memalign(block);
        num_pages = (block / PAGE_SIZE)/nnodes;

        for(i=0;i<nnodes;i++)
	{
		newnode = 1 << nodemask[i];
        	err = mbind (t, num_pages*PAGE_SIZE, MPOL_BIND , &newnode, MASK_SIZE, MPOL_MF_MOVE);
	 	if (err) {
	  		perror("mbind error:");
	 	 	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, nodemask, MASK_SIZE);
	  	} else {
	        	 DEBUG_PRINT("mbind returns %d : Success\n", err); }
		t = t + (num_pages*PAGE_SIZE);
	}
        return err;
}

int mai_prime(void *ptr, size_t size, unsigned long *nodemask, int nnodes, int prime)
{
        int err,num_pages=0,i;
        void *t;
           
        unsigned long newnode[HEAP_NBMAXNODES];
	int mem_node,npagi,pm_no;

        t=ptr;

 	size_t block = (get_totalcache()*1024)/PAGE_SIZE;
        num_pages = size/PAGE_SIZE;	 
        int work = num_pages / block; 	   
 
       for(i=0;i<work;i++){

	npagi = i*num_pages;
 	if((num_pages%prime) > nnodes)
        {
         	pm_no = npagi % prime;
         	while(pm_no > nnodes)
         	{
           	  pm_no = (npagi/prime)*(prime-nnodes)+((npagi%prime)-nnodes);
           	  npagi = pm_no/prime;
           	  pm_no = pm_no % prime;
         	}
         	mem_node = pm_no;
        }
        else
          mem_node = npagi % prime;

  	  mask_zero(newnode, MASK_SIZE);
	  mask_set(newnode,nodemask[mem_node]);
	  err = mbind (t, PAGE_SIZE*block, MPOL_BIND , newnode, MASK_SIZE,MPOL_MF_MOVE);

	 if (err) {
	  	perror("mbind error:");
	  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, nodemask, MASK_SIZE);
	  } else {
	         DEBUG_PRINT("mbind returns %d : Success\n", err);
		 }

        t = t + (block*PAGE_SIZE);	
      }
       return err;
}


int mai_prime_block(void *ptr, size_t block, int *nodemask, int nnodes, int prime)
{
        int err,num_pages=0,i;
        void *t;
           
        unsigned long newnode[HEAP_NBMAXNODES];
	int mem_node,npagi,pm_no;

        t=ptr;

 	block = ha_memalign(block);

        num_pages = block/PAGE_SIZE;	   
        block = num_pages/nnodes;

       for(i=0;i<nnodes;i++){

	npagi = i*num_pages;
 	if((num_pages%prime) > nnodes)
        {
         	pm_no = npagi % prime;
         	while(pm_no > nnodes)
         	{
           	  pm_no = (npagi/prime)*(prime-nnodes)+((npagi%prime)-nnodes);
           	  npagi = pm_no/prime;
           	  pm_no = pm_no % prime;
         	}
         	mem_node = pm_no;
        }
        else
          mem_node = npagi % prime;

  	  mask_zero(newnode, MASK_SIZE);
	  mask_set(newnode,nodemask[mem_node]);
	  err = mbind (t, PAGE_SIZE*block, MPOL_BIND , newnode, MASK_SIZE,MPOL_MF_MOVE);

	 if (err) {
	  	perror("mbind error:");
	  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, nodemask, MASK_SIZE);
	  } else {
	         DEBUG_PRINT("mbind returns %d : Success\n", err);
		 }

        t = t + (block*PAGE_SIZE);	
      }
       return err;
}


int mai_skew(void *ptr, size_t size, int *nodemask, int nnodes)
{
 int err,num_pages=0,great_int,i;
        void *t;
           
        unsigned long newnode[HEAP_NBMAXNODES];
	int mem_node,npagi,pm_no;

        t=ptr;

 	size_t block = (get_totalcache()*1024)/PAGE_SIZE;
        num_pages = size/PAGE_SIZE;	 
        int work = num_pages / block;  	   
 
       for(i=0;i<work;i++){

	  great_int = i/nnodes;
  	  mask_zero(newnode, MASK_SIZE);
	  mask_set(newnode,nodemask[(i+great_int+1)%(nnodes-1)]);
	  err = mbind (t, PAGE_SIZE*block, MPOL_BIND , newnode, MASK_SIZE,MPOL_MF_MOVE);

	 if (err) {
	  	perror("mbind error:");
	  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, nodemask, MASK_SIZE);
	  } else {
	         DEBUG_PRINT("mbind returns %d : Success\n", err);
		 }

        t = t + (block*PAGE_SIZE);	
      }
       return err;
}

int mai_skew_block(void *ptr, size_t size, int *nodemask, int nnodes)
{
  int err,num_pages=0,great_int,i;
        void *t;
           
        unsigned long newnode[HEAP_NBMAXNODES];
	int mem_node,npagi,pm_no;

        t=ptr;

 	size = ha_memalign(size);

        num_pages = size/PAGE_SIZE;	   
        int block = num_pages/nnodes;
       for(i=0;i<nnodes;i++){

	  great_int = i/nnodes;
  	  mask_zero(newnode, MASK_SIZE);
	  mask_set(newnode,nodemask[(i+great_int+1)%(nnodes-1)]);
	  err = mbind (t, PAGE_SIZE*block, MPOL_BIND , newnode, MASK_SIZE,MPOL_MF_MOVE);

	 if (err) {
	  	perror("mbind error:");
	  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, nodemask, MASK_SIZE);
	  } else {
	         DEBUG_PRINT("mbind returns %d : Success\n", err);
		 }

        t = t + (block*PAGE_SIZE);	
      }
       return err;
}

int mai_random(void *ptr, size_t size, int *nodemask, int nnodes)
{
        int err,num_pages=0,great_int,i;
        void *t;  
        unsigned long newnode[HEAP_NBMAXNODES];
	int mem_node,npagi,pm_no;
        size_t block;

 	srand ( time(NULL) );
        t=ptr;

 	block = (get_totalcache()*1024)/PAGE_SIZE;
        num_pages = size/PAGE_SIZE;	 
        int work = num_pages / block;  
 
       for(i=0;i<work;i++){

  	  mask_zero(newnode, MASK_SIZE);
	  mask_set(newnode,nodemask[rand() % (nnodes-1)]);
	  err = mbind (t, PAGE_SIZE*block, MPOL_BIND , newnode, MASK_SIZE,MPOL_MF_MOVE);

	 if (err) {
	  	perror("mbind error:");
	  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, nodemask, MASK_SIZE);
	  } else {
	         DEBUG_PRINT("mbind returns %d : Success\n", err);
		 }

        t = t + (block*PAGE_SIZE);	
      }

   return err;
}

int mai_random_block(void *ptr, size_t size, int *nodemask, int nnodes)
{
        int err,num_pages=0,great_int,i;
        void *t;  
        unsigned long newnode[HEAP_NBMAXNODES];
	int mem_node,npagi,pm_no;

 	srand ( time(NULL) );
        t=ptr;

 	size = ha_memalign(size);

        num_pages = size/PAGE_SIZE;
        int block  = num_pages/nnodes;	   
 
       for(i=0;i<nnodes;i++){

  	  mask_zero(newnode, MASK_SIZE);
	  mask_set(newnode,nodemask[rand() % (nnodes-1)]);
	  err = mbind (t, PAGE_SIZE*block, MPOL_BIND , newnode, MASK_SIZE,MPOL_MF_MOVE);

	 if (err) {
	  	perror("mbind error:");
	  	DEBUG_PRINT("mbind args %p %ld %d %d %d 0\n",ptr, size, MPOL_BIND|MPOL_MF_MOVE, newnode, MASK_SIZE);
	  } else {
	         DEBUG_PRINT("mbind returns %d : Success\n", err);
		 }

        t = t + (block*PAGE_SIZE);	
      }
}



