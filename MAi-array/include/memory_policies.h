/*=====================================================================================
  MAi is a interface to manage memory policy in NUMA architectures

  (C) Copyright 2010 INRIA 
  Projet: MESCAL / ANR NUMASIS

  Author: Christiane Pousa Ribeiro

  The author may be reached at pousa@imag.fr
 *====================================================================================*/

#ifndef __MEMORY_POLICIES_H
#define __MEMORY_POLICIES_H

#ifdef __cplusplus
extern "C" {
#endif

/*Interleave memory policy - function prototypes*/
void mai_cyclic(void *p);
void mai_cyclic_possible(void *p);
void mai_skew_mapp(void *p);
void mai_prime_mapp(void *p,int prime);
void mai_cyclic_rows(void *p,int nr);
void mai_skew_mapp_rows(void *p,int nr);
void mai_prime_mapp_rows(void *p,int prime,int nr);
void mai_cyclic_columns(void *p,int nc);
void mai_skew_mapp_columns(void *p,int nc);
void mai_prime_mapp_columns(void *p,int prime,int nc);
void mai_cyclic_3D(void *p,int n3d);
void mai_skew_mapp_3D(void *p,int n3d);
void mai_prime_mapp_3D(void *p,int prime,int n3d);

/*Regular Bind memory policy - function prototypes*/
void mai_bind_all(void *p);
void mai_bind_all_mynode(void *p);
void mai_bind_rows(void *p);
void mai_bind_columns(void *p);
void mai_bind_rows_mynode(void *p);
void mai_bind_columns_mynode(void *p);
void mai_bind_3D(void *p);

/*Irregular Bind memory policy - function prototypes*/
void mai_ibind_rows(void *p,int nr);
void mai_ibind_columns(void *p,int nc);
void mai_ibind_3D(void *p,int n3d);

/*Next-touch - function prototype*/
void mai_next_touch(void *p);
void mai_handler(int sig, siginfo_t *info, void *_context);

/*Interleave memory policy - function prototypes*/
void mai_cyclic_sn(void *p,int nnodes,int *nodes);
void mai_skew_mapp_sn(void *p,int nnodes,int *nodes);
void mai_prime_mapp_sn(void *p,int prime,int nnodes,int *nodes);
void mai_cyclic_rows_sn(void *p,int nr,int nnodes,int *nodes);
void mai_skew_mapp_rows_sn(void *p,int nr,int nnodes,int *nodes);
void mai_prime_mapp_rows_sn(void *p,int prime,int nr,int nnodes,int *nodes);
void mai_cyclic_columns_sn(void *p,int nc,int nnodes,int *nodes);
void mai_skew_mapp_columns_sn(void *p,int nc,int nnodes,int *nodes);
void mai_prime_mapp_columns_sn(void *p,int prime,int nc,int nnodes,int *nodes);
void mai_cyclic_3D_sn(void *p,int n3d,int nnodes,int *nodes);
void mai_skew_mapp_3D_sn(void *p,int n3d,int nnodes,int *nodes);
void mai_prime_mapp_3D_sn(void *p,int prime,int n3d,int nnodes,int *nodes);

/*Regular Bind memory policy - function prototypes*/
void mai_bind_all_sn(void *p,int nnodes,int *nodes);
void mai_bind_rows_sn(void *p,int nnodes,int *nodes);
void mai_bind_columns_sn(void *p,int nnodes,int *nodes);
void mai_bind_3D_sn(void *p,int nnodes,int *nodes);

/*Irregular Bind memory policy - function prototypes*/
void mai_ibind_rows_sn(void *p,int nr,int nnodes,int *nodes);
void mai_ibind_columns_sn(void *p,int nc,int nnodes,int *nodes);
void mai_ibind_3D_sn(void *p,int n3d,int nnodes,int *nodes);

/*Migration - function prototypes*/
void mai_migrate(void *p,unsigned long node,int nr, int startr,int nc, int startc);
void mai_migrate_rows(void *p,unsigned long node,int nr, int start);
void mai_migrate_columns(void *p,unsigned long node,int nc, int start);
void mai_migrate_scatter(void *p,unsigned long nodes);
void mai_migrate_gather(void *p,unsigned long node);

#ifdef __cplusplus
}
#endif

#endif
