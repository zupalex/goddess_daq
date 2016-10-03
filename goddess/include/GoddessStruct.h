#ifndef GODDESSSTRUCT_H
#define GODDESSSTRUCT_H

#include <string>
#include <vector>
#include <map>

#include "TArrayF.h"
#include "TMath.h"
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
    ///Timestamp of the gamma
    unsigned long long int time;
};

///Structure of silicon data from ORRUBA. This one is used when running geb_sort in default mode
class SiDataBase
{
private:

public:
    ///Default constructor.
    SiDataBase();

    ///Default destructor.
    virtual ~SiDataBase();

    virtual void Clear();

    ///Function used in GoddessData.cxx to set the address of a member of SiDataBase class by name.
    virtual float* SetMemberAddress ( std::string member, float** address );
    ///Function used in GoddessData.cxx to set the address of a member of SiDataBase class by name.
    virtual int* SetMemberAddress ( std::string member, int** address );
    ///Function used in GoddessData.cxx to set the address of a member of SiDataBase class by name.
    virtual std::vector<float>* SetMemberAddress ( std::string member, std::vector<float>** address );
    ///Function used in GoddessData.cxx to set the address of a member of SiDataBase class by name.
    virtual std::vector<int>* SetMemberAddress ( std::string member, std::vector<int>** address );
    ///Function used in GoddessData.cxx to set the address of a member of SiDataDetailed class by name.
    virtual std::vector<unsigned long long int>* SetMemberAddress ( std::string member, std::vector<unsigned long long int>** address );

    ///Method which can be used within the root Draw command to plot the sum of the energies collected in a specific layer and a specific side.
    virtual float ESumLayer ( unsigned short layer = 1, bool isNType = false ) const;

    ///Method which can be used within the root Draw command to plot the strip number which collected the max energy in a specific layer and specific side.
    virtual int StripMaxLayer ( unsigned short layer = 1, bool isNType = false ) const;

    virtual unsigned long long int TimestampMaxLayer ( unsigned short layer = 1, bool isNType = false ) const;

    ///Method which can be used within the root Draw command to plot the position of a hit in the dE layer
    virtual TVector3 PosdE() const;
    ///Method which can be used within the root Draw command to plot the position of a hit in the E1 layer
    virtual TVector3 PosE1() const;
    ///Method which can be used within the root Draw command to plot the position of a hit in the E2 layer
    virtual TVector3 PosE2() const;

    ///Method which can be used within the root Draw command to reconstruct the Q value.
    /**massBeam is the mass of the beam in a.m.u.
     * kBeam is the energy of the beam in MeV
     * massTarget is the mass of the target in a.m.u.
     * massEjec is the mass of the emitted light particle in a.m.u.
     */
    virtual float QValue ( float massBeam = 134., float kBeam = 1337.7, float massTarget = 2., float massEjec = 1. ) const;

    ///Method which can be used within the root Draw command to plot the angular distribution of the particles detected in a specific layer
    virtual float Angle ( unsigned short layer = 1 ) const;

    /// Vector containing the sum of the energies gathered in the different layers.
    /**They are not sorted so do not use with thr Draw command in root.
     * Use ESumLayer() method instead.
     */
    std::vector<float> eSum;

    /// Vector containing the number of the strips which gathered the max energy in the different layers.
    /**They are not sorted so do not use with thr Draw command in root.
     * Use ESumLayer() method instead.
     */
    std::vector<int> stripMax;

    std::vector<unsigned long long int> timestampMax;

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

    PNPair<unsigned long long int> timestamp;

    ///Clear the different class members of SiDetStripInfo
    inline void Clear()
    {
        en.Clear();
        strip.Clear();
        timestamp.Clear();
    }

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( SiDetEnStripInfo, 1 );
    /// \endcond
};

ClassImp ( SiDetEnStripInfo )

///Structure of silicon data from ORRUBA. This one is used when running geb_sort in high details level.
class SiDataDetailed : public SiDataBase
{
private:

public:
    ///Default constructor.
    SiDataDetailed();

    ///Default destructor.
    virtual ~SiDataDetailed();

    ///Function used in GoddessData.cxx to set the address of a member of SiDataDetailed class by name.
    float* SetMemberAddress ( std::string member, float** address );
    ///Function used in GoddessData.cxx to set the address of a member of SiDataDetailed class by name.
    int* SetMemberAddress ( std::string member, int** address );
    ///Function used in GoddessData.cxx to set the address of a member of SiDataDetailed class by name.
    std::vector<float>* SetMemberAddress ( std::string member, std::vector<float>** address );
    ///Function used in GoddessData.cxx to set the address of a member of SiDataDetailed class by name.
    std::vector<int>* SetMemberAddress ( std::string member, std::vector<int>** address );
    ///Function used in GoddessData.cxx to set the address of a member of SiDataDetailed class by name.
    std::vector<unsigned long long int>* SetMemberAddress ( std::string member, std::vector<unsigned long long int>** address );

    ///Clear the different class members of SiDataDetailed
    void Clear();

    ///Object containing the info on the dE layer
    SiDetEnStripInfo dE;
    ///Object containing the info on the E1 layer
    SiDetEnStripInfo E1;
    ///Object containing the info on the E2 layer
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

///Structure of the GammaSphere data for the raw tree
struct GSRawData
{
    ///GammaSphere detector type
    int type;
    ///GammaSphere detector number
    int num;
    ///The uncalibrated gamma energy can be obtained by doing post_rise_energy - pre_rise_energy
    unsigned long int pre_rise_energy;
    ///The uncalibrated gamma energy can be obtained by doing post_rise_energy - pre_rise_energy
    unsigned long int post_rise_energy;
};

///Support class for the ORRUBARawData structure
class ChValPair
{
private:

public:
    ///Default constructor.
    ChValPair();
    ///Default destructor.
    virtual ~ChValPair();

    ///Method which can be used within the root Draw command to plot the calibrated values from the raw tree
    float GetFastCalEn ( std::map<unsigned short, std::pair<float, float>>* calibParams = 0 ) const;

    unsigned short channel;
    unsigned long int value;

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( ChValPair,1 )
    /// \endcond
};

///Structure of the ORRUBA data for the raw tree
class ORRUBARawData
{
private:

public:
    ///Default constructor.
    ORRUBARawData();
    ///Default destructor.
    virtual ~ORRUBARawData();

    void Clear();

    ///Method which can be used within the root Draw command to plot the multiplicity for a range of ADC channels
    unsigned short GetMultRange ( unsigned short beg = 1, unsigned short end = 32 ) const;

    ///Vector containing the (channel, value) pairs
    std::vector<ChValPair> data;
    ///Vector of boolean giving the DAQ type
    std::vector<bool> isDigital;

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef ( ORRUBARawData, 1 )
    /// \endcond
};

#endif







