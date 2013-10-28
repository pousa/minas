/*
A synthetic parallel 3D data application to make experiences with MAI.

Author: Christiane Pousa
Data: 03-04-2008
Data of last update: 25-09-2009
*/

//libraries
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <mai.h>

//defines
#define TEMP 100        //number of application iterations
#define X 200
#define Y 200
#define Z 120

//Global and shared array
double ***tab;

int* compute_distance(int nnodes)
{
  int i,j,find;
  double *band,aux,high;
  unsigned long *nodes;
  band = malloc((nnodes-1)*sizeof(unsigned long));
  nodes = malloc((nnodes)*sizeof(unsigned long));

  nodes[0]=1;

  for(i=1;i<nnodes;i++)
   band[i-1]=mai_bandwidth(1,i+1);
 
 for(j=1;j<nnodes;j++) {
  high = band[0];
  find = 2;
  for(i=1;i<nnodes;i++){
   if(high < band[i]){
    high = band[i];
    find = i+1;
   }
    band[find-1]=-1;
    nodes[j]=find;
  }
  
  return nodes;
}

void my_firsttouch(void *array,int nthreads)
{
  int nnodes,*nodes;
   nnodes = mai_get_maxnodes();
 
  if(nthreads<=nnodes){
   nodes = compute_distance(nthreads);
   mai_nodes(nthreads,nodes);
  }
  else{
   nodes = compute_distance(nnodes);
   mai_nodes(nnodes,nodes);
  }
  
  mai_bytes(array,mai_getsize(array)/nthreads);
  mai_regularbind(array);
}

//----------------------------------------------------------------
int main(int argc, char* argv[])
{
  //  struct config information;
  double sum,b1,b2;
  int i,j,k,temp;
  size_t size;

  sum=b1=b2=0.0;   

 omp_set_num_threads(atoi(argv[1]));

  mai_init(NULL);

  tab = mai_alloc_3D(X,Y,Z,sizeof(double),DOUBLE);
  size = X*Y*Z*sizeof(double);
  my_firsttouch(tab,omp_get_num_threads());
 
#pragma omp parallel for default(shared) private(j,k)  
    for(i=0;i<X;i++)
     for(j=0;j<Y;j++)
       for(k=0;k<Z;k++)  
         tab[i][j][k]=0.0;
 
 
//--------------------------------------------------begin of computation
for(temp=0;temp<TEMP;temp++)
{
#pragma omp parallel for default(shared) private(j,k) firstprivate(sum,b1,b2)
  for(i=0;i<X;i++)
    for(j=0;j<Y;j++)
      for(k=0;k<Z;k++)
      { 
       sum = tab[i][j][k]* 2.0;
       tab[i][j][k]= sum * 2.0;
       sum++;
       b2=sum;
       b1 += tab[i][j][k];
      }  

#pragma omp parallel for default(shared) private(j,k) firstprivate(sum,b1,b2)
 for(i=0;i<X;i++)
    for(j=0;j<Y;j++)
      for(k=0;k<Z;k++) 
	{  
          sum = tab[i][j][k]* 2.0;
	  tab[i][j][k]= sum * 2.0;
          sum++;
	  b2=sum;
	  b1 += tab[i][j][k];
        }
}
//-----------------------------------------------------------------------

mai_final();

return 0;
}
