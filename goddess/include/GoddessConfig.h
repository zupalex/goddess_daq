#ifndef GODDESSCONFIG_H
#define GODDESSCONFIG_H

#include <map>
#include <utility>
#include <string>
#include "Detector.h"
#include "TClonesArray.h"
#include "IonChamber.h"
#include "LiquidScint.h"

#include "SolidVector.h"

struct GeomInfos
{
    double barrelRadius;
    double barrelLength;

    double barrelLayerSpacing;
    double endcapLayerSpacing;

    TVector3 SuperX3Offset;
    TVector3 qqq5Offset;
    TVector3 beamSpotOffset;

    double firstStripLowEdge;
    double firstStripWidth;
    double deltaPitch;

    double qqq5RefPhi;
    double qqq5DeltaPhi;

    double qqq5RefZrot;
    double qqq5DeltaZrot;

    double sX3ActiveLength;
    double sX3ActiveWidth;

    double sX3RefPhi;
    double sX3DeltaPhi;

    double sX3RefZrot;
    double sX3DeltaZrot;

    double bb10ActiveWidth;
};

class GoddessConfig
{
private:
    ///Read the configuration file.
    void ReadConfig ( std::string filename, std::string sx3EnAdjustFName = "", std::string qqq5EnAdjustFName = "" );
    ///Read the position configuration.
    void ReadPosition ( std::string filename );
    ///Read the config line for the ion chamber.
    IonChamber *ReadIonChamberConfig ( std::istringstream &line );

    ///Map of channels to silicon detector.
    std::map<std::pair<short, short>, std::pair<Detector *, bool>> chMap;
    typedef std::pair<short, short> MapKey;

    ///Pointer to array of registered superX3 detectors.
    TClonesArray *superX3s;
    ///Pointer to array of registered BB10 detectors.
    TClonesArray *bb10s;
    ///Pointer to array of registered QQQ5 detectors.
    TClonesArray *qqq5s;
    ///Pointer to the registered ion chamber.
    IonChamber *ionChamber;
    std::vector<LiquidScint *> liquidScints;

    ///Check if this type of detector is insertable in the channel map at the specified postion.
    bool IsInsertable ( short daqType, int daqCh, Detector* det_, int numDetCh );

public:
    ///Default constructor.
    GoddessConfig ( std::string positionFile, std::string configFile, std::string sx3EnAdjustFName = "", std::string qqq5EnAdjustFName = "" );
    ///Default destructor.
    virtual ~GoddessConfig();
    
//     GeomInfos geom;
    std::map<string, double> geomInfos;

    ///Gets the detector and contact number for the specified digitizer channel.
    Detector *SetRawValue ( short daqType, short digitizerCh, unsigned int rawValue, int ignThr, unsigned long long timestamp = 0 );

    ///Parse and id string to get position details.
    bool ParseID ( std::string id, short &sector, short &depth, bool &upStream );

    ///Compute vector and rotation angle to detector.
    SolidVector GetPosVector ( const std::string type, const short sector, const short depth, const bool upStream );

    ///Get a vector of all registered SuperX3s.
    TClonesArray *GetSuperX3s()
    {
        return superX3s;
    }
    ///Get a vector of all registered BB10s.
    TClonesArray *GetBB10s()
    {
        return bb10s;
    }
    ///Get a vector of all registered QQQ5s.
    TClonesArray *GetQQQ5s()
    {
        return qqq5s;
    }
    std::vector<LiquidScint *> GetLiquidScints()
    {
        return liquidScints;
    }

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( GoddessConfig, 1 );
    /// \endcond

};

#endif
