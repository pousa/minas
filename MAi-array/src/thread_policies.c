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
#include <sched.h>
#include <omp.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>


#include "common.h"
#include "numArch.h"
#include "thread_policies.h"

//Number of threads to register
static int nThreads=0;

/*
Set the total number of threads
parameters: total number of threads
return: none
*/
void mai_set_number_threads(int nthreads)
{
  pthread_mutex_lock(&(sinfo.lock_place)); 
  sinfo.nthreads = nthreads;
  pthread_mutex_unlock(&(sinfo.lock_place)); 
}


/*
Find number of threads - executed by master thread
parameters: process id
return: number of threads
*/
int get_number_threads()
{
 char command[120];
 command[0]='\0';
 char temp[10];
 int nthreads=0,ID=getpid();
 FILE *fp;

 sprintf(temp,"%i",ID);
 strcpy(command,"ps -eLf|grep ");
 strcat(command,temp);
 strcat(command,"|grep -v grep |wc -l");
// printf("\n %s\n",command);

 fp=popen(command,"r");
 fscanf(fp,"%d",&nthreads);
 fclose(fp);
#ifdef DEBUG 
 printf("\n %d\n",nthreads);
#endif
 
 if(nthreads <= 1)
  return 0;
 else 
  return nthreads; 
}


/*
Find number of threads - executed by master thread
parameters: process id
return: number of threads
*/
int get_num_threads_proc(int ID)
{
 char command[120];
 command[0]='\0';
 char temp[10];
 int nthreads=0;
 FILE *fp;

 sprintf(temp,"%i",ID);
 strcpy(command,"ps -eLf|grep ");
 strcat(command,temp);
 strcat(command,"|grep -v grep |wc -l");
// printf("\n %s\n",command);

 fp=popen(command,"r");
 fscanf(fp,"%d",&nthreads);
 fclose(fp);
#ifdef DEBUG 
 printf("\n %d\n",nthreads);
#endif

 if(nthreads <= 1)
  return 0;
 else
  return nthreads;
}


/*
Set the threads OMP Id in the library vector
parameters: none
return: none
*/
void set_thread_id_omp()
{
  pthread_mutex_lock(&(mai_mutex)); 
  thread_id[omp_get_thread_num()] = tid();
  nThreads++;
  pthread_mutex_unlock(&(mai_mutex)); 
}

/*
Set the threads Id in the library vector
parameters: none
return: none
*/
void mai_set_thread_id_posix()
{
  pthread_mutex_lock(&(mai_mutex)); 
  thread_id[nThreads] = tid();
  nThreads++; 
  pthread_mutex_unlock(&(mai_mutex)); 
}


/*
Set the threads Id in the library vector
parameters: none
return: cpu/core
*/
int get_thread_cpu()
{
  int idxcpu=0;

  idxcpu = sched_getcpu();		
 
  return idxcpu;
}



/*
Show the threads id - executed by master thread
parameters: none
return: none
*/
void mai_show_thread_ids()
{
  int i;
  unsigned int pid ;
  
  printf("\n----------- THREADS-------");
  for(i=0;i<sinfo.nthreads;i++)
    {
      pid = thread_id[i];
      printf("\n Thread id: %d",pid);
    }
  printf("\n---------------------------\n");
       
}


/*
Bind all threads to specified cpus/cores
parameters: none
return: none
*/
void bind_threads()
{
  int i,err;
  unsigned long cpu;
  unsigned int pid ;
  
if(sinfo.nthreads!=0){
  for(i=0;i<sinfo.nthreads;i++)
    {
      pid = thread_id[i];
      cpu = sinfo.cpus[i];
//      printf("\n i %d Thread id: %d core %d\n\n\n",i,pid,cpu);      
      err=sched_setaffinity(pid,CPU_MASK_SIZE,&cpu);
  
      if(err<0){
	identify_error();
        printf("\t Erro: bind_threads!");
      }
    }  
  }
}

/*
Bind a omp thread to specified cpus/cores
parameters: none
return: none
*/
void bind_thread_omp()
{
  unsigned long cpu;
  int err;
      
     if(sinfo.nthreads!=0)
      cpu = sinfo.cpus[omp_get_thread_num()];
     else
       cpu = sinfo.cpus[omp_get_thread_num()%sinfo.ncpus];
      err=sched_setaffinity(0,CPU_MASK_SIZE,&cpu);
  
      if(err<0){
	identify_error();
        printf("\t Erro: bind_thread_omp!");
      }   
}

/*
Bind a thread to specified cpus/cores
parameters: none
return: none
*/
void bind_thread_posix(pid_t tid)
{
  unsigned long cpu;
  int err;
     
      cpu = sinfo.cpus[tid%sinfo.ncpus];
      err=sched_setaffinity(tid,CPU_MASK_SIZE,&cpu);
  
      if(err<0){
	identify_error();
        printf("\t Erro: bind_thread_posix!");
      }
      
}

/*
Migrate a thread to a new cpu/core
parameters: id-Thread Id, cpu-Cpu/core Id
return: none
*/
void mai_migrate_thread(pid_t id,unsigned long cpu)
{
  int i,err,find=0;
  double TimeI;

 pthread_mutex_lock(&(sinfo.lock_place));
  TimeI = mai_my_second();   
  
  err=sched_setaffinity(id,MASK_SIZE,&cpu);

#ifdef OMP
  for(i=0;i<sinfo.nthreads && !find;i++)
    {
      if(id==thread_id[i])
	{
	  sinfo.cpus[i]=cpu;
	  find = 1;
	}  
    }
#endif

#ifdef POSIX
  for(i=0;i<sinfo.nthreads && !find;i++)
    {
      if(id==thread_id[i])
	{
	  sinfo.cpus[i%sinfo.ncpus]=cpu;
	  find = 1;
	}  
    }
#endif

  time_last_tmig = mai_my_second() - TimeI;
 pthread_mutex_unlock(&(sinfo.lock_place));

  if(err<0){
      identify_error(); 
      printf("\t Erro: migrate_threads!");
  }
}


