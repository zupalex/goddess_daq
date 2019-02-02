#ifndef GRPROCESSOR_H
#define GRPROCESSOR_H

#include "GEBSort.h"
#include "GTMerge.h"
#include "DGSProcessor.h"

class GRProcessor {

public:
  
  GRProcessor(){};
  ~GRProcessor(){};

protected: 

    /* parameters */
    double* angle;
    double* anglePhi;

    double* dopCorFac;
    double* aCFac;

    /* Other variables */
    unsigned long long int  EvTimeStam0 = 0;


private:


};

#endif