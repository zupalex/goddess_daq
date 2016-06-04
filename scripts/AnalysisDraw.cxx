#include "goddess_analysis_macros.h"

std::vector<unsigned short> GetStripsListToTreat ( string strips )
{
    std::vector<unsigned short> stripsList;
    stripsList.clear();

    if ( strips == "all" )
    {
        for ( int i = 0; i < 4; i++ )
            stripsList.push_back ( i );
    }
    else
    {
        istringstream stripsss;
        stripsss.clear();
        stripsss.str ( strips );

        unsigned short st_;
        bool read = false;

        read = ( stripsss >> st_ );

        while ( read )
        {
            stripsList.push_back ( st_ );

            read = ( stripsss >> st_ );
        }
    }

    return stripsList;
}

template<typename First, typename... Rest> TH2F* DrawEnergyVsAngleSX3 ( TChain* chain, int nentries,
        string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, bool isUpstream_, string strips,
        First fstSector, Rest... otherSectors )
{
    std::vector<unsigned short> sectorsList;
    sectorsList.clear();

    GetListOfSectorsToTreat<First, Rest...> ( &sectorsList, fstSector, otherSectors... );

    std::vector<unsigned short> stripsList = GetStripsListToTreat ( strips );

    std::vector<SiDataBase>* siDataVect = new std::vector<SiDataBase>();
    siDataVect->clear();

    chain->SetBranchAddress ( "si", &siDataVect );

    if ( nentries > chain->GetEntries() || nentries == 0 ) nentries = chain->GetEntries();

    TH2F* hist = new TH2F ( hname.c_str(), hname.c_str(), nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY );

    std::cout << std::endl;

    for ( int i = 0; i < nentries; i++ )
    {
        chain->GetEntry ( i );

        if ( i%10000 == 0 ) std::cout << "Treated " << i << " / " << nentries << " entries ( " << ( ( float ) i ) / ( ( float ) nentries ) * 100. << " % )\r" << std::flush;

        if ( siDataVect->size() == 0 ) continue;

        for ( unsigned short j = 0; j < siDataVect->size(); j++ )
        {
            SiDataBase siData = siDataVect->at ( j );

            if ( siData.isUpstream == isUpstream_ )
            {
                int sect = -1;

                for ( unsigned short k = 0; k < sectorsList.size(); k++ )
                {
                    if ( siData.sector == sectorsList[k] ) sect = sectorsList[k];
                }

                if ( sect == -1 ) continue;

                int stPos = -1;

                for ( unsigned short k = 0; k < stripsList.size(); k++ )
                {
                    if ( DecodeStripNumber ( siData.stripMax[k] ) == stripsList[k] ) stPos = k;
                }

                if ( stPos == -1 ) continue;

                if ( siData.pos[stPos].Mag() > 0.0 && !siData.isBarrel )
                {
                    hist->Fill ( siData.pos[stPos].Angle ( TVector3 ( 0, 0, 1 ) ) *180/TMath::Pi(), siData.eSum[stPos] );
                }
            }
        }
    }

    hist->Draw ( drawOpts.c_str() );

    std::cout << std::endl;

    return hist;
}

template<typename T> void DrawPosCal ( TTree* tree, bool isUpstream_, unsigned short sector_, unsigned short strip_, int nentries, T* drawResult )
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

TGraph* DrawPosCalGraph ( TTree* tree, bool isUpstream_, int nentries, unsigned short sector_, unsigned short strip_ )
{
    TGraph* newGraph = new TGraph();

    DrawPosCal ( tree, isUpstream_, sector_,strip_, nentries, newGraph );

    newGraph->Draw ( "AP" );

    return newGraph;
}

TH2F* DrawPosCalHist ( TTree* tree, bool isUpstream_, int nentries, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, unsigned short sector_, unsigned short strip_ )
{
    string hname = Form ( "hPosCal_sector%s%d_strip%d", isUpstream_ ? "U" : "D", sector_, strip_ );

    TH2F* newHist = new TH2F ( hname.c_str(), hname.c_str(), nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY );

    DrawPosCal ( tree, isUpstream_, sector_,strip_, nentries, newHist );

    newHist->Draw ( drawOpts.c_str() );

    return newHist;
}

template<typename First, typename... Rest> std::map<string, TH2F*> DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, First fstSector, Rest... otherSectors )
{
    std::vector<unsigned short> sectorsList;
    sectorsList.clear();

    GetListOfSectorsToTreat<First, Rest...> ( &sectorsList, fstSector, otherSectors... );

    std::map<string, TH2F*> histsMap;
    histsMap.clear();

    string isUpstreamID = isUpstream_ ? "U" : "D";

    for ( unsigned short i = 0; i < sectorsList.size(); i++ )
    {
        for ( int j = 0; j < 4; j++ )
        {
            string hname = Form ( "%s%d_%d", isUpstreamID.c_str(), sectorsList[i], j );

            std::cout << "Initializing graph for sector " << isUpstreamID << sectorsList[i] << " strip " << j << " and storing it in the map at key \"" << hname << "\"" << std::endl;

            histsMap[hname.c_str()] = new TH2F ( hname.c_str(), hname.c_str(), nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY );
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
                    histsMap[Form("%s%d_%d", isUpstreamID.c_str(), sect, siData.E1.strip.p[k])]->Fill ( ( siData.E1.en.n[k] - siData.E1.en.p[k] ) / ( siData.E1.en.p[k] + siData.E1.en.n[k] ), ( siData.E1.en.p[k] + siData.E1.en.n[k] ) );
                    break;
                }
            }
        }
    }

    std::cout << std::endl;
    
    return histsMap;
}

std::map<string, TH2F*> DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, unsigned short sector_ )
{
  return DrawPosCalHistBatch(tree, isUpstream_, nentries,nbinsX,binMinX, binMaxX, nbinsY, binMinY, binMaxY, drawOpts, sector_);
}
