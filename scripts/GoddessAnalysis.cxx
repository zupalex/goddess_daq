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

        read = ( stripsss >> st_ );

        while ( read )
        {
            stripsList.push_back ( st_ );

            read = ( stripsss >> st_ );
        }
    }

    return stripsList;
}



ClassImp(GoddessAnalysis)
