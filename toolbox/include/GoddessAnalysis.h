#ifndef __GODDESSANALYSIS__
#define __GODDESSANALYSIS__

#include "goddess_analysis_macros.h"

class GoddessAnalysis : public TObject, public TQObject
{
private:
    string defaultTreeName1;
    string defaultTreeName2;
    string defaultFileName;

public:
    GoddessAnalysis();
    virtual ~GoddessAnalysis();

    double gainAdjusts[500];

    void SetGainAdjust ( string fName );

    // ---------------- General useful functions -------------------------- //

    static void RegisterClassForROOTSession () {}

    template<typename T> static void DisplayMapKeys ( std::map<string, T> map_ );
    inline static void DisplayMapKeys ( std::map<string, float> map_ );

    inline static int DecodeStripNumber ( int st_ );

    template<typename T> static void GetListOfSectorsToTreat ( vector<unsigned short>* sectorsList, T sector );

    template<typename First, typename... Rest> static void GetListOfSectorsToTreat ( vector<unsigned short>* sectorsList, First fstSector, Rest... otherSectors );

    template<typename First, typename... Rest> TH2F* DrawEnergyVsAngleSX3 ( TChain* chain, int nentries,
            string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, bool isUpstream_, string strips,
            First fstSector, Rest... otherSectors );

    TH2F* DrawEnergyVsAngleSX3 ( TChain* chain, int nentries,
                                 string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, bool isUpstream_, string strips,
                                 unsigned short sector );

    vector<unsigned short> GetStripsListToTreat ( string strips );

    double GetEnergyShiftFromQVal ( double qValShift, double qValRef, double angle );
    double GetEnergyShiftFromEx ( double exShift, double exRef, double angle );

    // ------------------ For the check mapping tool ------------------------ //

    GoddessAnalysis ( string filename );
    GoddessAnalysis ( string filename, string treename, string treename2 );

    void SetDefaultFileAndTrees ( string fileName, string treeName1, string treeName2 );

    void CheckMapping ( string filename, string treename, string treename2, unsigned short channel1, unsigned short channel2, bool Digital, string DetectorID );

    void CheckMapping ( unsigned short channel1, unsigned short channel2, bool Digital, string DetectorID );

    // ****************** Reaction parameters *************************** //

    GoddessGeomInfos* geomInfo;
    GoddessReacInfos* reacInfo;

    void AutoReadAndSetReacParameters();
    void SetReacParameters ( float beamMass_ = 134., float beamEk_ = 1312., float targetMass_ = 2., float ejecMass_ = 1., float recoilMass_ = 135., float qValGsGs_ = 4.14 );
    void SetBeamParameters ( float beamMass_ = 134., float beamEk_ = 1312. );
    void SetBeamEk ( float beamEk_ = 1312. );
    void SetBeamMass ( float beamMass_ = 134. );
    void SetTargetMass ( float targetMass_ = 2. );
    void SetEjectileMass ( float ejecMass_ = 1. );
    void SetRecoilMass ( float recoilMass_ = 135. );
    void SetQvalueGsToGs ( float qvalGsGs_ = 4.14 );

    // ------------------ For the User Macros ------------------------------- //

    TChain* userChain;
    TTree* userTree;
    vector<TTree*> userTreeList;

    void PrintUserTreeList();

    void AddFileToTreat ( TFile* inFile, string treeName );
    void AddFileToTreat ( string inFile, string treeName );
    void AddFileToTreat ( vector<string> inFile, string treeName );

    template<typename T> void InitUserAnalysis ( string treeName, T inFile1 );
    template<typename First, typename... Rest> void InitUserAnalysis ( string treeName, First inFile1, Rest... inFileRest );

    ClassDef ( GoddessAnalysis, 1 )
};

template<typename T> void GoddessAnalysis::InitUserAnalysis ( string treeName, T inFile1 )
{
    AddFileToTreat ( ( string ) inFile1, treeName );
}

template<typename First, typename... Rest> void GoddessAnalysis::InitUserAnalysis ( string treeName, First inFile1, Rest... inFileRest )
{
    InitUserAnalysis<string> ( treeName, inFile1 );

    InitUserAnalysis ( treeName, inFileRest... );
}

template<typename T> void GoddessAnalysis::DisplayMapKeys ( std::map<string, T> map_ )
{
    for ( auto itr = map_.begin(); itr != map_.end(); itr++ )
    {
        cout << itr->first << endl;
    }
}

inline void GoddessAnalysis::DisplayMapKeys ( std::map<string, float> map_ )
{
    for ( auto itr = map_.begin(); itr != map_.end(); itr++ )
    {
        cout << itr->first << endl;
    }
}

inline int GoddessAnalysis::DecodeStripNumber ( int st_ )
{
    return st_%100;
}

template<typename T> void GoddessAnalysis::GetListOfSectorsToTreat ( vector<unsigned short>* sectorsList, T sector )
{
    cout << "adding " << sector << " to the list of sectors to treat..." << endl;

    sectorsList->push_back ( sector );
}

template<typename First, typename... Rest> void GoddessAnalysis::GetListOfSectorsToTreat ( vector<unsigned short>* sectorsList, First fstSector, Rest... otherSectors )
{
    GetListOfSectorsToTreat<unsigned short> ( sectorsList, fstSector );

    GetListOfSectorsToTreat ( sectorsList, otherSectors... );
}

template<typename First, typename... Rest> TH2F* GoddessAnalysis::DrawEnergyVsAngleSX3 ( TChain* chain, int nentries,
        string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, bool isUpstream_, string strips,
        First fstSector, Rest... otherSectors )
{
    vector<unsigned short> sectorsList;
    sectorsList.clear();

    GetListOfSectorsToTreat<First, Rest...> ( &sectorsList, fstSector, otherSectors... );

    vector<unsigned short> stripsList = GetStripsListToTreat ( strips );

    vector<SiDataBase>* siDataVect = new vector<SiDataBase>();
    siDataVect->clear();

    chain->SetBranchAddress ( "si", &siDataVect );

    if ( nentries > chain->GetEntries() || nentries == 0 ) nentries = chain->GetEntries();

    TH2F* hist = new TH2F ( hname.c_str(), hname.c_str(), nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY );

    cout << endl;

    for ( int i = 0; i < nentries; i++ )
    {
        chain->GetEntry ( i );

        if ( i%10000 == 0 ) cout << "Treated " << i << " / " << nentries << " entries ( " << ( ( float ) i ) / ( ( float ) nentries ) * 100. << " % )\r" << std::flush;

        if ( siDataVect->size() == 0 ) continue;

        for ( unsigned short j = 0; j < siDataVect->size(); j++ )
        {
            SiDataBase siData = siDataVect->at ( j );

            if ( siData.pos.size() == 0 ) continue;

            if ( siData.isUpstream == isUpstream_ )
            {
                int sect = -1;

                for ( unsigned short k = 0; k < sectorsList.size(); k++ )
                {
                    if ( siData.sector == sectorsList[k] ) sect = sectorsList[k];
                }

                if ( sect == -1 ) continue;

                int stPos = -1;

                for ( unsigned short st = 0; st < siData.stripMax.size(); st++ )
                {
                    for ( unsigned short k = 0; k < stripsList.size(); k++ )
                    {
                        if ( DecodeStripNumber ( siData.stripMax[st] ) == stripsList[k] ) stPos = st;
                    }
                }

                if ( stPos == -1 ) continue;

                if ( siData.pos[stPos].Mag() > 0.0 && siData.isBarrel )
                {
                    hist->Fill ( -siData.pos[stPos].Angle ( TVector3 ( 0, 0, 1 ) ) *180/TMath::Pi(), siData.eSum[stPos] );
                }
            }
        }
    }

    hist->Draw ( drawOpts.c_str() );

    cout << endl;

    return hist;
}

// -------------------- User Analysis Macros Stuffs --------------------------------------- //

extern GoddessAnalysis* gA;
extern unsigned int ghistcount;

extern std::map<string, std::pair<TObject*, vector<GamData*>>> histsMap;
extern vector<string> specialHists;

inline void LoadTrees()
{
    cout << "To initialize the chain of runs, type:\n   LoadTrees( (string) treeName, (string) fileName1, (string) fileName2, (string) ... )\n\n";
}

template<typename First, typename... Rest> void LoadTrees ( string treeName, First fileName1, Rest... fileNameRest )
{
    if ( gA == nullptr ) gA = new GoddessAnalysis();

    if ( gA->userChain != nullptr ) delete gA->userChain;
    gA->userChain = nullptr;

//     gA->userTreeList.clear();
//
    gA->InitUserAnalysis ( treeName, fileName1, fileNameRest... );
}

void ResetHistsStates ( bool all = false );

bool CheckHistState ( string histName, GamData* gD );

bool CheckHistState ( TObject* hist, GamData* gD );

void AddToHistState ( string histName, GamData* gD );

void AddToHistState ( TObject* hist, GamData* gD );

template<typename THist> THist* DrawHist ( string hname, string opt = "" )
{
    THist* hist = nullptr;

    TObject* obj = gDirectory->FindObjectAny ( hname.c_str() );

    if ( obj != nullptr )
    {
        hist = dynamic_cast<THist*> ( obj );

        if ( hist != nullptr ) hist->Draw ( opt.c_str() );
    }

    return hist;
}

inline TH1F* Draw1DHist ( string hname, string opt = "" )
{
    return DrawHist<TH1F> ( hname, opt );
}

inline TH2F* Draw2DHist ( string hname, string opt = "" )
{
    return DrawHist<TH2F> ( hname, opt );
}

template<typename THist> void AddHists ( THist* h1, THist* h2 )
{
    if ( std::is_same<THist, TH1F>::value || std::is_same<THist, TH2F>::value )
    {
        h1->Add ( h2 );
    }
}

void AddHists ( TH1F* h1, TH1F* h2 );

void AddHists ( TH2F* h1, TH2F* h2 );

template<typename THist, typename... Rest> void AddHists ( THist* h1, THist* h2, Rest... otherHists )
{
    if ( std::is_same<THist, TH1F>::value || std::is_same<THist, TH2F>::value )
    {
        AddHists ( h1, h2 );
        AddHists ( h1, otherHists... );
    }
}

template<typename THist, typename... Rest> THist* DrawSum ( THist* h1, THist* h2, Rest... otherHists )
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

inline TH1F* DrawSum ( TH1F* h1, TH1F* h2 )
{
    TH1F* hist = ( TH1F* ) h1->Clone();

    hist->Add ( h2 );

    hist->Draw();

    return hist;
}

inline TH2F* DrawSum ( TH2F* h1, TH2F* h2 )
{
    TH2F* hist = ( TH2F* ) h1->Clone();

    hist->Add ( h2 );

    hist->Draw ( "colz" );

    return hist;
}

TH1F* DrawSum ( TH1F* h1, TH1F* h2, bool cloneFirst = true, bool doDraw = true );

TH1F* DrawSum ( TH1F** hists, string toSum );

TH2F* DrawSum ( TH2F** hists, string toSum );

TH1F* DrawSum ( TH1F** hists );

template<typename THist> void DrawSum ( string toSum, THist*& dest )
{
    vector<string> histsNames = DecodeItemsToTreat ( toSum, "root" );

    for ( unsigned int i = 0; i < histsNames.size(); i++ )
    {
        cout << histsNames[i] << endl;

        THist* hist = dynamic_cast<THist*> ( gDirectory->FindObjectAny ( histsNames[i].c_str() ) );

        if ( hist != nullptr )
        {
//             cout << "Found it in gDirectory...\n";

            if ( dest == nullptr )
            {
//                 cout << "Destination histogram was NULL. Initializing it...\n";
                dest = ( THist* ) hist->Clone();
            }
            else dest->Add ( hist );
        }
    }

    return;
}

void DrawSum ( string toSum, TH1F*& dest );
void DrawSum ( string toSum, TH2F*& dest );

TGraph* DrawGraphFromDataset ( vector<double> xSet, vector<double> ySet );

TH2F* DrawCombinedKinematics ( string qqq5List, string sX3List );
TGraph* SuperimposeCalculatedKinematics ( string input );

void PrintProgress ( long long int maxEvents_, long long int currEvt, int fileNbr = -1 );

TH1F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX, bool addToSpecialList = false );
TH2F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY, bool addToSpecialList = false );
TH2F* MakeNewHist ( string name, string title, int nBinsX, double* binsX, unsigned int nBinsY, int minY, int maxY, bool addToSpecialList = false );
TH3F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY, unsigned int nBinsZ, int minZ, int maxZ, bool addToSpecialList );
TH3F* MakeNewHist ( string name, string title, int nBinsX, double* binsX, unsigned int nBinsY, int minY, int maxY, unsigned int nBinsZ, int minZ, int maxZ, bool addToSpecialList );

TH1D* DrawGodHist ( TH2F* source, string toDraw, string opt = "" );
TH2D* DrawGodHist ( TH3F* source, string toDraw, string opt = "" );

TVector3 GetDetPos ( GoddessGeomInfos* geomInfo, bool isUpstream_, bool isBarrel_, int sector_, int depth_, int verbose = 0 );
TVector3 GetDetPos ( TChain* c, bool isUpstream_, bool isBarrel_, int sector_, int depth_, int verbose = 0 );
TVector3 GetDetPos ( TTree* tree, bool isUpstream_, bool isBarrel_, int sector_, int depth_, int verbose = 0 );

TVector3 GetStripPos ( GoddessGeomInfos* geomInfo, bool isUpstream_, bool isBarrel_, int sector_, int strip_, int depth_, int verbose = 0 );

vector<double> GetBinsEdges ( GoddessGeomInfos* geomInfo, bool isUpstream_, bool isBarrel_, int sector_, int depth_, int verbose = 0 );

void InsertReacInfo ( TFile* file, GoddessReacInfos* reacInfo_, bool overwriteIfExists = false );
void InsertGeomInfo ( TFile* file, GoddessGeomInfos* geomInfo_, bool overwriteIfExists = false );
void InsertReacInfo ( string files, GoddessReacInfos* reacInfo_, bool overwriteIfExists = false );
void InsertGeomInfo ( string files, GoddessGeomInfos* geomInfo_, bool overwriteIfExists = false );

extern string pathToGDAQ;

void WriteUserHists ( string outName );
void UpdateUserHists ( string outName );
void ClearUserHists();

#endif

































