/* Jacobi iteration using pthreads
 *
 *    a.out gridSize numWorkers numIters
 *
 *              */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/times.h>
#include <limits.h>
#include <unistd.h>

#include "mai.h"

#define SHARED 1
#define MAXGRID 8196   /* maximum grid size, including boundaries */
#define MAXWORKERS 16  /* maximum number of worker threads */

void *Worker(void *);
void InitializeGrids(double** grid1, double** grid2);
void Barrier();

//struct tms buffer;        /* used for timing */
//clock_t start, finish;

pthread_mutex_t barrier;  /* mutex semaphore for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numArrived = 0;       /* count of the number who have arrived */

int gridSize, numWorkers, numIters, stripSize;
double maxDiff[MAXWORKERS];
#include <sys/time.h>
double myms()
{
/* struct timeval { long        tv_sec;
            long        tv_usec;        };

struct timezone { int   tz_minuteswest;
             int        tz_dsttime;      };     */

        struct timeval tp;
        struct timezone tzp;
        int i;

        i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec * 1.e+6 + (double) tp.tv_usec);
}


/* main() -- read command line, initialize grids, and create threads
 *           when the threads are done, print the results */

int main(int argc, char *argv[]) {
  /* thread ids and attributes */
  pthread_t workerid[MAXWORKERS];
  pthread_attr_t attr;
  int i, j;
  double maxdiff = 0.0,tma=0.0;
  FILE *results;

  mai_init(argv[4]);
  
  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line and initialize grids */
  if (argc < 4) {
	printf("jacobi size n_thread n_iter\n");
	exit(0);
  }
  gridSize = atoi(argv[1]);
  numWorkers = atoi(argv[2]);
  numIters = atoi(argv[3]);
  stripSize = gridSize/numWorkers;
tma = myms();
  /* create the workers, then wait for them to finish */
  for (i = 0; i < numWorkers; i++)
    pthread_create(&workerid[i], &attr, Worker, (void *) i);
  for (i = 0; i < numWorkers; i++)
    pthread_join(workerid[i], NULL);
  tma = myms() -tma;
  printf("\nTotal %f",tma);

  /* print the results */
  for (i = 0; i < numWorkers; i++)
    if (maxdiff < maxDiff[i])
      maxdiff = maxDiff[i];
  printf("number of iterations:  %d\nmaximum difference:  %e\n",
          numIters, maxdiff);
}


/* Each Worker computes values in one strip of the grids.
 *    The main worker loop does two computations to avoid copying from
 *       one grid to the other.  */

void *Worker(void *arg) {
  int myid = (int) arg;
  double maxdiff, temp;
  int i, j, iters;
  int first, last;
  double **grid1;
  double **grid2;
  double tmp=0.0;
  
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
  tmp = myms();
 /*Since threads are allocating their own data, MAi allocator will place them in the same node of the Thread*/
  grid1 = (double**) mai_alloc_2D((stripSize+3),gridSize+3,sizeof(double),DOUBLE);
  grid2 = (double**) mai_alloc_2D((stripSize+3),gridSize+3,sizeof(double),DOUBLE);
  mai_bind_all_mynode(grid1);
  mai_bind_all_mynode(grid2);
  tmp = myms() - tmp;
  printf("\n Malloc%f",tmp);
  Barrier();
  InitializeGrids(grid1,grid2);

  /* determine first and last rows of my strip of the grids */

  for (iters = 1; iters <= numIters; iters++) {
    /* update my points */
    for (i = 1; i < stripSize; i++) {
      for (j = 1; j <= gridSize; j++) {
        grid2[i][j] = (grid1[i-1][j] + grid1[i+1][j] + 
                       grid1[i][j-1] + grid1[i][j+1]) * 0.25;
      }
    }
    Barrier();
    /* update my points again */
    for (i = 1; i < stripSize; i++) {
      for (j = 1; j <= gridSize; j++) {
        grid1[i][j] = (grid2[i-1][j] + grid2[i+1][j] +
               grid2[i][j-1] + grid2[i][j+1]) * 0.25;
      }
    }
    Barrier();
  }
  /* compute the maximum difference in my strip and set global variable */
  maxdiff = 0.0;
  for (i = 1; i <= stripSize; i++) {
    for (j = 1; j <= gridSize; j++) {
      temp = grid1[i][j]-grid2[i][j];
      if (temp < 0)
        temp = -temp;
      if (maxdiff < temp)
        maxdiff = temp;
    }
  }
  maxDiff[myid] = maxdiff;
}

void InitializeGrids(double **grid1, double **grid2) {
  /* initialize the grids (grid1 and grid2)
   * set boundaries to 1.0 and interior points to 0.0  */
  int i, j;

  for (i = 0; i <= stripSize; i++)
    for (j = 0; j <= gridSize+1; j++) {
      grid1[i][j] = 0.0;
      grid2[i][j] = 0.0;
    }
  for (i = 0; i <= stripSize; i++) {
    grid1[i][0] = 1.0;
    grid1[i][gridSize+1] = 1.0;
    grid2[i][0] = 1.0;
    grid2[i][gridSize+1] = 1.0;
  }
  for (j = 0; j <= gridSize+1; j++) {
    grid1[0][j] = 1.0;
    grid2[0][j] = 1.0;
    grid1[stripSize][j] = 1.0;
    grid2[stripSize][j] = 1.0;
  }
}

void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}
