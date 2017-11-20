#include "ORRUBA.h"

ORRUBA::ORRUBA()
{
}

void ORRUBA::SetEvent ( std::string quadrant_, float dE_, float E1_, float E2_, TVector3 pos_ )
{
    quadrant = quadrant_;
    dE = dE_;
    E1 = E1_;
    E2 = E2_;
    pos = pos_;
}

ClassImp ( ORRUBA )
