/*
A synthetic parallel application to make experiences with MAI.

Author: Christiane Pousa
Data: 27-03-2009
Data of last update: 25-09-2009
*/

#include <math.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <mai.h>

#define N 1024

struct mydata{
int i;
double a;
};

int main(int argc, char* argv[])
{
  int i,j,k;
  struct mydata *t;
  double **tab;

  //set information about cpus and nodes explicity or MAi retrieves from numarch module
  mai_init(argv[1]);

  //allocation data
  t = (struct mydata*) mai_alloc_1D(N,sizeof(struct mydata),STRUCT);
  tab = (double**)mai_alloc_2D(N,N,sizeof(double),DOUBLE);

  /*Memory policies*/
   mai_next_touch(t);
   mai_bind_rows(tab);		
   
  for(i=0;i<N;i++){
    t[i].a=cos(i);
    t[i].i=0;
    for(j=0;j<N;j++)
      tab[i][j]=-1.0;
  }  
 
  //just some operations
#pragma omp parallel for private(j)
  for(i=0;i<N;i++)
    {   
       t[i].i= pow(i,2);
       for(j=1;j<N;j++) 	
         tab[i][j]=t[i].i*tab[i][j-1];
    }

  mai_cyclic(tab);

 //just some operations
#pragma omp parallel for private(j)
  for(i=N-1;i>=1;i--)
    {   
      for(j=0;j<N;j++) 	
         tab[i][j]=t[i].a*tab[i][j];
    }

  printf("\n");
 //finalize structures and library
 mai_final();

 //show nodes in use
 mai_show_nodes();

 //show all cpus in use
 mai_show_cpus();

}

