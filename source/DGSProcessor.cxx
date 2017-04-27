#include "GammasphereAngles.h"

#include "DGSProcessor.h"

extern TH1D* ehi[MAXDETPOS + 1];

DGSProcessor::DGSProcessor ( int nGsGe_,  int* tlkup_, int* tid_, int* ng_, PARS* pars_ )
{
    nGsGe = nGsGe_;

    pars = pars_;

    M = 350.0;

    ehigain = new float[nGsGe + 1];
    ehioffset = new float[nGsGe + 1];
    ehibase = new float[nGsGe + 1];
    ehiPZ = new float[nGsGe + 1];

    angle = new double[nGsGe];
    anglePhi = new double[nGsGe];

    dopCorFac = new double[nGsGe];
    aCFac = new double[nGsGe];

    tlkup = tlkup_;
    tid = tid_;
    ng = ng_;
}

void DGSProcessor::GetCal ( char* file )
{
    int i, ret = 0;
    float a, b, c, d;
    char mystring [1000];
    std::FILE* fp;

    fp = fopen ( file, "r" ); // read mode

    // if Error opening file - end.

    if ( fp == NULL )
    {
        perror ( "Error while opening the file.\n" );
        exit ( EXIT_FAILURE );
    }

    // read file and parse

    while ( fgets ( mystring , 100 , fp ) != NULL )
    {
        //printf("i= %i %s",i,mystring);
        ret = strncmp ( mystring, "#", 1 );
        if ( ret == 0 )
        {
            //printf("%s",mystring);
        }
        else
        {
            ret = sscanf ( mystring, "%i %f %f %f %f", &i, &a, &b, &c, &d );
            if ( ret == 1 )
            {
                M = i;
                printf ( "M = %i\n", i );
            }
            if ( ret == 5 )
            {
                printf ( "%i %f %f %f %f \n", i, a, b, c, d );
                ehiPZ[i] = b;
                ehibase[i] = a;
                ehigain[i] = d;
                ehioffset[i] = c;
            }
        }
    }
    fclose ( fp );
}

int DGSProcessor::SupDGS ()
{
    /* declarations */

    char file_name[] = "./dgscal.dat";      // place this is sort directory

    if ( !pars->noHists )
    {
        gDirectory->mkdir ( "dgs" )->cd();

// 2-D's for Rate

        hEventCounter = make1D ( "EvntCounter", 14400, 0, 14400 ); // Good for 4 hours if Counts/sec
        hGeCounter    = make2D ( "GeCounter", 14400, 0, 14400, 110, 1, 111 );
        hBGOCounter   = make2D ( "BGOCounter", 14400, 0, 14400, 110, 1, 111 );

// 2-D's for Energy

        hEhiRaw   = make2D ( "EhiRaw", 16384, 0, 16384, 110, 1, 111 );
        hEhiCln   = make2D ( "EhiCln", 16384, 0, 16384, 110, 1, 111 );
        hEhiDrty  = make2D ( "EhiDrty", 16384, 0, 16384, 110, 1, 111 );

// 2-D's for Tacs

        hGeBGO_DT = make2D ( "GeBGO_DT", 400, -200, 200, 110, 1, 111 );

// 2-D's for PZ and Baseline

        hTrB = make2D ( "TrBase", 4096, 0, 4096, 110, 1, 111 );
        hFwB = make2D ( "FwBase", 4096, 0, 4096, 110, 1, 111 );

        //  for (int i = 1; i < NGE+1; i++ ){
        //    sprintf(str, "E_TrB%i", i) ;
        //hE_TrB[i] = make2D(str,2500,0,5000,1024,0,8192);
        //  }

        /* list what we have */

        //printf (" we have define the following spectra:\n");

        pars->wlist = gDirectory->GetList();
        //pars->list->Print ();
    }

// Set Default Calibration parameters

    for ( int i = 0; i < nGsGe + 1; i++ )
    {
        printf ( "\nSupDGS %i \n", i );
        ehigain[i] = 1.0;
        ehioffset[i] = 0.0;
        ehiPZ[i] = 1.0;
        ehibase[i] = 0.0;
    };

// This is the DGS calibration file

    GetCal ( file_name );

    printf ( "\nSupDGS done!!\n" );

    return ( 0 );

}

int DGSProcessor::DGSEvDecompose_v3 ( unsigned int* ev, int len, DGSEVENT* thedgsEvt )
{

    /* firmware circa 9/2013 */

    /* declarations */

    int i, k;
    unsigned int ui0 = 0;
    unsigned int t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    unsigned long long int ulli1;

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "entered DGSEvDecompose_v3:\n" );
    }

    /* swap the bytes */

//     std::cerr << "====Entered DGSEvDecompose_v3====\n";
//     std::cerr << "Swapping the bytes........\n";

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

    // Decode the generic part of the header.

//     std::cerr << "Decode the generic part of the header...\n";

    thedgsEvt->chan_id = ( * ( ev + 0 ) & 0x0000000f );
    thedgsEvt->board_id = ( ( * ( ev + 0 ) & 0x0000fff0 ) >> 4 ); // USER_DEF
    thedgsEvt->id = thedgsEvt->board_id * 10 + thedgsEvt->chan_id;
    thedgsEvt->packet_length = ( ( * ( ev + 0 ) & 0x07ff0000 ) >> 16 );
    thedgsEvt->geo_addr = ( ( * ( ev + 0 ) & 0xf8000000 ) >> 27 );

    thedgsEvt->header_type = ( ( * ( ev + 2 ) & 0x000f0000 )  >> 16 );
    thedgsEvt->event_type = ( ( * ( ev + 2 ) & 0x03800000 )  >> 23 ); // hope this is right.
    thedgsEvt->header_length = ( ( * ( ev + 2 ) & 0xFC000000 )  >> 26 ); // hope this is right.

    /* extract the LED time stamp */
//     std::cerr << "Extract the LED timestamp...\n";

    thedgsEvt->event_timestamp = 0;
    thedgsEvt->event_timestamp = ( unsigned long long int ) * ( ev + 1 );
    ulli1 = ( unsigned long long int ) ( * ( ev + 2 ) & 0x0000ffff );
    ulli1 = ( ulli1 << 32 );
    thedgsEvt->event_timestamp += ulli1;

    /* store the type and type ID */
//     std::cerr << "Store the type and typeID... (dgsEvt ID = " << thedgsEvt->id << " )\n";

    thedgsEvt->tpe = tlkup[thedgsEvt->id];
    thedgsEvt->tid = tid[thedgsEvt->id];
    thedgsEvt->flag = 0;

    /* extract the energy */
//     std::cerr << "Extract the energy...\n";

    switch ( thedgsEvt->header_type )
    {
    case 0:                // Original LED header (Compatibility mode)
        thedgsEvt->timestamp_match_flag   = 0;
        thedgsEvt->external_disc_flag     = 0;
        thedgsEvt->cfd_valid_flag         = 0;
        thedgsEvt->pileup_only_flag       = 0;
        thedgsEvt->cfd_sample_0           = 0;
        thedgsEvt->cfd_sample_1           = 0;
        thedgsEvt->cfd_sample_2           = 0;

        thedgsEvt->peak_valid_flag        = ( ( * ( ev + 3 ) & 0x00000200 ) >> 9 );                        // Word 3: 9
        thedgsEvt->offset_flag            = ( ( * ( ev + 3 ) & 0x00000400 ) >> 10 );                       // Word 3: 10
        thedgsEvt->sync_error_flag        = ( ( * ( ev + 3 ) & 0x00001000 ) >> 12 );                       // Word 3: 12
        thedgsEvt->general_error_flag     = ( ( * ( ev + 3 ) & 0x00002000 ) >> 13 );                       // Word 3: 13
        thedgsEvt->pileup_flag            = ( ( * ( ev + 3 ) & 0x00008000 ) >> 15 );                       // Word 3: 15
        thedgsEvt->last_disc_timestamp    = ( ( ( unsigned long long int ) ( * ( ev + 3 ) & 0xFFFF0000 ) ) >> 16 ) | // Word 3: 31..16 &
                                            ( ( ( unsigned long long int ) ( * ( ev + 4 ) & 0xFFFFFFFF ) ) << 16 ); // Word 4 :31..0
        thedgsEvt->sampled_baseline       = ( ( * ( ev + 5 ) & 0x00FFFFFF ) >> 0 );                        // Word 5: 23..0
        thedgsEvt->pre_rise_energy        = ( ( * ( ev + 7 ) & 0x00FFFFFF ) >> 0 );                        // Word 7: 23..0
        thedgsEvt->post_rise_energy       = ( ( * ( ev + 7 ) & 0xFF000000 ) >> 28 ) |                      // Word 7: 31..24 &
                                            ( ( * ( ev + 8 ) & 0x0000FFFF ) << 8 );                        // Word 8: 15..0
        thedgsEvt->peak_timestamp         = ( ( ( unsigned long long int ) ( * ( ev + 8 ) & 0xFFFF0000 ) ) >> 16 ) | // Word 8: 31..16 &
                                            ( ( ( unsigned long long int ) ( * ( ev + 9 ) & 0xFFFFFFFF ) ) << 16 ); // Word 9 :31..0
        thedgsEvt->m1_begin_sample        = ( ( * ( ev + 10 ) & 0x00003FFF ) >> 0 );                       // Word 10:13..0
        thedgsEvt->m1_end_sample          = ( ( * ( ev + 10 ) & 0x3FFF0000 ) >> 16 );                      // Word 10:29..16
        thedgsEvt->m2_begin_sample        = ( ( * ( ev + 11 ) & 0x00003FFF ) >> 0 );                       // Word 11:13..0
        thedgsEvt->m2_end_sample          = ( ( * ( ev + 11 ) & 0x3FFF0000 ) >> 16 );                      // Word 11:29..16
        thedgsEvt->peak_sample            = ( ( * ( ev + 12 ) & 0x00003FFF ) >> 0 );                       // Word 12:13..0
        thedgsEvt->base_sample            = ( ( * ( ev + 12 ) & 0x3FFF0000 ) >> 16 );                      // Word 12:29..16
        break;

    case 1:                // New LED Header
        thedgsEvt->timestamp_match_flag   = 0;
        thedgsEvt->cfd_valid_flag         = 0;
        thedgsEvt->cfd_sample_0           = 0;
        thedgsEvt->cfd_sample_1           = 0;
        thedgsEvt->cfd_sample_2           = 0;

        thedgsEvt->external_disc_flag     = ( ( * ( ev + 3 ) & 0x00000100 ) >> 8 );                        // Word 3: 8
        thedgsEvt->peak_valid_flag        = ( ( * ( ev + 3 ) & 0x00000200 ) >> 9 );                        // Word 3: 9
        thedgsEvt->offset_flag            = ( ( * ( ev + 3 ) & 0x00000400 ) >> 10 );                       // Word 3: 10
        thedgsEvt->sync_error_flag        = ( ( * ( ev + 3 ) & 0x00001000 ) >> 12 );                       // Word 3: 12
        thedgsEvt->general_error_flag     = ( ( * ( ev + 3 ) & 0x00002000 ) >> 13 );                       // Word 3: 13
        thedgsEvt->pileup_only_flag       = ( ( * ( ev + 3 ) & 0x00004000 ) >> 14 );                       // Word 3: 14
        thedgsEvt->pileup_flag            = ( ( * ( ev + 3 ) & 0x00008000 ) >> 15 );                       // Word 3: 15
        thedgsEvt->last_disc_timestamp    = ( ( ( unsigned long long int ) ( * ( ev + 3 ) & 0xFFFF0000 ) ) >> 16 ) | // Word 3: 31..16 &
                                            ( ( ( unsigned long long int ) ( * ( ev + 4 ) & 0xFFFFFFFF ) ) << 16 ); // Word 4 :31..0
        thedgsEvt->sampled_baseline       = ( ( * ( ev + 5 ) & 0x00FFFFFF ) >> 0 );                        // Word 5: 23..0
        thedgsEvt->pre_rise_energy        = ( ( * ( ev + 7 ) & 0x00FFFFFF ) >> 0 );                        // Word 7: 23..0
        thedgsEvt->post_rise_energy       = ( ( * ( ev + 7 ) & 0xFF000000 ) >> 28 ) |                      // Word 7: 31..24 &
                                            ( ( * ( ev + 8 ) & 0x0000FFFF ) << 8 );                        // Word 8: 15..0
        thedgsEvt->peak_timestamp         = ( ( ( unsigned long long int ) ( * ( ev + 8 ) & 0xFFFF0000 ) ) >> 16 ) | // Word 8: 31..16 &
                                            ( ( ( unsigned long long int ) ( * ( ev + 9 ) & 0xFFFFFFFF ) ) << 16 ); // Word 9: 31..0
        thedgsEvt->m1_begin_sample        = ( ( * ( ev + 10 ) & 0x00003FFF ) >> 0 );                       // Word 10:13..0
        thedgsEvt->m1_end_sample          = ( ( * ( ev + 10 ) & 0x3FFF0000 ) >> 16 );                      // Word 10:29..16
        thedgsEvt->m2_begin_sample        = ( ( * ( ev + 11 ) & 0x00003FFF ) >> 0 );                       // Word 11:13..0
        thedgsEvt->m2_end_sample          = ( ( * ( ev + 11 ) & 0x3FFF0000 ) >> 16 );                      // Word 11:29..16
        thedgsEvt->peak_sample            = ( ( * ( ev + 12 ) & 0x00003FFF ) >> 0 );                       // Word 12:13..0
        thedgsEvt->base_sample            = ( ( * ( ev + 12 ) & 0x3FFF0000 ) >> 16 );                      // Word 12:29..16
        break;

    case 2:                // CFD Header
        thedgsEvt->timestamp_match_flag   = ( ( * ( ev + 3 ) & 0x00000080 ) >> 7 );                        // Word 3: 7
        thedgsEvt->external_disc_flag     = ( ( * ( ev + 3 ) & 0x00000100 ) >> 8 );                        // Word 3: 8
        thedgsEvt->peak_valid_flag        = ( ( * ( ev + 3 ) & 0x00000200 ) >> 9 );                        // Word 3: 9
        thedgsEvt->offset_flag            = ( ( * ( ev + 3 ) & 0x00000400 ) >> 10 );                       // Word 3: 10
        thedgsEvt->cfd_valid_flag         = ( ( * ( ev + 3 ) & 0x00000800 ) >> 11 );                       // Word 3: 11
        thedgsEvt->sync_error_flag        = ( ( * ( ev + 3 ) & 0x00001000 ) >> 12 );                       // Word 3: 12
        thedgsEvt->general_error_flag     = ( ( * ( ev + 3 ) & 0x00002000 ) >> 13 );                       // Word 3: 13
        thedgsEvt->pileup_only_flag       = ( ( * ( ev + 3 ) & 0x00004000 ) >> 14 );                       // Word 3: 14
        thedgsEvt->pileup_flag            = ( ( * ( ev + 3 ) & 0x00008000 ) >> 15 );                       // Word 3: 15
        thedgsEvt->last_disc_timestamp    = ( ( ( unsigned long long int ) ( * ( ev + 3 ) & 0xFFFF0000 ) ) >> 16 ) | // Word 3: 31..16 &
                                            ( ( ( unsigned long long int ) ( * ( ev + 4 ) & 0xFFFFFFFF ) ) << 16 ); // Word 4 :31..0
        thedgsEvt->cfd_sample_0           = ( ( * ( ev + 4 ) & 0x3FFF0000 ) >> 16 );                       // Word 4: 29..16
        thedgsEvt->sampled_baseline       = ( ( * ( ev + 5 ) & 0x00FFFFFF ) >> 0 );                        // Word 5: 23..0
        thedgsEvt->cfd_sample_1           = ( ( * ( ev + 6 ) & 0x00003FFF ) >> 0 );                        // Word 6: 13..0
        thedgsEvt->cfd_sample_2           = ( ( * ( ev + 6 ) & 0x3FFF0000 ) >> 16 );                       // Word 6: 29..16
        thedgsEvt->pre_rise_energy        = ( ( * ( ev + 7 ) & 0x00FFFFFF ) >> 0 );                        // Word 7: 23..0
        thedgsEvt->post_rise_energy       = ( ( * ( ev + 7 ) & 0xFF000000 ) >> 28 ) |                      // Word 7: 31..24 &
                                            ( ( * ( ev + 8 ) & 0x0000FFFF ) << 8 );                        // Word 8: 15..0
        thedgsEvt->peak_timestamp         = ( ( ( unsigned long long int ) ( * ( ev + 8 ) & 0xFFFF0000 ) ) >> 16 ) | // Word 8: 31..16 &
                                            ( ( ( unsigned long long int ) ( * ( ev + 9 ) & 0xFFFFFFFF ) ) << 16 ); // Word 9: 31..0
        thedgsEvt->m1_begin_sample        = ( ( * ( ev + 10 ) & 0x00003FFF ) >> 0 );                       // Word 10:13..0
        thedgsEvt->m1_end_sample          = ( ( * ( ev + 10 ) & 0x3FFF0000 ) >> 16 );                      // Word 10:29..16
        thedgsEvt->m2_begin_sample        = ( ( * ( ev + 11 ) & 0x00003FFF ) >> 0 );                       // Word 11:13..0
        thedgsEvt->m2_end_sample          = ( ( * ( ev + 11 ) & 0x3FFF0000 ) >> 16 );                      // Word 11:29..16
        thedgsEvt->peak_sample            = ( ( * ( ev + 12 ) & 0x00003FFF ) >> 0 );                       // Word 12:13..0
        thedgsEvt->base_sample            = ( ( * ( ev + 12 ) & 0x3FFF0000 ) >> 16 );                      // Word 12:29..16
        break;
    }

    thedgsEvt->baseline = ( thedgsEvt->m1_begin_sample + thedgsEvt->m1_end_sample ) / 2 ;


    /* Now load Trace into thedgsEvt Structure */
//     std::cerr << "Load trace into the dgsEvt structure...\n";

    thedgsEvt->traceLen = 0;
    for ( i = 13; i < len - 1; i++ )
    {
        if ( i < 1037 )
        {
            thedgsEvt->trace[2 * ( i - 13 )] = ( unsigned short int ) ( * ( ev + i ) & 0xffff );
            thedgsEvt->trace[2 * ( i - 13 ) + 1] = ( unsigned short int ) ( ( * ( ev + i ) >> 16 ) & 0xffff );
            thedgsEvt->traceLen += 2;
        }
    }

    /* done */

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "exit DGSEvDecompose_v3:\n" );
    }

    return 0;

}

/* ----------------------------------------------------------------- */

int DGSProcessor::BinDgs ( GEB_EVENT* theGEBEvent, DGSEVENT* thedgsEvt )
{
    /* declarations */

    char str[128];
    int j;
    //DGSEVENT dgsEvt[MAXCOINEV];

    int RelEvT = 0, tdiff = 0;
    float Energy;

    /* prototypes */

    int GebTypeStr ( int type, char str[] );

    /* Print debug */

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "entered bin_dgs:\n" );
    }

//     std::cerr << "****************Entered bin_dgs****************\n";
//     std::cerr << "theGEBEvent size = " << theGEBEvent->ptgd.size() << "\n";

    /* loop through the coincidence event and fish out DGS data */
    /* (gamma rays) count in ng */

    *ng = 0;

    for ( unsigned int i = 0; i < theGEBEvent->ptgd.size(); i++ )
    {
//       std::cerr << "Found theGEBEvent type " << theGEBEvent->ptgd[i]->type << " (DGS type is " << GEB_TYPE_DGS << ")\n";
//       std::cerr << "--> length: " << theGEBEvent->ptgd[i]->length << "\n";
//       std::cerr << "--> timestamp: " << theGEBEvent->ptgd[i]->timestamp << "\n";

        if ( theGEBEvent->ptgd[i]->type == GEB_TYPE_DGS )
        {
            if ( pars->CurEvNo <= pars->NumToPrint )
            {
                GebTypeStr ( theGEBEvent->ptgd[i]->type, str );
                printf ( "bin_mode1, %2i> %2i, %s, TS=%lli\n", i, theGEBEvent->ptgd[i]->type, str,
                         theGEBEvent->ptgd[i]->timestamp );
            }

            DGSEvDecompose_v3 ( ( unsigned int* ) theGEBEvent->ptinp[i], theGEBEvent->ptgd[i]->length / sizeof ( unsigned int ), &thedgsEvt[*ng] );

//             std::cerr << "DGS event: theGEBEvent TS = " << theGEBEvent->ptgd[i]->timestamp << " / DGSEents TS = " << thedgsEvt[*ng].event_timestamp << "\n";

            ( *ng ) ++;
        }
    }

    // Initialize EvTimeStam0

    //printf("Stat 1 \n");
    if ( pars->CurEvNo < 100 )
    {
        printf ( "\n\nCurEvNo: %i", pars->CurEvNo );
        for ( int i = 0; i < *ng; i++ )
        {
            printf ( "\n thedgsEvt[%i].event_timestamp: %llu", i, thedgsEvt[i].event_timestamp );
        }
    }

    if ( EvTimeStam0 == 0 )
    {
        EvTimeStam0 = thedgsEvt[0].event_timestamp;
    }
    RelEvT = ( int ) ( ( thedgsEvt[0].event_timestamp - EvTimeStam0 ) / 100000000 ); // overflow?
    if ( !pars->noHists ) hEventCounter->Fill ( RelEvT );


// GS ENERGY CALIBRATION

    double d1;

    /* Compton Suppression Loop */

    for ( int i = 0; i < *ng; i++ )
    {
        if ( thedgsEvt[i].tpe == GE )
        {
            int gsid = thedgsEvt[i].tid;
//             if ( !pars->noHists ) hGeCounter->Fill ( ( int ) ( ( thedgsEvt[0].event_timestamp - EvTimeStam0 ) / 100000000 ), gsid );
            Energy = ( ( float ) ( thedgsEvt[i].post_rise_energy ) - ( float ) ( thedgsEvt[i].pre_rise_energy ) * ehiPZ[gsid] ) / M * ehigain[gsid];
            Energy = Energy - float ( thedgsEvt[i].base_sample ) * ( 1. - ehiPZ[gsid] ) * ehigain[gsid] + ehioffset[gsid];


            if ( pars->beta != 0 )
            {
//                 std::cerr << "Beta used for Doppler Correction: " << pars->beta << std::endl;

                d1 = angtheta[gsid - 1] / 57.29577951;
                Energy = Energy * ( 1 - pars->beta * cos ( d1 ) ) / sqrt ( 1 - pars->beta * pars->beta );
            }

            thedgsEvt[i].ehi = Energy;

            for ( j = 0; j < *ng; j++ )
            {
                if ( thedgsEvt[j].tpe == BGO && thedgsEvt[j].tid == gsid )    // BGO & GE in coincidence
                {
                    tdiff = ( int ) ( thedgsEvt[i].event_timestamp - thedgsEvt[j].event_timestamp );
//                     if ( !pars->noHists ) hGeBGO_DT->Fill ( tdiff, gsid );
                    if ( abs ( tdiff ) <= 50 )
                    {
                        thedgsEvt[i].flag = 1;    // Mark as Dirty Ge
                    }
                }
            }
        }
        else if ( thedgsEvt[i].tpe == BGO )
        {
//             if ( !pars->noHists ) hBGOCounter->Fill ( ( int ) ( ( thedgsEvt[0].event_timestamp - EvTimeStam0 ) / 100000000 ), thedgsEvt[i].tid );
            thedgsEvt[i].ehi = ( float ) ( thedgsEvt[i].post_rise_energy ) - ( float ) ( thedgsEvt[i].pre_rise_energy );
        }
        else if ( thedgsEvt[i].tpe == SIDE )
        {
            std::cerr << "Got a SIDE contact of Gammasphere here!\n";

            thedgsEvt[i].ehi = ( float ) ( thedgsEvt[i].post_rise_energy ) - ( float ) ( thedgsEvt[i].pre_rise_energy );
        }
    }

    /* Energy Histogram loop */

//     for ( int i = 0; i < *ng; i++ )
//     {
//         if ( !pars->noHists&& thedgsEvt[i].tpe == GE )
//         {
//             int e = ( int ) thedgsEvt[i].ehi;
//             int gsid = thedgsEvt[i].tid;
//
//             hEhiRaw->Fill ( e, gsid );
//             if ( thedgsEvt[i].flag == 0 )
//             {
//                 hEhiCln->Fill ( e, gsid );
//             }
//             if ( thedgsEvt[i].flag == 1 )
//             {
//                 hEhiDrty->Fill ( e, gsid );
//             }
//         }
//     }
//
//     /* Histogram's for PZ and baseline correction */
//
//     for ( int i = 0; i < *ng; i++ )
//     {
//         if ( !pars->noHists && thedgsEvt[i].tpe == GE )
//         {
//             int gsid = thedgsEvt[i].tid;
//
//             hTrB->Fill ( thedgsEvt[i].baseline, gsid );
//             hFwB->Fill ( thedgsEvt[i].base_sample, gsid );;
//         }
//     }

    /* debug list the gamma rays we found */

    if ( pars->CurEvNo <= pars->NumToPrint )
        for ( int i = 0; i < *ng; i++ )
        {
            printf ( "we have %i gamma rays\n", *ng );
            printf ( "%2i> ", i );
            printf ( "chan_id=%i; ", thedgsEvt[i].chan_id );
            printf ( "board_id=%i; ", thedgsEvt[i].board_id );
            printf ( "id =%i; ", thedgsEvt[i].id );
            printf ( "tpe=%i; ", thedgsEvt[i].tpe );
            printf ( "tid=%i; ", thedgsEvt[i].tid );
            printf ( "EventTS=%llu; ", thedgsEvt[i].event_timestamp );
            printf ( "ehi=%8f ", thedgsEvt[i].ehi );
            printf ( "\n" );
            fflush ( stdout );
        };

    /* done */

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "exit bin_dgs\n" );
    }

    return ( 0 );



}

/*-----------------------------------------------------------*/


