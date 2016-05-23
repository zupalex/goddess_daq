#ifndef SIDET_H
#define SIDET_H

#include <map>
#include "Detector.h"

///A class to handle basic silicon detectors.
/**
 * \author Karl Smith
 * \date July 2015
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
    ValueMap enRawP;
    ///Vector of raw energies for n type contacts.
    ValueMap enRawN;
    ///Vector of calibrated energies for p type contacts.
    ValueMap enCalP;
    ///Vector of calibrated energies for n type contacts.
    ValueMap enCalN;

    ///Map of p-type contact timestamps.
    TimeMap timeP;
    ///Map of n-type contact timestamps.
    TimeMap timeN;

    ///The calibration parameters for p type contacts
    std::vector<std::vector<float>> parEnCalP; //!
    ///The calibration parameters for n type contacts
    std::vector<std::vector<float>> parEnCalN; //!

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
    void SetNumContacts(int pType, int nType = 0);

    ///Check if specified contact has been declared.
    bool ValidContact(unsigned int contact, bool nType);

    ///Set the raw energy of the channel.
    virtual void SetRawValue(unsigned int channel, unsigned int rawValue, int ignThr);
    ///Set the raw energy of the contact and compute the calibrated value.
    virtual void SetRawValue(unsigned int contact, bool nType, unsigned int rawValue, int ignThr);
    ///Set the timestamp for the channel.
    virtual void SetTimeStamp(unsigned int contact, bool contactType, unsigned long long timestamp);

    ///Get the calibrated energy of the contact specified.
    float GetCalEnergy(int contact, bool nType = false);
    ///Return the total number of fired contacts above theshold.
    int GetContactMult();
    ///Return the number of fired contacts above threhsold for the specified type.
    int GetContactMult(bool contactType);

    ///Return the smallest timestamp.
    unsigned long long GetTimeStamp();

    ///Return true if the contact energy was set.
    bool ContactHit(int contact, bool nType);

    ///Return the number of channels of the type specified.
    int GetNumChannels(bool nType);

    ///Specify the polynomial calibration parameters of the specified contact.
    bool SetEnergyCalib(std::vector<float> par, int contact, bool contactType = siDet::pType);

    ///Specify the thresholds for each channel.
    bool SetThresholds(std::vector<int> thresholds, bool contactType = siDet::pType, int thrSize = 0);

    ///Get a map of the raw energies.
    ValueMap GetRawEn(bool nType);
    ///Get a map of the calibrated energies.
    ValueMap GetCalEn(bool nType);
   
    ///Get a map of the thresholds.
    std::vector<int> GetThresholds(bool nType);
    
    ClassDef(siDet, 1)

};

#endif
