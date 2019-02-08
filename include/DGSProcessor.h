#ifndef DGSPROCESSOR_H
#define DGSPROCESSOR_H

#include "GEBSort.h"
#include "GTMerge.h"


class DGSProcessor {
private:
    
    int nGsGe;

    PARS* pars;
    
        /* Gain Calibrtation */
    float M; // changed from 350.0

    float* ehigain;
    float* ehioffset;
    float* ehibase;
    float* ehiPZ;
    
        /* parameters */
    double* angle;
    double* anglePhi;
    double* dopCorFac;
    double* aCFac;
  
  
    /* pointers to ROOT spectra */

    TH1D* hEventCounter;
    TH2F* hGeCounter, *hBGOCounter;
    TH2F* hEhiRaw, *hEhiCln, *hEhiDrty;
    TH2F* hGeBGO_DT;
    TH2F* hTrB, *hFwB;//,*hE_TrB[NGE+1];

    /* Other variables */
    unsigned long long int  EvTimeStam0 = 0;

public:
  

    int* tlkup;
    int* tid;
    int* ng;
  
    DGSProcessor ( int nGsGe_, int* tlkup_, int* tid_, int* ng_, PARS* pars_ );
    ~DGSProcessor() {}

    void GetCal ( char* file );
    int SupDGS ();

    int DGSEvDecompose_v3 ( unsigned int* ev, int len, DGSEVENT* thedgsEvt );
    int BinDgs ( GEB_EVENT* theGEBEvent, DGSEVENT* thedgsEvt );
};

#endif
