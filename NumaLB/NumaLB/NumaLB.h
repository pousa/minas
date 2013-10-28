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

#ifndef _NUMALB_H_
#define _NUMALB_H_

#include "CentralLB.h"
#include "NumaLB.decl.h"
#include "archTopology.h"
#include "ckgraph.h"
#include "lbstats.h"

void CreateNumaLB();
BaseLB * AllocateNumaLB();

class NumaLB : public CentralLB {
public:
    NumaLB(const CkLBOptions &);
    NumaLB(CkMigrateMessage *m);
    void pup(PUP::er &p){ CentralLB::pup(p); }
    CmiBool QueryBalanceNow(int step);
    void work(LDStats* stats);

private:
    void init();
    double alpha;
    float* numaFactorMatrix;
    int* cpusToNodes;
    int max_cpus;
    int max_nodes;
};

#endif

/*@}*/

