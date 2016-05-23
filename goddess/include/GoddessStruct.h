#ifndef GODDESSSTRUCT_H
#define GODDESSSTRUCT_H

#include <string>
#include <vector>
#include "TVector3.h"

///Structure of gamma ray data from DGS
struct GamData
{
    ///The calibrated energy of the gamma.
    float en;
    ///The type of gamma ray detector. GE=1, BGO=2
    int type;
    //The number of the gamma ray detector
    int num;
    ///The time of the gamma relative to the beginning of the event.
    unsigned int time;
};

///Structure of silicon data from ORRUBA
class SiDataBase
{
private:

public:
    ///Default constructor.
    SiDataBase();

    ///Default destructor.
    virtual ~SiDataBase();

    virtual void Clear();

    virtual float* SetMemberAddress ( std::string member, float** address );
    virtual int* SetMemberAddress ( std::string member, int** address );
    virtual std::vector<float>* SetMemberAddress ( std::string member, std::vector<float>** address );
    virtual std::vector<int>* SetMemberAddress ( std::string member, std::vector<int>** address );

    virtual float eSumLayer ( unsigned short layer = 1, bool isNType = false ) const;

    virtual int stripMaxLayer ( unsigned short layer = 1, bool isNType = false ) const;

//     ///The info about all the strips which fired and the energies collected by each of them for the dE layer.
//     float dE_eSum_p;
//     ///The info about all the strips which fired and the energies collected by each of them for the E1 layer.
//     float E1_eSum_p;
//     ///The info about all the strips which fired and the energies collected by each of them for the E2 layer.
//     float E2_eSum_p;
//
//     ///The info about all the strips which fired and the energies collected by each of them for the dE layer.
//     float dE_eSum_n;
//     ///The info about all the strips which fired and the energies collected by each of them for the E1 layer.
//     float E1_eSum_n;
//     ///The info about all the strips which fired and the energies collected by each of them for the E2 layer.
//     float E2_eSum_n;
//
//     ///The info about all the strips which fired and the energies collected by each of them for the dE layer.
//     int dE_stripMax_p;
//     ///The info about all the strips which fired and the energies collected by each of them for the E1 layer.
//     int E1_stripMax_p;
//     ///The info about all the strips which fired and the energies collected by each of them for the E2 layer.
//     int E2_stripMax_p;
//
//     ///The info about all the strips which fired and the energies collected by each of them for the dE layer.
//     int dE_stripMax_n;
//     ///The info about all the strips which fired and the energies collected by each of them for the E1 layer.
//     int E1_stripMax_n;
//     ///The info about all the strips which fired and the energies collected by each of them for the E2 layer.
//     int E2_stripMax_n;

    std::vector<float> eSum;
    std::vector<int> stripMax;

    ///The unique ID of the telescope to allow an easier selection of a specific portion of the detection system.
    /** This ID is composed of 4 digits following this pattern
     *  [1 for endcap / 2 for barrel] [1 for downstream / 2 for upstream] [sector numer]
     *   Examples:
     *   -> QQQ5-UA would belong to telescope 1200 (1 for endcap)(2 for upstream)(00 for sector #0)
     *   -> SX3-D10 would belong to telescope 2110 (2 for barrel)(1 for downstream)(10 for sector #10)
     *   Note that the same result could be achieved using combined conditions over the members "isBarrel", "isUpstream" and "sector"
     */
    ushort telescopeID;

    ///The sector the silicon event occured. For the barrel detector: goes from 0 (90 degrees compared to beamline) to 11 clockwise. For QQQ5: A == 0, B == 1, C == 2, D == 3.
    ushort sector;
    ///The event interaction position.
    std::vector<TVector3> pos;
    ///True if in the barrel, otherwise occured in the endcap.
    bool isBarrel;
    ///True if upstream, otherwise occured downstream.
    bool isUpstream;
    ///The time of the particle relative to the beginning of the event.
    unsigned int time;


    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( SiDataBase, 1 );
    /// \endcond
};

///Template support Class to make the sorting easier when storing the data relative to the Silicon Detectors in the root tree
template <class pairType>
class PNPair
{
private:

public:
    ///Default constructor.
    PNPair() {}

    ///Default destructor.
    virtual ~PNPair() {}

//     inline TArrayF* near() const;
//     inline TArrayF* far() const;

    ///Vector containing the info concerning the front side. The energies and strip numbers are using this convention.
    std::vector<pairType> p;

    ///Vector containing the info concerning the back side. The energies and strip numbers are using this convention.
    std::vector<pairType> n;

    ///Clear the two vector members of the PNPair class
    inline void Clear()
    {
        p.clear();
        n.clear();
    }

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( PNPair, 1 )
    /// \endcond
};

templateClassImp ( PNPair )

///Support Class to make the sorting easier when storing the data relative to the Silicon Detectors in the root tree
class SiDetEnStripInfo
{
private:

public:
    ///Default constructor.
    SiDetEnStripInfo() {}

    ///Default destructor.
    virtual ~SiDetEnStripInfo() {}

    ///The energies collected by the different strips which fired.
    PNPair<float> en;
    ///The number of the strips which fired.
    PNPair<int> strip;

    ///Clear the different class members of SiDetStripInfo
    inline void Clear()
    {
        en.Clear();
        strip.Clear();
    }

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( SiDetEnStripInfo, 1 );
    /// \endcond
};

ClassImp ( SiDetEnStripInfo )

class SiDataDetailed : public SiDataBase
{
private:

public:
    ///Default constructor.
    SiDataDetailed();

    ///Default destructor.
    virtual ~SiDataDetailed();

    float* SetMemberAddress ( std::string member, float** address );
    int* SetMemberAddress ( std::string member, int** address );
    std::vector<float>* SetMemberAddress ( std::string member, std::vector<float>** address );
    std::vector<int>* SetMemberAddress ( std::string member, std::vector<int>** address );

    void Clear();

//     ///The info about all the strips which fired and the energies collected by each of them for the dE layer.
//     std::vector<float> dE_e_p;
//     ///The info about all the strips which fired and the energies collected by each of them for the E1 layer.
//     std::vector<float> E1_e_p;
//     ///The info about all the strips which fired and the energies collected by each of them for the E2 layer.
//     std::vector<float> E2_e_p;
// 
//     ///The info about all the strips which fired and the energies collected by each of them for the dE layer.
//     std::vector<float> dE_e_n;
//     ///The info about all the strips which fired and the energies collected by each of them for the E1 layer.
//     std::vector<float> E1_e_n;
//     ///The info about all the strips which fired and the energies collected by each of them for the E2 layer.
//     std::vector<float> E2_e_n;
// 
//     ///The info about all the strips which fired and the energies collected by each of them for the dE layer.
//     std::vector<int> dE_strip_p;
//     ///The info about all the strips which fired and the energies collected by each of them for the E1 layer.
//     std::vector<int> E1_strip_p;
//     ///The info about all the strips which fired and the energies collected by each of them for the E2 layer.
//     std::vector<int> E2_strip_p;
// 
//     ///The info about all the strips which fired and the energies collected by each of them for the dE layer.
//     std::vector<int> dE_strip_n;
//     ///The info about all the strips which fired and the energies collected by each of them for the E1 layer.
//     std::vector<int> E1_strip_n;
//     ///The info about all the strips which fired and the energies collected by each of them for the E2 layer.
//     std::vector<int> E2_strip_n;
    
    SiDetEnStripInfo dE;
    SiDetEnStripInfo E1;
    SiDetEnStripInfo E2;

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( SiDataDetailed, 1 )
    /// \endcond
};


///Structure of the ion chamber data, including the scintillator.
struct IonData
{
    ///The energy deposited in the dE anodes.
    float dE;
    ///The residaul energy in resE anodes.
    float resE;
    ///The energy deposited in the E1 anodes.
    float E;
    ///Energy deposited in the siPMTs.
    float scintE[4];
    ///Timing of the siPMTs.
    float scintT[4];
    ///The time of the ion relative to the beginning of the event.
    unsigned int time;
};

struct GSRawData
{
    int type;
    int num;
    unsigned long int pre_rise_energy;
    unsigned long int post_rise_energy;
};

struct ORRUBARawData
{
    unsigned short channel;
    unsigned long int value;
    bool isDigital;
};

#endif







