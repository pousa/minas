
#ifndef __MA_COMPONENTS_H
#define __MA_COMPONENTS_H

#define ALL 999
#define OS  0
#define LOCAL 1
#define INTERLEAVE 2

/*
* Describes the machine components
*
* nnodes - number of numa nodes
* nsocktes - total number of sockets
* ncores - total number of cores
* ncaches - total number of caches per socket
* nshared_caches - total number of shared caches between cores
* nsiblings - number of direct sibling cores
* nmemcontroller - number of memory banks per socket
*/
struct arch_topology
{
 int nnodes;
 int nsockets;
 int ncores;
 int npus;
 int ncaches;
 int nshared_caches;
 int nsiblings;
 int nmemcontroller;
};

/*
* Console output
*/
struct machine_output{
 char console_output[8192];
 int len;
};

/*
* Describes the components of a core
* Internal structure of the machine library
*
* id - physical id of the core
* caches - size of the caches levels for each core
* shared_caches - cache memories levels that are shared between cores
* siblings_id - physical ids of the core neighbors \
* freq - core frequency
*/
struct core{
 unsigned id;
 double freq;  // TODO: get frequence of each core
 size_t *caches;
 int *shared_caches;
 int nsiblings;
 unsigned *siblings_id;
};

/*
* Describes the components of a node
* Internal structure of the machine library
*
* id - node id
* ncores - number of core per node
* mycores - physical ids of the node cores
* memory - the amount of memory per node
* nneighbors - number of direct neighbors of the node
* neighbors_id - the ids of the neighbors of the node
*/
struct node{
 unsigned id;
 int ncores;
 size_t memory;
 unsigned *mycores;
 int nneighbors;
 int *neighbors_id; //TODO: get the node neighbors of each NUMA node
};


/******************************************************************************
 HWLOC based functions 
 - to extract the machine topology
 - to map process and threads to the machine processig units
******************************************************************************/
void ma_place();

//Get the machine hierarchy
int hw_topology_init (struct arch_topology *topo);
int hw_topology_destroy (struct arch_topology *topo);

void set_phys_siblings(int index, unsigned myid, hwloc_obj_t obj, int ncores, int nsiblings,int type);
void set_node_cores(hwloc_topology_t topology, hwloc_obj_t obj, int num_core);

void ma_phys_pu_topology();
void ma_pu_topology ();
void ma_machine_topology ();
void ma_resume();

//Create a string with the machine topology - by object type
void print_machine(hwloc_topology_t topology, hwloc_obj_t obj, int depth);
void print_children_physical(hwloc_topology_t topology, hwloc_obj_t obj);

//Process related functions - set/get core, numa node, mempol
int ma_proc_node();
int ma_proc_core();
void ma_set_proc_core(int core);
void ma_set_mempol(int mempol);
void ma_get_mempol();

//Thread related functions - set/get core and numa node
int ma_my_node();
int ma_my_core();
unsigned ma_get_myid();
void ma_set_my_core(int cpu);

#endif
