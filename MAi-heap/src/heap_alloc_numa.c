/*
 * src/maheap_nuha_alloc.c
 *
 * Definition of memory allocator inside a predefined heap
 * working for  NUMA architecture and multithread environements.
 * 
 * Author: Martinasso Maxime
 *
 * (C) Copyright 2007 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 *
 */

#include <sys/mman.h>
#include <errno.h>
#include <stdarg.h>
#include <numaif.h>
#include <sys/syscall.h> /* used to get node pages by calling move_pages syscall */

#include "heap_alloc_numa.h"

/* --- ha_hmove_memory ---
 * Move touched  physical pages to numa paramter: mempolicy, weight and nodemask of each heap having the ppinfo numa parameter
 * Static memory attached to heap are also moved, however the mbind will only success if the static memory are aligned to pages
 */
int ha_hmove_memory(ha_pinfo_t *ppinfo, int mempolicy, int weight, unsigned long *nodemask, unsigned long maxnode,  ha_heap_t *heap)  {
	ha_heap_t *current_heap, *next_same_heap, *match_heap;
	ha_ub_t* current_bloc_used;
	int nb_attach;

	/* look for corresponding heap */
	current_heap = ha_aget_heap_from_list(ppinfo->mempolicy,ppinfo->weight,ppinfo->nodemask,ppinfo->maxnode,heap);
	DEBUG_PRINT("ha_hmove_memory (%d,%d) numa=%ld current_heap %p\n",ppinfo->mempolicy,ppinfo->weight,*ppinfo->nodemask,current_heap);
	if (current_heap == NULL) return 0;

	if (ppinfo->mempolicy == mempolicy && ppinfo->weight == weight && mask_equal(ppinfo->nodemask,ppinfo->maxnode,nodemask,maxnode)) {
		return 0;
	}
	
	DEBUG_LIST("ha_hmove_memory before\n",current_heap);

	/* look for a existing matching heap to link */
	match_heap = ha_aget_heap_from_list(mempolicy,weight,nodemask,maxnode,heap);

	/* map heap */
	next_same_heap = current_heap;
	while(IS_HEAP(next_same_heap)) {
		pthread_mutex_lock(&next_same_heap->lock_heap);
		if (ha_amaparea(next_same_heap, mempolicy, weight, nodemask, maxnode) != 0) {
			DEBUG_PRINT("ha_amaparea failed\n");
		}
		nb_attach = next_same_heap->nb_attach;
		current_bloc_used = next_same_heap->used;
		while(current_bloc_used != NULL && nb_attach > 0) {
			if (current_bloc_used->data != NULL) {
				if (ha_maparea(current_bloc_used->data, current_bloc_used->stat_size, mempolicy, nodemask, maxnode) != 0) {
					DEBUG_PRINT("ha_amaparea failed\n");
				}
				nb_attach--;
			}
			current_bloc_used = current_bloc_used->next;
		}
		pthread_mutex_unlock(&next_same_heap->lock_heap);
		next_same_heap = next_same_heap->next_same_heap;
	}
	
	/* rebuild list */
	if( match_heap != NULL) {
		if (current_heap > match_heap) {
			ha_aconcat_local_list(match_heap,current_heap);
		} else {
			ha_aconcat_local_list(current_heap,match_heap);
		}
	}
	return 1;
}

/* --- ha_hget_pinfo ---
 * Retrieve the numa information of (void*)ptr into ppinfo from heap
 * Works also if (void*)ptr is a static memory attached to heap
 */
void ha_hget_pinfo(void *ptr, ha_pinfo_t* ppinfo, ha_heap_t *heap) {
	ha_heap_t* current_heap;
	ha_ub_t* current_bloc_used;
	int i,found = 0;
	int max_nodes = numa_max_node() + 1;
	current_heap = heap;
	while(IS_HEAP(current_heap)) {
		pthread_mutex_lock(&current_heap->lock_heap);
		if ((unsigned long) ptr < (unsigned long)current_heap + HEAP_GET_SIZE(current_heap) && (unsigned long)ptr > (unsigned long)current_heap) {
			/* found: heap < data < heap+size */
			DEBUG_PRINT("found %p in %p\n",ptr,current_heap);
			ppinfo->mempolicy = current_heap->mempolicy;
			ppinfo->weight = current_heap->weight;
			for (i = 0; i < current_heap->maxnode/WORD_SIZE; ++i) {		
				ppinfo->nodemask[i] = current_heap->nodemask[i];
			}
			ppinfo->maxnode = current_heap->maxnode;
			for(i = 0; i < max_nodes; ++i) {
				ppinfo->nb_touched[i] = 0;
			}
			found = 1;
			pthread_mutex_unlock(&current_heap->lock_heap);
			break;
		}
		pthread_mutex_unlock(&current_heap->lock_heap);
		current_heap = current_heap->next_heap;
	}

	if (!found) { /* not found as regular block check if it's an attached memory */
		current_heap = heap;
		while(current_heap != NULL && !found) {
			pthread_mutex_lock(&current_heap->lock_heap);
			current_bloc_used = current_heap->used;
			while(current_bloc_used != NULL) {
				if (current_bloc_used->data == ptr) {
					ppinfo->mempolicy = current_heap->mempolicy;
					ppinfo->weight = current_heap->weight;
					for (i = 0; i < current_heap->maxnode/WORD_SIZE; ++i) {		
						ppinfo->nodemask[i] = current_heap->nodemask[i];
					}
					ppinfo->maxnode = current_heap->maxnode;
					for(i = 0; i < max_nodes; ++i) {
						ppinfo->nb_touched[i] = 0;	
					}
					found = 1;
					break;
				}
				current_bloc_used = current_bloc_used->next;
			}
			pthread_mutex_unlock(&current_heap->lock_heap);
			current_heap = current_heap->next_heap;
		}
	}
}

/* --- ha_hupdate_memory ---
 * Retrieve the number of pages touched for each numa nodes
 * Data are stored in ppinfo, and concerne each heap and static memory associated to numa information of ppinfo
 * To have this information the method call the syscall __NR_move_pages
 */
#ifdef __NR_move_pages
void ha_hupdate_memory_nodes(ha_pinfo_t *ppinfo, ha_heap_t *heap) {
	ha_heap_t* current_heap;
	ha_ub_t* current_bloc_used;
	int i, nb_pages, pagesize, node, nb_attach;
	void **addr;
	int *node_pages;

	int max_nodes = numa_max_node() + 1;
	
	if (IS_HEAP(heap)) {
		pagesize = getpagesize();
		for(i = 0; i < max_nodes; ++i) {
			ppinfo->nb_touched[i] = 0;	
		}
		// look for corresponding heap 
		current_heap = ha_aget_heap_from_list(ppinfo->mempolicy,ppinfo->weight,ppinfo->nodemask,ppinfo->maxnode,heap);

		if (IS_HEAP(current_heap)) {
			while(IS_HEAP(current_heap)) {
				pthread_mutex_lock(&current_heap->lock_heap);
				nb_pages = HEAP_GET_SIZE(current_heap)/pagesize;
				addr = malloc(nb_pages*sizeof(void*));
				node_pages = malloc(nb_pages*sizeof(int));
				for(i = 0; i < nb_pages; ++i) {
			                addr[i] = (void*)(current_heap+i*pagesize);
		        	        node_pages[i] = -1;
				}
					
				int ret = syscall(__NR_move_pages, 0, nb_pages, addr, NULL, node_pages, MPOL_MF_MOVE);
				if (ret != 0) {
					if (errno == ENOSYS)
						fprintf(stderr,"upgrade to kernel >= 2.6.18\n");
					else
						fprintf(stderr,"errno syscall6 = %d\n", errno);
				}
				for(i = 0; i < nb_pages; ++i) {
					ppinfo->nb_touched[node_pages[i]]++;
					
		               	}
				free(addr);
				free(node_pages);

				if (current_heap->nb_attach > 0) {
					nb_attach = current_heap->nb_attach;
					current_bloc_used = current_heap->used;
					while(current_bloc_used != NULL && nb_attach > 0) {
						if (current_bloc_used->data != NULL) {
							nb_pages = current_bloc_used->stat_size/pagesize;
							addr = malloc(nb_pages*sizeof(void*));
							node_pages = malloc(nb_pages*sizeof(int));
							for(i = 0; i < nb_pages; ++i) {
						                addr[i] = (void*)(current_bloc_used->data+i*pagesize);
		        				        node_pages[i] = -1;
							}
							syscall(__NR_move_pages, 0, nb_pages, addr, NULL, node_pages, MPOL_MF_MOVE);
							for(i = 0; i < nb_pages; ++i) {
								ppinfo->nb_touched[node_pages[i]]++;
		               				}
							free(addr);
							free(node_pages);
							nb_attach--;
						}
						current_bloc_used = current_bloc_used->next;
					}
				}
				pthread_mutex_unlock(&current_heap->lock_heap);
				current_heap = current_heap->next_same_heap;
			}
		}
	}
}
#endif

/* --- ha_hnext_pinfo ---
 * Iterate over the different numa information of a list of heap
 * To initiate the iteration ppinfo has to be null
 * ppinfo is allocated inside this method one time
 */
int ha_hnext_pinfo(ha_pinfo_t **ppinfo, ha_heap_t* heap) {
	ha_heap_t *current_heap, *current_same_heap;
	int i,iterator_num = HEAP_ITERATOR_ID_UNDEFINED;
	int max_nodes = numa_max_node() + 1;

	if (IS_HEAP(heap)) {
		if (*ppinfo == NULL) {
			/* first call, allocate iterator */
			if (heap->iterator == NULL) {
				heap->iterator = ha_amalloc(sizeof(ha_pinfo_t)+heap->maxnode,heap);
			}
			*ppinfo = heap->iterator;
			(*ppinfo)->mempolicy = HEAP_UNSPECIFIED_POLICY;
			(*ppinfo)->weight = HEAP_UNSPECIFIED_WEIGHT;
			(*ppinfo)->maxnode = 0;
			(*ppinfo)->nodemask = heap->iterator+sizeof(ha_pinfo_t); 

			/* reset iterator, some node may have changed */
			current_heap = heap;
			while (IS_HEAP(current_heap)) {
				pthread_mutex_lock(&current_heap->lock_heap);
				if (current_heap->iterator_num != HEAP_ITERATOR_ID_UNDEFINED) {
					current_heap->iterator_num = HEAP_ITERATOR_ID_UNDEFINED;
				}
				pthread_mutex_unlock(&current_heap->lock_heap);
				current_heap = current_heap->next_heap;
			}
			DEBUG_PRINT("ha_hnext_pinfo: first call, it=%d\n",iterator_num);
		}

		/* set iterator id list */
		current_heap = heap;
		while (IS_HEAP(current_heap)) {
			if (current_heap->iterator_num == HEAP_ITERATOR_ID_UNDEFINED) {
				pthread_mutex_lock(&current_heap->lock_heap);
				current_heap->iterator_num = iterator_num++;
				current_same_heap = current_heap;
				pthread_mutex_unlock(&current_heap->lock_heap);
				while(IS_HEAP(current_same_heap)) {
					pthread_mutex_lock(&current_same_heap->lock_heap);
					current_same_heap->iterator_num = iterator_num;
					pthread_mutex_unlock(&current_same_heap->lock_heap);
					current_same_heap = current_same_heap->next_same_heap;
				}
			} 
			current_heap = current_heap->next_heap;
		}
		DEBUG_LIST("ha_hnext_pinfo list:\n",heap);
	
		/* goes to next heap higher iterator_num */
		current_heap = ha_aget_heap_from_list((*ppinfo)->mempolicy,(*ppinfo)->weight,(*ppinfo)->nodemask,(*ppinfo)->maxnode,heap);
		if (current_heap == NULL) {
			current_heap = heap;
			iterator_num = HEAP_ITERATOR_ID_UNDEFINED;
		} else {
			iterator_num = current_heap->iterator_num;
		}
		current_heap = heap;
		while (IS_HEAP(current_heap)) {
			if (current_heap->iterator_num == iterator_num + 1) {
				DEBUG_PRINT("next heap %p it=%d\n",current_heap,current_heap->iterator_num);
				break;
			}
			current_heap = current_heap->next_heap;
		}
		if (!IS_HEAP(current_heap)) { /* no more pinfo */
			return 0;
		}
		// set ppinfo 
		pthread_mutex_lock(&current_heap->lock_heap);
		(*ppinfo)->mempolicy = current_heap->mempolicy;
		(*ppinfo)->weight = current_heap->weight;
		for (i = 0; i < current_heap->maxnode/sizeof(unsigned long); ++i) {		
			(*ppinfo)->nodemask[i] = current_heap->nodemask[i];
		}	
		(*ppinfo)->maxnode = current_heap->maxnode;
		for(i = 0; i < max_nodes; ++i) {
			(*ppinfo)->nb_touched[i] = 0;
		}
		pthread_mutex_unlock(&current_heap->lock_heap);
		return 1;
	}
	return -1;
}
/* --- ha_hfusion_heap
 * merge heaps
 * note: the iterator of (ha_heap_t*)h is freed
 */
void ha_hfusion_heap(ha_heap_t *hacc, ha_heap_t *h) {
	ha_heap_t* current_heap;
	ha_heap_t* merging_heap;

	if (IS_HEAP(hacc) && IS_HEAP(h) && hacc != h) {
		current_heap = hacc;
		while (IS_HEAP(current_heap)) {
			pthread_mutex_lock(&current_heap->lock_heap);
			/* look for last numa heap */
			if (current_heap->next_same_heap == NULL) {
				/* look for matching heap */
				merging_heap = ha_aget_heap_from_list(current_heap->mempolicy,current_heap->weight,current_heap->nodemask,current_heap->maxnode,h);
				if (merging_heap != NULL) {
					ha_aconcat_local_list(current_heap,merging_heap);
				}
			}
			pthread_mutex_unlock(&current_heap->lock_heap);
			current_heap = current_heap->next_heap;
		}
		ha_aconcat_global_list(hacc,h);
		if (h->iterator != NULL) {
			ha_afree(h->iterator);
		}
	}
}

/* --- ha_hcreate_heap
 * Create a heap with a minimum size
 */
ha_heap_t* ha_hcreate_heap(void) {
	unsigned int pagesize = getpagesize();
	return ha_acreate(HEAP_MINIMUM_PAGES*pagesize, HEAP_DYN_ALLOC);
}


/* --- ha_hdelete_heap
 * Delete a heap
 */
void ha_hdelete_heap(ha_heap_t *heap) {
	ha_adelete(&heap);
}


/* --- ha_hattach_memory
 * Attach memory to a heap
 * first a ha_hamalloc is done with a size of 0, then the created bloc is set with the mem_stat pointer and the stat_size of the bloc
 * Note that for the mbind to sucess it needs a static size aligned to page sizes 
 */
void ha_hattach_memory(void *ptr, size_t size, int mempolicy, int weight, unsigned long *nodemask, unsigned long maxnode, ha_heap_t *heap) {
	ha_ub_t* current_bloc_used;
	ha_heap_t* current_heap;
	void* local_ptr;

	local_ptr = ha_hmalloc(0, mempolicy, weight, nodemask, maxnode, heap);
	current_bloc_used = (ha_ub_t *)((char*)local_ptr-BLOCK_SIZE_T);
	current_heap = current_bloc_used->heap;;
	current_bloc_used->data = ptr;
	current_bloc_used->stat_size = size;
	pthread_mutex_lock(&current_heap->lock_heap);
	current_heap->attached_size += size;
	current_heap->nb_attach++;
	ha_maparea(ptr,size,mempolicy,current_heap->nodemask,current_heap->maxnode);
	pthread_mutex_unlock(&current_heap->lock_heap);
}

/* --- ha_hdetach_memory
 * Detach a memory attached to a heap
 */
void ha_hdetach_memory(void *ptr, ha_heap_t *heap) {
	int found = 0;
	ha_ub_t* current_bloc_used;
	ha_heap_t* current_heap;

	current_heap = heap;
	while(IS_HEAP(current_heap) && !found) {
		pthread_mutex_lock(&current_heap->lock_heap);
		current_bloc_used = current_heap->used;
		while(current_bloc_used != NULL) {
			if (current_bloc_used->data == ptr) {
				current_heap->attached_size -= current_bloc_used->stat_size;
				current_heap->nb_attach--;
				found = 1;
				if (current_bloc_used->prev != NULL) {
					current_bloc_used->prev->next = current_bloc_used->next;
					if(current_bloc_used->next != NULL) {
						current_bloc_used->next->prev = current_bloc_used->prev;
						current_bloc_used->next->prev_free_size += BLOCK_SIZE_T+current_bloc_used->prev_free_size;
					}
				} else {
					current_heap->used = current_bloc_used->next;
					if(current_bloc_used->next != NULL) {
						current_bloc_used->next->prev_free_size += BLOCK_SIZE_T+current_bloc_used->prev_free_size;
						current_bloc_used->next->prev = NULL;
					}
				}
				break;
			}
			current_bloc_used = current_bloc_used->next;
		}
		pthread_mutex_unlock(&current_heap->lock_heap);
		current_heap = current_heap->next_heap;
	}

}

/* --- ha_hmalloc
 * Call ha_amalloc to allocate memory
 * bind heap if is not already binded
 */
void *ha_hmalloc(size_t size, int mempolicy, int weight, unsigned long *nodemask, unsigned long maxnode, ha_heap_t *heap) {
	ha_heap_t* current_heap;
	fprintf(stderr,"ha_hmalloc size=%ld at %p (%d,%d) numa=%ld\n",size,heap,mempolicy,weight,*nodemask); 
	DEBUG_PRINT("ha_hmalloc size=%ld at %p (%d,%d) numa=%ld\n",size,heap,mempolicy,weight,*nodemask); 
	/* look for corresponding heap */
	current_heap = ha_aget_heap_from_list(mempolicy,weight,nodemask,maxnode,heap);
	//fprintf(stderr,"ha_hmalloc mempolicy %d, nodemask %ld, maxnode %ld, thread %p\n", mempolicy, *nodemask, maxnode, MARCEL_SELF);
	if (current_heap == NULL) {
		pthread_mutex_lock(&heap->lock_heap);
		if (heap->mempolicy == HEAP_UNSPECIFIED_POLICY && heap->weight == HEAP_UNSPECIFIED_WEIGHT && heap->maxnode == 0) {
			ha_amaparea(heap,mempolicy,weight,nodemask,maxnode);
			pthread_mutex_unlock(&heap->lock_heap);
			return ha_amalloc(size,heap);
		} else {
			/* new heap to create */
			pthread_mutex_unlock(&heap->lock_heap);
			current_heap = ha_acreatenuma(2*size,HEAP_DYN_ALLOC,mempolicy,weight,nodemask,maxnode);
			ha_aconcat_global_list(heap,current_heap);
		}
	}
	return ha_amalloc(size,current_heap);
}

/* --- ha_hrealloc
 * Realloc memory, call to ha_arealloc
 */
void *ha_hrealloc(void *ptr, size_t size) {
	ha_ub_t* current_bloc_used;
	ha_heap_t *current_heap;

	current_bloc_used = (ha_ub_t *)((char*)ptr-BLOCK_SIZE_T);
	current_heap = current_bloc_used->heap;
	return ha_arealloc(ptr,size,current_heap);
}

/* --- ha_hcalloc
 * Call ha_acalloc to allocate memory
 * bind heap if is not already binded
 */
void *ha_hcalloc(size_t nmemb, size_t size, int mempolicy, int weight, unsigned long* nodemask, unsigned long maxnode, ha_heap_t *heap) {
	ha_heap_t* current_heap;

	/* look for corresponding heap */
	current_heap = ha_aget_heap_from_list(mempolicy,weight,nodemask,maxnode,heap);

	if (current_heap == NULL) {
		pthread_mutex_lock(&heap->lock_heap);
		if (size < HEAP_GET_SIZE(heap)+BLOCK_SIZE_T && heap->mempolicy == HEAP_UNSPECIFIED_POLICY && heap->weight == HEAP_UNSPECIFIED_WEIGHT && heap->maxnode == 0) {
			ha_amaparea(heap,mempolicy,weight,nodemask,maxnode);
			pthread_mutex_unlock(&heap->lock_heap);
			return ha_acalloc(nmemb,size,heap);
		} else {
			/* new heap to create */
			pthread_mutex_unlock(&heap->lock_heap);
			current_heap = ha_acreatenuma(2*size,HEAP_DYN_ALLOC,mempolicy,weight,nodemask,maxnode);
			ha_aconcat_global_list(heap,current_heap);
			ha_aconcat_local_list(heap,current_heap);
		}
	}
	return ha_acalloc(nmemb,size,current_heap);
}
/* --- ha_hfree ---
 *  free memory by calling ha_afree
 */
void ha_hfree(void *data) {
	ha_afree(data);
}


