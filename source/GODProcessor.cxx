#include "GODProcessor.h"

TH1D* h1_god_en;
TH2F* h2_god_en;
TH2F* h2_dTg_god;
TH2F* h2_g_god;

TH1D* h1_x, *h1_x_g;

TH1D* h1_agod_en;
TH2F* h2_agod_en;
TH2F* h2_dTg_agod;
TH2F* h2_g_agod;
TH1D* h1_agod_dTS;

GODProcessor::GODProcessor ( int* tlkup_, int* tid_, int* ng_, PARS* pars_, GoddessConfig* gConf_, unsigned int* numDGOD_, unsigned int* numAGOD_, unsigned long long* lastTS_ )
{
    tlkup = tlkup_;
    tid = tid_;
    ng = ng_;
    pars= pars_;
    gConf = gConf_;
    numDGOD = numDGOD_;
    numAGOD = numAGOD_;
    lastTS = lastTS_;
}

int GODProcessor::SupDGOD()
{
    if ( !pars->noHists )
    {
        gDirectory->mkdir ( "dgod" )->cd();

        h1_god_en = mkTH1D ( ( char* ) "god_all_en", ( char* ) "god_all_en", 16000, 0, 1600000 );
        h2_god_en = mkTH2F ( ( char* ) "god_en", ( char* ) "god_en", 16000, 0, 1600000, 400, 0, 400 );

        h2_dTg_god = mkTH2F ( ( char* ) "dTg_god", ( char* ) "dTg_god", 4000, -2000, 2000, 400, 0, 400 );

        h2_g_god  = mkTH2F ( ( char* ) "g_god", ( char* ) "g_god", 4000, 0, 4000, 4000, 0, 400000 );

        h1_x = mkTH1D ( ( char* ) "x", ( char* ) "x", 16000, 0, 16000 );
        h1_x_g = mkTH1D ( ( char* ) "xg", ( char* ) "x", 16000, 0, 16000 );

        gDirectory->cd ( "/" );

        /* list what we have */

        printf ( " we have define the following spectra:\n" );

        pars->wlist = gDirectory->GetList();
        pars->wlist->Print();
    }

    return 0;

}

int GODProcessor::DGODEvDecompose ( unsigned int* ev, int len, DFMAEVENT* thedfmaEvt )
{
    /* firmware circa Sept 2014 */

    int i, k;
    unsigned int ui0 = 0, ui1 = 0, ui2 = 0;
    unsigned int PRE_RISE_SUM = 0, POST_RISE_SUM = 0;
    int rawE;
    unsigned int t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    unsigned long long int ulli1;


    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "entered DGSEvDecompose_v3:\n" );
        printf ( "\nmapping\n" );


        for ( i = 2010; i < 2020; i++ )
        {
            printf ( "lkup %d tid %d\n", tlkup[i], tid[i] );

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

    if ( pars->CurEvNo <= pars->NumToPrint )
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

    thedfmaEvt->chan_id = ( * ( ev + 0 ) & 0x0000000f );
    thedfmaEvt->board_id = ( ( * ( ev + 0 ) >> 4 ) & 0xfff );
    thedfmaEvt->id = thedfmaEvt->board_id * 10 + thedfmaEvt->chan_id;

    /* store the type and type ID */

    thedfmaEvt->tpe = tlkup[thedfmaEvt->id];
    thedfmaEvt->tid = tid[thedfmaEvt->id];

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "chan_id = %i, board_id=%i, id=%i\n", thedfmaEvt->chan_id, thedfmaEvt->board_id, thedfmaEvt->id );
    }

    /* extract the energy */

    PRE_RISE_SUM = * ( ev + 7 ) & 0x00ffffff;

    if ( pars->CurEvNo <= pars->NumToPrint )
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

    if ( pars->CurEvNo <= pars->NumToPrint )
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
    thedfmaEvt->ehi = rawE / 10;
    if ( thedfmaEvt->ehi < 0 )
    {
        thedfmaEvt->ehi = -thedfmaEvt->ehi;
    }

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "rawE = 0x%8.8x %i, thedfmaEvt->ehi= %i\n", rawE, rawE, thedfmaEvt->ehi );
    }

    /* extract the LED time stamp */

    thedfmaEvt->LEDts = 0;
    thedfmaEvt->LEDts = ( unsigned long long int ) * ( ev + 1 );
    ulli1 = ( unsigned long long int ) ( * ( ev + 2 ) & 0x0000ffff );
    ulli1 = ( ulli1 << 32 );
    thedfmaEvt->LEDts += ulli1;

    /* extract trace */


    thedfmaEvt->traceLen = 0;
    for ( i = 0; i < len - HDRLENINTS; i++ )
    {

        thedfmaEvt->trace[thedfmaEvt->traceLen] = ( unsigned long long int ) ( * ( ev + 13 + i ) & 0x00003fff );
        thedfmaEvt->traceLen++;
        thedfmaEvt->trace[thedfmaEvt->traceLen] = ( unsigned long long int ) ( * ( ev + 13 + i ) >> 16 & 0x00003fff );
        thedfmaEvt->traceLen++;

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

    thedfmaEvt->baseline = baseline;
    thedfmaEvt->postrisebeg = postrisebeg;
    thedfmaEvt->prerisebeg = prerisebeg;
    thedfmaEvt->postriseend = postriseend;
    thedfmaEvt->preriseend = preriseend;
    thedfmaEvt->peaksample = peaksample;
    thedfmaEvt->basesample = basesample;
    thedfmaEvt->prevTS = prevTS;
    thedfmaEvt->prerisesum = prerisesum;


    /* done */

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "exit DGSEvDecompose_v3:\n" );
    }

    return 0;
}

/* ----------------------------------------------------------------- */

int GODProcessor::BinDGOD ( GEB_EVENT* gebEvt, DFMAEVENT* dfmaEvt, DGSEVENT* dgsEvt, GRETEVENT* gretEvt )
{
    char str[128];
    int j;
    int ndssd;
    int ndfma;
    int nfp;
    int nsubev;

    /* prototypes */

    int GebTypeStr ( int type, char str[] );

//if(1)return(0);

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "entered BinGOD:\n" );
    }

    ndfma = 0;
    ndssd = 0;
    nsubev = 0;
    nfp = 0;

    *numDGOD = 0;

    /* loop through the coincidence event and fish out GEB_TYPE_DFMA data */

    for ( unsigned int i = 0; i < gebEvt->ptgd.size(); i++ )
    {

        if ( gebEvt->ptgd[i]->type == 16 )
        {

            if ( pars->CurEvNo <= pars->NumToPrint )
            {
                GebTypeStr ( gebEvt->ptgd[i]->type, str );
                printf ( "bin_template, %2i> %2i, %s, TS=%lli\n", i, gebEvt->ptgd[i]->type, str,
                         gebEvt->ptgd[i]->timestamp );
            }

            DGODEvDecompose ( ( unsigned int* ) gebEvt->ptinp[i], gebEvt->ptgd[i]->length / sizeof ( unsigned int ),
                              &dfmaEvt[nsubev] );

            if ( dfmaEvt[nsubev].tpe == DSSD )
            {
                ndssd++;
                ndfma++;
            }
            if ( dfmaEvt[nsubev].tpe == FP )
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

        if ( !pars->noHists && dfmaEvt[i].tpe == DSSD )
        {
            h2_god_en->Fill ( dfmaEvt[i].ehi, dfmaEvt[i].tid );
            h1_god_en->Fill ( dfmaEvt[i].ehi );
        }
    }

    // time differences

    double dTg_god;
    dTg_god = 0.0;

    for ( int i = 0; i < nsubev; i++ )
    {
        if ( ( dfmaEvt[i].LEDts > 0 ) && ( dfmaEvt[i].tpe == DSSD ) && ( *ng > 0 ) )
        {
            if ( pars->GammaProcessor == 0 )
            {
                dTg_god = double ( dgsEvt[0].event_timestamp ) - double ( dfmaEvt[i].LEDts );
            }
            else if ( pars->GammaProcessor == 1 )
            {
                dTg_god = double ( gretEvt[0].timestamp ) - double ( dfmaEvt[i].LEDts );
            }
            else
            {
                cout<<"Not a viable gamma ray processor."<<endl;
                return 1;
            }
            if ( !pars->noHists ) h2_dTg_god->Fill ( dTg_god, dfmaEvt[i].tid );

        }
    }

    for ( int i = 0; i < nsubev; i++ )
    {
        for ( j = 0; j < *ng; j++ )
        {
            if ( pars->GammaProcessor == 0 )
            {
                dTg_god = double ( dgsEvt[j].event_timestamp ) - double ( dfmaEvt[i].LEDts );
            }
            else if ( pars->GammaProcessor == 1 )
            {
                dTg_god = double ( gretEvt[j].timestamp ) - double ( dfmaEvt[i].LEDts );
            }
            else
            {
                cout<<"Not a viable gamma ray processor."<<endl;
                return 1;
            }

            if ( !pars->noHists && ( dfmaEvt[i].tid == 10 ) && ( dTg_god > 260 ) & ( dTg_god < 290 ) )

            {
                if ( pars->GammaProcessor==0 )
                {
                    h2_g_god->Fill ( dgsEvt[j].ehi, dfmaEvt[i].ehi );
                }
                else if ( pars->GammaProcessor ==1 )
                {
                    h2_g_god->Fill ( gretEvt[j].e, dfmaEvt[i].ehi );
                }
                else
                {
                    cout<<"Not a viable gamma ray processor."<<endl;
                    return 1;
                }
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
        if ( dfmaEvt[i].tpe == FP )
        {
            dfmaEvt[i].ehi = dfmaEvt[i].ehi / 30;
            if ( dfmaEvt[i].tid == 1 )
            {
                left = dfmaEvt[i].ehi;
            }
            if ( dfmaEvt[i].tid == 2 )
            {
                right = dfmaEvt[i].ehi;
            }
        }

    }

    if ( ( left > 0 ) && ( right > 0 ) )
    {
        x = left - right + 8000;
        lr = left + right;
    }

    if ( !pars->noHists ) h1_x->Fill ( x );

// Si-FMA

    unsigned int left_g = 0;
    unsigned int right_g = 0;

    for ( int i = 0; i < nsubev; i++ )
    {
        if ( ( dfmaEvt[i].LEDts > 0 ) && ( dfmaEvt[i].tpe == FP ) )
        {

            for ( j = 0; j < nsubev; j++ )
            {
                if ( ( dfmaEvt[j].tpe == DSSD ) && ( dfmaEvt[j].tid > 0 ) && ( dfmaEvt[j].tid < 107 ) )
                {
                    dTg_god = double ( dfmaEvt[j].LEDts ) - double ( dfmaEvt[i].LEDts );

                    if ( ( dTg_god > 130 ) && ( dTg_god < 170 ) )
                    {
                        if ( dfmaEvt[i].tid == 1 )
                        {
                            left_g = dfmaEvt[i].ehi;
                        }
                        if ( dfmaEvt[i].tid == 2 )
                        {
                            right_g = dfmaEvt[i].ehi;
                        }
                        // Fill 1d gated x spectrum
                        if ( !pars->noHists && ( left_g > 0 ) && ( right_g > 0 ) )
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

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "exit BinDGOD\n" );
    }

    return 0;

}

void GODProcessor::AGODEvDecompose ( unsigned int* ev, int len, AGODEVENT* theagodEvt )
{
    theagodEvt->channels.clear();
    theagodEvt->values.clear();

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
            theagodEvt->channels.push_back ( channel );
            theagodEvt->values.push_back ( value );
            readWords++;
        }
    }

    theagodEvt->timestamp = timestamp;
}

void GODProcessor::SupAGOD()
{
    if ( !pars->noHists )
    {
        /* declarations */

        gDirectory->mkdir ( "agod" )->cd();

        h1_agod_en = mkTH1D ( ( char* ) "agod_all_en", ( char* ) "agod_all_en", 4096, 0, 4096 );
        h2_agod_en = mkTH2F ( ( char* ) "agod_en", ( char* ) "agod_en", 4096, 0, 4096, 400, 0, 400 );
        h1_agod_dTS = mkTH1D ( ( char* ) "agod_dTS", ( char* ) "agod_dTS", 1000, -2E10, 2E10 );

        h2_dTg_agod = mkTH2F ( ( char* ) "dTg_agod", ( char* ) "dTg_agod", 4000, -2000, 2000, 400, 0, 400 );

        h2_g_agod  = mkTH2F ( ( char* ) "g_agod", ( char* ) "g_agod", 4000, 0, 4000, 4096, 0, 4096 );

        gDirectory->cd ( "/" );

        /* list what we have */

        printf ( " we have define the following spectra:\n" );

        pars->wlist = gDirectory->GetList();
        pars->wlist->Print();
    }
}

int GODProcessor::BinAGOD ( GEB_EVENT* gebEvt, AGODEVENT* agodEvt, DGSEVENT* dgsEvt, GRETEVENT* gretEvt )
{
    char str[128];

    /* prototypes */

    int GebTypeStr ( int type, char str[] );

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        *numAGOD = 0;

        /* loop through the coincidence event and fish out GEB_TYPE_AGOD data */

        for ( unsigned int i = 0; i < gebEvt->ptgd.size(); i++ )
        {
            // look for analog marker 0x13 = 19
            if ( gebEvt->ptgd[i]->type == 19 )
            {
                if ( pars->CurEvNo <= pars->NumToPrint )
                {
                    GebTypeStr ( gebEvt->ptgd[i]->type, str );
                    //printf ("bin_template, %2i> %2i, %s, TS=%lli\n", i, gebEvt->ptgd[i]->type, str, gebEvt->ptgd[i]->timestamp);
                }

                AGODEvDecompose ( ( unsigned int* ) gebEvt->ptinp[i], gebEvt->ptgd[i]->length / sizeof ( unsigned int ), &agodEvt[*numAGOD] );

//             std::cerr << "Analog ORRUBA event: gebEvt TS = " << gebEvt->ptgd[i]->timestamp << " / AGODEVENT TS = " << agodEvt[*numAGOD].timestamp << "\n";

                ( *numAGOD ) ++;
            }
        }
    }

    // histogram incrementation
    for ( unsigned int i = 0; i < *numAGOD; i++ )
    {
        if ( !pars->noHists ) h1_agod_dTS->Fill ( ( long long ) ( agodEvt[i].timestamp - *lastTS ) );
        *lastTS = agodEvt[i].timestamp;
        for ( size_t j = 0; j < agodEvt[i].values.size(); j++ )
        {
            if ( !pars->noHists ) h2_agod_en->Fill ( agodEvt[i].values[j], agodEvt[i].channels[j] );
            if ( !pars->noHists ) h1_agod_en->Fill ( agodEvt[i].values[j] );
        }
    }

    // time differences

    double dTg_agod;
    dTg_agod = 0.0;

    for ( unsigned int i = 0; i < *numAGOD; i++ )
    {
        //printf("AGOD: %d %d %d %d\n", i, numAGOD, *ng, theagodEvt[i].values.size());
        if ( *ng > 0 )
        {
            for ( size_t j = 0; j < agodEvt[i].values.size(); j++ )
            {
                if ( pars->GammaProcessor == 0 )
                {
                    dTg_agod = double ( dgsEvt[0].event_timestamp ) - double ( agodEvt[i].timestamp );
                }
                else if ( pars->GammaProcessor == 1 )
                {
                    dTg_agod = double ( gretEvt[0].timestamp ) - double ( agodEvt[i].timestamp );
                }
                else
                {
                    cout<<"Not a viable gamma ray processor."<<endl;
                    return 1;
                }

                if ( !pars->noHists ) h2_dTg_agod->Fill ( dTg_agod, agodEvt[i].channels[j] );
            }
        }
    }

    for ( unsigned int i = 0; i < *numAGOD; i++ )
    {
        for ( int j = 0; j < *ng; j++ )
        {
            if ( pars->GammaProcessor == 1 ||  dgsEvt[j].tpe == GE )
            {
                if ( pars->GammaProcessor == 0 )
                {
                    dTg_agod = double ( dgsEvt[j].event_timestamp ) - double ( agodEvt[i].timestamp );
                }

                else if ( pars->GammaProcessor == 1 )
                {
                    dTg_agod = double ( gretEvt[j].timestamp ) - double ( agodEvt[i].timestamp );
                }
                else
                {
                    cout<<"Not a viable gamma ray processor."<<endl;
                    return 1;
                }

                for ( size_t k = 0; k < agodEvt[i].values.size(); k++ )
                {
                    if ( ( agodEvt[i].channels[k] == 10 ) && ( dTg_agod > 407 ) & ( dTg_agod < 420 ) )
                    {
                        if ( pars->GammaProcessor == 0 )
                        {
                            if ( !pars->noHists ) h2_g_agod->Fill ( dgsEvt[j].ehi, agodEvt[i].values[k] );
                        }
                        else if ( pars->GammaProcessor == 1 )
                        {
                            if ( !pars->noHists ) h2_g_agod->Fill ( gretEvt[j].e, agodEvt[i].values[k] );
                        }
                        else
                        {
                            cout<<"Not a viable gamma ray processor."<<endl;
                            return 1;
                        }
                    }
                }
            }
        }
    }

    /* done */

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "exit BinAGOD\n" );
    }

    return 0;

}

void GODProcessor::SupGOD()
{
    godData = new GoddessData ( pars, gConf );
}

int GODProcessor::BinGOD ( GEB_EVENT* gebEvt, AGODEVENT* agodEvt, DFMAEVENT* dfmaEvt, DGSEVENT* dgsEvt, GRETEVENT* gretEvt )
{
    std::vector<AGODEVENT> agodEvts;
    std::vector<DFMAEVENT> dfmaEvts;
    std::vector<DGSEVENT> dgsEvts;
    std::vector<GRETEVENT> gretEvts;

    for ( unsigned int i = 0; i < *numAGOD; i++ )
    {
        agodEvts.push_back ( agodEvt[i] );
    }

    for ( unsigned int i = 0; i < *numDGOD; i++ )
    {
        dfmaEvts.push_back ( dfmaEvt[i] );
    }

    for ( int i = 0; i < *ng; i++ )
    {
        if ( pars->GammaProcessor == 0 )
        {
            dgsEvts.push_back ( dgsEvt[i] );
        }
        else if ( pars->GammaProcessor == 1 )
        {
            gretEvts.push_back ( gretEvt[i] );
        }

        else
        {
            cout<<"Not a viable gamma ray processor."<<endl;
            return 1;
        }
    }
    return godData->Fill ( gebEvt, &dgsEvts, &dfmaEvts, &agodEvts, &gretEvts );
}
