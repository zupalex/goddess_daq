#ifndef QQQ5_H
#define QQQ5_H

#include "orrubaDet.h"
#include "TVector3.h"

///Class designed to compute hit information on a Micron QQQ5 detector.
/**The QQQ5 is a annular detector.
 *
 * \author Karl Smith
 * \date July 2015
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

    TVector3 pStripCenterPos[32];
    TVector3 nStripCenterPos[4];

    ///Pair strip# and calibrated energy of the p type side.
    std::pair<int, float> enPtype;
    ///Pair strip# and calibrated energy of the n type side.
    std::pair<int, float> enNtype;
    ///The total deposited energy in the detector.
    float enCal;

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

    TVector3* GetPStripCenterPos()
    {
        return pStripCenterPos;
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

    ///Return the computed event position.
    TVector3 GetEventPosition(int pStripHit, int nStripHit, float eNear, float eFar);
    ///Return a pair (strip#,energy)from the p type side.
    std::pair<int, float> GetPtypeEnergy()
    {
        return enPtype;
    };
    ///Return a pair (strip#,energy) from the n type side.
    std::pair<int, float> GetNtypeEnergy()
    {
        return enNtype;
    };
    ///Return the total energy deposited in the detector.
    float GetEnergy()
    {
        return enCal;
    };

    ///Set the raw energy of the contact and compute the calibrated value.
    virtual void SetRawValue ( unsigned int contact, bool nType, int rawValue, int ignThr );

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( QQQ5, 1 );
    /// \endcond
};


#endif
