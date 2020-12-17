#ifndef QQQ5_H
#define QQQ5_H

#include "orrubaDet.h"
#include "TVector3.h"

///Class designed to compute hit information on a Micron QQQ5 detector.
/**The QQQ5 is a annular detector.
 *
 * created by Karl Smith July 2015
 * updated by Alex Lepailleur June 2016
 */
class QQQ5 : public orrubaDet
{
private:
    ///Vector pointing to mid point of p type strip edge in mm.
    TVector3 pStripEdgePos[33]; //!
    ///Vector pointing to mid point of n type strip edge in mm.
    TVector3 nStripEdgePos[5]; //!

    ///The bin edges along the p-type strips in mm.
    Float_t binsP[33]; //!
    ///The bin center along the p-type strips in mm.
    Float_t binsPcenter[32]; //!
    ///The bin edges along the n-type strips in mm.
    Float_t binsN[5]; //!
    ///The bin center along the n-type strips in mm.
    Float_t binsNcenter[4]; //!
    ///The bin edges along the cylindrical radial directions rho in degrees.
    Float_t binsRho[33]; //!
    ///The bin edges along the phi axis in degrees.
    Float_t binsAzimuthal[5]; //!
    ///The bin edges along the phi axis in degrees.
    Float_t binsPolar[33]; //!
    ///The bin center along the phi axis in degrees.
    Float_t binsPolarcenter[33]; //!

    ///The absolute centers of the front side strips
    TVector3 pStripCenterPos[32];
    ///The absolute centers of the back side strips
    TVector3 nStripCenterPos[4];

    ///Computed raw position of event.
    float stripPosRaw[4];
    ///Computed calibrated position of event.
    float stripPosCal[4];

    ///The constructed position of the event.
    TVector3 eventPos;

    ///Construct array of bin edges.
    void ConstructBins();

protected:
    ///Determines the position ID string.
    void SetPosID();

public:
    ///Default constructor.
    QQQ5();
    QQQ5 ( std::string serialNum, unsigned short sector, unsigned short depth,
           bool upStream, SolidVector position );
    ///Default destructor.
    virtual ~QQQ5();

    ///Clear the stored values in this detector.
    void Clear();

    double firstStripWidth;
    double deltaPitch;

    TVector3 GetPStripCenterPos ( int strip )
    {
        return pStripCenterPos[strip];
    }

    TVector3* GetNStripCenterPos()
    {
        return nStripCenterPos;
    }

    ///Return the number of bins.
    static int GetNumNtypeBins()
    {
        return 4;
    }
    ///Return the number of bins.
    static int GetNumPtypeBins()
    {
        return 32;
    }
    ///Return the bins boundaries of the dimension along the p type strips in mm.
    float* GetPtypeBins()
    {
        return binsP;
    };
    ///Return the bins center of the dimension along the p type strips in mm.
    float* GetPtypeBinsCenter()
    {
        return binsPcenter;
    };
    ///Return the bins boundaries of the dimension along the n type strips in mm.
    float* GetNtypeBins()
    {
        return binsN;
    };
    ///Return the bins center of the dimension along the n type strips in mm.
    float* GetNtypeBinsCenter()
    {
        return binsNcenter;
    };
    ///Return the  in mm.
    float* GetRhoBins()
    {
        return binsRho;
    };
    ///Return the bins along the azimuthal direction in degrees.
    float* GetAzimuthalBins()
    {
        return binsAzimuthal;
    };
    ///Return the bins along the polar direction in degrees.
    float* GetPolarBins()
    {
        return binsPolar;
    };
    ///Return the bins centers along the polar direction in degrees.
    float* GetPolarBinsCenter()
    {
        return binsPolarcenter;
    };

    ///Return the total number of fired contacts above theshold.
    virtual int GetContactMult ( bool calibrated = true );
    ///Return the number of fired contacts above threhsold for the specified type.
    virtual int GetContactMult ( bool contactType, bool calibrated );

    virtual float GetEnSum ( bool nType = false, bool calibrated = true, float pos = 0 );
    virtual float GetPosCh(bool calibrated = true, bool in_encal = false, float e_near = 0, float e_far =0);
    virtual float UpdatePosCh (float posch = 0);
  
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
    ClassDef ( QQQ5, 1 );
    /// \endcond
};


#endif
