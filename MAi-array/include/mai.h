/*=====================================================================================
  MAi is a interface to manage memory policy in NUMA architectures

  (C) Copyright 2010 INRIA 
  Projet: MESCAL / ANR NUMASIS

  Author: Christiane Pousa Ribeiro

  The author may be reached at pousa@imag.fr
 *====================================================================================*/

#ifndef __MAI_H
#define __MAI_H

#ifdef __cplusplus
extern "C" {
#endif


#include "common.h"
#include "memory_policies.h"
#include "thread_policies.h"
#include "mai_statistics.h"
#include "alloc_memory.h"
#include "../../numArch/include/numArch.h"
#include "own_mempolicies.h"

void mai_barrier();

/*get struct place_info data*/
int mai_get_num_nodes();
int mai_get_num_threads();
int mai_get_num_cpu(); 
unsigned long* mai_get_nodes_id();
unsigned long* mai_get_cpus_id();

/*show nodes and cpus in use*/
void mai_show_nodes();
void mai_show_cpus();

/*check the system*/
int numa_check();

/*initialize the library*/
void mai_init(char filename[]);

/*finilize the library*/
void mai_final();

/*change cpus and nodes information*/
void mai_change(char filename[]);

#ifdef __cplusplus
}
#endif

#endif
