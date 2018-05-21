#include "IonChamber.h"
#include <cmath>
#include <iostream>

/**
 * \note We assume the channels are connected with the anodes first in order of beam
 * direction, followed by the scintillator energy and then time for each PMT.
 *
 * \param[in] numAnode The number of DAQ channels used to read out the ion chamber
 *  anodes.
 * \param[in] numScint The number of DAQ channels used to read out the scintillator.
 * \param[in] numDE The number of anodes summed to compute the energy loss.
 * \param[in] numEres The number of anodes summed to compute the residual energy.
 *
 */
IonChamber::IonChamber ( int numAnode, int numScint, int nDE, int nEres ) :
    numDE ( nDE ), numEres ( nEres )
{
    anodeRaw.resize ( numAnode );
    anodeCal.resize ( numAnode );

    scintRawE.resize ( numScint );
    scintRawT.resize ( numScint );
    scintCalE.resize ( numScint );
    scintCalT.resize ( numScint );

    parAnodeEnCal.resize ( numAnode );
    parScintEnCal.resize ( numScint );
    parScintTimeCal.resize ( numScint );

    Clear();
}

IonChamber::~IonChamber()
{

}

void IonChamber::Clear()
{
    anodeRaw.assign ( anodeRaw.size(), 0 );
    anodeCal.assign ( anodeCal.size(), 0 );

    scintRawE.assign ( scintRawE.size(), 0 );
    scintRawT.assign ( scintRawT.size(), 0 );
    scintCalE.assign ( scintCalE.size(), 0 );
    scintCalT.assign ( scintCalT.size(), 0 );

    dE = 0;
    resE = 0;
    E = 0;
}

/**Set the raw value of the specified channel and compute the calibrated value.
 * The dE, resE and E are computed.
 *
 * \param[in] channel The channel of the ion chamber.
 * \param[in] rawValue The raw DAQ value.
 */
void IonChamber::SetRawValue ( unsigned int channel, bool scintType, unsigned int rawValue, int /*ignThr*/ )
{
    if ( !scintType && channel < anodeRaw.size() )
    {
        anodeRaw.at ( channel ) = rawValue;
        for ( size_t power = 0; power < parAnodeEnCal.at ( channel ).size(); power++ )
        {
            anodeCal.at ( channel ) += parAnodeEnCal.at ( channel ).at ( power ) * pow ( rawValue, power );
        }

        //Compute the dE values
        dE = 0;
        for ( int ch = 0; ch < numDE; ch++ )
        {
            dE += anodeRaw.at ( ch );
        }

        resE = 0;
        for ( int ch = 0; ch < numEres; ch++ )
        {
            resE += anodeRaw.at ( ch + numDE );
        }

        E = dE + resE;
    }
    else if ( scintType && channel < scintRawE.size() )
    {
        //We expect the energy signal and then the time.
        if ( channel % 2 == 0 )
        {
            //We perform integer division to get the appropriate PMT number.
            channel /= 2;
            scintRawE.at ( channel ) = rawValue;
            for ( size_t power = 0; power < parScintEnCal.at ( channel ).size(); power++ )
            {
                scintCalE.at ( channel ) += parScintEnCal.at ( channel ).at ( power ) * pow ( rawValue, power );
            }
        }
        else
        {
            //We perform integer division to get the appropriate PMT number.
            channel /= 2;
            scintRawT.at ( channel ) = rawValue;
            for ( size_t power = 0; power < parScintTimeCal.at ( channel ).size(); power++ )
            {
                scintCalT.at ( channel ) += parScintTimeCal.at ( channel ).at ( power ) * pow ( rawValue, power );
            }
        }
    }
    else
    {
        std::cerr << "ERROR: The channel specified, " << channel << " is not valid! Unable to set the energy.\n";
        return;
    }
}

///Dummy definition of SetTimeStamp to avoid warning flood about unused variables...
void IonChamber::SetTimeStamp ( unsigned int detectorChannel, bool secondaryType, unsigned long long timestamp )
{
    if ( secondaryType && detectorChannel && timestamp ) {}
}

bool IonChamber::ValidAnode ( size_t ch )
{
    if ( ( int ) ch < 0 || ch >= anodeRaw.size() )
    {
        std::cerr << "ERROR: Invalid anode channel: " << ch << "\n";
        return false;
    }
    return true;
}

bool IonChamber::ValidScint ( size_t ch )
{
    if ( ( int ) ch < 0 || ch >= scintRawE.size() )
    {
        std::cerr << "ERROR: Invalid scintillator channel: " << ch << "\n";
        return false;
    }
    return true;
}

void IonChamber::SetAnodeEnCalPars ( int ch, std::vector<float> pars )
{
    if ( ValidAnode ( ch ) )
    {
        parAnodeEnCal.at ( ch ) = pars;
    }
}
void IonChamber::SetScintEnCalPars ( int ch, std::vector<float> pars )
{
    if ( ValidScint ( ch ) )
    {
        parScintEnCal.at ( ch ) = pars;
    }
}
void IonChamber::SetScintTimeCalPars ( int ch, std::vector<float> pars )
{
    if ( ValidScint ( ch ) )
    {
        parScintTimeCal.at ( ch ) = pars;
    }
}

int IonChamber::GetNumChannels ( bool scintType )
{
    if ( scintType )
    {
        return scintRawE.size();
    }
    return anodeRaw.size();
}

ClassImp ( IonChamber )
