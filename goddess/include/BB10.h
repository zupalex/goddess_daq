#ifndef BB10_H
#define BB10_H

#include "orrubaDet.h"
#include "TVector3.h"

///Class designed to compute hit information on a Micron BB10 detector.
/**The BB10 is a single sided strip, rectangular, detector. It has eight strips.
 *
 * \author Karl Smith
 * \date July 2015
 */

class BB10 : public orrubaDet
{
private:
    ///Vector pointing to mid point of p type strip edge in mm.
    TVector3 pStripEdgePos[9]; //!

    ///The bin edges along the p-type strips in mm.
    Float_t binsP[9]; //!
    ///The bin edges along the phi axis in radians.
    Float_t binsAzimuthal[9]; //!
    ///The bin centers along the p-type strips in mm.
    Float_t binsPCenter[8]; //!
    ///The bin centers along the phi axis in radians.
    Float_t binsAzimuthalCenter[8]; //!

    ///The absolute centers of the strips
    TVector3 pStripCenterPos[8];

    ///The constructed position of the event.
    TVector3 eventPos;

    ///Construct array of bin edges.
    void ConstructBins();

public:
    ///Default constructor.
    BB10();
    BB10 ( std::string serialNum, unsigned short sector, unsigned short depth,
           bool upStream, SolidVector detPos );
    ///Default destructor.
    virtual ~BB10() {};

    ///Clear the stored values in this detector.
    void Clear();

    double activeWidth;

    TVector3 GetPStripCenterPos ( int strip )
    {
        return pStripCenterPos[strip];
    }

    ///Return the number of bins.
    int GetNumBins()
    {
        return 8;
    }
    ///Return the bins boundaries of the dimension along the p type strips.
    float* GetPtypeBins()
    {
        return binsP;
    };
    ///Return the bins along the azimuthal direction.
    float* GetAzimuthalBins()
    {
        return binsAzimuthal;
    };
    ///Return the bins centers of the dimension along the p type strips.
    float* GetPtypeBinCenters()
    {
        return binsPCenter;
    };
    ///Return the bin centers along the azimuthal direction.
    float* GetAzimuthalBinCenters()
    {
        return binsAzimuthalCenter;
    };

    ///Return the total number of fired contacts above theshold.
    virtual int GetContactMult();
    ///Return the number of fired contacts above threhsold for the specified type.
    virtual int GetContactMult ( bool contactType );

    virtual float GetEnSum ( bool nType = false, bool calibrated = true );

    virtual void SortAndCalibrate ( bool doCalibrate = true );

    ///Return the computed event position.
    TVector3 GetEventPosition ( bool calibrated = true );

    virtual void SetGeomParams ( map<string, double> geomInfos_ );

    ///Set the raw energy of the contact and compute the calibrated value.
    virtual void SetRawValue ( unsigned int contact, bool nType, int rawValue, int ignThr );

    virtual void SetEnShiftVsPosGraph ( std::string graphFileName );

    std::vector<int> stripsP;
    std::vector<float> enRawP;
    std::vector<float> enCalP;
    std::vector<long long unsigned int> timeP;

    virtual std::vector<float> GetHitsInfo ( std::string info, std::vector<float>* dest = nullptr );
    virtual std::vector<int> GetHitsInfo ( std::string info, std::vector<int>* dest = nullptr );
    virtual std::vector<long long unsigned int> GetHitsInfo ( std::string info, std::vector<long long unsigned int>* dest = nullptr );
    virtual void GetMaxHitInfo ( int* stripMaxP, long long unsigned int* timeSampMaxP, int* stripMaxN, long long unsigned int* timeSampMaxN, bool calibrated = true );
    virtual int GetMultiplicity ( bool nType = false, bool calibrated = true );

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( BB10, 1 )
    /// \endcond
};

#endif

