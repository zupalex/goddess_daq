#ifndef SUPERX3_H
#define SUPERX3_H

#include "orrubaDet.h"
#include "TVector3.h"

///Class designed to compute hit information on a Micron Super X3 detector.
/**The Super X3 (http://www.micronsemiconductor.co.uk/pdf/cat.pdf) is a double sided
 * silicon strip detector with four strips on each side. The p-type side is position
 * sensitive via a restive strip with contacts on each side of the strip. The
 * detector has an active area of 40.3 x 75.0 mm&sup2;.
 *
 * \author Karl Smith
 * \date July 2015
 */
class superX3 : public orrubaDet
{
private:
    ///Vector pointing to mid point of p type strip edge in mm.
    TVector3 pStripEdgePos[5]; //!
    ///Vector pointing to mid point of n type strip edge in mm.
    TVector3 nStripEdgePos[5]; //!

    ///The bin edges along the p-type strips in mm.
    Float_t binsP[5]; //!
    ///The bin edges along the n-type strips in mm.
    Float_t binsN[5]; //!
    ///The bin Centers along the p-type strips in mm.
    Float_t binsPCenter[4]; //!
    ///The bin Centers along the n-type strips in mm.
    Float_t binsNCenter[4]; //!
    ///The bin edges along the z-axis in mm.
    Float_t binsZ[5]; //!
    ///The bin Centers along the z-axis in mm.
    Float_t binsZCenter[4]; //!
    ///The bin edges along the phi axis in degrees.
    Float_t binsAzimuthal[5]; //!
    ///The bin centers along the phi axis in degrees.
    Float_t binsAzimuthalCenter[4]; //!
    ///The bin edges along the phi axis in degrees.
    Float_t binsPolar[5]; //!

    ///The absolute centers of the front side strips
    TVector3 pStripCenterPos[4];
    ///The absolute center of the back side strips
    TVector3 nStripCenterPos[4];

    ///Computed raw position of event.
    siDet::ValueMap stripPosRaw;
    ///Computed calibrated position of event.
    siDet::ValueMap stripPosCal;

    ///The number of contacts fired on a strip.
    int stripContactMult[4];

    ///The constructed position of the event.
    TVector3 eventPos;

    ///Vector containing position calibration polynomial parameters.
    std::vector<float> parPosCal[4]; //!
    ///Vector containing strip energy calibration polynomial parameters.
    std::vector<float> parStripEnCal[4]; //!

    ///Construct array of bin edges.
    void ConstructBins();

public:
    ///Default constructor.
    superX3();
    superX3 ( std::string serialNum, unsigned short sector, unsigned short depth,
              bool upStream, SolidVector position );
    ///Default destructor.
    virtual ~superX3();

    ///Clear the stored values in this detector.
    void Clear();

    double activeLength;
    double activeWidth;

    ///Calculate the position on the resistive strip detectors.
    void UpdatePosition ( int strip );

    ///Assign a vector of float to the calibration parameters of the specified strip.
    void SetStripPosCalibPars ( int strip, std::vector<float> );
    ///Assign a vector of float to the calibration parameters of the specified strip.
    void SetStripEnCalibPars ( int strip, std::vector<float> );

    ///Return the strip in which the specified contact is attached.
    static int GetStrip ( int contact );

    TVector3 GetPStripCenterPos ( int strip )
    {
        return pStripCenterPos[strip];
    }

    TVector3* GetNStripCenterPos()
    {
        return nStripCenterPos;
    }

    ///Return the number of bins.
    int GetNumBins()
    {
        return 4;
    }
    ///Return the bins boundaries of the dimension along the p type strips in mm.
    float* GetPtypeBins()
    {
        return binsP;
    };
    ///Return the bins boundaries of the dimension along the n type strips in mm.
    float* GetNtypeBins()
    {
        return binsN;
    };
    ///Returns the bin centers along the z (beam) direction in mm.
    float* GetPtypeCenterBins()
    {
        return binsPCenter;
    };
    ///Returns the bin centers along the z (beam) direction in mm.
    float* GetNtypeCenterBins()
    {
        return binsNCenter;
    };
    ///Return the bins boundaries along the z (beam) direction in mm.
    float* GetZbins()
    {
        return binsZ;
    };
    ///Returns the bin centers along the z (beam) direction in mm.
    float* GetZCenterBins()
    {
        return binsZCenter;
    };
    ///Return the bins along the azimuthal direction in degrees.
    float* GetAzimuthalBins()
    {
        return binsAzimuthal;
    };
    ///Return the bins center along the azimuthal direction in degrees.
    float* GetAzimuthalCenterBins()
    {
        return binsAzimuthalCenter;
    };
    ///Return the bins along the polar direction in degrees.
    float* GetPolarBins()
    {
        return binsPolar;
    };

    virtual float GetEnSum ( bool nType = false, bool calibrated = true );
    std::vector<float> GetResEn ( bool calibrated = true );
    float GetNearEn ( bool calibrated = true );
    float GetFarEn ( bool calibrated = true );

    virtual void SortAndCalibrate ( bool doCalibrate = true );

    ///Returns true if strip number is valid.
    bool ValidStrip ( int strip );

    ///Return the total number of fired contacts above theshold.
    virtual int GetContactMult();
    ///Return the number of fired contacts above threhsold for the specified type.
    virtual int GetContactMult ( bool contactType );

    ///Returns the ValueMap of the raw strips ranginf from -1 to +1.
    siDet::ValueMap GetStripPosRaw()
    {
        return stripPosRaw;
    };
    ///Returns the ValueMap of the calibrated strip positions in mm.
    siDet::ValueMap GetStripPosCal()
    {
        return stripPosCal;
    };
    ///Return the computed event position.
    TVector3 GetEventPosition ( bool calibrated = true );
    ///Return the vector containing resistive strip energy calibration polynomial parameters.
    std::vector<float>* GetResStripParCal()
    {
        return parStripEnCal;
    }
    ///Return the contact for the near end of the strip.
    static unsigned short GetNearContact ( unsigned short strip );
    ///Return the contact for the far end of the strip.
    static unsigned short GetFarContact ( unsigned short strip );

    virtual void SetGeomParams ( map<string, double> geomInfos_ );

    ///Set the raw energy of the contact and compute the calibrated value.
    virtual void SetRawValue ( unsigned int contact, bool nType, int rawValue, int ignThr );

    ///Graph giving the energy shift coefficient to apply as a function of the position interaction in the detector.
    TGraph* enShiftVsPosGraph[4];
    std::map<float, std::pair<float, float>> enJumpsCorrectionGraphs[4];
    virtual void SetEnShiftVsPosGraph ( std::string graphFileName );

    std::vector<int> stripsP;
    std::vector<float> enNearRaw;
    std::vector<float> enFarRaw;
    std::vector<float> enNearCal;
    std::vector<float> enFarCal;
    std::vector<long long unsigned int> timeNear;
    std::vector<long long unsigned int> timeFar;

    std::vector<int> stripsN;
    std::vector<float> enRawN;
    std::vector<float> enCalN;
    std::vector<long long unsigned int> timeN;

    virtual std::vector<float> GetHitsInfo ( std::string info, std::vector<float>* dest = nullptr );
    virtual std::vector<int> GetHitsInfo ( std::string info, std::vector<int>* dest = nullptr );
    virtual std::vector<long long unsigned int> GetHitsInfo ( std::string info, std::vector<long long unsigned int>* dest = nullptr );
    virtual void GetMaxHitInfo ( int* stripMaxP, long long unsigned int* timeSampMaxP, int* stripMaxN, long long unsigned int* timeSampMaxN, bool calibrated = true );
    virtual int GetMultiplicity ( bool nType = false, bool calibrated = true );

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( superX3, 1 );
    /// \endcond
};


#endif
