#include <stdlib.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <assert.h>
#include <zlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "gdecomp.h"
#include "GTMerge.h"
#include "MergeManager.h"

using std::string;

MSTAT* nstat;
CONTROL control;

bool printDebug = false;

// MergeManager* MergeManager::s_instance = nullptr;

#if (USEZLIB == 0 )
int bread ( std::ifstream inFile, char *val, int *pos, int *buf, int *bufsiz )
#else
int bread ( gzFile in, int *val, int *pos, int *buf, int *bufsiz )
#endif
{
    /* buffered inter read */

    /* declarations */

    int siz;

    /* read new buffer */

    if ( *pos == 0 )
    {

        /* read in a buffer of data */

#if (USEZLIB == 0 )

        inFile.read ( ( char * ) buf, *bufsiz * sizeof ( int ) );

#else

        siz = gzread ( in, ( char * ) buf, *bufsiz * sizeof ( int ) );

#endif

        *bufsiz = siz / sizeof ( int );

        if ( siz <= 0 )
            return ( -1 );

    };

    /* return a value */

    *val = * ( buf + ( *pos ) );

    ( *pos ) ++;

    /* force buffer read the next time? */

    if ( *pos == *bufsiz )
        *pos = 0;

    /* done, report we got 4 bytes like 'read' would */

    return ( sizeof ( int ) );

}

EVENT* GTGetDiskEv ( InDataInfo inFile, bool printInfo )
{
    if ( printDebug ) std::cerr << "File is " << ( inFile.istream->is_open() ? "open" : "NOT open!" ) << "\n";
    /* returns: */
    /*   0: success */
    /*   1: header */
    /*   2: read trouble; end of file */
    /*   5: bad test pattern */

    /* declarations */

    MergeManager* theMergeManager = MergeManager::sinstance();

    int siz, i1;
    int storeNo = theMergeManager->Event.size();

    EVENT* bufEvent = new EVENT;

    /* attempt to read a GEB header */

#if (USEBREAD)

    siz = bread ( inFile.istream, ( char * ) bufEvent.gd, &bread_pos[storeNo], bread_buf[storeNo], &bread_bufsiz[storeNo] );

    if ( siz != sizeof ( GEBDATA ) )
    {
        printf ( "failed to read %lu bytes for header, got %i\n", sizeof ( GEBDATA ), siz );
        return nullptr;
    }

    nstat->inbytes += siz;
    control.filesiz[storeNo] += siz;

#else

    if ( printDebug ) std::cerr << "*** Reading the header...\n";

    ( * ( inFile.istream ) ).read ( ( char* ) bufEvent->gd, sizeof ( GEBDATA ) );

    if ( printDebug )
    {
        std::cerr << "=> Header read, now checking it...\n";
        std::cerr << "   - type is " << bufEvent->gd->type << " / length is " << bufEvent->gd->length << "\n";
        std::cerr << "   - timestamp is " << bufEvent->gd->timestamp << "\n";
    }

    if ( ( inFile.istream )->gcount() != sizeof ( GEBDATA ) || ( inFile.istream )->eof() )
    {
        printf ( "failed to read %lu bytes for header, got %i\n", sizeof ( GEBDATA ), ( inFile.istream )->gcount() );
        theMergeManager->RemoveFromInputList ( inFile.fileName );

        return nullptr;
    }

    if ( printDebug ) std::cerr << "Header looks fine... Filling some stats...\n";

    nstat->inbytes += ( inFile.istream )->gcount();
    control.filesiz[storeNo] += ( inFile.istream )->gcount();

#endif

    if ( printDebug ) std::cerr << "Header stats filled...\n";

    if ( printInfo ) printf ( "\ngot initial header, TS=%lli for storeNo=%i\n", bufEvent->gd->timestamp, storeNo );

    /* attempt to read payload */

    i1 = bufEvent->gd->length;

    bufEvent->payload = new char[i1];

#if (USEBREAD)

    siz = bread ( inData, ( char * ) bufEvent->payload, &bread_pos[storeNo], bread_buf[storeNo], &bread_bufsiz[storeNo] );

    if ( siz != i1 )
    {
        printf ( "failed to read %i bytes for payload, got %i\n", i1, siz );
        return nullptr;
    }

    nstat->inbytes += siz;
    control.filesiz[storeNo] += siz;
    control.fileEventsRead[storeNo]++;

#else

    if ( printDebug ) std::cerr << "Reading the payload...\n";

    ( inFile.istream )->read ( ( char * ) bufEvent->payload, i1 );

    if ( printDebug ) std::cerr << "Payload read... now checking it...\n";

    if ( ( inFile.istream )->gcount() != i1 || ( inFile.istream )->eof() )
    {
        printf ( "failed to read %i bytes for payload, got %i\n", i1, siz );
        theMergeManager->RemoveFromInputList ( inFile.fileName );

        return nullptr;
    }

    if ( printDebug ) std::cerr << "Payload seems fine... now filling some stats...\n";

    nstat->inbytes += ( inFile.istream )->gcount();
    control.filesiz[storeNo] += ( inFile.istream )->gcount();
    control.fileEventsRead[storeNo]++;

    if ( printDebug ) std::cerr << "Payload stats filled...\n";

#endif

    if ( printInfo ) printf ( "read initial payload of siz=%i into  storeNo=%i\n", siz, storeNo );

    /* done */

    if ( printDebug ) std::cerr << "Done with GTGetDiskEv... Exiting...\n";

    return bufEvent;
}

int main ( int argc, char **argv )
{
//     std::cerr << "Entering main function of GEBMerge.cxx...\n";

    /* declarations */

    MergeManager* theMergeManager = MergeManager::sinstance();
    std::vector<InDataInfo>* inData = theMergeManager->inData;
    std::ofstream* outData = &theMergeManager->outData;

#if(USEZLIB==1)

    gzFile* zFile = theMergeManager->zFile;

#endif

    gzFile zoutData;
    int nfiles = 0, maxNoEvents = 0, nPoolEvents = 0;
    FILE *fp;
    int i1, i2;
    int st, nn, argcoffset = 0;
    float ehigain[NGE + 1], ehioffset[NGE + 1], r1;
    char str[STRLEN], str1[512];
    unsigned int seed = 0;
    struct tms timesThen;
    int i7, i8, reportinterval;
    int echo = 0, nni, nret;
    int size;
    int wosize = 0;
    int nprint;

    /* help */

    if ( argc == 1 )
    {
        printf ( "use: GEBMerge chatfile outfile     file1  file2  file3  file4 .....\n" );
        printf ( "eg., GEBMerge gtmerge.chat c.gtd   t1.gtd t2.gtd t3.gtd t4.gtd\n" );
        exit ( 0 );
    };


    /* initialize random number generator etc */

    get_a_seed ( &seed );
    srand ( seed );
    nstat = ( MSTAT * ) calloc ( 1, sizeof ( MSTAT ) );
    bzero ( ( char * ) &control, sizeof ( CONTROL ) );
    bzero ( ( char * ) nstat, sizeof ( MSTAT ) );

    control.dtsfabort = 5;
    control.dtsbabort = 5;

    for ( int i = 0; i < NCHANNELS; i++ )
        nstat->id_hit[i] = 0;

    for ( int i = 0; i <= NGE; i++ )
    {
        ehigain[i] = 1;
        ehioffset[i] = 0;
    };

    /* open chat file */

    std::ifstream chatFile ( argv[1] );

    if ( !chatFile.is_open() )
    {
        printf ( "error: could not open chat file: <%s>\n", argv[1] );
        exit ( 0 );
    }

    printf ( "chat file: <%s> open\n", argv[1] );
    printf ( "\n" );
    fflush ( stdout );

    if ( printDebug ) std::cerr << "Reading chatfile...\n";

    /* read chatfile content and act */

    string readLine;

    nn = 0;
    while ( std::getline ( chatFile, readLine ) )
    {
        if ( readLine.empty() ) continue;

        if ( echo )
            printf ( "chat->%s", readLine.c_str() );
        fflush ( stdout );

        /* attemp to interpret the line */

        if ( readLine.find ( "echo" ) != string::npos )
        {
            echo = 1;
            if ( echo )
                printf ( "will echo command lines\n" );
            fflush ( stdout );

        }
        else if ( readLine[0] == 35 )
        {
            /* '#' comment line, do nothing */
            nni--;                /* don't count as instruction */

        }
        else if ( readLine[0] == 59 )
        {
            /* ';' comment line, do nothing */
            nni--;                /* don't count as instruction */

        }
        else if ( readLine[0] == 10 )
        {
            /* empty line, do nothing */
            nni--;                /* don't count as instruction */

        }
        else if ( readLine.find ( "maxNoEvents" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %i", str1, &maxNoEvents );
            CheckNoArgs ( nret, 2, readLine );
        }
        else if ( readLine.find ( "reportinterval" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %i", str1, &reportinterval );
            CheckNoArgs ( nret, 2, readLine );
        }
        else if ( readLine.find ( "chunksiz" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %i", str1, &control.chunksiz );
            CheckNoArgs ( nret, 2, readLine );
        }
        else if ( readLine.find ( "bigbufsize" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %i", str1, &size );
            CheckNoArgs ( nret, 2, readLine );
            assert ( size <= MAXBIGBUFSIZ );
            r1 = ( size * sizeof ( EVENT ) + ( size + 1 ) * sizeof ( int ) ) / 1024.0 / 1024.0;
            printf ( "sizeof(EVENT)= %lu\n", sizeof ( EVENT ) );
            printf ( "will use a bigbuffer size of %i, or %7.3f MBytes\n", size, r1 );
        }
        else if ( readLine.find ( "nprint" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %i", str1, &nprint );
            CheckNoArgs ( nret, 2, readLine );
            printf ( "will print information for first %i events\n", nprint );
        }
        else if ( readLine.find ( "wosize" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %f", str1, &r1 );
            r1 = ( r1 / 100.0 * size );
            wosize = ( int ) r1;
            CheckNoArgs ( nret, 2, readLine );
            printf ( "will use a bigbuffer wosize of %i\n", wosize );
            assert ( wosize <= size );
        }
        else if ( readLine.find ( "startTS" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %llu %llu ", str1, &control.startTS_lo, &control.startTS_hi );
            CheckNoArgs ( nret, 3, readLine );
            printf ( "startTS from %lli to %lli\n", control.startTS_lo, control.startTS_hi );
            control.startTS = 1;
        }
        else if ( readLine.find ( "zzipout" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s", str1 );
            CheckNoArgs ( nret, 1, readLine );
            control.zzipout = 1;
            printf ( "will zzip output, %i\n", control.zzipout );
            fflush ( stdout );

        }
        else if ( readLine.find ( "TSlistelen" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %i %i %i", str1, &control.TSlistelen, &control.TSlist_lo, &control.TSlist_hi );
            CheckNoArgs ( nret, 4, readLine );
        }
        else if ( readLine.find ( "dts_min" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %lli", str1, &control.dts_min );
            CheckNoArgs ( nret, 2, readLine );
            printf ( "control.dts_min=%lli\n", control.dts_min );
        }
        else if ( readLine.find ( "dts_max" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %lli", str1, &control.dts_max );
            CheckNoArgs ( nret, 2, readLine );
            printf ( "control.dts_max=%lli\n", control.dts_max );
        }
        else if ( readLine.find ( "dtsfabort" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %i", str1, &control.dtsfabort );
            CheckNoArgs ( nret, 2, readLine );
        }
        else if ( readLine.find ( "dtsbabort" ) != string::npos )
        {
            nret = sscanf ( readLine.c_str(), "%s %i", str1, &control.dtsbabort );
            CheckNoArgs ( nret, 2, readLine );
        }
        else
        {
            /* --------------------------- */
            /* chatscript read error point */
            /* --------------------------- */

            printf ( "line %2.2i in chat script, option :%s \n__not understood\n", nn, readLine.c_str() );
            printf ( "%i\n", readLine[0] );
            printf ( "aborting\n" );
            fflush ( stdout );
            exit ( 0 );
        }

        /* read next line in chat script */

        nn++;                     /* line counter */
        nni++;                    /* instruction counter */
    }

    chatFile.close();

    /* extract and repeat parameters */

    printf ( "%s: will produce a max of %i events\n", argv[0], maxNoEvents );

    printf ( "%s: will write combined data to file \"%s\"\n", argv[0], argv[2] );

    /* offset for data file name reads, find  nfiles etc */

    argcoffset = 3;
    nfiles = ( argc - argcoffset );
    printf ( "%s: we have %i datafiles to combine\n", argv[0], nfiles );
    fflush ( stdout );

    nPoolEvents = nfiles;
    printf ( "%s: will keep a pool of %i events to combine from \n", argv[0], nPoolEvents );
    assert ( nPoolEvents < MAXCOINEV );

    /* just to be sure */

    assert ( nfiles == nPoolEvents );

    /************************/
    /* open all input files */
    /************************/

    std::ifstream ifstreamArray[nfiles];

    for ( int i = 0; i < nfiles; i++ )
    {
        nn = i + argcoffset;

#if(USEZLIB==0)

        ifstreamArray[i].open ( argv[nn], std::ios_base::in );

        InDataInfo* newInDataInfo = new InDataInfo ( ifstreamArray[i] );

        if ( !newInDataInfo->istream->is_open() )
        {
            printf ( "could not open input data file [%i] %s, quit!\n", nn, argv[nn] );
            exit ( 1 );
        }

        newInDataInfo->fileName = argv[nn];
        newInDataInfo->fileNum = i;

        inData->push_back ( *newInDataInfo );

//         std::cerr << "Pushed file " << theMergeManager->inData->at ( theMergeManager->inData->size()-1 ).fileName << " (file is " << ( theMergeManager->inData->at ( theMergeManager->inData->size()-1 ).istream->is_open() ? "open" : "NOT open!" ) << ")\n";

#else

        zFile[i] = gzdopen ( argv[nn], "r" );

        if ( zFile[i] == NULL )
        {
            printf ( "could not open input data file [%i] %s, quit!\n", nn, argv[nn] );
            exit ( 1 );
        }

#endif

        printf ( "%s: input data file \"%s\", number %i, is open\n", argv[0], argv[nn], i );
        fflush ( stdout );
        control.nOpenFiles++;
        control.fileActive[i] = 1;
    }

    if ( printDebug )
    {
        for ( unsigned int j = 0; j < theMergeManager->inData->size(); j++ )
        {
            if ( !theMergeManager->inData->at ( j ).istream->is_open() )
            {
                std::cerr << theMergeManager->inData->at ( j ).fileName << " is NOT open!!\n";
            }
        }
    }

#if (USEBREAD)

    for ( int i = 0; i < nfiles; i++ )
    {
        bread_buf[i] = ( int * ) calloc ( BREAD_BUFSIZE, sizeof ( int ) );
        bread_pos[i] = 0;
        bread_bufsiz[i] = BREAD_BUFSIZE;
    }

#endif


    /* ----------- */
    /* output file */
    /* ----------- */

    /*                        + name of data file */
    /*                        |                   */
    char outName[512];
    sprintf ( outName, "%s_%3.3i", argv[2], control.chunkno );

    if ( control.zzipout == 0 )
    {
        outData->open ( outName, std::ios_base::out | std::ios_base::binary );

        if ( !outData->is_open() )
        {
            printf ( "could not open output data file %s, quit!\n", outName );
            exit ( 1 );
        }
    }
    else
    {
        int gzdfd = open ( outName, O_WRONLY | O_CREAT | O_TRUNC, PMODE );
        zoutData = gzdopen ( gzdfd, "w" );

        if ( zoutData == NULL )
        {
            printf ( "could not open output data file %s, quit!\n", outName );
            exit ( 1 );
        }
    }

    printf ( "%s: output data file \"%s\" is open\n", argv[0], outName );
    fflush ( stdout );

// #if(0)
//     /* write output header file */
//
//     bzero ( ( char * ) &outheader, sizeof ( DGSHEADER ) );
//     outheader.id = 1000;
//     if ( control.zzipout == 0 )
//         siz = write ( outData, ( char * ) &outheader, sizeof ( DGSHEADER ) );
//     else
//         siz = gzwrite ( zoutData, ( char * ) &outheader, sizeof ( DGSHEADER ) );
//
//     printf ( "header written to output file\n" );
// #endif

    /* -------------------- */
    /* read in the map file */
    /* -------------------- */

    if ( printDebug ) std::cerr << "Reading or creating the map.dat...\n";

    for ( int i = 0; i < NCHANNELS; i++ )
    {
        theMergeManager->tlkup[i] = NOTHING;
        theMergeManager->tid[i] = NOTHING;
    }

    fp = fopen ( "map.dat", "r" );
    if ( fp == NULL )
    {
        printf ( "need a \"map.dat\" file to run\n" );
        int sysRet = system ( "./mkMap > map.dat" );
        if ( sysRet == -1 )
        {
            std::cerr << "ERROR WHILE MAKING map.dat !!!!!!!!!" << std::endl;
            return -1;
        }
        printf ( "just made you one...\n" );
        fp = fopen ( "map.dat", "r" );
        assert ( fp != NULL );
    };

    printf ( "\nmapping\n" );

    i2 = fscanf ( fp, "\n%i %i %i %s", &i1, &i7, &i8, str );
    printf ( "Successfully read %i items: %i %i %i %s\n", i2, i1, i7, i8, str );
    while ( i2 == 4 )
    {
        theMergeManager->tlkup[i1] = i7;
        theMergeManager->tid[i1] = i8;
        i2 = fscanf ( fp, "\n%i %i %i %s", &i1, &i7, &i8, str );
        if ( i2 == 4 ) printf ( "Successfully read %i items: %i %i %i %s\n", i2, i1, i7, i8, str );
    };
    fclose ( fp );


    /* start timer */

    times ( ( struct tms * ) &timesThen );

    /* -------------------------------------------- */
    /* read until we have filled our pool of events */
    /* -------------------------------------------- */

    if ( printDebug ) std::cerr << "Starting the merge procedure... (theMergeManager->inData size is " << theMergeManager->inData->size() << ")\n";

    bool firstExec = true;

    unsigned long long int evCounter = 0;

    while ( theMergeManager->inData->size() > 0  || theMergeManager->overflowEvent.size() > 0 )
    {
        if ( evCounter%1000 == 0 )
        {
            int ofEvSize = 0;

            for ( auto ofItr = theMergeManager->overflowEvent.begin(); ofItr != theMergeManager->overflowEvent.end(); ofItr++ )
            {
                ofEvSize += ofItr->second.size();
            }

            std::cerr << "Treated " << evCounter << " events (" << ofEvSize << " events currently waiting in the buffer)\r" << std::flush;
        }

        if ( printDebug ) std::cerr << "About to clear theMergeManager->Event... Size of theMergeManager->Event is " << theMergeManager->Event.size() << "\n";

        theMergeManager->Event.clear();

        if ( printDebug )
        {
            std::cerr << "Cleared theMergeManager->Event... Size of theMergeManager->overflowEvent is " << theMergeManager->overflowEvent.size() << "\n";
            std::cerr << "-*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*-\n";
            std::cerr << "-*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*-\n";
            std::cerr << "-*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*--*-*-*-*-*-\n";
        }

        if ( theMergeManager->overflowEvent.size()  == 0 )
        {
            if ( printDebug ) std::cerr << "-*-*-*-*-*- Fill the first pool of events -*-*-*-*-*-*-\n";

            for ( unsigned int i = 0; i < theMergeManager->inData->size(); i++ )
            {
                if ( printDebug ) std::cerr << "------------ Entering GTGetDiskEv for " << theMergeManager->inData->at ( i ).fileName << " (file # " << i << "\n";

                EVENT* newEv = GTGetDiskEv ( theMergeManager->inData->at ( i ), false );

                if ( newEv != NULL )
                {
                    if ( firstExec )
                    {
                        firstExec = false;

                        theMergeManager->inData->at ( i ).firstTimestamp = newEv->gd->timestamp;
                    }

                    std::pair<InDataInfo*, EVENT> newOfEv;
                    newOfEv.first = &theMergeManager->inData->at ( i );
                    newOfEv.second = *newEv;

                    if ( theMergeManager->overflowEvent.find ( newEv->gd->timestamp ) == theMergeManager->overflowEvent.end() )
                    {
                        std::vector<std::pair<InDataInfo*, EVENT>> newMapEntry;
                        newMapEntry.clear();

                        newMapEntry.push_back ( newOfEv );
                        theMergeManager->overflowEvent[newEv->gd->timestamp] = newMapEntry;
                    }
                    else
                    {
                        if ( printDebug ) std::cerr << "!!!! Timestamp already present in the map. Pushing it back as a vector...\n";

                        theMergeManager->overflowEvent[newEv->gd->timestamp].push_back ( newOfEv );
                    }
                }
            }

            if ( printDebug ) std::cerr << "++++++++++++++++++++ First pool filled! +++++++++++++++++++++++\n";
        }

        if ( printDebug ) std::cerr << "size of theMergeManager->inData is " << theMergeManager->inData->size() << "\n";
        if ( printDebug ) std::cerr << "size of theMergeManager->overflowEvent is " << theMergeManager->overflowEvent.size() << "\n";

        bool bufIsFull = false;

        auto itr = theMergeManager->overflowEvent.begin();

        auto nextItr = itr;
        nextItr++;

        unsigned long long int longestTs = -1;

        unsigned long long int ts, nextTs;

        ts = itr->first;
        nextTs = ( nextItr != theMergeManager->overflowEvent.end() ) ? nextItr->first : longestTs;

        if ( itr->second.size() > 1 )
        {
            if ( printDebug )
            {
                std::cerr << "!!!!!!!!!!!!!!!!!!!!!! Encountered a timestamp entry with multiple events... ( TS = " << ts << ")\n";
                std::cerr << "!!!!!!!!!!!!!!!!!!!!!! Multiplicity is " << itr->second.size() << "\n";
            }

            for ( unsigned m = 0; m < itr->second.size(); m++ )
            {
                InDataInfo* input = itr->second[m].first;
                EVENT evtToCopy = itr->second[m].second;

                if ( printDebug ) std::cerr << "Copying entry from file #" << input->fileNum << " to the list of events to be written on file: TS = " << evtToCopy.gd->timestamp << ")\n";

                if ( theMergeManager->Event.find ( evtToCopy.gd->timestamp ) == theMergeManager->Event.end() )
                {
                    std::vector<EVENT> newMapEntry;
                    newMapEntry.clear();

                    newMapEntry.push_back ( evtToCopy );
                    theMergeManager->Event[evtToCopy.gd->timestamp] = newMapEntry;
                }
                else
                {
                    if ( printDebug ) std::cerr << "!!!! Timestamp already present in the map. Pushing it back as a vector...\n";

                    theMergeManager->Event[evtToCopy.gd->timestamp].push_back ( evtToCopy );
                }

                if ( printDebug ) std::cerr << "------------ Entering GTGetDiskEv for " << input->fileName << " (file # " << input->fileNum << "\n";

                EVENT* newEv = GTGetDiskEv ( *input, false );

                if ( newEv != NULL )
                {
                    std::pair<InDataInfo*, EVENT> newOfEv;
                    newOfEv.first = input;
                    newOfEv.second = *newEv;

                    if ( theMergeManager->overflowEvent.find ( newEv->gd->timestamp ) == theMergeManager->overflowEvent.end() )
                    {
                        std::vector<std::pair<InDataInfo*, EVENT>> newMapEntry;
                        newMapEntry.clear();

                        newMapEntry.push_back ( newOfEv );
                        theMergeManager->overflowEvent[newEv->gd->timestamp] = newMapEntry;
                    }
                    else
                    {
                        if ( printDebug ) std::cerr << "!!!! Timestamp already present in the map. Pushing it back as a vector...\n";
                        theMergeManager->overflowEvent[newEv->gd->timestamp].push_back ( newOfEv );
                    }
                }
            }

            theMergeManager->overflowEvent.erase ( itr );
        }

        else if ( itr->second.size() == 1 )
        {
            InDataInfo* input = itr->second[0].first;
            EVENT firstFileEvt = itr->second[0].second;

            if ( printDebug ) std::cerr << "Copying entry from file #" << input->fileNum << " to the list of events to be written on file: TS = " << firstFileEvt.gd->timestamp << ")\n";

            if ( theMergeManager->Event.find ( ts ) == theMergeManager->Event.end() )
            {
                std::vector<EVENT> newMapEntry;
                newMapEntry.clear();

                newMapEntry.push_back ( firstFileEvt );
                theMergeManager->Event[ts] = newMapEntry;
            }
            else
            {
                if ( printDebug ) std::cerr << "!!!! Timestamp already present in the map. Pushing it back as a vector...\n";
                theMergeManager->Event[ts].push_back ( firstFileEvt );
            }

            while ( ts < nextTs )
            {
                if ( printDebug ) std::cerr << "------------ Entering GTGetDiskEv for " << input->fileName << " (file # " << input->fileNum << "\n";

                EVENT* newEv = GTGetDiskEv ( *input, false );

                ts = newEv->gd->timestamp;

                if ( newEv != NULL )
                {
                    if ( ts < nextTs )
                    {
                        if ( printDebug )
                        {
                            std::cerr << "-*\\_/*-> Timestamp is smaller than the one from the next file (file #";
                            std::cerr << ( ( nextItr != theMergeManager->overflowEvent.end() ) ? nextItr->second[0].first->fileNum : -1 ) << "): " << ts << " < " << nextTs << "\n";
                        }

                        if ( theMergeManager->Event.find ( newEv->gd->timestamp ) == theMergeManager->Event.end() )
                        {
                            std::vector<EVENT> newMapEntry;
                            newMapEntry.clear();

                            newMapEntry.push_back ( *newEv );
                            theMergeManager->Event[newEv->gd->timestamp] = newMapEntry;
                        }
                        else
                        {
                            if ( printDebug ) std::cerr << "!!!! Timestamp already present in the map. Pushing it back as a vector...\n";
                            theMergeManager->Event[newEv->gd->timestamp].push_back ( *newEv );
                        }
                    }
                    else
                    {
                        if ( printDebug )
                        {
                            std::cerr << "-*\\_/*-> Timestamp is bigger than the one from the next file #";
                            std::cerr << ( ( nextItr != theMergeManager->overflowEvent.end() ) ? nextItr->second[0].first->fileNum : -1 ) << "): " << ts << " > " << nextTs << "\n";
                        }

                        std::pair<InDataInfo*, EVENT> newOfEv;
                        newOfEv.first = input;
                        newOfEv.second = *newEv;

                        if ( theMergeManager->overflowEvent.find ( newEv->gd->timestamp ) == theMergeManager->overflowEvent.end() )
                        {
                            std::vector<std::pair<InDataInfo*, EVENT>> newMapEntry;
                            newMapEntry.clear();

                            newMapEntry.push_back ( newOfEv );
                            theMergeManager->overflowEvent[newEv->gd->timestamp] = newMapEntry;
                        }
                        else
                        {
                            if ( printDebug ) std::cerr << "!!!! Timestamp already present in the map. Pushing it back as a vector...\n";

                            theMergeManager->overflowEvent[newEv->gd->timestamp].push_back ( newOfEv );
                        }

                        theMergeManager->overflowEvent.erase ( itr );
                    }
                }
                else
                {
                    std::cerr << "Reached the end of  file #" << input->fileNum << "\n";
                    
                    theMergeManager->overflowEvent.erase ( itr );
                    break;
                }
            }
        }

        if ( theMergeManager->Event.size() > 0 )
        {
            for ( auto readItr = theMergeManager->Event.begin(); readItr != theMergeManager->Event.end(); readItr++ )
            {
                for ( unsigned m = 0; m < readItr->second.size(); m++ )
                {
                    theMergeManager->outData.write ( ( char* ) readItr->second[m].gd, sizeof ( GEBDATA ) );
                    theMergeManager->outData.write ( ( char* ) readItr->second[m].payload, readItr->second[m].gd->length );
                }
            }
        }
        else break;

        evCounter++;
    }

    theMergeManager->outData.close();

    return 0;

}



