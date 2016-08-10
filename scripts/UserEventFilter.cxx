#include <iostream>

#include "GoddessStruct.h"

bool SortManager::GetWriteEventFlag()
{
    bool flag = false;

    if ( gammaDets->size() > 0 )
    {

    }

    if ( siDets->size() > 0 )
    {
        for ( auto itr = siDets->begin(); itr != siDets->end(); itr++ )
        {
            SiDataBase siData = *itr;

            if ( siData.eSumLayer ( 1, false ) > 0.8 )
            {
                flag = true;
                break;
            }
        }
    }

    if ( ionChamber->size() > 0 )
    {

    }

    return flag;
}
