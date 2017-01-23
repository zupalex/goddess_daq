#include "GEBSort.h"
#include "GTMerge.h"

#include "GoddessData.h"
#include "GoddessConfig.h"

#include "SortManager.h"

#include <iostream>
#include <string>

void sup_god()
{
//     PARS* Pars = SortManager::sinstance()->execParams;

//     std::string confName = Pars->ConfigFile;
    SortManager::sinstance()->godData = new GoddessData ();
}

int bin_god ( GEB_EVENT* gebEvt )
{
    std::vector<AGODEVENT> AGODEvts;
    std::vector<DFMAEVENT> DFMAEvts;
    std::vector<DGSEVENT> DGSEvts;

    for ( unsigned int i = 0; i < SortManager::sinstance()->numAGOD; i++ )
    {
        AGODEvts.push_back ( SortManager::sinstance()->AGODEvent[i] );
    }

    for ( unsigned int i = 0; i < SortManager::sinstance()->numDGOD; i++ )
    {
        DFMAEvts.push_back ( SortManager::sinstance()->DFMAEvent[i] );
    }

    for ( int i = 0; i < SortManager::sinstance()->ng; i++ )
    {
        DGSEvts.push_back ( SortManager::sinstance()->DGSEvent[i] );
    }

    return SortManager::sinstance()->godData->Fill ( gebEvt, &DGSEvts, &DFMAEvts, &AGODEvts );
}
