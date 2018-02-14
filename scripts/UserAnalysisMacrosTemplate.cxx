// UserAnalysisTemplate version 170328 //

#include "GoddessAnalysis.h"
#include <sys/times.h>

using std::string;


//******************************************************************************************//
//******************************************************************************************//
// ------------------------- Initialization and Utilities --------------------------------- //
//******************************************************************************************//
//******************************************************************************************//

AnalysisParams godAnaParams = AnalysisParams();

// ----------------------- A few functions to make life easier ----------------------------- //

bool OrrubaIsDigital ( SiDataBase* siData_ )
{
    return ( siData_->isBarrel && ( siData_->sector >=1 && siData_->sector <= 6 ) ) ||
           ( !siData_->isBarrel && ( ( siData_->isUpstream && ( siData_->sector == 0 || siData_->sector == 1 ) ) || ( !siData_->isUpstream && siData_->sector == 2 ) ) );
}

bool OrrubaGoodSector ( SiDataBase* siData_ )
{
    return ( !siData_->isBarrel )
           || ( siData_->isBarrel && siData_->isUpstream && ( siData_->sector == 0 || siData_->sector == 11 ||  siData_->sector == 9 ) )
           || ( siData_->isBarrel && !siData_->isUpstream );
}

bool OrrubaGsGoodTs ( SiDataBase* siData_, GamData* gamData_ )
{
    bool isDigital = OrrubaIsDigital ( siData_ );

    unsigned long long int orrubaTs = siData_->TimestampMaxLayer ( 1, false );

    unsigned long long int gsTs = gamData_->time;

    return ( isDigital && gsTs - orrubaTs > 180 && gsTs - orrubaTs < 220 ) || ( !isDigital && gsTs - orrubaTs > 410 && gsTs - orrubaTs < 450 );
}

bool OrrubaEnergyThr ( SiDataBase* siData_ )
{
    bool isDigital = OrrubaIsDigital ( siData_ );

    return ( !siData_->isUpstream && ( ( isDigital && siData_->ESumLayer ( 1,false ) > 5000 ) || ( !isDigital && siData_->ESumLayer ( 1,false ) > 500 ) ) )
           || ( siData_->isUpstream && (
                    ( !siData_->isBarrel && siData_->ESumLayer ( 1,false ) > 1.2 )
                    || ( siData_->isBarrel && (
                             ( ( siData_->sector == 0 || siData_->sector == 11 ) && siData_->ESumLayer ( 1,false ) > 1.6 )
                             || ( siData_->sector == 9 && siData_->ESumLayer ( 1,true ) > 1.7 ) )
                       )
                )
              );
}

std::map<string, std::pair<TObject*, std::vector<GamData*>>> histsMap;
std::vector<string> specialHists;

// -------------------------------- A Class to make the processing of the data easier --------------------------------- //

class UserAnalysis
{
private:

protected:

public:
    UserAnalysis() {}
    ~ UserAnalysis() {}

    struct siStruct
    {
        int sector;
        int strip;

        bool isUpstream;
        bool isBarrel;

        int mult;

        float fEn;
        float bEn;
        float angle;

        float qval;
        float ex;

        unsigned long long int ts;

        bool isDigital;
        bool energyOK;
        bool sectorOK;

        TVector3 pos;
    };

    struct gamStruct
    {
        int type;

        int detNum;

        unsigned long long int gsTs;
        double gsEn;

        bool timestampOK;
    };

    void SetSiData ( SiDataBase* siData );
    void SetSiData ( SiDataDetailed* SiData );
    void SetGamData ( GamData* gamData );

    SiDataBase* siDataPtr;
    SiDataDetailed* siDataDPtr;
    GamData* gamDataPtr;
    GoddessGeomInfos* geomInfoPtr;
    GoddessReacInfos* reacInfoPtr;

    TVector3 qqq5Offsets[2][4];
    TVector3 sX3Offsets[2][12];

    TVector3 beamDir;

    TVector3 targetLadderDir;
    TVector3 targetOffset;

    siStruct si;
    gamStruct gam;

    std::pair<vector<double>, vector<double>> energyLossData;
};

void UserAnalysis::SetSiData ( SiDataBase* siData )
{
    siDataPtr = siData;

    si.sector = siData->sector;
    si.strip = siData->StripMaxLayer ( 1, false );
    si.isUpstream = siData->isUpstream;
    si.isBarrel = siData->isBarrel;
    si.ts = siData->TimestampMaxLayer ( 1, false );

    si.fEn = siData->ESumLayer ( 1, false );
    si.bEn = siData->ESumLayer ( 1, true );

    TVector3 localOff = si.isBarrel ? sX3Offsets[si.isUpstream][si.sector] : qqq5Offsets[si.isUpstream][si.sector];

    si.pos = siData->PosE1() + localOff - targetOffset;

    double effThickness = GetEffectiveThickness ( si.pos.Angle ( targetLadderDir ) - TMath::PiOver2(), reacInfoPtr->targetThickness );

    double estELoss = ComputeEnergyLoss ( energyLossData.first, energyLossData.second, si.fEn/reacInfoPtr->ejecA, reacInfoPtr->ejecA, 0, effThickness, 0.01, "Interpolation" );
    si.fEn += estELoss;

//     si.angle = siData->Angle ( 1, geomInfoPtr->targetOffset.X(), geomInfoPtr->targetOffset.Y(), geomInfoPtr->targetOffset.Z() );

    si.angle = si.pos.Angle ( beamDir );

    si.qval = SiDataBase::QValue ( reacInfoPtr, si.fEn, si.angle );
//     si.qval = siData->QValue ( gA->beamMass, gA->beamEk, gA->targetMass, gA->ejecMass, gA->recoilMass );
    si.ex = reacInfoPtr->qValGsGs - si.qval;

    si.angle *= TMath::RadToDeg();

    si.mult = siData->MultLayer ( 1, false );

    si.isDigital = OrrubaIsDigital ( siData );
    si.energyOK = OrrubaEnergyThr ( siData );
    si.sectorOK = OrrubaGoodSector ( siData );
}

void UserAnalysis::SetSiData ( SiDataDetailed* siData )
{
    siDataDPtr = siData;

    si.sector = siData->sector;
    si.strip = siData->StripMaxLayer ( 1, false );
    si.isUpstream = siData->isUpstream;
    si.isBarrel = siData->isBarrel;
    si.ts = siData->TimestampMaxLayer ( 1, false );

    si.fEn = siData->ESumLayer ( 1, false );
    si.bEn = siData->ESumLayer ( 1, true );

    TVector3 localOff = si.isBarrel ? sX3Offsets[si.isUpstream][si.sector] : qqq5Offsets[si.isUpstream][si.sector];

    si.pos = siData->PosE1() + localOff - targetOffset;

    double effThickness = GetEffectiveThickness ( si.pos.Angle ( targetLadderDir ) - TMath::PiOver2(), reacInfoPtr->targetThickness );

    double estELoss = ComputeEnergyLoss ( energyLossData.first, energyLossData.second, si.fEn/reacInfoPtr->ejecA, reacInfoPtr->ejecA, 0, effThickness, 0.01, "Interpolation" );
    si.fEn += estELoss;

//     si.angle = siData->Angle ( 1, geomInfoPtr->targetOffset.X(), geomInfoPtr->targetOffset.Y(), geomInfoPtr->targetOffset.Z() );

    si.angle = si.pos.Angle ( beamDir );

    si.qval = SiDataBase::QValue ( reacInfoPtr, si.fEn, si.angle );
//     si.qval = siData->QValue ( gA->beamMass, gA->beamEk, gA->targetMass, gA->ejecMass, gA->recoilMass );

    si.angle *= TMath::RadToDeg();

    si.ex = reacInfoPtr->qValGsGs - si.qval;
    si.mult = siData->MultLayer ( 1, false );

    si.isDigital = OrrubaIsDigital ( siData );
    si.energyOK = OrrubaEnergyThr ( siData );
    si.sectorOK = OrrubaGoodSector ( siData );
}

void UserAnalysis::SetGamData ( GamData* gamData )
{
    gamDataPtr = gamData;

    gam.type = gamData->type;

    gam.detNum = gamData->num;

    gam.gsEn = gamData->en/3.;
    gam.gsTs = gamData->time;
    gam.timestampOK = OrrubaGsGoodTs ( ( siDataPtr != nullptr ? siDataPtr : siDataDPtr ), gamData );
}

//******************************************************************************************//
//******************************************************************************************//
// ------------------------- Write your macros here --------------------------------------- //
//******************************************************************************************//
//******************************************************************************************//

// ---------------------------------------------------- //
// ------------- Particles Energy vs. Angle ----------- //
// ---------------------------------------------------- //

const int maxMult = 4;

void InitEvsAHist ( unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY, TVector3 targetPos = TVector3 ( 0, 0, 0 ), GoddessGeomInfos* geomInfo = nullptr )
{
    for ( int up = 0; up < 2; up++ )
    {
        for ( int i = 0; i < 2; i++ )
        {
            for ( int j = 0; j < 12; j++ )
            {
                for ( int k = 1; k < maxMult; k++ )
                {
                    MakeNewHist ( Form ( "En_%s_vs_Angle_SX3%s%d_mult%d", i == 0 ? "front" : "back", up == 0 ? "D" : "U", j, k ),
                                  Form ( "Energy %s vs Angle SX3 %s%d mult %d", i == 0 ? "front" : "back", up == 0 ? "D" : "U", j, k ),
                                  nBinsX, minX, maxX, nBinsY, minY, maxY );
                }
            }

            for ( int j = 0; j < 4; j++ )
            {
                std::vector<double> binsEdgesList = GetBinsEdges ( geomInfo, up, false, j, 1 );

                for ( int k = 1; k < maxMult; k++ )
                {
//                     MakeNewHist ( Form ( "En_%s_vs_Strip_QQQ5%s%d_mult%d", i == 0 ? "front" : "back", up == 0 ? "D" : "U", j, k ),
//                                   Form ( "Energy %s vs Strip# QQQ5 %s%d mult %d", i == 0 ? "front" : "back", up == 0 ? "D" : "U", j, k ), 34, -1, 33, nBinsY, minY, maxY );

                    MakeNewHist ( Form ( "En_%s_vs_Angle_QQQ5%s%d_mult%d", i == 0 ? "front" : "back", up == 0 ? "D" : "U", j, k ),
                                  Form ( "Energy %s vs Angle QQQ5 %s%d mult %d", i == 0 ? "front" : "back", up == 0 ? "D" : "U", j, k ),
                                  binsEdgesList.size()-1, &binsEdgesList[0], nBinsY, minY, maxY );
                }
            }
        }
    }

    for ( int k = 1; k < maxMult; k++ )
    {
        MakeNewHist ( Form ( "En_front_vs_Strip_mult%d", k ), Form ( "Energy front vs Strip# mult %d", k ), 500, 0, 500, nBinsY, minY, maxY, false );
        MakeNewHist ( Form ( "En_back_vs_Strip_mult%d", k ), Form ( "Energy back vs Strip# mult %d", k ), 500, 0, 500, nBinsY, minY, maxY, false );
    }
}

bool FillEvsAHist ( UserAnalysis* analysis_ )
{
    bool filled = false;

    if ( analysis_->si.mult >= maxMult ) return false;

    string detTypeStr = Form ( "%s", analysis_->si.isBarrel ? "SX3" : "QQQ5" );
    string sectStr = Form ( "%s%d", analysis_->si.isUpstream == 0 ? "D" : "U", analysis_->si.sector );

    string eVSaSectFrontStr = Form ( "En_front_vs_Angle_%s%s_mult%d", detTypeStr.c_str(), sectStr.c_str(), analysis_->si.mult );
    string eVSaSectBackStr = Form ( "En_back_vs_Angle_%s%s_mult%d", detTypeStr.c_str(), sectStr.c_str(), analysis_->si.mult );

    TH2F* hEvsASectFront = ( TH2F* ) histsMap[eVSaSectFrontStr].first;
    TH2F* hEvsASectBack = ( TH2F* ) histsMap[eVSaSectBackStr].first;

    if ( analysis_->si.mult < maxMult )
    {
        if ( analysis_->si.angle != 0 )
        {
            int globStripID = ToStripID ( analysis_->si.isUpstream, analysis_->si.isBarrel, true, analysis_->si.sector, analysis_->si.strip );

            if ( analysis_->si.fEn > 0 )
            {
                hEvsASectFront->Fill ( analysis_->si.angle, analysis_->si.fEn );

                TH2F* hEvsStrip = ( TH2F* ) histsMap[Form ( "En_front_vs_Strip_mult%d", analysis_->si.mult )].first;
                hEvsStrip->Fill ( globStripID, analysis_->si.fEn );

                filled = true;
            }

            if ( analysis_->si.bEn > 0 )
            {
                hEvsASectBack->Fill ( analysis_->si.angle, analysis_->si.bEn );

                TH2F* hEvsStrip = ( TH2F* ) histsMap[Form ( "En_back_vs_Strip_mult%d", analysis_->si.mult )].first;
                hEvsStrip->Fill ( globStripID, analysis_->si.bEn );
            }

            filled = true;
        }
    }

    return filled;
}

// --------------------------------------------------------------- //
// ------------- Q-Value and Excitation Energy spectra ----------- //
// --------------------------------------------------------------- //

void InitQvalHist ( unsigned int nBinsX, int minX, int maxX )
{
    MakeNewHist ( "Qval_vs_Strips", "Q-value vs. Strip Number (Upstream: [0-143]=QQQ5s , [144-239]=SX3s / Downstream: Upstream+240)", 500, 0, 500, nBinsX, minX, maxX, false );
    MakeNewHist ( "Ex_vs_Strips", "Excitation Energy vs. Strip Number (Upstream: [0-143]=QQQ5s , [144-239]=SX3s / Downstream: Upstream+240)", 500, 0, 500, nBinsX, minX, maxX, false );
}

bool FillQvalHist ( UserAnalysis* analysis_ )
{
    bool filled = false;

    int globStripID = ToStripID ( analysis_->si.isUpstream, analysis_->si.isBarrel, true, analysis_->si.sector, analysis_->si.strip );

    if ( analysis_->si.isUpstream && analysis_->si.angle != 0 && analysis_->si.fEn > 0 )
    {
        ( ( TH2F* ) histsMap["Qval_vs_Strips"].first )->Fill ( globStripID, analysis_->si.qval );
        ( ( TH2F* ) histsMap["Ex_vs_Strips"].first )->Fill ( globStripID, analysis_->si.ex );

        filled = true;
    }

    return filled;
}

// ---------------------------------------------------------------------------- //
// ------------- GammaSphere vs. ORRUBA (excitation energy) spectra ----------- //
// ---------------------------------------------------------------------------- //

void InitGsVsExHists ( unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY )
{
    MakeNewHist ( "GsVsEx_tot", "Gamma Energy vs. Excitation Energy tot", nBinsX, minX, maxX, nBinsY, minY, maxY, true );
    MakeNewHist ( "GsVsEx_SX3U_tot", "Gamma Energy vs. Excitation Energy SX3s Upstream", nBinsX, minX, maxX, nBinsY, minY, maxY, true );
    MakeNewHist ( "GsVsEx_QQQ5U_tot", "Gamma Energy vs. Excitation Energy QQQ5s Upstream", nBinsX, minX, maxX, nBinsY, minY, maxY, true );

    for ( int i = 0; i < 12; i++ )
    {
        MakeNewHist ( Form ( "GsVsEx_SX3U%d", i ), Form ( "Gamma Energy vs. Excitation Energy SX3 U%d", i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
    }

    for ( int i = 0; i < 4; i++ )
    {
        MakeNewHist ( Form ( "GsVsEx_QQQ5U%d", i ), Form ( "Gamma Energy vs. Excitation Energy QQQ5 U%d", i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
    }

    MakeNewHist ( "GsBGOVetoVsEx_tot", "Gamma Energy BGO Veto vs. Excitation Energy tot", nBinsX, minX, maxX, nBinsY, minY, maxY, true );
    MakeNewHist ( "GsBGOVetoVsEx_SX3U_tot", "Gamma Energy BGO Veto vs. Excitation Energy SX3s Upstream", nBinsX, minX, maxX, nBinsY, minY, maxY, true );
    MakeNewHist ( "GsBGOVetoVsEx_QQQ5U_tot", "Gamma Energy BGO Veto vs. Excitation Energy QQQ5s Upstream", nBinsX, minX, maxX, nBinsY, minY, maxY, true );
    MakeNewHist ( "GsBGOVetoVsEx_QQQ5U_goodstrips", "Gamma Energy vs. Excitation Energy QQQ5s Upstream", nBinsX, minX, maxX, nBinsY, minY, maxY, true );

    for ( int i = 0; i < 12; i++ )
    {
        MakeNewHist ( Form ( "GsBGOVetoVsEx_SX3U%d", i ), Form ( "Gamma Energy BGO Veto vs. Excitation Energy SX3 U%d", i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
    }

    for ( int i = 0; i < 4; i++ )
    {
        MakeNewHist ( Form ( "GsBGOVetoVsEx_QQQ5U%d", i ), Form ( "Gamma Energy BGO Veto vs. Excitation Energy QQQ5 U%d", i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
    }
}

bool FillGsVsExHist ( UserAnalysis* analysis_, bool vetoBGO )
{
    bool filled = false;

    bool doFill;
//     doFill = analysis_->gam.timestampOK && analysis_->si.sectorOK && analysis_->si.energyOK;
    doFill = analysis_->gam.timestampOK && analysis_->si.sectorOK && analysis_->si.fEn > 0;

    string histsBaseName;

    if ( vetoBGO ) histsBaseName = "GsBGOVetoVsEx_";
    else histsBaseName = "GsVsEx_";
    TH2F* gsVsExtot = ( TH2F* ) histsMap[ ( string ) ( histsBaseName + "tot" )].first;

    if ( analysis_->si.isBarrel ) histsBaseName += "SX3";
    else histsBaseName += "QQQ5";

    if ( analysis_->si.isUpstream ) histsBaseName += "U";
    else histsBaseName += "D";
    TH2F* gsVsExU = ( TH2F* ) histsMap[ ( string ) ( histsBaseName + "_tot" )].first;
    TH2F* gsVsExGoodStrips = ( TH2F* ) histsMap[ ( string ) ( histsBaseName + "_goodstrips" )].first;

    string histsSectorName = Form ( "%s%d", histsBaseName.c_str(), analysis_->si.sector );
    TH2F* gsVsExSector = ( TH2F* ) histsMap[histsSectorName].first;

    if ( doFill && analysis_->si.isUpstream )
    {
        gsVsExSector->Fill ( analysis_->si.ex, analysis_->gam.gsEn );

        if ( !CheckHistState ( gsVsExU, analysis_->gamDataPtr ) )
        {
            gsVsExU->Fill ( analysis_->si.ex, analysis_->gam.gsEn );
            AddToHistState ( gsVsExU, analysis_->gamDataPtr );
        }

        if ( !CheckHistState ( gsVsExtot, analysis_->gamDataPtr ) )
        {
            gsVsExtot->Fill ( analysis_->si.ex, analysis_->gam.gsEn );
            AddToHistState ( gsVsExtot, analysis_->gamDataPtr );
        }

        if ( vetoBGO && !analysis_->si.isBarrel && analysis_->si.isUpstream &&
//                 ( ( analysis_->si.strip >= 0 && analysis_->si.strip <= 43 ) || ( analysis_->si.strip >= 65 && analysis_->si.strip <= 78 ) || ( analysis_->si.strip >= 114 && analysis_->si.strip <= 128 ) ) &&
//                 ( ( analysis_->si.sector == 0 && analysis_->si.strip >= 0 && analysis_->si.strip <= 31 ) || ( analysis_->si.sector == 1 && analysis_->si.strip >= 0 && analysis_->si.strip <= 16 ) ) &&
//                 ( analysis_->si.sector == 2 && analysis_->si.strip >= 0 && analysis_->si.strip <= 16 ) &&
                ( analysis_->si.sector == 3 && analysis_->si.strip >= 15 && analysis_->si.strip <= 31 ) &&
                !CheckHistState ( gsVsExGoodStrips, analysis_->gamDataPtr ) )
        {
            gsVsExGoodStrips->Fill ( analysis_->si.ex, analysis_->gam.gsEn );
            AddToHistState ( gsVsExGoodStrips, analysis_->gamDataPtr );
        }

//         int globStripID = ToStripID ( analysis_->si.isUpstream, analysis_->si.isBarrel, true, analysis_->si.sector, analysis_->si.strip );
//
//         string histKey = Form ( "Gs_%svs_Ex_nocheck_vs_Strips", ( vetoBGO ? "BGOVeto_" : "" ) );
//         string histKeyNoCheck = Form ( "Gs_%svs_Ex_vs_Strips", ( vetoBGO ? "BGOVeto_" : "" ) );
//
//         TH3F* hGSvsEx = ( TH3F* ) histsMap[histKeyNoCheck].first;
//
//         hGSvsEx->Fill ( analysis_->si.ex, analysis_->gam.gsEn, globStripID );
//
//         hGSvsEx = ( TH3F* ) histsMap[histKey].first;
//
//         if ( !CheckHistState ( hGSvsEx, analysis_->gamDataPtr ) )
//         {
//             hGSvsEx->Fill ( analysis_->si.ex, analysis_->gam.gsEn, globStripID );
//             AddToHistState ( hGSvsEx, analysis_->gamDataPtr );
//         }

        filled = true;
    }

    return filled;
}

// ------------------------------------------------------------------------ //
// ------------- Time difference between GammaSphere and ORRUBA ----------- //
// ------------------------------------------------------------------------ //

void InitdTGsORRUBAHists ( unsigned int nBinsX = 1000, int minX = -500, int maxX = 500 )
{
    MakeNewHist ( "dT_GS_vs_Strips", "dT GS vs. Strip Number (Upstream: [0-143]=QQQ5s , [144-239]=SX3s / Downstream: Upstream+240)", 500, 0, 500, nBinsX, minX, maxX, false );

    MakeNewHist ( "dT_GS_BGOVeto_vs_Strips", "dT GS BGO Veto vs. Strip Number (Upstream: [0-143]=QQQ5s , [144-239]=SX3s / Downstream: Upstream+240)", 500, 0, 500, nBinsX, minX, maxX, false );
}

bool FilldTGammaORRUBA ( UserAnalysis* analysis_, bool vetoBGO = false )
{
    bool filled = false;

    if ( analysis_->gam.type == 2 ) return false;

    int globStripID = ToStripID ( analysis_->si.isUpstream, analysis_->si.isBarrel, true, analysis_->si.sector, analysis_->si.strip );

    string histKey = Form ( "dT_GS_%svs_Strips", ( vetoBGO ? "BGOVeto_" : "" ) );

    ( ( TH2F* ) histsMap[histKey].first )->Fill ( globStripID, analysis_->gam.gsTs - analysis_->si.ts );

    filled = true;

    return filled;
}

// ------------------------------------------------------------------------- //
// ------------------------ GammaSphere Spectra ---------------------------- //
// ------------------------------------------------------------------------- //

void InitGsGateORRUBAHists ( unsigned int nBinsX = 5000, int minX = 0, int maxX = 5000 )
{
    MakeNewHist ( "Gs_Gates_ORRUBA_vs_Strips", "GammaSphere Gates ORRUBA vs. Strip Number (Upstream: [0-143]=QQQ5s , [144-239]=SX3s / Downstream: Upstream+240)",
                  500, 0, 500, nBinsX, minX, maxX, true );

    MakeNewHist ( "Gs_BGOVeto_Gates_ORRUBA_vs_Strips", "GammaSphere BGO Veto Gates ORRUBA vs. Strip Number (Upstream: [0-143]=QQQ5s , [144-239]=SX3s / Downstream: Upstream+240)",
                  500, 0, 500, nBinsX, minX, maxX, true );

    MakeNewHist ( "Gs_Gates_ORRUBA_nocheck_vs_Strips", "GammaSphere Gates ORRUBA No Check vs. Strip Number (Upstream: [0-143]=QQQ5s , [144-239]=SX3s / Downstream: Upstream+240)",
                  500, 0, 500, nBinsX, minX, maxX, false );

    MakeNewHist ( "Gs_BGOVeto_Gates_ORRUBA_nocheck_vs_Strips", "GammaSphere BGO Veto Gates ORRUBA No Check vs. Strip Number (Upstream: [0-143]=QQQ5s , [144-239]=SX3s / Downstream: Upstream+240)",
                  500, 0, 500, nBinsX, minX, maxX, false );

    MakeNewHist ( "Gs_BGOVeto_vs_DetNum_Gate_ORRUBA", "Gammasphere BGO Veto vs. Crystal Number gated by ORRUBA", 115, 0, 115, nBinsX, minX, maxX, true );
    MakeNewHist ( "Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_aboveSn", "Gammasphere BGO Veto vs. Crystal Number gated by ORRUBA above Sn", 115, 0, 115, nBinsX, minX, maxX, true );
    MakeNewHist ( "Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_aboveSn_restrictSect", "Gammasphere BGO Veto vs. Crystal Number gated by ORRUBA above Sn QQQ5U or SX3 U0 U11", 115, 0, 115, nBinsX, minX, maxX, true );
    MakeNewHist ( "Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_badgate_aboveSn", "Gammasphere BGO Veto vs. Crystal Number gated by ORRUBA bad time gate above Sn", 115, 0, 115, nBinsX, minX, maxX, true );
    MakeNewHist ( "Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_badgate_aboveSn_restrictSect", "Gammasphere BGO Veto vs. Crystal Number gated by ORRUBA bad time gate above Sn QQQ5U or SX3 U0 U11",
                  115, 0, 115, nBinsX, minX, maxX, true );
}

bool FillGsGateORRUBA ( UserAnalysis* analysis_, bool vetoBGO = false )
{
    bool filled = false;

    bool doFill;
    doFill = analysis_->si.sectorOK && analysis_->gam.timestampOK && analysis_->si.energyOK;
//     doFill = analysis_->si.sectorOK && analysis_->gam.timestampOK && analysis_->si.fEn > 0;

    string histsBaseName;

    if ( doFill )
    {
        int globStripID = ToStripID ( analysis_->si.isUpstream, analysis_->si.isBarrel, true, analysis_->si.sector, analysis_->si.strip );

        if ( vetoBGO && !CheckHistState ( histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA"].first, analysis_->gamDataPtr ) )
        {
            ( ( TH2F* ) histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA"].first )->Fill ( analysis_->gam.detNum, analysis_->gam.gsEn );
            AddToHistState ( histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA"].first, analysis_->gamDataPtr );
        }

        if ( vetoBGO && !CheckHistState ( histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_aboveSn"].first, analysis_->gamDataPtr ) )
        {
            if ( analysis_->si.ex > 8.0 )
            {
                ( ( TH2F* ) histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_aboveSn"].first )->Fill ( analysis_->gam.detNum, analysis_->gam.gsEn );
                AddToHistState ( histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_aboveSn"].first, analysis_->gamDataPtr );
            }
        }

        if ( vetoBGO && !CheckHistState ( histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_aboveSn_restrictSect"].first, analysis_->gamDataPtr ) )
        {
            if ( analysis_->si.ex > 8.0 && analysis_->si.isUpstream && ( !analysis_->si.isBarrel || ( analysis_->si.sector == 0 || analysis_->si.sector == 11 ) ) )
            {
                ( ( TH2F* ) histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_aboveSn_restrictSect"].first )->Fill ( analysis_->gam.detNum, analysis_->gam.gsEn );
                AddToHistState ( histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_aboveSn_restrictSect"].first, analysis_->gamDataPtr );
            }
        }

        string histKey = Form ( "Gs_%sGates_ORRUBA_vs_Strips", ( vetoBGO ? "BGOVeto_" : "" ) );
        string histKeyNoCheck = Form ( "Gs_%sGates_ORRUBA_nocheck_vs_Strips", ( vetoBGO ? "BGOVeto_" : "" ) );

        TH2F* hGS = ( TH2F* ) histsMap[histKeyNoCheck].first;

        hGS->Fill ( globStripID, analysis_->gam.gsEn );

        hGS = ( TH2F* ) histsMap[histKey].first;

        if ( !CheckHistState ( hGS, analysis_->gamDataPtr ) )
        {
            hGS->Fill ( globStripID, analysis_->gam.gsEn );
            if ( analysis_->si.isUpstream && !analysis_->si.isBarrel ) AddToHistState ( hGS, analysis_->gamDataPtr );
        }

        filled = true;
    }

    if ( analysis_->si.sectorOK && !analysis_->gam.timestampOK && analysis_->si.fEn > 0 )
    {
        if ( vetoBGO && !CheckHistState ( histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_badgate_aboveSn"].first, analysis_->gamDataPtr ) )
        {
            if ( analysis_->si.ex > 8.0 )
            {
                ( ( TH2F* ) histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_badgate_aboveSn"].first )->Fill ( analysis_->gam.detNum, analysis_->gam.gsEn );
                AddToHistState ( histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_badgate_aboveSn"].first, analysis_->gamDataPtr );
            }
        }

        if ( vetoBGO && !CheckHistState ( histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_badgate_aboveSn_restrictSect"].first, analysis_->gamDataPtr ) )
        {
            if ( analysis_->si.ex > 8.0 && analysis_->si.isUpstream && ( !analysis_->si.isBarrel || ( analysis_->si.sector == 0 || analysis_->si.sector == 11 ) ) )
            {
                ( ( TH2F* ) histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_badgate_aboveSn_restrictSect"].first )->Fill ( analysis_->gam.detNum, analysis_->gam.gsEn );
                AddToHistState ( histsMap["Gs_BGOVeto_vs_DetNum_Gate_ORRUBA_badgate_aboveSn_restrictSect"].first, analysis_->gamDataPtr );
            }
        }
    }

    return filled;
}

// ------------------------------------------------------------------------- //
// ---------------------- Gamma Gamma Coincidences ------------------------- //
// ------------------------------------------------------------------------- //

void InitGsvsGsBGOVeto ( unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY )
{
    for ( int i = 2; i < 5; i++ )
    {
        for ( int j = 0; j < 12; j++ )
        {
            MakeNewHist ( Form ( "Gs_vs_Gs_BGOVeto_Gate_SX3U%d_gsmult_%d", j, i ), Form ( "Gamma-Gamma Coincidences BGO Veto Gate SX3 U%d (gamma mult = %d)", j, i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
        }

        for ( int j = 0; j < 12; j++ )
        {
            MakeNewHist ( Form ( "Gs_vs_Gs_BGOVeto_Gate_SX3D%d_gsmult_%d", j, i ), Form ( "Gamma-Gamma Coincidences BGO Veto Gate SX3 D%d (gamma mult = %d)", j, i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
        }

        for ( int j = 0; j < 4; j++ )
        {
            MakeNewHist ( Form ( "Gs_vs_Gs_BGOVeto_Gate_QQQ5U%d_gsmult_%d", j, i ), Form ( "Gamma-Gamma Coincidences BGO Veto Gate QQQ5 U%d (gamma mult = %d)", j, i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
        }

        for ( int j = 0; j < 4; j++ )
        {
            MakeNewHist ( Form ( "Gs_vs_Gs_BGOVeto_Gate_QQQ5D%d_gsmult_%d", j, i ), Form ( "Gamma-Gamma Coincidences BGO Veto Gate QQQ5 D%d (gamma mult = %d)", j, i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
        }
    }
}

bool FillGsVsGsBGOVeto ( UserAnalysis* analysis_, vector<GamData>* vectGam_ )
{
    if ( vectGam_->size() < 2 ) return false;

    std::list<double> gamEns;

    for ( unsigned int i = 0; i < vectGam_->size(); i++ )
    {
        if ( OrrubaGsGoodTs ( analysis_->siDataPtr, &vectGam_->at ( i ) ) ) gamEns.push_back ( vectGam_->at ( i ).en/3. );
    }

    if ( gamEns.size() < 2 || gamEns.size() > 4 ) return false;

    gamEns.sort();
    gamEns.reverse();

    bool filled = false;

    string mapKey = Form ( "gamma mult %lu", gamEns.size() );

    string histName;

    histName = "Gs_vs_Gs_BGOVeto_Gate_";

    histName += Form ( "%s", ( analysis_->si.isBarrel ? "SX3" : "QQQ5" ) );
    histName += Form ( "%s", ( analysis_->si.isUpstream ? "U" : "D" ) );
    histName += Form ( "%d_gsmult_%lu", analysis_->si.sector, gamEns.size() );

    TH2F* hGsGs = ( TH2F* ) histsMap[histName].first;

    if ( analysis_->si.sectorOK && analysis_->si.fEn > 0.0 )
    {
        auto itr = gamEns.begin();

        double gam1, gam2;

        gam1 = *itr++;
        gam2 = *itr;

        hGsGs->Fill ( gam2, gam1 );

        filled = true;
    }

    return filled;
}

// -------------------------------------------------------------------------- //
// --------- Put here the functions you want to be processed ---------------- //
// -------------------------------------------------------------------------- //

void FillUserHists ( long long int maxEvents = 0 )
{
    TChain* uChain = gA->userChain;

    if ( uChain == nullptr )
    {
        cerr << "The user TChain has not been initialized... Aborting...\n";

        return;
    }

    cout << "the chain contains " << uChain->GetEntries() << " entries...\n";

    //*******************************************************************************************//
    //___________________________ Linking tree to local variables _______________________________//
    //*******************************************************************************************//

    std::vector<SiDataBase>* vectSiData = new std::vector<SiDataBase>;
    std::vector<SiDataDetailed>* vectSiDataD = new vector<SiDataDetailed>;

    std::vector<GamData>* vectGamData = new std::vector<GamData>();

    if ( uChain->SetBranchAddress ( "si", &vectSiData ) == -1 )
    {
        vectSiData = nullptr;
        cout << "Trying vector<SiDataDetailed>* for the silicon branch instead...\n";

        if ( uChain->SetBranchAddress ( "si", &vectSiDataD ) == -1 )
        {
            cerr << "si branch is not recognized!\n";
            return;
        }

        cout << "vector<SiDataDetailed>* matches the silicon branch. Processing...\n";
    }

    uChain->SetBranchAddress ( "gam", &vectGamData );

//     PrintHistsMapContent();

    //*******************************************************************************************//
    //______________________________ Histograms Initialization __________________________________//
    //*******************************************************************************************//

    TVector3 targetPosition ( 0, 0, 0 );
    GoddessGeomInfos* geomInfo = ( GoddessGeomInfos* ) uChain->GetFile()->FindObjectAny ( "GoddessGeom" );
    GoddessReacInfos* reacInfo = ( GoddessReacInfos* ) uChain->GetFile()->FindObjectAny ( "GoddessReac" );

    cout << "Initializing Time difference GammaSphere - ORRUBA...\n";
    InitdTGsORRUBAHists();

    cout << "Initializing GammaSphere gated by ORRUBA...\n";
    InitGsGateORRUBAHists();

    cout << "Initializing ORRUBA Energy vs. Angle...\n";
    InitEvsAHist ( 1800, 0, 180, 1500, 0, 15, targetPosition, geomInfo );

    cout << "Initializing Q-Value...\n";
    InitQvalHist ( 800, -20, 20 );

    cout << "Initializing GammaSphere vs. ORRUBA...\n";
    InitGsVsExHists ( 800, -20, 20, 5000, 0, 5000 );

    cout << "Initializing Gamma Gamma Coincidences...\n";
    InitGsvsGsBGOVeto ( 5000, 0, 5000, 5000, 0, 5000 );

    cout << "Done initializing all histograms...\n";

    int eventNbr = 0;

    if ( maxEvents <= 0 || maxEvents > uChain->GetEntries() ) maxEvents = uChain->GetEntries();

    //*******************************************************************************************//
    //___________________________________ Processing Loop _______________________________________//
    //*******************************************************************************************//

    UserAnalysis* analysis = new UserAnalysis();

    analysis->reacInfoPtr = ( GoddessReacInfos* ) reacInfo->Clone();
    analysis->geomInfoPtr = ( GoddessGeomInfos* ) geomInfo->Clone();

    analysis->beamDir = TVector3 ( 0, 0, 1 );

    TVector3 targetLadderDir ( 0, 0, 1 );
    targetLadderDir.SetTheta ( geomInfo->targetLadderAngle*TMath::DegToRad() );
    targetLadderDir.SetPhi ( TMath::PiOver2() );

    analysis->targetLadderDir = targetLadderDir;

    analysis->targetOffset = godAnaParams.targetOffset;
    TVector3 sX3Offset = godAnaParams.sx3sOffset;
    TVector3 qqq5Offset = godAnaParams.qqq5sOffset;

//     analysis->targetOffset = TVector3 ( 0, 0, 0 );
//     TVector3 sX3Offset = TVector3 ( 0, 0, 0 );
//     TVector3 qqq5Offset = TVector3 ( 0, 0, 0 );

    for ( int i = 0; i < 2; i++ )
    {
        for ( int j = 0; j < 4; j++ )
        {
            TVector3 detPos = GetDetPos ( analysis->geomInfoPtr, i, false, j, 1 );

            double sectPhi = detPos.Phi();
            analysis->qqq5Offsets[i][j] = qqq5Offset;

            analysis->qqq5Offsets[i][j].SetPhi ( sectPhi );

            if ( i==1 ) analysis->qqq5Offsets[i][j].SetZ ( -analysis->qqq5Offsets[i][j].Z() );
        }

        for ( int j = 0; j < 12; j++ )
        {
            TVector3 detPos = GetDetPos ( analysis->geomInfoPtr, i, true, j, 1 );

            double sectPhi = detPos.Phi();
            analysis->sX3Offsets[i][j] = sX3Offset;

            analysis->sX3Offsets[i][j].SetPhi ( sectPhi );

            if ( i==1 ) analysis->sX3Offsets[i][j].SetZ ( -analysis->sX3Offsets[i][j].Z() );
        }
    }

    std::ifstream mass_input ( pathToGDAQ + "/share/mass_db.dat", std::ios_base::in );

    if ( !mass_input.is_open() )
    {
        std::cerr << "Failed to open the mass database. Energy loss has not been computed...\n";

        return;
    }

    string projStr = "";

    GetAtomicFormula ( mass_input, reacInfo->ejecA, reacInfo->ejecZ, projStr );

    char* tryFindStr = new char[512];

    sprintf ( tryFindStr, "%s_in_%s_%dmg_cm3*range*_vs_energy*", projStr.c_str(), reacInfo->targetType.c_str(), ( int ) reacInfo->targetDensity );

    string tryPath = "";
    vector<string> tryFindTable = DecodeItemsToTreat ( ( string ) tryFindStr, "system", false );

    if ( tryFindTable.size() != 1 )
    {
        std::cout << "Requested to compute the energy loss for " << projStr << " but no stopping power table was given or auto search failed...\n";
        std::cout << "Auto search: " << tryFindStr << std::endl;

        std::cout << "Location of the stopping power table? ";
        std::cin >> tryPath;

        tryFindTable = DecodeItemsToTreat ( tryPath + ( string ) tryFindStr, "system", false );

        if ( tryFindTable.size() != 1 )
        {
            std::cerr << "Unable to find requested stopping power table..." << endl;
            return;
        }
    }

    analysis->energyLossData = FillGraphFromFile ( tryPath + tryFindTable[0] );

    double beamEffThickness = GetEffectiveThickness ( analysis->beamDir.Angle ( targetLadderDir ) - TMath::PiOver2(), reacInfo->targetThickness );

    analysis->reacInfoPtr->beamEk = TryGetRemainingEnergy ( pathToGDAQ + "/share/mass_db.dat", reacInfo->beamA, reacInfo->beamZ, reacInfo->beamEk, beamEffThickness, 0.001,
                                    reacInfo->targetType,  reacInfo->targetDensity, "./", "Interpolation" );
    cout << "Beam Energy after computing energy loss: " << analysis->reacInfoPtr->beamEk << "MeV in effective thickness: " << beamEffThickness << " mg/cm2\n";

    int currTreeNum = 0;

    while ( eventNbr < maxEvents )
    {
        PrintProgress ( maxEvents, eventNbr );

        uChain->GetEntry ( eventNbr );

        if ( currTreeNum != uChain->GetTreeNumber() )
        {
            currTreeNum = uChain->GetTreeNumber();

            std::cout << "Switched to next file (#" << currTreeNum << "): " << uChain->GetFile()->GetName() << std::endl;

            geomInfo = ( GoddessGeomInfos* ) uChain->GetFile()->FindObjectAny ( "GoddessGeom" );
            reacInfo = ( GoddessReacInfos* ) uChain->GetFile()->FindObjectAny ( "GoddessReac" );

            analysis->reacInfoPtr = ( GoddessReacInfos* ) reacInfo->Clone();
            analysis->geomInfoPtr = ( GoddessGeomInfos* ) geomInfo->Clone();

            analysis->reacInfoPtr->beamEk = TryGetRemainingEnergy ( pathToGDAQ + "/share/mass_db.dat", reacInfo->beamA, reacInfo->beamZ, reacInfo->beamEk, beamEffThickness, 0.001,
                                            reacInfo->targetType,  reacInfo->targetDensity, "./", "Interpolation" );
            cout << "Beam Energy after computing energy loss: " << analysis->reacInfoPtr->beamEk << "MeV in effective thickness: " << beamEffThickness << " mg/cm2\n";
        }

        ResetHistsStates();

        unsigned int vectSiSize = ( vectSiData != nullptr ) ? vectSiData->size() : vectSiDataD->size();

        for ( unsigned int i = 0; i < vectSiSize; i++ )
        {
            SiDataBase siData = ( vectSiData != nullptr ) ? vectSiData->at ( i ) : vectSiDataD->at ( i );

            if ( siData.PosE1().Mag() == 0 ) continue;

            analysis->SetSiData ( &siData );

            FillEvsAHist ( analysis );
            FillQvalHist ( analysis );

            bool noBGO = true;

            for ( unsigned int j = 0; j < vectGamData->size(); j++ )
            {
                GamData gamData = vectGamData->at ( j );

                analysis->SetGamData ( &gamData );

                if ( gamData.type == 1 )
                {
                    FilldTGammaORRUBA ( analysis, false );

                    FillGsGateORRUBA ( analysis, false );

                    FillGsVsExHist ( analysis, false );
                }
                else noBGO = false;
            }

            if ( noBGO )
            {
                FillGsVsGsBGOVeto ( analysis, vectGamData );

                for ( unsigned int j = 0; j < vectGamData->size(); j++ )
                {
                    FilldTGammaORRUBA ( analysis, true );

                    FillGsGateORRUBA ( analysis, true );

                    FillGsVsExHist ( analysis, true );
                }
            }
        }

        eventNbr++;
    }

    std::cout << "\n\n";

    return;
}










