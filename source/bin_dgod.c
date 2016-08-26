#include "gdecomp.h"
#include "GEBSort.h"
#include "GTMerge.h"

#include "TH1D.h"
#include "TH1F.h"
#include "TH2F.h"

#include "SortManager.h"

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


/* pointers to ROOT spectra */


/* parameters */

TH1D* h1_god_en;
TH2F* h2_god_en;
TH2F* h2_dTg_god;
TH2F* h2_g_god;

TH1D* h1_x, *h1_x_g;

/*-----------------------------------------------------*/

int sup_dgod()
{
    /* declarations */
    PARS* Pars = SortManager::sinstance()->execParams;

    if ( !Pars->noHists )
    {
        gDirectory->mkdir ( "dgod" )->cd();
        TH1D* mkTH1D ( char*, char*, int, double, double );
        TH2F* mkTH2F ( char*, char*, int, double, double, int, double, double );

        h1_god_en = mkTH1D ( ( char* ) "god_all_en", ( char* ) "god_all_en", 16000, 0, 1600000 );
        h2_god_en = mkTH2F ( ( char* ) "god_en", ( char* ) "god_en", 16000, 0, 1600000, 400, 0, 400 );

        h2_dTg_god = mkTH2F ( ( char* ) "dTg_god", ( char* ) "dTg_god", 4000, -2000, 2000, 400, 0, 400 );

        h2_g_god  = mkTH2F ( ( char* ) "g_god", ( char* ) "g_god", 4000, 0, 4000, 4000, 0, 400000 );

        h1_x = mkTH1D ( ( char* ) "x", ( char* ) "x", 16000, 0, 16000 );
        h1_x_g = mkTH1D ( ( char* ) "xg", ( char* ) "x", 16000, 0, 16000 );

        gDirectory->cd ( "/" );

        /* list what we have */

        printf ( " we have define the following spectra:\n" );

        Pars->wlist = gDirectory->GetList();
        Pars->wlist->Print();
    }

    return 0;

}

int DFMAEvDecompose_v3 ( unsigned int* ev, int len, DFMAEVENT* theDFMAEvent )
{
    /* firmware circa Sept 2014 */

    /* declarations */
    PARS* Pars = SortManager::sinstance()->execParams;
    int i, k;
    unsigned int ui0 = 0, ui1 = 0, ui2 = 0;
    unsigned int PRE_RISE_SUM = 0, POST_RISE_SUM = 0;
    int rawE;
    unsigned int t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    unsigned long long int ulli1;


    if ( Pars->CurEvNo <= Pars->NumToPrint )
    {
        printf ( "entered DGSEvDecompose_v3:\n" );
        printf ( "\nmapping\n" );


        for ( i = 2010; i < 2020; i++ )
        {
            printf ( "lkup %d tid %d\n", SortManager::sinstance()->tlkup[i], SortManager::sinstance()->tid[i] );

        };
    }

    /* swap the bytes */

    i = 0;
    while ( i < len )
    {

        /* before 4 3 2 1 */
        /*        | | | | */
        /* after  1 2 3 4 */

        t1 = ( * ( ev + i ) & 0x000000ff ) << 24;
        t2 = ( * ( ev + i ) & 0x0000ff00 ) << 8;
        t3 = ( * ( ev + i ) & 0x00ff0000 ) >> 8;
        t4 = ( * ( ev + i ) & 0xff000000 ) >> 24;
        * ( ev + i ) = t1 + t2 + t3 + t4;

        i++;
    }


    /* debug print */

    if ( Pars->CurEvNo <= Pars->NumToPrint )
    {
        printf ( "event len=%i (%lu bytes) >\n", len, len * sizeof ( unsigned int ) );
        for ( i = 0; i < len; i++ )
        {
            printf ( "%3i[doc: %3i]: %12u, 0x%8.8x; ", i, i + 1, * ( ev + i ), * ( ev + i ) );
            ui0 = 0x80000000;
            printf ( "|" );
            for ( k = 0; k <= 31; k++ )
            {
                if ( ( * ( ev + i ) & ui0 ) == ui0 )
                {
                    printf ( "1" );
                }
                else
                {
                    printf ( "0" );
                }
                ui0 = ui0 / 2;
                if ( ( k + 1 ) % 4 == 0 )
                {
                    printf ( "|" );
                }
            };
            printf ( "\n" );
        };
    };

    /* extract IDs */

    theDFMAEvent->chan_id = ( * ( ev + 0 ) & 0x0000000f );
    theDFMAEvent->board_id = ( ( * ( ev + 0 ) >> 4 ) & 0xfff );
    theDFMAEvent->id = theDFMAEvent->board_id * 10 + theDFMAEvent->chan_id;

    /* store the type and type ID */

    theDFMAEvent->tpe = SortManager::sinstance()->tlkup[theDFMAEvent->id];
    theDFMAEvent->tid = SortManager::sinstance()->tid[theDFMAEvent->id];

    if ( Pars->CurEvNo <= Pars->NumToPrint )
    {
        printf ( "chan_id = %i, board_id=%i, id=%i\n", theDFMAEvent->chan_id, theDFMAEvent->board_id, theDFMAEvent->id );
    }

    /* extract the energy */

    PRE_RISE_SUM = * ( ev + 7 ) & 0x00ffffff;

    if ( Pars->CurEvNo <= Pars->NumToPrint )
    {
        printf ( "PRE_RISE_SUM =  0x%8.8x %12i  ", PRE_RISE_SUM, PRE_RISE_SUM );
        ui0 = 0x80000000;
        printf ( "|" );
        for ( k = 0; k <= 31; k++ )
        {
            if ( ( PRE_RISE_SUM & ui0 ) == ui0 )
            {
                printf ( "1" );
            }
            else
            {
                printf ( "0" );
            }
            ui0 = ui0 / 2;
            if ( ( k + 1 ) % 4 == 0 )
            {
                printf ( "|" );
            }
        };
        printf ( "\n" );
    };

    ui1 = * ( ev + 7 ) & 0xff000000;
    ui1 >>= 24;
    ui2 = * ( ev + 8 ) & 0x0000ffff;
    ui2 <<= 8;
    POST_RISE_SUM = ui1 + ui2;

    if ( Pars->CurEvNo <= Pars->NumToPrint )
    {
        printf ( "POST_RISE_SUM = 0x%8.8x %12i  ", POST_RISE_SUM, POST_RISE_SUM );
        ui0 = 0x80000000;
        printf ( "|" );
        for ( k = 0; k <= 31; k++ )
        {
            if ( ( POST_RISE_SUM & ui0 ) == ui0 )
            {
                printf ( "1" );
            }
            else
            {
                printf ( "0" );
            }
            ui0 = ui0 / 2;
            if ( ( k + 1 ) % 4 == 0 )
            {
                printf ( "|" );
            }
        };
        printf ( "\n" );
    };

    /* note: POS/PRE_RISE_SUM only have 24 bits */
    /* so making ints of them is not a problem */

    rawE = ( int ) POST_RISE_SUM - ( int ) PRE_RISE_SUM;
    theDFMAEvent->ehi = rawE / 10;
    if ( theDFMAEvent->ehi < 0 )
    {
        theDFMAEvent->ehi = -theDFMAEvent->ehi;
    }

    if ( Pars->CurEvNo <= Pars->NumToPrint )
    {
        printf ( "rawE = 0x%8.8x %i, theDFMAEvent->ehi= %i\n", rawE, rawE, theDFMAEvent->ehi );
    }

    /* extract the LED time stamp */

    theDFMAEvent->LEDts = 0;
    theDFMAEvent->LEDts = ( unsigned long long int ) * ( ev + 1 );
    ulli1 = ( unsigned long long int ) ( * ( ev + 2 ) & 0x0000ffff );
    ulli1 = ( ulli1 << 32 );
    theDFMAEvent->LEDts += ulli1;

    /* extract trace */


    theDFMAEvent->traceLen = 0;
    for ( i = 0; i < len - HDRLENINTS; i++ )
    {

        theDFMAEvent->trace[theDFMAEvent->traceLen] = ( unsigned long long int ) ( * ( ev + 13 + i ) & 0x00003fff );
        theDFMAEvent->traceLen++;
        theDFMAEvent->trace[theDFMAEvent->traceLen] = ( unsigned long long int ) ( * ( ev + 13 + i ) >> 16 & 0x00003fff );
        theDFMAEvent->traceLen++;

    }

    /* DS changes 3/1/2014 */

    unsigned long long int prevTS;
    int baseline, basesample;
    int postrisebeg, postriseend, prerisebeg, preriseend;
    int peaksample;
    int prerisesum;

// previous TS

//prevTS = 0;
    prevTS = ( unsigned long long int ) ( * ( ev + 3 ) & 0xffff );
    ulli1 = ( unsigned long long int ) ( * ( ev + 4 ) );
    ulli1 = ( ulli1 << 16 );
    prevTS += ulli1;

// baseline

    baseline = ( int ) ( * ( ev + 5 ) & 0xffffff );

// post rise begin sample

    postrisebeg = ( int ) ( * ( ev + 10 ) & 0x3fff );

// pre rise begin sample

    prerisebeg = ( int ) ( * ( ev + 11 ) & 0x3fff );

// post rise end sample

    postriseend = ( int ) ( * ( ev + 10 ) & 0x3fff0000 );

    postriseend = postriseend >> 16;

// pre rise end sample

    preriseend = ( int ) ( * ( ev + 11 ) & 0x3fff0000 );

    preriseend = preriseend >> 16;

// peak sample

    peaksample = ( int ) ( * ( ev + 12 ) & 0x3fff );

// base sample

    basesample = ( int ) ( * ( ev + 12 ) & 0x3fff0000 );

    basesample = basesample >> 16;

    prerisesum = PRE_RISE_SUM / 400;

    theDFMAEvent->baseline = baseline;
    theDFMAEvent->postrisebeg = postrisebeg;
    theDFMAEvent->prerisebeg = prerisebeg;
    theDFMAEvent->postriseend = postriseend;
    theDFMAEvent->preriseend = preriseend;
    theDFMAEvent->peaksample = peaksample;
    theDFMAEvent->basesample = basesample;
    theDFMAEvent->prevTS = prevTS;
    theDFMAEvent->prerisesum = prerisesum;


    /* done */

    if ( Pars->CurEvNo <= Pars->NumToPrint )
    {
        printf ( "exit DGSEvDecompose_v3:\n" );
    }

    return 0;
}

/* ----------------------------------------------------------------- */

int bin_dgod ( GEB_EVENT* GEB_event )
{

    /* declarations */
    PARS* Pars = SortManager::sinstance()->execParams;

    char str[128];
    int j;
    int ndssd;
    int ndfma;
    int nfp;
    int nsubev;

    /* prototypes */

    int GebTypeStr ( int type, char str[] );

//if(1)return(0);

    if ( Pars->CurEvNo <= Pars->NumToPrint )
    {
        printf ( "entered bin_god:\n" );
    }

    ndfma = 0;
    ndssd = 0;
    nsubev = 0;
    nfp = 0;

    DFMAEVENT* DFMAEvent = SortManager::sinstance()->DFMAEvent;
    DGSEVENT* DGSEvent = SortManager::sinstance()->DGSEvent;

    unsigned int* numDGOD = &SortManager::sinstance()->numDGOD;
    *numDGOD = 0;

    int* ng = &SortManager::sinstance()->ng;

    /* loop through the coincidence event and fish out GEB_TYPE_DFMA data */

    for ( unsigned int i = 0; i < GEB_event->ptgd.size(); i++ )
    {

        if ( GEB_event->ptgd[i]->type == 16 )
        {

            if ( Pars->CurEvNo <= Pars->NumToPrint )
            {
                GebTypeStr ( GEB_event->ptgd[i]->type, str );
                printf ( "bin_template, %2i> %2i, %s, TS=%lli\n", i, GEB_event->ptgd[i]->type, str,
                         GEB_event->ptgd[i]->timestamp );
            }

            DFMAEvDecompose_v3 ( ( unsigned int* ) GEB_event->ptinp[i], GEB_event->ptgd[i]->length / sizeof ( unsigned int ),
                                 &DFMAEvent[nsubev] );

            if ( DFMAEvent[nsubev].tpe == DSSD )
            {
                ndssd++;
                ndfma++;
            }
            if ( DFMAEvent[nsubev].tpe == FP )
            {
                nfp++;
                ndfma++;
            }
            nsubev++;
            ( *numDGOD ) ++;

        };

    };

    // histogram incremantation

    for ( int i = 0; i < nsubev; i++ )
    {

        if ( !Pars->noHists && DFMAEvent[i].tpe == DSSD )
        {
            h2_god_en->Fill ( DFMAEvent[i].ehi, DFMAEvent[i].tid );
            h1_god_en->Fill ( DFMAEvent[i].ehi );
        }
    }

    // time differences

    double dTg_god;
    dTg_god = 0.0;

    for ( int i = 0; i < nsubev; i++ )
    {
        if ( ( DFMAEvent[i].LEDts > 0 ) && ( DFMAEvent[i].tpe == DSSD ) && ( *ng > 0 ) )
        {

            dTg_god = double ( DGSEvent[0].event_timestamp ) - double ( DFMAEvent[i].LEDts );
            if ( !Pars->noHists ) h2_dTg_god->Fill ( dTg_god, DFMAEvent[i].tid );

        }
    }

    for ( int i = 0; i < nsubev; i++ )
    {
        for ( j = 0; j < *ng; j++ )
        {

            dTg_god = double ( DGSEvent[j].event_timestamp ) - double ( DFMAEvent[i].LEDts );
            if ( !Pars->noHists && ( DFMAEvent[i].tid == 10 ) && ( dTg_god > 260 ) & ( dTg_god < 290 ) )
            {
                h2_g_god->Fill ( DGSEvent[j].ehi, DFMAEvent[i].ehi );
            }

        }
    }

// start fma stuffs

    unsigned int left = 0;
    unsigned int right = 0;
    unsigned int lr = 0;
    unsigned int x = 0;

    for ( int i = 0; i < nsubev; i++ )
    {
        if ( DFMAEvent[i].tpe == FP )
        {
            DFMAEvent[i].ehi = DFMAEvent[i].ehi / 30;
            if ( DFMAEvent[i].tid == 1 )
            {
                left = DFMAEvent[i].ehi;
            }
            if ( DFMAEvent[i].tid == 2 )
            {
                right = DFMAEvent[i].ehi;
            }
        }

    }

    if ( ( left > 0 ) && ( right > 0 ) )
    {
        x = left - right + 8000;
        lr = left + right;
    }

    if ( !Pars->noHists ) h1_x->Fill ( x );

// Si-FMA

    unsigned int left_g = 0;
    unsigned int right_g = 0;

    for ( int i = 0; i < nsubev; i++ )
    {
        if ( ( DFMAEvent[i].LEDts > 0 ) && ( DFMAEvent[i].tpe == FP ) )
        {

            for ( j = 0; j < nsubev; j++ )
            {
                if ( ( DFMAEvent[j].tpe == DSSD ) && ( DFMAEvent[j].tid > 0 ) && ( DFMAEvent[j].tid < 107 ) )
                {
                    dTg_god = double ( DFMAEvent[j].LEDts ) - double ( DFMAEvent[i].LEDts );

                    if ( ( dTg_god > 130 ) && ( dTg_god < 170 ) )
                    {
                        if ( DFMAEvent[i].tid == 1 )
                        {
                            left_g = DFMAEvent[i].ehi;
                        }
                        if ( DFMAEvent[i].tid == 2 )
                        {
                            right_g = DFMAEvent[i].ehi;
                        }
                        // Fill 1d gated x spectrum
                        if ( !Pars->noHists && ( left_g > 0 ) && ( right_g > 0 ) )
                        {
                            x = ( left_g - right_g + 8000 );
                            h1_x_g->Fill ( x );
                        }
                    }
                }
            }
        }
    }

    /* done */

    if ( Pars->CurEvNo <= Pars->NumToPrint )
    {
        printf ( "exit bin_dgod\n" );
    }

    return 0;

}
