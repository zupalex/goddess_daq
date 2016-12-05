#ifndef ORRUBADET_H
#define ORRUBADET_H

#include "siDet.h"
#include "SolidVector.h"
#include "TFile.h"
#include "TGraph.h"
#include "TString.h"
#include "TList.h"
#include "TIterator.h"

///A class to handle basic ORRUBA silicon detectors.
/**This class builds on a basic siDet by adding detector position information and
 * serial number.
 *
 * \author Karl Smith
 * \date July 2015
 */
class orrubaDet : public siDet
{
private:

protected:
    ///Serial number assigned to the detector.
    std::string serialNum;

    ///Which region of the barrel the detector is located.
    unsigned short sector;
    ///The number of detectors between this one and the target.
    unsigned short depth;
    ///Whether the detector is upstream or downstream of the target.
    bool upStream;

    ///The string form of the position ID.
    std::string posID;

    ///The detector position defining the center of the active silicon area.
    SolidVector detPos; //!

    ///Determines the position ID string.
    virtual void SetPosID();

    //Detector daq type
    unsigned short daqtype;


public:
    ///Default constructor.
    orrubaDet();
    orrubaDet ( std::string serialNum, unsigned short sector, unsigned short depth,
                bool upStream, SolidVector position );
    ///Default destructor.
    virtual ~orrubaDet();

    ///Get the detector's serial number.
    std::string GetSerialNum()
    {
        return serialNum;
    }

    ///Return the position id string of the detector.
    std::string GetPosID()
    {
        return posID;
    };
    ///Return the depth of the detector.
    /**Depth dE = 0, E1 =1, E2 = 2.
     */
    unsigned short GetDepth()
    {
        return depth;
    }
    /// Returns whether detector is upstream or downstream
    bool GetUpStream()
    {
        return upStream;
    }
    ///Returns the detctors sector.
    unsigned short GetSector()
    {
        return sector;
    }


    ///Abstract definition of a funtion to get the depositied energy of a strip.
    virtual float GetEnergy() = 0;
    ///Abstract definition of a funtion to get the strip number and depositied energy of a front strip.
    virtual std::pair<int, float> GetPtypeEnergy() = 0;
    ///Abstract definition of a funtion to get the strip number and depositied energy of a back strip.
    virtual std::pair<int, float> GetNtypeEnergy() = 0;
    ///Abstract definition of a funtion to get the vector containing resistive strip energy calibration polynomial parameters (implemented in SuperX3 class only).
//     virtual std::vector<float>* GetResStripParCal() = 0;

    virtual TVector3 GetPStripCenterPos ( int strip ) = 0;

    ///Abstract defintion of a function to get the position of an event on the detector.
    virtual TVector3 GetEventPosition ( int pStripHit, int nStripHit, float eNear, float eFar ) = 0;

    void SetDetector ( std::string serialNum, unsigned short sector, unsigned short depth, bool upStream, SolidVector position, std::string enShiftVsPosFName );
    virtual void ConstructBins() = 0;

    void SetDAQType ( unsigned short daqtype );
    unsigned short GetDAQType()
    {
        return daqtype;
    };

    virtual void SetEnShiftVsPosGraph ( std::string graphFileName ) = 0;

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( orrubaDet, 1 );
    /// \endcond
};


#endif

