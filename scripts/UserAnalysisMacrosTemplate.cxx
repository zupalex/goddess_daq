// UserAnalysisTemplate version 161117 //

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

std::map<string, std::pair<TObject*, std::vector<GamData*>>> histsMap;
std::vector<string> specialHists;

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

void ResetHistsStates ( bool all = false )
{
    if ( all )
    {
        for ( auto itr = histsMap.begin(); itr != histsMap.end(); itr++ )
        {
            ( itr->second ).second.clear();
        }
    }

    else
    {
        for ( unsigned int i = 0; i < specialHists.size(); i++ )
        {
            histsMap[specialHists[i]].second.clear();
        }
    }
}

bool CheckHistState ( string histName, GamData* gD )
{
    return ( std::find ( histsMap[histName].second.begin(), histsMap[histName].second.end(), gD ) != histsMap[histName].second.end() );
}

bool CheckHistState ( TObject* hist, GamData* gD )
{
    string histName = ( string ) hist->GetName();

    return CheckHistState ( histName, gD );
}

void AddToHistState ( string histName, GamData* gD )
{
    histsMap[histName].second.push_back ( gD );

    return;
}

void AddToHistState ( TObject* hist, GamData* gD )
{
    string histName = ( string ) hist->GetName();

    AddToHistState ( histName, gD );

    return;
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

void PrintHistsMapContent()
{
    std::cout << "List of Histograms: \n\n";

    for ( auto itr = histsMap.begin(); itr != histsMap.end(); itr++ )
    {
        std::cout << "-> " << itr->first << "\n";
    }

    return;
}

TH1F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX, bool addToSpecialList = false )
{
    TH1F* newHist = new TH1F ( name.c_str(), title.c_str(), nBinsX, minX, maxX );
    histsMap[name] = std::make_pair ( newHist, * ( new std::vector<GamData*> ) );

    if ( addToSpecialList ) specialHists.push_back ( name );

    return newHist;
}

TH2F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY, bool addToSpecialList = false )
{
    TH2F* newHist = new TH2F ( name.c_str(), title.c_str(), nBinsX, minX, maxX, nBinsY, minY, maxY );
    histsMap[name] = std::make_pair ( newHist, * ( new std::vector<GamData*> ) );

    if ( addToSpecialList ) specialHists.push_back ( name );

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
        TH1F* h1 = dynamic_cast<TH1F*> ( ( itr->second ).first );
        TH2F* h2 = dynamic_cast<TH2F*> ( ( itr->second ).first );

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

    return ( isDigital && gsTs - orrubaTs > 180 && gsTs - orrubaTs < 220 ) || ( !isDigital && gsTs - orrubaTs > 410 && gsTs - orrubaTs < 450 );
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

bool FillEvsAHist ( SiDataBase* siData_ )
{
    bool filled = false;

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

                filled = true;
            }
            else
            {
                hEvsA_QQQ5U_tot[mult]->Fill ( strip, energy );
                hEvsA_QQQ5U[sector][mult]->Fill ( strip, energy );

                filled = true;
            }
        }
    }

    return filled;
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

TH2F* hGsBGOVetoVsEx_tot;
TH2F* hGsBGOVetoVsEx_SX3U_tot;
TH2F* hGsBGOVetoVsEx_QQQ5U_tot;
TH2F* hGsBGOVetoVsEx_SX3U[12];
TH2F* hGsBGOVetoVsEx_QQQ5U[4];

void InitQvalHist ( unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY )
{
    hQval_tot = MakeNewHist ( "Qvalue_tot", "Q-value tot", nBinsX, minX, maxX );
    hQval_SX3U_tot = MakeNewHist ( "Qvalue_SX3U_tot", "Q-value SX3s Upstream", nBinsX, minX, maxX );
    hQval_QQQ5U_tot = MakeNewHist ( "Qvalue_QQQ5U_tot", "Q-value QQQ5s Upstream", nBinsX, minX, maxX );

    for ( int i = 0; i < 12; i++ )
    {
        hQval_SX3U[i] = MakeNewHist ( Form ( "Qvalue_SX3U%d", i ), Form ( "Qvalue SX3 U%d", i ), nBinsX, minX, maxX );
    }

    for ( int i = 0; i < 4; i++ )
    {
        hQval_QQQ5U[i] = MakeNewHist ( Form ( "Qvalue_QQQ5U%d", i ), Form ( "Qvalue QQQ5 U%d", i ), nBinsX, minX, maxX );
    }

    hEx_tot = MakeNewHist ( "Ex_tot", "Excitation Energy tot", nBinsX, minX, maxX );
    hEx_SX3U_tot = MakeNewHist ( "Ex_SX3U_tot", "Excitation Energy SX3s Upstream", nBinsX, minX, maxX );
    hEx_QQQ5U_tot = MakeNewHist ( "Ex_QQQ5U_tot", "Excitation Energy QQQ5s Upstream", nBinsX, minX, maxX );

    for ( int i = 0; i < 12; i++ )
    {
        hEx_SX3U[i] = MakeNewHist ( Form ( "Ex_SX3U%d", i ), Form ( "Excitation Energy SX3 U%d", i ), nBinsX, minX, maxX );
    }

    for ( int i = 0; i < 4; i++ )
    {
        hEx_QQQ5U[i] = MakeNewHist ( Form ( "Ex_QQQ5U%d", i ), Form ( "Excitation Energy QQQ5 U%d", i ), nBinsX, minX, maxX );
    }

    hGsVsEx_tot = MakeNewHist ( "GsVsEx_tot", "Gamma Energy vs. Excitation Energy tot", nBinsX, minX, maxX, nBinsY, minY, maxY, true );
    hGsVsEx_SX3U_tot = MakeNewHist ( "GsVsEx_SX3U_tot", "Gamma Energy vs. Excitation Energy SX3s Upstream", nBinsX, minX, maxX, nBinsY, minY, maxY, true );
    hGsVsEx_QQQ5U_tot = MakeNewHist ( "GsVsEx_QQQ5U_tot", "Gamma Energy vs. Excitation Energy QQQ5s Upstream", nBinsX, minX, maxX, nBinsY, minY, maxY, true );

    for ( int i = 0; i < 12; i++ )
    {
        hGsVsEx_SX3U[i] = MakeNewHist ( Form ( "GsVsEx_SX3U%d", i ), Form ( "Gamma Energy vs. Excitation Energy SX3 U%d", i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
    }

    for ( int i = 0; i < 4; i++ )
    {
        hGsVsEx_QQQ5U[i] = MakeNewHist ( Form ( "GsVsEx_QQQ5U%d", i ), Form ( "Gamma Energy vs. Excitation Energy QQQ5 U%d", i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
    }

    hGsBGOVetoVsEx_tot = MakeNewHist ( "GsBGOVetoVsEx_tot", "Gamma Energy BGO Veto vs. Excitation Energy tot", nBinsX, minX, maxX, nBinsY, minY, maxY, true );
    hGsBGOVetoVsEx_SX3U_tot = MakeNewHist ( "GsBGOVetoVsEx_SX3U_tot", "Gamma Energy BGO Veto vs. Excitation Energy SX3s Upstream", nBinsX, minX, maxX, nBinsY, minY, maxY, true );
    hGsBGOVetoVsEx_QQQ5U_tot = MakeNewHist ( "GsBGOVetoVsEx_QQQ5U_tot", "Gamma Energy BGO Veto vs. Excitation Energy QQQ5s Upstream", nBinsX, minX, maxX, nBinsY, minY, maxY, true );

    for ( int i = 0; i < 12; i++ )
    {
        hGsBGOVetoVsEx_SX3U[i] = MakeNewHist ( Form ( "GsBGOVetoVsEx_SX3U%d", i ), Form ( "Gamma Energy BGO Veto vs. Excitation Energy SX3 U%d", i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
    }

    for ( int i = 0; i < 4; i++ )
    {
        hGsBGOVetoVsEx_QQQ5U[i] = MakeNewHist ( Form ( "GsBGOVetoVsEx_QQQ5U%d", i ), Form ( "Gamma Energy BGO Veto vs. Excitation Energy QQQ5 U%d", i ), nBinsX, minX, maxX, nBinsY, minY, maxY );
    }
}

bool FillQvalHist ( SiDataBase* siData_, float massBeam, float massTarget, float massRecoil, float massEjec, float beamEk, float qValGsGs )
{
    bool filled = false;

    float angle = siData_->Angle ( 1 );
    float qval = siData_->QValue ( massBeam, beamEk, massTarget, massEjec );

    float exEn = -qval + qValGsGs;

    unsigned int sector = siData_->sector;

    double siEn = siData_->ESumLayer ( 1, false );

    string histsBaseName = "Qvalue_";

    if ( siData_->isBarrel ) histsBaseName += "SX3";
    else histsBaseName += "QQQ5";

    if ( siData_->isUpstream ) histsBaseName += "U";
    else histsBaseName += "D";

    TH1F* hQvalBorE = ( TH1F* ) histsMap[ ( string ) ( histsBaseName + "_tot" )].first;

    string histsSectorName = Form ( "%s%d", histsBaseName.c_str(), sector );
    TH1F* hQvalSector = ( TH1F* ) histsMap[histsSectorName].first;

    if ( siData_->isUpstream && angle != 0 && siEn > 0 )
    {
        hQvalSector->Fill ( qval );

        hQval_tot->Fill ( qval );

        hQvalBorE->Fill ( qval );

        filled = true;
    }

    return filled;
}

bool FillGsVsExHist ( GamData* gamData_, SiDataBase* siData_, bool vetoBGO, float massBeam, float massTarget, float massRecoil, float massEjec, float beamEk, float qValGsGs )
{
    bool filled = false;

    float angle = siData_->Angle ( 1 );
    float qval = siData_->QValue ( massBeam, beamEk, massTarget, massEjec );

    float exEn = -qval + qValGsGs;

    unsigned int sector = siData_->sector;

    unsigned long long int orrubaTs = siData_->TimestampMaxLayer ( 1, false );

    unsigned long long int gsTs = gamData_->time;

    double gsEn = ( double ) gamData_->en/3.;
    double siEn = siData_->ESumLayer ( 1, false );

    bool isDigital = OrrubaIsDigital ( siData_ );

    bool timestampOK = OrrubaGsGoodTs ( siData_, gamData_ );

    bool sectorOK = OrrubaGoodSector ( siData_ );

    bool energyOK = OrrubaEnergyThr ( siData_ );

    bool doFill = timestampOK && sectorOK && energyOK;

    string histsBaseName;

    if ( vetoBGO ) histsBaseName = "GsBGOVetoVsEx_";
    else histsBaseName = "GsVsEx_";
    TH2F* gsVsExtot = ( TH2F* ) histsMap[ ( string ) ( histsBaseName + "tot" )].first;

    if ( siData_->isBarrel ) histsBaseName += "SX3";
    else histsBaseName += "QQQ5";

    if ( siData_->isUpstream ) histsBaseName += "U";
    else histsBaseName += "D";
    TH2F* gsVsExU = ( TH2F* ) histsMap[ ( string ) ( histsBaseName + "_tot" )].first;

    string histsSectorName = Form ( "%s%d", histsBaseName.c_str(), sector );
    TH2F* gsVsExSector = ( TH2F* ) histsMap[histsSectorName].first;

    if ( doFill && siData_->isUpstream )
    {
        gsVsExSector->Fill ( exEn, gsEn );

        if ( !CheckHistState ( gsVsExU, gamData_ ) )
        {
            gsVsExU->Fill ( exEn, gsEn );
            AddToHistState ( gsVsExU, gamData_ );
        }

        if ( !CheckHistState ( gsVsExtot, gamData_ ) )
        {
            gsVsExtot->Fill ( exEn, gsEn );
            AddToHistState ( gsVsExtot, gamData_ );
        }

        filled = true;
    }

    return filled;
}

// ------------- Time difference between GammaSphere and ORRUBA ----------- //

TH1F* dTGsSX3D[12];
TH1F* dTGsSX3U[12];
TH1F* dTGsQQQ5D[4];
TH1F* dTGsQQQ5U[4];


TH1F* dTGsBGOVetoSX3D[12];
TH1F* dTGsBGOVetoSX3U[12];
TH1F* dTGsBGOVetoQQQ5D[4];
TH1F* dTGsBGOVetoQQQ5U[4];

void InitdTGsORRUBAHists ( unsigned int nBinsX = 1000, int minX = -500, int maxX = 500 )
{
    for ( int i = 0; i < 12; i++ )
    {
        dTGsSX3U[i] = MakeNewHist ( Form ( "dT_GS_SX3U%d", i ), Form ( "dT GS SX3 U%d", i ), nBinsX, minX, maxX );

        dTGsSX3D[i] = MakeNewHist ( Form ( "dT_GS_SX3D%d", i ), Form ( "dT GS SX3 D%d", i ), nBinsX, minX, maxX );

        dTGsBGOVetoSX3U[i] = MakeNewHist ( Form ( "dT_GS_BGOVeto_SX3U%d", i ), Form ( "dT GS No BGO SX3 U%d", i ), nBinsX, minX, maxX );

        dTGsBGOVetoSX3D[i] = MakeNewHist ( Form ( "dT_GS_BGOVeto_SX3D%d", i ), Form ( "dT GS No BGO SX3 D%d", i ), nBinsX, minX, maxX );
    }

    for ( int i = 0; i < 4; i++ )
    {
        dTGsQQQ5U[i] = MakeNewHist ( Form ( "dT_GS_QQQ5U%d", i ), Form ( "dT GS QQQ5 U%d", i ), nBinsX, minX, maxX );

        dTGsQQQ5D[i] = MakeNewHist ( Form ( "dT_GS_QQQ5D%d", i ), Form ( "dT GS QQQ5 D%d", i ), nBinsX, minX, maxX );

        dTGsBGOVetoQQQ5U[i] = MakeNewHist ( Form ( "dT_GS_BGOVeto_QQQ5U%d", i ), Form ( "dT GS No BGO QQQ5 U%d", i ), nBinsX, minX, maxX );

        dTGsBGOVetoQQQ5D[i] = MakeNewHist ( Form ( "dT_GS_BGOVeto_QQQ5D%d", i ), Form ( "dT GS No BGO QQQ5 D%d", i ), nBinsX, minX, maxX );
    }
}

bool FilldTGammaORRUBA ( SiDataBase* siData_, GamData* gamData_, bool vetoBGO = false )
{
    bool filled = false;

    if ( gamData_->type == 2 ) return false;

    unsigned long long int orrubaTs = siData_->TimestampMaxLayer ( 1, false );

    int sector = siData_->sector;

    unsigned long long int gsTs = gamData_->time;

    string histsBaseName = "dT_GS_";

    if ( vetoBGO ) histsBaseName += "BGOVeto_";

    if ( siData_->isBarrel ) histsBaseName += "SX3";
    else histsBaseName += "QQQ5";

    if ( siData_->isUpstream ) histsBaseName += "U";
    else histsBaseName += "D";

    string histsSectorName = Form ( "%s%d", histsBaseName.c_str(), sector );
    TH1F* dThist = ( TH1F* ) histsMap[histsSectorName].first;

    dThist->Fill ( gsTs - orrubaTs );

    filled = true;

    return filled;
}

// -------------------- GammaSphere gated by ORRUBA ------------------------ //

TH1F* gsGatedSX3U[12];
TH1F* gsGatedSX3U_tot;
TH1F* gsGatedSX3U_analog;
TH1F* gsGatedSX3U_digital;

TH1F* gsGatedSX3D[12];
TH1F* gsGatedSX3D_tot;
TH1F* gsGatedSX3D_analog;
TH1F* gsGatedSX3D_digital;

TH1F* gsGatedQQQ5U[4];
TH1F* gsGatedQQQ5U_tot;
TH1F* gsGatedQQQ5U_analog;
TH1F* gsGatedQQQ5U_digital;

TH1F* gsGatedQQQ5D[4];
TH1F* gsGatedQQQ5D_tot;
TH1F* gsGatedQQQ5D_analog;
TH1F* gsGatedQQQ5D_digital;

TH1F* gsBGOVetoGatedSX3U[12];
TH1F* gsBGOVetoGatedSX3U_tot;
TH1F* gsBGOVetoGatedSX3U_analog;
TH1F* gsBGOVetoGatedSX3U_digital;

TH1F* gsBGOVetoGatedSX3D[12];
TH1F* gsBGOVetoGatedSX3D_tot;
TH1F* gsBGOVetoGatedSX3D_analog;
TH1F* gsBGOVetoGatedSX3D_digital;

TH1F* gsBGOVetoGatedQQQ5U[4];
TH1F* gsBGOVetoGatedQQQ5U_tot;
TH1F* gsBGOVetoGatedQQQ5U_analog;
TH1F* gsBGOVetoGatedQQQ5U_digital;

TH1F* gsBGOVetoGatedQQQ5D[4];
TH1F* gsBGOVetoGatedQQQ5D_tot;
TH1F* gsBGOVetoGatedQQQ5D_analog;
TH1F* gsBGOVetoGatedQQQ5D_digital;

TH1F* gsBGOVetoNoTimeGate;

void InitGsGateORRUBAHists ( unsigned int nBinsX = 5000, int minX = 0, int maxX = 5000 )
{
    for ( int i = 0; i < 12; i++ )
    {
        gsGatedSX3U[i] = MakeNewHist ( Form ( "Gs_Gates_SX3U%d", i ), Form ( "GammaSphere Gates SX3 U%d", i ), nBinsX, minX, maxX );
    }

    gsGatedSX3U_tot = MakeNewHist ( "Gs_Gates_SX3U", "GammaSphere Gates SX3 Upstream", nBinsX, minX, maxX, true );
    gsGatedSX3U_analog = MakeNewHist ( "Gs_Gates_SX3U_Analog", "GammaSphere Gates SX3 Upstream Analog", nBinsX, minX, maxX, true );
    gsGatedSX3U_digital = MakeNewHist ( "Gs_Gates_SX3U_Digital", "GammaSphere Gates SX3 Upstream Digital", nBinsX, minX, maxX, true );

    for ( int i = 0; i < 12; i++ )
    {
        gsGatedSX3D[i] = MakeNewHist ( Form ( "Gs_Gates_SX3D%d", i ), Form ( "GammaSphere Gates SX3 D%d", i ), nBinsX, minX, maxX );
    }

    gsGatedSX3D_tot = MakeNewHist ( "Gs_Gates_SX3D", "GammaSphere Gates SX3 Downstream", nBinsX, minX, maxX, true );
    gsGatedSX3D_analog = MakeNewHist ( "Gs_Gates_SX3D_Analog", "GammaSphere Gates SX3 Downstream Analog", nBinsX, minX, maxX, true );
    gsGatedSX3D_digital = MakeNewHist ( "Gs_Gates_SX3D_Digital", "GammaSphere Gates SX3 Downstream Digital", nBinsX, minX, maxX, true );

    for ( int i = 0; i < 4; i++ )
    {
        gsGatedQQQ5U[i] = MakeNewHist ( Form ( "Gs_Gates_QQQ5U%d", i ), Form ( "GammaSphere Gates QQQ5 U%d", i ), nBinsX, minX, maxX );
    }

    gsGatedQQQ5U_tot = MakeNewHist ( "Gs_Gates_QQQ5U", "GammaSphere Gates QQQ5 Upstream", nBinsX, minX, maxX, true );
    gsGatedQQQ5U_analog = MakeNewHist ( "Gs_Gates_QQQ5U_Analog", "GammaSphere Gates QQQ5 Upstream Analog", nBinsX, minX, maxX, true );
    gsGatedQQQ5U_digital = MakeNewHist ( "Gs_Gates_QQQ5U_Digital", "GammaSphere Gates QQQ5 Upstream Digital", nBinsX, minX, maxX, true );

    for ( int i = 0; i < 4; i++ )
    {
        gsGatedQQQ5D[i] = MakeNewHist ( Form ( "Gs_Gates_QQQ5D%d", i ), Form ( "GammaSphere Gates QQQ5 D%d", i ), nBinsX, minX, maxX );
    }

    gsGatedQQQ5D_tot = MakeNewHist ( "Gs_Gates_QQQ5D", "GammaSphere Gates QQQ5 Downstream", nBinsX, minX, maxX, true );
    gsGatedQQQ5D_analog = MakeNewHist ( "Gs_Gates_QQQ5D_Analog", "GammaSphere Gates QQQ5 Downstream Analog", nBinsX, minX, maxX, true );
    gsGatedQQQ5D_digital = MakeNewHist ( "Gs_Gates_QQQ5D_Digital", "GammaSphere Gates QQQ5 Downstream Digital", nBinsX, minX, maxX, true );


    for ( int i = 0; i < 12; i++ )
    {
        gsBGOVetoGatedSX3U[i] = MakeNewHist ( Form ( "Gs_BGOVeto_Gates_SX3U%d", i ), Form ( "GammaSphere BGO Veto SX3 U%d", i ), nBinsX, minX, maxX );
    }

    gsBGOVetoGatedSX3U_tot = MakeNewHist ( "Gs_BGOVeto_Gates_SX3U", "GammaSphere BGO Veto SX3 Upstream", nBinsX, minX, maxX, true );
    gsBGOVetoGatedSX3U_analog = MakeNewHist ( "Gs_BGOVeto_Gates_SX3U_Analog", "GammaSphere BGO Veto SX3 Upstream Analog", nBinsX, minX, maxX, true );
    gsBGOVetoGatedSX3U_digital = MakeNewHist ( "Gs_BGOVeto_Gates_SX3U_Digital", "GammaSphere BGO Veto SX3 Upstream Digital", nBinsX, minX, maxX, true );

    for ( int i = 0; i < 12; i++ )
    {
        gsBGOVetoGatedSX3D[i] = MakeNewHist ( Form ( "Gs_BGOVeto_Gates_SX3D%d", i ), Form ( "GammaSphere BGO Veto SX3 D%d", i ), nBinsX, minX, maxX );
    }

    gsBGOVetoGatedSX3D_tot = MakeNewHist ( "Gs_BGOVeto_Gates_SX3D", "GammaSphere BGO Veto SX3 Downstream", nBinsX, minX, maxX, true );
    gsBGOVetoGatedSX3D_analog = MakeNewHist ( "Gs_BGOVeto_Gates_SX3D_Analog", "GammaSphere BGO Veto SX3 Downstream Analog", nBinsX, minX, maxX, true );
    gsBGOVetoGatedSX3D_digital = MakeNewHist ( "Gs_BGOVeto_Gates_SX3D_Digital", "GammaSphere BGO Veto SX3 Downstream Digital", nBinsX, minX, maxX, true );

    for ( int i = 0; i < 4; i++ )
    {
        gsBGOVetoGatedQQQ5U[i] = MakeNewHist ( Form ( "Gs_BGOVeto_Gates_QQQ5U%d", i ), Form ( "GammaSphere BGO Veto QQQ5 U%d", i ), nBinsX, minX, maxX );
    }

    gsBGOVetoGatedQQQ5U_tot = MakeNewHist ( "Gs_BGOVeto_Gates_QQQ5U", "GammaSphere BGO Veto QQQ5 Upstream", nBinsX, minX, maxX, true );
    gsBGOVetoGatedQQQ5U_analog = MakeNewHist ( "Gs_BGOVeto_Gates_QQQ5U_Analog", "GammaSphere BGO Veto QQQ5 Upstream Analog", nBinsX, minX, maxX, true );
    gsBGOVetoGatedQQQ5U_digital = MakeNewHist ( "Gs_BGOVeto_Gates_QQQ5U_Digital", "GammaSphere BGO Veto QQQ5 Upstream Digital", nBinsX, minX, maxX, true );

    for ( int i = 0; i < 4; i++ )
    {
        gsBGOVetoGatedQQQ5D[i] = MakeNewHist ( Form ( "Gs_BGOVeto_Gates_QQQ5D%d", i ), Form ( "GammaSphere BGO Veto QQQ5 D%d", i ), nBinsX, minX, maxX );
    }

    gsBGOVetoGatedQQQ5D_tot = MakeNewHist ( "Gs_BGOVeto_Gates_QQQ5D", "GammaSphere BGO Veto QQQ5 Downstream", nBinsX, minX, maxX, true );
    gsBGOVetoGatedQQQ5D_analog = MakeNewHist ( "Gs_BGOVeto_Gates_QQQ5D_Analog", "GammaSphere BGO Veto QQQ5 Downstream Analog", nBinsX, minX, maxX, true );
    gsBGOVetoGatedQQQ5D_digital = MakeNewHist ( "Gs_BGOVeto_Gates_QQQ5D_Digital", "GammaSphere BGO Veto QQQ5 Downstream Digital", nBinsX, minX, maxX, true );


    gsBGOVetoNoTimeGate = MakeNewHist ( "Gs_NoTS_BGOVeto_Gates_QQQ5U", "GammaSphere No Timestamp gates, BGO Veto QQQ5 Downstream Digital", nBinsX, minX, maxX, true );
}

bool FillGsGateORRUBA ( SiDataBase* siData_, GamData* gamData_, bool vetoBGO = false )
{
    bool filled = false;

    unsigned long long int orrubaTs = siData_->TimestampMaxLayer ( 1, false );

    int sector = siData_->sector;

    unsigned long long int gsTs = gamData_->time;

    double gsEn = ( double ) gamData_->en/3.;
    double siEn = siData_->ESumLayer ( 1, false );

    bool isDigital = OrrubaIsDigital ( siData_ );

    bool timestampOK = OrrubaGsGoodTs ( siData_, gamData_ );

    bool sectorOK = OrrubaGoodSector ( siData_ );

    bool energyOK = OrrubaEnergyThr ( siData_ );

    string histsBaseName;

    if ( vetoBGO ) histsBaseName = "Gs_BGOVeto_Gates_";
    else histsBaseName = "Gs_Gates_";

    if ( siData_->isBarrel ) histsBaseName += "SX3";
    else histsBaseName += "QQQ5";

    if ( siData_->isUpstream ) histsBaseName += "U";
    else histsBaseName += "D";
    TH1F* gsTot = ( TH1F* ) histsMap[histsBaseName].first;

    string histsAorDName = histsBaseName + ( isDigital ? "_Digital" : "_Analog" );
    TH1F* gsDorA = ( TH1F* ) histsMap[histsAorDName].first;

    string histsSectorName = Form ( "%s%d", histsBaseName.c_str(), sector );
    TH1F* gsSector = ( TH1F* ) histsMap[histsSectorName].first;

    if ( sectorOK && timestampOK && energyOK )
    {
        gsSector->Fill ( gsEn );

        if ( !CheckHistState ( gsDorA, gamData_ ) )
        {
            gsDorA->Fill ( gsEn );
            AddToHistState ( gsDorA, gamData_ );
        }

        if ( !CheckHistState ( gsTot, gamData_ ) )
        {
            gsTot->Fill ( gsEn );
            AddToHistState ( gsTot, gamData_ );
        }

        filled = true;
    }

    return filled;
}

// --------- Put here the functions you want to be processed ---------------- //

void FillUserHists ( unsigned long long int maxEvents = 0 )
{
    std::vector<SiDataBase>* siData = new std::vector<SiDataBase>();
    std::vector<GamData>* gamData = new std::vector<GamData>();

    std::vector<bool> filledGam, filledGamBGOVeto;

    uChain->SetBranchAddress ( "si", &siData );
    uChain->SetBranchAddress ( "gam", &gamData );

    InitdTGsORRUBAHists();
    InitGsGateORRUBAHists();
    InitEvsAHist ( 1800, 0, 180, 1000, 0, 10 );
    InitQvalHist ( 800, -20, 20, 5000, 0, 5000 );

    PrintHistsMapContent();

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

        ResetHistsStates();

        for ( unsigned int i = 0; i < siData->size(); i++ )
        {
            float angle = siData->at ( i ).Angle ( 1 );
            float qval = siData->at ( i ).QValue ( massBeam, beamEk, massTarget, massEjec, massRecoil );

            float ex = -qval + qValGsGs;

            unsigned int sector = siData->at ( i ).sector;

            unsigned long long int orrubaTs = siData->at ( i ).TimestampMaxLayer ( 1, false );

            double siEn = siData->at ( i ).ESumLayer ( 1, false );

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

                FillGsVsExHist ( & ( gamData->at ( j ) ), & ( siData->at ( i ) ),false, massBeam, massTarget, massRecoil, massEjec, beamEk, qValGsGs );

                if ( gamData->at ( j ).type == 1 )
                {
                    FilldTGammaORRUBA ( & ( siData->at ( i ) ), & ( gamData->at ( j ) ), false );

                    FillGsGateORRUBA ( & ( siData->at ( i ) ), & ( gamData->at ( j ) ), false );
                }
                else noBGO = false;
            }

            if ( noBGO )
            {
                for ( unsigned int j = 0; j < gamData->size(); j++ )
                {
                    FilldTGammaORRUBA ( & ( siData->at ( i ) ), & ( gamData->at ( j ) ), true );

                    FillGsGateORRUBA ( & ( siData->at ( i ) ), & ( gamData->at ( j ) ), true );
                }
            }
        }

        eventNbr++;
    }

    std::cout << "\n\n";

    return;
}






