#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define X      516
#define Y      128
#define Z      128
#define Xv     1000
#define LIMRX  32
#define LIMRY  16  

int fcos(int x, double rom, int y)
{
  return (abs(x+y)/rom);
}

int comp(double fz)
{
  return (fz < ALPHA);
}

/*common variables*/
int nrom[X],vel[X][Y][Z];
double tem[X][Y];

int main()
{
 int xux[LIMRX][LIMRY],j,i,k,vxy[LIMRX][Xv];
double acl[1024];
float len[100][200];
float beta = 1;

#pragma omp parallel for
 for(i=1;i<X;i++)
   for(j=0;j<Y;j++)
     for(k=0;k<Z;k++)
       vel[i][j][k]=fcos(beta,1);
 
#pragma omp parallel for shared(xux)
  for(i=1;i<X;i++)
   for(j=0;j<Y;j++) 
    {  xux[i][j] = len[i-1][j];
       beta = comp(xux[i][j],ALPHA);
       acl[i]=beta;
    }  

}
