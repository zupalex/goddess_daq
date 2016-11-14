#ifndef __GODDESSCALIB__
#define __GODDESSCALIB__

#include "GoddessAnalysis.h"

const int minModEndcaps = 40;
const int maxModEndcaps = 160;

extern vector<std::map<int, TH1F*>> hEn_QQQ5UA;
extern vector<std::map<int, TH1F*>> hEn_QQQ5UB;
extern vector<std::map<int, TH1F*>> hEn_QQQ5UC;
extern vector<std::map<int, TH1F*>> hEn_QQQ5UD;

extern vector<std::map<int, TH1F*>> hQVal_QQQ5UA;
extern vector<std::map<int, TH1F*>> hQVal_QQQ5UB;
extern vector<std::map<int, TH1F*>> hQVal_QQQ5UC;
extern vector<std::map<int, TH1F*>> hQVal_QQQ5UD;

extern TH2F* hEn_vs_strip_QQQ5UA;
extern TH2F* hEn_vs_strip_QQQ5UB;
extern TH2F* hEn_vs_strip_QQQ5UC;
extern TH2F* hEn_vs_strip_QQQ5UD;

extern TH2F* hQval_vs_strip_QQQ5UA;
extern TH2F* hQval_vs_strip_QQQ5UB;
extern TH2F* hQval_vs_strip_QQQ5UC;
extern TH2F* hQval_vs_strip_QQQ5UD;

extern TH2F* hEn_vs_strip_QQQ5UA_mod;
extern TH2F* hEn_vs_strip_QQQ5UB_mod;
extern TH2F* hEn_vs_strip_QQQ5UC_mod;
extern TH2F* hEn_vs_strip_QQQ5UD_mod;

extern TH2F* hQval_vs_strip_QQQ5UA_mod;
extern TH2F* hQval_vs_strip_QQQ5UB_mod;
extern TH2F* hQval_vs_strip_QQQ5UC_mod;
extern TH2F* hQval_vs_strip_QQQ5UD_mod;

class GoddessCalib : public GoddessAnalysis
{
private:
    GoddessCalib();

    static GoddessCalib* s_instance;
    TGMainFrame* controlFrame;
    pthread_t checkThread;

public:
    virtual ~GoddessCalib();

    static GoddessCalib* sinstance()
    {
        return s_instance;
    }

    static void DeleteSInstance();

    static GoddessCalib* StartCalib();
    static void StartSX3EnCalib ( string detectorType = "SuperX3", double refEnergy1 = 5.813 );

    string currDetType;
    double currRefEn1;

    static void OnClickParamsButtons ( string mode );
    static void ValidateParamsButtons ( string mode );

    static void OnClickUpdateConf();

    static void OnClickPlotEnCalGraphs();
    static void ValidatePlotEnCalGraphs();

    static void OnClickPlotPosCalGraphs();
    static void ValidatePlotPosCalGraphs();

    static void OnClickGetLinesInfo ( );
    static void OnClickGetStripsEdges();
    static void ValidateGetStripsEdges();

    TGMainFrame* GetControlFrame()
    {
        return controlFrame;
    }

    static TGWindow* FindWindowByName ( string winName );
    static TGFrame* FindFrameByName ( TGCompositeFrame* pFrame, string frameName );

    template<typename T> static TGFrame* FindFrameByName ( T* pFrame, string frameName )
    {
        TGCompositeFrame* tryCast = dynamic_cast<TGCompositeFrame*> ( pFrame );

        if ( tryCast != NULL ) return FindFrameByName ( tryCast, frameName );
        else return nullptr;
    }

    map<string, map<unsigned short, vector<double>>> resStripsCalMap;
    map<string, vector<double>> endcapsStripsCalMap;

    map<string, TH2F*> resStripsEnCalGraphsMap;
    map<string, TH2F*> resStripsPosCalGraphsMap;

    void InitializeCalMapKey ( string mapKey, unsigned short strip );

    static void PlotSX3ResStripsCalGraphsFromTree();
    static void PlotSX3ResStripsCalGraphs();
    static void Help();
    static void EnCalibHelp();
    static void PosCalibHelp();

    void DrawSX3EnCalGraph ( bool isUpstream, unsigned short sector, unsigned short strip );
    void DrawSX3PosCalHist ( bool isUpstream, unsigned short sector, unsigned short strip, string drawOpts );

    TH2F* GetSX3EnCalGraph ( bool isUpstream, unsigned short sector, unsigned short strip );
    TH2F* GetSX3PosCalHist ( bool isUpstream, unsigned short sector, unsigned short strip );

    void GetCornersCoordinates ( TCanvas* can, bool isUpstream, unsigned short sector, unsigned short strip, string detectorType = "SuperX3", double refEnergy1 = 5.813 );

    bool DumpFileToResCalMap ( string fileName = "Resistive_Strips_Calib_Params.txt" );

    void WriteResCalResults ( string fileName = "Resistive_Strips_Calib_Params.txt", string mode = "update" );

    bool UpdateParamsInConf ( string configFile, string detType, bool invertContactMidDet = true, string mode = "protected" );
    static void PopupInputfield ( TGWindow* pWin, short textSize, string label, TGLayoutHints* layHints, bool isEditable );

    TH2F* PlotSX3ResStripCalGraph ( TTree* tree, string varToPlot, unsigned short sector, unsigned short strip, string conditions );

    TH2F* PlotSX3ResStripCalGraph ( TTree* tree, bool isUpstream, unsigned short sector, unsigned short strip );

    template<typename T> inline void PlotSX3ResStripsCalGraphs ( TTree* tree, string varToPlot, string conditions, T sector );

    template<typename First, typename... Rest> inline void PlotSX3ResStripsCalGraphs ( TTree* tree, string varToPlot, string conditions, First fstSector, Rest... otherSectors );

    template<typename First, typename... Rest> inline void PlotSX3ResStripsCalGraphs ( TTree* tree, bool isUpstream, First fstSector, Rest... otherSectors );

    void PlotSX3ResStripsCalGraphsFromTree ( TTree* tree, long int nentries, bool isUpstream_, vector<unsigned short> sectorsList );
    template<typename FirstSector, typename... VarArgs> inline void PlotSX3ResStripsCalGraphsFromTree ( TTree* tree, bool isUpstream_, long int nentries, FirstSector fstSector, VarArgs... sectors );

    TGraph* DrawPosCalGraph ( TTree* tree, bool isUpstream_, int nentries, unsigned short sector_, unsigned short strip_ );

    template<typename T> inline void DrawPosCal ( TTree* tree, bool isUpstream_, unsigned short sector_, unsigned short strip_, int nentries, T* drawResult );

    TH2F* DrawPosCalHist ( TTree* tree, bool isUpstream_, int nentries,
                           int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts,
                           unsigned short sector_, unsigned short strip_ );

    map<string, TH2F*> DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries,
            int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, unsigned short sector_ );

    map<string, TH2F*> DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries,
            int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts,
            vector<unsigned short> sectorsList );

    template<typename First, typename... Rest> inline map<string, TH2F*> DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries,
            int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts,
            First fstSector, Rest... otherSectors );

    int GetPosCalEnBinMax ( TH2F* input, double threshold );

    TH1D* GetPosCalProjX ( TH2F* input, int projCenter, int projWidth );

    TF1* FitLeftEdge ( TH2F* input, int projWidth = 200, double threshold = 250 );
    TF1* FitRightEdge ( TH2F* input, int projWidth = 200, double threshold = 250 );
    void GetStripsEdges ( int projWidth = 200, double threshold = 250, bool drawResults = true );
    void GetStripsEdges ( TH2F* input, int projWidth = 200, double threshold = 250, bool drawResults = true );
    void GetStripsEdges ( TFile* input, int projWidth = 200, double threshold = 250, bool drawResults = true );

    void WritePosCalHistsToFile ( TTree* tree, string fileName );

    // ---------------------- QQQ5 stuffs -------------------------------- //

    TH1F* AddAllStrips ( vector<std::map<int, TH1F*>>* hists, int modCoeff_ = 100 );

    TH1F* AddAllStrips ( vector<std::map<int, TH1F*>>* hists, double modCoeffs[32] );

    TH1F* AddAllStrips ( vector<std::map<int, TH1F*>>* hists, vector<double> modCoeffs );

    TH1F* AddAllStrips ( vector<std::map<int, TH1F*>>* hists, string sector );

    TF1* FitQValGS ( TH1F* hist, float mean, float fwhm, float minBound = 0, float maxBound = 0 );

    TH2F* GetQvalVsStrip ( vector<std::map<int, TH1F*>>* src, TH2F* dest, int modCoeff_ = 100 );

    TH2F* GetQvalVsStrip ( vector<std::map<int, TH1F*>>* src, TH2F* dest, double modCoeffs[32] );

    TH2F* GetQvalVsStrip ( vector<std::map<int, TH1F*>>* src, TH2F* dest, vector<double> modCoeffs );

    TH2F* GetQvalVsStrip ( vector<std::map<int, TH1F*>>* src, TH2F* dest, string sector );

    void InitSiEnergySpectra ( unsigned int nBins, int binMin, int binMax );

    void InitQValSpectra ( unsigned int nBins, int binMin, int binMax );

    void GenerateEnergyHistPerStrip ( TChain* chain );

    vector<double> AdjustQValSpectrum ( vector<std::map<int, TH1F*>>* hists, float peakPos, float fwhm, 
					float minBound = 0, float maxBound = 0, int minModEndcaps_ = minModEndcaps, int maxModEndcaps_ = maxModEndcaps,
                                        /*string chi2Mode = "x <= best", string sigmaMode = "x <= best", string magnMode = "x => best", string integralMode = "x => best", string histIntegralMode = "x => best"*/
				      string betterFitMode = "chi2 <= bestChi2 && sigma <= bestSigma && magn >= bestMagn && gaussIntegral >= bestGaussIntegral && rasIntegral >= bestRawIntegral");

    template<typename First, typename... Rest> void GenerateEnergyHistPerStrip ( string treeName, First fileName1, Rest... fileNameRest );

    void LoadInternalCalib ( string fileName );

    ClassDef ( GoddessCalib, 1 )
};

extern GoddessCalib* gC;

template<typename T> inline void GoddessCalib::PlotSX3ResStripsCalGraphs ( TTree* tree, string varToPlot, string conditions, T sector )
{
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 0, conditions );
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 1, conditions );
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 2, conditions );
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 3, conditions );

    return;
}

template<typename First, typename... Rest> inline void GoddessCalib::PlotSX3ResStripsCalGraphs ( TTree* tree, string varToPlot, string conditions, First fstSector, Rest... otherSectors )
{
    if ( std::is_same<decltype ( fstSector ), int>::value  || ( unsigned short ) fstSector < 0 )
    {
        PlotSX3ResStripsCalGraphs<unsigned short> ( tree, varToPlot, conditions, fstSector );

        if ( sizeof... ( otherSectors ) > 0 )
        {
            PlotSX3ResStripsCalGraphs ( tree, varToPlot, conditions, otherSectors... );
        }
    }
    else
    {
        cerr << "One of the sector specified has an invalid type! (expecting positive integer)" << endl;
    }

    return;
}

template<typename First, typename... Rest> inline void GoddessCalib::PlotSX3ResStripsCalGraphs ( TTree* tree, bool isUpstream, First fstSector, Rest... otherSectors )
{
    string upstreamCond = isUpstream ? "si.isUpstream" : "!si.isUpstream" ;
    string cond = "si.isBarrel && " + upstreamCond;

    PlotSX3ResStripsCalGraphs<First, Rest...> ( tree, "si.E1.en.n:si.E1.en.p", cond, fstSector, otherSectors... );

    return;
}

template<typename FirstSector, typename... VarArgs> inline void GoddessCalib::PlotSX3ResStripsCalGraphsFromTree ( TTree* tree, bool isUpstream_, long int nentries, FirstSector fstSector, VarArgs... sectors )
{
    vector<unsigned short> sectorsList;

    GetListOfSectorsToTreat<FirstSector, VarArgs...> ( &sectorsList, fstSector, sectors... );

    PlotSX3ResStripsCalGraphsFromTree ( tree, nentries, isUpstream_, sectorsList );
}

template<typename T> void GoddessCalib::DrawPosCal ( TTree* tree, bool isUpstream_, unsigned short sector_, unsigned short strip_, int nentries, T* drawResult )
{
    vector<SiDataDetailed>* siDataVect = new vector<SiDataDetailed>();
    siDataVect->clear();

    tree->SetBranchAddress ( "si", &siDataVect );

    if ( nentries > tree->GetEntries() || nentries == 0 ) nentries = tree->GetEntries();

    for ( int i = 0; i < nentries; i++ )
    {
        tree->GetEntry ( i );

        if ( i%10000 == 0 ) cout << "Treated " << i << " / " << nentries << " entries ( " << ( ( float ) i ) / ( ( float ) nentries ) * 100. << " % )\r" << std::flush;

        if ( siDataVect->size() == 0 ) continue;

        for ( unsigned short j = 0; j < siDataVect->size(); j++ )
        {
            SiDataDetailed siData = siDataVect->at ( j );

            if ( siData.isBarrel && siData.isUpstream == isUpstream_ && siData.sector == sector_ && siData.E1.en.p.size() > 0 && siData.E1.en.n.size() > 0 )
            {
                for ( unsigned short k = 0; k < siData.E1.en.p.size(); k++ )
                {
                    if ( siData.E1.strip.p[k] != strip_ ) continue;

                    if ( std::is_same<decltype ( drawResult ), TH2F*>::value )
                    {
                        ( ( TH2F* ) drawResult )->Fill ( ( siData.E1.en.n[k] - siData.E1.en.p[k] ) / ( siData.E1.en.p[k] + siData.E1.en.n[k] ), ( siData.E1.en.p[k] + siData.E1.en.n[k] ) );
                        break;
                    }
                    else if ( std::is_same<decltype ( drawResult ), TGraph*>::value )
                    {
                        ( ( TGraph* ) drawResult )->SetPoint ( ( ( TGraph* ) drawResult )->GetN(), ( siData.E1.en.n[k] - siData.E1.en.p[k] ) / ( siData.E1.en.p[k] + siData.E1.en.n[k] ), ( siData.E1.en.p[k] + siData.E1.en.n[k] ) );
                        break;
                    }
                    else return;
                }
            }
        }
    }

    cout << endl;
}

template<typename First, typename... Rest> map<string, TH2F*> GoddessCalib::DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries,
        int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts,
        First fstSector, Rest... otherSectors )
{
    vector<unsigned short> sectorsList;
    sectorsList.clear();

    GetListOfSectorsToTreat<First, Rest...> ( &sectorsList, fstSector, otherSectors... );

    return DrawPosCalHistBatch ( tree, isUpstream_, nentries, nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY, drawOpts, sectorsList );
}

// ---------------------------------------- QQQ5 functions ----------------------------------------------- //


template<typename First, typename... Rest> void GoddessCalib::GenerateEnergyHistPerStrip ( string treeName, First fileName1, Rest... fileNameRest )
{
    GoddessAnalysis* gA = new GoddessAnalysis();

    gA->InitUserAnalysis ( treeName, fileName1, fileNameRest... );

    TChain* uChain = gA->userChain;

    GenerateEnergyHistPerStrip ( uChain );
}

#endif
