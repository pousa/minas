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
#include <numaif.h>
#include <numa.h>

#include "common.h"
#include "alloc_memory.h"

static mai_stats mai_info;

/*mai_alloc statistics*/
mai_stats mai_allocinfo()
{
  return mai_info;  
}

/*Alloc arrays of one dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void* mai_alloc_1D(int nx,size_t size_item,int type)
{
  void *t=NULL;
  unsigned long node=0,freemem=0;
  int dimensions[4]={0,0,0,0};//to save array dimmensions
  size_t size;
  int err;
  struct var_info* new_item;
  size = alignmem(nx*size_item);  

  new_item = create_var_info();

  pthread_mutex_lock(&(new_item->lock_var));
 
  t = mmap(0,(size_t)size,PROT_WRITE|PROT_READ,
	       MAP_PRIVATE|MAP_ANONYMOUS,-1,0);

  dimensions[0]=1;
  dimensions[1]=nx;

  set_var_info(t,NULL,1,dimensions,size_item,(size_t)size,type,new_item);

  pthread_mutex_unlock(&(new_item->lock_var));     

  pthread_mutex_lock(&(mai_mutex));
  mai_info.used_size = mai_info.used_size + size;
  mai_info.nmmaps = mai_info.nmmaps + 1;
  mai_info.block_h = mai_info.block_h + sizeof(struct var_info);
  mai_info.npinfo = mai_info.npinfo + (size/PAGE_SIZE);
  pthread_mutex_unlock(&(mai_mutex));
 
  if (t == MAP_FAILED)
  {
    printf("\nError (mai_alloc_1D): Bad allocation!");
    exit(1);
  }  
 
  return t;
}

/*Alloc arrays of two dimensions 
  parameters: nx-number of elements in x direction, ny-number of elementes 
              in y direction, type-C primitive type or a struct
  return: void pointer
*/
void* mai_alloc_2D(int nx,int ny,size_t size_item,int type)
{
  void **t=NULL,*p=NULL;
  int i;
  int dimensions[4]={0,0,0,0};//to save array dimmensions
  size_t size;
  struct var_info* new_item;  

  size = alignmem((nx*ny*size_item)+(nx*sizeof(void*))); 

  new_item = create_var_info();

  pthread_mutex_lock(&(new_item->lock_var));
  p = mmap(0,(size_t)(size),PROT_WRITE|PROT_READ,
	   MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
  t = p;
  t[0] = p + ((size_t)(nx*sizeof(void*)));

  for(i=1;i<nx;i++)
    t[i]= t[i-1]+(ny*size_item);
  
  dimensions[0]=nx; dimensions[1]=ny;
   
  set_var_info(t,t[0],2,dimensions,size_item,(size_t)(size),type,new_item);

  pthread_mutex_unlock(&(new_item->lock_var));       
 
  pthread_mutex_lock(&(mai_mutex));
  mai_info.used_size = mai_info.used_size + (size);
  mai_info.nmmaps = mai_info.nmmaps + 1;
  mai_info.block_h = mai_info.block_h + sizeof(struct var_info);
  mai_info.npinfo = mai_info.npinfo + ((size)/PAGE_SIZE);
  pthread_mutex_unlock(&(mai_mutex));

  if (t == MAP_FAILED){
    printf("\n Error (mai_alloc_2D): Bad allocation!");
    exit(1);
 }
  return t;
}

/*Alloc arrays of three dimensions 
  parameters: nx-number of elements in x direction, ny-number of elementes 
              in y direction, nz-number of elementes in z direction 
	      type-C primitive type or a struct
  return: void pointer
*/
void *mai_alloc_3D(int nx,int ny,int nz,size_t size_item,int type)
{

  void *t;
  struct mytype ***ts;
  char ***ta;
  float ***tf;
  double ***td;
  int ***ti,i,j;
  size_t t_size,size,size2d,size3d;
  int dimensions[4]={0,0,0,0};//to save array dimmensions
  struct var_info* new_item;

  size3d = (nx*ny*nz*size_item); 
  size = (nx*sizeof(void**));
  size2d = (nx*ny*sizeof(void*));
  t_size = alignmem(size+size2d+size3d);

  new_item = create_var_info();
  pthread_mutex_lock(&(new_item->lock_var));  
  t = mmap(0,(size_t)(t_size),PROT_WRITE|PROT_READ,
                      MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
  pthread_mutex_unlock(&(new_item->lock_var));  

  pthread_mutex_lock(&(mai_mutex));
  mai_info.used_size = mai_info.used_size + (t_size);
  mai_info.nmmaps = mai_info.nmmaps + 1;
  mai_info.block_h = mai_info.block_h + sizeof(struct var_info);
  mai_info.npinfo = mai_info.npinfo + ((t_size)/PAGE_SIZE);
  pthread_mutex_unlock(&(mai_mutex));

  switch(type)
  {
    case CHAR:
      ta= t;
      ta[0]= t + size;
      ta[0][0]= t + size + size2d;

      for(i=1;i<nx;i++)
        {
              ta[i]= ta[i-1]+ny;
              ta[i][0]= ta[i-1][0]+ny*nz;
              for(j=1;j<ny;j++){
	        ta[0][j]= ta[0][j-1]+nz;
                ta[i][j]= ta[i][j-1]+nz;}
        }
      t=ta;
      dimensions[0]=nx; dimensions[1]=ny; dimensions[2]=nz;
      pthread_mutex_lock(&(new_item->lock_var));  
      set_var_info(ta,ta[0][0],3,dimensions,size_item,(t_size),type,new_item);
    break;
    case DOUBLE:
      td = t;
      td[0] = t + size;
      td[0][0] = t + size + size2d;

      for(i=1;i<nx;i++)
        {
              td[i]= td[i-1]+ny;
              td[i][0]= td[i-1][0]+ny*nz;
              for(j=1;j<ny;j++){
	        td[0][j]= td[0][j-1]+nz;
                td[i][j]= td[i][j-1]+nz;}
        }	

      t=td;
      dimensions[0]=nx; dimensions[1]=ny; dimensions[2]=nz;
      pthread_mutex_lock(&(new_item->lock_var));   
      set_var_info(td,td[0][0],3,dimensions,size_item,(t_size),type,new_item);
    break;
    case FLOAT:
      tf= t;
      tf[0]= t + size; 
      tf[0][0]= t + size + size2d;

      for(i=1;i<nx;i++)
	{
              tf[i]= tf[i-1]+ny;
              tf[i][0]= tf[i-1][0]+ny*nz;
              for(j=1;j<ny;j++){
	         tf[0][j]= tf[0][j-1]+nz;
                 tf[i][j]= tf[i][j-1]+nz;}
        }
      t=tf;
      dimensions[0]=nx; dimensions[1]=ny; dimensions[2]=nz;
      pthread_mutex_lock(&(new_item->lock_var));  
      set_var_info(tf,tf[0][0],3,dimensions,size_item,(t_size),type,new_item);
    break;
    case INT:
      ti= t;
      ti[0]= t + size; 
      ti[0][0]= t + size + size2d;

      for(i=1;i<nx;i++)
        {
              ti[i]= ti[i-1]+ny;
              ti[i][0]= ti[i-1][0]+ny*nz;
              for(j=1;j<ny;j++){
		 ti[0][j]= ti[0][j-1]+nz;
                 ti[i][j]= ti[i][j-1]+nz;}
        }
      t=ti;
      dimensions[0]=nx; dimensions[1]=ny; dimensions[2]=nz;
      pthread_mutex_lock(&(new_item->lock_var));   
      set_var_info(ti,ti[0][0],3,dimensions,size_item,(t_size),type,new_item);
    break;
  case STRUCT:
      ts= t;
      ts[0]= t + size; 
      ts[0][0]= t + size + size2d;
 
      for(i=1;i<nx;i++) 
	{
	      ts[i] = ts[i-1]+ny;		    
	      ts[i][0]= ts[i-1][0]+(ny*nz);
	      for(j=1;j<ny;j++) {
		ts[0][j]= ts[0][j-1]+nz;
		ts[i][j]= ts[i][j-1]+nz; 
		}
	}     
    t=ts;
    dimensions[0]=nx; dimensions[1]=ny; dimensions[2]=nz;
    pthread_mutex_lock(&(new_item->lock_var));  
    set_var_info(ts,ts[0][0],3,dimensions,size_item,(t_size),type,new_item);
    break;
  }

 pthread_mutex_unlock(&(new_item->lock_var));  

  if (t == MAP_FAILED){
    printf("\n Error (mai_alloc_3D): Bad allocation!");
    exit(1);
  } 
return (t);
}



/*Free allocated arrays
  parameters: pointer to the array
  return: none
*/
void mai_free_array(void *p)
{
  int err,nitems;
  unsigned long lenght;

  err = munmap(p,get_var_tsize(p)); 

  if(err < 0)
   identify_error();
}
