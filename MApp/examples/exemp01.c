#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define X      516
#define Y      128
#define Xv     1000
#define LIMRX  32
#define LIMRY  16  
#define beta 0.1
#define alpha 10

int fcos(int x, double rom, int y)
{
  return (abs(x+y)/rom);
}

int comp(double fz)
{
  return (fz < alpha);
}

/*common variables*/
int nrom[X],vel[Xv][Y],aux;
double tem[X][Y];

int main()
{
 int xux[LIMRX][LIMRY],j,i,k,vxy[LIMRX][Xv];

int nrom[X];
int aux;
float ins[10];
double tem[X][Y];
int flag;
double acl[1024];
float grav, len[100][200];
double sum[1024];


#pragma omp parallel for shared(xux)
  for(i=1;i<X;i++)
   for(j=0;j<Y;j++) 
    {  xux[i][j] = len[i-1][j];
       acl[i]=beta;
    }  

#pragma omp parallel for private(j)
 for(i=1;i<LIMRX;i++)
   for(j=1;j<LIMRY;j++)
           vel[i-1][j-1]= fcos(xux[i][j],tem[i][j],vxy[i][j]);

#pragma omp parallel for private(j)
  for(i=X-1;i>=0;i--)
   for(j=Y-1;j>=0;j--)
      vel[i][j]= comp(tem[i][j]);
}
