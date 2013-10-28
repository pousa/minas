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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <numaif.h>
#include <numa.h>

#include <sys/types.h>
#include <dirent.h>

#include "common.h"
#include "thread_policies.h"
#include "mai_statistics.h"

/*
Print the nodes where pages are.
parameters: pageaddrs-pages address, size-number of pages
return: none
*/
void mai_print_pagenodes(unsigned long *pageaddrs,int size)
{
  int pagenodes[MAXPAGES];
  int i;
  int err;
   
  
  #ifdef NR_move_pages
  err = syscall(__NR_move_pages, 0, size, pageaddrs, NULL, pagenodes, 0); 
  if (err < 0) {
    perror("Error (print_pagenodes): Invalide pages");
    exit(-1);
  }

  for(i=0; i<size; i++) {
    if (pagenodes[i] == -ENOENT)
      printf("  page #%d is not allocated\n", i);
    else
      printf("  page #%d is on node #%d\n", i, pagenodes[i]);
  }
 #else
  printf("Must update Linux Kernel to get this information\n");  
 #endif
}


/*
Get the current time
parameters: none
return: time in sec
*/
double mai_get_time_pmigration()
{
  return time_last_pmig;
}


/*
Get the current time - executed by master thread
parameters: none
return: time in sec
*/
void mai_print_threadcpus()
{
  int i;

  for(i=0;i<sinfo.nthreads;i++)
    printf("\nThread id %d - CPU/CORE %d",thread_id[i],sinfo.cpus[i]);
}


/*
Get the current time
parameters: none
return: time in sec
*/
double mai_get_time_tmigration()
{
 return time_last_tmig;
}


/*
Get log for the application
parameters: none
return: none
*/
void mai_get_log()
{
  char temp[10],dir[100];
  char command[100],filename[100];
  char command1[] = "numactl --show > log_numactl.txt";
  unsigned int pid = getpid();
  int resp;
  DIR *d;
  FILE *fp,*fpf;

  command[0]='\0';

  fp=popen("pwd","r");
  fscanf(fp,"%s",dir);
  fclose(fp);

#ifdef DEBUG
  printf("%s\n",dir);
#endif

  sprintf(temp,"%i",pid);

  strcpy(filename,"/proc/");
  strcat(filename,temp);
  strcat(filename,"/numa_maps");

  fpf = fopen(filename,"r");
  if(fpf!=NULL){

  strcat(command,"cp /proc/");
  strcat(command,temp);
  strcat(command,"/numa_maps ");
  strcat(command,dir);
  
#ifdef DEBUG
  printf("%s\n",command1);
#endif

  fp=popen(command,"r");
  fscanf(fp,"%d",&resp);
  fclose(fp);
  rename("numa_maps","log_numamaps.txt");
  }

#ifdef DEBUG
  printf("%s\n",command1);
#endif
  fp=popen(command1,"r");
  fscanf(fp,"%d",&resp);
  fclose(fp);

  strcpy(command1,"numactl --hardware >> log_numactl.txt");
#ifdef DEBUG
  printf("%s\n",command1);
#endif
  fp=popen(command1,"r");
  fscanf(fp,"%d",&resp);
  fclose(fp);

}


/*
Get memory policy of an array
parameters: array
return: none
*/
void mai_mempol(void* ph)
{
  int mpol=-1;
  
  pthread_mutex_lock(&mai_mutex); 
  mpol = get_var_policy(ph);
  pthread_mutex_unlock(&mai_mutex); 

  switch(mpol)
  {
   case CYCLIC:
    printf("CYCLIC\n");
   break;
   case CYCLIC_B:
    printf("CYCLIC_BLOCK\n");
   break;
   case SKEW:
    printf("SKEW_MAPP\n");
   break;
   case SKEW_B:
    printf("SKEW_MAPP_BLOCK\n");
   break;
   case PRIME:
    printf("PRIME_MAPP\n");
   break;
   case PRIME_B:
    printf("PRIME_MAPP_BLOCK\n");
   break;
   case BIND:
    printf("BIND\n");
   break;
   case BIND_B:
    printf("BIND_BLOCK\n");
   break;
   case RANDOM:
    printf("RANDOM\n");
   break;
   case RANDOM_B:
    printf("RANDOM_BLOCK\n");
   break;
   case OWN:
    printf("OWN\n");
   break;
   default:
    printf("NO MEM POLICY\n");
   break;
  }
}

/*
Get nodes of an array
parameters: array
return: none
*/
void mai_arraynodes(void* ph)
{
 int nnodes=0,*nodes=NULL,i;

 pthread_mutex_lock(&mai_mutex); 
 nodes = get_var_nodes(ph,&nnodes);
 pthread_mutex_unlock(&mai_mutex);  

 if(nodes!=NULL){
  printf("\n");
  for(i=0;i<nnodes;i++);
   printf("Node #%d ",nodes[i]);
  printf("\n");
 }
}

