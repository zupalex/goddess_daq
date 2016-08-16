#ifndef GODDESSDATA_H
#define GODDESSDATA_H

#include "GoddessConfig.h"
#include "ORRUBA.h"
#include "orrubaDet.h"
#include "IonChamber.h"
#include "LiquidScint.h"
#include "GoddessStruct.h"
#include "ProcessManagers.h"


#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

#include "GTMerge.h"
#include "GEBSort.h"

#include <vector>
#include <array>
#include <map>
#include <utility>

class GoddessData
{
public:
    GoddessData(std::string configFilename);
    ~GoddessData();

    int Fill(GEB_EVENT* gebEvt, std::vector<DGSEVENT>* dgsEvts, std::vector<DFMAEVENT>* dgodEvts, std::vector<AGODEVENT>* agodEvt);

private:
    struct GammaData {
        float energy;
        unsigned long long timestamp;
    };

    GoddessConfig* config;

    ///The lowest timestamp int he current event.
    unsigned long long firstTimestamp;
    ///The pointer to the vector of gamma information.
    std::vector<GamData>* gamData;
    ///Pointer to the vector of silicon information.
    std::vector<SiDataBase>* siData;
    std::vector<SiDataDetailed>* siDataD;
    ///Pointer to the vector of ion chamber information.
    std::vector<IonData>* ionData;

    ///The pointer to the vector of gamma sorted but not calibrated information.
    std::vector<GamData>* gamData_snc;
    ///Pointer to the vector of silicon sorted but not calibrated information.
    std::vector<SiDataDetailed>* siData_snc;
    ///Pointer to the vector of ion chamber sorted but not calibrated information.
    std::vector<IonData>* ionData_snc;

    ///The pointer to the vector containing channel/value pairs to store in the raw tree for the digital part of ORRUBA
    ORRUBARawData* orrubaRaw;
    ///The pointer to the vector containing channel/value pairs to store in the raw tree for GAMMASPHERE (digital)
    std::vector<GSRawData>* gsRaw;

    void InitLiquidScintHists();
    void InitSuperX3Hists();
    void InitQQQ5Hists();
    void InitBB10Hists();
    void InitGammaHists();

    int FillTrees(std::vector<DGSEVENT>* dgsEvts/*, std::vector<DFMAEVENT> *dgodEvts, std::vector<AGODEVENT> *agodEvt*/);
    void FillHists(std::vector<DGSEVENT>* dgsEvts);

    ///Map of all fired detectors in an event keyed by position ID.
    std::map<std::string, Detector*> firedDets;
    ///Map of fired silicon detectors in an event keyed by position ID.
    std::map<std::string, orrubaDet*> siDets;
    IonChamber* ionChamber;
    std::map<std::string, LiquidScint*> liquidScints;

    TH2F* enRawA;
    TH2F* enRawD;
    TH2F* enCalA;
    TH2F* enCalD;
    TH1F* analogMult;
    TH1F* analogADCMult;
    TH1F* digitalMult;
    TH1F* detMult;
    TH1F* hDetPosMult;

    TH2F* endcapHitPatternUpstream;
    TH2F* endcapHitPatternDownstream;

    TH2F* sX3HitPattern;

    // BB10s
    std::map<std::string, TH2F*> BB10RawEn;
    std::map<std::string, TH2F*> BB10CalEn;


    // QQQ5s
    std::map<std::string, TH2F*> QQQenRawFront;
    std::map<std::string, TH2F*> QQQenCalFront;
    std::map<std::string, TH2F*> QQQenRawBack;
    std::map<std::string, TH2F*> QQQenCalBack;

    std::map<std::string, TH2F*> QQQHitPat;
    std::map<std::string, TH1F*> QQQFrontMult;
    std::map<std::string, TH1F*> QQQBackMult;

    // superX3s
    std::map<std::string, TH2F*> sX3stripEnCal;
    std::map<std::string, TH2F*> sX3stripEnRaw;
    std::map<std::string, TH2F*> sX3backEnCal;
    std::map<std::string, TH2F*> sX3backEnRaw;
    std::map<std::string, TH2F*> sx3StripTotEn;
    std::map<std::string, TH2F*> sX3near_far;
    std::map<std::string, TH2F*> sX3enCal_posRaw;
    std::map<std::string, TH2F*> sX3enCal_posCal;
    std::map<std::string, TH2F*> sX3HitPat;
    std::map<std::string, TH1F*> sX3frontMult;
    std::map<std::string, TH1F*> sX3backMult;
    std::map<std::string, TH2F**> sX3nearFar;
    std::map<std::string, TH2F**> sX3nearFarCal;
    std::map<std::string, TH2F**> sX3posRaw_enRaw;
    std::map<std::string, TH2F**> sX3posRaw_enCal;
    std::map<std::string, TH2F**> sX3posCal_enCal;


    // IC
    std::map<std::string, TH1F*> icE1;
    std::map<std::string, TH1F*> icE2;
    std::map<std::string, TH1F*> icE;
    std::map<std::string, TH1F**> scint_T;
    std::map<std::string, TH1F**> scint_E;

    // gammas
    TH1F* upstreamGam;
    TH1F* downstreamGam;

    // liquid scint
    std::map<std::string, TH2F*> LiquidScint_PSD_E;
    std::map<std::string, TH1F*> LiquidScint_enRaw;
    std::map<std::string, TH1F*> LiquidScint_psdRaw;
    std::map<std::string, TH1F*> LiquidScint_tacRaw;
    std::map<std::string, TH1F*> hLiqPSDNeuGate;
    std::map<std::string, TH1F*> hLiqRawEnNeuGate;
    std::map<std::string, TH1F*> hLiqTACNeuGate;
    std::map<std::string, TH1F*> hGamNeuGate;
    std::map<std::string, TH2F*> hGamVsLiqNeuGate;

    TTree* mike_tree;
    std::vector<float>* NeutID;
    std::vector<float>* NeutPSD;
    std::vector<float>* NeutEnergy;
    std::vector<float>* NeutTAC;
    bool Neutron;

    TTree* tree;
    TTree* sortedTree;
    TTree* rawTree;

    unsigned short DAQchannel;
    float DAQCh_Energy[400];

    float timeFlag;

    std::vector<float>* energy;
    float psd;
    float tac;

};

#endif
