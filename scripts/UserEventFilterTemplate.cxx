// UserEventFilterTemplate version 170426 //

#include <iostream>

#include "GoddessData.h"

bool GoddessData::GetWriteEventFlag ( GEB_EVENT* gebEvt )
{
    ( void ) gebEvt;

    bool flag = false;

//     if ( gebEvt->ptgd.size() > 0 )
//     {
//         for ( unsigned int i = 0; i < gebEvt->ptgd.size(); i++ )
//         {
//             if ( gebEvt->ptgd[i]->type == 16 )
//             {
//                 return false;
//             }
//         }
//     }

//     if ( gamData->size() > 0 )
//     {
//
//     }
//
    if ( siData->size() > 0 )
    {
        for ( auto itr = siData->begin(); itr != siData->end(); itr++ )
        {
            SiDataBase siDatum = *itr;

            if ( siDatum.isBarrel && siDatum.isUpstream )
            {
                flag = true;
                break;
            }
        }
    }
//
//     if ( ionData->size() > 0 )
//     {
//
//     }

//     flag = true;

    return flag;
}
