#ifndef ORRUBA_H
#define ORRUBA_H

#include "TObject.h"
#include "TVector3.h"

class ORRUBA : public TObject
{
private:
    ///Energy loss in first layer, may be zero if only one layer.
    float dE;
    ///Energy deposited in second layer.
    float E1;
    ///Energy deposited in third layer, may be zero if only two layers.
    float E2;

    ///The event position.
    TVector3 pos;

    ///A string specifying which quadrant the position occurred.
    std::string quadrant;
public:
    ORRUBA();

    ///Set the event parameters;
    void SetEvent(std::string quadrant_, float dE_, float E1_, float E2_, TVector3 pos_);

    ClassDefNV(ORRUBA, 1)
};

#endif
