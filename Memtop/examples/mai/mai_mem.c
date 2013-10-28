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
#include <sys/times.h>
#include <syscall.h>
#include <limits.h>
#include <unistd.h>

#include "mai.h"
#include "memon.h"


//defines
#define TEMP 100000        //number of application iterations
#define X 200
#define Y 200
#define Z 120

//Global and shared array
double ***tab;

int main(int argc, char* argv[])
{
  //  struct config information;
  double TimeI,TimeF,TII,TIF,TI;
  double sum,b1,b2;
  int i,j,k,temp;
  char msg[215]; 
  sum=b1=b2=0.0;   


  /* initialize memon */
   init_memon();


  mai_init(argv[1]);
    


  tab = mai_alloc_3D(X,Y,Z,sizeof(double),DOUBLE);
  mai_skew_mapp(tab);

/* attach memory to memtop */
  sprintf(msg,"array 3D");
  attach_memory(tid(),msg,tab[0][0],X*Y*Z*sizeof(double));

#pragma omp parallel for default(shared) private(j,k)  
    for(i=0;i<X;i++)
     for(j=0;j<Y;j++)
       for(k=0;k<Z;k++)  
         tab[i][j][k]=0.0;
 
  //-------------------------------------------------------------------


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

detach_memory(tid(),msg,tab[0][0]);
/* finalize memon */
  finalize_memon();

return 0;
}
