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
#define TEMP 10000       //number of application iterations
#define X 256
#define Y 256
#define Z 56

//Global and shared array
double ***tab;
int main(int argc, char* argv[])
{
  //  struct config information;
  double sum,b1,b2;
  int i,j,k,temp;


  sum=b1=b2=0.0;   

  //set information about cpus and nodes explicity or MAi retrieves from numarch module
  mai_init(argv[1]);

 //allocation data
 tab = mai_alloc_3D(X,Y,Z,sizeof(double),DOUBLE);
 mai_skew_mapp(tab);

#pragma omp parallel for default(shared) private(j,k) 
    for(i=0;i<X;i++)
     for(j=0;j<Y;j++)
       for(k=0;k<Z;k++)  
         tab[i][j][k]=1.0;
 
 
//--------------------------------------------------begin of computation
for(temp=0;temp<TEMP;temp++)
{
#pragma omp parallel for default(shared) private(j,k) firstprivate(sum,b1,b2)
  for(i=X-1;i>=0;i--)
    for(j=Y-1;j>=0;j--)
      for(k=Z-1;k>=0;k--)
      { 
       sum = tab[i][j][k]* 2.0;
       tab[i][j][k]= sum * 2.0;
       sum++;
       b2=sum;
       b1 += tab[i][j][k];
      }  

#pragma omp parallel for default(shared) private(j,k) firstprivate(sum,b1,b2)
 for(i=1;i<X;i++)
    for(j=1;j<Y;j++)
      for(k=0;k<Z-1;k++) 
	{  
          sum = tab[i-1][j-1][k]* 2.0;
	  tab[i][j][k]= sum * 2.0;
          sum++;
	  b2=sum;
	  b1 += tab[i][j][k+1];
        }
}
//-----------------------------------------------------------------------

mai_final();

return 0;
}
