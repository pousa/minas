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
#include <stdlib.h>
#include <unistd.h>
#include <numaif.h>
#include <numa.h>
#include <time.h>
#include <string.h>

#include "common.h"
#include "mai.h"
#include "memory_policies.h"
#include "thread_policies.h"

static int mai_sigsegv_handler_set = 0;
static size_t block_size=0;

void c_1dreal_f_(int *carrayref, float *carray, int *nx);
void c_1dlreal_f_(int *carrayref, float *carray, int *nx);
void c_1dint_f_(int *carrayref, float *carray, int *nx);
void c_1dbyte_f_(int *carrayref, float *carray, int *nx);
void c_1dcomplex_f_(int *carrayref, float *carray, int *nx);

void c_2dreal_f_(int *carrayref, float *carray, int *nx, int *ny);
void c_2dlreal_f_(int *carrayref, float *carray, int *nx, int *ny);
void c_2dbyte_f_(int *carrayref, float *carray, int *nx, int *ny);
void c_2dint_f_(int *carrayref, float *carray, int *nx, int *ny);
void c_2dcomplex_f_(int *carrayref, float *carray, int *nx, int *ny);

void c_3dreal_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz);
void c_3dlreal_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz);
void c_3dbyte_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz);
void c_3dint_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz);
void c_3dcomplex_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz);

void c_4dreal_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz,int *nk);
void c_4dlreal_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz,int *nk);
void c_4dbyte_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz,int *nk);
void c_4dint_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz,int *nk);
void c_4dcomplex_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz,int *nk);

void c_5dreal_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz,int *nk,int *nl);
void c_5dlreal_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz,int *nk,int *nl);
void c_5dbyte_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz,int *nk,int *nl);
void c_5dint_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz,int *nk,int *nl);
void c_5dcomplex_f_(int *carrayref, float *carray, int *nx, int *ny,int *nz,int *nk,int *nl);


/*Alloc arrays of one dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_1d_real_(int *carrayref,int *nx){
	int nitems = *nx;
        float *array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitems*sizeof(float));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (float *) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 1D real array\n");
	} 

      dimensions[0]=1;
      dimensions[1]=nx;
      set_var_info_(array,array,1,dimensions,sizeof(float),size,FLOAT);

      printf("%p\n",array);

      pthread_mutex_unlock(&(mai_mutex));     

      c_1dreal_f_(carrayref,array,nx);
}

/*Alloc arrays of one dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_1d_lreal_(int *carrayref,int *nx){
	int nitems = *nx;
        double *array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitems*sizeof(double));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (double *) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 1D long real array\n");
	} 

      dimensions[0]=1;
      dimensions[1]=nx;
      set_var_info_(array,array,1,dimensions,sizeof(double),size,DOUBLE);

      pthread_mutex_unlock(&(mai_mutex));     

      c_1dlreal_f_(carrayref,array,nx);
}

/*Alloc arrays of one dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_1d_int_(int *carrayref,int *nx){
	int nitems = *nx;
        int *array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitems*sizeof(int));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (int *) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 1D integer array\n");
	} 

      dimensions[0]=1;
      dimensions[1]=nx;
      set_var_info_(array,array,1,dimensions,sizeof(int),size,INT);

      pthread_mutex_unlock(&(mai_mutex));     

      c_1dint_f_(carrayref,array,nx);
}

/*Alloc arrays of one dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_1d_byte_(int *carrayref,int *nx){
	int nitems = *nx;
        char *array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitems*sizeof(char));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (char *) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 1D byte array\n");
	} 

      dimensions[0]=1;
      dimensions[1]=nx;
      set_var_info_(array,array,1,dimensions,sizeof(char),size,CHAR);

      pthread_mutex_unlock(&(mai_mutex));     

      c_1dbyte_f_(carrayref,array,nx);
}

/*Alloc arrays of one dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_1d_complex_(int *carrayref,int *nx){
	int nitems = *nx;
        float *array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitems*2*sizeof(float));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (float *) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 1D complex array\n");
	} 

      dimensions[0]=1;
      dimensions[1]=nx;
      set_var_info_(array,array,1,dimensions,2*sizeof(float),size,STRUCT);

      pthread_mutex_unlock(&(mai_mutex));     

      c_1dcomplex_f_(carrayref,array,nx);
}

/*Alloc arrays of two dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_2d_real_(int *carrayref,int *nx,int *ny){
	int nitemsx = *nx;
        int nitemsy = *ny;
        float **array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*sizeof(float));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (float **) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 2D real array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      set_var_info_(array,array,1,dimensions,sizeof(float),size,FLOAT);

      pthread_mutex_unlock(&(mai_mutex));     

      c_2dreal_f_(carrayref,array,nx,ny);
}

/*Alloc arrays of two dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_2d_lreal_(int *carrayref,int *nx,int *ny){
	int nitemsx = *nx;
        int nitemsy = *ny;
        double **array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*sizeof(double));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (double **) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 2D long real array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      set_var_info_(array,array,1,dimensions,sizeof(double),size,DOUBLE);

      pthread_mutex_unlock(&(mai_mutex));     

      c_2dlreal_f_(carrayref,array,nx,ny);
}

/*Alloc arrays of two dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_2d_int_(int *carrayref,int *nx,int *ny){
	int nitemsx = *nx;
        int nitemsy = *ny;
        int **array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*sizeof(int));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (int **) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 2D integer array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      set_var_info_(array,array,1,dimensions,sizeof(int),size,INT);

      pthread_mutex_unlock(&(mai_mutex));     

      c_2dint_f_(carrayref,array,nx,ny);
}

/*Alloc arrays of two dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_2d_byte_(int *carrayref,int *nx,int *ny){
	int nitemsx = *nx;
        int nitemsy = *ny;
        char **array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*sizeof(char));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (char **) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 2D byte array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      set_var_info_(array,array,1,dimensions,sizeof(char),size,CHAR);

      pthread_mutex_unlock(&(mai_mutex));     

      c_2dbyte_f_(carrayref,array,nx,ny);
}

/*Alloc arrays of two dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_2d_complex_(int *carrayref,int *nx,int *ny){
	int nitemsx = *nx;
        int nitemsy = *ny;
        float **array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*2*sizeof(float));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (float **) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 2D complex array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      set_var_info_(array,array,1,dimensions,2*sizeof(float),size,STRUCT);

      pthread_mutex_unlock(&(mai_mutex));     

      c_2dcomplex_f_(carrayref,array,nx,ny);
}

/*Alloc arrays of three dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_3d_real_(int *carrayref,int *nx,int *ny,int *nz){
	int nitemsx = *nx;
        int nitemsy = *ny;
        int nitemsz = *nz;
        float ***array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*nitemsz*sizeof(float));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (float ***) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 3D real array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      dimensions[2]=nz;
      set_var_info_(array,array,1,dimensions,sizeof(float),size,FLOAT);

      pthread_mutex_unlock(&(mai_mutex));     

      c_3dreal_f_(carrayref,array,nx,ny,nz);
}

/*Alloc arrays of three dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_3d_lreal_(int *carrayref,int *nx,int *ny,int *nz){
	int nitemsx = *nx;
        int nitemsy = *ny;
        int nitemsz = *nz;
        double ***array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*nitemsz*sizeof(double));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (double ***) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 3D long real array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      dimensions[2]=nz;
      set_var_info_(array,array,1,dimensions,sizeof(double),size,DOUBLE);

      pthread_mutex_unlock(&(mai_mutex));     

      c_3dlreal_f_(carrayref,array,nx,ny,nz);
}

/*Alloc arrays of three dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_3d_int_(int *carrayref,int *nx,int *ny,int *nz){
	int nitemsx = *nx;
        int nitemsy = *ny;
        int nitemsz = *nz;
        int ***array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*nitemsz*sizeof(int));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (int ***) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 3D integer array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      dimensions[2]=nz;
      set_var_info_(array,array,1,dimensions,sizeof(int),size,INT);

      pthread_mutex_unlock(&(mai_mutex));     

      c_3dint_f_(carrayref,array,nx,ny,nz);
}

/*Alloc arrays of three dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_3d_byte_(int *carrayref,int *nx,int *ny,int *nz){
	int nitemsx = *nx;
        int nitemsy = *ny;
        int nitemsz = *nz;
        char ***array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*nitemsz*sizeof(char));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (char ***) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 3D byte array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      dimensions[2]=nz;
      set_var_info_(array,array,1,dimensions,sizeof(char),size,CHAR);

      pthread_mutex_unlock(&(mai_mutex));     

      c_3dbyte_f_(carrayref,array,nx,ny,nz);
}

/*Alloc arrays of three dimension 
  parameters: nx-number of elements, size_t size_item
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_3d_complex_(int *carrayref,int *nx,int *ny,int *nz){
	int nitemsx = *nx;
        int nitemsy = *ny;
        int nitemsz = *nz;
        float ***array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*nitemsz*2*sizeof(float));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (float ***) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 3D complex array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      dimensions[2]=nz;
      set_var_info_(array,array,1,dimensions,2*sizeof(float),size,FLOAT);

      pthread_mutex_unlock(&(mai_mutex));     

      c_3dcomplex_f_(carrayref,array,nx,ny,nz);
}

/*Alloc arrays of four dimension 
  parameters: nx-number of elements, size_t size_item
              ny-number of elements, ny-number of elements,
	      nz-number of elements,
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_4d_real_(int *carrayref,int *nx,int *ny,int *nz,int *nk){
	int nitemsx = *nx;
        int nitemsy = *ny;
        int nitemsz = *nz;
        int nitemsk = *nk;
        float ****array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*nitemsz*nitemsk*sizeof(float));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (float ****) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 4D real array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      dimensions[2]=nz;
      dimensions[3]=nz;
      set_var_info_(array,array,1,dimensions,sizeof(float),size,FLOAT);

      pthread_mutex_unlock(&(mai_mutex));     

      c_4dreal_f_(carrayref,array,nx,ny,nz,nk);
}

/*Alloc arrays of four dimension 
  parameters: nx-number of elements, size_t size_item
              ny-number of elements, ny-number of elements,
	      nz-number of elements,
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_4d_lreal_(int *carrayref,int *nx,int *ny,int *nz,int *nk){
	int nitemsx = *nx;
        int nitemsy = *ny;
        int nitemsz = *nz;
        int nitemsk = *nk;
        double ****array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*nitemsz*nitemsk*sizeof(double));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (double ****) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 4D long real array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      dimensions[2]=nz;
      dimensions[3]=nz;
      set_var_info_(array,array,1,dimensions,sizeof(double),size,DOUBLE);

      pthread_mutex_unlock(&(mai_mutex));     

      c_4dlreal_f_(carrayref,array,nx,ny,nz,nk);
}

/*Alloc arrays of four dimension 
  parameters: nx-number of elements, size_t size_item
              ny-number of elements, ny-number of elements,
	      nz-number of elements,
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_4d_int_(int *carrayref,int *nx,int *ny,int *nz,int *nk){
	int nitemsx = *nx;
        int nitemsy = *ny;
        int nitemsz = *nz;
        int nitemsk = *nk;
        int ****array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*nitemsz*nitemsk*sizeof(int));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (int ****) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 4D integer array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      dimensions[2]=nz;
      dimensions[3]=nz;
      set_var_info_(array,array,1,dimensions,sizeof(int),size,INT);

      pthread_mutex_unlock(&(mai_mutex));     

      c_4dint_f_(carrayref,array,nx,ny,nz,nk);
}

/*Alloc arrays of four dimension 
  parameters: nx-number of elements, size_t size_item
              ny-number of elements, ny-number of elements,
	      nz-number of elements,
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_4d_byte_(int *carrayref,int *nx,int *ny,int *nz,int *nk){
	int nitemsx = *nx;
        int nitemsy = *ny;
        int nitemsz = *nz;
        int nitemsk = *nk;
        char ****array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*nitemsz*nitemsk*sizeof(char));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (char ****) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 4D byte array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      dimensions[2]=nz;
      dimensions[3]=nz;
      set_var_info_(array,array,1,dimensions,sizeof(char),size,CHAR);

      pthread_mutex_unlock(&(mai_mutex));     

      c_4dbyte_f_(carrayref,array,nx,ny,nz,nk);
}

/*Alloc arrays of four dimension 
  parameters: nx-number of elements, size_t size_item
              ny-number of elements, ny-number of elements,
	      nz-number of elements,
              type-C primitive type or a struct
  return: void pointer
*/
void  mai_alloc_4d_complex_(int *carrayref,int *nx,int *ny,int *nz,int *nk){
	int nitemsx = *nx;
        int nitemsy = *ny;
        int nitemsz = *nz;
        int nitemsk = *nk;
        float ****array;
        size_t size;
        int dimensions[4]={0,0,0,0};//to save array dimmensions

        size = alignmem(nitemsx*nitemsy*nitemsz*nitemsk*2*sizeof(float));
        pthread_mutex_lock(&(mai_mutex));  

	if ((array = mmap(0,size,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0)) 
	== (float ****) NULL) {
		(void) fprintf(stderr,"Failed to allocate space for 4D complex array\n");
	} 

      dimensions[0]=nx;
      dimensions[1]=ny;
      dimensions[2]=nz;
      dimensions[3]=nz;
      set_var_info_(array,array,1,dimensions,2*sizeof(float),size,FLOAT);

      pthread_mutex_unlock(&(mai_mutex));     

      c_4dcomplex_f_(carrayref,array,nx,ny,nz,nk);
}

/*Free allocated arrays
  parameters: pointer to the array
  return: none
*/
void mai_free_array_(void *p)
{
   mai_free_array(p);
}

/*
Initialize the environment - executed by master thread
parameters: filename-the name of the configuration file
return: none
*/
void mai_init_(char *file_f)
{
  char *name=NULL;
  int strsize;

  if(file_f != NULL)
  {
	int strsize;
	strsize = strlen(file_f)+1;
	name = malloc(strsize*sizeof(char));
	strncpy(name,file_f,strsize-1);
	name[strsize-1] = '\0';
  }
 
 mai_init(name); 
}

/*
Change the environment - executed by master thread
parameters: filename-the name of the configuration file
return: none
*/
void mai_change_(char *file_f)
{
  char *name=NULL;
  int strsize;

  if(file_f != NULL)
  {
	int strsize;
	strsize = strlen(file_f)+1;
	name = malloc(strsize*sizeof(char));
	strncpy(name,file_f,strsize-1);
	name[strsize-1] = '\0';
  }
 
 mai_change(name); 

}

/*
Destroy the environment - executed by master thread
parameters: none
return: none
*/
void mai_final_()
{
  mai_final();
}

/*
Made a round-robin with the memory pages of data pointed by void *p.
Round-robin starts in the node that causes the first page fault.
parameters: p-pointer to data structure  
return: none
*/
void mai_cyclic_(void *p)
{
  int err=0,i,actualnnodes;
  unsigned long node=0,totalmem=0,freemem=0; 
  void *t=NULL;

   t = get_var_pl(p);
     if(t == NULL)
         t = p;

  //optimize data placement considering the number of threads running in the application!
  if(sinfo.nthreads == 0)
    sinfo.nthreads = get_number_threads();

 if(sinfo.nthreads>0 && sinfo.nthreads<sinfo.nnodes)
    {
      for(i=0;i<sinfo.nthreads;i++)
       {
	node += sinfo.nodes[i];
        freemem+=na_get_memnode(i);
       }

      //verify if there is enough memory
      totalmem=(get_var_tsize(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(actualnnodes);
          node += sinfo.nodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else
    {
      for(i=0;i<sinfo.nnodes;i++)
	node += sinfo.nodes[i];

      actualnnodes = sinfo.nnodes;
    }

  if(get_var_policy(p) != CYCLIC && get_var_policy(p) != -1)//changing memory policy
  {
   if(totalmem < (KBYTE*freemem)){ //have enough memory
    #ifdef MPOL_MF_MOVE
    err=mbind(t,get_var_tsize(p),MPOL_INTERLEAVE,
	    &node,MASK_SIZE,MPOL_MF_MOVE);
    set_var_nodes(p,actualnnodes,sinfo.nodes);
    #else
    printf("\n Operating system do not support data migration!");
    exit(-1);
    #endif 
   }
   else
    {
     printf("\n Nodes do not have enough memory!");
    }
  }
  else //applying cyclic memory policy
  {
    err=mbind(t,get_var_tsize(p),MPOL_INTERLEAVE,
	    &node,MASK_SIZE,0);
    set_var_policy(p,CYCLIC);
    set_var_nodes(p,actualnnodes,sinfo.nodes);
  }
  
  if(err < 0)
  {
    identify_error();     
    printf("\n function cyclic!");
  }
}

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure
return: none
*/
void mai_skew_mapp_(void *p)
{
  int err,i,np=0,block,rest=0,nitens,*ti,type,great_int,actualnnodes=0,mig=0;
  size_t size;
  unsigned long node,*acnodes=NULL,totalmem=0,freemem=0,nodes=0;
  void *t=NULL;
  unsigned long pagsize,npag;

  npag=NPAG;

  acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	
	
  nitens = get_var_itens(p);
  size = get_var_size(p);
  type = get_var_type(p);

  np = get_var_tsize(p)/PAGE_SIZE;

  block = np/npag;

    t = get_var_pl(p);
     if(t == NULL)
         t = p;

  //optimize data placement considering the number of threads running in the application!
  if(sinfo.nthreads == 0)
    sinfo.nthreads = get_number_threads();

  if(sinfo.nthreads>0 && sinfo.nthreads<sinfo.nnodes)
    {
      for(i=0;i<sinfo.nthreads;i++)
       {
	acnodes[i]= sinfo.nodes[i];
        freemem+=na_get_memnode(i);
        nodes += acnodes[i];
       }

      //verify if there is enough memory
      totalmem=(get_var_tsize(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(actualnnodes);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
          nodes += acnodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++){
	acnodes[i] = sinfo.nodes[i];
        nodes +=acnodes[i];}
      actualnnodes = sinfo.nnodes;
    }
  
  if(get_var_policy(p) != SKEW && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,SKEW);
    set_var_nodes(p,actualnnodes,acnodes);
  
  for(i=0;i<block;i++)
 {
    great_int = i/actualnnodes;
    node = acnodes[(i+great_int+1)%actualnnodes];

   if(mig)//changing memory policy
   {
   if(totalmem < (KBYTE*freemem)){ //have enough memory
    #ifdef MPOL_MF_MOVE
    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
    #else
    printf("\n Operating system do not support data migration!");
    exit(-1);
    #endif 
   }
   else
    {
     printf("\n Nodes do not have enough memory!");
    }
  }
  else //applying skew memory policy
  {
    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,0);
  }

    t = (npag*PAGE_SIZE)+ t;
 }

   if(err < 0)
    {
        identify_error();
        printf("\t function skew-mapp");
   }
}

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure
return: none
*/
void mai_prime_mapp_(void *p,int prime)
{
 int err=0,i,rest=0,nitens,*ti,type,mem_node,npagi,pm_no;
  size_t size;
  unsigned long node,nodes=0;
  void *t=NULL;
  unsigned long pagsize,block,np,npag=NPAG;

  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;

  acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	
  nitens = get_var_itens(p);
  size = get_var_size(p);
  type = get_var_type(p);

  np = get_var_tsize(p)/PAGE_SIZE;
 
  block = np/npag;
  rest = np%npag;

     t = get_var_pl(p);
     if(t == NULL)
         t = p;

 //optimize data placement considering the number of threads running in the application!
  if(sinfo.nthreads == 0)
    sinfo.nthreads = get_number_threads();

  if(sinfo.nthreads>0 && sinfo.nthreads<sinfo.nnodes)
    {
      for(i=0;i<sinfo.nthreads;i++)
       {
	acnodes[i]= sinfo.nodes[i];
        freemem+=na_get_memnode(i);
        nodes += acnodes[i];
       }

      //verify if there is enough memory
      totalmem=(get_var_tsize(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(actualnnodes);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
          nodes += acnodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++){
	acnodes[i] = sinfo.nodes[i];
        nodes += acnodes[i];}
      actualnnodes = sinfo.nnodes;
    }

  if(get_var_policy(p) != PRIME && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,PRIME);
    set_var_nodes(p,actualnnodes,acnodes); 

  for(i=0;i<block;i++)
 {
        npagi = i*npag;
        if((npagi%prime) >= actualnnodes)
        {
         pm_no = npagi % prime;
         while(pm_no >= actualnnodes)
         {
           pm_no = (npagi/prime)*(prime-actualnnodes)+((npagi%prime)-actualnnodes);
           npagi = pm_no/prime;
           pm_no = pm_no % prime;
         }
         mem_node = pm_no;
        }
        else
         mem_node = npagi % prime;
  
        node = acnodes[mem_node];
	
	 if(mig)//changing memory policy
  	 {
  		 if(totalmem < (KBYTE*freemem)){ //have enough memory
   	 #ifdef MPOL_MF_MOVE
   		 err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
   	 #else
   		 printf("\n Operating system do not support data migration!");
    		 exit(-1);
    	#endif 
   	}
   	else
   	 {
     		printf("\n Nodes do not have enough memory!");
    	 }
  	}
  	else //applying memory policy
  	{
   		 err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,0);
  	}

    	 t = (npag*PAGE_SIZE)+ t;
 }

   if(err < 0)
    {
        identify_error();
        printf("\t function prime-mapp");
   }
}    

/*
Made a bind with memory pages pointed by void *p.
Bind is donne in the nodes contained in variable nodes .
parameters: p-pointer to data structure
return: none
*/
void mai_bind_all_(void *p)
{
  int err=0,i,nitens,actualnnodes;
  unsigned long nodes=0,totalmem=0,freemem=0; 
  size_t size;
  void *t=NULL;


  nitens = get_var_itens(p);
  size = get_var_size(p);

   t = get_var_pl(p);
  
     if(t == NULL)
         t = p;

//optimize data placement considering the number of threads running in the application!
  if(sinfo.nthreads == 0)
    sinfo.nthreads = get_number_threads();

  if(sinfo.nthreads>0 && sinfo.nthreads<sinfo.nnodes)
    {
      for(i=0;i<sinfo.nthreads;i++)
       {
	nodes += sinfo.nodes[i];
        freemem+=na_get_memnode(i);
       }

      //verify if there is enough memory
      totalmem=(get_var_tsize(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(actualnnodes);
          nodes += sinfo.nodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else
    {
      for(i=0;i<sinfo.nnodes;i++)
	nodes += sinfo.nodes[i];

     actualnnodes = sinfo.nnodes;
    }
 
  if(get_var_policy(t) != BIND && get_var_policy(t) != -1)//changing memory policy
  {
   if(totalmem < (KBYTE*freemem)){ //have enough memory
    #ifdef MPOL_MF_MOVE
    err=mbind(t,get_var_tsize(p),MPOL_BIND,&nodes,MASK_SIZE,MPOL_MF_MOVE);
    set_var_nodes(t,actualnnodes,sinfo.nodes);
    #else
    printf("\n Operating system do not support data migration!");
    exit(-1);
    #endif 
   }
   else
    {
     printf("\n Nodes do not have enough memory!");
    }
  }
  else //applying cyclic memory policy
  {
    err=err=mbind(t,get_var_tsize(p),MPOL_BIND,&nodes,MASK_SIZE,0);
    set_var_policy(t,BIND);
    set_var_nodes(t,actualnnodes,sinfo.nodes);
  }
  
  if(err < 0){
    identify_error(); 
    printf("\n function bind_all!");
  }
}

/*
Made a bind with blocks(rows) of memory pages pointed by void *p.
Bind is donne in the nodes contained in sinfo and starts with the
first node in the sinfo .
parameters: p-pointer to data structure
return: none
*/
void mai_bind_rows_(void *p)
{
  int err=0,i,block,nitens,*ti,type;
  size_t size;
  unsigned long node,nodes=0;
  void *t=NULL;
  unsigned long pagsize,np=0,rest=0;
  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;


    acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	

 //optimize data placement considering the number of threads running in the application!
  if(sinfo.nthreads == 0)
    sinfo.nthreads = get_number_threads();

  if(sinfo.nthreads>0 && sinfo.nthreads<sinfo.nnodes)
    {
      for(i=0;i<sinfo.nthreads;i++)
       {
	acnodes[i]= sinfo.nodes[i];
        freemem+=na_get_memnode(i);
        nodes += acnodes[i];
       }

      //verify if there is enough memory
      totalmem=(get_var_tsize(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(actualnnodes);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
          nodes += acnodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++){
	acnodes[i] = sinfo.nodes[i];
        nodes += acnodes[i];}
      actualnnodes = sinfo.nnodes;
    }

  if(get_var_policy(p) != BIND_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,BIND_B);
    set_var_nodes(p,actualnnodes,acnodes);

	   nitens = get_var_itens(p);
	  size = get_var_size(p);
	  type = get_var_type(p);

	  np = get_var_tsize(p)/PAGE_SIZE;

	  if(!(REST(size*nitens)))
	     np++;

	  block = np/actualnnodes;
	  rest = np%actualnnodes;

	    t=get_var_pl(p);
	    if(t == NULL)
		 t = p;
	  
       for(i=0;i<actualnnodes;i++)
       {
          node = acnodes[i];

	 if(mig)//changing memory policy
	   {
	   if(totalmem < (KBYTE*freemem)){ //have enough memory
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,block*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE); 
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   }
	   else
	    {
	     printf("\n Nodes do not have enough memory!");
	    }
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,block*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
	  t = (block*PAGE_SIZE) + t; 	
       }   

    if(err < 0)
    {
      identify_error();
      printf("\t (function bind_rows)");
    }
 
}

/*
Made a bind with blocks(columns) of memory pages pointed by void *p.
Bind is donne in the nodes contained in sinfo and starts with the
first node in the sinfo .
parameters: p-pointer to data structure
return: none
*/
void mai_bind_columns_(void *p)
{
  int err=0,i,j,descColumn=1,nc,nitens,*ti;
  size_t size;
  unsigned long node,nodes=0;
  void *t=NULL;
  unsigned long pagsize,npRow=1,npag,rest=0;

int actualnnodes=0,mig=0;
unsigned long *acnodes=NULL,totalmem=0,freemem=0;


acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	

 //optimize data placement considering the number of threads running in the application!
  if(sinfo.nthreads == 0)
    sinfo.nthreads = get_number_threads();

  if(sinfo.nthreads>0 && sinfo.nthreads<sinfo.nnodes)
    {
      for(i=0;i<sinfo.nthreads;i++)
       {
	acnodes[i]= sinfo.nodes[i];
 	nodes += acnodes[i];
        freemem+=na_get_memnode(i);
       }

      //verify if there is enough memory
      totalmem=(get_var_tsize(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(actualnnodes);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
          nodes += acnodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++){
	acnodes[i] = sinfo.nodes[i];
	nodes += acnodes[i];
	}

      actualnnodes = sinfo.nnodes;
    }
   

  if(get_var_policy(p) != BIND_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,BIND_B);
    set_var_nodes(p,actualnnodes,acnodes);

  nitens = get_var_itens(p);
  size = get_var_size(p);

  for(i=3;get_var_dim(p,i)!=0;i++)
    npRow  *= get_var_dim(p,i);

  npRow *= size*get_var_dim(p,2);
  npRow = npRow / PAGE_SIZE;

  nc = get_var_dim(p,2)/actualnnodes;

  for(i=3;get_var_dim(p,i)!=0;i++)
    descColumn *= get_var_dim(p,i);

  descColumn *= nc * size;
  descColumn = descColumn/PAGE_SIZE;
  
  t=get_var_pl(p);
    if(t == NULL)
         t = p;
  
    for(j=0;j<actualnnodes;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          node = sinfo.nodes[j];
	  
	 if(mig)//changing memory policy
	   {
	   if(totalmem < (KBYTE*freemem)){ //have enough memory
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);   
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   }
	   else
	    {
	     printf("\n Nodes do not have enough memory!");
	    }
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
         t = (npRow*PAGE_SIZE) + t; 	
       }   
     
       t=get_var_pl(p);
       if(t == NULL)
         t = p;

       t = t + ((j+1)*descColumn*PAGE_SIZE);
    }

    if(err < 0)
    {
      identify_error();
      printf("\t (function bind_columns)");
    }
 
}

/*
Made a random placement with memory pages pointed by void *p.
Placement is donne left-to-right in the nodes drawned by 
a uniform distribution. This memory policy try to guarantee
conflict-free in pages access and more randomness in page
placement.
parameters:  p-pointer to data structure, npag-number of pages 
             in a block. 
return: none
*/
void mai_random_(void *p)
{
  int err=0,i,np=0,j,nitens,type,icpt,npag=NPAG;
  size_t size;
  unsigned long node,nodes=0;
  void *t=NULL;
  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;   


	acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	

	 //optimize data placement considering the number of threads running in the application!
	  if(sinfo.nthreads == 0)
	    sinfo.nthreads = get_number_threads();

	  if(sinfo.nthreads>0 && sinfo.nthreads<sinfo.nnodes)
	    {
	      for(i=0;i<sinfo.nthreads;i++)
	       {
		acnodes[i]= sinfo.nodes[i];
		freemem+=na_get_memnode(i);
 		nodes += acnodes[i];
	       }

	      //verify if there is enough memory
	      totalmem=(get_var_tsize(p));
	      actualnnodes = sinfo.nthreads;
	      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
		{
		  freemem+=na_get_memnode(actualnnodes);
		  acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
		  nodes += acnodes[actualnnodes];
		  actualnnodes++;
		}         
	    }
	  else //use all possible nodes
	    {
	      for(i=0;i<sinfo.nnodes;i++){
		acnodes[i] = sinfo.nodes[i];
 		nodes += acnodes[i];
		}

	      actualnnodes = sinfo.nnodes;
	    }

	  
	  if(get_var_policy(p) != RANDOM && get_var_policy(p) != -1)
	    mig = 1; //data migration

	    set_var_policy(p,RANDOM);
	    set_var_nodes(p,actualnnodes,acnodes);

  nitens = get_var_itens(p);
  size = get_var_size(p);
  type = get_var_type(p);
  icpt = 0;

  srand ( time(NULL) );

  np = get_var_tsize(p)/PAGE_SIZE;
  icpt = np/npag;

     t = get_var_pl(p);
     if(t == NULL)
         t = p;
    
 for(i=0;i<icpt;i++)
 {
   node = rand() % actualnnodes;
   node = 1 << node;

	if(mig)//changing memory policy
	   {
	   if(totalmem < (KBYTE*freemem)){ //have enough memory
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);   
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   }
	   else
	    {
	     printf("\n Nodes do not have enough memory!");
	    }
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,0);
	  }
   
    t = (npag*PAGE_SIZE) + t; 
        
 }   

   if(err < 0)
    {
        identify_error();
	printf("\t function random_uniform");
   }
}

void mai_handler_(int sig, siginfo_t *info, void *_context) {
  void *p,*real_p,*initp_block;
  unsigned long node;
  unsigned long cpu;
  int err;

  p = info->si_addr;
  real_p = get_true_pointer_handler(p);
  if (real_p == NULL) {
       struct sigaction new_action;

       /* Set up the structure to specify the new action. */
       pthread_mutex_lock(&mai_mutex);
       mai_sigsegv_handler_set = 0;
       sigemptyset (&new_action.sa_mask);
       new_action.sa_flags = SA_SIGINFO;
       new_action.sa_sigaction = SIG_DFL;
       err = sigaction(SIGSEGV, &new_action, NULL);
       pthread_mutex_unlock(&mai_mutex); 
  }
  else{
    if(block_size == 0){  // next-touch
    	node = na_get_nodeidcpu(sched_getcpu());
    	mai_migrate_gather(real_p,node);
    	err = mprotect(get_var_pl(real_p), get_var_tsize(real_p), PROT_READ|PROT_WRITE);
    }
    else{  //bind_block
       initp_block = get_pointer_block(p,real_p,block_size);
       node = na_get_nodeidcpu(sched_getcpu());
       err=mbind(initp_block,block_size,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE); 
       err = mprotect(initp_block,block_size, PROT_READ|PROT_WRITE);
      }	
    }
   
}

/*Next-touch memory policy
parameters: p-pointer to data structure
return: none
*/
void mai_next_touch_(void *p)
{
 int err;
 unsigned long node;

 node = na_get_nodeidcpu(sched_getcpu());


  if(get_var_policy(p) != NEXTOUCH && get_var_policy(p) != -1)//changing memory policy
   {  
    set_var_nodes(p,1,node);
   }
 else //applying next-touch memory policy
  {
    set_var_policy(p,NEXTOUCH);
    set_var_nodes(p,1,node);  
  }

  if(!mai_sigsegv_handler_set)
  {
      struct sigaction new_action;

       /* Set up the structure to specify the new action. */
       pthread_mutex_lock(&mai_mutex); 
       mai_sigsegv_handler_set = 1;
       sigemptyset (&new_action.sa_mask);
       new_action.sa_flags = SA_SIGINFO;
       new_action.sa_sigaction = mai_handler_;
       err = sigaction(SIGSEGV, &new_action, NULL);
       pthread_mutex_unlock(&mai_mutex); 
       if (err < 0)    printf("\t (Error on next-touch memory policy - signal)\n");
  }
 
  err = mprotect(get_var_pl(p), get_var_tsize(p), PROT_NONE);
      if (err < 0)    printf("\t (Error on next-touch memory policy - mprotect)\n");
}



void mai_bind_rows_mynode_(void *p)
{
   int err=0,i,block,nitens,*ti,type;
  size_t size;
  unsigned long node,nodes=0;
  void *t=NULL;
  unsigned long pagsize,np=0,rest=0;
  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;

 block_size = 0;

  acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	

 //optimize data placement considering the number of threads running in the application!
pthread_mutex_lock(&(sinfo.lock_place));
  if(sinfo.nthreads == 0)
    sinfo.nthreads = get_number_threads();
  pthread_mutex_unlock(&(sinfo.lock_place));

  if(sinfo.nthreads>0 && sinfo.nthreads<sinfo.nnodes)
    {
      for(i=0;i<sinfo.nthreads;i++)
       {
	acnodes[i]= sinfo.nodes[i];
        freemem+=na_get_memnode(i);
        nodes += acnodes[i];
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(actualnnodes);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
          nodes += acnodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++){
	acnodes[i] = sinfo.nodes[i];
        nodes += acnodes[i];}
      actualnnodes = sinfo.nnodes;
    }

   //place pointers on all nodes
  size=0;
  size_t size_void=sizeof(void*);
  int ndim = get_var_ndim(p);
  if (ndim > 1){
   
   for(i=0;i<ndim-1;i++)
    size += get_var_dim(p,i+1)*size_void;

   size = alignmem(size);  
   err=mbind(p,size,MPOL_INTERLEAVE,
	    &nodes,MASK_SIZE,MPOL_MF_MOVE);
   if(err < 0)
   {
    identify_error();     
    printf("\n Pointers - function cyclic!");
   }
  }
 
    set_var_policy(p,BIND_B);
    set_var_nodes(p,actualnnodes,acnodes);

   nitens = get_var_itens(p);
   size = get_var_size(p);
   type = get_var_type(p);

	  np = (size*nitens)/PAGE_SIZE;

	  if(!(REST(size*nitens)))
	     np++;

  	  block = np/actualnnodes;
	  rest = np%actualnnodes;

	    t=get_var_pl(p);
	    if(t == NULL)
		 t = p;

    if(totalmem < (KBYTE*freemem)){ //have enough memory
      for(i=0;i<block;i++)
	{
         if(!mai_sigsegv_handler_set)
         {
          struct sigaction new_action;

         /* Set up the structure to specify the new action. */
         pthread_mutex_lock(&mai_mutex); 
         mai_sigsegv_handler_set = 1;
         sigemptyset (&new_action.sa_mask);
         new_action.sa_flags = SA_SIGINFO;
         new_action.sa_sigaction = mai_handler;
         err = sigaction(SIGSEGV, &new_action, NULL);
         pthread_mutex_unlock(&mai_mutex); 
         if (err < 0)    printf("\t (Error on next-touch memory policy - signal)\n");
         }
 	 block_size = block*PAGE_SIZE;
         err = mprotect(t,block*PAGE_SIZE, PROT_NONE);
	 t = t + block*PAGE_SIZE;
         if (err < 0)    printf("\t (Error on bind_rows memory policy - mprotect)\n");

	}

    }
 	else
    	     printf("\n Nodes do not have enough memory!");
}


void mai_bind_columns_mynode_(void *p)
{
  int err=0,i,j,descColumn=1,nc,nitens,*ti;
  size_t size;
  unsigned long node,nodes=0;
  void *t=NULL;
  unsigned long pagsize,npRow=1,npag,rest=0;

int actualnnodes=0,mig=0;
unsigned long *acnodes=NULL,totalmem=0,freemem=0;

block_size = 0;
acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	

 //optimize data placement considering the number of threads running in the application!
pthread_mutex_lock(&(sinfo.lock_place));
  if(sinfo.nthreads == 0)
    sinfo.nthreads = get_number_threads();
  pthread_mutex_unlock(&(sinfo.lock_place));

  if(sinfo.nthreads>0 && sinfo.nthreads<sinfo.nnodes)
    {
      for(i=0;i<sinfo.nthreads;i++)
       {
	acnodes[i]= sinfo.nodes[i];
 	nodes += acnodes[i];
        freemem+=na_get_memnode(i);
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(actualnnodes);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
          nodes += acnodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++){
	acnodes[i] = sinfo.nodes[i];
	nodes += acnodes[i];
	}

      actualnnodes = sinfo.nnodes;
    }
   
   //place pointers on all nodes
  size=0;
  size_t size_void=sizeof(void*);
  int ndim = get_var_ndim(p);
  if (ndim > 1){
   
   for(i=0;i<ndim-1;i++)
    size += get_var_dim(p,i+1)*size_void;

   size = alignmem(size);  
   err=mbind(p,size,MPOL_INTERLEAVE,
	    &nodes,MASK_SIZE,MPOL_MF_MOVE);
   if(err < 0)
   {
    identify_error();     
    printf("\n Pointers - function cyclic!");
   }
  }

    set_var_policy(p,BIND_B);
    set_var_nodes(p,actualnnodes,acnodes);

  nitens = get_var_itens(p);
  size = get_var_size(p);

  for(i=3;get_var_dim(p,i)!=0;i++)
    npRow  *= get_var_dim(p,i);

  npRow *= size*get_var_dim(p,2);
  npRow = npRow / PAGE_SIZE;

  nc = get_var_dim(p,2)/actualnnodes;

  for(i=3;get_var_dim(p,i)!=0;i++)
    descColumn *= get_var_dim(p,i);

  descColumn *= nc * size;
  descColumn = descColumn/PAGE_SIZE;
  
  t=get_var_pl(p);
    if(t == NULL)
         t = p;
  
    for(j=0;j<actualnnodes;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          node = sinfo.nodes[j];
	  if(!mai_sigsegv_handler_set)
          {
          struct sigaction new_action;

         /* Set up the structure to specify the new action. */
         pthread_mutex_lock(&mai_mutex); 
         mai_sigsegv_handler_set = 1;
         sigemptyset (&new_action.sa_mask);
         new_action.sa_flags = SA_SIGINFO;
         new_action.sa_sigaction = mai_handler;
         err = sigaction(SIGSEGV, &new_action, NULL);
         pthread_mutex_unlock(&mai_mutex); 
         if (err < 0)    printf("\t (Error on next-touch memory policy - signal)\n");
         }
         block_size = descColumn*PAGE_SIZE;
         err = mprotect(t,descColumn*PAGE_SIZE, PROT_NONE);
	 t = (npRow*PAGE_SIZE) + t; 	

   	 }	  
    
       t=get_var_pl(p);
       if(t == NULL)
         t = p;

       t = t + ((j+1)*descColumn*PAGE_SIZE);
    }

    if(err < 0)
    {
      identify_error();
      printf("\t (function bind_columns_mynode)");
    } 

}


/*
Migrate a memory page pointed by void *p to node.
parameters: p-pointer to data structure, node-new node
            nr-number of rows,start-start row
return: none
*/
void mai_migrate_(void *p,unsigned long node,int nr, int startr,int nc, int startc)
{
  int *status=NULL,*nodes=NULL,actualnnodes=0,find=0;
  int err=0,i,j,descColumn=1,nitens,num_pages_c=1;
  size_t size;
  void *t=NULL;
  unsigned long npRow=1,totalmem=0,freemem=0; 
  
  
 
   if(nc != 0 && nr !=0)
    {  
      
      for(i=3;get_var_dim(p,i)!=0;i++)
	npRow  *= get_var_dim(p,i);

      npRow *= size*get_var_dim(p,2);
      npRow = npRow / PAGE_SIZE;

      for(i=3;get_var_dim(p,i)!=0;i++)
	{
	  descColumn *= get_var_dim(p,i);
	  num_pages_c *= get_var_dim(p,i);
	}  

      num_pages_c *= size;
      num_pages_c = size / PAGE_SIZE;
      descColumn *= nc * size;
      descColumn = descColumn/PAGE_SIZE;

      t=get_var_pl(p);
      if(t == NULL)
         t = p;
      
      for(i=0;i<startr;i++)
         t = (npRow*PAGE_SIZE) + t; 
      for(i=0;i<startc;i++)
	t = (num_pages_c*PAGE_SIZE) + t; 

	//computing total memory to be migrated and free memory on the node
	totalmem=descColumn*PAGE_SIZE*nr;
        freemem=na_get_memnode(log(node)/log(2));
        
      if(totalmem < freemem){ //if there are enough memory
 
	//set node
	nodes = get_var_nodes(p,&actualnnodes);
	i=0;
	do{
          if(nodes[i] == node)
            find=1;
	}while(i<actualnnodes && nodes[i] != node && nodes[i] != -1); 

	if(!find  &&  actualnnodes < sinfo.nnodes){
	  sinfo.nodes[i] = node;
	  actualnnodes++;
	  set_var_nodes(p,actualnnodes,sinfo.nodes);
        }

       for(i=0;i<nr;i++)
       {
          #ifdef MPOL_MF_MOVE 
          node = 1 << node;
	  err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);
          #else
	  printf("\t Operating system do not support data migration");
          #endif 
	  t = (npRow*PAGE_SIZE) + t; 	
       }   }
     else
       printf("\t Node does not have enough memory!");
        
    }//end columns and rows


  if(err < 0){
    identify_error();
    printf("\t function migrate_rows");
   }

}

/*
Migrate a memory page pointed by void *p to node.
parameters: p-pointer to data structure, node-new node
            nr-number of rows,start-start row
return: none
*/
void mai_migrate_rows_(void *p,unsigned long node,int nr, int start)
{ 
  int *status=NULL,*nodes=NULL,actualnnodes,find=0;
  int err=0,i,j,descColumn=1,nitens;
  size_t size;
  void *t=NULL;
  unsigned long npRow=1,totalmem=0,freemem=0; 
  
 
  nitens = get_var_itens(p);
  size = get_var_size(p);

  for(i=3;get_var_dim(p,i)!=0;i++)
    npRow  *= get_var_dim(p,i);

  npRow *= size*get_var_dim(p,2);
  npRow = npRow / PAGE_SIZE;

  t=get_var_pl(p);
    if(t == NULL)
         t = p;

    for(i=0;i<start;i++)
      t = t + (npRow*PAGE_SIZE);

  //computing total memory to be migrated and free memory on the node
	totalmem=npRow*nr*PAGE_SIZE;
        freemem=na_get_memnode(log(node)/log(2));

if(totalmem <freemem) //there are enough memory
{
  //set node
	nodes = get_var_nodes(p,&actualnnodes);
	i=0;
	do{
          if(nodes[i] == node)
            find=1;
	}while(i<actualnnodes && nodes[i] != node && nodes[i] != -1); 

	if(!find  &&  actualnnodes < sinfo.nnodes){
	  sinfo.nodes[i] = node;
	  actualnnodes++;
	  set_var_nodes(p,actualnnodes,sinfo.nodes);
        }
  node = 1 << node;  
  #ifdef MPOL_MF_MOVE
  err=mbind(t,nr*npRow*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);
   if(err < 0){
    identify_error();
    printf("\t function migrate_rows");
   }

  #elif NR_move_pages
  err = syscall(__NR_move_pages, 0, nr*npRow*PAGE_SIZE, t,&node,status, 0);
  if(err < 0){
    identify_error();
    printf("\t function migrate_rows");
   }
  #else 
   printf("\t There is no support for migration!");
  #endif 
 }
 else
  printf("\t Node does not have enough memory!");
}

/*
Migrate a memory page pointed by void *p to node.
parameters: p-pointer to data structure, node-new node
            nc-number of columns,start-start column
return: none
*/
void mai_migrate_columns_(void *p,unsigned long node,int nc, int start)
{ 
  int *status=NULL,*nodes,actualnnodes,find=0;
  int err=0,i,j,descColumn=1,nitens;
  size_t size;
  void *t=NULL;
  unsigned long npRow=1,totalmem=0,freemem=0; 
  
 
  nitens = get_var_itens(p);
  size = get_var_size(p);

  for(i=3;get_var_dim(p,i)!=0;i++)
    npRow  *= get_var_dim(p,i);

  npRow *= size*get_var_dim(p,2);
  npRow = npRow / PAGE_SIZE;

  for(i=3;get_var_dim(p,i)!=0;i++)
    descColumn *= get_var_dim(p,i);

  descColumn *= size;
  descColumn = descColumn/PAGE_SIZE;

  t=get_var_pl(p);
    if(t == NULL)
         t = p;

    t = t + ((start+1)*descColumn*PAGE_SIZE);  

//computing total memory to be migrated and free memory on the node
	totalmem=descColumn*nc*PAGE_SIZE;
        freemem=na_get_memnode(log(node)/log(2));

if(totalmem <freemem) //there are enough memory
{
   //set node
	nodes = get_var_nodes(p,&actualnnodes);
	i=0;
	do{
          if(nodes[i] == node)
            find=1;
	}while(i<actualnnodes && nodes[i] != node && nodes[i] != -1); 

	if(!find  &&  actualnnodes < sinfo.nnodes){
	  sinfo.nodes[i] = node;
	  actualnnodes++;
	  set_var_nodes(p,actualnnodes,sinfo.nodes);
        }
  node = 1 << node;  
  for(i=0;i<get_var_dim(p,1);i++)
   {

     #ifdef MPOL_MF_MOVE
     err=mbind(t,nc*descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);
     if(err < 0){
     identify_error();
     printf("\t function migrate_rows");
     }
     #elif NR_move_pages
     err = syscall(__NR_move_pages, 0, nc*descColumn*PAGE_SIZE, t,&node,status, 0);
     if(err < 0){
     identify_error();
     printf("\t function migrate_rows");
     }
     #else
     printf("\t There is no support for migration");
     #endif    
     t = (npRow*PAGE_SIZE) + t; 	
   }
  }
  else
   printf("\t Node does not have enough memory!");  
}

/*
Migrate all memory pages pointed by void *p to nodes, considering an
interleave way.
parameters: p-pointer to data structure, size-number of elements
            nodes-mask that contains the new nodes.
return: none
*/
void mai_migrate_scatter_(void *p,unsigned long nodes)
{ 
 mai_migrate_scatter(p,nodes);
}

/*
Migrate all memory pages pointed by void *p to a new node.
parameters: p-pointer to data structure, node-mask that contains
            the new node.
return: none
*/
void mai_migrate_gather_(void *p,unsigned long node)
{ 
  mai_migrate_gather(p,node);
}

/*
Print the nodes where pages are.
parameters: pageaddrs-pages address, size-number of pages
return: none
*/
void mai_print_pagenodes_(long *pageaddrs,int *size)
{
  mai_print_pagenodes_(pageaddrs,*size);
}

/*
Get the current time
parameters: none
return: time in sec
*/
void mai_get_time_pmigration_()
{
  printf("%f",mai_get_time_pmigration());
}

/*
Get the current time - executed by master thread
parameters: none
return: time in sec
*/
void mai_print_threadcpus_()
{
 mai_print_threadcpus();
}

/*
Get the current time
parameters: none
return: time in sec
*/
void mai_get_time_tmigration_()
{
printf("%f",mai_get_time_tmigration());
}

/*
Get log for the application
parameters: none
return: none
*/
void mai_get_log_()
{
 mai_get_log();
}

/*
Get memory policy of an array
parameters: array
return: none
*/
void mai_mempol_(void* ph)
{
   mai_mempol(ph);
}

/*
Get nodes of an array
parameters: array
return: none
*/
void mai_arraynodes_(void* ph)
{
 mai_arraynodes(ph);
}

/*
Set the total number of threads
parameters: total number of threads
return: none
*/
void mai_set_number_threads_(int *nthreads)
{
  mai_set_number_threads(*nthreads);
}

/*
Set the threads Id in the library vector
parameters: none
return: none
*/
void mai_set_thread_id_posix_()
{
  mai_set_thread_id_posix(); 
}

/*
Show the threads id - executed by master thread
parameters: none
return: none
*/
void mai_show_thread_ids_()
{
mai_show_thread_ids();
}

/*
Migrate a thread to a new cpu/core
parameters: id-Thread Id, cpu-Cpu/core Id
return: none
*/
void mai_migrate_thread_(int *id,long *cpu)
{
  mai_migrate_thread(*id,*cpu);
}

/*
Show cpus used to place memory  - executed by master thread
parameters: none
return: none
*/
void mai_show_cpus_()
{
 mai_show_cpus();
}

/*
Show nodes used to place memory - executed by master thread
parameters: none
return: none
*/
void mai_show_nodes_()
{
mai_show_nodes();
}

/*
Get struct place_info data
parameters: none
return: number of threads
*/
void mai_get_num_cpus_()
{
  printf("%d",mai_get_num_cpus());
}

/*
Get struct place_info data
parameters: none
return: number of nodes
*/
void mai_get_num_nodes_()
{
   printf("%d",mai_get_num_nodes());
}

/*
Get struct place_info data
parameters: none
return: number of threads
*/
void mai_get_num_threads_()
{
  printf("%d",mai_get_num_threads());
}
