#include "MergeManager.h"

void CheckNoArgs ( int required, int actual, string str )
{
    if ( required < actual )
    {
        printf ( "argument problem with chat option\n" );
        printf ( "--> %s\n", str.c_str() );
        printf ( "required # arguments: %i\n", required - 1 );
        printf ( "actual   # arguments: %i\n", actual - 1 );
        printf ( "Please fix and try again, quitting...\n" );
        exit ( 0 );
    }
}

// --------------------- InDataInfo ---------------------- //

InDataInfo::InDataInfo ( std::ifstream& istream_ )
{
    istream = &istream_;
}

// --------------------- MergeManager ---------------------- //

MergeManager* MergeManager::s_instance = nullptr;

MergeManager::MergeManager()
{
    maxCoincEv = MAXCOINEV;

    maxBigBufSize = MAXBIGBUFSIZ;
    bigbuf = new std::vector<char*>();


    inData = new std::vector<InDataInfo>();
}

MergeManager::~MergeManager()
{

}

MergeManager* MergeManager::sinstance()
{
    if ( s_instance == NULL )
    {
        s_instance = new MergeManager();
    }

    return s_instance;
}

void MergeManager::RemoveFromInputList ( string input )
{
    for ( auto itr = inData->begin(); itr != inData->end(); itr++ )
    {
        if ( input == ( *itr ).fileName )
        {
            inData->erase ( itr );

            return;
        }
    }
}
