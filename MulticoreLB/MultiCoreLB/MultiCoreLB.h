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

#ifndef _MultiCoreLB_H_
#define _MultiCoreLB_H_

#include <hwloc.h>

#include "CentralLB.h"
#include "MultiCoreLB.decl.h"
#include "archTopology.h"
#include "ckgraph.h"
#include "lbstats.h"

void CreateMultiCoreLB();
BaseLB * AllocateMultiCoreLB();

class MultiCoreLB : public CentralLB {
public:
    MultiCoreLB(const CkLBOptions &);
    MultiCoreLB(CkMigrateMessage *m);
    void pup(PUP::er &p){ CentralLB::pup(p); }
    CmiBool QueryBalanceNow(int step);
    void work(LDStats* stats);

private:
    void init();
    double alpha;

    
    /* Core-to-core factor square matrix. */
    double **MultiCoreFactorMatrix;

    /* The number of cores the current machine holds. */
    int ncores;

    /* The number of NUMA nodes the current machine holds. */
    int nnodes;
    
    /* Core-to-node correspondance. */
    int *cpusToNodes;

};

#endif

/*@}*/

