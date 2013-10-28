/*****************************************************************************
 * $Source$
 * $Author$
 * $Date$
 * $Revision$
 *****************************************************************************/

/**
 * \addtogroup CkLdb
*/
/*@{*/

/*
Status:
  * support processor avail bitvector
  * support nonmigratable attrib
  * support background load

  rewritten by Gengbin Zheng to use the new load balancer database and hash table;
      modified to recognize the nonmigratable attrib of an object 
      by Gengbin Zheng, 7/28/2003
    */

//#include <charm++.h>
#include <cstdio>
#include <algorithm>
#include "MultiCoreLB.h"
#include <cfloat>

class ProcLoadGreater {
  public:
    bool operator()(ProcInfo p1, ProcInfo p2) {
      return (p1.getTotalLoad() >  p2.getTotalLoad());
    }
};

class ObjLoadGreater {
  public:
    bool operator()(Vertex v1, Vertex v2) {
      return (v1.getVertexLoad() < v2.getVertexLoad());
    }
};

CreateLBFunc_Def(MultiCoreLB, "Greedy algorithm which takes the communication graph and the core-to-core latency into account. Tries to avoid too many migrations.")

void MultiCoreLB::init ()
{
  int i, j;

  lbname = (char *)"MultiCoreLB";
  alpha = 1E-9;

  na_init ();

  
  if (CkMyPe () == 0)
    {
      CkPrintf ("[%d] MultiCoreLB starting\n", CkMyPe ());
      ncores = na_get_maxcpus();
      nnodes = na_get_maxnodes();

      //++++++++++++++Debug
      if (_lb_args.debug () > 1) 
	CkPrintf("[%d] CPUs = %d\t Nodes = %d\n", CkMyPe (), ncores, nnodes);
      //++++++++++++++Debug end

      /* Core to Node correspondance array. */
      cpusToNodes = new int [ncores];
      for (i = 0; i < ncores; i++)
	cpusToNodes[i] = na_get_nodeidcpu (i);

      /* Core-to-core factor square matrix. */
      MultiCoreFactorMatrix = new double*[ncores];
	for(i=0;i<ncores;i++)
	  MultiCoreFactorMatrix[i] = new double[ncores];
      na_fill_multicore_latency_matrix(MultiCoreFactorMatrix);

      //++++++++++++++Debug
      if (_lb_args.debug () > 2) 
	{
	  for (i = 0; i < ncores; i++)
	    {
	      for (j = 0; j < ncores; j++)
		CkPrintf ("%.2f ", MultiCoreFactorMatrix[i][j]);
	      CkPrintf("\n");
	    }
	  
	  for (i = 0; i < ncores; i++)
	    CkPrintf ("CPU %d -> Node %d\n", i, cpusToNodes[i]);
	}
      //++++++++++++++Debug end
    }
}


MultiCoreLB::MultiCoreLB(const CkLBOptions &opt): CentralLB(opt)
{
    init();
    if (CkMyPe() == 0)
	CkPrintf("[%d] MultiCoreLB created\n",CkMyPe());
}

MultiCoreLB::MultiCoreLB(CkMigrateMessage *m):CentralLB(m) {
    init();
}

CmiBool MultiCoreLB::QueryBalanceNow(int _step)
{
    //  CkPrintf("[%d] Balancing on step %d\n",CkMyPe(),_step);
    return CmiTrue;
}


void MultiCoreLB::work(LDStats *stats) {
  	int i, obj, proc, comm, node, best_proc;
	double weight, best_weight;

    	if (_lb_args.debug()) CkPrintf("[%d] In MultiCoreLB strategy\n",CkMyPe());

  	/** ========================== INITIALIZATION ============================= */
  	ProcArray *parr = new ProcArray( stats );       // Processor Array
  	ObjGraph *ogr = new ObjGraph( stats );          // Object Graph
	std::vector< Vertex > loads( ogr->vertices );	// Sorted list of objects loads
	
	/*for( obj=0 ; obj<ogr->vertices.size() ; obj++) { 
		CkPrintf("%d %d -- %d %d\n",ogr->vertices[obj].getVertexId(),ogr->vertices[obj].getCurrentPe(),
						loads[obj].getVertexId(),loads[obj].getCurrentPe());
	}*/
	std::sort( loads.begin(), loads.end(), Vertex::compareHigherVertexLoad ); // Sorts objects by load

	for( obj=0 ; obj<ogr->vertices.size() ; obj++) { 
		ogr->vertices[obj].setNewPe( ogr->vertices[obj].getCurrentPe() ); 
    //		if (_lb_args.debug()) CkPrintf("%d ",ogr->vertices[obj].getCurrentPe());
	}
    //	if (_lb_args.debug()) CkPrintf("\n");

        if(_lb_args.debug()){
                allStats(parr,ogr);
	}
  	/** ============================= STRATEGY ================================ */
	//gets the minimal W(k,*) for each chare
        /* N = number of processors
         * K = number of chares
         * i,j = processors
         * k = chare
         * L(i) = load on processor i
         * NF(i,j) = NUMA Factor from i to j
         * M(k,i) = amount of bytes that chare k communicates with chares on processor i
         * W(k,i) = scheduling weight (likeability of chare k to migrate to processor i)
         *
         * W(k,i) = L(i) - M(k,i) + sum of j=1..N where j!=i (M(k,j)*NF(j,i))
         * optimization: gets M(k,i) directly from the communications with
         * chares
         */

 	for( i=0 ; i<ogr->vertices.size(); i++ ){ //iterates through all chares
		obj = loads[i].getVertexId();
		if( !ogr->vertices[obj].isMigratable() ) continue;

		#define NUMALB_MAX_DOUBLE 1e29
		best_weight = NUMALB_MAX_DOUBLE;
		best_proc = 0;
		parr->procs[ ogr->vertices[obj].getCurrentPe() ].setTotalLoad( //remove the load of the chare   
			parr->procs[ ogr->vertices[obj].getCurrentPe() ].getTotalLoad()
			- ogr->vertices[obj].getVertexLoad() );		

		for( proc=0 ; proc<parr->procs.size() ; proc++){
			if( !parr->procs[proc].isAvailable() ) continue;

			weight = 0.0;
			for( comm=0 ; comm<ogr->vertices[obj].recvFromList.size() ; comm++ ){
				int ngb_id = ogr->vertices[obj].recvFromList[comm].getNeighborId(); //neighbor's id
				int ngb_proc = ogr->vertices[ngb_id].getNewPe(); //neighbor's processor
				//if( proc == ngb_proc ){
				//	weight -= ogr->vertices[obj].recvFromList[comm].getNumMsgs();//local communication	
				//} else {	
				 	weight += ogr->vertices[obj].recvFromList[comm].getNumMsgs() *
					( MultiCoreFactorMatrix[ proc ][ ngb_proc ] );	//remote communication
				//}
			}	//end for comm
			weight = weight*alpha + parr->procs[proc].getTotalLoad();
			
			if(weight < best_weight){
                		/*if (_lb_args.debug()>2) {
					CkPrintf("%d to %d with w(k,i) = %lf %lf\n",obj,proc,weight,best_weight);
				}*/
				best_weight = weight;
				best_proc = proc;
			}
		}	//end for procs
		parr->procs[best_proc].setTotalLoad(			//add the load of the chare
			parr->procs[best_proc].getTotalLoad()
			+ ogr->vertices[obj].getVertexLoad() );
		ogr->vertices[obj].setNewPe(best_proc);		//set new processor
	}	//end for chares
	/** ============================== CLEANUP ================================ */
  	ogr->convertDecisions(stats);         // Send decisions back to LDStats
	
}

#include "MultiCoreLB.def.h"

/*@}*/

