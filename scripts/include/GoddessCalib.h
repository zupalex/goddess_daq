#ifndef __GODDESSCALIB__
#define __GODDESSCALIB__

#include "GoddessAnalysis.h"

class GoddessCalib : public GoddessAnalysis
{
private:

public:
    GoddessCalib() {}
    virtual ~GoddessCalib() {}

    std::map<std::string, std::map<unsigned short, double[6]>> resStripsCalMap;

    std::map<std::string, TGraph*> resStripsEnCalGraphsMap;
    std::map<std::string, TH2F*> resStripsPosCalGraphsMap;

    void InitializeCalMapKey ( std::string mapKey, unsigned short strip );

    void PlotSX3ResStripsCalGraphsFromTree();
    void PlotSX3ResStripsCalGraphs();
    void CalibHelp();

    void DrawSX3EnCalGraph ( bool isUpstream, unsigned short sector, unsigned short strip );
    void DrawSX3PosCalHist ( bool isUpstream, unsigned short sector, unsigned short strip, std::string drawOpts );

    TGraph* GetSX3EnCalGraph ( bool isUpstream, unsigned short sector, unsigned short strip );
    TH2F* GetSX3PosCalHist ( bool isUpstream, unsigned short sector, unsigned short strip );

    void GetCornersCoordinates ( TCanvas* can, bool isUpstream, unsigned short sector, unsigned short strip, std::string detectorType = "SuperX3", double refEnergy1 = 5.813 );

    bool DumpFileToResCalMap ( std::string fileName = "Resistive_Strips_Calib_Params.txt" );

    void WriteResCalResults ( std::string fileName = "Resistive_Strips_Calib_Params.txt", std::string mode = "update" );

    bool UpdateResParamsInConf ( std::string configFile, bool invertContactMidDet = true, std::string mode = "protected" );

    TGraph* PlotSX3ResStripCalGraph ( TTree* tree, std::string varToPlot, unsigned short sector, unsigned short strip, std::string conditions );

    TGraph* PlotSX3ResStripCalGraph ( TTree* tree, bool isUpstream, unsigned short sector, unsigned short strip );

    template<typename T> inline void PlotSX3ResStripsCalGraphs ( TTree* tree, std::string varToPlot, std::string conditions, T sector );

    template<typename First, typename... Rest> inline void PlotSX3ResStripsCalGraphs ( TTree* tree, std::string varToPlot, std::string conditions, First fstSector, Rest... otherSectors );

    template<typename First, typename... Rest> inline void PlotSX3ResStripsCalGraphs ( TTree* tree, bool isUpstream, First fstSector, Rest... otherSectors );

    template<typename FirstSector, typename... VarArgs> inline void PlotSX3ResStripsCalGraphsFromTree ( TTree* tree, bool isUpstream_, long int nentries, FirstSector fstSector, VarArgs... sectors );

    TGraph* DrawPosCalGraph ( TTree* tree, bool isUpstream_, int nentries, unsigned short sector_, unsigned short strip_ );

    template<typename T> inline void DrawPosCal ( TTree* tree, bool isUpstream_, unsigned short sector_, unsigned short strip_, int nentries, T* drawResult );

    TH2F* DrawPosCalHist ( TTree* tree, bool isUpstream_, int nentries,
                           int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts,
                           unsigned short sector_, unsigned short strip_ );

    std::map<std::string, TH2F*> DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries,
            int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts, unsigned short sector_ );

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

    std::cout << sectorsList.size() << " sectors will be treated..." << std::endl;

    int sizeOfSectors = sectorsList.size();

    int numberOfGraphs = sizeOfSectors*4;

    for ( int i =0; i < sizeOfSectors; i++ )
    {
        for ( int j =0; j < 4; j++ )
        {
            std::string grID = Form ( "sector%s%d_strip%d", isUpstream_ ? "U" : "D", i, j );

            std::cout << "Creating graph " << grID << std::endl;

            TGraph* newGraph = new TGraph();

            newGraph->SetName ( grID.c_str() );
            newGraph->SetTitle ( grID.c_str() );

            resStripsEnCalGraphsMap[Form ( "sector %s%d strip %d", isUpstream_ ? "U" : "D", i, j )] = newGraph;

//             std::cout << "Stored graph in the TGraph map..." << std::endl;
        }
    }

    if ( nentries == 0 || nentries > tree->GetEntries() ) nentries = tree->GetEntries();

    std::cout << nentries << " entries will be treated" <<std::endl;

    std::vector<SiDataDetailed>* siInfo = new std::vector<SiDataDetailed>();
    siInfo->clear();

    std::cout << "Preparing the readout of the tree..." << std::endl;

    tree->SetBranchAddress ( "si", &siInfo );

//     std::cout << "Linked the \"si\" branch to a SiDataDetailed object..." << std::endl;

    for ( int i = 0; i < nentries; i++ )
    {
        tree->GetEntry ( i );

        if ( i%10000 == 0 ) std::cout << "Treated " << i << " / " << nentries << " entries ( " << ( ( float ) i ) / ( ( float ) nentries ) * 100. << "% )\r" << std::flush;

        if ( siInfo->size() == 0 ) continue;

        for ( unsigned short j = 0; j < siInfo->size(); j++ )
        {
            int sectorNbr = -1;

            for ( int k = 0; k < sizeOfSectors; k++ )
            {
                if ( siInfo->at ( j ).sector == sectorsList[k] )
                {
                    sectorNbr = siInfo->at ( j ).sector;
                    break;
                }
            }

            if ( sectorNbr == -1 ) continue;

            if ( siInfo->at ( j ).isBarrel && siInfo->at ( j ).isUpstream == isUpstream_ && siInfo->at ( j ).E1.en.p.size() > 0 )
            {
                for ( unsigned short st = 0; st < siInfo->at ( j ).E1.en.p.size(); st++ )
                {
                    std::string grID = Form ( "sector %s%d strip %d", isUpstream_ ? "U" : "D", sectorNbr, siInfo->at ( j ).E1.strip.p[st] );

                    TGraph* gr = resStripsEnCalGraphsMap[grID];

                    gr->SetPoint ( gr->GetN(), siInfo->at ( j ).E1.en.p[st], siInfo->at ( j ).E1.en.n[st] );
                }
            }
        }
    }

    std::cout << std::endl;

    std::string currPath = ( std::string ) gDirectory->GetPath();

    std::string rootFileName = "Resistive_Strips_Calib_Graphs_";

    std::string treeFName = tree->GetCurrentFile()->GetName();

    std::size_t begRunName = treeFName.find ( "run" );
    std::size_t endRunName = treeFName.find ( "_", begRunName );

    if ( begRunName != std::string::npos && endRunName != std::string::npos ) rootFileName += treeFName.substr ( begRunName, endRunName ) + ".root";
    else rootFileName += treeFName;

    TFile* f = new TFile ( rootFileName.c_str(), "update" );

    f->cd();

    for ( auto itr = resStripsEnCalGraphsMap.begin(); itr != resStripsEnCalGraphsMap.end(); itr++ )
    {
        TGraph* gr = itr->second;

        if ( f->FindKey ( gr->GetName() ) != NULL || f->FindObject ( gr->GetName() ) != NULL )
        {
            std::string objToDelete = gr->GetName();
            objToDelete += ";*";
            f->Delete ( objToDelete.c_str() );
        }

        std::cout << "Writing " << gr->GetName() << " to file..." << std::endl;

        gr->Write();
    }

    f->Close();

    gDirectory->cd ( currPath.c_str() );
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

    std::string isUpstreamID = isUpstream_ ? "U" : "D";

    for ( unsigned short i = 0; i < sectorsList.size(); i++ )
    {
        for ( int j = 0; j < 4; j++ )
        {
            std::string hname = Form ( "%s%d_%d", isUpstreamID.c_str(), sectorsList[i], j );

            std::cout << "Initializing graph for sector " << isUpstreamID << sectorsList[i] << " strip " << j << " and storing it in the map at key \"" << hname << "\"" << std::endl;

            resStripsPosCalGraphsMap[hname.c_str()] = new TH2F ( hname.c_str(), hname.c_str(), nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY );
        }
    }

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

            if ( siData.isBarrel && siData.isUpstream == isUpstream_ && siData.E1.en.p.size() > 0 && siData.E1.en.n.size() > 0 )
            {
                int sect = -1;

                for ( unsigned short sec = 0; sec < sectorsList.size(); sec++ )
                {
                    if ( siData.sector == sectorsList[sec] ) sect = siData.sector;
                }

                if ( sect == -1 ) continue;

                for ( unsigned short k = 0; k < siData.E1.en.p.size(); k++ )
                {
                    TH2F* hh = resStripsPosCalGraphsMap[Form ( "%s%d_%d", isUpstreamID.c_str(), sect, siData.E1.strip.p[k] )];
                    hh->Fill ( ( siData.E1.en.p[k] - siData.E1.en.n[k] ) / ( siData.E1.en.p[k] + siData.E1.en.n[k] ), ( siData.E1.en.p[k] + siData.E1.en.n[k] ) );
                    break;
                }
            }
        }
    }

    std::cout << std::endl;

    WritePosCalHistsToFile ( tree, "resistive_Strips_PosCal_hists" );

    return resStripsPosCalGraphsMap;
}

#endif
