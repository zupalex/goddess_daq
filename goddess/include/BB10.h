#ifndef BB10_H
#define BB10_H

#include "orrubaDet.h"
#include "TVector3.h"

///Class designed to compute hit information on a Micron BB10 detector.
/**The BB10 is a single sided strip, rectangular, detector. It has eight strips.
 *
 * created by Karl Smith July 2015
 * updated by Alex Lepailleur June 2016
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

    ///Active width of the detector. Used in geometry reconstruction.
    double activeWidth;

    ///Returns the position of the center of a front side strip
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
    virtual int GetContactMult ( bool calibrated = true );
    ///Return the number of fired contacts above threhsold for the specified type.
    virtual int GetContactMult ( bool contactType, bool calibrated );

    ///Returns the sum of the energies gathered on the front (back if nType is true) side.
    virtual float GetEnSum ( bool nType = false, bool calibrated = true, float pos = 0 );
    
    //for BB10 and QQQ5 only returns zero
    virtual float GetPosCh(bool calibrated = true, bool in_encal = false, float e_near = 0, float e_far = 0);
    
    virtual float UpdatePosCh (float posch = 0);

    ///Assign the proper strip number (and calibrate in energy)
    virtual void SortAndCalibrate ( bool doCalibrate = true );

    ///Return the computed event position.
    TVector3 GetEventPosition ( bool calibrated = true );

    ///Set the geometry parameters for the detector
    virtual void SetGeomParams ( map<string, double> geomInfos_ );

    ///Set the raw energy of the contact and compute the calibrated value.
    virtual void SetRawValue ( unsigned int contact, bool nType, int rawValue, int ignThr );

    ///If the energy is dependent of the position of interaction in the detector, this function assign an Energy vs. Position correction graph
    /// \note used only for SX3s or other position sensitive detectors
    virtual void SetEnShiftVsPosGraph ( std::string graphFileName );

    ///The list of front side strips which fired
    std::vector<int> stripsP;

    ///The list of raw energies gathered on the front side
    std::vector<float> enRawP;

    ///The list of calibrated energies gathered on the front side
    std::vector<float> enCalP;

    ///The list of timestamps associated to each hits
    std::vector<long long unsigned int> timeP;

    ///Returns the vector containing the data corresponding to the requested "info"
    virtual std::vector<float> GetHitsInfo ( std::string info, std::vector<float>* dest = nullptr );

    ///Returns the vector containing the data corresponding to the requested "info"
    virtual std::vector<int> GetHitsInfo ( std::string info, std::vector<int>* dest = nullptr );

    ///Returns the vector containing the data corresponding to the requested "info"
    virtual std::vector<long long unsigned int> GetHitsInfo ( std::string info, std::vector<long long unsigned int>* dest = nullptr );

    ///Set the values of the parameters using the hit with the highest energy
    virtual void GetMaxHitInfo ( int* stripMaxP, long long unsigned int* timeSampMaxP, int* stripMaxN, long long unsigned int* timeSampMaxN, bool calibrated = true );

    ///Returns the multiplicity of the event
    virtual int GetMultiplicity ( bool nType = false, bool calibrated = true );

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( BB10, 1 )
    /// \endcond
};

#endif

