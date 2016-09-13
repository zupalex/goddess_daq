#include "hribfBuffer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <deque>

int usage ( std::string executable )
{
    std::cout << "Usage: " << executable << " input.ldf output.geb\n";
    return 1;
}

int main ( int argc, char *argv[] )
{
    const bool verbose = false;

    if ( argc != 3 )
    {
        return usage ( argv[0] );
    }
    std::string inputName = argv[1];
    std::string outputName = argv[2];

    hribfBuffer buffer ( inputName.c_str() );

    std::ofstream output ( outputName,std::ofstream::binary );

    const int type = 19;

    //The parameter numbers of the MYRIAD timestamp, low to high bits.
    const unsigned short myriadParam[3]  = {1002,1001,1000};

    unsigned int eventCnt=0;
    unsigned int myriadMalformed = 0;
    unsigned int invertedOrderMalformed = 0;
    unsigned int myriadMissing = 0;
    float avgNumADCs = 0;
    float stdDevNumADCs = 0;
    std::map<unsigned short, unsigned short> *values = buffer.GetMap();
    const int numStamps = 10;
    std::deque< unsigned long long int > lastTimestamp;
    for ( int i=0; i<numStamps; i++ )
        lastTimestamp.push_back ( -1 );
    while ( buffer.ReadNextBuffer() > 0 )
    {

        //Print a buffer counter so the user sees that it is working.
        if ( buffer.GetBufferNumber() % 100 == 0 )
        {
            printf ( "Buffer: %d File: %5.2f%%\r",buffer.GetBufferNumber(),buffer.GetFilePositionPercentage() );
            fflush ( stdout );
        }

        if ( buffer.GetBufferType() == hribfBuffer::BUFFER_TYPE_DATA )
        {


            while ( buffer.GetEventsRemaining() )
            {
                eventCnt++;

                buffer.ReadEvent();

                unsigned long long int timestamp = 0;
                bool myriadFail = false;
                for ( int i=0; i<3; i++ )
                {
                    if ( values->find ( myriadParam[i] ) == values->end() )
                    {
                        std::cerr << "ERROR: Timestamp not found in event " << eventCnt << ", buffer " << buffer.GetBufferNumber() << "! Parameters checked";
                        for ( int param=0; param < 3; param++ )
                            std::cerr << ", " << myriadParam[param];
                        std::cerr << ".\n";
                        std::cerr << "\tFor your reading pleasure an event dump:\n";
                        for ( auto itr=values->begin(); itr != values->end(); ++itr )
                        {
                            std::cerr << "\tValue[" << itr->first << "]=" << itr->second << "\n";
                        }
                        myriadMissing++;
                        myriadFail = true;
                        break;
                    }

                    timestamp |= ( unsigned long long ) values->at ( myriadParam[i] ) << ( i * 16 );
                }

                if ( myriadFail ) continue;

                if ( values->at ( myriadParam[2] ) == 0xAAAA )
                {
                    myriadMalformed++;
                    /*    std::cerr << "WARNING: " << myriadParam[2] << "!\n";
                    std::cerr << "Last Last Timestamp:\t" << lastlastTimestamp << " " << std::hex <<  lastlastTimestamp << std::dec << "\n";
                    std::cerr << "Last Timestamp:\t" << lastTimestamp << " " << std::hex <<  lastTimestamp << std::dec << "\n";
                    std::cerr << "Timestamp:\t" << timestamp << " " << std::hex << timestamp << std::dec << "\n";*/

                    if ( verbose )
                    {
                        std::cout << "\n";
                        for ( auto itr=values->begin(); itr != values->end(); ++itr )
                        {
                            std::cout << "\tValue[" << itr->first << "]=" << std::hex << itr->second << "\n";
                        }
                        std::cout << std::hex << timestamp << "\n";
                    }

                    continue;
                }
//                 if ( values->at ( myriadParam[1] ) == 0xAAAA && values->at ( myriadParam[2] ) != lastTimestamp[4] >> 32 )
//                 {
//                     // myriadMalformed++;
//                     std::cerr << "WARNING: " << myriadParam[1] << "!\n";
//                     for ( int i=0; i<numStamps; i++ )
//                     {
//                         std::cerr << std::hex << lastTimestamp[i] << " ";
//                     }
//                     std::cout << std::hex << timestamp << std::dec << "\n";
// 
//                 }
//                 if ( values->at ( myriadParam[0] ) == 0xAAAA && values->at ( myriadParam[2] ) != lastTimestamp[4] >> 32 )
//                 {
//                     // myriadMalformed++;
//                     std::cerr << "WARNING: " << myriadParam[0] << "!\n";
//                     for ( int i=0; i<numStamps; i++ )
//                     {
//                         std::cerr << std::hex << lastTimestamp[i] << " ";
//                     }
//                     std::cout << std::hex << timestamp << std::dec << "\n";
// 
//                 }
//                 if ( timestamp == 0 )
//                 {
//                     std::cerr << "WARNING: 0!\n";
//                     for ( int i=0; i<numStamps; i++ )
//                     {
//                         std::cerr << std::hex << lastTimestamp[i] << " ";
//                     }
//                     std::cout << std::hex << timestamp << std::dec << "\n";
//                 }
//                 if ( timestamp < lastTimestamp.back() )
//                 {
//                     invertedOrderMalformed++;
// 
//                     std::cerr << "WARNING: Current timestamp smaller than last timestamp!\n";
//                     for ( int i=0; i<numStamps; i++ )
//                     {
//                         std::cerr << std::hex << lastTimestamp[i] << " ";
//                     }
//                     std::cout << std::hex << timestamp << std::dec << "\n";
//                 }

                int length = values->size() * 2 * sizeof ( short );
                output.write ( ( char* ) &type,sizeof ( int ) );
                output.write ( ( char* ) &length,sizeof ( int ) );
                output.write ( ( char* ) &timestamp,sizeof ( long long ) );
                for ( auto itr=values->begin(); itr != values->end(); ++itr )
                {
                    output.write ( ( char* ) & ( itr->first ), sizeof ( short ) );
                    output.write ( ( char* ) & ( itr->second ), sizeof ( short ) );
                }

                //Compute average number of ADCs triggered
                avgNumADCs = avgNumADCs + ( values->size() - avgNumADCs ) / eventCnt;
                if ( eventCnt > 1 ) stdDevNumADCs = stdDevNumADCs + ( float ) ( eventCnt ) / ( eventCnt -1 ) * pow ( avgNumADCs - values->size(),2 );

                lastTimestamp.push_back ( timestamp );
                lastTimestamp.pop_front();


            } // Next event
        } //If Data Buffer

    } //next Buffer

    stdDevNumADCs = sqrt ( 1./ ( eventCnt - 1. ) * stdDevNumADCs );

    std::cout << "Converted " << eventCnt << " events.           \n";
    std::cout << "Avg number of ADCs per event: " << avgNumADCs << " +/- " << stdDevNumADCs << "\n";
    std::cout << "Missing Myriad Events: " << myriadMissing << " " << ( float ) myriadMissing*100/eventCnt << "%\n";
    std::cout << "Single Word Malformed Myriad Events: " << myriadMalformed << " " << ( float ) myriadMalformed*100/eventCnt << "%\n";
    std::cout << "Inverted Timestamp Order Events: " << invertedOrderMalformed << " " << ( float ) invertedOrderMalformed*100/eventCnt << "%\n";

    return 0;
}
