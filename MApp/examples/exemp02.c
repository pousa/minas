#include <stdio.h>
#include <stdlib.h>

#include "incl1.h"
#include "incl2.h"

int main(){
 int xux[1024][10],i,j,k;
 double sum[1024];
 float beta = 1;

#pragma omp parallel for
 for(i=1;i<X;i++)
   for(j=0;j<Y;j++)
     for(k=0;k<Z;k++)
       vel[X][Y][Z]=0;
 
#pragma omp parallel for shared(xux)
  for(i=1;i<X;i++)
   for(j=0;j<Y;j++) 
    {  xux[i][j] = len[i-1][j];
       beta = 0;
       acl[i]=beta;
    }  

}

