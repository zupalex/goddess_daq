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

InDataInfo::InDataInfo ( std::ifstream* istream_ )
{
    istream = istream_;
}

// --------------------- EVENT ---------------------- //

EVENT::EVENT()
{
    gd = new GebData;
    payload = new char[50000];
    key = -1;
}

EVENT::~EVENT()
{

}

// --------------------- MergeManager ---------------------- //

MergeManager* MergeManager::s_instance = nullptr;

MergeManager::MergeManager()
{
    maxCoincEv = MAXCOINEV;

    maxBigBufSize = MAXBIGBUFSIZ;
    bigbuf = new std::vector<char*>();


    inData = new std::vector<InDataInfo*>();
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
        if ( input == ( *itr )->fileName )
        {
            ( *itr )->istream->close();

            inData->erase ( itr );

            return;
        }
    }
}

std::pair< unsigned int, unsigned long long int > MergeManager::GetSizeAndBytesCount ( bool ofType )
{
    unsigned int ofEvSize = 0;

    unsigned long long int bufferBytesCount = 0;

    if ( ofType )
    {
        for ( auto ofItr = overflowEvent.begin(); ofItr != overflowEvent.end(); ofItr++ )
        {
            ofEvSize += ofItr->second->size();

            for ( unsigned int m = 0; m < ofItr->second->size(); m++ )
            {
                bufferBytesCount += sizeof ( GebData ) + ofItr->second->at ( m )->second->gd->length;
            }
        }
    }

    else
    {
        for ( auto ofItr = Event.begin(); ofItr != Event.end(); ofItr++ )
        {
            ofEvSize += ofItr->second->size();

            for ( unsigned int m = 0; m < ofItr->second->size(); m++ )
            {
                bufferBytesCount += sizeof ( GebData ) + ofItr->second->at ( m )->gd->length;
            }
        }
    }

    return std::make_pair ( ofEvSize, bufferBytesCount );
}

