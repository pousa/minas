/*
 * include/heap_alloc.h
 *
 * Definition of memory allocator inside a predefined heap
 * working for  NUMA architecture
 * and multithread environements
 *
 * Authors: Martinasso Maximevoid 
 *          Christiane Pousa Ribeiro
 *
 * (C) Copyright 2010 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 * 
 */

#include <errno.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <numaif.h>
#include <numa.h>
#include <utmpx.h>
#include <pthread.h>
#include <sys/time.h>
#include <dirent.h>
#include <ctype.h>
#include <limits.h>
#include<sched.h>

#include "heap_alloc.h"
#include "heap_atomic.h"
#include "numArch.h"
#include "archTopo.h"

static ha_central_heap_t h_local;

#ifdef HEAP_DEBUG
/* --- print_list ---
 ** Display the list of heap starting from (ha_heap_t*)heap
 */
void ha_print_list(const char* str, ha_heap_t* heap) {
	ha_heap_t* h;
	int count = 0;
  	unsigned int i, max_node = numa_max_node() - 1;

	printf("%s",str);
	h = heap;
	while (IS_HEAP(h)) {
		pthread_mutex_lock(&h->lock_heap);
		printf("heap #%d %p (it=%d) (%d,%d) numa=",count,h,h->iterator_num,h->mempolicy,h->weight);
		for(i = 0; i < h->maxnode/WORD_SIZE; i++) {
			printf("%ld|",h->nodemask[i]);
		}
		for(i = 0; i < max_node+1; ++i) {
			if(mask_isset(h->nodemask,h->maxnode,i)) {
				printf("%d-",i);
			}
		}
		printf(" : next_same=%p|",h->next_same_heap);
		printf(" : next=%p|",h->next_heap);
		printf(" : %d : ",(int)HEAP_GET_SIZE(h));
		ha_print_heap(h->used);
		count++;
		pthread_mutex_unlock(&h->lock_heap);
		h = h->next_heap;
	}
}
/* --- print heap ---
 * Display the list of bloc starting from (ha_ub_t*)root
 */
void ha_print_heap(struct ub* root) {
	int count = 0;
	while(root != NULL) {
		printf("[%d| %p (u:%zu,f:%zu,s:%zu) (p:%p,n:%p)",count, root, root->size, root->prev_free_size, root->stat_size, root->prev, root->next);
		if (root->data != NULL) {
			printf("S:%p",root->data);
		} 
		printf("]");
		root = root->next;
		count++;
	}
	if (count != 0) {
		printf("\n");
	} else {
		printf("Empty\n");
	}	
}
#endif /* HEAP_DEBUG */

/* --- memalign ---
 * Align (size_t)mem to a multiple of HMALLOC_ALIGNMENT
 */
size_t ha_memalign(size_t mem) {
	unsigned long nb;
	if (mem == 0) return 0;
	if (mem < HMALLOC_ALIGNMENT) {
		return HMALLOC_ALIGNMENT;
	}
	/* look if is not already a multiple of HMALLOC_ALIGNMENT */
	if (mem % HMALLOC_ALIGNMENT == 0) {
		return mem;
	}
	nb = mem/HMALLOC_ALIGNMENT;
    	return (nb+1) * HMALLOC_ALIGNMENT;
}


/* --- mai_init() ---
 * Initializes NUMA heaps on the machine nodes
 */
void mai_init(int mempol)
{
  int nodes,i,err,j;
  size_t size;
  unsigned int pagesize;
  unsigned long node;

if( na_is_numa())  {
	
  nodes = numa_max_node() + 1;
  pagesize = getpagesize () ;

 //allocating the index level - allocation classes
  h_local.node_heaps = (ha_heap_t*) malloc(nodes*sizeof(ha_heap_t*)); 

  pthread_mutex_init(&mai_mutex,NULL); 
  load_hash();  
  
  for(i=0;i<nodes;i++)
  {
        node = 1 << i;
	h_local.node_heaps[i] = ha_acreatenuma(HEAP_MINIMUM_PAGES,HEAP_DYN_ALLOC,mempol,LOW_WEIGHT,&node,MASK_SIZE);
  } 
 }
  else{
    printf("This is not a NUMA platform\n");	  	
    exit(-1); }
} 


/* --- ha_amaparea ---
 * Mbind a heap and set its numa data: mempolicy, weight and nodemask
 */
int ha_amaparea(ha_heap_t* heap, int mempolicy, int weight, unsigned long *nodemask, unsigned long maxnode) {
	int err;
	int i;
	err = ha_maparea(heap,HEAP_GET_SIZE(heap),mempolicy, nodemask, maxnode);
	if (err == 0) {
		heap->mempolicy = mempolicy;
		heap->weight = weight;
		heap-> maxnode = maxnode;
		for (i = 0; i < heap->maxnode/WORD_SIZE; ++i) {		
			heap->nodemask[i] = nodemask[i];
		}	
	}
	return err;
}


/* --- ha_acreate ---
 * Return an adress pointing to a heap of size (size_t)size.
 * This method call mmap to reserve memory.
 * In case of mmap failed or the initialization of the mutex failed the returning adress is NULL.
 * Size is aligned to pagesize.
 */
ha_heap_t* ha_acreate(size_t size, int alloc_policy) {
	ha_heap_t* heap;	
	unsigned int pagesize, i; //nb_pages;

	int err;

	pagesize = getpagesize () ;
	if (size < HEAP_MINIMUM_PAGES*pagesize) {
		size = HEAP_MINIMUM_PAGES*pagesize;
	} else {
		size = ha_memalign(size);
	}

	/* align to pagesize */
  	if ((size % pagesize) != 0) {
		size = ((size / pagesize)+1)*pagesize ;
	}
	if (alloc_policy == HEAP_PAGE_ALLOC) {
		size += pagesize;
	}

	heap = (ha_heap_t *) mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if (heap == MAP_FAILED) {
		heap = NULL;
		perror("mmap failed in acreate\n");
		DEBUG_PRINT("ha_acreate: mmap failed\n");
	} else {
		heap->mempolicy = HEAP_UNSPECIFIED_POLICY;
		heap->weight = HEAP_UNSPECIFIED_WEIGHT;
		heap->iterator = NULL;
		heap->iterator_num = HEAP_ITERATOR_ID_UNDEFINED;
		heap->nb_attach = 0;
		heap->maxnode = 0;
		heap->used = NULL;
		heap->next_heap = NULL;
		heap->next_same_heap = NULL;
		heap->attached_size = 0;
		heap->alloc_policy = alloc_policy;
		if (alloc_policy == HEAP_DYN_ALLOC) {
			heap->free_size = size - HEAP_SIZE_T;
			heap->used_size = HEAP_SIZE_T;
			heap->touch_size = HEAP_SIZE_T;
		} else {
			heap->free_size = size - pagesize;
			heap->used_size = pagesize;
			heap->touch_size = pagesize;
			for(i = 0; i < HEAP_BINMAP_SIZE; ++i) {
				clearbit(heap->bitmap,i);
			}
		}
		
		if (pthread_mutex_init(&heap->lock_heap, NULL) != 0) {
			heap = NULL;
			perror("Initilization of the mutex failed in acreate\n");
			DEBUG_PRINT("ha_acreate: mutex_init failed\n");
		}
	}
	DEBUG_PRINT("ha_acreate: heap created at %p size=%ld\n",heap,size);

	return heap;
}

/* --- acreatenuma ---
 * Return an adress pointing to a heap of size (size_t)size
 * This method is different to ha_acreate in the way that physical pages
 * are mapped to the numa bloc following the numa parameter: mempolicy, weight and nodemask
 * Size is aligned to pagesize.
 */
ha_heap_t* ha_acreatenuma(size_t size, int alloc_policy, int mempolicy, int weight, unsigned long *nodemask, unsigned long maxnode) {
	ha_heap_t *heap;

	heap = ha_acreate(size,alloc_policy);
	if (mempolicy != HEAP_UNSPECIFIED_POLICY && weight != HEAP_UNSPECIFIED_WEIGHT && maxnode != 0) {
		pthread_mutex_lock(&heap->lock_heap);
		ha_amaparea(heap, mempolicy, weight, nodemask, maxnode);
		pthread_mutex_unlock(&heap->lock_heap);
	}
	return heap;
}

/* --- ha_adelete --- 
 * This method call munmap to give back  memory adresses of each heap of the list starting at (ha_heap_t**)heap.
 * If the mutex is already lock by another thread this method has an undefined behavior (mainly the methode do nothing).
 * If the munmap call failed, the method do nothing and heap is set to NULL.
 */
void ha_adelete(ha_heap_t **heap) {
	size_t size_heap;
	int failed = 0;
	ha_heap_t* next_heap;
	ha_heap_t* next_heap_tmp;

	next_heap = *heap;
	if (IS_HEAP(next_heap)) {
		while(next_heap != NULL) {
			pthread_mutex_lock(&next_heap->lock_heap);
			size_heap = HEAP_GET_SIZE(next_heap);
			next_heap_tmp = next_heap->next_heap;
			pthread_mutex_unlock(&next_heap->lock_heap);
			if (pthread_mutex_destroy(&next_heap->lock_heap) == 0) {
				/* if mutex destroy success mainly no other thread has locked the mutex */
				if (munmap(next_heap,size_heap) != 0) {
					DEBUG_PRINT("ha_adelete: freearea error addr=%p size=%d\n",next_heap, (int)size_heap);
				}
				DEBUG_PRINT("ha_adelete: succeed\n");
			} else {
				/* may destroy an locked mutex: undefined behavior */
				DEBUG_PRINT("ha_adelete: may destroy a locked mutex: undefined behavior\n");
				failed = 1;
			}
			next_heap = next_heap_tmp;
		}
		if (!failed) *heap = NULL;
	}
}

/* --- ha_is_empty_heap ---
 * Returns true if the heap has no malloc set or memory attached
 */
int ha_is_empty_heap(ha_heap_t* heap) {
	ha_heap_t* current_heap;

	current_heap = heap;
	while(IS_HEAP(current_heap)) {
		if (current_heap->used != NULL) {
			return 0;
		}
		current_heap = current_heap->next_heap;
	}
	return 1;
}

/* --- ha_aconcat_global_list ---
 * Concat htgt at the end of hsrc
 * This method call an atomic cmp&swap method to link both lists
 */
void ha_aconcat_global_list(ha_heap_t *hsrc, ha_heap_t *htgt) {
	ha_heap_t *current_heap;
	int valid = 0;

	if (IS_HEAP(hsrc) && IS_HEAP(htgt) && hsrc != htgt) {
		current_heap = hsrc;
		while(!valid) {
			while(IS_HEAP(current_heap->next_heap)) {
				current_heap = current_heap->next_heap;
			}
			if(ha_at_cmpchg((volatile void*)&current_heap->next_heap, (unsigned long)NULL, (unsigned long)htgt,sizeof(*(&current_heap->next_heap))) == (int)NULL) {
				valid = 1;
				DEBUG_PRINT("ha_aconcat_global link %p with %p\n",hsrc,htgt);
				//DEBUG_LIST("ha_aconcat_global\n",hsrc);
			}
		}
	}
}

/* --- ha_aconcat_local_list ---
 * Concat htgt at the end of hsrc for list of heaps with same numa inforamtion
 * This method call an atomic cmp&swap method to link both lists
 */
void ha_aconcat_local_list(ha_heap_t *hsrc, ha_heap_t *htgt) {
	ha_heap_t *current_heap;
	int valid = 0;

	if (IS_HEAP(hsrc) && IS_HEAP(htgt) && hsrc != htgt) {
		current_heap = hsrc;
		while(!valid) {
			while(IS_HEAP(current_heap->next_same_heap)) {
				current_heap = current_heap->next_same_heap;
			}
			if(ha_at_cmpchg((volatile void*)&current_heap->next_same_heap, (unsigned long)NULL, (unsigned long)htgt,sizeof(*(&current_heap->next_same_heap))) == (int)NULL) {
				valid = 1;
				DEBUG_PRINT("ha_aconcat_local link %p with %p\n",hsrc,htgt);
				//DEBUG_LIST("ha_aconcat_local\n",hsrc);
			}
		}
	}
}

/* --- ha_amalloc --- 
 * Allocate memory into the (ha_heap_t*)heap memory area.
 * This allocation is structured by a double chained list of used bloc.
 * A used bloc is a structure including pointers to next and previous
 * bloc the current used size and the size, which it's free between this current
 * bloc and the previous bloc
 * This data structure are addressed inside the heap as follow:
 *
 * 				|	heap			|
 * 				| ...(list of blocs)...		|
 * 				|	.......			|
 * 		new bloc ->	+-------------------------------+ 
 * 				| current size			|
 * 				+-------------------------------+
 * 				| previous size			|
 * 				+-------------------------------+
 * 				| adress of the current heap	|
 * 				+-------------------------------+
 *	 			| address of previous bloc	|
 *	 			+-------------------------------+
 *	 			| address of next bloc		|
 *	 			+-------------------------------+
 *	 			| address of static memory	|
 *	 			+-------------------------------+
 * 				| size of static memory		|
 * 	returned address -> 	+-------------------------------+
 * 				| user data ...			|
 * 				|				|
 * 				+-------------------------------+
 * 				|				|
 * 				|				|
 *
 * The research of enough free space between blocs is done by a first-fit method.
 * If the heap has not enough memory to allocate the caller request , the method create a new heap
 * binded in cases of numa information are set
 * If the memory cannot ba allocated the returning adress to the caller is NULL.
 */
void *ha_amalloc(size_t size, ha_heap_t* heap) {
	ha_ub_t *current_bloc_used, *current_bloc_used_prev, *temp_bloc_used;
	ha_heap_t *temp_heap, *next_same_heap, *prev_next_same_heap;
	void *ptr;

	size = ha_memalign(size);

	DEBUG_PRINT("ha_amalloc size=%d ",(int)size);
	if (!IS_HEAP_POLICY(heap,HEAP_DYN_ALLOC)) {
		DEBUG_PRINT("bad heap\n");
		return NULL;	
	}
	next_same_heap = prev_next_same_heap = heap;
	/* check for heap with enough remaining free space */
	while(IS_HEAP(next_same_heap)) {
		pthread_mutex_lock(&next_same_heap->lock_heap);
		if (next_same_heap->free_size > size + BLOCK_SIZE_T) {
			pthread_mutex_unlock(&next_same_heap->lock_heap);
			break;
		}
		pthread_mutex_unlock(&next_same_heap->lock_heap);
		prev_next_same_heap = next_same_heap;
		next_same_heap = next_same_heap->next_same_heap;
	}
	if (!IS_HEAP(next_same_heap)) {
		/* not enough space found in heap list */
		/* create a new heap */
		DEBUG_PRINT("create new heap from %p\n",prev_next_same_heap);
		temp_heap = ha_acreatenuma(HEAP_MINIMUM_PAGES, HEAP_DYN_ALLOC, heap->mempolicy, heap->weight, heap->nodemask, heap->maxnode);
		/* link heap */
		ha_aconcat_global_list(heap,temp_heap);
		ha_aconcat_local_list(prev_next_same_heap,temp_heap);
		return ha_amalloc(size,temp_heap);
	}
	/* enough space in heap */	
	while(IS_HEAP(next_same_heap)) {
		
		pthread_mutex_lock(&next_same_heap->lock_heap);

		if (next_same_heap->used == NULL) { /* first used bloc in list of current heap */
			DEBUG_PRINT("first bloc of list\n");
			next_same_heap->used = (ha_ub_t*)((char*)next_same_heap + HEAP_SIZE_T);
		
			set_bloc(next_same_heap->used, size, 0, next_same_heap, NULL, NULL);
		
			HEAP_ADD_USED_SIZE(size+BLOCK_SIZE_T,next_same_heap);
			next_same_heap->touch_size = size + BLOCK_SIZE_T;
	
			pthread_mutex_unlock(&next_same_heap->lock_heap);
			DEBUG_LIST("->",next_same_heap);
			return (void*)((char*)(next_same_heap->used) + BLOCK_SIZE_T);
		}
	
		
		/* find first free block: first fit */
		current_bloc_used_prev = current_bloc_used = next_same_heap->used;
		while(current_bloc_used != NULL && current_bloc_used->prev_free_size < size + BLOCK_SIZE_T) {
			current_bloc_used_prev = current_bloc_used;
			current_bloc_used = current_bloc_used->next;
		}
		//DEBUG_PRINT(" %p %p ok\n",current_bloc_used_prev,current_bloc_used);

		if (current_bloc_used == NULL) { /* not enough free space between used bloc */
			DEBUG_PRINT("no free space between used bloc found ");
		
			/* goes to offset of the last used bloc */
			current_bloc_used = current_bloc_used_prev;

			/* check for enough remaining contigous free space */
			if ( (char*)next_same_heap+HEAP_GET_SIZE(next_same_heap) - ((char*)current_bloc_used+BLOCK_SIZE_T+current_bloc_used->size) < size + BLOCK_SIZE_T) { 
				/* goes to next heap */
				pthread_mutex_unlock(&next_same_heap->lock_heap);
				if (IS_HEAP(next_same_heap->next_same_heap)) {
					next_same_heap = next_same_heap->next_same_heap;
					continue; 
				} else {
					/* no bloc was found in each heap and no heap has enough contiguous space remaining */
					/* we need to create a new heap */
					DEBUG_PRINT("create new heap from %p\n",next_same_heap);
					temp_heap = ha_acreatenuma(HEAP_MINIMUM_PAGES, HEAP_DYN_ALLOC, heap->mempolicy, heap->weight, heap->nodemask, heap->maxnode);
					ha_aconcat_global_list(heap,temp_heap);
					ha_aconcat_local_list(next_same_heap,temp_heap);
					return ha_amalloc(size,temp_heap);
				}
			}

			/* create new used bloc */
			DEBUG_PRINT("heap found\n");
			current_bloc_used->next = (ha_ub_t*)((char*)current_bloc_used+BLOCK_SIZE_T+current_bloc_used->size);
			set_bloc(current_bloc_used->next, size, 0, next_same_heap, current_bloc_used, NULL);
			next_same_heap->touch_size = (unsigned long)current_bloc_used->next + size + BLOCK_SIZE_T - (unsigned long)next_same_heap;
			HEAP_ADD_USED_SIZE(size+BLOCK_SIZE_T,next_same_heap);
		
			ptr = (char*)(current_bloc_used->next) + BLOCK_SIZE_T;
	
		} else { /* free space found between block */
			DEBUG_PRINT("free space between used bloc found\n");
		
			if(current_bloc_used->prev != NULL) {
				/* goes to offset of the last used bloc */
				temp_bloc_used = current_bloc_used;
				current_bloc_used = (ha_ub_t*)((char*)current_bloc_used_prev+BLOCK_SIZE_T+current_bloc_used_prev->size);
				set_bloc(current_bloc_used, size, 0, next_same_heap, current_bloc_used_prev, temp_bloc_used);
				ptr = (char*)(current_bloc_used) + BLOCK_SIZE_T;
				current_bloc_used_prev->next = current_bloc_used;
				if (current_bloc_used->next != NULL) {
					current_bloc_used->next->prev_free_size -= (size + BLOCK_SIZE_T);
					current_bloc_used->next->prev = current_bloc_used;
				}
			} else {
				/* used bloc at begining */
				temp_bloc_used = current_bloc_used;
				current_bloc_used = (ha_ub_t*)((char *)next_same_heap+HEAP_SIZE_T);
				next_same_heap->used = current_bloc_used;
				set_bloc(current_bloc_used,size, 0, next_same_heap, NULL, temp_bloc_used);
				ptr = (char*)(current_bloc_used) + BLOCK_SIZE_T;
				if (current_bloc_used->next != NULL) {
					current_bloc_used->next->prev_free_size -= (size + BLOCK_SIZE_T);
					current_bloc_used->next->prev = current_bloc_used;
				}
			}
			HEAP_ADD_USED_SIZE(size+BLOCK_SIZE_T,next_same_heap);
		}
		pthread_mutex_unlock(&next_same_heap->lock_heap);
		return ptr;
	}
	return NULL;
}


/* --- ha_acalloc ---
 * This method reserve a memory space of num_elts*elt_size in (void*)heap
 * The memory is set to zero
 */
void *ha_acalloc(size_t nmemb, size_t size, ha_heap_t* heap) {
	void* ptr;
	size_t local_size = ha_memalign(nmemb*size);

	/* amalloc */
	DEBUG_PRINT("ha_acalloc: call to amalloc\n");
	ptr = ha_amalloc(local_size,heap);
	if (ptr == NULL) {
		DEBUG_PRINT("ha_acalloc: call to ha_amalloc failed\n");
		return NULL;
	}
	/* zeroing memory: only memory area specified */
	MALLOC_ZERO(ptr,local_size);	
	
	return ptr;
}

/* --- ha_arealloc ---
 * ha_arealloc try to realloc a memory reserved area with a new size and return the adress of the new allocated memory.
 * If the new size is equal to zero, the ha_arealloc call ha_afree to remove the bloc from the list.
 * If the (void*)ptr memory reference is NULL, the ha_arealloc call ha_amalloc.
 * The reallocation try to find first to augment the reserved space of the bloc adressed by (void*)ptr.
 * If the space between its previous and next bloc is not enough, a new bloc is allocated.
 * Data are moved in case of the returning adress is different to (void*)ptr.
 */
void *ha_arealloc(void *ptr, size_t size, ha_heap_t* heap) {
	ha_ub_t *current_bloc_used;
	//ha_ub_t *current_bloc_used_prev;
	//ha_ub_t *temp_bloc_used;
	void* new_ptr;
	
	size = ha_memalign(size);

	if (!IS_HEAP_POLICY(heap,HEAP_DYN_ALLOC)) {
		DEBUG_PRINT("ha_arealloc: bad heap\n");
		return NULL;	
	}

	if (ptr == NULL) { /* call amalloc */
		ptr = ha_amalloc(size,heap);
		if (ptr == NULL) {
			DEBUG_PRINT("ha_arealloc: call to amalloc failed\n");
			return NULL;
		}
		return ptr;
	}
	if (size == 0) { /* afree ptr */
		DEBUG_PRINT("ha_arealloc: size = 0 free memory\n");
		ha_afree_heap(ptr,heap);
		return NULL;
	}

	pthread_mutex_unlock(&heap->lock_heap);
	current_bloc_used = (ha_ub_t *)((char*)ptr-BLOCK_SIZE_T);
	if (current_bloc_used->heap == heap) {
		if (current_bloc_used->size == size) { /* do nothing */
			DEBUG_PRINT("ha_arealloc: nothing to do\n");
			pthread_mutex_unlock(&heap->lock_heap);
			return ptr;
		}

		if (current_bloc_used->size > size) { /* reduce bloc size */
			DEBUG_PRINT("ha_arealloc: reduce bloc\n");
			if (current_bloc_used->next != NULL) {
				current_bloc_used->next->prev_free_size += current_bloc_used->size - size;
			}
			current_bloc_used->size = size;
			HEAP_ADD_FREE_SIZE(current_bloc_used->size - size,heap);
			pthread_mutex_unlock(&heap->lock_heap);
			return ptr;
		}	
	
		/* size area need to be increased */
		/* first check enough space after bloc then if not call amalloc, move data and free ptr */ 

		if (current_bloc_used->next != NULL && current_bloc_used->next->prev_free_size >= size - current_bloc_used->size) {
			/* enough space after bloc and not last bloc */ 
			DEBUG_PRINT("ha_arealloc: enough space after bloc not last bloc\n");
			current_bloc_used->next->prev_free_size -= (size - current_bloc_used->size); 
			HEAP_ADD_USED_SIZE(size - current_bloc_used->size,heap);
			current_bloc_used->size = size;
			pthread_mutex_unlock(&heap->lock_heap);
			return ptr;
		}
		if (current_bloc_used->next == NULL  && (char*)heap+HEAP_SIZE_T+HEAP_GET_SIZE(heap) > (char*)current_bloc_used+BLOCK_SIZE_T+size) {
			/* last block and enough remaining space */
			DEBUG_PRINT("ha_arealloc: enough space after bloc and last bloc\n");
			HEAP_ADD_USED_SIZE(size - current_bloc_used->size,heap);
			current_bloc_used->size = size;
			pthread_mutex_unlock(&heap->lock_heap);
			return ptr;
		}	
	}

	/* make a malloc */
	pthread_mutex_unlock(&heap->lock_heap);
	new_ptr = ha_amalloc(size,heap);
	if (new_ptr != NULL) {
		/* move data */
		MALLOC_COPY(new_ptr,ptr,size);	
		/* make a free */
		ha_afree_heap(ptr,heap);
		return new_ptr;
	}
	return ptr;
}



void mai_free(void *ptr)
{
  ha_afree(ptr);
}

/* --- ha_afree ---
 * Free the memory of (void*)ptr by calling ha_afree_heap.
 * Matching heap was stored in the used bloc referencing by (void*)ptr.
 */
void ha_afree(void *ptr) {
	ha_ub_t *current_bloc_used;
	if (ptr != NULL) {
		current_bloc_used = (ha_ub_t *)((char*)ptr-BLOCK_SIZE_T);
		DEBUG_PRINT("afree: heap=%p\n",current_bloc_used->heap);
		ha_afree_heap(ptr,current_bloc_used->heap);		
	}
}

/* --- ha_afree_heap --- 
 * A call to ha_afree_heap consists to remove the used bloc pointed by (void*)ptr
 * from the list of used bloc.
 * In case of a next bloc follow the removed bloc,
 * previous free size of next bloc is increased by the free space of the removed bloc.
 */
void ha_afree_heap(void *ptr, ha_heap_t* heap) {
	ha_ub_t *current_bloc_used;
	size_t size;
	
	if (ptr != NULL && IS_HEAP_POLICY(heap,HEAP_DYN_ALLOC)) {
		pthread_mutex_lock(&heap->lock_heap);
		current_bloc_used = (ha_ub_t *)((char*)ptr-BLOCK_SIZE_T);
		size = current_bloc_used->size;
		if (current_bloc_used->prev != NULL) {
			current_bloc_used->prev->next = current_bloc_used->next;
			if(current_bloc_used->next != NULL) {
				current_bloc_used->next->prev = current_bloc_used->prev;
				current_bloc_used->next->prev_free_size += current_bloc_used->size+BLOCK_SIZE_T+current_bloc_used->prev_free_size;
			}
		} else {
			heap->used = current_bloc_used->next;
			if(current_bloc_used->next != NULL) {
				current_bloc_used->next->prev_free_size += current_bloc_used->size+BLOCK_SIZE_T+current_bloc_used->prev_free_size;
				current_bloc_used->next->prev = NULL;
			}
		}
		HEAP_ADD_FREE_SIZE(size+BLOCK_SIZE_T,heap);
		pthread_mutex_unlock(&heap->lock_heap);
		DEBUG_LIST("ha_afree_heap\n",heap);
	} else {
		/* argument = NULL: undefined behavior */
		DEBUG_PRINT("ha_afree_heap: passing NULL as argument caused undefined behavior\n");
	}
}

/*Allocates and place in the same node of the thread*/
void *mai_malloc(size_t size)
{
  int node,cpu;
  void *ptr=NULL;


  cpu = sched_getcpu();
  node = na_get_nodeidcpu(cpu);

  //Is there an available heap?
   if ( h_local.node_heaps[node] != NULL )	  
     ptr = ha_amalloc(size,h_local.node_heaps[node]);

  return ptr;  
}


void *mai_calloc(size_t nmemb, size_t size)
{
    int node;
    void *ptr=NULL;

  node = na_get_nodeidcpu(sched_getcpu());

  //Is there an available heap?
   if ( h_local.node_heaps[node] != NULL )
      ptr = ha_acalloc(nmemb,size,h_local.node_heaps[node]);

  return ptr;  
}

void *mai_realloc(void *ptr, size_t size)
{
   int node,class;
  void *new_ptr=NULL;

  node = na_get_nodeidcpu(sched_getcpu());

  //Is there an available heap?
   if ( h_local.node_heaps[node] != NULL )
      ptr = ha_arealloc(ptr,size,h_local.node_heaps[node]);
 
  return new_ptr;  

}

/* --- ha_amallinfo --- 
 * Return a structure with information of the usage of the heap:
 * - free memory remaining
 * - used memory
 * - touch memory
 * - attached memory size
 * - number of heaps
 */
ha_amalloc_stat_t ha_amallinfo(ha_heap_t* heap) {
	ha_heap_t * next_heap;
	ha_amalloc_stat_t stats;
	stats.free_size = 0;
	stats.used_size = 0;
	stats.touch_size = 0;
	stats.attached_size = 0;
	stats.npinfo = 0;
	
	next_heap = heap;
	while(IS_HEAP(next_heap)) {
		pthread_mutex_lock(&next_heap->lock_heap);
		stats.free_size += next_heap->free_size;
		stats.used_size += next_heap->used_size;
		stats.touch_size += next_heap->touch_size;
		stats.attached_size += next_heap->attached_size;
		stats.npinfo++;
		pthread_mutex_unlock(&next_heap->lock_heap);
		next_heap = next_heap->next_heap;
	}
	DEBUG_LIST("ha_amallinfo:\n",heap);
	return stats;
}

/* --- ha_aget_heap_from_list ---
 * Return the heap of the list which has numa information: mempolicy, weight and nodemask
 * In case of heap is not found the call return null
 */
ha_heap_t* ha_aget_heap_from_list(int mempolicy, int weight, unsigned long *nodemask, unsigned long maxnode, ha_heap_t* heap) {
	ha_heap_t *current_heap;

	DEBUG_PRINT("ha_aget_heap_from_list from %p (%d,%d) numa=%ld ",heap,mempolicy,weight,*nodemask);
	current_heap = heap;
	while (IS_HEAP(current_heap)) {
		pthread_mutex_lock(&current_heap->lock_heap);
	//	DEBUG_PRINT("[check %d = %d && %d = %d && %d = %d]",*nodemask,*current_heap->nodemask,maxnode, current_heap->maxnode,mempolicy,current_heap->mempolicy,weight,current_heap->weight);
		if (mask_equal(nodemask,maxnode,current_heap->nodemask,current_heap->maxnode) && mempolicy == current_heap->mempolicy && weight == current_heap->weight) {
			pthread_mutex_unlock(&current_heap->lock_heap);
			DEBUG_PRINT(" found: %p\n",current_heap);
			return current_heap;
		}
		pthread_mutex_unlock(&current_heap->lock_heap);
		current_heap = current_heap->next_heap;
	}
	DEBUG_PRINT(" not found\n");
	return NULL;
}



