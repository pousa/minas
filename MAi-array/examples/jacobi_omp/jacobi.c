#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>

#ifdef MAI
#include "mai.h"
#endif
#define MAXGRID  16392  /* maximum grid size, including boundaries */

int gridSize, numIters;
double **grid1;
double **grid2;

void InitializeGrids() {
  int i, j;

#pragma omp parallel for private(j)  
  for (i = 0; i <= gridSize; i++)
    for (j = 0; j <= gridSize; j++) {
      grid1[i][j] = 0.0;
      grid2[i][j] = 0.0;
    }
  for (i = 0; i <= gridSize; i++) {
    grid1[i][0] = 1.0;
    grid1[i][gridSize] = 1.0;
    grid2[i][0] = 1.0;
    grid2[i][gridSize] = 1.0;
  }
  for (j = 0; j <= gridSize; j++) {
    grid1[0][j] = 1.0;
    grid2[0][j] = 1.0;
  }
}

void jacobi() {
  int i, j, iters;

  for (iters = 1; iters <= numIters; iters++) {

  #pragma omp parallel for private(j)	  
    for (i = 1; i < gridSize; i++) {
      for (j = 1; j <= gridSize; j++) {
        grid2[i][j] = (grid1[i-1][j] + grid1[i+1][j] + 
                       grid1[i][j-1] + grid1[i][j+1]) * 0.25;
      }
    }

  #pragma omp parallel for private(j)	  
    for (i = 1; i < gridSize; i++) {
      for (j = 1; j <= gridSize; j++) {
        grid1[i][j] = (grid2[i-1][j] + grid2[i+1][j] +
               grid2[i][j-1] + grid2[i][j+1]) * 0.25;
      }
    }
  }
}


void irregular(int gridsize)
{
  int *rand_n,i,j,iters;

#ifdef MAI
  rand_n = mai_alloc_1D((gridsize+4)*sizeof(int));
  mai_bind_columns(rand_n);
#endif
#ifndef MAI
  rand_n = malloc((gridsize+4)*sizeof(int));
#endif
  
  srand(time(NULL));

  for(i=0;i<gridSize;i++)
     rand_n[i]=rand() % gridsize; 

   for (iters = 1; iters <= 2*numIters; iters++) {

    for (i = 1; i < gridSize; i++) { 
    #pragma omp parallel for 
     for (j = 1; j <= gridSize; j++) {
        grid1[rand_n[i]][j] = (grid2[i-1][rand_n[j]] + grid2[i+1][rand_n[j]] +
               grid2[i][j-1] + grid2[i][j+1]) * 0.25;
      }
    }
  
    for (i = 1; i < gridSize; i++) { 
    #pragma omp parallel for 
     for (j = 1; j <= gridSize; j++) {
        grid2[i][j] = (grid1[i-1][j] + grid1[i+1][j] +
               grid1[i][j-1] + grid1[i][j+1]) * 0.25;
      }
    }

   for (i = 1; i < gridSize; i++) {
    #pragma omp parallel for 
     for (j = 1; j <= gridSize; j++) {
        grid2[rand_n[i]][j] = (grid1[i-1][rand_n[j]] + grid1[rand_n[i]][j] +
               grid1[i][rand_n[j]] + grid1[rand_n[i]][j+1]) * 0.25;
      }
      }
  }
}	

double myms()
{
        struct timeval tp;
        struct timezone tzp;
        int i;

        i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec * 1.e+6 + (double) tp.tv_usec);
}


int main(int argc, char *argv[]) {
  int i, j;
  double tmp=0,tma=0;

  /* read command line and initialize grids */
  if (argc < 2) {
	printf("jacobi size n_iter\n");
	exit(0);
  }
  gridSize = atoi(argv[1]);
  numIters = atoi(argv[3]);

#ifndef MAI
 tma=0.0;
 tma = myms();
grid1 = (double**)malloc((gridSize+3)*sizeof(double*));
  grid2 = (double**)malloc((gridSize+3)*sizeof(double*));
  for(i = 0; i <= gridSize; i++) {
	grid1[i] = (double*)malloc((gridSize+3)*sizeof(double));	
	grid2[i] = (double*)malloc((gridSize+3)*sizeof(double));	
  }
 tma = myms() - tma;
 printf("Malloc time %f\n",tma/1.e+6);
#endif  

#ifdef MAI  
  mai_init(NULL);
  tma=0.0;
  tma = myms();
  grid1 = mai_alloc_2D(gridSize,gridSize, sizeof(double),DOUBLE);
  grid2 = mai_alloc_2D(gridSize,gridSize, sizeof(double),DOUBLE);
  tma = myms() - tma;
  printf("MAi alloc time: %f\n",tma/1.e+6);
#endif

  mai_bind_rows(grid1);
  mai_bind_rows(grid2);
  InitializeGrids();

  tma=0.0;
  tma = myms();

  jacobi();

#ifdef IRREGULAR
  mai_cyclic(grid1);
  mai_cyclic(grid2);
  irregular(gridSize);
#endif

  tma = myms() - tma;
  printf("Execution time %f\n",tma/1.e+6);
}



