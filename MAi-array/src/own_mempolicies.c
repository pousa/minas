/*=====================================================================================
  MAi is a interface to manage memory policy in NUMA architectures

  (C) Copyright 2010 INRIA 
  Projet: MESCAL / ANR NUMASIS

  Author: Christiane Pousa Ribeiro

  The author may be reached at pousa@imag.fr
 *====================================================================================*/

#include <sys/mman.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stddef.h>
#include <numa.h>
#include <numaif.h>
#include <math.h>
#include <utmpx.h>


#include "common.h"
#include "numArch.h"
#include "own_mempolicies.h"
#include "thread_policies.h"

/*Divides the array p on a subarray 
parameters: p-start address,dim[]-dimmensions of the subarray
return:none
*/
void mai_subarray(void *p,int dim[])
{
 size_t nbytes,bytes,size;
 int np,nitens,block,nx,ny,nz,nk,i;

 nx = (dim[0]>0)? dim[0]:1;
 ny = (dim[1]>0)? dim[1]:1;
 nz = (dim[2]>0)? dim[2]:1;
 nk = (dim[3]>0)? dim[3]:1;

 if(ny == 1 || ny == get_var_dim(p,2))
 {
  for(i=0;i<4;i++)
     block *= dim[i];
  size = block * get_var_size(p);
  //align memory to be bind
  nbytes = alignmem(size);  
  block = nbytes/PAGE_SIZE;
 }
 else
  block = 0;
 
 set_var_block(p,block);
 
}

/*Divides the array p on blocks of bytes  
parameters: p-start address,bytes-size of block
return:-none
*/
void mai_bytes(void* p,size_t bytes)
{
 size_t nbytes,size;
 int np,nitens,block;

 //align memory to be bind
 nbytes = alignmem(bytes)/PAGE_SIZE; 
  
 set_var_block(p,nbytes);
 
}

/*Bind blocks of data from array p to nodes  
parameters: p-start address
return:-1- error, 0-bind was done
*/
int mai_regularbind(void *p)
{
 unsigned long rnode,freemem=0;
 int err,block,nitens,mig=0,i;
 size_t size,totalmem;
 void *t;

 t = get_var_pl(p); 
 block = get_var_block(p);

 for(i=0;i<nown_nodes;i++)
  freemem += na_get_memnode(own_nodes[i]);

 nitens = get_var_itens(p);
 size = get_var_size(p);
 totalmem = nitens*size;
 
  if(get_var_policy(p) != OWN && get_var_policy(p) != -1)
    mig=1;

    if(block !=0){
     
	 //if there are enough memory
	 if(totalmem < (freemem*KBYTE))
	  {
	   for(i=0;i<((totalmem/PAGE_SIZE)/block);i++){
            rnode = own_nodes[i%nown_nodes];
	    if(mig)
	    {
	      #ifdef MPOL_MF_MOVE
	      err=mbind(t,block*PAGE_SIZE,MPOL_BIND,&rnode,MASK_SIZE,MPOL_MF_MOVE);    
	      #else
	      printf("\n Operating system do not support data migration!");
	      exit(-1);
	      #endif 
	     }
	    else
	       err=mbind(t,block*PAGE_SIZE,MPOL_BIND,&rnode,MASK_SIZE,0);

	    t = t + block*PAGE_SIZE;
	   }
	   set_var_policy(p,OWN);
	  
	  }
	  else{
	   printf("\n Nodes do not have enough memory!");
	   err = -1;
	  }
    }//end per block distribution
   else
   { printf("\n There is no support for column distribution!");}
  return err;

}

/*Bind bytes to node starting from p
parameters: p-start address, bytes-number of bytes to bind
            node-node bank to be used
return:-1- error, 0-bind was done
*/
int mai_irregularbind(void *p,size_t bytes,int node)
{
 size_t nbytes;
 unsigned long rnode,freemem;
 int err;
 
 //align memory to be bind
 nbytes = alignmem(bytes);
 
 //compute real node id
 rnode = 1 << (node - 1);

 freemem = na_get_memnode(node-1);
 
 //if there are enough memory
 if(nbytes < (freemem*KBYTE))
  {
  	
   if(get_var_policy(get_var_ph(p)) != OWN && get_var_policy(get_var_ph(p)) != -1)
     {
      #ifdef MPOL_MF_MOVE
      err=mbind(p,nbytes,MPOL_BIND,&rnode,MASK_SIZE,MPOL_MF_MOVE);    
      #else
      printf("\n Operating system do not support data migration!");
      exit(-1);
      #endif 
     }
    else
       err=mbind(p,nbytes,MPOL_BIND,&rnode,MASK_SIZE,0);

    set_var_policy(get_var_ph(p),OWN);
   	
  }
  else{
   printf("\n Nodes do not have enough memory!");
   err = -1;
  }
 
  return err;

}

/*Get cpus/cores id of a node
parameters: node id
return: cpus/core id
*/
int* mai_get_cpusnode(int nodeid)
{
  int *ids=NULL,i;

  ids = na_get_cpusidnode(nodeid);
  for(i=0;i<na_get_cpusnode();i++)
    ids[i]=ids[i]+1;

  return ids;
}

/*Get size of an array
parameters: p-array
return: size of array
*/
size_t mai_getsize(void *p)
{
 int nitens = get_var_itens(p);
 size_t size = get_var_size(p);

 return size*nitens;
}

/*Get number of cpus/cores per node
parameters: none
return: number of cpus/core
*/
int mai_get_ncpusnode()
{
  return na_get_cpusnode();
}

/*Get number of nodes
parameters: none
return: number of nodes
*/
int mai_get_maxnodes()
{
  return na_get_maxnodes();
}

/*Set number of nodes and nodes id for an array
parameters: ph-array,nnodes-number of nodes, nodes-nodes id
return: none
*/
void mai_set_arraynodes(void* ph,int nnodes,int nodes[])
{
  set_var_nodes(ph,nnodes,nodes);
}

/*Get number of cpus
parameters: none
return: number of cpus
*/
int mai_get_maxcpus()
{
  return na_get_maxcpus();
}

/*Set number of nodes and nodes to be used
parameters: nnodes-number of nodes, nodes-
	    array with nodes id
return: node
*/
void mai_nodes(unsigned long nnodes, int *nodes)
{
  int i;

  nown_nodes = nnodes;
  own_nodes = malloc(nown_nodes*sizeof(unsigned long));

  for(i=0;i<nnodes;i++)  
   if (nodes[i] <= na_get_maxnodes())
     own_nodes[i]= 1 << (nodes[i]-1);
}

/*
Get bandwidth inside a node
parameters:node- node
return: bandwidth
*/
float mai_nodebandwidth(int node)
{
   FILE *fp;
  int maxnodes,i=0,possible;
  char *str; 
  float bandwidth=0.0;
 
  maxnodes = na_get_maxnodes();

  fp = fopen("/tmp/bandwidth.minas","r");

  /*read the node bandwidth*/
  if ( fp != NULL && possible)
  {
    do{
      fscanf (fp,"%f",&(bandwidth));
      i++; 
    }while(i < (((node-1)*maxnodes)+ node));
   }
   else
   {
    printf("\n You may install Minas.");
    exit(1);
   }
 
   fclose(fp);
 
  return bandwidth;
}

/*
Get Numa factor between two nodes
parameters:node1-first node, node2-second node
return: numa factor
*/
float mai_numafactor(int node1, int node2)
{
  FILE *fp;
  int maxnodes,i=0,possible;
  char *str;
  float numafactor=1.0;
 
  maxnodes = na_get_maxnodes();

  fp = fopen("/tmp/numafactor.minas","r");

  /*read numa factor*/
 if ( fp != NULL)
  {
    do{
      fscanf (fp,"%f",&numafactor);
      i++; 
    }while(i < (((node1-1)*maxnodes)+ node2));
   }
   else
   {
    printf("\n You may install Minas.");
    exit(1);
   }
 
   fclose(fp);
 
  return numafactor;
}

/*
Get bandwidth between two nodes
parameters:node1-first node, node2-second node
return: bandwidth
*/
float mai_bandwidth(int node1, int node2)
{
  FILE *fp;
  int maxnodes,i=0;
  char *str;
  float bandwidth=0.0; 

  maxnodes = na_get_maxnodes();

  fp = fopen("/tmp/bandwidth.minas","r");

  if ( fp != NULL)
  {
    do{
      fscanf (fp,"%f",&(bandwidth));
      i++; 
    }while(i < (((node1-1)*maxnodes)+ node2));
   }
   else
   {
    printf("\n You may install Minas.");
    exit(1);
   }
 
   fclose(fp);
 
  return bandwidth;
}

/*
Get the real pointer reference of an array
parameters:p-array
return: real address
*/
void* mai_get_realpointer(void *p)
{
 return get_var_pl(p);
}

/*
Move a pointer, always considering a memory page as
unit
parameters:p-pointer, bytes-number of bytes
return: new address
*/
void* mai_move_pointer(void *p,size_t bytes)
{
 void *t;
 size_t nbytes;

pthread_mutex_lock(&(mai_mutex));
 t = p;
 nbytes = alignmem(bytes);
 t = t + nbytes;
pthread_mutex_unlock(&(mai_mutex));
 return t;
}

/*
Move a pointer, always considering memory page
parameters:p-pointer, rows-number of rows
return: new address
*/
void* mai_mv_pointer_rows(void *p,int rows)
{
  void *t,*pointer=NULL;
  int moving,i;

  t = get_var_ph(p); 
  pointer = p;

  for(i=2;get_var_dim(t,i)!=0;i++)
        moving *= get_var_dim(t,i);
 
  moving *= rows*get_var_size(t);
  moving = moving/PAGE_SIZE;
  
  pointer = pointer + moving*PAGE_SIZE;

  return pointer;
}

/*
Move a pointer, always considering memory page
parameters:p-pointer, rows-number of cols
return: new address
*/
void* mai_mv_pointer_cols(void *p,int cols)
{
  void *t,*pointer=NULL;
  int moving,i;

  t = get_var_ph(p); 
  pointer = p;

  for(i=3;get_var_dim(t,i)!=0;i++)
        moving *= get_var_dim(t,i);
 
  moving *= cols*get_var_dim(t,1)*get_var_size(t);
  moving = moving/PAGE_SIZE;
  
  pointer = pointer + moving*PAGE_SIZE;

  return pointer;
}

/*
Move a pointer, always considering memory page
parameters:p-pointer, rows-number of plans
return: new address
*/
void* mai_mv_pointer_plans(void *p,int plans)
{
  void *t,*pointer=NULL;
  int moving,i;

  t = get_var_ph(p); 
  pointer = p;

  for(i=1;i<4;i++)
        moving *= get_var_dim(t,i);
 
  moving *= plans*get_var_size(t);
  moving = moving/PAGE_SIZE;
  
  pointer = pointer + moving*PAGE_SIZE;

  return pointer;
}

/*
Get the array memory policy
parameters: ph-array address
return: memory policy
*/
int mai_get_mempol(void* ph)
{
  int mpol=-1;
  
  mpol = get_var_policy(ph);

  return mpol;
}

/*
Get the array nodes
parameters: ph-array address
return: pointer with nodes id
*/
int* mai_get_arraynodes(void* ph)
{
 int nnodes=0,*nodes=NULL,i;

 nodes = get_var_nodes(ph,&nnodes);
 for(i=0;i<nnodes;i++)
  nodes[i]=nodes[i]+1; 

 return nodes; 
}

/*
Get the thread cpu
parameters: tid-thread id
return: cpu/core number
*/
int mai_get_threadcpu(pid_t tid)
{
return get_thread_cpu();
}

/*
Bind a thread to specified cpu/core
parameters: tid-thread id, cpu- cpu/core
return: none
*/
void mai_bind_thread(pid_t tid,int cpu)
{
 int err;
 unsigned long ncpu;

 ncpu = 1 << (cpu-1);

 err=sched_setaffinity(tid,CPU_MASK_SIZE,&ncpu);
  
      if(err<0){
	identify_error();
        printf("\t Erro: bind_thread!");
     }
}
