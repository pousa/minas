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

#include <cstdio>
#include <algorithm>
#include "NumaLB.h"

#define NUMALB_MAX_DOUBLE 1e29
#define NUMALB_ALPHA 0.00001

CreateLBFunc_Def(NumaLB, "Greedy algorithm which takes the communication graph and the NUMA factor into account. Tries to avoid too many migrations.")

void NumaLB::init()
    {
        lbname = (char*)"NumaLB";
#ifndef NUMALB_ALPHA
        alpha = _lb_args.alpha();
#else
	alpha = NUMALB_ALPHA;
#endif

        int i,j;
        if (CkMyPe()==0){
            CkPrintf("[%d] NumaLB starting\n",CkMyPe());
		CkPrintf("Alpha = %lf\n",alpha);
            if(na_is_numa()){	//if NUMA machine, create NUMA-factor table (LB only working with NUMA machines)
                max_cpus = na_get_maxcpus();
                max_nodes = na_get_maxnodes();
                //++++++++++++++Debug
                if (_lb_args.debug()>1) 
                    CkPrintf("[%d] CPUs = %d\t Nodes = %d\n",CkMyPe(),max_cpus,max_nodes);
                //++++++++++++++Debug end
                numaFactorMatrix = new float [max_nodes*max_nodes]; //NUMA factor square matrix
                for(i=0;i<max_nodes;i++){ //fills NUMA factor matrix
                    for(j=0;j<max_nodes;j++){
                        numaFactorMatrix[i*max_nodes+j] = na_numafactor(i+1,j+1);
                    }
                }
                cpusToNodes = new int [max_cpus];
                for(i=0;i<max_cpus;i++){ //fills cpus to nodes array
                    cpusToNodes[i] = na_get_nodeidcpu(i);
                }

                //++++++++++++++Debug
                if (_lb_args.debug()>2) {
                    for(i =0;i<max_nodes;i++){ //fills NUMA factor matrix
                        for(j=0;j<max_nodes;j++){
                            CkPrintf("%f\t",numaFactorMatrix[i*max_nodes+j]);
                        }
                        CkPrintf("\n");
                    }
                    for(i=0;i<max_cpus;i++){ //fills cpus to nodes array
                        CkPrintf("CPU %d -> Node %d\n",i,cpusToNodes[i]);
                    }
                }
                //++++++++++++++Debug end
        }
    }
}

NumaLB::NumaLB(const CkLBOptions &opt): CentralLB(opt)
{
    init();
    if (CkMyPe() == 0)
	CkPrintf("[%d] NumaLB created\n",CkMyPe());
}

NumaLB::NumaLB(CkMigrateMessage *m):CentralLB(m) {
    init();
}

CmiBool NumaLB::QueryBalanceNow(int _step)
{
    //  CkPrintf("[%d] Balancing on step %d\n",CkMyPe(),_step);
    return CmiTrue;
}
void NumaLB::work(LDStats *stats) {
	int i, obj, proc, comm, node, best_proc;
	double weight, best_weight, final_weight;

    	if (_lb_args.debug()) CkPrintf("[%d] In NumaLB strategy\n",CkMyPe());

  	/** ========================== INITIALIZATION ============================= */
  	ProcArray *parr = new ProcArray( stats );       // Processor Array
  	ObjGraph *ogr = new ObjGraph( stats );          // Object Graph
	std::vector< Vertex > loads( ogr->vertices );	// Sorted list of objects loads
	
	std::sort( loads.begin(), loads.end(), Vertex::compareHigherVertexLoad ); // Sorts objects by load
#ifdef NOOVERHEAD
	CkPrintf("No overhead\n");
	for( proc=0 ; proc<parr->procs.size() ; proc++){
		parr->procs[proc].setTotalLoad( parr->procs[proc].getTotalLoad() - parr->procs[proc].getOverhead()); 
	}
#endif
	for( obj=0 ; obj<ogr->vertices.size() ; obj++) { 
		ogr->vertices[obj].setNewPe( ogr->vertices[obj].getCurrentPe() ); 
	}

	if(_lb_args.debug()){
		allStats(parr,ogr);
/*		maxProcessorLoad(parr,1);
		minProcessorLoad(parr,1);
		averageProcessorLoad(parr,1);
		maxCharesPerProcessor(parr,ogr,1);
		minCharesPerProcessor(parr,ogr,1);
		averageCharesPerProcessor(parr,ogr,1);
		allInfoMessagesAmongProcessors(parr,ogr,1);
		allInfoMessagesAmongNumaNodes(parr,ogr,1);*/
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

		best_weight = NUMALB_MAX_DOUBLE;
		best_proc = 0;
		parr->procs[ ogr->vertices[obj].getCurrentPe() ].setTotalLoad( //remove the load of the chare   
			parr->procs[ ogr->vertices[obj].getCurrentPe() ].getTotalLoad()
			- ogr->vertices[obj].getVertexLoad() );		

		node = -1;	//no NUMA node has been considered yet
		for( proc=0 ; proc<parr->procs.size() ; proc++){
			if( !parr->procs[proc].isAvailable() ) continue;
			if( node != cpusToNodes[proc] ){ //only computes a new weight if it is not the same for the last processor
						       	 //the weight will be the same if both processors are in the same NUMA node
				weight = 0.0;
				node = cpusToNodes[proc]; 
				for( comm=0 ; comm<ogr->vertices[obj].recvFromList.size() ; comm++ ){
					int ngb_id = ogr->vertices[obj].recvFromList[comm].getNeighborId(); //neighbor's id
					int ngb_proc = ogr->vertices[ngb_id].getNewPe(); //neighbor's processor
					int ngb_node = cpusToNodes[ngb_proc];				//neighbor's NUMA node
					if( node == ngb_node )
						weight -= ogr->vertices[obj].recvFromList[comm].getNumMsgs();//local communication	
					else 	weight += ogr->vertices[obj].recvFromList[comm].getNumMsgs() *
						( numaFactorMatrix[ node*max_nodes + ngb_node ] );	//remote communication
				}	//end for comm
			}
			final_weight = weight*alpha + parr->procs[proc].getTotalLoad();
			
			if( final_weight < best_weight ){
				best_weight = final_weight;
				best_proc = proc;
			}
		}	//end for procs
		parr->procs[best_proc].setTotalLoad(			//add the load of the chare
			parr->procs[best_proc].getTotalLoad()
			+ ogr->vertices[obj].getVertexLoad() );
		ogr->vertices[obj].setNewPe(best_proc);		//set new processor
	}	//end for chares
	/** ============================== CLEANUP ================================ */
	if(_lb_args.debug()){
		CkPrintf("After LB\n");
	//	allStats(parr,ogr);
		maxProcessorLoad(parr,1);
		minProcessorLoad(parr,1);
	}
  	ogr->convertDecisions(stats);         // Send decisions back to LDStats
	
}

#include "NumaLB.def.h"

/*@}*/

