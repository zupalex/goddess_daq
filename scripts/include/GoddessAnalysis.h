#ifndef __GODDESSANALYSIS__
#define __GODDESSANALYSIS__

#include "goddess_analysis_macros.h"

class GoddessAnalysis : public TObject, public TQObject
{
private:
    std::string defaultTreeName1;
    std::string defaultTreeName2;
    std::string defaultFileName;

public:
    GoddessAnalysis();
    GoddessAnalysis ( std::string filename );
    GoddessAnalysis ( std::string filename, std::string treename, std::string treename2 );

    virtual ~GoddessAnalysis();
    
    void SetDefaultFileAndTrees(std::string fileName, std::string treeName1, std::string treeName2);

    template<typename T> inline static void DisplayMapKeys ( std::map<std::string, T> map_ );
    inline static void DisplayMapKeys ( std::map<std::string, float> map_ );

    inline static int DecodeStripNumber ( int st_ );

    template<typename T> inline static void GetListOfSectorsToTreat ( std::vector<unsigned short>* sectorsList, T sector );

    template<typename First, typename... Rest> inline static void GetListOfSectorsToTreat ( std::vector<unsigned short>* sectorsList, First fstSector, Rest... otherSectors );

    template<typename First, typename... Rest> inline TH2F* DrawEnergyVsAngleSX3 ( TChain* chain, int nentries,
            std::string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts, bool isUpstream_, std::string strips,
            First fstSector, Rest... otherSectors );

    TH2F* DrawEnergyVsAngleSX3 ( TChain* chain, int nentries,
                                 std::string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts, bool isUpstream_, std::string strips,
                                 unsigned short sector );

    std::vector<unsigned short> GetStripsListToTreat ( std::string strips );

    void CheckMapping ( std::string filename, std::string treename, std::string treename2, unsigned short channel1, unsigned short channel2, bool Digital, std::string DetectorID );

    void CheckMapping ( unsigned short channel1, unsigned short channel2, bool Digital, std::string DetectorID );

    ClassDef ( GoddessAnalysis, 1 )
};

template<typename T> inline void GoddessAnalysis::DisplayMapKeys ( std::map<std::string, T> map_ )
{
    for ( auto itr = map_.begin(); itr != map_.end(); itr++ )
    {
        std::cout << itr->first << std::endl;
    }
}

inline void GoddessAnalysis::DisplayMapKeys ( std::map<std::string, float> map_ )
{
    for ( auto itr = map_.begin(); itr != map_.end(); itr++ )
    {
        std::cout << itr->first << std::endl;
    }
}

inline int GoddessAnalysis::DecodeStripNumber ( int st_ )
{
    return st_%100;
}

template<typename T> inline void GoddessAnalysis::GetListOfSectorsToTreat ( std::vector<unsigned short>* sectorsList, T sector )
{
    std::cout << "adding " << sector << " to the list of sectors to treat..." << std::endl;

    sectorsList->push_back ( sector );
}

template<typename First, typename... Rest> inline void GoddessAnalysis::GetListOfSectorsToTreat ( std::vector<unsigned short>* sectorsList, First fstSector, Rest... otherSectors )
{
    GetListOfSectorsToTreat<unsigned short> ( sectorsList, fstSector );

    GetListOfSectorsToTreat ( sectorsList, otherSectors... );
}

template<typename First, typename... Rest> TH2F* GoddessAnalysis::DrawEnergyVsAngleSX3 ( TChain* chain, int nentries,
        std::string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts, bool isUpstream_, std::string strips,
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

    std::cout << std::endl;

    return hist;
}



#endif

