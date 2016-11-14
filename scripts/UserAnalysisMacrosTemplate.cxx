// UserAnalysisTemplate version 161104 //

#include "GoddessAnalysis.h"
#include <sys/times.h>

using std::string;


//******************************************************************************************//
//******************************************************************************************//
// --------- Initialization and Utilities. Should not be touched -------------------------- //
// --------- User functions should be defined in the next section ------------------------- //
//******************************************************************************************//
//******************************************************************************************//


GoddessAnalysis* gA = 0;

TChain* uChain = 0;

std::map<string, TObject*> histsMap;

unsigned long long int eventNbr = 0;
unsigned long long int totEvents = 0;

void LoadTrees()
{
    std::cout << "To initialize the chain of runs, type:\n   LoadTrees( (string) treeName, (string) fileName1, (string) fileName2, (string) ... )\n\n";
}

template<typename First, typename... Rest> void LoadTrees ( string treeName, First fileName1, Rest... fileNameRest )
{
    gA = new GoddessAnalysis();

    gA->InitUserAnalysis ( treeName, fileName1, fileNameRest... );

    uChain = gA->userChain;
    totEvents = uChain->GetEntries();
}

template<typename THist> void AddHists ( THist* h1, THist* h2 )
{
    if ( std::is_same<THist, TH1F>::value || std::is_same<THist, TH2F>::value )
    {
        h1->Add ( h2 );
    }
}

void AddHists ( TH1F* h1, TH1F* h2 )
{
    AddHists<TH1F> ( h1, h2 );
}

void AddHists ( TH2F* h1, TH2F* h2 )
{
    AddHists<TH2F> ( h1, h2 );
}

template<typename THist, typename... Rest> void AddHists ( THist* h1, THist* h2, Rest... otherHists )
{
    if ( std::is_same<THist, TH1F>::value || std::is_same<THist, TH2F>::value )
    {
        AddHists ( h1, h2 );
        AddHists ( h1, otherHists... );
    }
}

template<typename THist, typename... Rest> TH1F* DrawSum ( THist* h1, THist* h2, Rest... otherHists )
{
    if ( std::is_same<THist, TH1F>::value || std::is_same<THist, TH2F>::value )
    {
        THist* hSum = ( THist* ) h1->Clone();

        AddHists ( hSum, h2, otherHists... );

        hSum->Draw();

        return hSum;
    }
    else return nullptr;
}

TH1F* DrawSum ( TH1F* h1, TH1F* h2, bool cloneFirst = true, bool doDraw = true )
{
    TH1F* hSum;

    if ( cloneFirst )
    {
        hSum = ( TH1F* ) h1->Clone();
    }
    else
    {
        hSum = h1;
    }

    hSum->Add ( h2,1 );

    if ( doDraw ) hSum->Draw();

    return hSum;
}

TH1F* DrawSum ( TH1F** hists, string toSum )
{
    std::vector<unsigned short> listOfHists = DecodeSectorsString ( toSum, false );

    TH1F* hSum;

    if ( listOfHists.size() > 0 )
    {
        hSum = ( TH1F* ) hists[listOfHists[0]]->Clone();

        for ( unsigned int i = 1; i < listOfHists.size(); i++ )
        {
            hSum->Add ( hists[listOfHists[i]], 1 );
        }

        hSum->Draw();

        return hSum;
    }

    return nullptr;
}

TH2F* DrawSum ( TH2F** hists, string toSum )
{
    std::vector<unsigned short> listOfHists = DecodeSectorsString ( toSum, false );

    TH2F* hSum;

    if ( listOfHists.size() > 0 )
    {
        hSum = ( TH2F* ) hists[listOfHists[0]]->Clone();

        for ( unsigned int i = 1; i < listOfHists.size(); i++ )
        {
            hSum->Add ( hists[listOfHists[i]], 1 );
        }

        hSum->Draw ( "colz" );

        return hSum;
    }

    return nullptr;
}

TH1F* DrawSum ( TH1F** hists )
{
    if ( *hists != nullptr )
    {
        TH1F* hSum = dynamic_cast<TH1F*> ( *hists );

        if ( hSum != nullptr )
        {
            int counter = 1;

            while ( * ( hists+counter ) != nullptr )
            {
                TH1F* hAdd = dynamic_cast<TH1F*> ( * ( hists+counter ) );

                if ( hAdd != nullptr )
                {
                    hSum->Add ( hAdd );
                }

                counter++;
            }

            return hSum;
        }
    }

    return nullptr;
}

void PrintProgress ( unsigned long long int maxEvents_ )
{
    if ( eventNbr % 10000 == 0 )
    {
        std::cout << "Treated " << std::setw ( 11 ) << eventNbr << " / " << std::setw ( 11 ) << std::left << maxEvents_;
        std::cout << " ( " << std::setw ( 5 ) << std::setprecision ( 2 ) << std::fixed << ( float ) eventNbr/maxEvents_ * 100. << " % )\r" << std::flush;
    }
}

TH1F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX )
{
    TH1F* newHist = new TH1F ( name.c_str(), title.c_str(), nBinsX, minX, maxX );
    histsMap[title] = newHist;

    return newHist;
}

TH2F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY )
{
    TH2F* newHist = new TH2F ( name.c_str(), title.c_str(), nBinsX, minX, maxX, nBinsY, minY, maxY );
    histsMap[title] = newHist;

    return newHist;
}

// ----------- Handles the writing to a TFile ---------------------- //

void WriteUserHists ( string outName )
{
    string mode = "recreate";

    TFile* outRootFile = new TFile ( outName.c_str(), "read" );

    int userChoice;

    if ( outRootFile->IsOpen() )
    {
        std::cout << "File " << outName << " already exists...\n";
        std::cout << "Would you like to overwrite it [1] or update it [2]? ";
        std::cin >> userChoice;

        if ( userChoice == 2 )
        {
            mode = "update";
        }
        else if ( std::cin.fail() || userChoice < 1 || userChoice > 2 )
        {
            std::cout << "Invalid input... aborting...\n";
            return;
        }

        outRootFile->Close();

        std::cout << "\n";
    }

    outRootFile = new TFile ( outName.c_str(), mode.c_str() );

    for ( auto itr = histsMap.begin(); itr != histsMap.end(); itr++ )
    {
        TH1F* h1 = dynamic_cast<TH1F*> ( itr->second );
        TH2F* h2 = dynamic_cast<TH2F*> ( itr->second );

        if ( h1 != NULL )
        {
            h1->Write();
        }
        else if ( h2 != NULL )
        {
            h2->Write();
        }
    }

    outRootFile->Close();
}

// ----------------------- A few functions to make life easier ----------------------------- //

bool OrrubaIsDigital ( SiDataBase* siData_ )
{
    return ( siData_->isBarrel && ( siData_->sector >=1 && siData_->sector <= 6 ) ) ||
           ( !siData_->isBarrel && ( ( siData_->isUpstream && ( siData_->sector == 0 || siData_->sector == 1 ) ) || ( !siData_->isUpstream && siData_->sector == 2 ) ) );
}

bool OrrubaGoodSector ( SiDataBase* siData_ )
{
    return ( !siData_->isBarrel )
           || ( siData_->isBarrel && siData_->isUpstream && ( siData_->sector != 9 || siData_->sector != 10 ) )
           || ( siData_->isBarrel && !siData_->isUpstream );
}

bool OrrubaGsGoodTs ( SiDataBase* siData_, GamData* gamData_ )
{
    bool isDigital = OrrubaIsDigital ( siData_ );

    unsigned long long int orrubaTs = siData_->TimestampMaxLayer ( 1, false );

    unsigned long long int gsTs = gamData_->time;

    return ( isDigital && gsTs - orrubaTs > 190 && gsTs - orrubaTs < 210 ) || ( !isDigital && gsTs - orrubaTs > 410 && gsTs - orrubaTs < 430 );
}

bool OrrubaEnergyThr ( SiDataBase* siData_ )
{
    bool isDigital = OrrubaIsDigital ( siData_ );

    return ( !siData_->isUpstream && ( ( isDigital && siData_->ESumLayer ( 1,false ) > 5000 ) || ( !isDigital && siData_->ESumLayer ( 1,false ) > 500 ) ) )
           || ( siData_->isUpstream && siData_->ESumLayer ( 1,false ) > 1.2 );
}


//******************************************************************************************//
//******************************************************************************************//
// ------------------------- Write your macros here --------------------------------------- //
// ----------------- Use the variable uChain to process your root files ------------------- //
//******************************************************************************************//
//******************************************************************************************//

// ------------- Particles Energy vs. Angle ----------- //

const int maxMult = 10;

TH2F* hEvsA[maxMult];
TH2F* hEvsA_SX3U_tot[maxMult];
TH2F* hEvsA_QQQ5U_tot[maxMult];
TH2F* hEvsA_SX3U[12][maxMult];
TH2F* hEvsA_QQQ5U[4][maxMult];

void InitEvsAHist ( unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY )
{
    for ( int j = 0; j < maxMult; j++ )
    {
        hEvsA[j] = MakeNewHist ( Form ( "En_vs_Angle_mult%d", j ), Form ( "Energy vs. Angle mult %d", j ), nBinsX, minX, maxX, nBinsY, minY, maxY );

        hEvsA_SX3U_tot[j] = MakeNewHist ( Form ( "En_vs_Angle_SX3_mult%d", j ), Form ( "Energy vs. Angle SX3 Upstream mult %d", j ), nBinsX, minX, maxX, nBinsY, minY, maxY );

        hEvsA_QQQ5U_tot[j] = MakeNewHist ( Form ( "En_vs_Angle_QQQ5_mult%d", j ), Form ( "Energy vs. Angle QQQ5 mult %d", j ), 34, -1, 33, nBinsY, minY, maxY );
    }

    for ( int i = 0; i < 12; i++ )
    {
        for ( int j = 0; j < maxMult; j++ )
        {
            hEvsA_SX3U[i][j] = MakeNewHist ( Form ( "En_vs_Angle_SX3U%d_mult%d", i, j ), Form ( "Energy vs Angle SX3 U%d mult %d", i, j ), nBinsX, minX, maxX, nBinsY, minY, maxY );
        }
    }

    for ( int i = 0; i < 4; i++ )
    {
        for ( int j = 0; j < maxMult; j++ )
        {
            hEvsA_QQQ5U[i][j] = MakeNewHist ( Form ( "En_vs_Angle_QQQ5U%d_mult%d", i, j ), Form ( "Energy vs Angle QQQ5 U%d mult %d", i, j ), 34, -1, 33, nBinsY, minY, maxY );
        }
    }
}

void FillEvsAHist ( SiDataBase* siData_ )
{
    float energy = siData_->ESumLayer ( 1, false );
    float angle = siData_->Angle ( 1 );

    int sector = siData_->sector;
    int strip = siData_->StripMaxLayer ( 1, false );

    int mult = siData_->MultLayer ( 1, false );

    if ( energy > 0  && mult < 3 )
    {
        if ( angle != 0 ) hEvsA[mult]->Fill ( angle, energy );

        if ( siData_->isUpstream )
        {
            if ( angle != 0 && siData_->isBarrel )
            {
                hEvsA_SX3U_tot[mult]->Fill ( angle, energy );
                hEvsA_SX3U[sector][mult]->Fill ( angle, energy );
            }
            else
            {
                hEvsA_QQQ5U_tot[mult]->Fill ( strip, energy );
                hEvsA_QQQ5U[sector][mult]->Fill ( strip, energy );
            }
        }
    }
}


// ------------- Q-Value spectra ----------- //

TH1F* hQval_tot;
TH1F* hQval_SX3U_tot;
TH1F* hQval_QQQ5U_tot;
TH1F* hQval_SX3U[12];
TH1F* hQval_QQQ5U[4];

TH1F* hEx_tot;
TH1F* hEx_SX3U_tot;
TH1F* hEx_QQQ5U_tot;
TH1F* hEx_SX3U[12];
TH1F* hEx_QQQ5U[4];

TH2F* hGsVsEx_tot;
TH2F* hGsVsEx_SX3U_tot;
TH2F* hGsVsEx_QQQ5U_tot;
TH2F* hGsVsEx_SX3U[12];
TH2F* hGsVsEx_QQQ5U[4];

void InitQvalHist ( unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY )
{
    hQval_tot = MakeNewHist ( "Qvalue_tot", "Q-value tot", nBinsX, minX, maxX );
    hQval_SX3U_tot = MakeNewHist ( "Qvalue_tot_SX3U", "Q-value SX3s Upstream", nBinsX, minX, maxX );
    hQval_QQQ5U_tot = MakeNewHist ( "Qvalue_tot_QQQ5U", "Q-value QQQ5s Upstream", nBinsX, minX, maxX );

    for ( int i = 0; i < 12; i++ )
    {
        hQval_SX3U[i] = MakeNewHist ( Form ( "Qvalue_SX3U%d", i ), Form ( "Qvalue SX3 U%d", i ), nBinsX, minX, maxX );
    }

    for ( int i = 0; i < 4; i++ )
    {
        hQval_QQQ5U[i] = MakeNewHist ( Form ( "Qvalue_QQQ5U%d", i ), Form ( "Qvalue QQQ5 U%d", i ), nBinsX, minX, maxX );
    }

    hEx_tot = MakeNewHist ( "Ex_tot", "Excitation Energy tot", nBinsX, minX, maxX );
    hEx_SX3U_tot = MakeNewHist ( "Ex_tot_SX3U", "Excitation Energy SX3s Upstream", nBinsX, minX, maxX );
    hEx_QQQ5U_tot = MakeNewHist ( "Ex_tot_QQQ5U", "Excitation Energy QQQ5s Upstream", nBinsX, minX, maxX );

    for ( int i = 0; i < 12; i++ )
    {
        hEx_SX3U[i] = MakeNewHist ( Form ( "Ex_SX3U%d", i ), Form ( "Excitation Energy SX3 U%d", i ), nBinsX, minX, maxX );
    }

    for ( int i = 0; i < 4; i++ )
    {
        hEx_QQQ5U[i] = MakeNewHist ( Form ( "Ex_QQQ5U%d", i ), Form ( "Excitation Energy QQQ5 U%d", i ), nBinsX, minX, maxX );
    }

    hGsVsEx_tot = MakeNewHist ( "GsVsEx_tot", "Gamma Energy vs. Excitation Energy tot", nBinsX, minX, maxX, nBinsY, minY, maxY );
    hGsVsEx_SX3U_tot = MakeNewHist ( "GsVsEx_tot_SX3U", "Gamma Energy vs. Excitation Energy SX3s Upstream", nBinsX, minX, maxX, nBinsY, minY, maxY );
    hGsVsEx_QQQ5U_tot = MakeNewHist ( "GsVsEx_tot_QQQ5U", "Gamma Energy vs. Excitation Energy QQQ5s Upstream", nBinsX, minX, maxX, nBinsY, minY, maxY );

    for ( int i = 0; i < 12; i++ )
    {
        hGsVsEx_SX3U[i] = MakeNewHist ( Form ( "GsVsEx_SX3U%d", i ), Form ( "Gamma Energy vs. Excitation Energy SX3 U%d", i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
    }

    for ( int i = 0; i < 4; i++ )
    {
        hGsVsEx_QQQ5U[i] = MakeNewHist ( Form ( "GsVsEx_QQQ5U%d", i ), Form ( "Gamma Energy vs. Excitation Energy QQQ5 U%d", i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
    }
}

void FillQvalHist ( SiDataBase* siData_, float massBeam, float massTarget, float massRecoil, float massEjec, float beamEk, float qValGsGs )
{
    float angle = siData_->Angle ( 1 );
    float qval = siData_->QValue ( massBeam, beamEk, massTarget, massEjec );

    float exEn = -qval + qValGsGs;

    unsigned int sector = siData_->sector;

    double siEn = siData_->ESumLayer ( 1, false );


    if ( angle != 0 && siEn > 0 )
    {
        hQval_tot->Fill ( qval );
        hEx_tot->Fill ( qval );

        if ( siData_->isUpstream )
        {
            if ( siData_->isBarrel )
            {
                hQval_SX3U_tot->Fill ( qval );
                hQval_SX3U[sector]->Fill ( qval );

                hEx_SX3U_tot->Fill ( exEn );
                hEx_SX3U[sector]->Fill ( exEn );
            }
            else
            {
                hQval_QQQ5U_tot->Fill ( qval );
                hQval_QQQ5U[sector]->Fill ( qval );

                hEx_QQQ5U_tot->Fill ( exEn );
                hEx_QQQ5U[sector]->Fill ( exEn );
            }
        }
    }
}

void FillGsVsExHist ( GamData* gamData_, SiDataBase* siData_, float massBeam, float massTarget, float massRecoil, float massEjec, float beamEk, float qValGsGs )
{
    float angle = siData_->Angle ( 1 );
    float qval = siData_->QValue ( massBeam, beamEk, massTarget, massEjec );

    float exEn = -qval + qValGsGs;

    unsigned int sector = siData_->sector;

    unsigned long long int orrubaTs = siData_->TimestampMaxLayer ( 1, false );

    unsigned long long int gsTs = gamData_->time;

    double gsEn = ( double ) gamData_->en/3.;
    double siEn = siData_->ESumLayer ( 1, false );

    bool filled = false;

    bool isDigital = OrrubaIsDigital ( siData_ );

    bool timestampOK = OrrubaGsGoodTs ( siData_, gamData_ );

    bool sectorOK = OrrubaGoodSector ( siData_ );

    bool energyOK = OrrubaEnergyThr ( siData_ );

    bool doFill = timestampOK && sectorOK && energyOK;

    if ( doFill )
    {
        if ( siData_->isBarrel )
        {
            if ( siData_->isUpstream )
            {
                hGsVsEx_SX3U[sector]->Fill ( exEn, gsEn );
            }
            else
            {

            }
        }
        else
        {
            if ( siData_->isUpstream )
            {
                hGsVsEx_QQQ5U[sector]->Fill ( exEn, gsEn );
            }
            else
            {

            }
        }
    }

}

// ------------- Time difference between GammaSphere and ORRUBA ----------- //

TH1F* dTGsSX3D[12];
TH1F* dTGsSX3U[12];
TH1F* dTGsQQQ5D[4];
TH1F* dTGsQQQ5U[4];


TH1F* dTGsNoBGOSX3D[12];
TH1F* dTGsNoBGOSX3U[12];
TH1F* dTGsNoBGOQQQ5D[4];
TH1F* dTGsNoBGOQQQ5U[4];

void InitdTGsORRUBAHists ( unsigned int nBinsX = 1000, int minX = -500, int maxX = 500 )
{
    for ( int i = 0; i < 12; i++ )
    {
        dTGsSX3U[i] = MakeNewHist ( Form ( "dT_GS_SX3U%d", i ), Form ( "dT GS SX3 U%d", i ), nBinsX, minX, maxX );

        dTGsSX3D[i] = MakeNewHist ( Form ( "dT GS SX3D%d", i ), Form ( "dT GS SX3 D%d", i ), nBinsX, minX, maxX );

        dTGsNoBGOSX3U[i] = MakeNewHist ( Form ( "dT_GS_NoBGO SX3U%d", i ), Form ( "dT GS No BGO SX3 U%d", i ), nBinsX, minX, maxX );

        dTGsNoBGOSX3D[i] = MakeNewHist ( Form ( "dT_GS_NoBGO_SX3D%d", i ), Form ( "dT GS No BGO SX3 D%d", i ), nBinsX, minX, maxX );
    }

    for ( int i = 0; i < 4; i++ )
    {
        dTGsQQQ5U[i] = MakeNewHist ( Form ( "dT_GS_QQQ5U%d", i ), Form ( "dT GS QQQ5 U%d", i ), nBinsX, minX, maxX );

        dTGsQQQ5D[i] = MakeNewHist ( Form ( "dT_GS_QQQ5D%d", i ), Form ( "dT GS QQQ5 D%d", i ), nBinsX, minX, maxX );

        dTGsNoBGOQQQ5U[i] = MakeNewHist ( Form ( "dT_GS_NoBGO QQQ5U%d", i ), Form ( "dT GS No BGO QQQ5 U%d", i ), nBinsX, minX, maxX );

        dTGsNoBGOQQQ5D[i] = MakeNewHist ( Form ( "dT_GS_NoBGO QQQ5D%d", i ), Form ( "dT GS No BGO QQQ5 D%d", i ), nBinsX, minX, maxX );
    }
}

void FilldTGammaORRUBA ( SiDataBase* siData_, GamData* gamData_ )
{
    if ( gamData_->type == 2 ) return;

    unsigned long long int orrubaTs = siData_->TimestampMaxLayer ( 1, false );

    int sector = siData_->sector;

    unsigned long long int gsTs = gamData_->time;

    if ( siData_->isBarrel )
    {
        if ( siData_->isUpstream )
        {
            dTGsSX3U[sector]->Fill ( gsTs - orrubaTs );
        }
        else
        {
            dTGsSX3D[sector]->Fill ( gsTs - orrubaTs );
        }
    }
    else
    {
        if ( siData_->isUpstream )
        {
            dTGsQQQ5U[sector]->Fill ( gsTs - orrubaTs );
        }
        else
        {
            dTGsQQQ5D[sector]->Fill ( gsTs - orrubaTs );
        }
    }


    return;
}


void FilldTNoBGOGammaORRUBA ( SiDataBase* siData_, GamData* gamData_ )
{
    unsigned long long int orrubaTs = siData_->TimestampMaxLayer ( 1, false );

    int sector = siData_->sector;

    unsigned long long int gsTs = gamData_->time;

    if ( siData_->isBarrel )
    {
        if ( siData_->isUpstream )
        {
            dTGsNoBGOSX3U[sector]->Fill ( gsTs - orrubaTs );
        }
        else
        {
            dTGsNoBGOSX3D[sector]->Fill ( gsTs - orrubaTs );
        }
    }
    else
    {
        if ( siData_->isUpstream )
        {
            dTGsNoBGOQQQ5U[sector]->Fill ( gsTs - orrubaTs );
        }
        else
        {
            dTGsNoBGOQQQ5D[sector]->Fill ( gsTs - orrubaTs );
        }
    }


    return;
}

// -------------------- GammaSphere gated by ORRUBA ------------------------ //

TH1F* gsGatedSX3U[12];
std::pair<TH1F*,bool> gsGatedSX3U_analog;
std::pair<TH1F*,bool> gsGatedSX3U_digital;

TH1F* gsGatedSX3D[12];
std::pair<TH1F*,bool> gsGatedSX3D_analog;
std::pair<TH1F*,bool> gsGatedSX3D_digital;

TH1F* gsGatedQQQ5U[4];
std::pair<TH1F*,bool> gsGatedQQQ5U_analog;
std::pair<TH1F*,bool> gsGatedQQQ5U_digital;

TH1F* gsGatedQQQ5D[4];
std::pair<TH1F*,bool> gsGatedQQQ5D_analog;
std::pair<TH1F*,bool> gsGatedQQQ5D_digital;


TH1F* gsNoBGOGatedSX3U[12];
std::pair<TH1F*,bool> gsNoBGOGatedSX3U_tot;
std::pair<TH1F*,bool> gsNoBGOGatedSX3U_analog;
std::pair<TH1F*,bool> gsNoBGOGatedSX3U_digital;

TH1F* gsNoBGOGatedSX3D[12];
std::pair<TH1F*,bool> gsNoBGOGatedSX3D_tot;
std::pair<TH1F*,bool> gsNoBGOGatedSX3D_analog;
std::pair<TH1F*,bool> gsNoBGOGatedSX3D_digital;

TH1F* gsNoBGOGatedQQQ5U[4];
std::pair<TH1F*,bool> gsNoBGOGatedQQQ5U_tot;
std::pair<TH1F*,bool> gsNoBGOGatedQQQ5U_analog;
std::pair<TH1F*,bool> gsNoBGOGatedQQQ5U_digital;

TH1F* gsNoBGOGatedQQQ5D[4];
std::pair<TH1F*,bool> gsNoBGOGatedQQQ5D_tot;
std::pair<TH1F*,bool> gsNoBGOGatedQQQ5D_analog;
std::pair<TH1F*,bool> gsNoBGOGatedQQQ5D_digital;

void ResetGsHistsState()
{
    gsGatedSX3U_analog.second = false;
    gsGatedSX3U_digital.second = false;

    gsGatedSX3D_analog.second = false;
    gsGatedSX3D_digital.second = false;

    gsGatedQQQ5U_analog.second = false;
    gsGatedQQQ5U_digital.second = false;

    gsGatedQQQ5D_analog.second = false;
    gsGatedQQQ5D_digital.second = false;

    gsNoBGOGatedSX3U_tot.second = false;
    gsNoBGOGatedSX3U_analog.second = false;
    gsNoBGOGatedSX3U_digital.second = false;

    gsNoBGOGatedSX3D_tot.second = false;
    gsNoBGOGatedSX3D_analog.second = false;
    gsNoBGOGatedSX3D_digital.second = false;

    gsNoBGOGatedQQQ5U_tot.second = false;
    gsNoBGOGatedQQQ5U_analog.second = false;
    gsNoBGOGatedQQQ5U_digital.second = false;

    gsNoBGOGatedQQQ5D_tot.second = false;
    gsNoBGOGatedQQQ5D_analog.second = false;
    gsNoBGOGatedQQQ5D_digital.second = false;
}

void InitGsGateORRUBAHists ( unsigned int nBinsX = 5000, int minX = 0, int maxX = 5000 )
{
    ResetGsHistsState();

    for ( int i = 0; i < 12; i++ )
    {
        gsGatedSX3U[i] = MakeNewHist ( Form ( "Gs_Gates_SX3U%d", i ), Form ( "GammaSphere Gates SX3 U%d", i ), nBinsX, minX, maxX );
    }

    gsGatedSX3U_analog.first = MakeNewHist ( "Gs_GatesSX3U_Analog", "GammaSphere Gates SX3 Upstream Analog", nBinsX, minX, maxX );
    gsGatedSX3U_digital.first = MakeNewHist ( "Gs_Gates_SX3U_Digital ", "GammaSphere Gates SX3 Upstream Digital", nBinsX, minX, maxX );

    for ( int i = 0; i < 12; i++ )
    {
        gsGatedSX3D[i] = MakeNewHist ( Form ( "Gs_Gates_SX3D%d", i ), Form ( "GammaSphere Gates SX3 D%d", i ), nBinsX, minX, maxX );
    }

    gsGatedSX3D_analog.first = MakeNewHist ( "Gs_Gates_SX3D_Analog", "GammaSphere Gates SX3 Downstream Analog", nBinsX, minX, maxX );
    gsGatedSX3D_digital.first = MakeNewHist ( "Gs_Gates_SX3D_Digital ", "GammaSphere Gates SX3 Downstream Digital", nBinsX, minX, maxX );

    for ( int i = 0; i < 4; i++ )
    {
        gsGatedQQQ5U[i] = MakeNewHist ( Form ( "Gs_Gates_QQQ5U%d", i ), Form ( "GammaSphere Gates QQQ5 U%d", i ), nBinsX, minX, maxX );
    }

    gsGatedQQQ5U_analog.first = MakeNewHist ( "Gs_Gates_QQQ5U_Analog", "GammaSphere Gates QQQ5 Upstream Analog", nBinsX, minX, maxX );
    gsGatedQQQ5U_digital.first = MakeNewHist ( "Gs_Gates_QQQ5U_Digital ", "GammaSphere Gates QQQ5 Upstream Digital", nBinsX, minX, maxX );

    for ( int i = 0; i < 4; i++ )
    {
        gsGatedQQQ5D[i] = MakeNewHist ( Form ( "Gs_Gates_QQQ5D%d", i ), Form ( "GammaSphere Gates QQQ5 D%d", i ), nBinsX, minX, maxX );
    }

    gsGatedQQQ5D_analog.first = MakeNewHist ( "Gs_Gates_QQQ5D_Analog", "GammaSphere Gates QQQ5 Downstream Analog", nBinsX, minX, maxX );
    gsGatedQQQ5D_digital.first = MakeNewHist ( "Gs_Gates_QQQ5D_Digital ", "GammaSphere Gates QQQ5 Downstream Digital", nBinsX, minX, maxX );


    for ( int i = 0; i < 12; i++ )
    {
        gsNoBGOGatedSX3U[i] = MakeNewHist ( Form ( "Gs_NoBGO_Gates_SX3U%d", i ), Form ( "GammaSphere No BGO Gates SX3 U%d", i ), nBinsX, minX, maxX );
    }

    gsNoBGOGatedSX3U_tot.first = MakeNewHist ( "Gs_NoBGO_Gates_SX3U", "GammaSphere No BGO Gates SX3 Upstream", nBinsX, minX, maxX );
    gsNoBGOGatedSX3U_analog.first = MakeNewHist ( "Gs_NoBGO_Gates_SX3U_Analog", "GammaSphere No BGO Gates SX3 Upstream Analog", nBinsX, minX, maxX );
    gsNoBGOGatedSX3U_digital.first = MakeNewHist ( "Gs_NoBGO_Gates_SX3U_Digital ", "GammaSphere No BGO Gates SX3 Upstream Digital", nBinsX, minX, maxX );

    for ( int i = 0; i < 12; i++ )
    {
        gsNoBGOGatedSX3D[i] = MakeNewHist ( Form ( "Gs_NoBGO_Gates_SX3D%d", i ), Form ( "GammaSphere No BGO Gates SX3 D%d", i ), nBinsX, minX, maxX );
    }

    gsNoBGOGatedSX3D_tot.first = MakeNewHist ( "Gs_NoBGO_Gates_SX3D", "GammaSphere No BGO Gates SX3 Downstream", nBinsX, minX, maxX );
    gsNoBGOGatedSX3D_analog.first = MakeNewHist ( "Gs_NoBGO_Gates_SX3D_Analog", "GammaSphere No BGO Gates SX3 Downstream Analog", nBinsX, minX, maxX );
    gsNoBGOGatedSX3D_digital.first = MakeNewHist ( "Gs_NoBGO_Gates_SX3D_Digital ", "GammaSphere No BGO Gates SX3 Downstream Digital", nBinsX, minX, maxX );

    for ( int i = 0; i < 4; i++ )
    {
        gsNoBGOGatedQQQ5U[i] = MakeNewHist ( Form ( "Gs_NoBGO_Gates_QQQ5U%d", i ), Form ( "GammaSphere No BGO Gates QQQ5 U%d", i ), nBinsX, minX, maxX );
    }

    gsNoBGOGatedQQQ5U_tot.first = MakeNewHist ( "Gs_NoBGO_Gates_QQQ5U", "GammaSphere No BGO Gates QQQ5 Upstream", nBinsX, minX, maxX );
    gsNoBGOGatedQQQ5U_analog.first = MakeNewHist ( "Gs_NoBGO_Gates_QQQ5U_Analog", "GammaSphere No BGO Gates QQQ5 Upstream Analog", nBinsX, minX, maxX );
    gsNoBGOGatedQQQ5U_digital.first = MakeNewHist ( "Gs_NoBGO_Gates_QQQ5U_Digital ", "GammaSphere No BGO Gates QQQ5 Upstream Digital", nBinsX, minX, maxX );

    for ( int i = 0; i < 4; i++ )
    {
        gsNoBGOGatedQQQ5D[i] = MakeNewHist ( Form("Gs_NoBGO_Gates_QQQ5D%d", i), Form("GammaSphere No BGO Gates QQQ5 D%d", i), nBinsX, minX, maxX );
    }

    gsNoBGOGatedQQQ5D_tot.first = MakeNewHist ( "Gs_NoBGO_Gates_QQQ5D", "GammaSphere No BGO Gates QQQ5 Downstream", nBinsX, minX, maxX );
    gsNoBGOGatedQQQ5D_analog.first = MakeNewHist ( "Gs_NoBGO_Gates_QQQ5D_Analog", "GammaSphere No BGO Gates QQQ5 Downstream Analog", nBinsX, minX, maxX );
    gsNoBGOGatedQQQ5D_digital.first = MakeNewHist ( "Gs_NoBGO_Gates_QQQ5D_Digital ", "GammaSphere No BGO Gates QQQ5 Downstream Digital", nBinsX, minX, maxX );
}

bool FillGsGateORRUBA ( SiDataBase* siData_, GamData* gamData_ )
{
    unsigned long long int orrubaTs = siData_->TimestampMaxLayer ( 1, false );

    int sector = siData_->sector;

    unsigned long long int gsTs = gamData_->time;

    double gsEn = ( double ) gamData_->en/3.;
    double siEn = siData_->ESumLayer ( 1, false );

    bool filled = false;

    bool isDigital = OrrubaIsDigital ( siData_ );

    bool timestampOK = OrrubaGsGoodTs ( siData_, gamData_ );

    bool sectorOK = OrrubaGoodSector ( siData_ );

    bool energyOK = OrrubaEnergyThr ( siData_ );

    if ( sectorOK && timestampOK && energyOK )
//     if ( timestampOK && energyOK )
//     if ( timestampOK )
    {
        if ( siData_->isBarrel )
        {
            if ( siData_->isUpstream )
            {
                gsGatedSX3U[sector]->Fill ( gsEn );

                if ( sector > 0 && sector < 7 )
                {
                    if ( !gsGatedSX3U_digital.second )
                    {
                        gsGatedSX3U_digital.second = true;
                        gsGatedSX3U_digital.first->Fill ( gsEn );
                    }
                }
                else
                {
                    if ( !gsGatedSX3U_analog.second )
                    {
                        gsGatedSX3U_analog.second = true;
                        gsGatedSX3U_analog.first->Fill ( gsEn );
                    }
                }
            }
            else
            {
                gsGatedSX3D[sector]->Fill ( gsEn );

                if ( sector > 0 && sector < 7 )
                {
                    if ( !gsGatedSX3D_digital.second )
                    {
                        gsGatedSX3D_digital.second = true;
                        gsGatedSX3D_digital.first->Fill ( gsEn );
                    }
                }
                else
                {
                    if ( !gsGatedSX3D_analog.second )
                    {
                        gsGatedSX3D_analog.second = true;
                        gsGatedSX3D_analog.first->Fill ( gsEn );
                    }
                }
            }

//                 filled = true;
        }
        else
        {
            if ( siData_->isUpstream )
            {
                gsGatedQQQ5U[sector]->Fill ( gsEn );

                if ( sector == 0 || sector == 1 )
                {
                    if ( !gsGatedQQQ5U_digital.second )
                    {
                        gsGatedQQQ5U_digital.second = true;
                        gsGatedQQQ5U_digital.first->Fill ( gsEn );
                    }
                }
                else
                {
                    if ( !gsGatedQQQ5U_analog.second )
                    {
                        gsGatedQQQ5U_analog.second = true;
                        gsGatedQQQ5U_analog.first->Fill ( gsEn );
                    }
                }
            }
            else
            {
                gsGatedQQQ5D[sector]->Fill ( gsEn );

                if ( sector == 2 || sector == 3 )
                {
                    if ( !gsGatedQQQ5D_digital.second )
                    {
                        gsGatedQQQ5D_digital.second = true;
                        gsGatedQQQ5D_digital.first->Fill ( gsEn );
                    }
                }
                else
                {
                    if ( !gsGatedQQQ5D_analog.second )
                    {
                        gsGatedQQQ5D_analog.second = true;
                        gsGatedQQQ5D_analog.first->Fill ( gsEn );
                    }
                }
            }

//                 filled = true;
        }
    }

    return filled;
}

bool FillGsNoBGOGateORRUBA ( SiDataBase* siData_, GamData* gamData_ )
{
    unsigned long long int orrubaTs = siData_->TimestampMaxLayer ( 1, false );

    int sector = siData_->sector;

    unsigned long long int gsTs = gamData_->time;

    double gsEn = ( double ) gamData_->en/3.;
    double siEn = siData_->ESumLayer ( 1, false );

    bool filled = false;

    bool isDigital = OrrubaIsDigital ( siData_ );

    bool timestampOK = OrrubaGsGoodTs ( siData_, gamData_ );

    bool sectorOK = OrrubaGoodSector ( siData_ );

    bool energyOK = OrrubaEnergyThr ( siData_ );

    if ( sectorOK && timestampOK && energyOK )
//     if ( timestampOK && energyOK )
//     if ( timestampOK )
    {
        if ( siData_->isBarrel )
        {
            if ( siData_->isUpstream )
            {
                gsNoBGOGatedSX3U[sector]->Fill ( gsEn );

                gsNoBGOGatedSX3U_tot.second = true;
                gsNoBGOGatedSX3U_tot.first->Fill ( gsEn );

                if ( sector > 0 && sector < 7 )
                {
                    if ( !gsNoBGOGatedSX3U_digital.second )
                    {
                        gsNoBGOGatedSX3U_digital.second = true;
                        gsNoBGOGatedSX3U_digital.first->Fill ( gsEn );
                    }
                }
                else
                {
                    if ( !gsNoBGOGatedSX3U_analog.second )
                    {
                        gsNoBGOGatedSX3U_analog.second = true;
                        gsNoBGOGatedSX3U_analog.first->Fill ( gsEn );
                    }
                }
            }
            else
            {
                gsNoBGOGatedSX3D[sector]->Fill ( gsEn );

                gsNoBGOGatedSX3D_tot.second = true;
                gsNoBGOGatedSX3D_tot.first->Fill ( gsEn );

                if ( sector > 0 && sector < 7 )
                {
                    if ( !gsNoBGOGatedSX3D_digital.second )
                    {
                        gsNoBGOGatedSX3D_digital.second = true;
                        gsNoBGOGatedSX3D_digital.first->Fill ( gsEn );
                    }
                }
                else
                {
                    if ( !gsNoBGOGatedSX3D_analog.second )
                    {
                        gsNoBGOGatedSX3D_analog.second = true;
                        gsNoBGOGatedSX3D_analog.first->Fill ( gsEn );
                    }
                }
            }

//                 filled = true;
        }
        else
        {
            if ( siData_->isUpstream )
            {
                gsNoBGOGatedQQQ5U[sector]->Fill ( gsEn );

                gsNoBGOGatedQQQ5U_tot.second = true;
                gsNoBGOGatedQQQ5U_tot.first->Fill ( gsEn );

                if ( sector == 0 || sector == 1 )
                {
                    if ( !gsNoBGOGatedQQQ5U_digital.second )
                    {
                        gsNoBGOGatedQQQ5U_digital.second = true;
                        gsNoBGOGatedQQQ5U_digital.first->Fill ( gsEn );
                    }
                }
                else
                {
                    if ( !gsNoBGOGatedQQQ5U_analog.second )
                    {
                        gsNoBGOGatedQQQ5U_analog.second = true;
                        gsNoBGOGatedQQQ5U_analog.first->Fill ( gsEn );
                    }
                }
            }
            else
            {
                gsNoBGOGatedQQQ5D[sector]->Fill ( gsEn );

                gsNoBGOGatedQQQ5D_tot.second = true;
                gsNoBGOGatedQQQ5D_tot.first->Fill ( gsEn );

                if ( sector == 2 || sector == 3 )
                {
                    if ( !gsNoBGOGatedQQQ5D_digital.second )
                    {
                        gsNoBGOGatedQQQ5D_digital.second = true;
                        gsNoBGOGatedQQQ5D_digital.first->Fill ( gsEn );
                    }
                }
                else
                {
                    if ( !gsNoBGOGatedQQQ5D_analog.second )
                    {
                        gsNoBGOGatedQQQ5D_analog.second = true;
                        gsNoBGOGatedQQQ5D_analog.first->Fill ( gsEn );
                    }
                }
            }

//                 filled = true;
        }
    }

    return filled;
}

// --------- Put here the functions you want to be processed ---------------- //

void FillUserHists ( unsigned long long int maxEvents = 0 )
{
    std::vector<SiDataBase>* siData = new std::vector<SiDataBase>();
    std::vector<GamData>* gamData = new std::vector<GamData>();

    uChain->SetBranchAddress ( "si", &siData );
    uChain->SetBranchAddress ( "gam", &gamData );

    InitdTGsORRUBAHists();
    InitGsGateORRUBAHists();
    InitEvsAHist ( 1800, 0, 180, 1000, 0, 10 );
    InitQvalHist ( 800, -20, 20, 5000, 0, 5000 );

    eventNbr = 0;

    float massBeam = 134.;
    float beamEk = 1337;
    float massTarget = 2.;
    float massRecoil = 135.;
    float massEjec = 1.;
    float qValGsGs = 4.1;

    if ( maxEvents <= 0 || maxEvents > totEvents ) maxEvents = totEvents;

    while ( eventNbr < maxEvents )
    {
        PrintProgress ( maxEvents );

        uChain->GetEntry ( eventNbr );

        ResetGsHistsState();

        for ( unsigned int i = 0; i < siData->size(); i++ )
        {
            float angle = siData->at ( i ).Angle ( 1 );
            float qval = siData->at ( i ).QValue ( massBeam, beamEk, massTarget, massEjec, massRecoil );

            float ex = -qval + qValGsGs;

            unsigned int sector = siData->at ( i ).sector;

            unsigned long long int orrubaTs = siData->at ( i ).TimestampMaxLayer ( 1, false );

            double siEn = siData->at ( i ).ESumLayer ( 1, false );

            bool filled = false;

            bool isDigital = OrrubaIsDigital ( & ( siData->at ( i ) ) );

            bool sectorOK = OrrubaGoodSector ( & ( siData->at ( i ) ) );

            bool energyOK = OrrubaEnergyThr ( & ( siData->at ( i ) ) );

            FillEvsAHist ( & ( siData->at ( i ) ) );
            FillQvalHist ( & ( siData->at ( i ) ), massBeam, massTarget, massRecoil, massEjec, beamEk, qValGsGs );

            bool noBGO = true;

            for ( unsigned int j = 0; j < gamData->size(); j++ )
            {
                unsigned long long int gsTs = gamData->at ( j ).time;

                double gsEn = ( double ) gamData->at ( j ).en/3.;

                bool timestampOK = OrrubaGsGoodTs ( & ( siData->at ( i ) ), & ( gamData->at ( j ) ) );

                bool doFill = timestampOK && sectorOK && energyOK;

                int gamType = gamData->at ( j ).type;

                FillGsVsExHist ( & ( gamData->at ( j ) ), & ( siData->at ( i ) ), massBeam, massTarget, massRecoil, massEjec, beamEk, qValGsGs );

                if ( gamData->at ( j ).type == 1 )
                {
                    FilldTGammaORRUBA ( & ( siData->at ( i ) ), & ( gamData->at ( j ) ) );

                    FillGsGateORRUBA ( & ( siData->at ( i ) ), & ( gamData->at ( j ) ) );
                }
                else noBGO = false;
            }

            if ( noBGO )
            {
                for ( unsigned int j = 0; j < gamData->size(); j++ )
                {
                    FilldTNoBGOGammaORRUBA ( & ( siData->at ( i ) ), & ( gamData->at ( j ) ) );

                    FillGsNoBGOGateORRUBA ( & ( siData->at ( i ) ), & ( gamData->at ( j ) ) );
                }
            }
        }

//         for ( unsigned int j = 0; j < std::min ( gamData->size(), siData->size() ); j++ )
//         {
//             FillGsGateORRUBA ( & ( siData->at ( j ) ), & ( gamData->at ( j ) ) );
//         }

        eventNbr++;
    }

    std::cout << "\n\n";

    return;
}





