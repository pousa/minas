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
#include <pthread.h>

#include "common.h"
#include "memory_policies.h"
#include "thread_policies.h"

static int mai_sigsegv_handler_set = 0;
static int done;

/*
Made a round-robin with the memory pages of data pointed by void *p.
All numa nodes are used.
Round-robin starts in the node that causes the first page fault.
parameters: p-pointer to data structure  
return: none
*/
void mai_cyclic(void *p)
{
  int err=0,i,actualnnodes;
  unsigned long node=0,totalmem=0,freemem=0; 

   for(i=0;i<sinfo.nnodes;i++)
   {
     node += sinfo.nodes[i];
     freemem+=na_get_memnode(sinfo.lnodes[i]);
   }   
   actualnnodes = sinfo.nnodes;

   if(totalmem < (KBYTE*freemem)){ //have enough memory
    #ifdef MPOL_MF_MOVE
    err=mbind(p,get_var_tsize(p),MPOL_INTERLEAVE,
	    &node,MASK_SIZE,MPOL_MF_MOVE);   
    #else
    printf("\n Operating system do not support data migration!");
    exit(-1);
    #endif 
   } 
  else
    {
     printf("\n Nodes do not have enough memory!");
    }
    set_var_policy(p,CYCLIC);
    set_var_nodes(p,actualnnodes,sinfo.nodes);
  
  if(err < 0)
  {
    identify_error();     
    printf("\n function cyclic!");
  }
}

/*
Made a round-robin with the memory pages of data pointed by void *p.
Only numa nodes with threads running on it are used.
Round-robin starts in the node that causes the first page fault.
parameters: p-pointer to data structure  
return: none
*/
void mai_cyclic_possible(void *p)
{
  int err=0,i,actualnnodes;
  unsigned long node=0,totalmem=0,freemem=0; 
  int *nodes;

  nodes = malloc(sinfo.nnodes*sizeof(int));
  for(i=0;i<sinfo.nnodes;i++)
    nodes[i]=0;

  for(i=0;i<sinfo.nthreads;i++)
    nodes[na_get_nodeidcpu(sinfo.lcpus[i])]=1;

   for(i=0;i<sinfo.nnodes;i++)
   {
     if(nodes[i]){
       node += sinfo.nodes[i];
       freemem+=na_get_memnode(sinfo.lnodes[i]);
       actualnnodes++;
     }
   }   

   if(totalmem < (KBYTE*freemem)){ //have enough memory
    #ifdef MPOL_MF_MOVE
    err=mbind(p,get_var_tsize(p),MPOL_INTERLEAVE,
	    &node,MASK_SIZE,MPOL_MF_MOVE);   
    #else
    printf("\n Operating system do not support data migration!");
    exit(-1);
    #endif 
   } 
  else
    {
     printf("\n Nodes do not have enough memory!");
    }
    set_var_policy(p,CYCLIC);
    set_var_nodes(p,actualnnodes,sinfo.nodes);
  
  if(err < 0)
  {
    identify_error();     
    printf("\n function cyclic!");
  }
}


/*
Made a round-robin with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure
return: none
*/
void mai_skew_mapp(void *p)
{
  int err,i,j,block,great_int,actualnnodes=0,np=0;
  size_t size;
  unsigned long node,*acnodes=NULL,totalmem=0,freemem=0,nodes=0;
  void *t=NULL;
  unsigned long npag=NPAG;

  acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	
  size = get_var_tsize(p);
  np = size/PAGE_SIZE;
  block = np/npag;
  t=p;

  //optimize data placement considering the number of threads running in the application
  if(sinfo.nthreads == 0){
  pthread_mutex_lock(&(sinfo.lock_place));
    sinfo.nthreads = get_number_threads();
  pthread_mutex_unlock(&(sinfo.lock_place));}

  if(sinfo.nthreads>0 && sinfo.nthreads<=sinfo.nnodes)
    {
      for(i=0;i<sinfo.nthreads;i++)
       {
	acnodes[i]= sinfo.nodes[i];
        freemem+=na_get_memnode(sinfo.lnodes[i]);
        nodes += acnodes[i];
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
          nodes += acnodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++){
	acnodes[i] = sinfo.nodes[i];
        nodes +=acnodes[i];
         freemem+=na_get_memnode(sinfo.lnodes[i]);
      }
      actualnnodes = sinfo.nnodes;
    } 
    
    set_var_policy(p,SKEW);
    set_var_nodes(p,actualnnodes,acnodes);
  
  for(i=0;i<block;i++)
  {
    great_int = i/actualnnodes;
    node = acnodes[(i+great_int+1)%actualnnodes];

    if(totalmem < (KBYTE*freemem)){ //have enough memory
#ifdef MPOL_MF_MOVE
      err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
#else
      printf("\n Operating system do not support data migration!");
      exit(-1);
#endif 
    }
    else
     printf("\n Nodes do not have enough memory!");

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
void mai_prime_mapp(void *p,int prime)
{
 int err=0,i,rest=0,nitens,*ti,type,mem_node,npagi,pm_no,twork,j;
  size_t size;
  unsigned long node,nodes=0;
  void *t=NULL;
  unsigned long pagsize,block,np,npag=NPAG;

  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;

  acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	
  nitens = get_var_itens(p);
  size = get_var_tsize(p);
  type = get_var_type(p);

  np = (size)/PAGE_SIZE;
 
  block = np/npag;
  rest = np%npag;
  t = p;

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
        freemem+=na_get_memnode(sinfo.lnodes[i]);
        nodes += acnodes[i];
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
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
	freemem+=na_get_memnode(sinfo.lnodes[i]);
      }
      actualnnodes = sinfo.nnodes;
    }
   
    set_var_policy(p,PRIME);
    set_var_nodes(p,actualnnodes,acnodes); 
    
    t = get_var_pl(p);

 if(totalmem < (KBYTE*freemem)){ //have enough memory
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
	
   	 #ifdef MPOL_MF_MOVE
   		 err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
   	 #else
   		 printf("\n Operating system do not support data migration!");
    		 exit(-1);
    	#endif 
   	
    	  t = (npag*PAGE_SIZE)+ t;
     }

   if(err < 0)
    {
        identify_error();
        printf("\t function prime-mapp");
   }
 }
 else
   printf("\n Nodes do not have enough memory!");
}    


/*
Made an interleave with blocks(rows) of memory pages pointed by void *p.
Interleave starts in the first node in the sinfo list and it is
done considering sets of rows or columns.
parameters: p-pointer to data structure, nr-number of rows
return: none
*/
void mai_cyclic_rows(void *p,int nr)
{
  int err=0,i,j,nitens,icpt,twork;
  size_t size;
  unsigned long node,np,npag=1,rest=0,nodes=0;
  void *t=NULL;
  
  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;
 
  nitens = get_var_itens(p);
  size = get_var_tsize(p);

  np = (size)/PAGE_SIZE;

  if(nr!=0){
      for(i=2;get_var_dim(p,i)!=0;i++)
        npag *= get_var_dim(p,i);

      npag *= nr*get_var_size(p);
      npag = npag/PAGE_SIZE;

      rest = npag%PAGE_SIZE;
      if(rest !=0)
	npag++;

      icpt = np/npag;
 
         t = p;

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
        freemem+=na_get_memnode(sinfo.lnodes[i]);
        nodes += sinfo.nodes[i];
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
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
	freemem+=na_get_memnode(sinfo.lnodes[i]);
      }

      actualnnodes = sinfo.nnodes;
    } 

    set_var_policy(p,CYCLIC_B);
    set_var_nodes(p,actualnnodes,acnodes);
   
    
    for(i=0;i<icpt-1;i++)
       {
          node = acnodes[(i%actualnnodes)];
          
         	if(totalmem < (KBYTE*freemem)){ //have enough memory
   	 	#ifdef MPOL_MF_MOVE
    			err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
    		#else
    			printf("\n Operating system do not support data migration!");
    			exit(-1);
    		#endif 
   		}
   		else
	     	  printf("\n Nodes do not have enough memory!");
	    	
  

          t = (npag*PAGE_SIZE) + t;
       }

       //last mbind with less pages
       npag = np - npag*(icpt-1);
       node = acnodes[(i%actualnnodes)];

	  if(totalmem < (KBYTE*freemem)){ //have enough memory
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   }
	   else
	     printf("\n Nodes do not have enough memory!");

 }//end rows
}

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, nr-number of rows
return: none
*/
void mai_skew_mapp_rows(void *p,int nr)
{
  int err,i,np=0,npag,icpt,rest=0,nitens,*ti,great_int,j,twork;
  size_t size;
  unsigned long node,nodes=0;
  void *t=NULL;
  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;

  nitens = get_var_itens(p);
  size = get_var_tsize(p);

  np = (size)/PAGE_SIZE;

  if(nr != 0)
    {  
      for(i=2;get_var_dim(p,i)!=0;i++)
        npag *= get_var_dim(p,i);

      npag *= nr*get_var_size(p);
      npag = npag/PAGE_SIZE;

      rest = npag%PAGE_SIZE;
      if(rest !=0)
	npag++;

      icpt = np/npag;
      rest = np%npag;
      if(rest !=0)
	icpt++;
 
         t = p;

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
		freemem+=na_get_memnode(sinfo.lnodes[i]);
                nodes+=acnodes[i];
	       }

	      //verify if there is enough memory
	      totalmem=(get_var_itens(p)*get_var_size(p));
	      actualnnodes = sinfo.nthreads;
	      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
		{
		  freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
		  acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
		  nodes+=acnodes[actualnnodes];
		  actualnnodes++;
		}         
	    }
	  else //use all possible nodes
	    {
	      for(i=0;i<sinfo.nnodes;i++){
		acnodes[i] = sinfo.nodes[i];
                nodes+=acnodes[i];
		freemem+=na_get_memnode(sinfo.lnodes[i]);
	      }

	      actualnnodes = sinfo.nnodes;
	    }

	    set_var_policy(p,SKEW_B);
	    set_var_nodes(p,actualnnodes,acnodes);
	
        
       for(i=0;i<icpt-1;i++)
       {
	  great_int = i/sinfo.nnodes;
	  node = acnodes[(i+great_int+1)%actualnnodes];
          
	   if(totalmem < (KBYTE*freemem)){ //have enough memory
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   }
	   else
	     printf("\n Nodes do not have enough memory!");
	 
          t = (npag*PAGE_SIZE) + t;
       }

       //last mbind with less pages
       npag = np - npag*(icpt-1);
       node = acnodes[(i%actualnnodes)];

	   
	   if(totalmem < (KBYTE*freemem)){ //have enough memory
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   }
	   else
	     printf("\n Nodes do not have enough memory!");


    }//end rows


}

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, nr-number of rows
return: none
*/
void mai_prime_mapp_rows(void *p,int prime,int nr)
{
  int err=0,i,rest=0,nitens,*ti,mem_node,npagi,pm_no,twork,j;
  size_t size;
  unsigned long node;
  void *t=NULL;
  int icpt,np,npag;
  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;


  nitens = get_var_itens(p);
  size = get_var_tsize(p);

  np = (size)/PAGE_SIZE;
 
  if(nr != 0)
    {  
      for(i=2;get_var_dim(p,i)!=0;i++)
        npag *= get_var_dim(p,i);

      npag *= nr*get_var_size(p);
      npag = npag/PAGE_SIZE;

      rest = npag%PAGE_SIZE;
      if(rest !=0)
	npag++;

      icpt = np/npag;
      rest = np%npag;
      if(rest !=0)
	icpt++;
 
      t= p;

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
		freemem+=na_get_memnode(sinfo.lnodes[i]);
	       }

	      //verify if there is enough memory
	      totalmem=(get_var_itens(p)*get_var_size(p));
	      actualnnodes = sinfo.nthreads;
	      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
		{
		  freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
		  acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
		  actualnnodes++;
		}         
	    }
	  else //use all possible nodes
	    {
	      for(i=0;i<sinfo.nnodes;i++){
		acnodes[i] = sinfo.nodes[i];
		freemem+=na_get_memnode(sinfo.lnodes[i]);
              }
	      actualnnodes = sinfo.nnodes;
	    }
 	     
	    set_var_policy(p,PRIME_B);
	    set_var_nodes(p,actualnnodes,acnodes);

      

       for(i=0;i<icpt-1;i++)
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
          
	   if(totalmem < (KBYTE*freemem)){ //have enough memory
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   }
	   else
	     printf("\n Nodes do not have enough memory!");

          t = (npag*PAGE_SIZE) + t;
       }

       //last mbind with less pages
       npag = np - npag*(icpt-1);
       node = acnodes[(i%actualnnodes)];

	
	   if(totalmem < (KBYTE*freemem)){ //have enough memory
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   }
	   else
	     printf("\n Nodes do not have enough memory!");
	    

}//end rows
}    

/*
Made an interleave with blocks(columns) of memory pages pointed by void *p.
Interleave starts in the first node in the sinfo list and it is
done considering sets of rows or columns.
parameters: p-pointer to data structure, nc-number of columns
return: none
*/
void mai_cyclic_columns(void *p,int nc)
{
  int err=0,i,j,descColumn=1,nitens,icpt,twork;
  size_t size;
  unsigned long node,npRow=1;
  void *t=NULL;
  
  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;

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
		freemem+=na_get_memnode(sinfo.lnodes[i]);
	       }

	      //verify if there is enough memory
	      totalmem=(get_var_itens(p)*get_var_size(p));
	      actualnnodes = sinfo.nthreads;
	      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
		{
		  freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
		  acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
		  actualnnodes++;
		}         
	    }
	  else //use all possible nodes
	    {
	      for(i=0;i<sinfo.nnodes;i++)
		acnodes[i] = sinfo.nodes[i];

	      actualnnodes = sinfo.nnodes;
	    }
     	
	    set_var_policy(p,CYCLIC_B);
	    set_var_nodes(p,actualnnodes,acnodes);
             
  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(nc!=0){
    for(i=3;get_var_dim(p,i)!=0;i++)
	npRow  *= get_var_dim(p,i);

    npRow *= size*get_var_dim(p,2);
    npRow = npRow / PAGE_SIZE;
     
    for(i=3;get_var_dim(p,i)!=0;i++)
      descColumn *= get_var_dim(p,i);

    descColumn *= nc * size;
    descColumn = descColumn/PAGE_SIZE;

    
    if(t == NULL)
         t = p;
  
    for(j=0;j<get_var_dim(p,2)/nc;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          node = acnodes[j%actualnnodes];
	  
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
     
       
       if(t == NULL)
         t = p;

       t = t + ((j+1)*descColumn*PAGE_SIZE);
    }

   if(err < 0)
    {
      identify_error();
      printf("\t (function cyclic_columns)\n");
    }
  }

}

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, nc-number of columns
return: none
*/
void mai_skew_mapp_columns(void *p,int nc)
{
  int err=0,i,j,descColumn=1,nitens,great_int,twork;
  size_t size;
  unsigned long node,npRow=1;
  void *t=NULL;

  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(nc != 0)
    {  
      for(i=3;get_var_dim(p,i)!=0;i++)
	npRow  *= get_var_dim(p,i);

    npRow *= size*get_var_dim(p,2);
    npRow = npRow / PAGE_SIZE;

    for(i=3;get_var_dim(p,i)!=0;i++)
      descColumn *= get_var_dim(p,i);

    descColumn *= nc * size;
    descColumn = descColumn/PAGE_SIZE;

    
    if(t == NULL)
         t = p;
  
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
		freemem+=na_get_memnode(sinfo.lnodes[i]);
	       }

	      //verify if there is enough memory
	      totalmem=(get_var_itens(p)*get_var_size(p));
	      actualnnodes = sinfo.nthreads;
	      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
		{
		  freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
		  acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
		  actualnnodes++;
		}         
	    }
	  else //use all possible nodes
	    {
	      for(i=0;i<sinfo.nnodes;i++)
		acnodes[i] = sinfo.nodes[i];

	      actualnnodes = sinfo.nnodes;
	    }
  
	  if(get_var_policy(p) != SKEW_B && get_var_policy(p) != -1)
	    mig = 1; //data migration
	     	
	    set_var_policy(p,SKEW_B);
	    set_var_nodes(p,actualnnodes,acnodes);
	    

    for(j=0;j<get_var_dim(p,2)/nc;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          great_int = j/actualnnodes;
	  node = acnodes[(j+great_int+1)%actualnnodes];

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
     
       
       if(t == NULL)
         t = p;

       t = t + ((j+1)*descColumn*PAGE_SIZE);
    }//end rows

   if(err < 0)
    {
        identify_error();
        printf("\t function skew_mapp_columns");
    }
   }
 
}

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, nc-number of columns
return: none
*/
void mai_prime_mapp_columns(void *p,int prime,int nc)
{
  int err=0,i,j,descColumn=1,nitens,mem_node,npagi,pm_no;
  size_t size;
  unsigned long node,npRow=1;
  void *t=NULL;
  int actualnnodes=0,mig=0;
unsigned long *acnodes=NULL,totalmem=0,freemem=0;


  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(nc != 0)
    {  
       for(i=3;get_var_dim(p,i)!=0;i++)
	npRow  *= get_var_dim(p,i);

    npRow *= size*get_var_dim(p,2);
    npRow = npRow / PAGE_SIZE;

    for(i=3;get_var_dim(p,i)!=0;i++)
      descColumn *= get_var_dim(p,i);

    descColumn *= nc * size;
    descColumn = descColumn/PAGE_SIZE;

    
    if(t == NULL)
         t = p;

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
        freemem+=na_get_memnode(sinfo.lnodes[i]);
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++)
	acnodes[i] = sinfo.nodes[i];

      actualnnodes = sinfo.nnodes;
    }
  
  if(get_var_policy(p) != PRIME_B && get_var_policy(p) != -1)
    mig = 1; //data migration
     
    set_var_policy(p,PRIME_B);
    set_var_nodes(p,actualnnodes,acnodes);
    
    for(j=0;j<get_var_dim(p,2)/nc;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          npagi = j*descColumn;
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
     
       
       if(t == NULL)
         t = p;

       t = t + ((j+1)*descColumn*PAGE_SIZE);
    }//end rows

   if(err < 0)
    {
      identify_error();
      printf("\t (rest - function prime_mapp_columns)\n");
    }   
   }
 
}    

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, n3d-number of plans
return: none
*/
void mai_cyclic_3D(void *p,int n3d)
{
 int err=0,i,j,nblocks,nitens,mem_node,npagi,pm_no,nitens4d;
  size_t size;
  unsigned long node,shift=1;
  void *t=NULL;
  int actualnnodes=0,mig=0;
unsigned long *acnodes=NULL,totalmem=0,freemem=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(n3d != 0)
  {  
    //Are there any elements on 4 dimension?
    int temp = get_var_dim(p,4);
    nitens4d = (temp != 0) ? temp:1;

    shift = get_var_dim(p,1)*get_var_dim(p,2)*n3d*size*nitens4d;
    shift = shift/PAGE_SIZE;
    nblocks =  get_var_dim(p,3)/n3d;

    
    if(t == NULL)
         t = p;

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
        freemem+=na_get_memnode(sinfo.lnodes[i]);
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++)
	acnodes[i] = sinfo.nodes[i];

      actualnnodes = sinfo.nnodes;
    }
       
    set_var_policy(p,CYCLIC_B);
    set_var_nodes(p,actualnnodes,acnodes);

  

  if(totalmem < (KBYTE*freemem)){ //have enough memory
       for(j=0;j<nblocks;j++){
          node = acnodes[j%actualnnodes];

	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	  
       
       if(t == NULL)
         t = p;

       //moving pointer to n3d plan
       t = t + ((j+1)*shift*PAGE_SIZE);
     }
   }
  else
     printf("\n Nodes do not have enough memory!");

   }//end columns

}

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, n3d-number of plans
return: none
*/
void mai_skew_mapp_3D(void *p,int n3d)
{
    int err=0,i,j,nblocks,nitens,mem_node,great_int,nitens4d;
  size_t size;
  unsigned long node,shift=1;
  void *t=NULL;
  int actualnnodes=0,mig=0;
unsigned long *acnodes=NULL,totalmem=0,freemem=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(n3d != 0)
  {  
    //Are there any elements on 4 dimension?
    int temp = get_var_dim(p,4);
    nitens4d = (temp != 0) ? temp:1;

    shift = get_var_dim(p,1)*get_var_dim(p,2)*n3d*size*nitens4d;
    shift = shift/PAGE_SIZE;
    nblocks =  get_var_dim(p,3)/n3d;

    
    if(t == NULL)
         t = p;

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
        freemem+=na_get_memnode(sinfo.lnodes[i]);
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++)
	acnodes[i] = sinfo.nodes[i];

      actualnnodes = sinfo.nnodes;
    }

    set_var_policy(p,SKEW_B);
    set_var_nodes(p,actualnnodes,acnodes);
  
    for(j=0;j<nblocks;j++){
          great_int = j/actualnnodes;
	  node = acnodes[(j+great_int+1)%actualnnodes];

	   if(mig)//changing memory policy
	   {
	   if(totalmem < (KBYTE*freemem)){ //have enough memory
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
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
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
     
       
       if(t == NULL)
         t = p;

       //moving pointer to n3d plan
       t = t + ((j+1)*shift*PAGE_SIZE);
    }
 
   if(err < 0)
    {
      identify_error();
      printf("\t (Function skew_mapp_3d)\n");
    }   
   }

}


/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, n3d-number of plans
prime-number of virtual nodes
return: none
*/
void mai_prime_mapp_3D(void *p,int prime,int n3d)
{
  int err=0,i,j,nblocks,nitens,mem_node,npagi,pm_no,nitens4d;
  size_t size;
  unsigned long node,shift=1;
  void *t=NULL;
  int actualnnodes=0,mig=0;
unsigned long *acnodes=NULL,totalmem=0,freemem=0;

 
  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(n3d != 0)
  {  
    //Are there any elements on 4 dimension?
    int temp = get_var_dim(p,4);
    nitens4d = (temp != 0) ? temp:1;

    shift = get_var_dim(p,1)*get_var_dim(p,2)*n3d*size*nitens4d;
    shift = shift/PAGE_SIZE;
    nblocks =  get_var_dim(p,3)/n3d;

    
    if(t == NULL)
         t = p;

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
        freemem+=na_get_memnode(sinfo.lnodes[i]);
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++)
	acnodes[i] = sinfo.nodes[i];

      actualnnodes = sinfo.nnodes;
    }
 
  if(get_var_policy(p) != PRIME_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,PRIME_B);
    set_var_nodes(p,actualnnodes,acnodes);
  
    for(j=0;j<nblocks;j++){
          npagi = j*shift;
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
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
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
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
     
       
       if(t == NULL)
         t = p;

       //moving pointer to n3d plan
       t = t + ((j+1)*shift*PAGE_SIZE);
    }
    
   if(err < 0)
    {
      identify_error();
      printf("\t (Function prime_mapp_3d)\n");
    }   
   }
 
}

/*
Made a bind with memory pages pointed by void *p.
Bind is donne in the nodes contained in variable nodes .
parameters: p-pointer to data structure
return: none
*/
void mai_bind_all(void *p)
{
  int err=0,i,nitens,actualnnodes;
  unsigned long nodes=0,totalmem=0,freemem=0; 
  size_t size;

  nitens = get_var_itens(p);
  size = get_var_tsize(p);

//optimize data placement considering the number of threads running in the application!
pthread_mutex_lock(&(sinfo.lock_place));
  if(sinfo.nthreads == 0)
    sinfo.nthreads = get_number_threads();
  pthread_mutex_unlock(&(sinfo.lock_place));

  if(sinfo.nthreads>0 && sinfo.nthreads<sinfo.nnodes)
    {
      for(i=0;i<sinfo.nthreads;i++)
       {
	nodes += sinfo.nodes[i];
        freemem+=na_get_memnode(sinfo.lnodes[i]);
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
          nodes += sinfo.nodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else
    {
      for(i=0;i<sinfo.nnodes;i++){
	nodes += sinfo.nodes[i];
        freemem+=na_get_memnode(sinfo.lnodes[i]);}
     actualnnodes = sinfo.nnodes;
    }

  if(get_var_policy(p) != BIND && get_var_policy(p) != -1)//changing memory policy
  {
   if(totalmem < (KBYTE*freemem)){ //have enough memory
    #ifdef MPOL_MF_MOVE
    err=mbind(p,size,MPOL_BIND,&nodes,MASK_SIZE,MPOL_MF_MOVE);
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
  else //applying BIND_ALL memory policy
  {
    err=err=mbind(p,size,MPOL_BIND,&nodes,MASK_SIZE,0);
    set_var_policy(p,BIND);
    set_var_nodes(p,actualnnodes,sinfo.nodes);
  }
   
  if(err < 0){
    identify_error(); 
    printf("\n function bind_all!");
  }

}

/*
Made a bind with memory pages pointed by void *p.
Bind is donne in the nodes contained in variable nodes .
parameters: p-pointer to data structure
return: none
*/
void mai_bind_all_mynode(void *p)
{
  int err=0,i,nitens;
  unsigned long totalmem=0,freemem=0; 
  size_t size;
  unsigned long node,nodes[1];
  struct var_info *aux=NULL;

  get_lock(p);
  nodes[0]= node = na_get_nodeidcpu(get_thread_cpu());
  freemem = na_get_memnode(node);
  node = 1 << node;

  nitens = get_var_itens(p);
  size = get_var_tsize(p);

  totalmem = size;

   if(totalmem < (KBYTE*freemem)){ //have enough memory
    #ifdef MPOL_MF_MOVE
    err=mbind(p,size,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);
    release_lock(p);
    set_var_nodes(p,1,node);
    set_var_policy(p,BIND);
    #else
    printf("\n Operating system do not support data migration!");
    exit(-1);
    #endif 
   }
   else
    {
     printf("\n Nodes do not have enough memory!");
    }
 
  if(err < 0){
    identify_error(); 
    printf("\n function bind_all!");
  }
}

void mai_cyclic_neighbors(void *p)
{
  int err=0,i,block,nitens,*ti,type;
  size_t size;
  unsigned long node,nodes=0;
  unsigned long pagsize,np=0,rest=0;
  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,freemem=0;

  acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	

      for(i=0;i<sinfo.nnodes;i++){
	acnodes[i] = sinfo.nodes[i];
        nodes += acnodes[i];
	freemem+=na_get_memnode(sinfo.lnodes[i]);
       }
      actualnnodes = sinfo.nnodes;

   get_lock(p);
   done = 0;
   set_var_policy(p,NEXTOUCHN);
   set_var_nodes(p,actualnnodes,acnodes);

   size = get_var_tsize(p);

	  np = (size)/PAGE_SIZE;

	  if(!(REST(size)))
	     np++;

  	  block = np/actualnnodes;

    if(size < (KBYTE*freemem)){ //have enough memory
         if(!mai_sigsegv_handler_set)
         {
          struct sigaction mai_inst;
          pthread_mutex_lock(&mai_mutex); 
          mai_sigsegv_handler_set = 1;
          sigemptyset (&mai_inst.sa_mask);
          mai_inst.sa_flags = SA_SIGINFO;
          mai_inst.sa_sigaction = mai_handler;
          err = sigaction(SIGSEGV, &mai_inst, NULL);         
          if (err < 0)    printf("\t (Error on cyclic neighbors memory policy - signal not recongnized)\n");
         }
         set_var_block_size(p,block);
         err = mprotect(p,get_var_tsize(p), PROT_NONE);
         if (err < 0)    printf("\t (Error on cyclic neighbors memory policy - mprotect not working)\n");
         pthread_mutex_unlock(&mai_mutex);
     }
    else
        printf("\n Nodes do not have enough memory!");
    release_lock(p);
}


void mai_bind_rows_mynode(void *p)
{
  int err=0,i,block,nitens,*ti,type;
  size_t size;
  unsigned long node,nodes=0;
  unsigned long pagsize,np=0,rest=0;
  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,freemem=0;

  acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	

      for(i=0;i<sinfo.nnodes;i++){
	acnodes[i] = sinfo.nodes[i];
        nodes += acnodes[i];
	freemem+=na_get_memnode(sinfo.lnodes[i]);
       }
      actualnnodes = sinfo.nnodes;

get_lock(p);
   set_var_policy(p,BIND_B);
   set_var_nodes(p,actualnnodes,acnodes);

   size = get_var_tsize(p);

	  np = (size)/PAGE_SIZE;

	  if(!(REST(size)))
	     np++;

  	  block = np/actualnnodes;

    if(size < (KBYTE*freemem)){ //have enough memory
         if(!mai_sigsegv_handler_set)
         {
          struct sigaction mai_inst;
         pthread_mutex_lock(&mai_mutex); 
         mai_sigsegv_handler_set = 1;
         sigemptyset (&mai_inst.sa_mask);
         mai_inst.sa_flags = SA_SIGINFO;
         mai_inst.sa_sigaction = mai_handler;
         err = sigaction(SIGSEGV, &mai_inst, NULL);
          
         if (err < 0)    printf("\t (Error on bind_rows_mynode memory policy - signal not recongnized)\n");
         }
         set_var_block_size(p,block);
         err = mprotect(p,get_var_tsize(p), PROT_NONE);
         if (err < 0)    printf("\t (Error on bind_rows_mynode memory policy - mprotect not working)\n");
         pthread_mutex_unlock(&mai_mutex);
     }
    else
        printf("\n Nodes do not have enough memory!");
release_lock(p);
}

void mai_bind_page(void *p)
{
   int err=0,i,block,nitens,*ti,type;
  size_t size;
  unsigned long node,nodes=0;
  void *t=NULL;
  unsigned long pagsize,np=0,rest=0;
  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;

  acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	

      for(i=0;i<sinfo.nnodes;i++){
	freemem+=na_get_memnode(sinfo.lnodes[i]);
	acnodes[i] = sinfo.nodes[i];
        nodes += acnodes[i];
       }
      actualnnodes = sinfo.nnodes;
 get_lock(p);    
    set_var_policy(p,BIND_B);
    set_var_nodes(p,actualnnodes,acnodes);

   nitens = get_var_itens(p);
   size = get_var_tsize(p);
   type = get_var_type(p);

	  np = (size)/PAGE_SIZE;

	  if(!(REST(size)))
	     np++;

  	  block = np/actualnnodes;
	  t = p;

    if(size < (KBYTE*freemem)){ //have enough memory
         if(!mai_sigsegv_handler_set)
         {
          struct sigaction mai_inst;
         pthread_mutex_lock(&mai_mutex); 
         mai_sigsegv_handler_set = 1;
         sigemptyset (&mai_inst.sa_mask);
         mai_inst.sa_flags = SA_SIGINFO;
         mai_inst.sa_sigaction = mai_handler;
         err = sigaction(SIGSEGV, &mai_inst, NULL);
          
         if (err < 0)    printf("\t (Error on bind-page memory policy - signal not recongnized)\n");
         }
         set_var_block_size(p,PAGE_SIZE);
         err = mprotect(p,get_var_tsize(p), PROT_NONE);
         if (err < 0)    printf("\t (Error on bind-page memory policy - mprotect not working)\n");
         pthread_mutex_unlock(&mai_mutex);
      }
 	else
   	     printf("\n Nodes do not have enough memory!");
release_lock(p);
}

void mai_bind_columns_mynode(void *p)
{
  int err=0,i,j,descColumn=1,nc,nitens,*ti;
  size_t size;
  void *t=NULL;
  unsigned long node,nodes=0;
  unsigned long pagsize,npRow=1,npag,rest=0;

int actualnnodes=0,mig=0;
unsigned long *acnodes=NULL,totalmem=0,freemem=0;

acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	

      for(i=0;i<sinfo.nnodes;i++){
	acnodes[i] = sinfo.nodes[i];
	nodes += acnodes[i];
	freemem+=na_get_memnode(sinfo.lnodes[i]);
	}

      actualnnodes = sinfo.nnodes;
get_lock(p);
   
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
  
  
    if(t == NULL)
         t = p;
  
    for(j=0;j<actualnnodes;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          node = sinfo.nodes[j];
	  if(!mai_sigsegv_handler_set)
          {
          struct sigaction mai_inst;
         pthread_mutex_lock(&mai_mutex); 
         mai_sigsegv_handler_set = 1;
         sigemptyset (&mai_inst.sa_mask);
         mai_inst.sa_flags = SA_SIGINFO;
         mai_inst.sa_sigaction = mai_handler;
         err = sigaction(SIGSEGV, &mai_inst, NULL);
         pthread_mutex_unlock(&mai_mutex); 
         if (err < 0)    printf("\t (Error on next-touch memory policy - signal not recongnized)\n");
         }
         set_var_block_size(p,descColumn*PAGE_SIZE); 
         err = mprotect(t,descColumn*PAGE_SIZE, PROT_NONE);
	 t = (npRow*PAGE_SIZE) + t; 	

   	 }	  
    
         t = p;

       t = t + ((j+1)*descColumn*PAGE_SIZE);
    }

    if(err < 0)
    {
      identify_error();
      printf("\t (function bind_columns_mynode)");
    } 
release_lock(p);
}

/*
Made a bind with blocks(rows) of memory pages pointed by void *p.
Bind is donne in the nodes contained in sinfo and starts with the
first node in the sinfo .
parameters: p-pointer to data structure
return: none
*/
void mai_bind_rows(void *p)
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
pthread_mutex_lock(&(sinfo.lock_place));
  if(sinfo.nthreads == 0)
    sinfo.nthreads = get_number_threads();
  pthread_mutex_unlock(&(sinfo.lock_place));

  if(sinfo.nthreads>0 && sinfo.nthreads<sinfo.nnodes)
    {
      for(i=0;i<sinfo.nthreads;i++)
       {
	acnodes[i]= sinfo.nodes[i];
        freemem+=na_get_memnode(sinfo.lnodes[i]);
        nodes += acnodes[i];
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
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
        freemem+=na_get_memnode(sinfo.lnodes[i]);
      }
      actualnnodes = sinfo.nnodes;
    
    }

   size = get_var_tsize(p);

	  np = (size)/PAGE_SIZE;

	  if(!(REST(size)))
	     np++;

	  block = np/actualnnodes;
	  t = p;

       for(i=0;i<actualnnodes;i++)
       {
          node = acnodes[i];

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
void mai_bind_columns(void *p)
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
        freemem+=na_get_memnode(sinfo.lnodes[i]);
       }

      //verify if there is enough memory
      totalmem=(get_var_tsize(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
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
        freemem+=na_get_memnode(sinfo.lnodes[i]);  
	}

      actualnnodes = sinfo.nnodes;
    }
   
  size=0;
  int ndim = get_var_ndim(p);
    
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

  t = p;

    for(j=0;j<actualnnodes;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          node = sinfo.nodes[j];
	  
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
	  
         t = (npRow*PAGE_SIZE) + t; 	
       }   
     
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
Made a bind with blocks(columns) of memory pages pointed by void *p.
Bind is donne in the nodes contained in sinfo and starts with the
first node in the sinfo .
parameters: p-pointer to data structure
return: none
*/
void mai_bind_3D(void *p)
{
    int err=0,i,j,nblocks,nitens,mem_node,npagi,pm_no,nitens4d;
  size_t size;
  unsigned long node,shift=1;
  void *t=NULL;
  int actualnnodes=0,mig=0;
unsigned long *acnodes=NULL,totalmem=0,freemem=0;

 
  nitens = get_var_itens(p);
  size = get_var_size(p);

    //Are there any elements on 4 dimension?
    int temp = get_var_dim(p,4);
    nitens4d = (temp != 0) ? temp:1;

    
    if(t == NULL)
         t = p;

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
        freemem+=na_get_memnode(sinfo.lnodes[i]);
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++)
	acnodes[i] = sinfo.nodes[i];

      actualnnodes = sinfo.nnodes;
    }
 
   //setting number of blocks on 3d
    shift = get_var_dim(p,1)*get_var_dim(p,2)*size*nitens4d;
    shift = shift/PAGE_SIZE;
    nblocks =  get_var_dim(p,3)/actualnnodes;

    set_var_policy(p,BIND_B);
    set_var_nodes(p,actualnnodes,acnodes);

        

    //setting number of blocks on 3d
    shift = get_var_dim(p,1)*get_var_dim(p,2)*size*nitens4d;
    shift = shift/PAGE_SIZE;
    nblocks =  get_var_dim(p,3)/actualnnodes;
  
    for(j=0;j<actualnnodes;j++){
           node = acnodes[j];

	   if(totalmem < (KBYTE*freemem)){ //have enough memory
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,nblocks*shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   }
	   else
	     printf("\n Nodes do not have enough memory!");

       
       if(t == NULL)
         t = p;

       //moving pointer to n3d plan
       t = t + ((j+1)*shift*nblocks*PAGE_SIZE);
    } 
}

/*
Made a bind with blocks(rows) of memory pages pointed by void *p.
Bind is donne in the nodes contained in sinfo and starts with the
first node in the sinfo .
parameters: p-pointer to data structure,nr-number of rows
return: none
*/
void mai_ibind_rows(void *p,int nr)
{
   int err=0,i,block,nitens,*ti,type;
  size_t size;
  unsigned long node;
  void *t=NULL;
  unsigned long pagsize,np=0,rest=0;
  int actualnnodes=0,mig=0;
  unsigned long *acnodes=NULL,totalmem=0,freemem=0;

   if(nr!=0){
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
		freemem+=na_get_memnode(sinfo.lnodes[i]);
	       }

	      //verify if there is enough memory
	      totalmem=(get_var_itens(p)*get_var_size(p));
	      actualnnodes = sinfo.nthreads;
	      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
		{
		  freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
		  acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
		  actualnnodes++;
		}         
	    }
	  else //use all possible nodes
	    {
	      for(i=0;i<sinfo.nnodes;i++)
		acnodes[i] = sinfo.nodes[i];

	      actualnnodes = sinfo.nnodes;
	    }
 	  
	  if(get_var_policy(p) != BIND_B && get_var_policy(p) != -1)
	    mig = 1; //data migration

	    set_var_policy(p,BIND_B);
	    set_var_nodes(p,actualnnodes,acnodes);

		  size = get_var_size(p);
		  type = get_var_type(p);
                 
  		    for(i=2;get_var_dim(p,i)!=0;i++)
   			 nitens *= get_var_dim(p,i);

		  np = (size*nr*nitens)/PAGE_SIZE;

		  if(!(REST(size*nitens*nr)))
		     np++;

		  block = get_var_dim(p,1)/nr;
		
		    
		    if(t == NULL)
			 t = p;
		  
	       for(i=0;i<block;i++)
	       {
		  node = acnodes[i%actualnnodes];

		 if(mig)//changing memory policy
		   {
		   if(totalmem < (KBYTE*freemem)){ //have enough memory
		    #ifdef MPOL_MF_MOVE
		    err=mbind(t,np*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE); 
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
		    err=mbind(t,np*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
		  }
		  t = (np*PAGE_SIZE) + t; 	
	       }   
 
	    if(err < 0)
	    {
	      identify_error();
	      printf("\t (function bind_rows)");
	    }
	}    
}

/*
Made a bind with blocks(columns) of memory pages pointed by void *p.
Bind is donne in the nodes contained in sinfo and starts with the
first node in the sinfo .
parameters: p-pointer to data structure,nc-number of columns
return: none
*/
void mai_ibind_columns(void *p,int nc)
{
  int err=0,i,j,descColumn=1,nitens,*ti;
  size_t size;
  unsigned long node;
  void *t=NULL;
  unsigned long pagsize,npRow=1,npag,rest=0;

int actualnnodes=0,mig=0;
unsigned long *acnodes=NULL,totalmem=0,freemem=0;

   if(nc!=0){
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
        freemem+=na_get_memnode(sinfo.lnodes[i]);
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++)
	acnodes[i] = sinfo.nodes[i];

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

  for(i=3;get_var_dim(p,i)!=0;i++)
    descColumn *= get_var_dim(p,i);

  descColumn *= nc * size;
  descColumn = descColumn/PAGE_SIZE;
  
  
    if(t == NULL)
         t = p;
   
   int tcol = get_var_dim(p,2);

    for(j=0;j<tcol/nc;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          node = sinfo.nodes[j%actualnnodes];
	  
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
}


/*
Made a bind with blocks(plans) of memory pages pointed by void *p.
Bind is donne in the nodes contained in sinfo and starts with the
first node in the sinfo .
parameters: p-pointer to data structure, n3d-number of plans
return: none
*/
void mai_ibind_3D(void *p,int n3d)
{

    int err=0,i,j,nblocks,nitens,mem_node,npagi,pm_no,nitens4d;
  size_t size;
  unsigned long node,shift=1;
  void *t=NULL;
  int actualnnodes=0,mig=0;
unsigned long *acnodes=NULL,totalmem=0,freemem=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(n3d != 0)
  {  
    //Are there any elements on 4 dimension?
    int temp = get_var_dim(p,4);
    nitens4d = (temp != 0) ? temp:1;

    
    if(t == NULL)
         t = p;

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
        freemem+=na_get_memnode(sinfo.lnodes[i]);
       }

      //verify if there is enough memory
      totalmem=(get_var_itens(p)*get_var_size(p));
      actualnnodes = sinfo.nthreads;
      while(totalmem > (KBYTE*freemem) && actualnnodes<sinfo.nnodes)
	{
          freemem+=na_get_memnode(sinfo.lnodes[actualnnodes]);
          acnodes[actualnnodes] = sinfo.nodes[actualnnodes];
	  actualnnodes++;
	}         
    }
  else //use all possible nodes
    {
      for(i=0;i<sinfo.nnodes;i++)
	acnodes[i] = sinfo.nodes[i];

      actualnnodes = sinfo.nnodes;
    }
 
  if(get_var_policy(p) != BIND_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,BIND_B);
    set_var_nodes(p,actualnnodes,acnodes);

   //setting number of blocks on 3d
    shift = get_var_dim(p,1)*get_var_dim(p,2)*n3d*size*nitens4d;
    shift = shift/PAGE_SIZE;
    nblocks =  get_var_dim(p,3)/n3d;
  
    for(j=0;j<nblocks;j++){
           node = acnodes[j%actualnnodes];

	   if(mig)//changing memory policy
	   {
	   if(totalmem < (KBYTE*freemem)){ //have enough memory
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
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
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
     
       
       if(t == NULL)
         t = p;

       //moving pointer to n3d plan
       t = t + ((j+1)*shift*PAGE_SIZE);
    }
 
   if(err < 0)
    {
      identify_error();
      printf("\t (Function bind_3d)\n");
    }   
   }
}

/*Next-touch memory policy
parameters: p-pointer to data structure
return: none
*/
void mai_nexttouch_neighbors(void *p)
{
  int err=0,i,block,actualnnodes;
  size_t size,freemem=0;
  unsigned long np=0;
  unsigned long *acnodes=NULL ;

  acnodes = malloc(sinfo.nnodes*sizeof(unsigned long));	

      for(i=0;i<sinfo.nnodes;i++){
	acnodes[i] = sinfo.nodes[i];
	freemem+=na_get_memnode(sinfo.lnodes[i]);
       }
      actualnnodes = sinfo.nnodes;

get_lock(p);
 
    set_var_policy(p,NEXTOUCHN);
    set_var_nodes(p,actualnnodes,acnodes);

   size = get_var_tsize(p);

	  np = (size)/PAGE_SIZE;

	  if(!(REST(size)))
	     np++;

  	  block = np/actualnnodes;

    if(size < (KBYTE*freemem)){ //have enough memory
         if(!mai_sigsegv_handler_set)
         {
         struct sigaction mai_inst;
         pthread_mutex_lock(&mai_mutex); 
         mai_sigsegv_handler_set = 1;
         sigemptyset (&mai_inst.sa_mask);
         mai_inst.sa_flags = SA_SIGINFO;
         mai_inst.sa_sigaction = mai_handler;
         err = sigaction(SIGSEGV, &mai_inst, NULL);
          
         if (err < 0)    printf("\t (Error on bind_rows_mynode memory policy - signal not recongnized)\n");
         }
         set_var_block_size(p,block);
         err = mprotect(p,get_var_tsize(p), PROT_NONE);
         if (err < 0)    printf("\t (Error on Nexttouch neighbors memory policy - mprotect not working)\n");
         pthread_mutex_unlock(&mai_mutex);
     }
    else
        printf("\n Nodes do not have enough memory!");
release_lock(p);
}

/*Next-touch memory policy
parameters: p-pointer to data structure
return: none
*/
void mai_next_touch(void *p)
{
 int err;
  if(!mai_sigsegv_handler_set)
  {
      struct sigaction mai_inst;
       mai_sigsegv_handler_set = 1;
       sigemptyset (&mai_inst.sa_mask);
       mai_inst.sa_flags = SA_SIGINFO;
       mai_inst.sa_sigaction = mai_handler;
       err = sigaction(SIGSEGV, &mai_inst, NULL);
       if (err < 0)    printf("\t (Error on next-touch memory policy - signal not recongnized)\n");
  }
printf("NT pointer %p\n",p);
 get_lock(p);
 done = 0;
 set_var_policy(p,NEXTOUCH);
 set_var_block_size(p,0);
 err = mprotect(p,get_var_tsize(p), PROT_NONE);
      if (err < 0)    printf("\t (Error on next-touch memory policy - mprotect not working)\n");
release_lock(p);
}

void mai_handler(int sig, siginfo_t *info, void *_context) {
  void *p,*real_p,*initp_block;
  unsigned long node=0;
  unsigned long cpu;
  int mynode,err,i;
  size_t block_size=0;

  p = info->si_addr; 

  printf("%p\n",p);
  real_p = get_true_pointer_handler(p);
  printf("%p\n",real_p);
  get_lock(real_p);
  printf("Got lock\n");

  if (real_p == NULL) {
       struct sigaction mai_inst;
       mai_sigsegv_handler_set = 0;
       sigemptyset (&mai_inst.sa_mask);
       mai_inst.sa_flags = SA_SIGINFO;
       mai_inst.sa_sigaction = SIG_DFL;
       err = sigaction(SIGSEGV, &mai_inst, NULL);
  }
  else{
    block_size = get_var_block_size(real_p);
    if(block_size == 0 ){//&& !done){  // next-touch
    	node = na_get_nodeidcpu(get_thread_cpu());
        set_var_nodes(real_p,1,node);
    	mai_migrate_gather(real_p,node);
    	err = mprotect(real_p, get_var_tsize(real_p), PROT_READ|PROT_WRITE);
    }
    else{ 
       initp_block = get_pointer_block(p,real_p,block_size);
       mynode = na_get_nodeidcpu(get_thread_cpu());
       if(get_var_policy(real_p)==NEXTOUCHN && !done){//Next-touch neighbors
	 for(i=0;i<sinfo.nneighbors;i++) node += (1 << sinfo.neighbors[mynode][i]);
	 err=mbind(initp_block,block_size,MPOL_INTERLEAVE,&node,MASK_SIZE,MPOL_MF_MOVE);
         release_lock(real_p);
       }
       else{ //bind_block
	 node = 1 << na_get_nodeidcpu(get_thread_cpu());
	 err=mbind(initp_block,block_size,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE); }
         err = mprotect(initp_block,block_size*PAGE_SIZE, PROT_READ|PROT_WRITE);
         release_lock(real_p);
	 mai_barrier();
      }	
    }

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
void mai_migrate(void *p,unsigned long node,int nr, int startr,int nc, int startc)
{
  int *status=NULL,*nodes=NULL,find=0;
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

      
      if(t == NULL)
         t = p;
      
      for(i=0;i<startr;i++)
         t = (npRow*PAGE_SIZE) + t; 
      for(i=0;i<startc;i++)
	t = (num_pages_c*PAGE_SIZE) + t; 

	//computing total memory to be migrated and free memory on the node
	totalmem=descColumn*PAGE_SIZE*nr;
        freemem=na_get_memnode(convert_node(node));
        
      if(totalmem < freemem){ //if there are enough memory
 
get_lock(p);
       for(i=0;i<nr;i++)
       {
          #ifdef MPOL_MF_MOVE 
          node = 1 << node;
	  err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);
          #else
	  printf("\t Operating system do not support data migration");
          #endif 
	  t = (npRow*PAGE_SIZE) + t; 	
       }  
release_lock(p);
 }
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
void mai_migrate_rows(void *p,unsigned long node,int nr, int start)
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

  
    if(t == NULL)
         t = p;

    for(i=0;i<start;i++)
      t = t + (npRow*PAGE_SIZE);

  //computing total memory to be migrated and free memory on the node
	totalmem=npRow*nr*PAGE_SIZE;
        freemem=na_get_memnode(convert_node(node));

if(totalmem <freemem) //there are enough memory
{
  get_lock(p);
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
   release_lock(p);
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
void mai_migrate_columns(void *p,unsigned long node,int nc, int start)
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

  
    if(t == NULL)
         t = p;

    t = t + ((start+1)*descColumn*PAGE_SIZE);  

//computing total memory to be migrated and free memory on the node
	totalmem=descColumn*nc*PAGE_SIZE;
        freemem=na_get_memnode(convert_node(node));

if(totalmem <freemem) //there are enough memory
{
  get_lock(p);
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
  release_lock(p);
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
void mai_migrate_scatter(void *p,unsigned long nodes)
{ 
  int err=0,nitens,i,*acnodes=NULL,nnodes=0;
  size_t size;
  unsigned long totalmem=0,freemem=0,*mask; 
  mask = &nodes;
  acnodes = (int *) malloc(na_get_maxnodes()*sizeof(int));

  
  nitens = get_var_itens(p);
  totalmem = get_var_tsize(p);

 for(i=0;i<sinfo.nnodes;i++)
  {
	if(bit_is_marked(mask,i)){
          acnodes[nnodes]=i;
          freemem+=na_get_memnode(sinfo.lnodes[i]);
          nnodes++;
	}
  }  

 if(totalmem <freemem)
 {
   get_lock(p);
  //set node
  set_var_nodes(p,nnodes,acnodes);

 #ifdef MPOL_MF_MOVE
 err=mbind(p,totalmem,MPOL_INTERLEAVE,&nodes,MASK_SIZE,MPOL_MF_MOVE);
 #else   
 printf("\t function migrate_scatter - Linux Kernel version not updated!");
 #endif
 
 if(err < 0){
    identify_error();
    printf("\t function migrate_scatter");
   }
 release_lock(p);
 }
 else
  printf("\t Node does not have enough memory!");  
}

/*
Migrate all memory pages pointed by void *p to a new node.
parameters: p-pointer to data structure, node-mask that contains
            the new node.
return: none
*/
void mai_migrate_gather(void *p,unsigned long node)
{ 
  int err=0,nitens,nodes[1],np;
  size_t totalmem; 
  int *status=NULL;
  unsigned long freemem=0; 	 

  totalmem = get_var_tsize(p);

 //computing total memory to be migrated and free memory on the node
 freemem=na_get_memnode(node)*KBYTE;

if(totalmem <freemem) //there are enough memory
{
   //set node
   node = 1 << node;
   nodes[0]=node;
   set_var_nodes(p,1,nodes);

   node = 1 << node;
  #ifdef MPOL_MF_MOVE
  err=mbind(p,totalmem,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);
  if(err < 0){
    identify_error();
    printf("\t function migrate_gather");
   }
 
  #elif NR_move_pages
  err = syscall(__NR_move_pages, 0, totalmem,p,&node,status, 0);
  if(err < 0){
    identify_error();
    printf("\t function migrate_gather");
   }
  #else
   printf("\t function migrate_scatter - Linux Kernel version not updated!");
  #endif 
 }
 else
  printf("\t Node does not have enough memory!");  
 
}

//---------------------------------------------------Explicit Memory Policies

/*
Made a round-robin with the memory pages of data pointed by void *p.
Round-robin starts in the node that causes the first page fault.
parameters: p-pointer to data structure  
return: none
*/
void mai_cyclic_sn(void *p,int nnodes,int *nodes)
{
  int err=0,i;
  unsigned long node=0; 
  void *t=NULL;

   t = get_var_pl(p);
     if(t == NULL)
         t = p;

 for(i=0;i<nnodes;i++)
	node += 1 << nodes[i];

  //place pointers on all nodes
  size_t size=0, size_void=sizeof(void*);
  int ndim = get_var_ndim(p);
  if (ndim > 1){
   
   for(i=0;i<ndim-1;i++)
    size += get_var_dim(p,i+1)*size_void;

   size = alignmem(size);  
   err=mbind(p,size,MPOL_INTERLEAVE,
	    &node,MASK_SIZE,MPOL_MF_MOVE);
   if(err < 0)
   {
    identify_error();     
    printf("\n Pointers - function cyclic!");
   }
  }
  
  get_lock(p);  
  if(get_var_policy(p) != CYCLIC && get_var_policy(p) != -1)//changing memory policy
  {
    #ifdef MPOL_MF_MOVE
    err=mbind(t,get_var_itens(p)*get_var_size(p),MPOL_INTERLEAVE,
	    &node,MASK_SIZE,MPOL_MF_MOVE);
    set_var_nodes(p,nnodes,node);
    #else
    printf("\n Operating system do not support data migration!");
    exit(-1);
    #endif 
  }
  else //applying cyclic memory policy
  {
    err=mbind(t,get_var_itens(p)*get_var_size(p),MPOL_INTERLEAVE,
	    &node,MASK_SIZE,0);
    set_var_policy(p,CYCLIC);
    set_var_nodes(p,nnodes,node);
  }
  release_lock(p);
  
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
void mai_skew_mapp_sn(void *p,int nnodes,int *nodes)
{
  int err,i,np=0,block,rest=0,nitens,*ti,type,great_int,mig=0;
  size_t size;
  void *t=NULL;
  unsigned long pagsize,npag=NPAG,node=0;
	
  nitens = get_var_itens(p);
  size = get_var_size(p);
  type = get_var_type(p);
  np = (size*nitens)/PAGE_SIZE;

  block = np/npag;

    t = get_var_pl(p);
     if(t == NULL)
         t = p;

    for(i=0;i<nnodes;i++)
	node += 1 << nodes[i];
     
    get_lock(p);
  //place pointers on all nodes
  size=0;
  size_t size_void=sizeof(void*);
  int ndim = get_var_ndim(p);
  if (ndim > 1){
   
   for(i=0;i<ndim-1;i++)
    size += get_var_dim(p,i+1)*size_void;

   size = alignmem(size);  
   err=mbind(p,size,MPOL_INTERLEAVE,
	    &node,MASK_SIZE,MPOL_MF_MOVE);
   if(err < 0)
   {
    identify_error();     
    printf("\n Pointers - function cyclic!");
   }
  
}
   
  if(get_var_policy(p) != SKEW && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,SKEW);
    set_var_nodes(p,nnodes,nodes);
  
  for(i=0;i<block;i++)
 {
    great_int = i/nnodes;
    node = 1 << nodes[(i+great_int+1)%nnodes];

   if(mig)//changing memory policy
   {
    #ifdef MPOL_MF_MOVE
    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
    #else
    printf("\n Operating system do not support data migration!");
    exit(-1);
    #endif 
   }
  else //applying skew memory policy
  {
    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,0);
  }

    t = (npag*PAGE_SIZE)+ t;
 }
  release_lock(p);  
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
void mai_prime_mapp_sn(void *p,int prime,int nnodes,int *nodes)
{
 int err=0,i,rest=0,nitens,*ti,type,npagi,pm_no,mem_node;
  size_t size;
  unsigned long node=0;
  void *t=NULL;
  unsigned long pagsize,block,np,npag=NPAG;

  int mig=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);
  type = get_var_type(p);

  np = (size*nitens)/PAGE_SIZE;
 
  block = np/npag;
  rest = np%npag;

     t = get_var_pl(p);
     if(t == NULL)
         t = p;

 for(i=0;i<nnodes;i++)
	node += 1 << nodes[i];

 get_lock(p);
   //place pointers on all nodes
  size=0;
  size_t size_void=sizeof(void*);
  int ndim = get_var_ndim(p);
  if (ndim > 1){
   
   for(i=0;i<ndim-1;i++)
    size += get_var_dim(p,i+1)*size_void;

   size = alignmem(size);  
   err=mbind(p,size,MPOL_INTERLEAVE,
	    &node,MASK_SIZE,MPOL_MF_MOVE);
   if(err < 0)
   {
    identify_error();     
    printf("\n Pointers - function prime mapp!");
   }
  }

 
  if(get_var_policy(p) != PRIME && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,PRIME);
    set_var_nodes(p,nnodes,nodes); 

  for(i=0;i<block;i++)
 {
        npagi = i*npag;
        if((npagi%prime) >= nnodes)
        {
         pm_no = npagi % prime;
         while(pm_no >= nnodes)
         {
           pm_no = (npagi/prime)*(prime-nnodes)+((npagi%prime)-nnodes);
           npagi = pm_no/prime;
           pm_no = pm_no % prime;
         }
         mem_node = pm_no;
        }
        else
         mem_node = npagi % prime;
  
        node = 1 << nodes[mem_node];
	
	 if(mig)//changing memory policy
  	 {
   	 #ifdef MPOL_MF_MOVE
   		 err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
   	 #else
   		 printf("\n Operating system do not support data migration!");
    		 exit(-1);
    	#endif 
  	}
  	else //applying memory policy
  	{
   		 err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,0);
  	}

    	 t = (npag*PAGE_SIZE)+ t;
 }
  release_lock(p); 
   if(err < 0)
    {
        identify_error();
        printf("\t function prime-mapp");
   }
}    


/*
Made an interleave with blocks(rows) of memory pages pointed by void *p.
Interleave starts in the first node in the sinfo list and it is
done considering sets of rows or columns.
parameters: p-pointer to data structure, nr-number of rows
return: none
*/
void mai_cyclic_rows_sn(void *p,int nr,int nnodes,int *nodes)
{
  int err=0,i,j,nitens,icpt;
  size_t size;
  unsigned long node,np,npag=1,rest=0;
  void *t=NULL;
  
  int mig=0;
 
  nitens = get_var_itens(p);
  size = get_var_size(p);

  np = (size*nitens)/PAGE_SIZE;

  if(nr!=0){
      for(i=2;get_var_dim(p,i)!=0;i++)
        npag *= get_var_dim(p,i);

      npag *= nr*get_var_size(p);
      npag = npag/PAGE_SIZE;

      rest = npag%PAGE_SIZE;
      if(rest !=0)
	npag++;

      icpt = np/npag;
      rest = np%npag;
      if(rest !=0)
	icpt++;
 
      
      if(t == NULL)
         t = p;

 for(i=0;i<nnodes;i++)
	node += 1 << nodes[i];
 
 //place pointers on all nodes
  size=0;
  size_t size_void=sizeof(void*);
  int ndim = get_var_ndim(p);
  if (ndim > 1){

   for(i=0;i<ndim-1;i++)
    size += get_var_dim(p,i+1)*size_void;

   get_lock(p);
   size = alignmem(size);
   err=mbind(p,size,MPOL_INTERLEAVE,
            &node,MASK_SIZE,MPOL_MF_MOVE);
   if(err < 0)
   {
    identify_error();
    printf("\n Pointers - function prime mapp!");
   }
  }



  if(get_var_policy(p) != CYCLIC_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,CYCLIC_B);
    set_var_nodes(p,nnodes,nodes);

       for(i=0;i<icpt-1;i++)
       {
          node = 1 << nodes[(i%nnodes)];
          
        if(mig)//changing memory policy
   	{
   		
   	 	#ifdef MPOL_MF_MOVE
    			err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
    		#else
    			printf("\n Operating system do not support data migration!");
    			exit(-1);
    		#endif 
   		
  	}
  	else //applying memory policy
  	{
    		err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,0);
  	}

          t = (npag*PAGE_SIZE) + t;
       }

       //last mbind with less pages
       npag = np - npag*(icpt-1);
       node = 1 << nodes[(i%nnodes)];

	 if(mig)//changing memory policy
	   {
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,0);
	  }



    }//end rows
  release_lock(p);  
   if(err < 0)
    {
      identify_error();
      printf("\t (function cyclic_rows)\n");
    }
}

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, nr-number of rows
return: none
*/
void mai_skew_mapp_rows_sn(void *p,int nr,int nnodes,int *nodes)
{
  int err,i,np=0,npag,icpt,rest=0,nitens,*ti,great_int;
  size_t size;
  unsigned long node;
  void *t=NULL;
  int mig=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);

  np = (size*nitens)/PAGE_SIZE;

  if(nr != 0)
    {  
      for(i=2;get_var_dim(p,i)!=0;i++)
        npag *= get_var_dim(p,i);

      npag *= nr*get_var_size(p);
      npag = npag/PAGE_SIZE;

      rest = npag%PAGE_SIZE;
      if(rest !=0)
	npag++;

      icpt = np/npag;
      rest = np%npag;
      if(rest !=0)
	icpt++;
 
      
      if(t == NULL)
         t = p;

 for(i=0;i<nnodes;i++)
	node += 1 << nodes[i];

         //place pointers on all nodes
  size=0;
  size_t size_void=sizeof(void*);
  int ndim = get_var_ndim(p);
  if (ndim > 1){

   for(i=0;i<ndim-1;i++)
    size += get_var_dim(p,i+1)*size_void;

   get_lock(p);
   size = alignmem(size);
   err=mbind(p,size,MPOL_INTERLEAVE,
            &node,MASK_SIZE,MPOL_MF_MOVE);
   if(err < 0)
   {
    identify_error();
    printf("\n Pointers - function prime mapp!");
   }
  }


 	  
	  if(get_var_policy(p) != SKEW_B && get_var_policy(p) != -1)
	    mig = 1; //data migration

	    set_var_policy(p,SKEW_B);
	    set_var_nodes(p,nnodes,nodes);


       for(i=0;i<icpt-1;i++)
       {
	  great_int = i/nnodes;
	  node = 1 << nodes[(i+great_int+1)%nnodes];
          
	   if(mig)//changing memory policy
	   {
	  
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif
	  }
	  else //applying skew memory policy
	  {
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,0);
	  }
          t = (npag*PAGE_SIZE) + t;
       }

       //last mbind with less pages
       npag = np - npag*(icpt-1);
       node = 1 << nodes[(i%nnodes)];

	   if(mig)//changing memory policy
	   {
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   
	  }
	  else //applying skew memory policy
	  {
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,0);
	  }

    }//end rows
  release_lock(p); 
   if(err < 0)
    {
        identify_error();
        printf("\t function skew_mapp_rows");
    }
}

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, nr-number of rows
return: none
*/
void mai_prime_mapp_rows_sn(void *p,int prime,int nr,int nnodes,int *nodes)
{
  int err=0,i,rest=0,nitens,*ti,mem_node,npagi,pm_no;
  size_t size;
  unsigned long node;
  void *t=NULL;
  int icpt,np,npag;
  int mig=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);

  np = (size*nitens)/PAGE_SIZE;
 
  if(nr != 0)
    {  
      for(i=2;get_var_dim(p,i)!=0;i++)
        npag *= get_var_dim(p,i);

      npag *= nr*get_var_size(p);
      npag = npag/PAGE_SIZE;

      rest = npag%PAGE_SIZE;
      if(rest !=0)
	npag++;

      icpt = np/npag;
      rest = np%npag;
      if(rest !=0)
	icpt++;
 
      
      if(t == NULL)
         t = p;

	
      get_lock(p);
	  if(get_var_policy(p) != PRIME_B && get_var_policy(p) != -1)
	    mig = 1; //data migration

	    set_var_policy(p,PRIME_B);
	    set_var_nodes(p,nnodes,nodes);

       for(i=0;i<icpt-1;i++)
       {
	  npagi = i*npag;
	  if((npagi%prime) >= nnodes)
	    {
	      pm_no = npagi % prime;
	      while(pm_no >= nnodes)
		{
		  pm_no = (npagi/prime)*(prime-nnodes)+((npagi%prime)-nnodes);
		  npagi = pm_no/prime;
		  pm_no = pm_no % prime;
		}
	      mem_node = pm_no;
	    }
	  else
	    mem_node = npagi % prime;
  
	  node = 1 << nodes[mem_node];
          
	   if(mig)//changing memory policy
	   {
	 
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,0);
	  }
          t = (npag*PAGE_SIZE) + t;
       }

       //last mbind with less pages
       npag = np - npag*(icpt-1);
       node = 1 << nodes[(i%nnodes)];

	if(mig)//changing memory policy
	   {
	 
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);    
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,PAGE_SIZE*npag,MPOL_BIND,&node,MASK_SIZE,0);
	  }
    }//end rows
  release_lock(p);
   if(err < 0)
    {
      identify_error();
      printf("\t (rest - function prime_mapp_rows)\n");
    }
}    

/*
Made an interleave with blocks(columns) of memory pages pointed by void *p.
Interleave starts in the first node in the sinfo list and it is
done considering sets of rows or columns.
parameters: p-pointer to data structure, nc-number of columns
return: none
*/
void mai_cyclic_columns_sn(void *p,int nc,int nnodes,int *nodes)
{
  int err=0,i,j,descColumn=1,nitens,icpt;
  size_t size;
  unsigned long node,npRow=1;
  void *t=NULL;
  int mig=0;
	  
  get_lock(p);	  
	  if(get_var_policy(p) != CYCLIC_B && get_var_policy(p) != -1)
	    mig = 1; //data migration

	    set_var_policy(p,CYCLIC_B);
	    set_var_nodes(p,nnodes,nodes);

  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(nc!=0){
    for(i=3;get_var_dim(p,i)!=0;i++)
	npRow  *= get_var_dim(p,i);

    npRow *= size*get_var_dim(p,2);
    npRow = npRow / PAGE_SIZE;
     
    for(i=3;get_var_dim(p,i)!=0;i++)
      descColumn *= get_var_dim(p,i);

    descColumn *= nc * size;
    descColumn = descColumn/PAGE_SIZE;

    
    if(t == NULL)
         t = p;
  
    for(j=0;j<get_var_dim(p,2)/nc;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          node = 1 << nodes[j%nnodes];
	  
	   if(mig)//changing memory policy
	   {
	  
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
	  t = (npRow*PAGE_SIZE) + t; 	
       }   
     
       
       if(t == NULL)
         t = p;

       t = t + ((j+1)*descColumn*PAGE_SIZE);
    }
    
   if(err < 0)
    {
      identify_error();
      printf("\t (function cyclic_columns)\n");
    }

  }
  release_lock(p);
}

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, nc-number of columns
return: none
*/
void mai_skew_mapp_columns_sn(void *p,int nc,int nnodes,int *nodes)
{
  int err=0,i,j,descColumn=1,nitens,great_int;
  size_t size;
  unsigned long node,npRow=1;
  void *t=NULL;

  int mig=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(nc != 0)
    {  
      for(i=3;get_var_dim(p,i)!=0;i++)
	npRow  *= get_var_dim(p,i);

    npRow *= size*get_var_dim(p,2);
    npRow = npRow / PAGE_SIZE;

    for(i=3;get_var_dim(p,i)!=0;i++)
      descColumn *= get_var_dim(p,i);

    descColumn *= nc * size;
    descColumn = descColumn/PAGE_SIZE;

    
    if(t == NULL)
         t = p;

    get_lock(p); 	  
	  if(get_var_policy(p) != SKEW_B && get_var_policy(p) != -1)
	    mig = 1; //data migration

	    set_var_policy(p,SKEW_B);
	    set_var_nodes(p,nnodes,nodes);

    for(j=0;j<get_var_dim(p,2)/nc;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          great_int = j/nnodes;
	  node = 1 << nodes[(j+great_int+1)%nnodes];

	 if(mig)//changing memory policy
	   {
	 
	    #ifdef MPOL_MF_MOVE
	     err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);   
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }

	  t = (npRow*PAGE_SIZE) + t; 	
       }   
     
       
       if(t == NULL)
         t = p;

       t = t + ((j+1)*descColumn*PAGE_SIZE);
    }//end rows
  
   if(err < 0)
    {
        identify_error();
        printf("\t function skew_mapp_columns");
    }
   }
  release_lock(p);
}

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, nc-number of columns
return: none
*/
void mai_prime_mapp_columns_sn(void *p,int prime,int nc,int nnodes,int *nodes)
{
  int err=0,i,j,descColumn=1,nitens,mem_node,npagi,pm_no;
  size_t size;
  unsigned long node,npRow=1;
  void *t=NULL;
  int mig=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(nc != 0)
    {  
       for(i=3;get_var_dim(p,i)!=0;i++)
	npRow  *= get_var_dim(p,i);

    npRow *= size*get_var_dim(p,2);
    npRow = npRow / PAGE_SIZE;

    for(i=3;get_var_dim(p,i)!=0;i++)
      descColumn *= get_var_dim(p,i);

    descColumn *= nc * size;
    descColumn = descColumn/PAGE_SIZE;

    
    if(t == NULL)
         t = p;

    get_lock(p);
  if(get_var_policy(p) != PRIME_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,PRIME_B);
    set_var_nodes(p,nnodes,nodes);
  
    for(j=0;j<get_var_dim(p,2)/nc;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          npagi = j*descColumn;
	  if((npagi%prime) >= nnodes)
	    {
	      pm_no = npagi % prime;
	      while(pm_no >= nnodes)
		{
		  pm_no = (npagi/prime)*(prime-nnodes)+((npagi%prime)-nnodes);
		  npagi = pm_no/prime;
		  pm_no = pm_no % prime;
		}
	      mem_node = pm_no;
	    }
	  else
	    mem_node = npagi % prime;
  
	  node = 1 << nodes[mem_node];

	   if(mig)//changing memory policy
	   {

	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	  
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }

	  t = (npRow*PAGE_SIZE) + t; 	
       }   
     
       
       if(t == NULL)
         t = p;

       t = t + ((j+1)*descColumn*PAGE_SIZE);
    }//end rows
  
   if(err < 0)
    {
      identify_error();
      printf("\t (rest - function prime_mapp_columns)\n");
    }   
   }
  release_lock(p);
}    

/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, n3d-number of plans
return: none
*/
void mai_cyclic_3D_sn(void *p,int n3d,int nnodes,int *nodes)
{
  int err=0,i,j,nblocks,nitens,mem_node,npagi,pm_no,nitens4d;
  size_t size;
  unsigned long node,shift=1;
  void *t=NULL;
  int mig=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(n3d != 0)
  {  
    //Are there any elements on 4 dimension?
    int temp = get_var_dim(p,4);
    nitens4d = (temp != 0) ? temp:1;

    shift = get_var_dim(p,1)*get_var_dim(p,2)*n3d*size*nitens4d;
    shift = shift/PAGE_SIZE;
    nblocks =  get_var_dim(p,3)/n3d;

    
    if(t == NULL)
         t = p;


    get_lock(p);
  if(get_var_policy(p) != CYCLIC_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,CYCLIC_B);
    set_var_nodes(p,nnodes,nodes);
  
    for(j=0;j<nblocks;j++){
          node = 1 << nodes[j%nnodes];

	   if(mig)//changing memory policy
	   {
	   
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
     
       
       if(t == NULL)
         t = p;

       //moving pointer to n3d plan
       t = t + ((j+1)*shift*PAGE_SIZE);
    }
 
   if(err < 0)
    {
      identify_error();
      printf("\t (Function cyclic_3d)\n");
    }   
   }
  release_lock(p);
}


/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, n3d-number of plans
return: none
*/
void mai_skew_mapp_3D_sn(void *p,int n3d,int nnodes,int *nodes)
{
    int err=0,i,j,nblocks,nitens,mem_node,great_int,nitens4d;
  size_t size;
  unsigned long node,shift=1;
  void *t=NULL;
  int mig=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(n3d != 0)
  {  
    //Are there any elements on 4 dimension?
    int temp = get_var_dim(p,4);
    nitens4d = (temp != 0) ? temp:1;

    shift = get_var_dim(p,1)*get_var_dim(p,2)*n3d*size*nitens4d;
    shift = shift/PAGE_SIZE;
    nblocks =  get_var_dim(p,3)/n3d;

    
    if(t == NULL)
         t = p;

    get_lock(p);
 
  if(get_var_policy(p) != SKEW_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,SKEW_B);
    set_var_nodes(p,nnodes,nodes);
  
    for(j=0;j<nblocks;j++){
          great_int = j/nnodes;
	  node = 1 << nodes[(j+great_int+1)%nnodes];

	   if(mig)//changing memory policy
	   {
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
     
       
       if(t == NULL)
         t = p;

       //moving pointer to n3d plan
       t = t + ((j+1)*shift*PAGE_SIZE);
    }
 
   if(err < 0)
    {
      identify_error();
      printf("\t (Function skew_mapp_3d)\n");
    }   
   }
  release_lock(p);
}


/*
Made a round-robin  with memory pages pointed by void *p.
Round-robin is donne left-to right in the nodes contained 
in variable nodes. This memory policy try to guarantee
conflict-free in pages access.
parameters: p-pointer to data structure, n3d-number of plans
prime-number of virtual nodes
return: none
*/
void mai_prime_mapp_3D_sn(void *p,int prime,int n3d,int nnodes,int *nodes)
{
  int err=0,i,j,nblocks,nitens,mem_node,npagi,pm_no,nitens4d;
  size_t size;
  unsigned long node,shift=1;
  void *t=NULL;
  int mig=0;
 
  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(n3d != 0)
  {  
    //Are there any elements on 4 dimension?
    int temp = get_var_dim(p,4);
    nitens4d = (temp != 0) ? temp:1;

    shift = get_var_dim(p,1)*get_var_dim(p,2)*n3d*size*nitens4d;
    shift = shift/PAGE_SIZE;
    nblocks =  get_var_dim(p,3)/n3d;
    
    if(t == NULL)
         t = p;

    get_lock(p);
 
  if(get_var_policy(p) != PRIME_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,PRIME_B);
    set_var_nodes(p,nnodes,nodes);
  
    for(j=0;j<nblocks;j++){
          npagi = j*shift;
	  if((npagi%prime) >= nnodes)
	    {
	      pm_no = npagi % prime;
	      while(pm_no >= nnodes)
		{
		  pm_no = (npagi/prime)*(prime-nnodes)+((npagi%prime)-nnodes);
		  npagi = pm_no/prime;
		  pm_no = pm_no % prime;
		}
	      mem_node = pm_no;
	    }
	  else
	    mem_node = npagi % prime;
  
	  node = 1 << nodes[mem_node];

	   if(mig)//changing memory policy
	   {
	  
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	   
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
            
       if(t == NULL)
         t = p;

       //moving pointer to n3d plan
       t = t + ((j+1)*shift*PAGE_SIZE);
    }
    
   if(err < 0)
    {
      identify_error();
      printf("\t (Function prime_mapp_3d)\n");
    }   
   }
  release_lock(p);
}


/*
Made a bind with memory pages pointed by void *p.
Bind is donne in the nodes contained in variable nodes .
parameters: p-pointer to data structure
return: none
*/
void mai_bind_all_sn(void *p,int nnodes,int *nodes)
{
  int err=0,i,nitens;
  unsigned long node=0; 
  size_t size;
  void *t=NULL;

  nitens = get_var_itens(p);
  size = get_var_size(p);

   t = get_var_pl(p);
  
     if(t == NULL)
         t = p;

 for(i=0;i<nnodes;i++)
	node += 1 << nodes[i];

 get_lock(p);
  if(get_var_policy(p) != BIND && get_var_policy(p) != -1)//changing memory policy
  {
   
    #ifdef MPOL_MF_MOVE
    err=mbind(t,nitens*size,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);
    set_var_nodes(p,nnodes,node);
    #else
    printf("\n Operating system do not support data migration!");
    exit(-1);
    #endif 
  
  }
  else //applying cyclic memory policy
  {
    err=err=mbind(t,nitens*size,MPOL_BIND,&nodes,MASK_SIZE,0);
    set_var_policy(p,BIND);
    set_var_nodes(p,nnodes,node);
  }
  release_lock(p);   
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
void mai_bind_rows_sn(void *p,int nnodes,int *nodes)
{
  int err=0,i,block,nitens,*ti,type;
  size_t size;
  unsigned long node=0;
  void *t=NULL;
  unsigned long pagsize,np=0,rest=0;
  int mig=0;

 for(i=0;i<nnodes;i++)
	node += 1 << nodes[i];

   //place pointers on all nodes
  size=0;
  size_t size_void=sizeof(void*);
  int ndim = get_var_ndim(p);
  if (ndim > 1){
   
   for(i=0;i<ndim-1;i++)
    size += get_var_dim(p,i+1)*size_void;
   get_lock(p);
   size = alignmem(size);  
   err=mbind(p,size,MPOL_INTERLEAVE,
	    &node,MASK_SIZE,MPOL_MF_MOVE);
   if(err < 0)
   {
    identify_error();     
    printf("\n Pointers - function cyclic!");
   }
  }
 
  if(get_var_policy(p) != BIND_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,BIND_B);
    set_var_nodes(p,nnodes,nodes);

	   nitens = get_var_itens(p);
	  size = get_var_size(p);
	  type = get_var_type(p);

	  np = (size*nitens)/PAGE_SIZE;

	  if(!(REST(size*nitens)))
	     np++;

	  block = np/nnodes;
	  rest = np%nnodes;

	    
	    if(t == NULL)
		 t = p;
	  
       for(i=0;i<nnodes;i++)
       {
          node = 1<<nodes[i];

	 if(mig)//changing memory policy
	   {
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,block*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE); 
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	  
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,block*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
	  t = (block*PAGE_SIZE) + t; 	
       }   
       release_lock(p);
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
void mai_bind_columns_sn(void *p,int nnodes,int *nodes)
{
  int err=0,i,j,descColumn=1,nc,nitens,*ti;
  size_t size;
  unsigned long node=0;
  void *t=NULL;
  unsigned long pagsize,npRow=1,npag,rest=0;

int mig=0;

 for(i=0;i<nnodes;i++)
	node += 1 << nodes[i];
   
   //place pointers on all nodes
  size=0;
  size_t size_void=sizeof(void*);
  int ndim = get_var_ndim(p);
  if (ndim > 1){
   
   for(i=0;i<ndim-1;i++)
    size += get_var_dim(p,i+1)*size_void;
   get_lock(p);
   size = alignmem(size);  
   err=mbind(p,size,MPOL_INTERLEAVE,
	    &node,MASK_SIZE,MPOL_MF_MOVE);
   if(err < 0)
   {
    identify_error();     
    printf("\n Pointers - function cyclic!");
   }
  }
 
  if(get_var_policy(p) != BIND_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,BIND_B);
    set_var_nodes(p,nnodes,nodes);

  nitens = get_var_itens(p);
  size = get_var_size(p);

  for(i=3;get_var_dim(p,i)!=0;i++)
    npRow  *= get_var_dim(p,i);

  npRow *= size*get_var_dim(p,2);
  npRow = npRow / PAGE_SIZE;

  nc = get_var_dim(p,2)/nnodes;

  for(i=3;get_var_dim(p,i)!=0;i++)
    descColumn *= get_var_dim(p,i);

  descColumn *= nc * size;
  descColumn = descColumn/PAGE_SIZE;
  
  
    if(t == NULL)
         t = p;
  
    for(j=0;j<nnodes;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          node = 1<<nodes[j];
	  
	 if(mig)//changing memory policy
	   {
	   
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);   
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
         t = (npRow*PAGE_SIZE) + t; 	
       }   
     
       
       if(t == NULL)
         t = p;

       t = t + ((j+1)*descColumn*PAGE_SIZE);
    }
    release_lock(p);
    if(err < 0)
    {
      identify_error();
      printf("\t (function bind_columns)");
    } 
}

/*
Made a bind with blocks(columns) of memory pages pointed by void *p.
Bind is donne in the nodes contained in sinfo and starts with the
first node in the sinfo .
parameters: p-pointer to data structure
return: none
*/
void mai_bind_3D_sn(void *p,int nnodes,int *nodes)
{
    int err=0,i,j,nblocks,nitens,mem_node,npagi,pm_no,nitens4d;
  size_t size;
  unsigned long node,shift=1;
  void *t=NULL;
  int mig=0;
 
  nitens = get_var_itens(p);
  size = get_var_size(p);

    //Are there any elements on 4 dimension?
    int temp = get_var_dim(p,4);
    nitens4d = (temp != 0) ? temp:1;

    
    if(t == NULL)
         t = p;

    get_lock(p);
 
  if(get_var_policy(p) != BIND_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,BIND_B);
    set_var_nodes(p,nnodes,nodes);

   //setting number of blocks on 3d
    shift = get_var_dim(p,1)*get_var_dim(p,2)*size*nitens4d;
    shift = shift/PAGE_SIZE;
    nblocks =  get_var_dim(p,3)/nnodes;
  
    for(j=0;j<nnodes;j++){
           node = 1<<nodes[j];

	   if(mig)//changing memory policy
	   {
	   
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,nblocks*shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	  
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,nblocks*shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
     
       
       if(t == NULL)
         t = p;

       //moving pointer to n3d plan
       t = t + ((j+1)*shift*nblocks*PAGE_SIZE);
    }
    release_lock(p);
   if(err < 0)
    {
      identify_error();
      printf("\t (Function bind_3d)\n");
    }   
}


/*
Made a bind with blocks(rows) of memory pages pointed by void *p.
Bind is donne in the nodes contained in sinfo and starts with the
first node in the sinfo .
parameters: p-pointer to data structure,nr-number of rows
return: none
*/
void mai_ibind_rows_sn(void *p,int nr,int nnodes,int *nodes)
{
   int err=0,i,block,nitens,*ti,type;
  size_t size;
  unsigned long node;
  void *t=NULL;
  unsigned long pagsize,np=0,rest=0;
  int mig=0;

   if(nr!=0){
	    
     get_lock(p); 	  
	  if(get_var_policy(p) != BIND_B && get_var_policy(p) != -1)
	    mig = 1; //data migration

	    set_var_policy(p,BIND_B);
	    set_var_nodes(p,nnodes,nodes);

		  size = get_var_size(p);
		  type = get_var_type(p);
                 
  		    for(i=2;get_var_dim(p,i)!=0;i++)
   			 nitens *= get_var_dim(p,i);

		  np = (size*nr*nitens)/PAGE_SIZE;

		  if(!(REST(size*nitens*nr)))
		     np++;

		  block = get_var_dim(p,1)/nr;
		
		    
		    if(t == NULL)
			 t = p;
		  
	       for(i=0;i<nnodes;i++)
	       {
		  node = 1<<nodes[i];

		 if(mig)//changing memory policy
		   {
		   
		    #ifdef MPOL_MF_MOVE
		    err=mbind(t,np*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE); 
		    #else
		    printf("\n Operating system do not support data migration!");
		    exit(-1);
		    #endif 
		  
		  }
		  else //applying memory policy
		  {
		    err=mbind(t,np*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
		  }
		  t = (np*PAGE_SIZE) + t; 	
	       }   
	       release_lock(p);
	    if(err < 0)
	    {
	      identify_error();
	      printf("\t (function bind_rows)");
	    }
	}    
}

/*
Made a bind with blocks(columns) of memory pages pointed by void *p.
Bind is donne in the nodes contained in sinfo and starts with the
first node in the sinfo .
parameters: p-pointer to data structure,nc-number of columns
return: none
*/
void mai_ibind_columns_sn(void *p,int nc,int nnodes,int *nodes)
{
  int err=0,i,j,descColumn=1,nitens,*ti;
  size_t size;
  unsigned long node;
  void *t=NULL;
  unsigned long pagsize,npRow=1,npag,rest=0;

int mig=0;

   if(nc!=0){
    
     get_lock(p);
  if(get_var_policy(p) != BIND_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,BIND_B);
    set_var_nodes(p,nnodes,nodes);

  nitens = get_var_itens(p);
  size = get_var_size(p);

  for(i=3;get_var_dim(p,i)!=0;i++)
    npRow  *= get_var_dim(p,i);

  npRow *= size*get_var_dim(p,2);
  npRow = npRow / PAGE_SIZE;

  for(i=3;get_var_dim(p,i)!=0;i++)
    descColumn *= get_var_dim(p,i);

  descColumn *= nc * size;
  descColumn = descColumn/PAGE_SIZE;
  
  
    if(t == NULL)
         t = p;
  
    for(j=0;j<nnodes;j++){
       for(i=0;i<get_var_dim(p,1);i++)
       {
          node = 1<<nodes[j];
	  
	 if(mig)//changing memory policy
	   {
	 
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);   
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,descColumn*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
         t = (npRow*PAGE_SIZE) + t; 	
       }   
     
       
       if(t == NULL)
         t = p;

       t = t + ((j+1)*descColumn*PAGE_SIZE);
    }
    release_lock(p);
    if(err < 0)
    {
      identify_error();
      printf("\t (function bind_columns)");
    }
   }
}


/*
Made a bind with blocks(plans) of memory pages pointed by void *p.
Bind is donne in the nodes contained in sinfo and starts with the
first node in the sinfo .
parameters: p-pointer to data structure, n3d-number of plans
return: none
*/
void mai_ibind_3D_sn(void *p,int n3d,int nnodes,int *nodes)
{

    int err=0,i,j,nblocks,nitens,mem_node,npagi,pm_no,nitens4d;
  size_t size;
  unsigned long node,shift=1;
  void *t=NULL;
  int mig=0;

  nitens = get_var_itens(p);
  size = get_var_size(p);

  if(n3d != 0)
  {  
    //Are there any elements on 4 dimension?
    int temp = get_var_dim(p,4);
    nitens4d = (temp != 0) ? temp:1;

    
    if(t == NULL)
         t = p;
    get_lock(p);
 
  if(get_var_policy(p) != BIND_B && get_var_policy(p) != -1)
    mig = 1; //data migration

    set_var_policy(p,BIND_B);
    set_var_nodes(p,nnodes,nodes);

   //setting number of blocks on 3d
    shift = get_var_dim(p,1)*get_var_dim(p,2)*n3d*size*nitens4d;
    shift = shift/PAGE_SIZE;
    nblocks =  get_var_dim(p,3)/n3d;
  
    for(j=0;j<nblocks;j++){
           node = 1<<nodes[j%nnodes];

	   if(mig)//changing memory policy
	   {
	    #ifdef MPOL_MF_MOVE
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,MPOL_MF_MOVE);  
	    #else
	    printf("\n Operating system do not support data migration!");
	    exit(-1);
	    #endif 
	  }
	  else //applying memory policy
	  {
	    err=mbind(t,shift*PAGE_SIZE,MPOL_BIND,&node,MASK_SIZE,0);
	  }
     
       
       if(t == NULL)
         t = p;

       //moving pointer to n3d plan
       t = t + ((j+1)*shift*PAGE_SIZE);
    }
    release_lock(p);
   if(err < 0)
    {
      identify_error();
      printf("\t (Function bind_3d)\n");
    }   
   }
}


