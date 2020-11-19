#include "siDet.h"
#include <GEBSort.h>
#include <cmath>
#include <iostream>
#include <limits.h>

siDet::siDet() :
    numPtype ( 0 ), numNtype ( 0 )
{
    Clear();
}
siDet::~siDet()
{

}


void siDet::Clear()
{
    enRawPMap.clear();
    enRawNMap.clear();
    enCalPMap.clear();
    enCalNMap.clear();
    timePMap.clear();
    timeNMap.clear();
}

void siDet::SetNumContacts ( int pType, int nType/*=0*/ )
{
    numPtype = pType;
    numNtype = nType;

    parEnCalP.resize ( pType );
    parEnCalN.resize ( nType );
}

/**
 * \param[in] contact The contact number to validate.
 * \param[in] nType Whether the contact is nType.
 *  \return True is contact has been declared.
 */
bool siDet::ValidContact ( unsigned int contact, bool nType/*=false*/ )
{
    size_t size = numPtype;
    if ( nType )
    {
        size = numNtype;
    }
    if ( contact >= size )
    {
        fprintf ( stderr, "ERROR: Contact specified, %u, is invalid!\n", contact );
        return false;
    }

    return true;
}


/**The raw value is stored and the calibrated value is computed based on the loaded
 * calibration parameters.
 *
 *  \param[in] channel The channel to be adjusted.
 *  \param[in] rawValue The raw contact value in channels.
 */
void siDet::SetRawValue ( unsigned int channel, unsigned int rawValue, int ignThr )
{
    if ( channel < numPtype )
    {
        SetRawValue ( channel, siDet::pType, rawValue, ignThr );
    }
    else if ( channel < numPtype + numNtype )
    {
        SetRawValue ( channel, siDet::nType, rawValue, ignThr );
    }
    else
    {
        std::cerr << "ERROR: Cannot set raw value for invalid channel: " << channel << "!\n";
    }
}
/**The raw value is stored and the calibrated value is computed based on the loaded
 * calibration parameters.
 *
 *  \param[in] contact The contact to be adjusted.
 *  \param[in] rawValue The raw contact value in channels.
 *  \param[in] nType Whether the contact selected is n type.
 */
void siDet::SetRawValue ( unsigned int contact, bool nType, unsigned int rawValue, int ignThr )
{
    if ( !ValidContact ( contact, nType ) )
    {
        return;
    }

    //Get pointer to the raw an calibrated storage location.
//     float *enCal = 0;
    unsigned int threshold = 0;
//     std::vector<float> *parEnCal;
    if ( nType )
    {
        if ( contact < threshN.size() )
        {
            threshold = threshN.at ( contact );
        }

        if ( ignThr != 0 || ( ignThr == 0 && rawValue > threshold ) )
        {
            enRawNMap[contact] = rawValue;
        }
    }
    else
    {
        if ( contact < threshP.size() )
        {
            threshold = threshP.at ( contact );
        }

        if ( ignThr != 0 || ( ignThr == 0 && rawValue > threshold ) )
        {
            enRawPMap[contact] = rawValue;
        }
    }
}

void siDet::SetTimeStamp ( unsigned int contact, bool contactType, unsigned long long timestamp /*=0*/ )
{
    if ( !ValidContact ( contact, contactType ) )
    {
        return;
    }
    if ( contactType == siDet::nType )
    {
        timeNMap[contact] = timestamp;
    }
    else
    {
        timePMap[contact] = timestamp;
    }
}

std::vector< std::vector< double > > siDet::GetEnParCal ( bool nType )
{
    if ( nType ) return parEnCalN;
    else return parEnCalP;
}

float siDet::GetCalEnergy ( int contact, bool nType/*=false*/ )
{
    if ( !ValidContact ( contact, nType ) )
    {
        return 0;
    }
    ValueMap *enCal;
    if ( nType )
    {
        enCal = &enCalNMap;
    }
    else
    {
        enCal = &enCalPMap;
    }

    auto itr = enCal->find ( contact );
    if ( itr != enCal->end() )
    {
        return itr->second;
    }
    return 0;
}

/*
 * \return True if successful.
 */
bool siDet::SetEnergyCalib ( std::vector<double> par, int contact, bool nType/*=false*/ )
{
    if ( !ValidContact ( contact, nType ) )
    {
        return false;
    }
    if ( nType )
    {
        parEnCalN.at ( contact ) = par;
    }
    else
    {
        parEnCalP.at ( contact ) = par;
    }
    return true;
}

bool siDet::SetThresholds ( std::vector<int> thresholds, bool contactType, int thrSize )
{
    if ( thrSize == 0 ) thrSize = ( unsigned int ) GetNumChannels ( contactType );

    //if ( thresholds.size() != ( unsigned int ) GetNumChannels ( contactType ) )
    if ( thresholds.size() != ( unsigned int ) thrSize )
    {
        //std::cerr << "ERROR: Vector specified for thresholds was not the equal to the number of contacts (" << thresholds.size() << ">" << GetNumChannels ( contactType ) << ")!\n";
        std::cerr << "ERROR: Size of the vector specified for thresholds (" << thresholds.size() << ") was not the one expected (" << thrSize << ")!\n";
        return false;
    }
    if ( contactType == siDet::nType )
    {
        threshN = thresholds;
    }
    else
    {
        threshP = thresholds;
    }
    return true;
}

int siDet::GetNumChannels ( bool nType )
{
    if ( nType )
    {
        return numNtype;
    }
    return numPtype;
}

std::vector<int> siDet::GetThresholds ( bool nType )
{
    if ( nType )
    {
        return threshN;
    }
    return threshP;
}

siDet::ValueMap siDet::GetRawEn ( bool nType )
{
    if ( nType )
    {
        return enRawNMap;
    }
    return enRawPMap;
}

siDet::ValueMap siDet::GetCalEn ( bool nType )
{
    if ( nType )
    {
        return enCalNMap;
    }
    return enCalPMap;
}

siDet::TimeMap siDet::GetTsMap ( bool nType )
{
    if ( nType ) return timeNMap;
    else return timePMap;
}

bool siDet::ContactHit ( int contact, bool nType )
{
    ValueMap *map = &enRawPMap;
    if ( nType )
    {
        map = &enRawNMap;
    }

    if ( map->find ( contact ) == map->end() )
    {
        return false;
    }
    return true;
}

unsigned long long siDet::GetTimeStamp()
{
    unsigned long long timestamp = ULLONG_MAX;
    for ( auto itr = timePMap.begin(); itr != timePMap.end(); ++itr )
    {
        if ( timestamp > itr->second )
        {
            timestamp = itr->second;
        }
    }
    for ( auto itr = timeNMap.begin(); itr != timeNMap.end(); ++itr )
    {
        if ( timestamp > itr->second )
        {
            timestamp = itr->second;
        }
    }
    return timestamp;
}

ClassImp ( siDet )
