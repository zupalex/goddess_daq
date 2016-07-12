#ifndef __GODDESSCALIB__
#define __GODDESSCALIB__

#include "GoddessAnalysis.h"

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
    static void StartSX3EnCalib ( std::string detectorType = "SuperX3", double refEnergy1 = 5.813 );

    std::string currDetType;
    double currRefEn1;

    static void OnClickGetLinesInfo ( );
    static void OnClickUpdateConf();

    static void OnClickPlotEnCalGraphs();
    static void ValidatePlotEnCalGraphs();

    static void OnClickPlotPosCalGraphs();
    static void ValidatePlotPosCalGraphs();

    TGMainFrame* GetControlFrame()
    {
        return controlFrame;
    }

    static TGWindow* FindWindowByName ( std::string winName );
    static TGFrame* FindFrameByName ( TGCompositeFrame* pFrame, std::string frameName );

    template<typename T> static TGFrame* FindFrameByName ( T* pFrame, std::string frameName )
    {
        TGCompositeFrame* tryCast = dynamic_cast<TGCompositeFrame*> ( pFrame );

        if ( tryCast != NULL ) return FindFrameByName ( tryCast, frameName );
        else return nullptr;
    }

    std::map<std::string, std::map<unsigned short, std::vector<double>>> resStripsCalMap;

    std::map<std::string, TGraph*> resStripsEnCalGraphsMap;
    std::map<std::string, TH2F*> resStripsPosCalGraphsMap;

    void InitializeCalMapKey ( std::string mapKey, unsigned short strip );

    static void PlotSX3ResStripsCalGraphsFromTree();
    static void PlotSX3ResStripsCalGraphs();
    static void Help();
    static void EnCalibHelp();
    static void PosCalibHelp();

    void DrawSX3EnCalGraph ( bool isUpstream, unsigned short sector, unsigned short strip );
    void DrawSX3PosCalHist ( bool isUpstream, unsigned short sector, unsigned short strip, std::string drawOpts );

    TGraph* GetSX3EnCalGraph ( bool isUpstream, unsigned short sector, unsigned short strip );
    TH2F* GetSX3PosCalHist ( bool isUpstream, unsigned short sector, unsigned short strip );

    void GetCornersCoordinates ( TCanvas* can, bool isUpstream, unsigned short sector, unsigned short strip, std::string detectorType = "SuperX3", double refEnergy1 = 5.813 );

    bool DumpFileToResCalMap ( std::string fileName = "Resistive_Strips_Calib_Params.txt" );

    void WriteResCalResults ( std::string fileName = "Resistive_Strips_Calib_Params.txt", std::string mode = "update" );

    bool UpdateResParamsInConf ( std::string configFile, bool invertContactMidDet = true, std::string mode = "protected" );
    static void PopupInputfield ( TGWindow* pWin, short textSize, std::string label, TGLayoutHints* layHints, bool isEditable );

    TGraph* PlotSX3ResStripCalGraph ( TTree* tree, std::string varToPlot, unsigned short sector, unsigned short strip, std::string conditions );

    TGraph* PlotSX3ResStripCalGraph ( TTree* tree, bool isUpstream, unsigned short sector, unsigned short strip );

    template<typename T> inline void PlotSX3ResStripsCalGraphs ( TTree* tree, std::string varToPlot, std::string conditions, T sector );

    template<typename First, typename... Rest> inline void PlotSX3ResStripsCalGraphs ( TTree* tree, std::string varToPlot, std::string conditions, First fstSector, Rest... otherSectors );

    template<typename First, typename... Rest> inline void PlotSX3ResStripsCalGraphs ( TTree* tree, bool isUpstream, First fstSector, Rest... otherSectors );

    void PlotSX3ResStripsCalGraphsFromTree ( TTree* tree, long int nentries, bool isUpstream_, std::vector<unsigned short> sectorsList );
    template<typename FirstSector, typename... VarArgs> inline void PlotSX3ResStripsCalGraphsFromTree ( TTree* tree, bool isUpstream_, long int nentries, FirstSector fstSector, VarArgs... sectors );

    TGraph* DrawPosCalGraph ( TTree* tree, bool isUpstream_, int nentries, unsigned short sector_, unsigned short strip_ );

    template<typename T> inline void DrawPosCal ( TTree* tree, bool isUpstream_, unsigned short sector_, unsigned short strip_, int nentries, T* drawResult );

    TH2F* DrawPosCalHist ( TTree* tree, bool isUpstream_, int nentries,
                           int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts,
                           unsigned short sector_, unsigned short strip_ );

    std::map<std::string, TH2F*> DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries,
            int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts, unsigned short sector_ );

    std::map<std::string, TH2F*> DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries,
            int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts,
            std::vector<unsigned short> sectorsList);
    
    template<typename First, typename... Rest> inline std::map<std::string, TH2F*> DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries,
            int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts,
            First fstSector, Rest... otherSectors );

    int GetPosCalEnBinMax ( TH2F* input );

    TH1D* GetPosCalProjX ( TH2F* input, int projCenter, int projWidth );

    TF1* FitLeftEdge ( TH2F* input, int projWidth = 200 );
    TF1* FitRightEdge ( TH2F* input, int projWidth = 200 );
    void GetStripsEdges ( int projWidth = 200, bool drawResults = true );
    void GetStripsEdges ( TH2F* input, int projWidth = 200, bool drawResults = true );
    void GetStripsEdges ( TFile* input, int projWidth = 200, bool drawResults = true );

    void WritePosCalHistsToFile ( TTree* tree, std::string fileName );

    ClassDef ( GoddessCalib, 1 )
};

template<typename T> inline void GoddessCalib::PlotSX3ResStripsCalGraphs ( TTree* tree, std::string varToPlot, std::string conditions, T sector )
{
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 0, conditions );
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 1, conditions );
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 2, conditions );
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 3, conditions );

    return;
}

template<typename First, typename... Rest> inline void GoddessCalib::PlotSX3ResStripsCalGraphs ( TTree* tree, std::string varToPlot, std::string conditions, First fstSector, Rest... otherSectors )
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
        std::cerr << "One of the sector specified has an invalid type! (expecting positive integer)" << std::endl;
    }

    return;
}

template<typename First, typename... Rest> inline void GoddessCalib::PlotSX3ResStripsCalGraphs ( TTree* tree, bool isUpstream, First fstSector, Rest... otherSectors )
{
    std::string upstreamCond = isUpstream ? "si.isUpstream" : "!si.isUpstream" ;
    std::string cond = "si.isBarrel && " + upstreamCond;

    PlotSX3ResStripsCalGraphs<First, Rest...> ( tree, "si.E1.en.n:si.E1.en.p", cond, fstSector, otherSectors... );

    return;
}

template<typename FirstSector, typename... VarArgs> inline void GoddessCalib::PlotSX3ResStripsCalGraphsFromTree ( TTree* tree, bool isUpstream_, long int nentries, FirstSector fstSector, VarArgs... sectors )
{
    std::vector<unsigned short> sectorsList;

    GetListOfSectorsToTreat<FirstSector, VarArgs...> ( &sectorsList, fstSector, sectors... );
    
    PlotSX3ResStripsCalGraphsFromTree(tree, nentries, isUpstream_, sectorsList);
}

template<typename T> void GoddessCalib::DrawPosCal ( TTree* tree, bool isUpstream_, unsigned short sector_, unsigned short strip_, int nentries, T* drawResult )
{
    std::vector<SiDataDetailed>* siDataVect = new std::vector<SiDataDetailed>();
    siDataVect->clear();

    tree->SetBranchAddress ( "si", &siDataVect );

    if ( nentries > tree->GetEntries() || nentries == 0 ) nentries = tree->GetEntries();

    for ( int i = 0; i < nentries; i++ )
    {
        tree->GetEntry ( i );

        if ( i%10000 == 0 ) std::cout << "Treated " << i << " / " << nentries << " entries ( " << ( ( float ) i ) / ( ( float ) nentries ) * 100. << " % )\r" << std::flush;

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

    std::cout << std::endl;
}

template<typename First, typename... Rest> std::map<std::string, TH2F*> GoddessCalib::DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries,
        int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts,
        First fstSector, Rest... otherSectors )
{
    std::vector<unsigned short> sectorsList;
    sectorsList.clear();

    GetListOfSectorsToTreat<First, Rest...> ( &sectorsList, fstSector, otherSectors... );

    return DrawPosCalHistBatch(tree, isUpstream_, nentries, nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY, drawOpts, sectorsList);
}

#endif
