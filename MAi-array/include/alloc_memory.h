/*=====================================================================================
  MAi is a interface to manage memory policy in NUMA architectures

  (C) Copyright 2010 INRIA 
  Projet: MESCAL / ANR NUMASIS

  Author: Christiane Pousa Ribeiro

  The author may be reached at pousa@imag.fr
 *====================================================================================*/

#ifndef __ALLOC_MEMORY_H
#define __ALLOC_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif
struct mytype
{
  double r;
  double i;
};


/*Alloc arrays - function prototype */
void* mai_alloc_1D(int nx,size_t size_item,int type);
void* mai_alloc_2D(int nx,int ny,size_t size_item, int type);
void* mai_alloc_3D(int nx,int ny,int nz,size_t size_item, int type);

/*Free allocated arrays*/
void mai_free_array(void *p);

/*mai_alloc statistics*/
mai_stats mai_allocinfo(); 

#ifdef __cplusplus
}
#endif

#endif

