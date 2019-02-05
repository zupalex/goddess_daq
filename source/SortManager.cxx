#include "SortManager.h"

int egemin = 2;

// --------------------- Global Functions ---------------------- //

void UPDSSHMEM ( time_t& t1, time_t& t2, TMapFile* mfile, char* shareMemFile )
{
    t1 = time ( NULL );
    mfile->Update();
    t2 = time ( NULL );
    printf ( "done! " );
    printf ( "shared memory size: %i\n", mfile->GetSize() );
    printf ( "update time: %li seconds ", t2 - t1 );
    printf ( "at %7.2f Mbytes/sec\n", ( float ) mfile->GetSize() / ( t2 - t1 ) / 1000000.0 );
    printf ( "to mapfile [%s] on ", shareMemFile );
    GetLocalTimeAndDate ( true );
    fflush ( stdout );

    return;
}

void CheckNoArgs ( int required, int actual, char* str )
{
    if ( required < actual )
    {
        printf ( "argument problem with chat option\n" );
        printf ( "--> %s\n", str );
        printf ( "required # arguments: %i\n", required - 1 );
        printf ( "actual   # arguments: %i\n", actual - 1 );
        printf ( "Please fix and try again, quitting...\n" );
        exit ( 0 );
    }
}

int GebTypeStr ( int type, char str[] )
{
//   printf("got type %i, %i\n", type,GEB_TYPE_DECOMP);
    if ( type == GEB_TYPE_DECOMP )
    {
        sprintf ( str, "GEB_TYPE_DECOMP      " );
    }
    else if ( type == GEB_TYPE_BGS )
    {
        sprintf ( str, "GEB_TYPE_BGS         " );
    }
    else if ( type == GEB_TYPE_RAW )
    {
        sprintf ( str, "GEB_TYPE_RAW         " );
    }
    else if ( type == GEB_TYPE_TRACK )
    {
        sprintf ( str, "GEB_TYPE_TRACK       " );
    }
    else if ( type == GEB_TYPE_S800_RAW )
    {
        sprintf ( str, "GEB_TYPE_S800_RAW    " );
    }
    else if ( type == GEB_TYPE_NSCLnonevent )
    {
        sprintf ( str, "GEB_TYPE_NSCLnonevent" );
    }
    else if ( type == GEB_TYPE_GT_SCALER )
    {
        sprintf ( str, "GEB_TYPE_GT_SCALER   " );
    }
    else if ( type == GEB_TYPE_GT_MOD29 )
    {
        sprintf ( str, "GEB_TYPE_GT_MOD29    " );
    }
    else if ( type == GEB_TYPE_S800PHYSDATA )
    {
        sprintf ( str, "GEB_TYPE_S800PHYSDATA" );
    }
    else if ( type == GEB_TYPE_G4SIM )
    {
        sprintf ( str, "GEB_TYPE_G4SIM       " );
    }
    else if ( type == GEB_TYPE_CHICO )
    {
        sprintf ( str, "GEB_TYPE_CHICO       " );
    }
    else if ( type == GEB_TYPE_NSCLNONEVTS )
    {
        sprintf ( str, "GEB_TYPE_NSCLNONEVTS " );
    }
    else if ( type == GEB_TYPE_DGS )
    {
        sprintf ( str, "GEB_TYPE_DGS         " );
    }
    else if ( type == GEB_TYPE_DGSTRIG )
    {
        sprintf ( str, "GEB_TYPE_DGSTRIG     " );
    }
    else if ( type == GEB_TYPE_DFMA )
    {
        sprintf ( str, "GEB_TYPE_DFMA        " );
    }
    else if ( type == GEB_TYPE_AGOD )
    {
        sprintf ( str, "GEB_TYPE_AGOD        " );
    }
    //else if (type == GEB_TYPE_PHOSWICH)
    //sprintf (str, "GEB_TYPE_PHOSWICH    ");
    //else if (type == GEB_TYPE_PHOSWICHAUX)
    //sprintf (str, "GEB_TYPE_PHOSWICHAUX ");

    else
    {
        sprintf ( str, "%i is unknown, maybe update 'GebTypeStr' function?", type );
        return ( 1 );
    };
//      printf("type: %s\n",str);

    return 0;

}

// --------------------- SortManager ---------------------- //

SortManager* SortManager::s_instance = nullptr;

SortManager::SortManager()
{
    execParams = new PARS();

    strcpy ( CommandFileName, "GEBSort.command" );

    nn1 = 0;
    nn2 = 0;
    nn3 = 0;
    firsttime = 1;

    nn = 0;
    ii = 0;
    nbadTS = 0;

    gebEvt = new GEB_EVENT();
    overflowGEBEv = new GEB_EVENT();

    for ( int i = 0; i < MAXCOINEV; i++ )
    {

        buffHeader[i] = new GebData;
        buffData[i] = new char[50000];

//         dfmaEvt[i] = new DFMAEVENT;
//         agodEvt[i] = new AGODEVENT;
//         dgsEvt[i] = new DGSEVENT;
    }

    tlkup = new int[NCHANNELS];
    tid = new int[NCHANNELS];

    for ( int i = 0; i < NCHANNELS; i++ )
    {
        tlkup[i] = NOTHING;
        tid[i] = NOTHING;
    }

    /* -------------------- */
    /* read in the map file */
    /* -------------------- */

    char str[STRLEN];
    int i1, i2, i7, i8;
    FILE* fp;

    fp = fopen ( "map.dat", "r" );
    if ( fp == NULL ) printf ( "need a \"map.dat\" file to run\n" );

    printf ( "\nmapping - started\n" );

    i2 = fscanf ( fp, "\n%i %i %i %s", &i1, &i7, &i8, str );
    printf ( "%i %i %i %s\n", i1, i7, i8, str );
    while ( i2 == 4 )
    {
        tlkup[i1] = i7;
        tid[i1] = i8;
        i2 = fscanf ( fp, "\n%i %i %i %s", &i1, &i7, &i8, str );
        //printf ("%i %i %i %s\n", i1, i7, i8, str);
    };
    fclose ( fp );

    printf ( "\nmapping - complete!!\n" );
}

SortManager::~SortManager()
{

}

SortManager* SortManager::sinstance()
{
    if ( s_instance == NULL )
    {
        s_instance = new SortManager();
    }

    return s_instance;
}

void SortManager::signal_catcher ( int sigval )
{
    printf ( "\nGSSort/GEBacq received signal <%i> on ", sigval );
    GetLocalTimeAndDate ( true );
    s_instance->execParams->WeWereSignalled = TRUE;
    fflush ( stdout );
}

int SortManager::ShowStatus()
{
    printf ( "read %i events; ", ( execParams->CurEvNo - execParams->firstEvent ) );
    printf ( "pars.beta=%6.4f; ", ( float ) execParams->beta );
    printf ( "time since last update %i seconds\n", ( int ) tdmp );
    printf ( "CommandFileName=\"%s\"\n", CommandFileName );
    fflush ( stdout );

    return 0;
}

int SortManager::GEBSort_read_chat ( char* name )
{
    /* declarations */

    FILE* fp, *fp1;
    char* pc, str[STRLEN] = { '0' }, str1[STRLEN] = { '0' }, str2[STRLEN] = { '0' };

    int nnj = 0, nni = 0, st;
    char* p;
    int i1, i2;

    float r1, r2, rr;
    int echo = 0, nret;

    /* open chat file */

    if ( ( fp = fopen ( name, "r" ) ) == NULL )
    {
        printf ( "error: could not open chat file: <%s>\n", name );
        exit ( 0 );
    };
    printf ( "chat file: <%s> open\n", name );
    printf ( "\n" );
    fflush ( stdout );

    /* read content and act */

    pc = fgets ( str, STRLEN, fp );

    /* rmEndComment(str, STRLEN); */

    while ( pc != NULL )
    {
        if ( echo )
        {
            printf ( "chat->%s", str );
        }
        fflush ( stdout );

        /* attemp to interpret the line */

        if ( ( p = strstr ( str, "nevents" ) ) != NULL )
        {
            if ( execParams->nEvents == 2000000000 )
            {
                nret = sscanf ( str, "%s %llu", str1, &execParams->nEvents );
                CheckNoArgs ( nret, 2, str );
            }

            printf ( "will sort a max of %llu events\n", execParams->nEvents );
            fflush ( stdout );
        }
        else if ( str[0] == 35 )
        {
            /* '#' comment line, do nothing */
            nni--; /* don't count as instruction */

        }
        else if ( str[0] == 59 )
        {
            /* ';' comment line, do nothing */
            nni--; /* don't count as instruction */

        }
        else if ( str[0] == 10 )
        {
            /* empty line, do nothing */
            nni--; /* don't count as instruction */

        }
        else if ( ( p = strstr ( str, "nocrystaltoworldrot" ) ) != NULL )
        {
            nret = sscanf ( str, "%s", str1 );
            execParams->nocrystaltoworldrot = 1;
            CheckNoArgs ( nret, 1, str );
        }
        else if ( ( p = strstr ( str, "firstEvent" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %u", str1, &execParams->firstEvent );
            CheckNoArgs ( nret, 2, str );
            printf ( "will start sorting at event %d\n", execParams->firstEvent );
            fflush ( stdout );

        }
        else if ( ( p = strstr ( str, "AGATA_data" ) ) != NULL )
        {
            nret = sscanf ( str, "%s", str1 );
            CheckNoArgs ( nret, 1, str );
            execParams->AGATA_data = 1;
            printf ( "execParams->AGATA_data=%i, process AGATA geometry\n", execParams->AGATA_data );
        }
        else if ( ( p = strstr ( str, "timewin" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %lli", str1, &execParams->dTS );
            CheckNoArgs ( nret, 2, str );
            printf ( "will sort using a time window of %lli\n", execParams->dTS );
            fflush ( stdout );
        }
        else if ( ( p = strstr ( str, "beamdir" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %f %f %f", str1, &execParams->beamdir[0], &execParams->beamdir[1], &execParams->beamdir[2] );
            CheckNoArgs ( nret, 4, str );
            rr = execParams->beamdir[0] * execParams->beamdir[0] + execParams->beamdir[1] * execParams->beamdir[1]
                 + execParams->beamdir[2] * execParams->beamdir[2];
            rr = sqrtf ( rr );
            printf ( "will use beam direction %f %f %f\n", execParams->beamdir[0], execParams->beamdir[1], execParams->beamdir[2] );
            printf ( "vector length= %f, normalizing\n", rr );
            execParams->beamdir[0] /= rr;
            execParams->beamdir[1] /= rr;
            execParams->beamdir[2] /= rr;
            printf ( "will use beam direction %f %f %f\n", execParams->beamdir[0], execParams->beamdir[1], execParams->beamdir[2] );
        }
        else if ( ( p = strstr ( str, "ndetlimits" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %i %i", str1, &execParams->ndetlimlo, &execParams->ndetlimhi );
            CheckNoArgs ( nret, 3, str );
            printf ( "execParams->ndetlimlo=%i,", execParams->ndetlimlo );
            printf ( "execParams->ndetlimhi=%i\n", execParams->ndetlimhi );
        }
        else if ( ( p = strstr ( str, "fomlimits" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %i %f %f", str1, &i1, &r1, &r2 );
            CheckNoArgs ( nret, 3, str );
            execParams->fomlo[i1] = r1;
            execParams->fomhi[i1] = r2;
        }
        else if ( ( p = strstr ( str, "modwrite" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %i", str1, &execParams->modwrite );
            CheckNoArgs ( nret, 2, str );
        }

        else if ( ( p = strstr ( str, "requiretracked" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %i", str1, &execParams->requiretracked );
            CheckNoArgs ( nret, 2, str );
            printf ( "will require tracked data before mode2 binning\n" );
        }
        else if ( ( p = strstr ( str, "tsnumwrites" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %u", str1, &execParams->tsnumwrites );
            CheckNoArgs ( nret, 2, str );

        }
        else if ( ( p = strstr ( str, "CCcal" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %s", str1, str2 );
            CheckNoArgs ( nret, 2, str );
            fp1 = fopen ( str2, "r" );
            if ( fp1 == NULL )
            {
                printf ( "WARNING: could not open CCcal file \"%s\", quit\n", str2 );
//              exit (1);
            }
            else
            {
                printf ( "reading cal file \"%s\"\n", str2 );
                nnj = 0;
                st = fscanf ( fp1, "%i %f %f", &i1, &r1, &r2 );
                while ( st == 3 )
                {
                    nnj++;
                    execParams->CCcal_offset[i1] = r1;
                    execParams->CCcal_gain[i1] = r2;
                    printf ( "CC det/offset/gain %3i %6.3f %9.6f\n", i1, execParams->CCcal_offset[i1], execParams->CCcal_gain[i1] );
                    st = fscanf ( fp1, "%i %f %f", &i1, &r1, &r2 );
                };
                printf ( "done, read %i calibration \n", nnj );
                fclose ( fp1 );
            };
        }
        else if ( ( p = strstr ( str, "SEGcal" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %s", str1, str2 );
            CheckNoArgs ( nret, 2, str );
            fp1 = fopen ( str2, "r" );
            if ( fp1 == NULL )
            {
                printf ( "WARNING: could not open SEGcal file \"%s\", quit\n", str2 );
            }
            else
            {
                printf ( "reading cal file \"%s\"\n", str2 );
                nnj = 0;
                st = fscanf ( fp1, "%i %i %f %f", &i1, &i2, &r1, &r2 );
                while ( st == 4 )
                {
                    nnj++;
                    execParams->SEGcal_offset[i1][i2] = r1;
                    execParams->SEGcal_gain[i1][i2] = r2;
                    printf ( "SEG det/seg/offset/gain %3i %2i %6.3f %9.6f\n", i1, i2, execParams->SEGcal_offset[i1][i2], execParams->SEGcal_gain[i1][i2] );
                    st = fscanf ( fp1, "%i %i %f %f", &i1, &i2, &r1, &r2 );
                };
                printf ( "done, read %i calibrations \n", nnj );
                fclose ( fp1 );
            };
        }
        else if ( ( p = strstr ( str, "abort" ) ) != NULL )
        {

            printf ( "will abort\n" );
            fclose ( fp );
            printf ( "\n" );
            printf ( "chat file: <%s> closed\n", name );
            printf ( "processed %i sort instructions and %i lines\n", nni, nnj );
            printf ( "\n" );
            fflush ( stdout );
            exit ( 0 );

        }
        else if ( ( p = strstr ( str, "include" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %s", str1, str2 );
            CheckNoArgs ( nret, 2, str );
            printf ( "will now include chatscript %s\n", str2 );
            fflush ( stdout );
            GEBSort_read_chat ( str2 );
            printf ( "done including chatscript %s\n", str2 );
            fflush ( stdout );

        }
        else if ( ( p = strstr ( str, "printevents" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %u", str1, &execParams->NumToPrint );
            CheckNoArgs ( nret, 2, str );
            printf ( "will print details of the first %i events\n", execParams->NumToPrint );
            fflush ( stdout );

        }
        else if ( ( p = strstr ( str, "multlims" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %i %i", str1, &execParams->multlo, &execParams->multhi );
            CheckNoArgs ( nret, 2, str );
            printf ( "will require mult bt: %i %i\n", execParams->multlo, execParams->multhi );
            fflush ( stdout );
        }
        else if ( ( p = strstr ( str, "beta" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %f", str1, &execParams->beta );
            CheckNoArgs ( nret, 2, str );
            printf ( "initialize execParams->Beta (v/c) correction with %f\n", execParams->beta );
            fflush ( stdout );
        }
        else if ( ( p = strstr ( str, "DumpEvery" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %i", str1, &execParams->DumpEvery );
            CheckNoArgs ( nret, 2, str );
            printf ( "will dump to output file every %i seconds\n", execParams->DumpEvery );
            fflush ( stdout );

        }
        else if ( ( p = strstr ( str, "egemin" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %i", str1, &egemin );
            CheckNoArgs ( nret, 2, str );
            printf ( "will require %i minimum germanium signal\n", egemin );
            fflush ( stdout );

        }
        else if ( ( p = strstr ( str, "gglen" ) ) != NULL )
        {
            nret = sscanf ( str, "%s %i", str1, &execParams->GGMAX );
            CheckNoArgs ( nret, 2, str );
        }
        else if ( ( p = strstr ( str, "exit" ) ) != NULL )
        {

            printf ( "will skip rest of chat file\n" );
            fclose ( fp );
            printf ( "\n" );
            printf ( "chat file: <%s> closed\n", name );
            printf ( "processed %i sort instructions and %i lines\n", nni, nnj );
            printf ( "\n" );
            fflush ( stdout );
            return ( 0 );

        }
        else
        {

            /*-----------------------------*/
            /* chatscript read error point */
            /*-----------------------------*/

            printf ( "line %2.2i in chat script, option :%s \n__not understood\n", nnj, str );
            printf ( "%i\n", str[0] );
            printf ( "aborting\n" );
            fflush ( stdout );
            exit ( 0 );
        };

        /* read next line in chat script */

        nnj++; /* line counter */
        nni++; /* instruction counter */
        pc = fgets ( str, STRLEN, fp );

    };

    /* done */

    fclose ( fp );
    printf ( "\n" );
    printf ( "chat file: <%s> closed\n", name );
    printf ( "processed %i sort instructions and %i lines\n", nni, nnj );
    printf ( "\n" );
    fflush ( stdout );
    return 0;
}

int SortManager::GEBGetEv()
{
    unsigned long long int TS;
    long long int dTS;

    gebEvt->ptgd.clear();
    gebEvt->ptinp.clear();

    int coincCounter = 0;

    bool startFilling = false;

    if ( execParams->triggerMode == "free" ) startFilling = true;

    while ( !inData.eof() && coincCounter < MAXCOINEV )
    {
        if ( overflowGEBEv->ptgd.size() > 1 )
        {
            cerr << "ERROR: The gebEvt overflow contains more than 1 element!\n";

            return -1;
        }
        else if ( overflowGEBEv->ptgd.size() == 1 )
        {
            execParams->curTS = overflowGEBEv->ptgd[0]->timestamp;

            if ( startFilling || overflowGEBEv->ptgd[0]->type == 16 || overflowGEBEv->ptgd[0]->type == 19 )
            {
                *buffHeader[coincCounter] = * ( overflowGEBEv->ptgd[0] );

                memcpy ( buffData[coincCounter], overflowGEBEv->ptinp[0], overflowGEBEv->ptgd[0]->length );

                gebEvt->ptgd.push_back ( buffHeader[coincCounter] );
                gebEvt->ptinp.push_back ( buffData[coincCounter] );

                startFilling = true;

                coincCounter++;
            }

            overflowGEBEv->ptgd.clear();
            overflowGEBEv->ptinp.clear();
        }

        inData.read ( ( char* ) buffHeader[coincCounter], sizeof ( GebData ) );

        if ( inData.gcount() != sizeof ( GebData ) || inData.fail() )
        {
            printf ( "failed to read %lu bytes for header, got %li\n", sizeof ( GebData ), inData.gcount() );
            return 1;
        }

        totBytesRead += sizeof ( GebData );

        if ( gebEvt->ptgd.size() == 0 && execParams->CurEvNo <= execParams->NumToPrint )
        {
            printf ( "read initial payload of siz=%li\n", inData.gcount() );
            fflush ( stdout );

            printf ( "__ptgd[0]->type=%2i; ", buffHeader[coincCounter]->type );
            printf ( "ptgd[0]->length=%4i; ", buffHeader[coincCounter]->length );
            printf ( "ptgd[0]->timestamp=%lli\n", buffHeader[coincCounter]->timestamp );
            fflush ( stdout );
        }

        execParams->nbytes += inData.gcount();

        inData.read ( buffData[coincCounter], buffHeader[coincCounter]->length );

        if ( inData.gcount() != buffHeader[coincCounter]->length || inData.fail() )
        {
            printf ( "failed to read %i bytes for payload, got %li\n", buffHeader[coincCounter]->length, inData.gcount() );
            return 2;
        }

        totBytesRead += buffHeader[coincCounter]->length;

        execParams->nbytes += inData.gcount();

        if ( gebEvt->ptgd.size() == 0 ) execParams->curTS = buffHeader[coincCounter]->timestamp;

        TS = buffHeader[coincCounter]->timestamp;

        // -------- TRAP FOR BAD TIMESTAMP -------- //

        if ( TS < execParams->curTS )
        {
            if ( nbadTS < 100 )
            {
                cerr << "batflag:: TS<pars.curTS, reset it at event # " << execParams->CurEvNo << "\n";
                cerr << "TS=" << TS << ", pars.curTS=" << execParams->curTS << ", DT=" << TS - execParams->curTS << "\n";
                printf ( "batflag:: TS<pars.curTS, reset it at event # %i\n", execParams->CurEvNo );
                printf ( "TS=%lli, pars.curTS=%lli, DT=%lli\n", TS, execParams->curTS, TS - execParams->curTS );
                fflush ( stdout );
            }
            execParams->curTS = TS;
            if ( nbadTS < 100 )
            {
                printf ( "new pars.curTS=%lli\n", execParams->curTS );
                printf ( "we have read %lli bytes so far\n", execParams->nbytes );
            }
            nbadTS++;

//             if ( nbadTS > 1000 )
//             {
//                 printf ( "too many bad TS, quit with error code 3\n" );
//                 fflush ( stdout );
// //              return (3);
//             }
        }

        if ( ( long long ) ( TS - execParams->curTS ) < execParams->dTS )
        {
            if ( !startFilling && ( buffHeader[coincCounter]->type == 16 || buffHeader[coincCounter]->type == 19 ) )
            {
                startFilling = true;
                execParams->curTS = buffHeader[coincCounter]->timestamp;
            }

            if ( startFilling )
            {
//             cerr << "..........................\n";
//             cerr << "Should push back in gebEvt:\n";
//             cerr << "---> type: " << buffHeader->type << "\n";
//             cerr << "---> length: " << buffHeader->length << "\n";
//             cerr << "---> timestamp: " << buffHeader->timestamp << "\n";

                gebEvt->ptgd.push_back ( buffHeader[coincCounter] );
                gebEvt->ptinp.push_back ( buffData[coincCounter] );

//             cerr << "Pushed back in gebEvt:\n";
//             cerr << "---> type: " << GEV_event->ptgd[GEV_event->ptgd.size()-1]->type << "\n";
//             cerr << "---> length: " << GEV_event->ptgd[GEV_event->ptgd.size()-1]->length << "\n";
//             cerr << "---> timestamp: " << GEV_event->ptgd[GEV_event->ptgd.size()-1]->timestamp << "\n";

                if ( execParams->CurEvNo <= execParams->NumToPrint || ( execParams->CurEvNo % execParams->modwrite == 0 ) )
                {
                    printf ( "ev# %5i ", execParams->CurEvNo );
                    char str[256];
                    GebTypeStr ( buffHeader[coincCounter]->type, str );
                    printf ( "%s ", str );
                    printf ( "%4iBytes ", buffHeader[coincCounter]->length );
                    printf ( "TS=%lli ", buffHeader[coincCounter]->timestamp );
                    printf ( "curTS=%lli ", execParams->curTS );
                    dTS = TS - execParams->curTS;
                    printf ( "dTS= %lli\n", dTS );
                    fflush ( stdout );
                }
            }
            else execParams->curTS = buffHeader[coincCounter]->timestamp;
        }
        else
        {
            overflowGEBEv->ptgd.push_back ( buffHeader[coincCounter] );
            overflowGEBEv->ptinp.push_back ( buffData[coincCounter] );
            break;
        }

        coincCounter++;
    }

    if ( coincCounter == MAXCOINEV )
    {
        cerr << "/!\\ WARNING: Maximum coincidence counter reached for event #" << execParams->CurEvNo << "\n";
    }

#if(DEBUG2)
    printf ( "complete event, next TS is %lli or  %lli out\n", TS, TS - execParams->curTS );
    printf ( "we found %i events in coincidence, timestamps are\n", GEV_event->ptgd.size() );
    for ( i = 0; i < GEV_event->ptgd.size(); i++ )
    {
        printf ( "[%i] TS=%lli\n", i, GEV_event->ptgd[i]->timestamp );
    }
    printf ( "we have read %lli bytes so far\n", execParams->nbytes );
    printf ( "GEV_event mult = %i\n", GEV_event->ptgd.size() );
    fflush ( stdout );
#endif

#if(DEBUG2)
    printf ( "GEBGetEv: done %i\n" );
    fflush ( stdout );
#endif

    return 0;
}

int SortManager::GEBacq ( char* ChatFileName )
{
    /* declarations */

    int NprintEvNo = 0, in, zero = 0;

    int st = 0, eov = 0, i1, i2, j, nret, siz;
    char str[256], str1[256];
    FILE* fp;
    time_t t1, t2;
    char* p, buffer[512];
    FILE* fp0;
    int ir[NUMAGATAPOS], dummy_i[NUMAGATAPOS];
    ;
    double d1, nsec;
    float r1, r2, r3;
    long long int tcur = 0;
    unsigned int typehit[MAX_GEB_TYPE];
    FILE* TSfile;
    //  TH2F *dtbtev;//unused
    unsigned long long int firtsTSinEvent, dTS;

//  ConnectionRetryCount = 10;

    /* root spectra pointers */

    TH1* hhtemp;

    TList* zlist;
    TIterator* hiterator;

    TMapFile* mfile = NULL;

    printf ( "\n" );
    printf ( "executing UserDeclare.h code\n" );
    printf ( "\n" );

    /*-------*/
    /* Hello */
    /*-------*/

    printf ( "\n" );
    printf ( "GEBsort running on: " );
    GetLocalTimeAndDate ( true );
    printf ( "\n" );

    /*------------*/
    /* initialize */
    /*------------*/

    for ( int i = 0; i < MAX_GEB_TYPE; i++ )
    {
        typehit[i] = 0;
    }

    printf ( "\n" );
    printf ( "initializing\n" );
    printf ( "\n" );
    fflush ( stdout );

    if ( execParams->nEvents == 0 )
    {
        printf ( "Number of events to treat not been specified... Initialized to an arbitrary value of 2000000000" );
        printf ( "\n" );
    }

    printf ( "\n******** Number of Events which will be treated (max) : %llu **********\n", execParams->nEvents );

    execParams->nEvents = ( execParams->nEvents == 0 ) ? 2000000000 : execParams->nEvents;
    execParams->WeWereSignalled = FALSE; /* signal  */
    execParams->UseRootFile = FALSE;
    execParams->SizeShareMemFile = FALSE;
    execParams->firstEvent = 0;
    execParams->GSudpPort = 1101;
    execParams->NumToPrint = 25;
    execParams->DumpEvery = 10;
    execParams->dTS = 500;
    execParams->nbytes = 0;
    execParams->beta = 0;
    execParams->modwrite = 1000;
    execParams->tsnumwrites = 100;
    execParams->nocrystaltoworldrot = 0;
    execParams->multlo = 1;
    execParams->multhi = 20;
    execParams->requiretracked = 0;

    /* get the GRETINA rotation matrices */

    sprintf ( str, "crmat.LINUX" );
    in = open ( str, O_RDONLY, 0 );
    if ( in > 0 )
    {
        printf ( "%s is open (input) binary format\n", str );
    }
    else
    {
        printf ( "could not open %s\n", str );
        exit ( 1 );
    };
    siz = read ( in, ( char* ) execParams->crmat, sizeof ( execParams->crmat ) );
    printf ( "read %i bytes into execParams->crmat\n", siz );
    close ( in );

    /* get the AGATA rotational matrices */

    printf ( "read in AGATA rotation and translation data\n" );

    fp0 = fopen ( "AGATA_crmat.dat", "r" );
    if ( fp0 != NULL )
    {
        printf ( "AGATA_crmat.dat is open for reading\n" );

        j = 0;
        for ( int i = 0; i < 180; i++ )
        {
            memset ( buffer, zero, sizeof ( buffer ) );
            if ( !fgets ( buffer, 150, fp0 ) )
            {
                cerr << "ERROR WHILE READING AGATA_crmat.dat !!!!!" << endl;
                return -1;
            }

            sscanf ( buffer, "%i %i %lf %lf %lf ", ir, dummy_i, &execParams->TrX[j], &execParams->TrY[j], &execParams->TrZ[j] );

            memset ( buffer, zero, sizeof ( buffer ) );
            if ( !fgets ( buffer, 150, fp0 ) )
            {
                cerr << "ERROR WHILE READING AGATA_crmat.dat !!!!!" << endl;
                return -1;
            }
            sscanf ( buffer, "%i %lf %lf %lf  ", dummy_i, &execParams->rotxx[j], &execParams->rotxy[j], &execParams->rotxz[j] );

            memset ( buffer, zero, sizeof ( buffer ) );
            if ( !fgets ( buffer, 150, fp0 ) )
            {
                cerr << "ERROR WHILE READING AGATA_crmat.dat !!!!!" << endl;
                return -1;
            }
            sscanf ( buffer, "%i %lf %lf %lf  ", dummy_i, &execParams->rotyx[j], &execParams->rotyy[j], &execParams->rotyz[j] );

            memset ( buffer, zero, sizeof ( buffer ) );
            if ( !fgets ( buffer, 150, fp0 ) )
            {
                cerr << "ERROR WHILE READING AGATA_crmat.dat !!!!!" << endl;
                return -1;
            }
            sscanf ( buffer, "%i %lf %lf %lf  ", dummy_i, &execParams->rotzx[j], &execParams->rotzy[j], &execParams->rotzz[j] );

            j++;
        }
        printf ( "read %i AGATA rotational/translational matrices\n", j );
    }
    else
    {
        printf ( "Error: ascii AGATA_crmat.dat not found, quit\n" );
        exit ( 1 );
    };

//     for ( j = 0; j < 180; j++ )
//     {
//         printf ( "%2i: \n", j );
//         printf ( "Tr?: %10.5f %10.5f %10.5f\n", execParams->TrX[j], execParams->TrX[j], execParams->TrZ[j] );
//         printf ( "rx?: %10.5f %10.5f %10.5f\n", execParams->rotxx[j], execParams->rotxy[j], execParams->rotxz[j] );
//         printf ( "ry?: %10.5f %10.5f %10.5f\n", execParams->rotyx[j], execParams->rotyy[j], execParams->rotyz[j] );
//         printf ( "rz?: %10.5f %10.5f %10.5f\n", execParams->rotzx[j], execParams->rotzy[j], execParams->rotzz[j] );
//     }

    /*------------------*/
    /* read chat script */
    /*------------------*/

    GEBSort_read_chat ( ChatFileName );

    printf ( "checking proper input of chat file...\n" );
    if ( execParams->InputSrc == NOTDEF )
    {
        printf ( "you must specify an input source\n" );
        exit ( 1 );
    }

    else if ( execParams->InputSrc == DISK )
    {
        /* attempt to open input file */

        if ( !execParams->userFilter.empty() && execParams->userFilter != "none" )
        {
            execParams->cleanedMerged.open ( execParams->userFilter.c_str(), ios_base::out | ios_base::trunc );
        }

        inData.open ( execParams->GTSortInputFile, ios_base::in | ios_base::binary );
        if ( !inData.is_open() )
        {
            printf ( "could not open\"%s\"; quit\n", execParams->GTSortInputFile );
            exit ( 1 );
        }
        else
        {
            printf ( "input file \"%s\" is open\n", execParams->GTSortInputFile );
        }

        /* find the very first GEB header to find start TS */

        inData.read ( ( char* ) buffHeader[0], sizeof ( GebData ) );

        printf ( "siz=%li;", inData.gcount() );
        printf ( "ptgd[i]->type=%2i; ", buffHeader[0]->type );
        printf ( "ptgd[i]->length=%4i; ", buffHeader[0]->length );
        printf ( "ptgd[i]->timestamp=%lli\n", buffHeader[0]->timestamp );
        fflush ( stdout );

        execParams->curTS = buffHeader[0]->timestamp;
        printf ( "start TS is %lli\n", execParams->curTS );
        cout << "position in file is " << inData.tellg() << "\n";

        /* reopen */

        inData.seekg ( 0, inData.beg );
        cout << "Setting back the initial position to 0: " << inData.tellg() << "\n";
    }

    else
    {
        printf ( "input source not recognized, quit\n" );
        return 1;
    };

    printf ( "input source is set up, supposedly\n" );

    /*---------------------*/
    /* other sanety checks */
    /*---------------------*/

    if ( execParams->InputSrc == NOTDEF )
    {
        printf ( "you must specify an input source!\n" );
        printf ( "quitting...\n" );
        return 1;
    };

    if ( execParams->UseShareMemFile && execParams->UseRootFile )
    {
        printf ( "you cannot use shared memory and a root disk\n" );
        printf ( "at the same time!\n" );
        return 1;
    };

    /* force user to declare intension with root file */
    /* so I can't be blamed for any overwrites!!      */

    if ( !execParams->UseShareMemFile ) if ( ( p = strstr ( execParams->ROOTFileOption, "UNDEFINED" ) ) != NULL )
        {
            printf ( "for root files you must specify either:\n" );
            printf ( "\n" );
            printf ( "    ROOTFileOption RECREATE\n" );
            printf ( "or\n" );
            printf ( "    ROOTFileOption UPDATE\n" );
            printf ( "\n" );
            printf ( "please modify your chat file and try again\n" );
            return -1;
        };

    printf ( "\n" );
    printf ( "executing ShareMemFileUserInit.h code\n" );
    printf ( "\n" );

    /*-----------------------------------*/
    /* if we are going to use the        */
    /* shared map file, create it!       */
    /* be careful about the map address! */
    /*-----------------------------------*/

    if ( execParams->UseShareMemFile )
    {
        printf ( "\n" );

        if ( execParams->StartMapAddress != 0 )
        {
            TMapFile::SetMapAddress ( ( Long_t ) execParams->StartMapAddress );
            printf ( "shared mem start address set to 0x%8.8x\n", execParams->StartMapAddress );
        }
        else
        {
            printf ( "will use system default for shared mem start address\n" );
        }

        mfile = TMapFile::Create ( execParams->ShareMemFile, "RECREATE", execParams->SizeShareMemFile, "GS shared mem" );
        if ( mfile == NULL )
        {
            printf ( "failed to create TMapFile\n" );
            return -1;
        };

        printf ( "shared memory [%s] created, size: %s bytes\n", execParams->ShareMemFile, execParams->ShareMemFile );
        fflush ( stdout );
        //mfile->Print ();
        printf ( "\n" );
    }

    NprintEvNo = 0;
    execParams->CurEvNo = 0;

    if ( !execParams->ShareMemFile )
    {
        execParams->DumpEvery = 2000000000;
        printf ( "\n" );
        printf ( "_since rootfile: setting `execParams->DumpEvery` to infinity..!\n" );
        printf ( "\n" );
    };

    /* delete any command file */

    sprintf ( str, "\\rm -f %s", CommandFileName );
    if ( system ( str ) )
    {
    }
    printf ( "deleted %s\n", str );

    gConfig = new GoddessConfig ( execParams->runNumber, ( string ) execParams->GeomFile, ( string ) execParams->ConfigFile, execParams->sx3EnAdjustFile,
                                  execParams->qqq5EnAdjustFile );

    // Calculate the beta of the beam from its energy, charge, mass and the target properties. Requires a corresponding energy loss lookup table

    if ( execParams->noDoppler ) execParams->beta = 0;
    else if ( gConfig->reacInfos["Beam Mass"] > 0 && gConfig->reacInfos["Beam Charge"] > 0 && gConfig->reacInfos["Beam Energy"] > 0
              && gConfig->reacInfos["Target Thickness"] > 0 )
    {
        string targetStr_;
        if ( gConfig->reacInfos.find ( "TargetType" ) != gConfig->reacInfos.end() ) targetStr_ = gConfig->reacInfos["Target Type"];
        else targetStr_ = "*";

        double beamERemaining = TryGetRemainingEnergy ( "mass_db.dat", gConfig->reacInfos["Beam Mass"], gConfig->reacInfos["Beam Charge"],
                                gConfig->reacInfos["Beam Energy"], gConfig->reacInfos["Target Thickness"] / 2., 0.01, targetStr_, gConfig->reacInfos["Target Density"],
                                "./ranges_tables", "Interpolation" );

        float bMass;

        if ( gConfig->reacInfos["Beam Atomic Mass"] > 0 ) bMass = gConfig->reacInfos["Beam Atomic Mass"];
        else bMass = gConfig->reacInfos["Beam Mass"];

        execParams->beta = sqrt ( 1 - pow ( 931.494 * bMass, 2 ) / pow ( beamERemaining + 931.494 * bMass, 2 ) );

        cout << "Energy loss of the beam in the target calculated using: Mass = " << bMass << " / Charge = " << gConfig->reacInfos["Beam Charge"];
        cout << " / Energy = " << gConfig->reacInfos["Beam Energy"] << " / Target Thickness = " << gConfig->reacInfos["Target Thickness"] << endl;
        cout << "Beta of the beam updated to take into account the energy loss in the target: New value = " << execParams->beta << endl;
    }

    /*------------------------------------------*/
    /* if we are using root file, then either   */
    /* read in old rootfile or create a new one */
    /*------------------------------------------*/

    if ( !execParams->UseShareMemFile )
    {
        if ( execParams->UpdateRootFile )
        {
            /* check here whether the old root file exists */

            fp = fopen ( execParams->ROOTFile, "r" );
            if ( fp == NULL )
            {
                printf ( "could not open old rootfile: %s\n", execParams->ROOTFile );
                printf ( "the old rootfile must exist if you  \n" );
                printf ( "want to use the UPDATE option\n" );
                printf ( "aborting...\n" );
                return 0;
            };
            fclose ( fp );

            /* read in old root file */

            execParams->f1 = NULL;
            execParams->f1 = new TFile ( execParams->ROOTFile, "UPDATE" );
            printf ( "read old root file <%s>\n", execParams->ROOTFile );
            if ( !execParams->f1->IsOpen() )
            {
                printf ( "could not open file....\n\n" );
                return -1;
            };
            printf ( "base=<%s>\n", execParams->f1->GetPath() );
            //execParams->f1->Print ();
        }

        else
        {
            /* create the rootfile */

            execParams->f1 = NULL;
            execParams->f1 = new TFile ( execParams->ROOTFile, "RECREATE" );
            printf ( "root file <%s>\n", execParams->ROOTFile );
            if ( !execParams->f1->IsOpen() )
            {
                printf ( "could not open file....\n\n" );
                return -1;
            }
            printf ( "base=<%s>\n", execParams->f1->GetPath() );
            execParams->f1->SetCompressionLevel ( 0 );
            execParams->f1->Flush();
        }
    }

    /* update shared mem with minimal info       */
    /* so it is not empty before the first update */
    /* shared memory wellness checkpoint          */

    if ( execParams->UseShareMemFile )
    {
        printf ( "\n" );
        printf ( "Note: if the command below fails,\n" );
        printf ( "increase the shared memory size!\n" );
        printf ( "\n" );
        printf ( "updating empty shared mem file... " );
        fflush ( stdout );
        //      mfile->Update ();
        UPDSSHMEM ( t1, t2, mfile, execParams->ShareMemFile );
        printf ( "Done!\n" );
        printf ( "\n" );
        fflush ( stdout );
    };

    TSfile = fopen ( "TS.list", "w" );

    /*--------------------------------*/
    /* setup the root spectra we need */
    /*--------------------------------*/

    if ( !execParams->noHists )
    {
        execParams->f1->cd();

        execParams->histDir = execParams->f1->mkdir ( "hists" );
        execParams->treeDir = execParams->f1->mkdir ( "trees" );
        execParams->f1->Write ( 0, TObject::kWriteDelete );
        execParams->histDir->cd();
    }
    /* spectra for different types of data */

    theGRProcessor = new GRProcessor ( 110, tlkup, tid, &ng, execParams );

    theDGSProcessor = new DGSProcessor();
    theDGSProcessor->SupDGS();
    theGretProcessor = new GretProcessor();

    if ( !execParams->noHists ) execParams->histDir->cd();

    theGODProcessor = new GODProcessor ( tlkup, tid, &ng, execParams, gConfig, &numDGOD, &numAGOD, &lastTS );
    theGODProcessor->SupDGOD();

    if ( !execParams->noHists ) execParams->histDir->cd();

    theGODProcessor->SupAGOD();

    if ( !execParams->noHists ) execParams->histDir->cd();

    theGODProcessor->SupGOD();

    if ( !execParams->noHists ) execParams->histDir->cd();

    if ( !execParams->noHists ) execParams->f1->SetCompressionLevel ( 0 );
    if ( !execParams->noHists ) execParams->histDir->Write ( 0, TObject::kOverwrite );
    if ( !execParams->noHists ) execParams->f1->Flush();

    printf ( "we have define the following ROOT spectra:\n" );

    execParams->wlist = gDirectory->GetList();

    /* azi only in detector systems for now... */

    if ( execParams->AGATA_data == 0 )
    {
        /* find the detector angles from crmat */

        printf ( "GRETINA detector angles\n" );

        for ( int i = 1; i <= MAXGTMODNO; i++ )
            for ( j = 0; j < 4; j++ )
            {
                printf ( "mod %2i, crystal %1i [%3i]: ", i, j, 4 * i + j );
                printf ( "(%6.2f ", execParams->crmat[i][j][0][3] );
                printf ( "%6.2f ", execParams->crmat[i][j][1][3] );
                printf ( "%6.2f) ", execParams->crmat[i][j][2][3] );

                r1 = execParams->crmat[i][j][0][3] * execParams->crmat[i][j][0][3] + +execParams->crmat[i][j][1][3] * execParams->crmat[i][j][1][3]
                     + +execParams->crmat[i][j][2][3] * execParams->crmat[i][j][2][3];
                r1 = sqrtf ( r1 );
                r2 = execParams->beamdir[0] * execParams->crmat[i][j][0][3] / r1 + execParams->beamdir[1] * execParams->crmat[i][j][1][3] / r1
                     + execParams->beamdir[2] * execParams->crmat[i][j][2][3] / r1;
                execParams->modCCang[i][j] = acosf ( r2 );
                printf ( "pol %7.2f ,  ", execParams->modCCang[i][j] / M_PI * 180 );
                r3 = atan2f ( execParams->crmat[i][j][1][3], execParams->crmat[i][j][2][3] );
                printf ( "azi %7.2f; ", r3 / M_PI * 180 );

                r1 = 1.0 - execParams->beta * execParams->beta;
                execParams->modCCdopfac[4 * i + j] = sqrtf ( r1 ) / ( 1.0 - execParams->beta * cosf ( execParams->modCCang[i][j] ) );

                printf ( "modCCdopfac %6.4f", execParams->modCCdopfac[4 * i + j] );
                printf ( "\n" );
            }
    }

    else if ( execParams->AGATA_data == 1 )
    {
        printf ( "AGATA detector angles\n" );

        for ( int i = 0; i < 180; i++ )
        {
            r1 = execParams->TrX[i] * execParams->TrX[i] + execParams->TrY[i] * execParams->TrY[i] + execParams->TrZ[i] * execParams->TrZ[i];
            r1 = sqrtf ( r1 );
            printf ( "%3i: %10.5f %10.5f %10.5f; ", i, execParams->TrX[i], execParams->TrY[i], execParams->TrZ[i] );

            r2 = execParams->beamdir[0] * execParams->TrX[i] / r1 + execParams->beamdir[1] * execParams->TrY[i] / r1
                 + execParams->beamdir[2] * execParams->TrZ[i] / r1;
            if ( r2 < -1.0 || r2 > 1.0 )
            {
                printf ( "r2 out of range=%f\n", r2 );
                exit ( 1 );
            };
            i1 = i / 3;
            i2 = i - i1;
            //          printf ("[%f %f] ", r2, acosf (r2)/ M_PI * 180);
            execParams->modCCang[i1][i2] = acosf ( r2 );
            printf ( "pol %7.2f , ", execParams->modCCang[i1][i2] / M_PI * 180 );
            r3 = atan2f ( execParams->TrY[i], execParams->TrX[i] );
            printf ( "azi %7.2f; ", r3 / M_PI * 180 );

            r1 = 1.0 - execParams->beta * execParams->beta;
            execParams->modCCdopfac[i] = sqrtf ( r1 ) / ( 1.0 - execParams->beta * cosf ( execParams->modCCang[i1][i2] ) );

            printf ( "modCCdopfac %6.4f", execParams->modCCdopfac[i] );
            printf ( "\n" );

        }

//         printf ( "\ntable\n\n" );
//         r1 = 0;
//         while ( r1 <= M_PI )
//         {
//             r2 = cosf ( r1 );
//             printf ( "r1=%frad %fdeg, cosf(r1)=r2=%f, acosf(r2)=%f\n", r1, r1 * 57.2958, r2, acosf ( r2 ) );
//             r1 += M_PI / 100.0;
//         }
    }

    /*----------------------*/
    /* setup signal catcher */
    /*----------------------*/

#ifdef LINUX
    signal ( SIGHUP, signal_catcher );
#endif
#ifdef SOLARIS
    sigset ( SIGHUP, signal_catcher );
#endif
    printf ( "setup signal catcher\n" );

    /*---------------*/
    /* start sorting */
    /*---------------*/

    printf ( "started sorting... " );
    if ( execParams->InputSrc == DISK )
    {
        printf ( "from disk...\n" );
    }
    else if ( execParams->InputSrc == GEB )
    {
        printf ( "from GEB...\n" );
    }
    else if ( execParams->InputSrc == NET )
    {
        printf ( "from net... SHOULD NOT HAPPEN\n" );
        exit ( 1 );
    };
    printf ( "\n" );
    fflush ( stdout );

    int userFlagedEvtCounter = 0;

    tdmplast = time ( NULL );

    unsigned long long int totBinSize = 0;

    inData.seekg ( 0, inData.end );

    totBinSize = inData.tellg();

    inData.seekg ( 0, inData.beg );

    totBytesRead = 0;

    while ( ( execParams->CurEvNo - execParams->firstEvent ) < execParams->nEvents && !inData.eof() )
    {
        /*----------------*/
        /* get next event */
        /*----------------*/

#if(0)
        printf ( "calling GEBGetEv, execParams->CurEvNo=%i\n", execParams->CurEvNo );
#endif

//         cerr << "========================= Entering GEBGetEv =========================\n";

        st = GEBGetEv();

//         cerr << "------- Exiting GEBGetEv -------\n";
//         for ( unsigned int i = 0; i < gebEvt->ptgd.size(); i++ )
//         {
//             cerr << "---> type: " << gebEvt->ptgd[i]->type << "\n";
//             cerr << "---> length: " << gebEvt->ptgd[i]->length << "\n";
//             cerr << "---> timestamp: " << gebEvt->ptgd[i]->timestamp << "\n";
//             cerr << "....................\n";
//         }

//         printf ( "st=%i\n", st );
//         printf ( "gebEvt mult =%i\n", gebEvt->ptgd.size() );
//         fflush ( stdout );
//         if ( 1 )
//         {
//             exit ( 0 );
//         }

        int gammatype;

        if ( gebEvt->ptgd.size() > 0 )
        {
            tcur = gebEvt->ptgd[0]->timestamp;

            /* count data types */

            firtsTSinEvent = LLONG_MAX;

            for ( unsigned int i = 0; i < gebEvt->ptgd.size(); i++ )
            {
                gammatype = gebEvt->ptgd[i]->type;

                if ( execParams->CurEvNo < execParams->tsnumwrites )
                {
                    if ( i == 0 )
                    {
                        fprintf ( TSfile, "\n" );
                    }

                    GebTypeStr ( gebEvt->ptgd[i]->type, str );
                    fprintf ( TSfile, "%4i/%2i: (%2i,%s) TS=%20lli; ", execParams->CurEvNo, i, gebEvt->ptgd[i]->type, str, gebEvt->ptgd[i]->timestamp );

                    fprintf ( TSfile, "dT=%lli\n", gebEvt->ptgd[i]->timestamp - TSprev );

                    TSprev = gebEvt->ptgd[i]->timestamp;
                }

                if ( gebEvt->ptgd[i]->type > 0 && gebEvt->ptgd[i]->type < MAX_GEB_TYPE )
                {
                    typehit[gebEvt->ptgd[i]->type]++;
                }

                if ( gebEvt->ptgd[i]->timestamp < firtsTSinEvent )
                {
                    firtsTSinEvent = gebEvt->ptgd[i]->timestamp;
                }
            }

            /* fill dtbtev spectrum */

            for ( unsigned int i = 0; i < gebEvt->ptgd.size(); i++ )
            {
                dTS = gebEvt->ptgd[i]->timestamp - firtsTSinEvent;
                d1 = ( double ) dTS;
                //if (d1 >= (double) 0 && d1 < RATELEN)
                //dtbtev->Fill (d1, gebEvt.ptgd[i]->type, 1);
            }

            /* statistics */

            execParams->CurEvNo++;
            NprintEvNo++;

            if ( execParams->CurEvNo <= execParams->NumToPrint )
            {
                printf ( "GEBGetEv returned st=%i\n", st );
                printf ( "we have read %lli bytes; ", execParams->nbytes );
                printf ( "CurEvNo=%i\n", execParams->CurEvNo );
                fflush ( stdout );
            }

            /* debug print some events */

            if ( execParams->CurEvNo <= execParams->NumToPrint )
            {
                printf ( "\n+++++++++++++++++++++++++++++++\n" );
                printf ( "*start event # %i with multiplicity %lu looks like this:\n", execParams->CurEvNo, gebEvt->ptgd.size() );
                for ( unsigned int i = 0; i < gebEvt->ptgd.size(); i++ )
                {
                    GebTypeStr ( gebEvt->ptgd[i]->type, str );
                    printf ( "%2i> %2i, %s, TS=%lli\n", i, gebEvt->ptgd[i]->type, str, gebEvt->ptgd[i]->timestamp );
                }
            }

//             printf ( "debug quit\n" );
//             exit ( 0 );

            if ( execParams->CurEvNo <= execParams->NumToPrint )
            {
                printf ( "*end of event # %i\n", execParams->CurEvNo );
                printf ( "+++++++++++++++++++++++++++++++\n" );
            }
            
            if ( gammatype == 14 )
            {
                theDGSProcessor->BinDgs ( gebEvt, dgsEvt );
                theGODProcessor->BinDGOD ( gebEvt, dfmaEvt, dgsEvt );
                theGODProcessor->BinAGOD ( gebEvt, agodEvt, dgsEvt );
                if ( theGODProcessor->BinGOD ( gebEvt, agodEvt, dfmaEvt, dgsEvt ) )

                {
                    userFlagedEvtCounter++;

                    if ( execParams->userFilter != "none" )
                    {
                        for ( unsigned int i = 0; i < gebEvt->ptgd.size(); i++ )
                        {
                            execParams->cleanedMerged.write ( ( char* ) gebEvt->ptgd[i], sizeof ( GebData ) );
                            execParams->cleanedMerged.write ( gebEvt->ptinp[i], gebEvt->ptgd[i]->length );
                        }
                    }
                }
            }
            
            if ( gammatype == 1 )
            {
                theGretProcessor->Bingr ( gebEvt,grEvt );
		theGODProcessor->BinDGOD ( gebEvt, dfmaEvt, grtrEvt );
                theGODProcessor->BinAGOD ( gebEvt, agodEvt, grtrEvt );
                if ( theGODProcessor->BinGOD ( gebEvt, agodEvt, dfmaEvt, grtrEvt ) )
                {
                    userFlagedEvtCounter++;

                    if ( execParams->userFilter != "none" )
                    {
                        for ( unsigned int i = 0; i < gebEvt->ptgd.size(); i++ )
                        {
                            execParams->cleanedMerged.write ( ( char* ) gebEvt->ptgd[i], sizeof ( GebData ) );
                            execParams->cleanedMerged.write ( gebEvt->ptinp[i], gebEvt->ptgd[i]->length );
                        }
                    }
                }
            }
        }
        //    assert (execParams->InputSrc == DISK);

        /*---------------------*/
        /* house keeping...... */
        /* done every so often */
        /*---------------------*/

        if ( execParams->CurEvNo % 10000 == 0 )
        {
            cerr << "Event: " << left << setw ( 12 ) << execParams->CurEvNo << " / Bytes read: " << setw ( 13 ) << totBytesRead << " out of " << setw ( 13 )
                 << totBinSize;
            cerr << " ( " << right << setw ( 6 ) << setprecision ( 2 ) << fixed << ( float ) totBytesRead / totBinSize * 100. << " % )\r" << flush;
            /* calc time since last dump */

            tdmp = time ( NULL );
            tdmp -= tdmplast;
        }

        /*-----------------------------------------------------------*/
        /* dump all spectra on signal or dump every execParams->DumpEvery events */
        /* or respond to 'interactive' command...................... */
        /*-----------------------------------------------------------*/

        if ( execParams->WeWereSignalled || ( int ) tdmp >= execParams->DumpEvery )
        {
            /* disarm signal */

            execParams->WeWereSignalled = FALSE;

            /* check for command file */

            fp = fopen ( CommandFileName, "r" );
            if ( fp != NULL )
            {

                printf ( "found command file: %s\n", CommandFileName );
                if ( fgets ( str, STRLEN, fp ) )
                {
                }
                printf ( "with command: %s\n", str );

                if ( ( p = strstr ( str, "DumpEvery" ) ) != NULL )
                {

                    sscanf ( str, "%s %i", str1, &execParams->DumpEvery );
                    printf ( "will dump to output file every %i seconds\n", execParams->DumpEvery );
                    fflush ( stdout );

                }
                else if ( ( p = strstr ( str, "printevents" ) ) != NULL )
                {
                    /* reset print event counter */

                    nret = sscanf ( str, "%s %i", str1, &i1 );
                    if ( nret == 2 )
                    {
                        execParams->NumToPrint = i1;
                    }
                    printf ( "will print %i events\n", execParams->NumToPrint );
                    NprintEvNo = 0;

                }
                else if ( ( p = strstr ( str, "status" ) ) != NULL )
                {

                    ShowStatus();

                }
                else if ( ( p = strstr ( str, "stopsort" ) ) != NULL )
                {
                    /* simulate end of event to stop sort */

                    eov = 1;

                }
                else if ( ( p = strstr ( str, "zapall" ) ) != NULL )
                {

                    /* zap spectra */
                    if ( execParams->UseShareMemFile )
                    {
                        zlist = mfile->GetDirectory()->GetList();
                        hiterator = zlist->MakeIterator();
                        while ( ( hhtemp = ( TH1* ) hiterator->Next() ) )
                        {
                            hhtemp->Reset();
                        }
                        printf ( "all spectra were zapped @ " );
                        GetLocalTimeAndDate ( true );
                        fflush ( stdout );

                        /* update */

                        printf ( "updating shared mem... " );
                        UPDSSHMEM ( t1, t2, mfile, execParams->ShareMemFile );
                        ;
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
                else if ( ( p = strstr ( str, "zap" ) ) != NULL )
                {
                    /* extract spectrum name */

                    sscanf ( str, "%s %s", str1, execParams->spname );
                    hhtemp = ( TH1D* ) gROOT->FindObject ( execParams->spname );
                    if ( execParams->UseShareMemFile )
                    {
                        hhtemp = ( TH1* ) mfile->Remove ( execParams->spname );
                        if ( hhtemp != NULL )
                        {
                            //hhtemp->Print ();
                            hhtemp->Reset();
                            mfile->Add ( hhtemp, execParams->spname );
                            mfile->Update ( hhtemp );
                        }
                        printf ( "spectrum %s zapped @ ", execParams->spname );
                        GetLocalTimeAndDate ( true );
                        fflush ( stdout );
                        /* update */
                    }
                    else
                    {
                        /* do nothing */
                    };

                }
                else
                {
                    printf ( "command not understood\n" );
                }

                /* delete command file */

                fclose ( fp );
                sprintf ( str, "\\rm %s", CommandFileName );
                if ( system ( str ) )
                {
                }
                printf ( "%s\n", str );

            }
            else
            {
                printf ( "\"%s\" was not found\n", CommandFileName );

                /* update sh mem or writeout root file */

                printf ( "time since last dump: %i second(s)\n", ( int ) tdmp );
                tdmp = 0;
                if ( !execParams->UseShareMemFile )
                {
                    printf ( "Writing ROOT file\n" );

                    execParams->f1 = execParams->mainTree->GetCurrentFile();

                    if ( !execParams->noHists ) execParams->histDir->Write ( 0, TObject::kOverwrite );
                    //execParams->treeDir->Write(0,TObject::kWriteDelete);
                    execParams->f1->Flush();

                }

                else
                {
                    printf ( "updating shared mem... " );

                    UPDSSHMEM ( t1, t2, mfile, execParams->ShareMemFile );
                    ;
                    ShowStatus();
                }

                tdmplast = time ( NULL );
            }

            printf ( "continuing the sort...\n" );
            fflush ( stdout );
        }
    }

    cerr << "\n";

    cout << "\nTerminating Sorting Process because ";

    if ( ( execParams->CurEvNo - execParams->firstEvent ) >= execParams->nEvents ) cout << "we treated " << execParams->CurEvNo << " out of " << execParams->nEvents
                << "\n";
    else if ( !inData.eof() ) cout << "we reached the end of the file...\n";

    cout << "we have read " << execParams->nbytes << "bytes\n";
    cout << "CurEvNo = " << execParams->CurEvNo << "\n";
    fflush ( stdout );

    /* terminate sort */

    cerr << "GEBSort done treating " << execParams->CurEvNo << " events...\n";
    cerr << userFlagedEvtCounter << " events fulfilled the user filter(s)...\n";
    cerr << nbadTS << " event(s) had a bad timestamp\n";

    if ( nbadTS == 0 )
    {
        cerr << "´´´´´´´´´´´´´´´´´´´´´´´¶¶¶¶¶¶¶¶" << endl;
        cerr << "´´´´´´¶¶¶¶¶´´´´´´´´¶¶¶¶´´´´´´´´¶¶¶¶" << endl;
        cerr << "´´´´´¶´´´´´¶´´´´¶¶´´´´´´´´´´´´´´´´´´¶¶" << endl;
        cerr << "´´´´´¶´´´´´¶´´´¶¶´´´´´´¶¶´´´´¶¶´´´´´´´¶¶" << endl;
        cerr << "´´´´´¶´´´´¶´´¶¶´´´´´´´´¶¶´´´´¶¶´´´´´´´´¶¶" << endl;
        cerr << "´´´´´´¶´´´¶´´´¶´´´´´´´´´´´´´´´´´´´´´´´´´¶¶" << endl;
        cerr << "´´´´¶¶¶¶¶¶¶¶¶¶¶¶´´´´´´´´´´´´´´´´´´´´´´´´´¶¶" << endl;
        cerr << "´´´¶´´´´´´´´´´´´¶´´´¶¶´´´´´´´´´´´¶¶´´´´´´¶¶" << endl;
        cerr << "´´¶¶´´´´´´´´´´´´¶´´´¶¶´´´´´´´´´´´¶¶´´´´´´¶¶" << endl;
        cerr << "´¶¶´´´¶¶¶¶¶¶¶¶¶¶¶´´´´´¶¶´´´´´´´¶¶´´´´´´´¶¶" << endl;
        cerr << "´¶´´´´´´´´´´´´´´´¶´´´´´´¶¶¶¶¶¶¶´´´´´´´´¶¶" << endl;
        cerr << "´¶¶´´´´´´´´´´´´´´¶´´´´´´´´´´´´´´´´´´´´¶¶" << endl;
        cerr << "´´¶´´´¶¶¶¶¶¶¶¶¶¶¶¶´´´´´´´´´´´´´´´´´´´¶¶" << endl;
        cerr << "´´¶¶´´´´´´´´´´´¶´´´¶¶¶¶´´´´´´´´´¶¶¶¶" << endl;
        cerr << "´´´¶¶¶¶¶¶¶¶¶¶¶¶´´´´´´´´¶¶¶¶¶¶¶¶¶" << endl;
    }
    else
    {
        cerr << "´´´´´´´´´´´´´´´´´´´´´´´¶¶¶¶¶¶¶¶" << endl;
        cerr << "´´´´´´´´´´´´´´´´´´´¶¶¶¶´´´´´´´´¶¶¶¶" << endl;
        cerr << "´´´´´´´´´´´´´´´´´¶¶´´´´´´´´´´´´´´´´¶¶" << endl;
        cerr << "´´´´´´´´´´´´´´´¶¶´´´´´´¶¶´´´´¶¶´´´´´´¶¶" << endl;
        cerr << "´´´´´´´´´´´´´¶¶´´´´´´´´¶¶´´´´¶¶´´´´´´´´¶¶" << endl;
        cerr << "´´´´´´´´´´´´´¶¶´´´´´´´´´´´´´´´´´´´´´´´´¶¶" << endl;
        cerr << "´´´´´´´´´´´´¶¶´´´´´´´´´´´´´´´´´´´´´´´´´´¶¶" << endl;
        cerr << "´´´´´´´´´´´´¶¶´´´´´´´´´´¶¶¶¶¶¶¶´´´´´´´´´´¶¶" << endl;
        cerr << "´´´´´´´´´´´´¶¶´´´´´´´´¶¶´´´´´´´¶¶´´´´´´´´¶¶" << endl;
        cerr << "´´´´´´´´´´´´´¶¶´´´´´¶¶´´´´´´´´´´´¶¶´´´´´´¶¶" << endl;
        cerr << "´´´´´´´´´´´´´´¶¶´´´´¶¶´´´´´´´´´´´¶¶´´´´¶¶" << endl;
        cerr << "´´´´´´´´´´´´´´´¶¶´´´´´´´´´´´´´´´´´´´´´¶¶" << endl;
        cerr << "´´´´´´´´´´´´´´´´¶¶´´´´´´´´´´´´´´´´´´´¶¶" << endl;
        cerr << "´´´´´´´´´´´´´´´´´´´¶¶¶¶´´´´´´´´´¶¶¶¶" << endl;
        cerr << "´´´´´´´´´´´´´´´´´´´´´´´¶¶¶¶¶¶¶¶¶" << endl;
    }

    /*-----------------------*/
    /* we are done sorting!! */
    /* save all ROOT spectra */
    /*-----------------------*/

    printf ( "\n" );
    printf ( "Sorting is done\n" );
    printf ( "attempting to save root or map file\n" );
    printf ( "\n" );
    fflush ( stdout );

    if ( execParams->InputSrc == DISK )
    {
        inData.close();
    }
    else if ( execParams->InputSrc == GEB )
    {

    }

    printf ( "\n" );
    fflush ( stdout );

    sprintf ( str, "sumTrackE.spe" );

    /* if we were using shared memory */

    if ( execParams->UseShareMemFile )
    {
        //UPDSSHMEM mfile->Print ();
        //printf ("\n");
        //mfile->ls ();
        //printf ("\n");
    };

    /* if we were using rootfile */

    if ( !execParams->UseShareMemFile )
    {
        printf ( "attempting to close root file...\n" );
        fflush ( stdout );
        execParams->f1->SetCompressionLevel ( 1 );

        if ( !execParams->noHists )
        {
            execParams->histDir->Write ( 0, TObject::kOverwrite );
            execParams->treeDir->Write ( 0, TObject::kWriteDelete );
        }

        execParams->f1->Write ( 0, TObject::kWriteDelete );

        execParams->f1->Flush();
        execParams->f1->Close();
        execParams->histDir = 0;
        execParams->treeDir = 0;
        printf ( "done saving rootfile \"%s\n\n", execParams->ROOTFile );
        fflush ( stdout );

        if ( execParams->cleanedMerged.is_open() ) execParams->cleanedMerged.close();
    }
    printf ( "\n" );

    /*-------------------------*/
    /* print simple statistics */
    /*-------------------------*/

    ShowStatus();

    //ExitDFMA();

    /* done */

    printf ( "\n" );
    printf ( "sorted timestamp range %lli-->%lli: %lli\n", t0, tcur, tcur - t0 );
    d1 = ( double ) ( tcur - t0 );
    d1 /= 100000000;
    printf ( "that is %.1f seconds or ", ( float ) d1 );
    nsec = d1;
    i1 = ( unsigned int ) d1 / 3600;
    d1 -= i1 * 3600;
    i2 = ( unsigned int ) d1 / 60;
    d1 -= i2 * 60;
    printf ( "%ih%im%is\n", i1, i2, ( int ) d1 );
    printf ( "^^^^^ any timestamp jumps will upset this accounting\n" );
    printf ( "\n" );

    printf ( "hit statistics per type\n" );
    i1 = 0;
    for ( int i = 1; i < MAX_GEB_TYPE; i++ )
        if ( typehit[i] > 0 )
        {
            GebTypeStr ( i, str );
            printf ( "%2i %s %10i ;", i, str, typehit[i] );
            i1 += typehit[i];
            d1 = ( double ) typehit[i] / nsec;
            printf ( " %9.2f Hz ", ( float ) d1 );
            printf ( "\n" );
        };
    printf ( "read a total of              %i ; header/payloads\n", i1 );
    printf ( "\n" );

    printf ( "\n" );
    printf ( "nn1=%i\n", nn1 );
    printf ( "nn2=%i\n", nn2 );
    printf ( "nn3=%i\n", nn3 );
    printf ( "\n" );

//     exit_mode2();

    printf ( "\n ** GEBSort is done at " );
    GetLocalTimeAndDate ( true );
    printf ( "\n" );
    return ( 0 );

}
