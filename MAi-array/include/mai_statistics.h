/*=====================================================================================
  MAi is a interface to manage memory policy in NUMA architectures

  (C) Copyright 2010 INRIA 
  Projet: MESCAL / ANR NUMASIS

  Author: Christiane Pousa Ribeiro

  The author may be reached at pousa@imag.fr
 *====================================================================================*/
#ifndef __STATISTICS_H
#define __STATISTICS_H

#ifdef __cplusplus
extern "C" {
#endif

/*Page information - function prototype*/
void mai_print_pagenodes(unsigned long *pageaddrs,int size);
int mai_number_page_migration(unsigned long *pageaddrs,int size);
double mai_get_time_pmigration();

/*Thread information - function prototype*/
void mai_print_threadcpus();
int mai_number_thread_migration(unsigned int *threads,int size);
double mai_get_time_tmigration();

/*Memory placement information*/
void mai_get_log();
void mai_mempol(void* ph);
void mai_arraynodes(void* ph);

#ifdef __cplusplus
}
#endif

#endif
