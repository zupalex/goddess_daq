#ifndef LIQUIDSCINT_H
#define LIQUIDSCINT_H

#include "Detector.h"

class LiquidScint : public Detector
{
private:
    std::string description;

    float enRaw;
    float psdRaw;
    float tacRaw;

public:
    LiquidScint();
    LiquidScint ( std::string desc );
    virtual ~LiquidScint();
    static const unsigned short numChannels = 3;

    ///Return the number of channels of the type specified.
    int GetNumChannels ( bool secondary = false )
    {
        ( void ) secondary; // to prevent useless warning about this variable not being used currently...
        return numChannels;
    };

    ///Set the raw energy of the channel.
    virtual void SetRawValue ( unsigned int detectorChannel, bool secondaryType, unsigned int rawValue, int ignThr );
    ///Set the timestamp for the channel.
    virtual void SetTimeStamp ( unsigned int detectorChannel, bool secondaryType, unsigned long long timestamp );

    void SetDescription ( std::string desc )
    {
        description = desc;
    };
    std::string GetDescription()
    {
        return description;
    };

    void Clear();

    float GetRawEnergy();
    float GetRawPSD();
    float GetRawTAC();


    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( LiquidScint, 1 );
    /// \endcond
};

#endif
