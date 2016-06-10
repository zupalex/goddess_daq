#include "GoddessAnalysis.h"

std::vector<unsigned short> GoddessAnalysis::GetStripsListToTreat ( std::string strips )
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
        std::istringstream stripsss;
        stripsss.clear();
        stripsss.str ( strips );

        unsigned short st_;
        bool read = false;

        while ( stripsss >> st_ )
        {
            stripsList.push_back ( st_ );
        }
    }

    return stripsList;
}

TH2F* GoddessAnalysis::DrawEnergyVsAngleSX3 ( TChain* chain, int nentries, std::string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts,
        bool isUpstream_, std::string strips, short unsigned int sector )
{
    DrawEnergyVsAngleSX3 ( chain, nentries, hname, nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY, drawOpts, isUpstream_, strips, sector );
}

ClassImp ( GoddessAnalysis )
