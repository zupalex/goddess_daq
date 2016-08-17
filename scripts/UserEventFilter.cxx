#include <iostream>

#include "ProcessManagers.h"

bool SortManager::GetWriteEventFlag()
{
    bool flag = false;

//     if ( GEB_event->ptgd.size() > 0 )
//     {
//         for ( unsigned int i = 0; i < GEB_event->ptgd.size(); i++ )
//         {
//             if ( GEB_event->ptgd[i]->type == 16 )
//             {
//                 return false;
//             }
//         }
//     }

    if ( gammaDets->size() > 0 )
    {

    }

    if ( siDets->size() > 0 )
    {
        for ( auto itr = siDets->begin(); itr != siDets->end(); itr++ )
        {
            SiDataBase siData = *itr;

            if ( siData.isBarrel && siData.isUpstream )
            {
                flag = true;
                break;
            }
        }
    }

    if ( ionChamber->size() > 0 )
    {

    }

//     flag = true;

    return flag;
}
