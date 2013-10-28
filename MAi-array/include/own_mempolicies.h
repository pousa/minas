/*=====================================================================================
  MAi is a interface to manage memory policy in NUMA architectures

  (C) Copyright 2010 INRIA 
  Projet: MESCAL / ANR NUMASIS

  Author: Christiane Pousa Ribeiro

  The author may be reached at pousa@imag.fr
 *====================================================================================*/

#ifndef __OWN_MEMPOLICIES_H
#define __OWN_MEMPOLICIES_H

#ifdef __cplusplus
extern "C" {
#endif

/*Nodes to be used*/
unsigned long *own_nodes,nown_nodes;

/*Compute array blocks - function prototype*/
void mai_subarray(void *p,int dim[]);
void mai_bytes(void* p,size_t bytes);

/*data distribution - function prototype */
int mai_regularbind(void *p);
int mai_irregularbind(void *p,size_t bytes,int node);

/*nodes settings - function prototype*/
void mai_nodes(unsigned long nnodes, int *nodes);
float mai_nodebandwidth(int node);
float mai_numafactor(int node1, int node2);
float mai_bandwidth(int node1, int node2);

/*pointer control - function prototype*/
void* mai_get_realpointer(void *p);
void* mai_move_pointer(void *p,size_t bytes);
void* mai_mv_pointer_rows(void *p,int rows);
void* mai_mv_pointer_cols(void *p,int cols);
void* mai_mv_pointer_plans(void *p,int plans);

/*array control - function prototype*/
size_t mai_getsize(void* ph);
int mai_get_mempol(void* ph);
int* mai_get_arraynodes(void* ph);
void mai_set_arraynodes(void* ph,int nnodes,int nodes[]);

/*Threads control - function prototype*/
int mai_get_threadnode(pid_t tid);
void mai_bind_thread(pid_t tid,int cpu);

/*Architecture control - function prototype*/
int mai_get_maxnodes();
int mai_get_maxcpus();
int mai_get_ncpusnode();
int* mai_get_cpusnode(int nodeid);

#ifdef __cplusplus
}
#endif

#endif

