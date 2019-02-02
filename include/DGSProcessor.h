#ifndef DGSPROCESSOR_H
#define DGSPROCESSOR_H

#include "GEBSort.h"
#include "GTMerge.h"
#include "GRProcessor.h"

class DGSProcessor : public GRProcessor {
private:
    int nGsGe;

    PARS* pars;

    /* pointers to ROOT spectra */

    TH1D* hEventCounter;
    TH2F* hGeCounter, *hBGOCounter;
    TH2F* hEhiRaw, *hEhiCln, *hEhiDrty;
    TH2F* hGeBGO_DT;
    TH2F* hTrB, *hFwB;//,*hE_TrB[NGE+1];

    /* Gain Calibrtation */
    float M; // changed from 350.0
    float* ehigain;
    float* ehioffset;
    float* ehibase;
    float* ehiPZ;

public:
    DGSProcessor ( int nGsGe_, int* tlkup_, int* tid_, int* ng_, PARS* pars_ );
    ~DGSProcessor() {}

    int* tlkup;
    int* tid;
    int* ng;

    void GetCal ( char* file );
    int SupDGS ();

    int DGSEvDecompose_v3 ( unsigned int* ev, int len, DGSEVENT* thedgsEvt );
    int BinDgs ( GEB_EVENT* theGEBEvent, DGSEVENT* thedgsEvt );
};

#endif
