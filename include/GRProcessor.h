#ifndef GRPROCESSOR_H
#define GRPROCESSOR_H

#include "GEBSort.h"
#include "GTMerge.h"

#define max_inputs 16

class GRProcessor {

public:
  
  GRProcessor(int nGsGe_,  int* tlkup_, int* tid_, int* ng_, PARS* pars_);
  ~GRProcessor(){};

  
  map<pair<int,int>,pair<float,float>> angles_map;
    /* parameters */
    double* angle;
    double* anglePhi;

    double* dopCorFac;
    double* aCFac;
    
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

    /* Other variables */
    unsigned long long int  EvTimeStam0 = 0;
    
    int* tlkup;
    int* tid;
    int* ng;

    
    struct GlobalHeader
{
    int32_t type;
    int32_t length;
    int64_t timestamp;

};

struct Gretina_header
{
    int type;
    int crystal_id;
    int num; //number of interaction points from decomp
    float tot_e;
    int core_e[4];
    long long int timestamp;
    long long trig_time;
    float t0;
    float cfd;
    float chisq;
    float norm_chisq;
    float baseline;
    float prestep;
    float poststep;
    int pad; //non0 gives error type
    struct
    {
        float x;
        float y;
        float z;
        float e;
        int seg;
        float seg_ener;
    } intpts[max_inputs];
};

struct Gretina_hit
{
    float x;
    float y;
    float z;
    float r;
    float theta;
    float phi;
    float e;
    int seg;
    float seg_ener;
    float tot_e;
    int quad;
    int crystal;
};


struct Gretina_Gamma_Ray
{
    float x;
    float y;
    float z;
    float e;
    float seg;
    float seg_ener;
    float raw_e;
    int quad;
    int crystal;
    int timestamp;
};


private:


};

#endif