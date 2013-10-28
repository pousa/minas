
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <hwloc.h>
#include <sys/syscall.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "ma_components.h"

//The HWLOC main object to hold the machine architecture
static hwloc_topology_t topology;
static struct core *machine_cores;
static struct node *machine_nodes;
static struct arch_topology *local_topo;

//String for the output in the console
static char console_output[8192];

void ma_place()
{
  local_topo = malloc(sizeof(struct arch_topology));  
  hw_topology_init(local_topo);
}

//Initializes HWLOC and load the machine architecture
int hw_topology_init (struct arch_topology *topo)
{
  hwloc_obj_t obj, obj1, obj2, core1, core2, ant;
  int count, i, j, error;

 
  //Create the machine representation
  error = hwloc_topology_init(&topology);
   
  //Go throught the topology only if HWLOC is 
  //successifully initialized
  if(!error)
  {
    hwloc_topology_load(topology);

    //Extract direct information
    if (hwloc_get_type_depth (topology, HWLOC_OBJ_NODE))
     topo->nnodes = hwloc_get_nbobjs_by_depth (topology, hwloc_get_type_depth (topology, HWLOC_OBJ_NODE));
    else
     topo->nnodes = 0;


    //Get number of cores, sockets and processing units
    topo->ncores = hwloc_get_nbobjs_by_depth (topology, hwloc_get_type_depth (topology, HWLOC_OBJ_CORE));
    topo->nsockets = hwloc_get_nbobjs_by_depth (topology, hwloc_get_type_depth (topology, HWLOC_OBJ_SOCKET)); 
    topo->npus = hwloc_get_nbobjs_by_depth (topology, hwloc_get_type_depth (topology, HWLOC_OBJ_PU));

   //Compute number of memory controlers per socket
   //basically the number of NUMA nodes per socket
   if (topo->nnodes > topo->nsockets)
    topo->nmemcontroller = topo->nnodes/topo->nsockets;
   else
    topo->nmemcontroller = 1;

   count = 0; 
   //Get derivate information
   for(obj = hwloc_get_obj_by_type(topology,HWLOC_OBJ_PU,0);
      obj; obj = obj->parent)
   {
	if (obj->type == HWLOC_OBJ_CACHE)
	{
	  count++;
	  topo->ncaches = count;
	}  
   }

   //Get the number of shared caches per siblings cores
   count = 0; 
   obj1 = hwloc_get_obj_by_type(topology, HWLOC_OBJ_PU, 0);
   do{
        if ((obj1->type == HWLOC_OBJ_CACHE) && (obj1->arity>1))         
                      count++;
        obj1 = obj1->parent;
    }while(obj1);

  topo->nshared_caches = count;

  //Number of direct siblings
  //Siblings cores are the ones that share at least one component
  //level of the architecture
  count = 0;
  core1 = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, 0);
  core2 = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, 1);
  obj   = hwloc_get_common_ancestor_obj(topology, core1, core2);
  if (obj) 
    topo->nsiblings = obj->arity;

  //Machine node and core representation
  machine_nodes = (struct node*) malloc (topo->nnodes*sizeof(struct node));
  machine_cores = (struct core*) malloc (topo->ncores*sizeof(struct core));
 
  //Get the caches sizes and other information for each core  
  for (i = 0; i < topo->ncores ; i++)
   {
	machine_cores[i].caches = malloc (topo->ncaches*sizeof(size_t));
        machine_cores[i].shared_caches = malloc (topo->ncaches*sizeof(int));

        for (j = 0; j < topo->ncaches; j++)
         machine_cores[i].shared_caches[j] = 0;
        for (j = topo->ncaches ; j > topo->ncaches - topo->nshared_caches; j--)
         machine_cores[i].shared_caches[j-1] = 1;

        machine_cores[i].nsiblings = topo->nsiblings;
        machine_cores[i].siblings_id = malloc (topo->nsiblings*sizeof(unsigned));

        if(topo->ncores == topo->npus){
          core1 = hwloc_get_obj_by_type(topology, HWLOC_OBJ_PU, i);
 	  machine_cores[i].id = core1->os_index;
          count = 0;
          for(obj = hwloc_get_obj_by_type(topology,HWLOC_OBJ_PU,0);
              obj; obj = obj->parent) {
	      if (obj->type == HWLOC_OBJ_CACHE)
		{
	        	machine_cores[i].caches[count] = obj->attr->cache.size / 1024;
			count++;
		}
 	   }

        }
        else{
	  core1 = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, i);
 	  machine_cores[i].id = core1->os_index;
          count = 0;
          for(obj = hwloc_get_obj_by_type(topology,HWLOC_OBJ_CORE,0);
              obj; obj = obj->parent) {
	      if (obj->type == HWLOC_OBJ_CACHE)
		{
	        	machine_cores[i].caches[count] = obj->attr->cache.size / 1024;
			count++;
		}
 	   }
        }
   }    

  //Get siblings id - so each core knows its siblings
  for (i = 0; i < topo->ncores ; i++)
   {
        if(topo->ncores == topo->npus){
          core1 = hwloc_get_obj_by_type(topology, HWLOC_OBJ_PU, i);
          set_phys_siblings(i,machine_cores[i].id,core1,topo->ncores,topo->nsiblings,HWLOC_OBJ_PU);
        }
        else{
          core1 = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, i);
          set_phys_siblings(i,machine_cores[i].id,core1,topo->ncores,topo->nsiblings,HWLOC_OBJ_CORE);
        }

  }

   int ncore_node = topo->ncores/topo->nnodes;
  //Get the information for each NUMAnode
  for (i = 0; i < topo->nnodes ; i++)
   {
        obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_NODE, i);
        
	machine_nodes[i].id = obj->os_index;
	machine_nodes[i].memory = obj->memory.total_memory;
        machine_nodes[i].ncores = ncore_node;
        machine_nodes[i].mycores = malloc (ncore_node*sizeof(unsigned)); 

        //Get the cores id of each NUMAnode
        set_node_cores(topology, obj, ncore_node - 1); 
   }    

 /*Local copy of the machine topology components*/ 
 
}

 return error;

}

/*
*Set the cores id of a NUMAnode
*Recursive function that goes throught the machine topology object
*an group them into hierarchical groups 
* topology: the HWLOC NUMA node object
*/
void set_node_cores(hwloc_topology_t topology, hwloc_obj_t obj, int num_core)
{
    unsigned i;

    if(obj->type == HWLOC_OBJ_PU)
        machine_nodes[i].mycores[num_core] = obj->os_index;
    for (i = 0; i < obj->arity; i++) {
        set_node_cores(topology, obj->children[i], num_core - 1);
    }
}

/*
*Deallocate HWLOC and the machine architecture representation
* topo: the HWLOC object to be deallocated
* return: -1 in case of topo is not valid object
*/
int hw_topology_destroy (struct arch_topology *topo)
{
  if(topo){
    hwloc_topology_destroy(topology);
    topo->nnodes = topo->nsockets = topo->ncores = 0;
    topo->nshared_caches = topo->nsiblings = 0;
    return 0;
  }
  else
    return -1;
}

/*
*Get the direct siblings of a core - it is allways considering cache sharing
* index:     the logical id of the core
* myid:      the physical id of the core
* obj:       the HWLOC representation of the core
* nsiblings: the number of siblings of the core 
*/
void set_phys_siblings(int index, unsigned myid, hwloc_obj_t obj, int ncores, int nsiblings,int type)
{
  int i,j = 0;
  hwloc_obj_t obj2, ant;

  for(i = 0; i< ncores && nsiblings > 0 ; i++)
   {
         obj2 = hwloc_get_obj_by_type(topology,type,i);
         ant =  hwloc_get_common_ancestor_obj(topology,obj,obj2);       

         if(ant->type == HWLOC_OBJ_CACHE && obj2->os_index != myid)
         {
            machine_cores[index].siblings_id[j] = obj2->os_index;
            j++; nsiblings--;
            ant = NULL;
	 }  
   }
}

void ma_resume()
{
    int nnodes = local_topo->nnodes; 
    int ncores = local_topo->ncores; 
    int nsockets = local_topo->nsockets;
    int nmemcontroller = local_topo->nmemcontroller;
    int nsiblings = local_topo->nsiblings;
    int ncaches = local_topo->ncaches;
    int nshared_caches = local_topo->nshared_caches;

     printf("\nMACHINE| Machine Architecture\n");
     if (nnodes < 1 )
       printf("MACHINE| No NUMA nodes\n");
     else 
      printf("MACHINE| Number of NUMA nodes %d\n",nnodes);
     
     printf("MACHINE| Number of sockets %d\n",nsockets);
     printf("MACHINE| Number of memory controllers per socket %d\n",nmemcontroller);
     printf("MACHINE| Number of cores %d\n", ncores);
     printf("MACHINE| Number of cores per socket %d\n", ncores/nsockets);
     printf("MACHINE| Number of siblings per core %d\n", nsiblings);
     printf("MACHINE| Number of caches levels  %d\n", ncaches);
     printf("MACHINE| Number of shared caches  %d\n", nshared_caches);  
}

/*
*Prints the physical ids of the last level of PU 
*Recursive function that goes throught the machine topology object
*an group them into hierarchical groups
* topology: the HWLOC object that represents the machine
* obj: the current object of a level
*/
void print_children_physical(hwloc_topology_t topology, hwloc_obj_t obj)
{
    char string[128],out_string[256];
    unsigned i,arity;

    if(obj->type == HWLOC_OBJ_PU){
       sprintf(out_string,"P%d", obj->os_index, string);
       strcat(console_output,out_string);      
    }
    else {//it is not a PU
      if(obj->first_child && obj->first_child->type == HWLOC_OBJ_PU)
       arity = 1; //number of children
      else
       arity = obj->arity;

     if(arity > 1 )
     {
      if(obj->type == HWLOC_OBJ_SOCKET){
       sprintf(out_string,"\n\t[", string);
       strcat(console_output,out_string);   
      } 
      else if(obj->type == HWLOC_OBJ_NODE || obj->type == HWLOC_OBJ_CACHE){
       sprintf(out_string,"( ", string);
       strcat(console_output,out_string);
      }	
     }
 
    for (i = 0; i < arity; i++) {
        print_children_physical(topology, obj->children[i]);
        if( obj->children[i]->next_sibling ){
           sprintf(out_string,", ", string);
           strcat(console_output,out_string);
        }
    }
     if(arity > 1 )
     {
      if(obj->type == HWLOC_OBJ_SOCKET){
           sprintf(out_string," ]", string);
           strcat(console_output,out_string);}
      else if(obj->type == HWLOC_OBJ_NODE || obj->type == HWLOC_OBJ_CACHE){
           sprintf(out_string," )", string);
           strcat(console_output,out_string);
      }	
     }
  }
}

/*
*Call the recursive function print_children_physical with the HWLOC object
*/
void ma_phys_pu_topology()
{
  struct machine_output *ma_out = malloc(sizeof(struct machine_output));

   console_output[0] = '\0';
   strcpy(ma_out->console_output, "\n MACHINE| Physical processing units organization\n");
   print_children_physical(topology, hwloc_get_root_obj(topology));
   strcat(ma_out->console_output, console_output);
   strcat(ma_out->console_output, "\0");
   ma_out->len = strlen(ma_out->console_output);
   printf("%s",ma_out->console_output);
}


/*
*Prints the machine hierarchy 
*Recursive function that goes throught the machine topology object
*an group them into hierarchical groups
* topology: the HWLOC object
* obj: the current object in the topology
* depth: the horizontal level in the machine topology 
*/
void print_machine(hwloc_topology_t topology, hwloc_obj_t obj, int depth)
{
    char string[128], out_string[128];
    unsigned i,arity;

    string[0] = out_string[0] = '\0';

     if(obj->type == HWLOC_OBJ_NODE || obj->type == HWLOC_OBJ_SOCKET ||
        obj->type == HWLOC_OBJ_MACHINE ){ 
       hwloc_obj_snprintf(string, sizeof(string), topology, obj, "#", 0);
       sprintf(out_string,"%*s%s\n", depth, "", string);
       strcat(console_output,out_string); }
     else {
       hwloc_obj_snprintf(string, sizeof(string), topology, obj, "#", 0);
      if(obj->type == HWLOC_OBJ_PU )
      {
       sprintf(out_string,"%*s%s\n", depth, "", string);
       strcat(console_output,out_string);
      }
      else if (obj->type == HWLOC_OBJ_CACHE && obj->arity>1){
       hwloc_obj_type_snprintf(string, sizeof(string), obj, 0);
       sprintf(out_string,"%*s%s", depth, "", string);
       strcat(console_output,out_string);
       sprintf(out_string," (%dMB)\n", obj->attr->cache.size/(1024*1024));
       strcat(console_output,out_string);       
      }
      else {
       sprintf(out_string,"%*s%s\t", depth, "", string);
       strcat(console_output,out_string);
      }
     }                 
    if (obj->type != HWLOC_OBJ_PU) {//it is not a PU
      if((obj->first_child && obj->first_child->type == HWLOC_OBJ_PU))
       arity = 1; //number of children
      else
       arity = obj->arity;

    for (i = 0; i < arity; i++) 
        print_machine(topology, obj->children[i],depth+1);
   }
}


/*
*Call the function that prints the processing units hierarchy
*/
void ma_machine_topology ()
{
  struct machine_output *ma_out = malloc(sizeof(struct machine_output));    
  console_output[0] = '\0';
  strcpy(ma_out->console_output, "\n MACHINE| Architecture organization\n\n");
  print_machine(topology, hwloc_get_root_obj(topology), 0);
  strcat(ma_out->console_output, console_output);
  strcat(ma_out->console_output, "\0");
  ma_out->len = strlen(ma_out->console_output);
  printf("%s",ma_out->console_output);
}


/*
* Get the node where the current process is running
* return the node of the core
*/
int ma_proc_node()
{
  int node;
  hwloc_cpuset_t set;
  hwloc_nodeset_t nset;

  if (local_topo->nnodes != 0 ){
    set = hwloc_bitmap_alloc();
    nset = hwloc_bitmap_alloc();
    hwloc_get_proc_cpubind (topology,0,set,HWLOC_CPUBIND_PROCESS);
    hwloc_cpuset_to_nodeset(topology,set,nset);
    node = hwloc_bitmap_first(nset); 	
    hwloc_bitmap_free(set);
    hwloc_bitmap_free(nset);
  }
 else
   node = -1;

  return node;
}

/*
* Get the node where the current thread is running
* return the node of the core
*/
int ma_my_node()
{
  int node;
  hwloc_cpuset_t set;
  hwloc_nodeset_t nset;

  if (local_topo->nnodes != 0 ){
    set = hwloc_bitmap_alloc();
    nset = hwloc_bitmap_alloc();
    hwloc_get_cpubind (topology,set,HWLOC_CPUBIND_THREAD);
    hwloc_cpuset_to_nodeset(topology,set,nset);
    node = hwloc_bitmap_first(nset); 	
    hwloc_bitmap_free(set);
    hwloc_bitmap_free(nset);
  }
 else
   node = -1;

  return node;
}

/*
TODO: get thread id - portable
*/
unsigned ma_get_myid()
{
  return syscall(SYS_gettid);
}

/*
* Set the core where the current process will run
* return the core
*/
void ma_set_proc_core(int core)
{
  hwloc_cpuset_t set;

  set = hwloc_bitmap_alloc();
  hwloc_bitmap_zero(set);
  hwloc_bitmap_set(set,core);
  hwloc_set_proc_cpubind (topology,0,set,HWLOC_CPUBIND_PROCESS);
  hwloc_bitmap_free(set);
}

/*
* Get the core where the current process is running
* return the core
*/
int ma_proc_core()
{
  int core;

  hwloc_cpuset_t set;
  set = hwloc_bitmap_alloc();
  hwloc_get_proc_cpubind (topology,0,set,HWLOC_CPUBIND_PROCESS);
  core = hwloc_bitmap_first(set);
  hwloc_bitmap_free(set);

  return core;
}

/*
* Get the memory policy of a process 
*/
void ma_get_mempol()
{
  int node;
  hwloc_nodeset_t nset;
  hwloc_membind_policy_t mempol=-1;

 if (local_topo->nnodes != 0 ){

  nset = hwloc_bitmap_alloc();
  hwloc_get_membind_nodeset(topology,nset,&mempol,0);  
  node = hwloc_bitmap_first(nset);
  switch(mempol)
  {
   case HWLOC_MEMBIND_FIRSTTOUCH:
     printf("Memory policy -> First-touch\n"); 
     printf("Node used #%d\n",node);
   break;
   case HWLOC_MEMBIND_BIND:
     printf("Memory policy -> Bind\n");
     printf("Node used #%d\n",node);      
   break;
   case HWLOC_MEMBIND_INTERLEAVE: 
     printf("Memory policy -> Interleaved\n");
     printf("All nodes are used \n");     
   break;
   default:
     printf("Default Memory policy\n");
   break;
  }
 }
 else
   printf("Default Memory policy\n");
}

/*
* Set the memory policy for data allocation for a process
*/
void ma_set_mempol(int mempol)
{
  int node;

  switch(mempol)
  {
   case OS:
    if (local_topo->nnodes != 0 ){
      hwloc_obj_t obj;
      obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_MACHINE,0);
      hwloc_set_membind_nodeset(topology,obj->nodeset,HWLOC_MEMBIND_FIRSTTOUCH,0);
     }     
   break;
   case LOCAL:
     if (local_topo->nnodes != 0 ){
      node =  ma_proc_node(); 
      hwloc_obj_t obj;
      obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_NODE, node);
      hwloc_set_membind_nodeset(topology,obj->nodeset,HWLOC_MEMBIND_BIND,0);
     }
  break;
   case INTERLEAVE:
    if (local_topo->nnodes != 0 ){
      node =  ma_proc_node();      
      hwloc_obj_t obj;
      obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_NODE, node);
      obj = obj->parent;
      hwloc_set_membind_nodeset(topology,obj->nodeset,HWLOC_MEMBIND_INTERLEAVE,0);
    }
   break;
  }
}

/*
* Set the core where the current thread will run
* return the core
*/
void ma_set_my_core(int cpu)
{
 
  hwloc_cpuset_t set;

  set = hwloc_bitmap_alloc();
  hwloc_bitmap_zero(set);
  hwloc_bitmap_set(set,cpu);
  hwloc_set_cpubind (topology,set,HWLOC_CPUBIND_THREAD);
  hwloc_bitmap_free(set);
}

/*
* Get the core where the current thread is running
* return the core
*/
int ma_my_core()
{
  int core;
  hwloc_cpuset_t set;

  set = hwloc_bitmap_alloc();
  hwloc_get_cpubind (topology,set,HWLOC_CPUBIND_THREAD);
  core = hwloc_bitmap_first(set);
  hwloc_bitmap_free(set);

  return core;
}
