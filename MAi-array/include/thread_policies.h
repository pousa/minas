/*=====================================================================================
  MAi is a interface to manage memory policy in NUMA architectures

  (C) Copyright 2010 INRIA 
  Projet: MESCAL / ANR NUMASIS

  Author: Christiane Pousa Ribeiro

  The author may be reached at pousa@imag.fr
 *====================================================================================*/

#ifndef __THREAD_POLICIES_H
#define __THREAD_POLICIES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include "common.h"

unsigned int thread_id[MAXTHREADS];

/*Get the number of threads*/
int get_number_threads();
int get_thread_cpu();

/*Set the number of threads*/
void mai_set_number_threads(int nthreads);

/*Set threads id - function prototype */
void set_thread_id_omp();
void mai_set_thread_id_posix();
void mai_show_thread_ids();

/*Bind Threads - function prototype */
void bind_threads();
void mai_bind_thread_posix(pid_t tid);
void bind_thread_omp();

/*Migrate Thread - function prototype */
void mai_migrate_thread(pid_t id,unsigned long cpu);

#ifdef __cplusplus
}
#endif

#endif
