/*=====================================================================================
  MAi is an interface to manage memory policy in NUMA architectures

  (C) Copyright 2010 INRIA 
  Projet: MESCAL / ANR NUMASIS

  Author: Christiane Pousa Ribeiro

  The author may be reached at pousa@imag.fr
 *====================================================================================*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <math.h> 
#include <sys/syscall.h>

#include "mai.h"
pthread_mutex_t barrier;  /* mutex semaphore for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numArrivedThreads = 0;       /* count of the number who have arrived */


void mai_barrier() {
  pthread_mutex_lock(&barrier);
  numArrivedThreads++;
  if (numArrivedThreads == sinfo.nthreads) {
    numArrivedThreads = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

/*
Get struct place_info data
parameters: none
return: number of nodes
*/
int mai_get_num_nodes()
{
  return sinfo.nnodes;
}

/*
Get struct place_info data
parameters: none
return: number of threads
*/
int mai_get_num_threads()
{
  return sinfo.nthreads;
}

/*
Get struct place_info data
parameters: none
return: number of threads
*/
int mai_get_num_cpus()
{
  return sinfo.ncpus;
}

/*
Get struct place_info data
parameters: none
return: nodes id
*/
unsigned long* mai_get_nodes_id()
{
   return sinfo.nodes;    
}

/*
Get struct place_info data 
parameters: none
return: cpus id
*/
unsigned long* mai_get_cpus_id()
{
  return sinfo.cpus;
}

/*
Show nodes used to place memory - executed by master thread
parameters: none
return: none
*/
void mai_show_nodes()
{
  int i,tmp;
  unsigned long x; 

  printf("\n---Listing nodes----");

  for(i=0;i<sinfo.nnodes;i++)
    {
      x = sinfo.nodes[i];   
      tmp = convert_node(&x); 
      printf("\nNODE: %d",tmp); 
    }
    
  printf("\n--------------------\n");
}

/*
Show cpus used to place memory  - executed by master thread
parameters: none
return: none
*/
void mai_show_cpus()
{
  int i,tmp;
  unsigned long x;

  printf("\n---Listing cpus/cores----\n");
   
  for(i=0;i<sinfo.nthreads;i++)
    {
      x = sinfo.cpus[i];
      tmp = convert_cpu(&x);
      printf("\nCPU/CORE: %d",tmp); 
    }

  printf("\n-------------------------\n");
}

/*
Check the system instalation - executed by master thread
parameters: none
return: <0 if system has not libnuma or migration support.
        =0 if the system is ok.
*/
int numa_check()
{
 int ret=0;

 ret = get_mempolicy(NULL, NULL, 0, 0, 0);

 if (ret < 0 && errno == ENOSYS)
    fprintf(stderr,"upgrade to kernel >= 2.6.7\n");

 return ret;

}


/*
Initialize the environment - executed by master thread
parameters: filename-the name of the configuration file
return: none
*/
void mai_init(char filename[])
{ 

 if( na_is_numa())  {
 
  /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  pthread_mutex_init(&(sinfo.lock_place), NULL);
  pthread_mutex_init(&mai_mutex,NULL);
  init_var_info();
  init_place_info();
  load_hash();  

  main_pid = tid();

  //get environment information  
  set_place_info(filename);

  time_last_pmig = time_last_tmig = 0.0;
#ifdef OMP_FIX
   if(sinfo.nthreads > 0 && sinfo.nthreads < sinfo.ncpus )
    { 
      omp_set_num_threads(sinfo.nthreads);
     
      //create and set threads ID
      #pragma omp parallel default(shared)
      {
        #pragma omp critical
	{
	 set_thread_id_omp();
	}  
      } 
      bind_threads();
    }
#endif   
  }
  else
    exit(-1); 
 
}

/*
Destroy the environment - executed by master thread
parameters: none
return: none
*/
void mai_final()
{
  struct var_info *aux;
  void *pointer=NULL;
  int i=0;

 if(numa_check() == 0)
 { 
  //look for the right array
  while(i<MAI_HASH_SIZE)
  { 
   aux = arraysHash[i];
    while(aux!=NULL)
    {
       pthread_mutex_destroy(&(aux->lock_var));
       mai_free_array(aux->phigh);
       aux=aux->child;
    }
   arraysHash[i]=NULL;
   i++;
  }
 }
 else
   exit(-1); 

 pthread_mutex_destroy(&(sinfo.lock_place));
 pthread_mutex_destroy(&(mai_mutex));
}

/*
Change the environment - executed by master thread
parameters: filename-the name of the configuration file
return: none
*/
void mai_change(char filename[])
{

 if(numa_check() == 0)
 {
 
  sinfo.nnodes=sinfo.nthreads=0;
  sinfo.cpus=sinfo.nodes=NULL;

  //get environment information
  set_place_info(filename);

  time_last_pmig = time_last_tmig = 0.0;
 }
 else
   exit(-1);
}
