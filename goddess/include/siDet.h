#ifndef SIDET_H
#define SIDET_H

#include "Detector.h"

///A class to handle basic silicon detectors.
/**
 * created by Karl Smith July 2015
 * updated by Alex Lepailleur June 2016
 */
class siDet : public Detector
{
public:
    ///A short hand for a map of values per channel.
    typedef std::map<short, float> ValueMap;
    ///A short hand for a map of timestamps per channel.
    typedef std::map<short, unsigned long long> TimeMap;
    static const bool nType = true;
    static const bool pType = false;

private:
    unsigned short numPtype;
    unsigned short numNtype;

    ///Vector of raw energies for p type contacts.
    ValueMap enRawPMap;
    ///Vector of raw energies for n type contacts.
    ValueMap enRawNMap;
    ///Vector of calibrated energies for p type contacts.
    ValueMap enCalPMap;
    ///Vector of calibrated energies for n type contacts.
    ValueMap enCalNMap;

    ///Map of p-type contact timestamps.
    TimeMap timePMap;
    ///Map of n-type contact timestamps.
    TimeMap timeNMap;

    ///The calibration parameters for p type contacts
    std::vector<std::vector<double>> parEnCalP; //!
    ///The calibration parameters for n type contacts
    std::vector<std::vector<double>> parEnCalN; //!

    ///The threshold values for the p-type contact in channel number.
    std::vector<int> threshP;
    ///The threshold values for the n-type contact in channel number.
    std::vector<int> threshN;

public:
    ///Default constructor.
    siDet();
    ///Default destructor.
    virtual ~siDet();
    ///Clear the stored detector values.
    void Clear();
    ///Set the number of contacts on the silicon detector.
    void SetNumContacts ( int pType, int nType = 0 );

    ///Check if specified contact has been declared.
    bool ValidContact ( unsigned int contact, bool nType );

    ///Set the raw energy of the channel.
    virtual void SetRawValue ( unsigned int channel, unsigned int rawValue, int ignThr );
    ///Set the raw energy of the contact and compute the calibrated value.
    virtual void SetRawValue ( unsigned int contact, bool nType, unsigned int rawValue, int ignThr );
    ///Set the timestamp for the channel.
    virtual void SetTimeStamp ( unsigned int contact, bool contactType, unsigned long long timestamp );

    virtual float GetEnSum ( bool nType = false, bool calibrated = true, float pos = 0 ) = 0;
    virtual float GetPosCh (bool calibrated = true) = 0;
    virtual float UpdatePosCh (float posch = 0) = 0;
    virtual void GetMaxHitInfo ( int* stripMaxP, long long unsigned int* timeSampMaxP, int* stripMaxN, long long unsigned int* timeSampMaxN, bool calibrated = true ) = 0;

    virtual void SortAndCalibrate ( bool doCalibrate = true ) = 0;

    virtual std::vector<float> GetHitsInfo ( std::string info, std::vector<float>* dest = nullptr ) = 0;
    virtual std::vector<int> GetHitsInfo ( std::string info, std::vector<int>* dest = nullptr ) = 0;
    virtual std::vector<long long unsigned int> GetHitsInfo ( std::string info, std::vector<long long unsigned int>* dest = nullptr ) = 0;
    virtual int GetMultiplicity ( bool nType = false, bool calibrated = true ) = 0;

    ///Get the calibrated energy of the contact specified.
    float GetCalEnergy ( int contact, bool nType = false );
    ///Return the total number of fired contacts above theshold.
    virtual int GetContactMult ( bool calibrated = true ) = 0;
    ///Return the number of fired contacts above threhsold for the specified type.
    virtual int GetContactMult ( bool contactType, bool calibrated ) = 0;

    ///Return the smallest timestamp.
    unsigned long long GetTimeStamp();

    ///Return true if the contact energy was set.
    bool ContactHit ( int contact, bool nType );

    ///Return the number of channels of the type specified.
    int GetNumChannels ( bool nType );

    ///Specify the polynomial calibration parameters of the specified contact.
    bool SetEnergyCalib ( std::vector<double> par, int contact, bool contactType = siDet::pType );

    ///Specify the thresholds for each channel.
    bool SetThresholds ( std::vector<int> thresholds, bool contactType = siDet::pType, int thrSize = 0 );

    ///Get a map of the raw energies.
    ValueMap GetRawEn ( bool nType );
    ///Get a map of the calibrated energies.
    ValueMap GetCalEn ( bool nType );

    TimeMap GetTsMap ( bool nType );

    std::vector<std::vector<double>> GetEnParCal ( bool nType = false );

    ///Get a map of the thresholds.
    std::vector<int> GetThresholds ( bool nType );

    ClassDef ( siDet, 1 )

};

#endif

