#include <iostream>

#include "gdecomp.h"
#include "GEBSort.h"
#include "GTMerge.h"

#include "TH1D.h"
#include "TH1F.h"
#include "TH2F.h"

#include "ProcessManagers.h"

// typedef struct PAYLOAD
// {
//     char p[MAXDATASIZE];
// } PAYLOAD;
//
// typedef struct TRACK_STRUCT
// {
//     int n;
//     GEBDATA* gd;
//     PAYLOAD* payload;
// } TRACK_STRUCT;


void AGODEvDecompose ( unsigned int* ev, int len, AGODEVENT* theAGODEvent )
{
    theAGODEvent->channels.clear();
    theAGODEvent->values.clear();

    unsigned int readWords = 0;
    unsigned long long timestamp = 0;
    for ( int i = 0; i < len; i++ )
    {
        unsigned int datum = *ev++;
        unsigned short channel = datum & 0xFFFF;
        unsigned short value = ( datum >> 16 ) & 0xFFFF;

        if ( channel >= 1000 && channel <= 1003 )
        {
            timestamp |= ( unsigned long long ) value << ( 16 * ( 246 - channel ) );
        }
        else
        {
            theAGODEvent->channels.push_back ( channel );
            theAGODEvent->values.push_back ( value );
            readWords++;
        }
    }
    theAGODEvent->timestamp = timestamp;
}

TH1D* h1_agod_en;
TH2F* h2_agod_en;
TH2F* h2_dTg_agod;
TH2F* h2_g_agod;
TH1D* h1_agod_dTS;

/*-----------------------------------------------------*/

void sup_agod()
{
    PARS* Pars = SortManager::sinstance()->execParams;

    if ( !Pars->noHists )
    {
        /* declarations */

        gDirectory->mkdir ( "agod" )->cd();
        TH1D* mkTH1D ( char*, char*, int, double, double );
        TH2F* mkTH2F ( char*, char*, int, double, double, int, double, double );

        h1_agod_en = mkTH1D ( ( char* ) "agod_all_en", ( char* ) "agod_all_en", 4096, 0, 4096 );
        h2_agod_en = mkTH2F ( ( char* ) "agod_en", ( char* ) "agod_en", 4096, 0, 4096, 400, 0, 400 );
        h1_agod_dTS = mkTH1D ( ( char* ) "agod_dTS", ( char* ) "agod_dTS", 1000, -2E10, 2E10 );

        h2_dTg_agod = mkTH2F ( ( char* ) "dTg_agod", ( char* ) "dTg_agod", 4000, -2000, 2000, 400, 0, 400 );

        h2_g_agod  = mkTH2F ( ( char* ) "g_agod", ( char* ) "g_agod", 4000, 0, 4000, 4096, 0, 4096 );

        gDirectory->cd ( "/" );

        /* list what we have */

        printf ( " we have define the following spectra:\n" );

        Pars->wlist = gDirectory->GetList();
        Pars->wlist->Print();
    }
}

/* ----------------------------------------------------------------- */

int bin_agod ( GEB_EVENT* GEB_event )
{
    PARS* Pars = SortManager::sinstance()->execParams;

    /* declarations */

    char str[128];

    /* prototypes */

    int GebTypeStr ( int type, char str[] );

    if ( Pars->CurEvNo <= Pars->NumToPrint )
    {
        printf ( "entered bin_agod:\n" );
    }

    AGODEVENT* AGODEvent = SortManager::sinstance()->AGODEvent;
    DGSEVENT* DGSEvent = SortManager::sinstance()->DGSEvent;

    unsigned int* numAGOD = &SortManager::sinstance()->numAGOD;
    *numAGOD = 0;
    
    int* ng = &SortManager::sinstance()->ng;

    /* loop through the coincidence event and fish out GEB_TYPE_AGOD data */

    for ( int i = 0; i < GEB_event->ptgd.size(); i++ )
    {
        // look for analog marker 0x13 = 19
        if ( GEB_event->ptgd[i]->type == 19 )
        {
            if ( Pars->CurEvNo <= Pars->NumToPrint )
            {
                GebTypeStr ( GEB_event->ptgd[i]->type, str );
                //printf ("bin_template, %2i> %2i, %s, TS=%lli\n", i, GEB_event->ptgd[i]->type, str, GEB_event->ptgd[i]->timestamp);
            }
            AGODEvDecompose ( ( unsigned int* ) GEB_event->ptinp[i], GEB_event->ptgd[i]->length / sizeof ( unsigned int ), &AGODEvent[*numAGOD] );

            ( *numAGOD ) ++;
        }
    }

    // histogram incrementation
    for ( unsigned int i = 0; i < *numAGOD; i++ )
    {
        if ( !Pars->noHists ) h1_agod_dTS->Fill ( ( long long ) ( AGODEvent[i].timestamp - SortManager::sinstance()->lastTS ) );
        SortManager::sinstance()->lastTS = AGODEvent[i].timestamp;
        for ( size_t j = 0; j < AGODEvent[i].values.size(); j++ )
        {
            if ( !Pars->noHists ) h2_agod_en->Fill ( AGODEvent[i].values[j], AGODEvent[i].channels[j] );
            if ( !Pars->noHists ) h1_agod_en->Fill ( AGODEvent[i].values[j] );
        }
    }

    // time differences

    double dTg_agod;
    dTg_agod = 0.0;

    for ( unsigned int i = 0; i < *numAGOD; i++ )
    {
        //printf("AGOD: %d %d %d %d\n", i, numAGOD, *ng, theAGODEvent[i].values.size());
        if ( *ng > 0 )
        {
            for ( size_t j = 0; j < AGODEvent[i].values.size(); j++ )
            {
                dTg_agod = double ( DGSEvent[0].event_timestamp ) - double ( AGODEvent[i].timestamp );
                if ( !Pars->noHists ) h2_dTg_agod->Fill ( dTg_agod, AGODEvent[i].channels[j] );
            }
        }
    }

    for ( unsigned int i = 0; i < *numAGOD; i++ )
    {
        for ( int j = 0; j < *ng; j++ )
        {
            if ( DGSEvent[j].tpe == GE )
            {
                dTg_agod = double ( DGSEvent[j].event_timestamp ) - double ( AGODEvent[i].timestamp );
                for ( size_t k = 0; k < AGODEvent[i].values.size(); k++ )
                {
                    if ( ( AGODEvent[i].channels[k] == 10 ) && ( dTg_agod > 407 ) & ( dTg_agod < 420 ) )
                    {
                        if ( !Pars->noHists ) h2_g_agod->Fill ( DGSEvent[j].ehi, AGODEvent[i].values[k] );
                    }
                }
            }
        }
    }

    /* done */

    if ( Pars->CurEvNo <= Pars->NumToPrint )
    {
        printf ( "exit bin_agod\n" );
    }

    return 0;

}
