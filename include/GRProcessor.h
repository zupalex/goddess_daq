#ifndef GRPROCESSOR_H
#define GRPROCESSOR_H

#include "GEBSort.h"
#include "GTMerge.h"
#include "DGSProcessor.h"
#include "GretProcessor.h"


class GRProcessor : public DGSProcessor, public GretProcessor
{
public:
  
  GRProcessor( int nGsGe_,  int* tlkup_, int* tid_, int* ng_, PARS* pars_);
    
private:
  
};

#endif