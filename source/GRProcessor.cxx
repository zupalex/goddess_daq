#include "GRProcessor.h"

GRProcessor::GRProcessor( int nGsGe_,  int* tlkup_, int* tid_, int* ng_, PARS* pars_) : DGSProcessor ( nGsGe_, tlkup_, tid_, ng_, pars_), GretProcessor ( nGsGe_, ng_, pars_ )
{

}
