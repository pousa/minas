#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/syscall.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <numa.h>
#include <numaif.h>
#include <dirent.h>

#include "ma_linux.h"

/* manage bitmap */
#define BITS_IN_BYTES 8
#define WORD_SIZE       (8*sizeof(unsigned long))
#define BITS_IN_WORDS (BITS_IN_BYTES*WORD_SIZE)

/* Mark/Clear bits with given index */
#define markbit(M,i)       (M[i / BITS_IN_WORDS] |= 1 << (i % BITS_IN_WORDS))
#define clearbit(M,i)      (M[i / BITS_IN_WORDS] &= ~(1 << (i % BITS_IN_WORDS)))
#define bit_is_marked(M,i) (M[i / BITS_IN_WORDS] & (1 << (i % BITS_IN_WORDS)))

static inline int mask_isset(unsigned long *a, unsigned long sa, int node) {
        if ((unsigned int)node >= sa*8) return 0;
        return bit_is_marked(a,node);
}


/*
TODO: get thread id - portable
*/
unsigned ma_linux_get_myid()
{
  return syscall(SYS_gettid);
}


/*
* Set the core where the current process will run
* return the core
*/
void ma_linux_set_proc_core(int core)
{
  cpu_set_t set;

  CPU_ZERO(&set);
  CPU_SET(core, &set);
  if(sched_setaffinity(0,sizeof(set),&set)!=0)
     printf("\n Error binding threads");
}

/*
* Get the core where the current process is running
* return the core
*/
int ma_linux_proc_core()
{
  int core;
  core = sched_getcpu();
  return core;
}

/*
* Set the core where the current thread will run
* return the core
*/
void ma_linux_set_my_core(int core)
{
  cpu_set_t set;

  CPU_ZERO(&set);
  CPU_SET(core, &set);

  if(sched_setaffinity(syscall(SYS_gettid),sizeof(set),&set)!=0)
     printf("\n Error binding threads");
}

/*
* Get the core where the current thread is running
* return the core
*/
int ma_linux_my_core()
{
  int core;
  core = sched_getcpu();
  return core;
}

int linux_is_numa()
{
 return numa_available();
}

/*get inumber of nodes
return 0 if the machine is not NUMA
*/
int ma_linux_get_nnodes()
{
        int i;
        int             node=-1;
        char            dirnamep[256];
        char            temp[33];
        struct dirent   *dirent;
        DIR             *dir;
        int maxnodes = 0;

        temp[0]=dirnamep[0]='\0';

        do{
        strcpy(dirnamep,"/sys/devices/system/node/node");
        temp[0]='\0';
        sprintf(temp, "%i", maxnodes);
        strcat(dirnamep,temp);
        dir = opendir(dirnamep);
           if(dir != NULL)
             maxnodes++;
        } while(dir != NULL);

        closedir(dir);

 return maxnodes;

}

/*get inumber of cores
*/
int ma_linux_get_ncores()
{
        int i;
        int             node=-1;
        char            dirnamep[256];
        char            temp[33];
        struct dirent   *dirent;
        DIR             *dir;
        int maxcores = 0;

        temp[0]=dirnamep[0]='\0';

        do{
        strcpy(dirnamep,"/sys/devices/system/cpu/cpu");
        temp[0]='\0';
        sprintf(temp, "%i", maxcores);
        strcat(dirnamep,temp);
        dir = opendir(dirnamep);
           if(dir != NULL)
             maxcores++;
        } while(dir != NULL);

        closedir(dir);

  return maxcores-1;
}


/*get node id of a cpu/core
return: 0 if UMA 
*/
int ma_linux_get_nodeid()
{
   	int             cpu,i;
        int             node=-1;
        char            dirnamep[256];
        char            cpuid[7],temp[33];
        struct dirent   *dirent;
        DIR             *dir;
        int maxnodes;
        maxnodes  =  ma_linux_get_nnodes();
        cpu = sched_getcpu();
      
      if( maxnodes > 1){  
	  temp[0]=cpuid[0]=dirnamep[0]='\0';
          strcpy(cpuid,"cpu");
          sprintf(temp, "%i", cpu);
          strcat(cpuid,temp);
          strcat(cpuid,"\0");


        for(i=0;i<maxnodes && node == -1;i++){
        strcpy(dirnamep,"/sys/devices/system/node/node");
        temp[0]='\0';
        sprintf(temp, "%i", i);
        strcat(dirnamep,temp);
        dir = opendir(dirnamep);
        if (dir == NULL) {
                return 0;
        }
        while ((dirent = readdir(dir)) != 0) {
                if (strcmp(cpuid, dirent->d_name)==0) {
                        node = i;
                        break;
                }
        }
        closedir(dir);
     }
    }
    else
      node = 0;

  return node;
}

/*
* Set the memory policy for data allocation for a process
*/
void ma_linux_set_mempol(int mempol)
{

  int node = numa_max_node();
  int core, i;
  unsigned long mask;

  switch(mempol)
  {
   case OS:
    if(node > 0)
      set_mempolicy(MPOL_DEFAULT,NULL,0); 
   break;
   case LOCAL:   
    if(node >0)
    {  
     core = sched_getcpu();
     node = linux_get_nodeid(core); 
     mask = 1 << node;
     set_mempolicy(MPOL_BIND,&mask,sizeof(unsigned long)+1);
    }
   break;
   case INTERLEAVE:
    if(node >0){
      for(i=0;i<node+1;i++)
        mask = mask + (1 << i);
        set_mempolicy(MPOL_INTERLEAVE,&mask,sizeof(mask)+1);      
    }
   break;
  }
}

/*
* Get the memory policy of a process
*/
void ma_linux_get_mempol()
{
  int node, mem_pol;
  unsigned long *nset;
  unsigned long addr;
  int mempol=-1,nnodes,i;
 
  nset = calloc((1+(8*sizeof(unsigned long)-1))/((8 * sizeof(unsigned long))), sizeof(unsigned long));
    
  nnodes = ma_linux_get_nnodes();

 if (nnodes != 0 ){
  node = 0;

  get_mempolicy (&mempol,nset,8*sizeof(nset)+1,0,0); 

  switch(mempol)
  {
   case MPOL_DEFAULT:
    printf("Default memory policy\n");
   break;
   case MPOL_BIND:
    printf("Bind memory policy\n");    
     for(i=0; i< nnodes;i++)
       if(mask_isset(nset,sizeof(nset),i))
          node = i;
    printf("Node used #%d\n");   
   break;
   case MPOL_INTERLEAVE:
    printf("Interleave memory policy\n");
    printf("All nodes are used\n");    
   break;
   default:
    printf("Default memory policy\n");     
   break;
  }
 }
 else
    printf("Default memory policy\n");
}

