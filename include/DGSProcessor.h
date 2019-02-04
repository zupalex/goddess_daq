#ifndef DGSPROCESSOR_H
#define DGSPROCESSOR_H

#include "GEBSort.h"
#include "GTMerge.h"
#include "GRProcessor.h"

class DGSProcessor : public GRProcessor {
private:

public:
    DGSProcessor ():GRProcessor( GRProcessor::nGsGe, GRProcessor::tlkup, GRProcessor::tid, GRProcessor::ng, GRProcessor::pars){};
    ~DGSProcessor() {};

    void GetCal ( char* file );
    int SupDGS ();

    int DGSEvDecompose_v3 ( unsigned int* ev, int len, DGSEVENT* thedgsEvt );
     int BinDgs ( GEB_EVENT* theGEBEvent, DGSEVENT* thedgsEvt );
    
    //int Bingr ( GEB_EVENT* theGEBEvent, DGSEVENT* thedgsEvt );
};

#endif
