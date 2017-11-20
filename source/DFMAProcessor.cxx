#include "DFMAProcessor.h"

DFMAProcessor::DFMAProcessor ( int* tlkup_, int* tid_, int* ng_, PARS* pars_ )
{
    RealCount = 0;
    trn1 = 0;
    trn2 = 0;
    TestTr1 = 0;
    TestTr2 = 0;
    TestTr3 = 0;
    TestTr4 = 0;
    leTr = 0;
    numDFMA = 0;
    numDGS = 0;

    tlkup = tlkup_;
    tid = tid_;
    ng = ng_;
}

int DFMAProcessor::ExitDFMA()
{
    int i, j;

    for ( i = 1; i < 161; i++ )
    {
        for ( j = 1; j < 161; j++ )
        {

            if ( dssd_corr[i][j].status > 1 )
            {
                chain = dssd_corr[i][j].chain;
                tree->Fill();
            }

        }
    }

#if(1)
    treef->cd();

    tree->Write ( "tree" );
    pars.f1->cd();
    treef->Close();
#endif

    return ( 0 );
}

/*-----------------------------------------------------*/

int DFMAProcessor::SupDFMA()
{

#if(1)
    // declarations *


    //char str1[STRLEN], str2[STRLEN];//unused
    //float pi;//unused
    int i, j;

    // gDirectory->mkdir("fma")->cd();

    TH1D* mkTH1D ( char*, char*, int, double, double );
    TH2F* mkTH2F ( char*, char*, int, double, double, int, double, double );

    char presortFile[100];
    char compstr[10];
    char onechar[10];

    t_first = 0;
    t_firstge = 0;
    t_last = 0;
    CurSubEvNo = 0;

// //TREES...
    strcpy ( compstr, "2" );
    strcpy ( presortFile, "TREE_FILES/new_presort." );

    i = strlen ( pars.ROOTFile );

    while ( pars.ROOTFile[i] != '/' )
    {
        i--;
    }

    for ( j = i + 1; j < static_cast<int> ( strlen ( pars.ROOTFile ) ) ; j++ )
    {
        onechar[0] = pars.ROOTFile[j];
        onechar[1] = '\0';
        strcat ( presortFile, onechar );
    }

    printf ( "ROOT file %s\n", pars.ROOTFile );
    printf ( "presort file %s\n", presortFile );



#if(1)
    treef = new TFile ( presortFile, "recreate" );
    tree = new TTree ( "tree", "presorted data" );

    pars.f1->cd();

    tree->Branch ( "s_fr", &chain.s_fr, "s_fr/i" );
    tree->Branch ( "s_ba", &chain.s_ba, "s_ba/i" );

    tree->Branch ( "recoil", &chain.recoil, "ts/l:en/i:pu/i:left/F:right/F:x/D:d2t0/i:d2t1/i:d2t2/i:nge/i:geehi[15]/L:tge[15]/l:getid[15]/i:traceLen/i:trace_fr[1000]/S:trace_ba[1000]/S" );

//tree->Branch("recoil",&chain.recoil,"ts/l:en/i:toten/i:pu:right:left:ppacde:ppacde2:icde:nge:geehi[15]/L:tge[15]/l:getid[15]/i:tof/l:d2t0/i:d2t1/i:fdecEv/i:traceLen/s:trace_fr[1000]/S:trace_ba[1000]/S");
    tree->Branch ( "ndec", &chain.ndec, "ndec/i" );

    tree->Branch ( "dec1", &chain.decay[0], "ts/l:en/i:pu_fr/i:pu_ba/i:time/l:traceLen/i:trace_fr[1000]/S:trace_ba[1000]/S:nge/i:geehi[15]/L:tge[15]/l:getid[15]/i:d2t0/i:d2t1/i:d2t2/i" );
    tree->Branch ( "dec2", &chain.decay[1], "ts/l:en/i:pu_fr/i:pu_ba/i:time/l:traceLen/i:trace_fr[1000]/S:trace_ba[1000]/S:nge/i:geehi[15]/L:tge[15]/l:getid[15]/i:d2t0/i:d2t1/i:d2t2/i" );
    tree->Branch ( "dec3", &chain.decay[2], "ts/l:en/i:pu_fr/i:pu_ba/i:time/l:traceLen/i:trace_fr[1000]/S:trace_ba[1000]/S:nge/i:geehi[15]/L:tge[15]/l:getid[15]/i:d2t0/i:d2t1/i:d2t2/i" );

//tree->Branch("dec1",&chain.decay[0],"ts/l:en/i:pu_fr/i:pu_ba/i:time/l:traceLen/i:trace_fr[1000]/S:trace_ba[1000]/S:nge/i:geehi[15]/L:tge[15]/l:getid[15]/i");
//tree->Branch("dec2",&chain.decay[1],"ts/l:en/i:pu_fr/i:pu_ba/i:time/l:traceLen/i:trace_fr[1000]/S:trace_ba[1000]/S:nge/i:geehi[15]/L:tge[15]/l:getid[15]/i");
//tree->Branch("dec3",&chain.decay[2],"ts/l:en/i:pu_fr/i:pu_ba/i:time/l:traceLen/i:trace_fr[1000]/S:trace_ba[1000]/S:nge/i:geehi[15]/L:tge[15]/l:getid[15]/i");


#endif


////TREES...

//***************************
// Histogram initialisation *
//***************************

// LIKE USERINIT.H!!!


    h1_short_dec_en = mkTH1D ( ( char* ) "short_dec_en", ( char* ) "short_dec_en", 5000, 0, 50000 );
    h2_le_output_left = mkTH2F ( ( char* ) "le_output_left", ( char* ) "le_output_left", 100, 0, 100, 1000, 0, 1000 );

    h2_rec_gammas = mkTH2F ( ( char* ) "rec_gammas", ( char* ) "rec_gammas", 10000, 0, 10000, 120, 0, 120 );
    h2_all_gammas = mkTH2F ( ( char* ) "all_gammas", ( char* ) "all_gammas", 10000, 0, 10000, 120, 0, 120 );
    h2_dssd_gammas = mkTH2F ( ( char* ) "dssd_gammas", ( char* ) "dssd_gammas", 10000, 0, 10000, 120, 0, 120 );
    h2_mcp_gammas = mkTH2F ( ( char* ) "mcp_gammas", ( char* ) "mcp_gammas", 10000, 0, 10000, 120, 0, 120 );
//h2_corr_gammas = mkTH2F((char *)"corr_gammas",(char *)"corr_gammas",5000,0,50000,1000,0,10000);
    h2_corr_gammas_short = mkTH2F ( ( char* ) "corr_gammas_short", ( char* ) "corr_gammas_short", 5000, 0, 50000, 5000, 0, 5000 );
    h2_gamma_gamma_shortdec = mkTH2F ( ( char* ) "gamma_gamma_shortdec", ( char* ) "gamma_gamma_shortdec", 1000, 0, 6000, 1000, 0, 6000 );
    h2_dTgdssd = mkTH2F ( ( char* ) "dTgdssd", ( char* ) "dTgdssd", 4000, -2000, 2000, 400, 0, 400 );
    h2_dTgdssd_ch40 = mkTH2F ( ( char* ) "dTgdssd_ch40", ( char* ) "dTgdssd_ch40", 4000, -2000, 2000, 400, 0, 400 );
    h2_dTgdssd_allge = mkTH2F ( ( char* ) "dTgdssd_allge", ( char* ) "dTgdssd_allge", 4000, -2000, 2000, 400, 0, 400 );
    h1_dTgl = mkTH1D ( ( char* ) "dTgl", ( char* ) "dTgl", 4000, -2000, 2000 );
    h1_dTgr = mkTH1D ( ( char* ) "dTgr", ( char* ) "dTgr", 4000, -2000, 2000 );

    h2_stripfr_cxng = mkTH2F ( ( char* ) "stripfr_cxng", ( char* ) "stripfr_cxng", 200, 0, 200, 4000, 0, 4000 );
    h2_stripba_cxng = mkTH2F ( ( char* ) "stripba_cxng", ( char* ) "stripba_cxng", 200, 0, 200, 4000, 0, 4000 );
    h2_stripba_cxng_frontgated = mkTH2F ( ( char* ) "stripba_cxng_frontgated", ( char* ) "stripba_cxng_frontgated", 200, 0, 200, 4000, 0, 4000 );
    h1_esi = mkTH1D ( ( char* ) "esi", ( char* ) "esi", 4000, 0, 40000 );
    h1_cl = mkTH1D ( ( char* ) "cl", ( char* ) "cl", 4000, 0, 40000 );
    h1_cr = mkTH1D ( ( char* ) "cr", ( char* ) "cr", 4000, 0, 40000 );
    h1_cx = mkTH1D ( ( char* ) "cx", ( char* ) "cx", 4000, 0, 4000 );
    h1_cxn = mkTH1D ( ( char* ) "cxn", ( char* ) "cxn", 4000, 0, 4000 );
    h2_clr = mkTH2F ( ( char* ) "clr", ( char* ) "clr", 4000, 0, 40000, 4000, 0, 40000 );

    h1_cx_en_g1 = mkTH1D ( ( char* ) "cx_en_g1", ( char* ) "cx_en_g1", 4000, 0, 4000 );
    h1_cxn_en_g1 = mkTH1D ( ( char* ) "cxn_en_g1", ( char* ) "cxn_en_g1", 4000, 0, 4000 );
    h1_cx_en_g2 = mkTH1D ( ( char* ) "cx_en_g2", ( char* ) "cx_en_g2", 4000, 0, 4000 );
    h1_cxn_en_g2 = mkTH1D ( ( char* ) "cxn_en_g2", ( char* ) "cxn_en_g2", 4000, 0, 4000 );



    h2_x_d_fr_e = mkTH2F ( ( char* ) "x_d_fr_e", ( char* ) "x_d_fr_e", 2000, 0, 4000, 5000, 0, 50000 );
    h2_x_d_fr_e_short = mkTH2F ( ( char* ) "x_d_fr_e_short", ( char* ) "x_d_fr_e_short", 2000, 0, 4000, 5000, 0, 50000 );
    h2_xehi = mkTH2F ( ( char* ) "xehi", ( char* ) "xehi", 2000, 0, 4000, 1000, 0, 10000 );
    h2_xehig = mkTH2F ( ( char* ) "xehig", ( char* ) "xehig", 2000, 0, 4000, 10000, 0, 10000 );
    h1_clg = mkTH1D ( ( char* ) "clg", ( char* ) "clg", 4000, 0, 40000 );
    h1_crg = mkTH1D ( ( char* ) "crg", ( char* ) "crg", 4000, 0, 40000 );
    h2_clrg = mkTH2F ( ( char* ) "clrg", ( char* ) "clrg", 4000, 0, 40000, 4000, 0, 40000 );
    h2_clrg_int = mkTH2F ( ( char* ) "clrg_int", ( char* ) "clrg_int", 4000, 0, 40000, 4000, 0, 40000 ); //new
    h1_cxg = mkTH1D ( ( char* ) "cxg", ( char* ) "cxg", 4000, 0, 4000 );
    h1_cxng = mkTH1D ( ( char* ) "cxng", ( char* ) "cxng", 4000, 0, 4000 );
    h2_clrg_en = mkTH2F ( ( char* ) "clrg_en", ( char* ) "clrg_en", 4000, 0, 40000, 4000, 0, 40000 );
    h2_clrg_en_g1 = mkTH2F ( ( char* ) "clrg_en_g1", ( char* ) "clrg_en_g1", 4000, 0, 40000, 4000, 0, 40000 );
    h2_clrg_en_g2 = mkTH2F ( ( char* ) "clrg_en_g2", ( char* ) "clrg_en_g2", 4000, 0, 40000, 4000, 0, 40000 );
    h2_traceg = mkTH2F ( ( char* ) "traceg", ( char* ) "traceg", 1000, 0, 1000, 1000, 0, 1000 );
    h2_traceg2 = mkTH2F ( ( char* ) "traceg2", ( char* ) "traceg2", 1000, 0, 1000, 1000, 0, 1000 );
// DSSD stuff

    h2_dssd_fr_emax = mkTH2F ( ( char* ) "dssd_fr_emax", ( char* ) "dssd_fr_emax", 5000, 0, 50000, 400, 0, 400 );
    h2_dssd_ba_emax = mkTH2F ( ( char* ) "dssd_ba_emax", ( char* ) "dssd_ba_emax", 5000, 0, 50000, 400, 0, 400 );

    h2_d_fr_e = mkTH2F ( ( char* ) "d_fr_e", ( char* ) "d_fr_e", 5000, 0, 50000, 400, 0, 400 );
    h2_d_ba_e = mkTH2F ( ( char* ) "d_ba_e", ( char* ) "d_ba_e", 5000, 0, 50000, 400, 0, 400 );
    h2_r_fr_e = mkTH2F ( ( char* ) "r_fr_e", ( char* ) "r_fr_e", 5000, 0, 50000, 400, 0, 400 );
    h2_r_ba_e = mkTH2F ( ( char* ) "r_ba_e", ( char* ) "r_ba_e", 5000, 0, 50000, 400, 0, 400 );


    h2_dssd_en_raw = mkTH2F ( ( char* ) "dssd_en_raw", ( char* ) "dssd_en_raw", 2000, 0, 100000, 400, 0, 400 );
    h2_dssd_en = mkTH2F ( ( char* ) "dssd_en", ( char* ) "dssd_en", 5000, 0, 50000, 400, 0, 400 );
    h2_dssd_rate = mkTH2F ( ( char* ) "dssd_rate", ( char* ) "dssd_rate", 5000, 0, 50000, 350, 0, 350 );
//h1_dssd_rate_1us = mkTH1D((char *)"dssd_rate_1us",(char *)"dssd_rate_1us",1000000,0,1000000);
    h1_decay_rate = mkTH1D ( ( char* ) "decay_rate", ( char* ) "decay_rate", 100000, 0, 100000 );
    h1_dssd_rate_1D = mkTH1D ( ( char* ) "dssd_rate_1D", ( char* ) "dssd_rate_1D", 100000, 0, 100000 );
    h2_FP_rate = mkTH2F ( ( char* ) "FP_rate", ( char* ) "FP_rate", 5000, 0, 5000, 10, 0, 10 );
    h1_FP_rate_left = mkTH1D ( ( char* ) "FP_rate_left", ( char* ) "FP_rate_left", 100000, 0, 100000 );
    h1_FP_rate_right = mkTH1D ( ( char* ) "FP_rate_right", ( char* ) "FP_rate_right", 100000, 0, 100000 );
    h1_GE_rate = mkTH1D ( ( char* ) "GE_rate", ( char* ) "GE_rate", 100000, 0, 100000 );
    h1_cl_int = mkTH1D ( ( char* ) "cl_int", ( char* ) "cl_int", 4000, 0, 40000 );
    h1_cr_int = mkTH1D ( ( char* ) "cr_int", ( char* ) "cr_int", 4000, 0, 40000 );
    h1_cxg_int = mkTH1D ( ( char* ) "cxg_int", ( char* ) "cxg_int", 4000, 0, 4000 );
    h1_cxng_int = mkTH1D ( ( char* ) "cxng_int", ( char* ) "cxng_int", 4000, 0, 4000 );
    h2_clr_int = mkTH2F ( ( char* ) "clr_int", ( char* ) "clr_int", 4000, 0, 40000, 4000, 0, 40000 );

    h2_tdssdmcp_fr = mkTH2F ( ( char* ) "tdssdmcp_fr", ( char* ) "tdssdmcp_fr", 1000, -500, 500, 200, 0, 200 );
    h2_tdssdmcp_ba = mkTH2F ( ( char* ) "tdssdmcp_ba", ( char* ) "tdssdmcp_ba", 1000, -500, 500, 200, 0, 200 );
    h2_tdssdmcp_fr_r = mkTH2F ( ( char* ) "tdssdmcp_fr_r", ( char* ) "tdssdmcp_fr_r", 1000, -500, 500, 200, 0, 200 );
    h2_tdssdmcp_ba_r = mkTH2F ( ( char* ) "tdssdmcp_ba_r", ( char* ) "tdssdmcp_ba_r", 1000, -500, 500, 200, 0, 200 );
    h1_ndssd = mkTH1D ( ( char* ) "ndssd", ( char* ) "ndssd", 350, 0, 350 );

    h2_lefttraces = mkTH2F ( ( char* ) "lefttraces", ( char* ) "lefttraces", 100, 0, 100, 1000, 0, 1000 );
    h2_righttraces = mkTH2F ( ( char* ) "righttraces", ( char* ) "righttraces", 100, 0, 100, 1000, 0, 1000 );

    h2_dssd_traces_fr = mkTH2F ( ( char* ) "dssd_traces_fr", ( char* ) "dssd_traces_fr", 100, 0, 100, 1000, 0, 1000 );
    h2_dssd_traces_ba = mkTH2F ( ( char* ) "dssd_traces_ba", ( char* ) "dssd_traces_ba", 100, 0, 100, 1000, 0, 1000 );

    h2_frba_t = mkTH2F ( ( char* ) "frba_t", ( char* ) "frba_t", 1000, -500, 500, 350, 0, 350 );


    h1_ch0 = mkTH1D ( ( char* ) "ch0", ( char* ) "ch0", 4000, 0, 40000 );
    h1_ch1 = mkTH1D ( ( char* ) "ch1", ( char* ) "ch1", 4000, 0, 40000 );
    h1_ch2 = mkTH1D ( ( char* ) "ch2", ( char* ) "ch2", 4000, 0, 40000 );
    h1_ch3 = mkTH1D ( ( char* ) "ch3", ( char* ) "ch3", 4000, 0, 40000 );
    h1_ch4 = mkTH1D ( ( char* ) "ch4", ( char* ) "ch4", 4000, 0, 40000 );
    h1_ch5 = mkTH1D ( ( char* ) "ch5", ( char* ) "ch5", 4000, 0, 40000 );

    h2_r_hitxy = mkTH2F ( ( char* ) "r_hitxy", ( char* ) "r_hitxy", 200, 0, 200, 200, 0, 200 );
    h2_d_hitxy = mkTH2F ( ( char* ) "d_hitxy", ( char* ) "d_hitxy", 200, 0, 200, 200, 0, 200 );
    h2_dssd_hitxy_phys = mkTH2F ( ( char* ) "dssd_hitxy_phys", ( char* ) "dssd_hitxy_phys", 200, 0, 200, 200, 0, 200 );


    h2_dssd_fr_p2 = mkTH2F ( ( char* ) "dssd_fr_p2", ( char* ) "dssd_fr_p2", 5000, 0, 50000, 400, 0, 400 );
    h2_dssd_ba_p2 = mkTH2F ( ( char* ) "dssd_ba_p2", ( char* ) "dssd_ba_p2", 5000, 0, 50000, 400, 0, 400 );
    h2_clr_p2 = mkTH2F ( ( char* ) "clr_p2", ( char* ) "clr_p2", 4000, 0, 40000, 4000, 0, 40000 );

    h2_dssd_fr_p1 = mkTH2F ( ( char* ) "dssd_fr_p1", ( char* ) "dssd_fr_p1", 5000, 0, 50000, 400, 0, 400 );
    h2_dssd_ba_p1 = mkTH2F ( ( char* ) "dssd_ba_p1", ( char* ) "dssd_ba_p1", 5000, 0, 50000, 400, 0, 400 );
    h2_clr_p1 = mkTH2F ( ( char* ) "clr_p1", ( char* ) "clr_p1", 4000, 0, 40000, 4000, 0, 40000 );


//Correlations


    h2_e1t1log = mkTH2F ( ( char* ) "e1t1log", ( char* ) "e1t1log", 6000, -10000, 50000, 200, 0, 20 );
//h2_e1t1c1 = mkTH2F((char *)"e1t1c1",(char *)"e1t1c1",600,-30000,30000,1000,0,1000);
//h2_e1t1c2 = mkTH2F((char *)"e1t1c2",(char *)"e1t1c2",600,-30000,30000,1000,0,1000);
//h2_e1t1c3 = mkTH2F((char *)"e1t1c3",(char *)"e1t1c3",600,-30000,30000,1000,0,1000);
//h2_e1t1c4 = mkTH2F((char *)"e1t1c4",(char *)"e1t1c4",600,-30000,30000,1000,0,1000);


    h2_e2t2log = mkTH2F ( ( char* ) "e2t2log", ( char* ) "e2t2log", 6000, -10000, 50000, 200, 0, 20 );
//h2_e2t2c1 = mkTH2F((char *)"e2t2c1",(char *)"e2t2c1",600,-30000,30000,1000,0,1000);
//h2_e2t2c2 = mkTH2F((char *)"e2t2c2",(char *)"e2t2c2",600,-30000,30000,1000,0,1000);
//h2_e2t2c3 = mkTH2F((char *)"e2t2c3",(char *)"e2t2c3",600,-30000,30000,1000,0,1000);
//h2_e2t2c4 = mkTH2F((char *)"e2t2c4",(char *)"e2t2c4",600,-30000,30000,1000,0,1000);


    h2_e3t3log = mkTH2F ( ( char* ) "e3t3log", ( char* ) "e3t3log", 600, -30000, 30000, 200, 0, 20 );
//h2_e3t3c1 = mkTH2F((char *)"e3t3c1",(char *)"e3t3c1",600,-30000,30000,1000,0,1000);
//h2_e3t3c2 = mkTH2F((char *)"e3t3c2",(char *)"e3t3c2",600,-30000,30000,1000,0,1000);
//h2_e3t3c3 = mkTH2F((char *)"e3t3c3",(char *)"e3t3c3",600,-30000,30000,1000,0,1000);
//h2_e3t3c4 = mkTH2F((char *)"e3t3c4",(char *)"e3t3c4",600,-30000,30000,1000,0,1000);


//<><><><><><><><><><><><>*
//        MAPFILES        *
//<><><><><><><><><><><><>*


    FILE* fmap1;
//char fmapname1[32];//unused

    int strip, phystrip, thr, baseline;
    float off, gain;


    fmap1 = fopen ( "MAP_FILES/dssd_fr_calib_gsfma324.map", "read" );

    if ( fmap1 == 0 )
    {
        printf ( "Failed to read mapfile..." );
    }
    if ( fmap1 != 0 )
    {
        printf ( "Read mapfile ok... " );
    }

    for ( i = 0; i < 161; i++ )
    {

        if ( fscanf ( fmap1, "%d %d %d %f %f %i", &strip, &phystrip, &thr, &off, &gain, &baseline ) ) {}

        map_fr[strip].thr = thr;
        map_fr[strip].phystrip = phystrip;
        map_fr[strip].off = off;
        map_fr[strip].gain = gain;
        map_fr[strip].baseline = baseline;

        printf ( "map front strip %d (phys strip %d) has thr %d, offset %f and gain %f and baseline %i\n", strip, phystrip, thr, off, gain, baseline );

    }

    fclose ( fmap1 );


    fmap1 = fopen ( "MAP_FILES/dssd_ba_calib_gsfma324.map", "read" );


    for ( i = 0; i < 161; i++ )
    {

        if ( fscanf ( fmap1, "%d %d %d %f %f %i", &strip, &phystrip, &thr, &off, &gain, &baseline ) ) {}

        map_ba[strip].thr = thr;
        map_ba[strip].phystrip = phystrip;
        map_ba[strip].off = off;
        map_ba[strip].gain = gain;
        map_ba[strip].baseline = baseline;

        printf ( "map back strip %d (phys strip %d) has thr %d, offset %f and gain %f and baseline %i\n", strip, phystrip, thr, off, gain, baseline );

    }

    fclose ( fmap1 );

// CLOVER MAP FILE

#if(0)
    fmap1 = fopen ( "MAP_FILES/clovers.map", "read" );

    for ( i = 0; i < 20; i++ )
    {

        fscanf ( fmap1, "%d %f %f", &strip, &off, &gain );
        clmap[strip].gain = gain;
        clmap[strip].off = off;

        printf ( "Clover map channel %d has offset %f and gain %f \n", strip, off, gain );

    }

    fclose ( fmap1 );
#endif

    pars.wlist = gDirectory->GetList();
    pars.wlist->Print();

#endif

    return ( 0 );
}

/* ----------------------------------------------------------------- */

/* ----------------------------------------------------------------- */

int DFMAProcessor::DFMAEvDecompose_v3 ( unsigned int* ev, int len, DFMAEVENT* thedfmaEvt )
{
    /* firmware circa Sept 2014 */

    /* declarations */
    //int il;//unused
    int i, k;
    unsigned int ui0 = 0, ui1 = 0, ui2 = 0;
    unsigned int PRE_RISE_SUM = 0, POST_RISE_SUM = 0;
    int rawE;
    unsigned int t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    unsigned long long int ulli1;


    if ( pars.CurEvNo <= pars.NumToPrint )
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

    if ( pars.CurEvNo <= pars.NumToPrint )
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

    if ( pars.CurEvNo <= pars.NumToPrint )
    {
        printf ( "chan_id = %i, board_id=%i, id=%i\n", thedfmaEvt->chan_id, thedfmaEvt->board_id, thedfmaEvt->id );
    }

    /* extract the energy */

    PRE_RISE_SUM = * ( ev + 7 ) & 0x00ffffff;

    if ( pars.CurEvNo <= pars.NumToPrint )
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

    if ( pars.CurEvNo <= pars.NumToPrint )
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

    if ( pars.CurEvNo <= pars.NumToPrint )
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

    if ( pars.CurEvNo <= pars.NumToPrint )
    {
        printf ( "exit DGSEvDecompose_v3:\n" );
    }

    return ( 0 );

}


//**********************************************************


int DFMAProcessor::BinDFMA ( GEB_EVENT* gebEvt )
{

    if ( pars.CurEvNo <= 100 ) //pars.NumToPrint){
    {
        printf ( "entered BinDFMA:\n" );
    }

#if(1)

    //if(pars.CurEvNo > 0){
    //if(((dfmaEvt[0].LEDts - t_first)/1E8)>30){
    if ( pars.CurEvNo > 0 )
    {


        // printf("\nTest1, event number %i\n",pars.CurEvNo);

        float cal_e;

        char strg[128];
        //int ii;//unused
        int i, j, jj;
        int ndssd;
        int ndfma;
        int nfp;
        int nsubev;
        //int trn[100];//unused

        //for(i=0;i<100;i++){
        // trn[i] = 0;
        //}

        int GebTypeStr ( int type, char strg[] );


        if ( pars.CurEvNo <= pars.NumToPrint )
        {
            printf ( "entered BinDFMA:\n" );
        }

        //*********************************************************************
        // loop through the coincidence event and fish out GEB_TYPE_DFMA data *
        //*********************************************************************

        ndfma = 0;
        ndssd = 0;
        nsubev = 0;
        nfp = 0;

        for ( i = 0; i < gebEvt->mult; i++ )
        {

            GebTypeStr ( gebEvt->ptgd[i]->type, strg );

            if ( gebEvt->ptgd[i]->type == 16 )
            {

                if ( pars.CurEvNo <= pars.NumToPrint )
                {
                    printf ( "BinDFMA, %2i> %2i, %s, TS=%lli\n", i, gebEvt->ptgd[i]->type, strg, gebEvt->ptgd[i]->timestamp );
                }

                DFMAEvDecompose_v3 ( ( unsigned int* ) gebEvt->ptinp[i], gebEvt->ptgd[i]->length / sizeof ( unsigned int ),
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
            }

            //nsubev++;

        }

        if ( !pars.noHists ) h1_ndssd->Fill ( ndssd );

        if ( dfmaEvt[0].LEDts != 0 )
        {
            numDFMA++;
        }
        if ( dgsEvt[0].event_timestamp != 0 )
        {
            numDGS++;
        }



        if ( first && ( numDFMA > 1000 ) && ( numDGS > 1000 ) )
        {
            t_first = dfmaEvt[0].LEDts;
            first = false;
        }
        if ( pars.CurEvNo % 10000 == 0 )
        {
            printf ( "Processing event number %i with timestamp %llu. Time elapsed: %i seconds\n", pars.CurEvNo, dfmaEvt[0].LEDts, int ( float ( dfmaEvt[0].LEDts - t_first ) / 1E8 ) );
        }


        if ( firstge )
        {
            t_firstge = dgsEvt[0].event_timestamp;
            firstge = false;
        }

//long long int tmp1, tmp2;//unused

// Declarations for variables


        // NEW STUFF HERE>>>>>>>>>>
//int l_num_bkgd, l_num_sig;//unused
//l_num_bkgd = 95;//unused
//l_num_sig = 400;//unused
//float l_bkgdsum, l_signal, l_avgbkgd;//unused
//l_bkgdsum = 0.0;//unused
//l_signal = 0.0;//unused
//l_avgbkgd = 0.0;//unused
        float cl_int;
        cl_int = 0;

//int r_num_bkgd, r_num_sig; //unused
//r_num_bkgd = 95;//unused
//r_num_sig = 350;//unused
//float r_bkgdsum, r_signal, r_avgbkgd;//unused
//r_bkgdsum = 0.0;//unused
//r_signal = 0.0;//unused
//r_avgbkgd = 0.0;//unused
        float cr_int;
        cr_int = 0;

        float crat_int, cratn_int;
        crat_int = 0.0;
        cratn_int = 0.0;

//>>>>>>>>>>>>>>>>>>>>>>>>>

        float cl, cr, esi, crat, cratn;

        float dssd_fr_emax;
        float dssd_ba_emax;
        dssd_fr_emax = 0.0;
        dssd_ba_emax = 0.0;
        int dssd_fr_subev;
        int dssd_ba_subev;
        dssd_fr_subev = 100000;
        dssd_ba_subev = 100000;

        cl = 0;
        cr = 0;
        esi = 0;
        crat = 0;
        cratn = 0;

        int gate = 0;

//<><><><><><><><><><><><><><><><><>*
// Dig out DSSD event and calibrate *
//<><><><><><><><><><><><><><><><><>*


        for ( i = 0; i < nsubev; i++ )
        {

            if ( dfmaEvt[i].tpe == DSSD )
            {

                cal_e = .0;
                //cal_e = dfmaEvt[i].ehi;

                if ( !pars.noHists ) h2_dssd_en_raw->Fill ( dfmaEvt[i].ehi, dfmaEvt[i].tid );

                // FRONT - selecting maximum energy strip and calibrating

                if ( dfmaEvt[i].tid < BATIDLOW )
                {
                    cal_e = double ( map_fr[dfmaEvt[i].tid].gain ) * ( dfmaEvt[i].ehi + double ( rand() ) / RAND_MAX - 0.5 ) + double ( map_fr[dfmaEvt[i].tid].off );

                    if ( cal_e > dssd_fr_emax )
                    {
                        dssd_fr_emax = cal_e;
                        dssd_fr_subev = i;
                    }
                }

                // BACK - selecting maximum energy strip and calibrating

                if ( dfmaEvt[i].tid > FRTIDHIGH )
                {
                    cal_e = double ( map_ba[dfmaEvt[i].tid].gain ) * ( dfmaEvt[i].ehi + double ( rand() ) / RAND_MAX - 0.5 ) + double ( map_ba[dfmaEvt[i].tid].off );

                    if ( cal_e > dssd_ba_emax )
                    {
                        dssd_ba_emax = cal_e;
                        dssd_ba_subev = i;
                    }
                }

                // UNCOMMENT THIS IF YOU WANT CALIBRATED ENERGIES PROPAGATED THROUGH REMAINDER OF CODE
                dfmaEvt[i].ehi = cal_e;


                if ( !pars.noHists ) h2_dssd_en->Fill ( dfmaEvt[i].ehi, dfmaEvt[i].tid );
                if ( !pars.noHists ) h2_dssd_rate->Fill ( ( dfmaEvt[i].LEDts - t_first ) / 1E8, dfmaEvt[i].tid );

            }
        }

//<><><><><><><><><><><><><><><><><><><><><><><*
// Time difference between DSSD front and back *
//<><><><><><><><><><><><><><><><><><><><><><><*

        signed long long int tdssd_fr;
        signed long long int tdssd_ba;

        tdssd_fr = 0;
        tdssd_ba = 0;

        double frba_t;
        frba_t = 0.0;

        if ( dssd_fr_emax != 0 && dssd_ba_emax != 0 )
        {

            frba_t = double ( dfmaEvt[dssd_fr_subev].LEDts ) - double ( dfmaEvt[dssd_ba_subev].LEDts );
            if ( !pars.noHists ) h2_frba_t->Fill ( frba_t, dfmaEvt[dssd_fr_subev].tid );
            if ( !pars.noHists ) h2_frba_t->Fill ( frba_t, dfmaEvt[dssd_ba_subev].tid );

            tdssd_fr = dfmaEvt[dssd_fr_subev].LEDts;
            tdssd_ba = dfmaEvt[dssd_ba_subev].LEDts;
        }

//<><><><><><><><><><><><><>*
// Dig out focal plane data *
//<><><><><><><><><><><><><>*

        signed long long int tdssdmcp_fr;
        signed long long int tdssdmcp_ba;
        signed long long int tdssdmcp_fr_r;
        signed long long int tdssdmcp_ba_r;

        tdssdmcp_fr = 0;
        tdssdmcp_ba = 0;
        tdssdmcp_fr_r = 0;
        tdssdmcp_ba_r = 0;

        int left_subev = 0;
        int right_subev = 0;

//int letrace[MCPTRLEN+1];//unused
//int let0 = 0;//unused
//int let1 = 0;//unused

#if(1)

        for ( i = 0; i < nsubev; i++ )
        {
            switch ( dfmaEvt[i].tpe )
            {
                //-------------*
                // Focal Plane *
                //-------------*

            case FP:
                if ( !pars.noHists )
                {
                    if ( dfmaEvt[i].tid == 1 )
                    {
                        h1_ch0->Fill ( dfmaEvt[i].ehi );
                    }
                    if ( dfmaEvt[i].tid == 2 )
                    {
                        h1_ch1->Fill ( dfmaEvt[i].ehi );
                    }
                    if ( dfmaEvt[i].tid == 3 )
                    {
                        h1_ch2->Fill ( dfmaEvt[i].ehi );
                    }
                    if ( dfmaEvt[i].tid == 4 )
                    {
                        h1_ch3->Fill ( dfmaEvt[i].ehi );
                    }
                    if ( dfmaEvt[i].tid == 5 )
                    {
                        h1_ch4->Fill ( dfmaEvt[i].ehi );
                    }
                    if ( dfmaEvt[i].tid == 6 )
                    {
                        h1_ch5->Fill ( dfmaEvt[i].ehi );
                    }
                }
                if ( dfmaEvt[i].tid == SICH && dfmaEvt[i].ehi > 0 )
                {
                    gate = 1;
                }


                // MWPC LEFT
                if ( dfmaEvt[i].tid == LCH && dfmaEvt[i].ehi > 0 )
                {

                    left_subev = i;

                    if ( !pars.noHists ) h2_FP_rate->Fill ( ( dfmaEvt[i].LEDts - t_first ) / 1E8, dfmaEvt[i].tid );
                    if ( !pars.noHists  && ( dfmaEvt[i].LEDts - t_first ) / 1E5 < 100000 )
                    {
                        h1_FP_rate_left->Fill ( ( dfmaEvt[i].LEDts - t_first ) / 1E5 );
                    }
                    tdssdmcp_fr = ( dfmaEvt[i].LEDts ) - ( tdssd_fr );
                    tdssdmcp_ba = ( dfmaEvt[i].LEDts ) - ( tdssd_ba );

                    if ( !pars.noHists && tdssd_fr != 0 )
                    {
                        h2_tdssdmcp_fr->Fill ( tdssdmcp_fr, dfmaEvt[dssd_fr_subev].tid );
                    }
                    if ( !pars.noHists && tdssd_ba != 0 )
                    {
                        h2_tdssdmcp_ba->Fill ( tdssdmcp_ba, dfmaEvt[dssd_ba_subev].tid - 160 );
                    }


                    cl = dfmaEvt[i].ehi;

                    /*


                    if (trn1<1000) {
                                for (jj=0;jj<1000;jj++) h2_traceg->Fill(trn1,jj,dfmaEvt[i].trace[jj]);
                                trn1=trn1+1;
                     }


                     // Trace integration >>>>>>>>>>

                                 // Firstly, find trigger point...



                                 for(j=0;j<1000;j++) letrace[j] = 0;
                                 let0 = 0;
                                 let1 = 0;

                                 leFilter(dfmaEvt[i].trace, 1000, 20, letrace, let0, let1, 200);


                                 if(leTr<100){
                                    for(j=0;j<1000;j++) h2_le_output_left->Fill(leTr,j,letrace[j]);
                                    leTr++;
                                 }


                     for(j=5;j<(5+l_num_bkgd);j++){
                        l_bkgdsum = l_bkgdsum + dfmaEvt[i].trace[j];
                     }

                     l_avgbkgd = (l_bkgdsum/l_num_bkgd);

                                 for(j=let0; j<(let0+l_num_sig); j++){
                        l_signal = l_signal + dfmaEvt[i].trace[j];
                     }

                     cl_int = l_signal - (l_avgbkgd*l_num_sig);

                                 */

                }

                // MWPC RIGHT
                if ( dfmaEvt[i].tid == RCH && dfmaEvt[i].ehi > 0 )
                {

                    right_subev = i;

                    if ( !pars.noHists ) h2_FP_rate->Fill ( ( dfmaEvt[i].LEDts - t_first ) / 1E8, dfmaEvt[i].tid );
                    if ( !pars.noHists && ( dfmaEvt[i].LEDts - t_first ) / 1E5 < 100000 )
                    {
                        h1_FP_rate_right->Fill ( ( dfmaEvt[i].LEDts - t_first ) / 1E5 );
                    }

                    tdssdmcp_fr_r = ( dfmaEvt[i].LEDts ) - ( tdssd_fr );
                    tdssdmcp_ba_r = ( dfmaEvt[i].LEDts ) - ( tdssd_ba );

                    if ( !pars.noHists  && tdssd_fr != 0 )
                    {
                        h2_tdssdmcp_fr_r->Fill ( tdssdmcp_fr_r, dfmaEvt[dssd_fr_subev].tid );
                    }
                    if ( !pars.noHists && tdssd_ba != 0 )
                    {
                        h2_tdssdmcp_ba_r->Fill ( tdssdmcp_ba_r, dfmaEvt[dssd_ba_subev].tid - 160 );
                    }

                    cr = dfmaEvt[i].ehi;

                    /*


                    if (trn2<1000) {
                    for (jj=0;jj<1000;jj++) h2_traceg2->Fill(trn2,jj,dfmaEvt[i].trace[jj]);
                    trn2=trn2+1;
                    }


                    // Trace integration>>>>>>>>>>
                    // Firstly, find trigger point...



                    for(j=0;j<1000;j++) letrace[j] = 0;
                    let0 = 0;
                    let1 = 0;

                    leFilter(dfmaEvt[i].trace, 1000, 20, letrace, let0, let1, 200);

                    for(j=5;j<(5+r_num_bkgd);j++){
                         r_bkgdsum = r_bkgdsum + dfmaEvt[i].trace[j];
                      }

                      r_avgbkgd = (r_bkgdsum/r_num_bkgd);

                                  for(j=let0;j<(let0+r_num_sig);j++){
                         r_signal = r_signal + dfmaEvt[i].trace[j];
                      }

                      cr_int = r_signal - (r_avgbkgd*r_num_sig);

                                  */

                    //>>>>>>>>>>>>>>>>>>>>>>>>>>

                }

                if ( dfmaEvt[i].tid == SICH && dfmaEvt[i].ehi > 0 )
                {
                    esi = dfmaEvt[i].ehi;
                }
                break;

            default:
                break;
            }
        }

#endif

//<><><><><><><><><><><><><><><><><><>*
//  Make deltaT between DGS and DFMA  *
//<><><><><><><><><><><><><><><><><><>*

        double dTgdssd;
        dTgdssd = 0.0;

        int geone = 1000;

        for ( i = 0; i < ng; i++ )
        {
            if ( dgsEvt[i].tpe == GE && geone == 1000 )
            {
                geone = i;
            }
        }

        int ch40subev = 1000;

        for ( j = 0; j < ng; j++ )
        {
            if ( dgsEvt[j].tid == 40 && dgsEvt[j].tpe == GE && dgsEvt[j].flag == 0 )
            {
                ch40subev = j;
            }
        }

        if ( ch40subev < 1000 )
        {
            for ( i = 0; i < nsubev; i++ )
            {
                if ( dfmaEvt[i].tpe == DSSD )
                {
                    dTgdssd = double ( dgsEvt[ch40subev].event_timestamp ) - double ( dfmaEvt[i].LEDts );
                    if ( !pars.noHists ) h2_dTgdssd_ch40->Fill ( dTgdssd, dfmaEvt[i].tid );
                }
            }
        }

        dTgdssd = 0.0;

        for ( i = 0; i < nsubev; i++ )
        {
            if ( ( dfmaEvt[i].LEDts > 0 ) && ( dfmaEvt[i].tpe == DSSD ) && ( ng > 0 ) )
            {

                dTgdssd = double ( dgsEvt[geone].event_timestamp ) - double ( dfmaEvt[i].LEDts );
                if ( !pars.noHists ) h2_dTgdssd->Fill ( dTgdssd, dfmaEvt[i].tid );

                if ( dTgdssd > 1000 && pars.CurEvNo < 10000 )
                {
                    //printf("\n\nDebugging!");
                    //printf("\nEvent number %i",pars.CurEvNo);
                    //printf("\n dfmaEvt[%i].LEDts: %llu",i,dfmaEvt[i].LEDts);
                    //printf("\n dgsEvt[%i].LEDts: %llu",geone,dgsEvt[geone].event_timestamp);
                }
            }
        }

        dTgdssd = 0.0;

        if ( dssd_fr_emax > 0 )
        {
            for ( i = 0; i < ng; i++ )
            {
                if ( dgsEvt[i].tpe == GE && dgsEvt[i].flag == 0 )
                {
                    dTgdssd = double ( dgsEvt[i].event_timestamp ) - double ( dfmaEvt[dssd_fr_subev].LEDts );
                    if ( !pars.noHists )  h2_dTgdssd_allge->Fill ( dTgdssd, dfmaEvt[dssd_fr_subev].tid );
                }
            }

        }

        double dTgl;
        dTgl = 0.0;
        double dTgr;
        dTgr = 0.0;

        if ( dfmaEvt[left_subev].LEDts > 0 )
        {
            dTgl = double ( dgsEvt[0].event_timestamp ) - double ( dfmaEvt[left_subev].LEDts );
            if ( !pars.noHists ) h1_dTgl->Fill ( dTgl );
        }
        if ( dfmaEvt[right_subev].LEDts > 0 )
        {
            dTgr = double ( dgsEvt[0].event_timestamp ) - double ( dfmaEvt[right_subev].LEDts );
            if ( !pars.noHists ) h1_dTgr->Fill ( dTgr );
        }

//<><><>*
// MWPC *
//<><><>*

        if ( !pars.noHists )
        {
            h1_esi->Fill ( esi );

            if ( cl != 0 )
            {
                if ( TestTr1 < 100 )
                {
                    for ( i = 0; i < 1000; i++ ) h2_lefttraces->Fill ( TestTr1, i, dfmaEvt[left_subev].trace[i] );

                    TestTr1++;
                }
            }

            if ( cr != 0 )
            {
                if ( TestTr4 < 100 )
                {
                    for ( i = 0; i < 1000; i++ ) h2_righttraces->Fill ( TestTr4, i, dfmaEvt[right_subev].trace[i] );

                    TestTr4++;
                }
            }
        }


        if ( cl != 0 || cr != 0 )
        {

            if ( cr > 0 )
            {
                crat = cl / cr * 1000.0;
            }
            if ( ( cl > 0 ) || ( cr > 0 ) )
            {
                cratn = cl / ( cl + cr ) * 1000.0;
            }

            if ( cr_int > 0 )
            {
                crat_int = cl_int / cr_int * 1000.0;    //new
            }
            if ( ( cl_int > 0 ) || ( cr_int > 0 ) )
            {
                cratn_int = cl_int / ( cl_int + cr_int ) * 1000.0;  //new
            }

            if ( !pars.noHists ) h1_cl_int->Fill ( cl_int ); // new
            if ( !pars.noHists ) h1_cr_int->Fill ( cr_int ); // new

            if ( !pars.noHists ) h1_cl->Fill ( cl );
            if ( !pars.noHists ) h1_cr->Fill ( cr );
            if ( !pars.noHists ) h2_clr->Fill ( cl / 10, cr / 10 );
            if ( !pars.noHists ) h2_clr_int->Fill ( cl_int / 50, cr_int / 50 ); //new

            if ( !pars.noHists ) h1_cx->Fill ( crat );

            if ( !pars.noHists ) h1_cxn->Fill ( cratn );

            if ( ndssd > 0 )
            {
                if ( !pars.noHists ) h1_clg->Fill ( cl );
                if ( !pars.noHists ) h1_crg->Fill ( cr );
                if ( !pars.noHists ) h2_clrg->Fill ( cl, cr );

                if ( dssd_fr_emax > 4500 && dssd_ba_emax > 4500 ) // Gating on Alpha energies
                {
                    if ( !pars.noHists ) h2_clrg_en->Fill ( cl, cr );

                    if ( !pars.noHists && map_fr[dfmaEvt[dssd_fr_subev].tid].phystrip >= 20 && map_fr[dfmaEvt[dssd_fr_subev].tid].phystrip < 40 )
                    {
                        h2_clrg_en_g1->Fill ( cl, cr );
                        h1_cx_en_g1->Fill ( cl / cr * 1000.0 );
                        h1_cxn_en_g1->Fill ( cl / ( cl + cr ) * 1000.0 );
                    }

                    if ( pars.noHists && map_fr[dfmaEvt[dssd_fr_subev].tid].phystrip >= 100 && map_fr[dfmaEvt[dssd_fr_subev].tid].phystrip < 120 )
                    {
                        h2_clrg_en_g2->Fill ( cl, cr );
                        h1_cx_en_g2->Fill ( cl / cr * 1000.0 );
                        h1_cxn_en_g2->Fill ( cl / ( cl + cr ) * 1000.0 );
                    }

                    if ( !pars.noHists )
                    {
                        h2_stripfr_cxng->Fill ( map_fr[dfmaEvt[dssd_fr_subev].tid].phystrip, cratn );
                        h2_stripba_cxng->Fill ( map_ba[dfmaEvt[dssd_ba_subev].tid].phystrip, cratn );
                    }

                    if ( !pars.noHists && map_fr[dfmaEvt[dssd_fr_subev].tid].phystrip > 74 && map_fr[dfmaEvt[dssd_fr_subev].tid].phystrip < 86 )
                    {
                        h2_stripba_cxng_frontgated->Fill ( map_ba[dfmaEvt[dssd_ba_subev].tid].phystrip, cratn );
                    }
                }

                if ( !pars.noHists )
                {
                    h2_clrg_int->Fill ( cl_int / 50, cr_int / 50 ); //new

                    h1_cxg->Fill ( crat );
                    h1_cxng->Fill ( cratn );

                    h1_cxg_int->Fill ( crat_int ); //new
                    h1_cxng_int->Fill ( cratn_int ); //new
                }
            }

        }

        // printf("\nTest2, event number %i\n",pars.CurEvNo);


//*********************************
// Now decide if recoil or decay! *
//*********************************

        int r_fr_emax, r_ba_emax;
        int s_r_fr, s_r_ba;
        int r_fr_subev, r_ba_subev;
        int r_fr_PU, r_ba_PU;
        int r_fr_trace_len, r_ba_trace_len;
        int r_fr_d2t0, r_ba_d2t0;
        int r_fr_d2t1, r_ba_d2t1;
        int r_fr_d2t2, r_ba_d2t2;
        int r_fr_trace[1000], r_ba_trace[1000];
        unsigned long long int r_fr_ts, r_ba_ts;

        int d_fr_emax, d_ba_emax;
        int s_d_fr, s_d_ba;
        int s_d_fr_phys, s_d_ba_phys;
        int d_fr_subev, d_ba_subev;
        int d_fr_PU, d_ba_PU;
        int d_fr_trace_len, d_ba_trace_len;
        int d_fr_d2t0, d_ba_d2t0;
        int d_fr_d2t1, d_ba_d2t1;
        int d_fr_d2t2, d_ba_d2t2;
        int d_fr_trace[1000], d_ba_trace[1000];
        unsigned long long int d_fr_ts, d_ba_ts;

        int rec_pu;
        int fdecEv;
        int s_fd_fr;
        int s_fd_ba;

        int fd_fr_emax, fd_ba_emax;
        unsigned long long int fd_fr_ts, fd_fr_ts2;
        int fd_fr_emax2;

        fd_fr_ts = 0;
        fd_fr_ts2 = 0;
        fd_fr_emax2 = 0;
        fd_fr_emax = 0;
        fd_ba_emax = 0;

        r_fr_emax = 0;
        r_ba_emax = 0;
        s_r_fr = 0;
        s_r_ba = 0;
        r_fr_subev = 0;
        r_ba_subev = 0;
        r_fr_PU = 0;
        r_ba_PU = 0;
        r_fr_trace_len = 0;
        r_ba_trace_len = 0;
        r_fr_d2t0 = 0;
        r_ba_d2t0 = 0;
        r_fr_d2t1 = 0;
        r_ba_d2t1 = 0;
        r_fr_d2t2 = 0;
        r_ba_d2t2 = 0;
        for ( i = 0; i < 1000; i++ )
        {
            r_fr_trace[i] = 0;
            r_ba_trace[i] = 0;
            d_fr_trace[i] = 0;
            d_ba_trace[i] = 0;
        }
        r_fr_ts = 0;
        r_ba_ts = 0;

        d_fr_emax = 0;
        d_ba_emax = 0;
        s_d_fr = 0;
        s_d_ba = 0;
        s_d_fr_phys = 0;
        s_d_ba_phys = 0;
        d_fr_subev = 0;
        d_ba_subev = 0;
        d_fr_PU = 0;
        d_ba_PU = 0;
        d_fr_trace_len = 0;
        d_ba_trace_len = 0;
        d_fr_d2t0 = 0;
        d_ba_d2t0 = 0;
        d_fr_d2t1 = 0;
        d_ba_d2t1 = 0;
        d_fr_d2t2 = 0;
        d_ba_d2t2 = 0;
        d_fr_ts = 0;
        d_ba_ts = 0;

        rec_pu = 0;
        fdecEv = 0;
        s_fd_fr = 0;
        s_fd_ba = 0;

#if(1)

        if ( ( dssd_fr_emax != 0 ) && ( dssd_ba_emax != 0 ) )
        {
            //***********************
            // Condition for recoil *
            //***********************

            if ( ( cl != 0 ) || ( cr != 0 ) )
            {
                //if((dssd_fr_emax >= 15000) && (dssd_ba_emax >= 15000)){
                //FRONT
                r_fr_emax = dssd_fr_emax;
                s_r_fr = dfmaEvt[dssd_fr_subev].tid;
                r_fr_ts = dfmaEvt[dssd_fr_subev].LEDts; // Take recoil timestamp from FRONT...
                r_fr_subev = dssd_fr_subev;
                r_fr_PU = dfmaEvt[dssd_fr_subev].pu;
                r_fr_trace_len = dfmaEvt[dssd_fr_subev].traceLen;
                r_fr_d2t0 = dfmaEvt[dssd_fr_subev].d2t0;
                r_fr_d2t1 = dfmaEvt[dssd_fr_subev].d2t1;
                r_fr_d2t2 = dfmaEvt[dssd_fr_subev].d2t2;
                for ( j = 0; j < dfmaEvt[dssd_fr_subev].traceLen; j++ )
                {
                    r_fr_trace[j] = dfmaEvt[dssd_fr_subev].trace[j];
                }

                //BACK
                r_ba_emax = dssd_ba_emax;
                s_r_ba = dfmaEvt[dssd_ba_subev].tid - NUMFR;
                r_ba_ts = dfmaEvt[dssd_ba_subev].LEDts; // Take recoil timestamp from BACK...
                r_ba_subev = dssd_ba_subev;
                r_ba_PU = dfmaEvt[dssd_ba_subev].pu;
                r_ba_trace_len = dfmaEvt[dssd_ba_subev].traceLen;
                r_ba_d2t0 = dfmaEvt[dssd_ba_subev].d2t0;
                r_ba_d2t1 = dfmaEvt[dssd_ba_subev].d2t1;
                r_ba_d2t2 = dfmaEvt[dssd_ba_subev].d2t2;
                for ( j = 0; j < dfmaEvt[dssd_ba_subev].traceLen; j++ )
                {
                    r_ba_trace[j] = dfmaEvt[dssd_ba_subev].trace[j];
                }

                //*****************************
                // DEALING WITH RECOIL PILEUP *
                //*****************************

                //*****************************
                // TWO-SIDED CORRELATIONS...
                //*****************************

                /*

                if ((r_fr_PU > 1)&&(r_ba_PU > 1)) rec_PU = 1;
                if ((rec_PU!=0)&&(s_r_fr!=0)&&(s_r_ba!=0)) {


                // true PU condition
                //if (((curt1s[s_r_fr]-curt1s[s_r_ba+NUMFR])<5)&&((curt1s[s_r_fr]-curt1s[s_r_ba+NUMFR])>-5)&&(curt1s[s_r_fr]>0)&&(curt1s[s_r_ba+NUMFR]>0)) {

                fdecEv = 1;
                              if((r_fr_PU == 3) && (r_ba_PU == 3)) fdecEv = 3;

                              s_fd_fr=s_r_fr;
                              s_fd_ba=s_r_ba;

                              t0 = curt0s[s_r_fr];
                              t1 = curt1s[s_r_fr];
                              t2 = curt2s[s_r_fr];


                              GetPUEn(t1,t0,r_fr_trace,r_fr_trace_len,e0,e1,m,kk,nn,trise);


                              e0 = e0/100;
                              e1 = e1/100;

                              e0 = map_fr[s_r_fr].gain*(e0 + float(rand())/RAND_MAX-0.5) + map_fr[s_r_fr].off;
                              e1 = map_fr[s_r_fr].gain*(e1 + float(rand())/RAND_MAX-0.5) + map_fr[s_r_fr].off;

                              ef0=e0; ef1=e1;

                              r_fr_emax=ef0;
                              fd_fr_emax=ef1;

                              fd_fr_ts=r_fr_ts+curt1s[s_r_fr]-curt0s[s_r_fr];

                              if(fdecEv == 3){

                                 GetPUEn(t2,t1,r_fr_trace,r_fr_trace_len,e1_b,e2,m,kk,nn,trise);

                                 e2 = e2/100;

                                 e2 = map_fr[s_r_fr].gain*(e2 + float(rand())/RAND_MAX-0.5) + map_fr[s_r_fr].off;
                                 ef2 = e2;

                                 fd_fr_emax2 = ef2;
                                 fd_fr_ts2 = r_fr_ts + curt2s[s_r_fr] - curt0s[s_r_fr];


                              }


                              // back energies
                              t0 = curt0s[s_r_ba+NUMFR];
                              t1 = curt1s[s_r_ba+NUMFR];
                              t2 = curt2s[s_r_ba+NUMFR];

                              GetPUEn(t1,t0,r_ba_trace,r_ba_trace_len,e0,e1,m,kk,nn,trise);


                              e0 = e0/10;
                              e1 = e1/10;
                              e0 = map_ba[s_r_ba+NUMFR].gain*(e0 + float(rand())/RAND_MAX-0.5) + map_ba[s_r_ba+NUMFR].off;
                              e1 = map_ba[s_r_ba+NUMFR].gain*(e1 + float(rand())/RAND_MAX-0.5) + map_ba[s_r_ba+NUMFR].off;
                              eb0=e0; eb1=e1;

                              r_ba_emax=eb0;
                              fd_ba_emax=eb1;
                              fd_ba_ts=r_ba_ts+curt1s[s_r_ba+NUMFR]-curt0s[s_r_ba+NUMFR];


                              if(fdecEv == 3){

                                 GetPUEn(t2,t1,r_ba_trace,r_ba_trace_len,e1_b,e2,m,kk,nn,trise);

                                 e2 = e2/100;

                                 e2 = map_ba[s_r_ba+NUMFR].gain*(e2 + float(rand())/RAND_MAX-0.5) + map_ba[s_r_ba+NUMFR].off;
                                 ef2 = e2;

                                 fd_ba_emax2 = ef2;
                                 fd_ba_ts2 = r_ba_ts + curt2s[s_r_ba+NUMFR] - curt0s[s_r_ba+NUMFR];
                              }
                    } */
            }

            //**************************
            // Condition for decays..  *
            //**************************

            if ( ( cl == 0 ) && ( cr == 0 ) )
            {
                //if(dssd_fr_emax < 8000){

                //FRONT
                d_fr_emax = dssd_fr_emax;
                s_d_fr = dfmaEvt[dssd_fr_subev].tid;
                s_d_fr_phys = map_fr[dfmaEvt[dssd_fr_subev].tid].phystrip;
                d_fr_subev = dssd_fr_subev;
                d_fr_ts = dfmaEvt[dssd_fr_subev].LEDts;
                d_fr_PU = dfmaEvt[dssd_fr_subev].pu;
                d_fr_trace_len = dfmaEvt[dssd_fr_subev].traceLen;
                for ( jj = 0; jj < dfmaEvt[dssd_fr_subev].traceLen; jj++ )
                {
                    d_fr_trace[jj] = dfmaEvt[dssd_fr_subev].trace[jj];
                }

                //BACK
                d_ba_emax = dssd_ba_emax;
                s_d_ba = dfmaEvt[dssd_ba_subev].tid - NUMFR;
                s_d_ba_phys = map_ba[dfmaEvt[dssd_ba_subev].tid].phystrip;
                d_ba_ts = dfmaEvt[dssd_ba_subev].LEDts;     // Take decay timestamp from BACK...
                d_ba_subev = dssd_ba_subev;
                d_ba_PU = dfmaEvt[dssd_ba_subev].pu;
                d_fr_trace_len = dfmaEvt[dssd_ba_subev].traceLen;
                for ( jj = 0; jj < dfmaEvt[dssd_ba_subev].traceLen; jj++ )
                {
                    d_ba_trace[jj] = dfmaEvt[dssd_ba_subev].trace[jj];
                }

                //****************************
                // DEALING WITH DECAY PILEUP *
                //****************************

                /*
                if ((d_fr_PU > 1)&&(d_ba_PU > 1))  dec_PU = 1;

                if ((dec_PU!=0)&&(s_d_fr!=0)&&(s_d_ba!=0)) {

                // true PU condition
                if (((curt1s[s_d_fr]-curt1s[s_d_ba+NUMFR])<5)&&((curt1s[s_d_fr]-curt1s[s_d_ba+NUMFR])>-5)&&(curt1s[s_d_fr]>0)&&(curt1s[s_d_ba+NUMFR]>0)) {

                   fdecEv=2;

                   s_fd_fr=s_d_fr;
                   s_fd_ba=s_d_ba;


                   t0 = curt0s[s_d_fr];
                   t1 = curt1s[s_d_fr];
                   GetPUEn(t1,t0,d_fr_trace,d_fr_trace_len,e0,e1,m,kk,n,trise);


                   e0 = e0/100;
                   e1 = e1/100;
                   e0 = map_fr[s_d_fr].gain*(e0 + float(rand())/RAND_MAX-0.5) + map_fr[s_d_fr].off;
                   e1 = map_fr[s_d_fr].gain*(e1 + float(rand())/RAND_MAX-0.5) + map_fr[s_d_fr].off;
                   ef0=e0; ef1=e1;

                   d_fr_emax=ef0;
                   fd_fr_emax=ef1;

                   fd_fr_ts=d_fr_ts+curt1s[s_d_fr]-curt0s[s_d_fr];


                  // back energies


                  t0 = curt0s[s_d_ba+NUMFR];
                  t1 = curt1s[s_d_ba+NUMFR];

                  GetPUEn(t1,t0,d_ba_trace,d_ba_trace_len,e0,e1,m,kk,n,trise);

                  e0 = e0/10;
                  e1 = e1/10;
                  e0 = map_ba[s_d_ba+NUMFR].gain*(e0 + float(rand())/RAND_MAX-0.5) + map_ba[s_d_ba+NUMFR].off;
                  e1 = map_ba[s_d_ba+NUMFR].gain*(e1 + float(rand())/RAND_MAX-0.5) + map_ba[s_d_ba+NUMFR].off;
                  eb0=e0; eb1=e1;

                  d_ba_emax=eb0;
                  fd_ba_emax=eb1;
                  fd_ba_ts=d_ba_ts+curt1s[s_d_ba+NUMFR]-curt0s[s_d_ba+NUMFR];



                }}*/
            }
        }

#endif

//<><><><><><><><><><><><><><><><>*
// DSSD hit patterns and energies *
//<><><><><><><><><><><><><><><><>*

        if ( !pars.noHists )
        {
            if ( s_r_fr != 0 && s_r_ba != 0 )
            {
                h2_r_hitxy->Fill ( s_r_fr, s_r_ba );
            }
            if ( s_d_fr != 0 && s_d_ba != 0 )
            {
                h2_d_hitxy->Fill ( s_d_fr, s_d_ba );
            }

            if ( dssd_fr_emax > 0 && dssd_ba_emax > 0 )
            {
                h2_dssd_hitxy_phys->Fill ( map_fr[dfmaEvt[dssd_fr_subev].tid].phystrip, map_ba[dfmaEvt[dssd_ba_subev].tid].phystrip );

                if ( TestTr3 < 100 )
                {
                    for ( i = 0; i < 1000; i++ )
                    {
                        h2_dssd_traces_fr->Fill ( TestTr3, i, dfmaEvt[dssd_fr_subev].trace[i] );
                        h2_dssd_traces_ba->Fill ( TestTr3, i, dfmaEvt[dssd_ba_subev].trace[i] );
                    }
                    TestTr3++;
                }
            }

            if ( dssd_fr_emax > 0 )
            {
                h2_dssd_fr_emax->Fill ( dssd_fr_emax, dfmaEvt[dssd_fr_subev].tid );
                h2_d_fr_e->Fill ( d_fr_emax, s_d_fr );
                h2_r_fr_e->Fill ( r_fr_emax, s_r_fr );

                if ( ( dfmaEvt[dssd_fr_subev].LEDts - t_first ) / 1E5 < 100000 )
                {
                    h1_dssd_rate_1D->Fill ( ( dfmaEvt[dssd_fr_subev].LEDts - t_first ) / 1E5 );
                    //h1_dssd_rate_1us->Fill((dfmaEvt[dssd_fr_subev].LEDts - t_first)/100);
                    if ( s_d_fr != 0 )
                    {
                        h1_decay_rate->Fill ( ( dfmaEvt[dssd_fr_subev].LEDts - t_first ) / 1E5 );
                    }
                }
            }

            if ( dssd_ba_emax > 0 )
            {
                h2_dssd_ba_emax->Fill ( dssd_ba_emax, dfmaEvt[dssd_ba_subev].tid - NUMFR );
                h2_d_ba_e->Fill ( d_ba_emax, s_d_ba );
                h2_r_ba_e->Fill ( r_ba_emax, s_r_ba );
            }
        }

        /*
        if(pars.CurEvNo > 1925000){
        printf("\n\nEvent number: %i, r_fr_subev: %i, s_r_fr: %i, s_r_ba : %i, ng: %i\n",pars.CurEvNo,r_fr_subev,s_r_fr,s_r_ba,ng);
        printf("status: %i\n",dssd_corr[s_r_fr][s_r_ba].status);
        printf("s_d_fr: %i, s_d_ba: %i\n",s_d_fr,s_d_ba);

        }
        */

#if(1) // comment out correlations code

//<><><><><><><><><><><><><><><><><><><>*
//                      *
// ******** Correlations Code ********* *
//                      *
//<><><><><><><><><><><><><><><><><><><>*

//<><><><><><><><><><>*
// Recoil correlation *
//<><><><><><><><><><>*

        if ( ( s_r_fr != 0 ) && ( s_r_ba != 0 ) )
        {
            //if(pars.CurEvNo > 1925000)printf("EvNum %i Are we in this loop?\n",pars.CurEvNo);

            switch ( dssd_corr[s_r_fr][s_r_ba].status )
            {
            case 0:

                // no recoils so far

            case 1:

                // recoil last

                recoil.pu = dfmaEvt[r_fr_subev].pu;
                recoil.en = r_fr_emax;
                recoil.ts = r_fr_ts;
                recoil.d2t2 = r_fr_d2t2;
                recoil.d2t0 = r_fr_d2t0;
                recoil.d2t1 = r_fr_d2t1;
                //recoil.fdecEv = fdecEv;
                recoil.x = cratn;

                //recoil.nge = ng;
                //for(i=0;i<ng;i++){
                //recoil.geehi[i] = clover.ehi[i];
                //recoil.tge[i] = clover.tge[i];
                //recoil.getid[i] = clover.tid[i];
                //}

                for ( i = 0; i < 1000; i++ )
                {
                    recoil.trace_ba[i] = r_ba_trace[i];
                    recoil.trace_fr[i] = r_fr_trace[i];
                }

#if(WITHDGS)
                // Storing DGS info for this recoil...
                recoil.nge = ng;
                jj = 0;
                for ( i = 0; i < ng; i++ )
                {
                    if ( dgsEvt[i].tpe == GE && jj < MAXNUMGE )
                    {
                        recoil.geehi[jj] = dgsEvt[i].ehi;
                        recoil.tge[jj] = dgsEvt[i].event_timestamp;
                        recoil.getid[jj] = dgsEvt[i].tid;
                        jj++;
                    }
                }
#endif

                dssd_corr[s_r_fr][s_r_ba].status = 1;
                dssd_corr[s_r_fr][s_r_ba].chain.recoil = recoil;
                dssd_corr[s_r_fr][s_r_ba].chain.s_fr = s_r_fr;
                dssd_corr[s_r_fr][s_r_ba].chain.s_ba = s_r_ba;

                // if(pars.CurEvNo > 1925800)printf("\nTest middle event number %i\n",pars.CurEvNo);

                break;

            default: // there is chain already

                // store previous chain in the tree

                chain = dssd_corr[s_r_fr][s_r_ba].chain;
                chain.corr_type = 1;
                tree->Fill();

                // reset the pixel

                recoil.pu = dfmaEvt[r_fr_subev].pu;
                recoil.en = r_fr_emax;
                recoil.ts = r_fr_ts;
                recoil.d2t2 = r_fr_d2t2;
                recoil.d2t0 = r_fr_d2t0;
                recoil.d2t1 = r_fr_d2t1;
                recoil.x = cratn;
                //recoil.fdecEv = fdecEv;

#if(WITHDGS)
                // Storing DGS info for this recoil...
                recoil.nge = ng;
                jj = 0;
                for ( i = 0; i < ng; i++ )
                {
                    if ( dgsEvt[i].tpe == GE && jj < MAXNUMGE )
                    {
                        recoil.geehi[jj] = dgsEvt[i].ehi;
                        recoil.tge[jj] = dgsEvt[i].event_timestamp;
                        recoil.getid[jj] = dgsEvt[i].tid;
                        jj++;
                    }
                }
#endif

                for ( i = 0; i < 1000; i++ )
                {
                    recoil.trace_ba[i] = r_ba_trace[i];
                    recoil.trace_fr[i] = r_fr_trace[i];
                }

                dssd_corr[s_r_fr][s_r_ba].chain.recoil = recoil;

                // reinitialising decays...
                for ( i = 0; i < dssd_corr[s_r_fr][s_r_ba].chain.ndec; i++ )
                {
                    if ( i < MAXNUMDEC )
                    {
                        dssd_corr[s_r_fr][s_r_ba].chain.decay[i].ts = 0;
                        dssd_corr[s_r_fr][s_r_ba].chain.decay[i].en = 0;
                        dssd_corr[s_r_fr][s_r_ba].chain.decay[i].pu_fr = 0;
                        dssd_corr[s_r_fr][s_r_ba].chain.decay[i].pu_ba = 0;
                        dssd_corr[s_r_fr][s_r_ba].chain.decay[i].time = 0;

                        for ( j = 0; j < dssd_corr[s_r_fr][s_r_ba].chain.decay[i].traceLen; j++ )
                        {
                            dssd_corr[s_r_fr][s_r_ba].chain.decay[i].trace_fr[j] = 0;
                            dssd_corr[s_r_fr][s_r_ba].chain.decay[i].trace_ba[j] = 0;
                        }

                        dssd_corr[s_r_fr][s_r_ba].chain.decay[i].traceLen = 0;

                        dssd_corr[s_r_fr][s_r_ba].chain.decay[i].nge = 0;
                        for ( j = 0; j < MAXNUMGE; j++ )
                        {
                            dssd_corr[s_r_fr][s_r_ba].chain.decay[i].geehi[j] = 0;
                            dssd_corr[s_r_fr][s_r_ba].chain.decay[i].tge[j] = 0;
                            dssd_corr[s_r_fr][s_r_ba].chain.decay[i].getid[j] = 0;
                        }
                    }
                }

                dssd_corr[s_r_fr][s_r_ba].chain.ndec = 0;
                dssd_corr[s_r_fr][s_r_ba].status = 1;
                dssd_corr[s_r_fr][s_r_ba].chain.s_fr = s_r_fr;
                dssd_corr[s_r_fr][s_r_ba].chain.s_ba = s_r_ba;

                break;
            } // switch

        } // f&b

//<><><><><><><><><><>*
// Decay correlations *
//<><><><><><><><><><>*

        int dtmin = 80; // 200;// was 80 //50 for 2us
        int dtmax = 130; // 250;// was 130  // 90 for 2us

#define T1COMP1 1.0
#define T1COMP2 1000.0
#define T1COMP3 1000000.0
#define T1COMP4 100000000.0

#define T2COMP1 1.0
#define T2COMP2 1000.0
#define T2COMP3 1000000.0

#define T3COMP1 1.0
#define T3COMP2 1000.0
#define T3COMP3 1000000.0

#define T1GATE 10000.0
#define T2GATE 100000.0
#define T3GATE 10000.0

        float logtime;
        logtime = .0;
        int decEv = 0;
        unsigned long long int decay0_time;
        decay0_time = 0;

        if ( ( s_d_fr != 0 ) && ( s_d_ba != 0 ) )
        {
            decEv = 1;
        }

        while ( decEv == 1 || fdecEv != 0 )
        {
            if ( decEv == 1 )
            {
                decay.en = d_fr_emax;
                decay.ts = d_fr_ts;

                //decay.decpos = 0;
                decay.d2t0 = d_fr_d2t0;
                decay.d2t1 = d_fr_d2t1;
                decay.d2t2 = d_fr_d2t2;

                // Decay-Decay PU
                decay.pu_fr = d_fr_PU;
                decay.pu_ba = d_ba_PU;
                for ( i = 0; i < 1000; i++ )
                {
                    decay.trace_fr[i] = d_fr_trace[i];
                    decay.trace_ba[i] = d_ba_trace[i];
                }
                decay.nge = ng;
                for ( j = 0; j < ng; j++ )
                {
                    decay.geehi[j] = clover.ehi[j];
                    decay.tge[j] = clover.tge[j];
                    decay.getid[j] = clover.tid[j];
                }

                decEv = 0;
            }

            else
            {
                if ( fdecEv != 0 )
                {
                    s_d_fr = s_fd_fr;
                    s_d_ba = s_fd_ba;

                    decay.en = fd_fr_emax;
                    decay.ts = fd_fr_ts;

                    if ( ( fdecEv == 1 ) || ( fdecEv == 3 ) ) // recoil PU
                    {
                        if ( fdecEv == 1 )
                        {
                            decay.pu_fr = r_fr_PU;
                            decay.pu_ba = r_ba_PU;
                            decay.d2t0 = r_fr_d2t0;
                            decay.d2t1 = r_fr_d2t1;
                            decay.d2t2 = r_fr_d2t2;

                            for ( i = 0; i < 1000; i++ )
                            {
                                decay.trace_fr[i] = r_fr_trace[i];
                                decay.trace_ba[i] = r_ba_trace[i];
                            }
                            decay.nge = ng;
                            for ( j = 0; j < ng; j++ )
                            {
                                decay.geehi[j] = clover.ehi[j];
                                decay.tge[j] = clover.tge[j];
                                decay.getid[j] = clover.tid[j];
                            }

                            fdecEv = 0;
                        }
                        else if ( fdecEv == 3 )
                        {
                            decay.pu_fr = r_fr_PU;
                            decay.pu_ba = r_ba_PU;
                            decay.d2t0 = r_fr_d2t0;
                            decay.d2t1 = r_fr_d2t1;
                            decay.d2t2 = r_fr_d2t2;

                            for ( i = 0; i < 1000; i++ )
                            {
                                decay.trace_fr[i] = r_fr_trace[i];
                                decay.trace_ba[i] = r_ba_trace[i];
                            }
                            decay.nge = ng;
                            for ( j = 0; j < ng; j++ )
                            {
                                decay.geehi[j] = clover.ehi[j];
                                decay.tge[j] = clover.tge[j];
                                decay.getid[j] = clover.tid[j];
                            }
                            fd_fr_emax = fd_fr_emax2;
                            fd_fr_ts = fd_fr_ts2;
                            fdecEv = 1;
                        }
                    }

                    if ( fdecEv == 2 ) // decay PU
                    {
                        //decay.decpos = 1;

                        decay.pu_fr = d_fr_PU;
                        decay.pu_ba = d_ba_PU;
                        decay.d2t0 = d_fr_d2t0;
                        decay.d2t1 = d_fr_d2t1;
                        decay.d2t2 = d_fr_d2t2;

                        for ( i = 0; i < 1000; i++ )
                        {
                            decay.trace_fr[i] = d_fr_trace[i];
                            decay.trace_ba[i] = d_ba_trace[i];
                        }
                        decay.nge = ng;
                        for ( j = 0; j < ng; j++ )
                        {
                            decay.geehi[j] = clover.ehi[j];
                            decay.tge[j] = clover.tge[j];
                            decay.getid[j] = clover.tid[j];
                        }
                        fdecEv = 0;
                    }
                }
            }

            switch ( dssd_corr[s_d_fr][s_d_ba].status )
            {
            case 0:   // No recoil before - do nothing.
                break;
            case 1:   // First decay generation

                decay.time = decay.ts - dssd_corr[s_d_fr][s_d_ba].chain.recoil.ts;
                dssd_corr[s_d_fr][s_d_ba].chain.decay[0] = decay;
                dssd_corr[s_d_fr][s_d_ba].status++;
                dssd_corr[s_d_fr][s_d_ba].chain.ndec = 1;

                if ( decay.time > 0 )
                {
                    logtime = log10 ( 10.0 * decay.time );
                    if ( !pars.noHists ) h2_e1t1log->Fill ( decay.en, logtime );
                }

                if ( !pars.noHists ) h2_x_d_fr_e->Fill ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.x, decay.en );

                if ( logtime < 8 && logtime > 0 )
                {
                    if ( !pars.noHists ) h2_x_d_fr_e_short->Fill ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.x, decay.en );

                    //if ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.nge > 0 )
                    //{
                    //for(j=0;j<dssd_corr[s_d_fr][s_d_ba].chain.recoil.nge;j++) h2_corr_gammas->Fill(decay.en,dssd_corr[s_d_fr][s_d_ba].chain.recoil.geehi[j]);
                    //}
                    if ( !pars.noHists ) h1_short_dec_en->Fill ( decay.en );
                }

                if ( logtime < 8 && logtime > 0 )
                {
                    if ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.nge > 0 )
                    {
                        for ( j = 0; j < dssd_corr[s_d_fr][s_d_ba].chain.recoil.nge; j++ )
                        {
                            dTgdssd = double ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.tge[j] ) - double ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.ts );
                            if ( !pars.noHists && dTgdssd > dtmin && dTgdssd < dtmax )
                            {
                                h2_corr_gammas_short->Fill ( decay.en, dssd_corr[s_d_fr][s_d_ba].chain.recoil.geehi[j] );
                            }
                        }

                        if ( decay.en > 3760 && decay.en < 3920 )
                        {
                            for ( j = 0; j < dssd_corr[s_d_fr][s_d_ba].chain.recoil.nge - 1; j++ )
                            {

                                dTgdssd = double ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.tge[j] ) - double ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.ts );

                                if ( dTgdssd > dtmin && dTgdssd < dtmax )
                                {
                                    for ( int k = j + 1; k < dssd_corr[s_d_fr][s_d_ba].chain.recoil.nge; k++ )
                                    {
                                        dTgdssd = double ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.tge[k] ) - double ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.ts );
                                        if ( !pars.noHists && dTgdssd > dtmin && dTgdssd < dtmax )
                                        {
                                            h2_gamma_gamma_shortdec->Fill ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.geehi[j], dssd_corr[s_d_fr][s_d_ba].chain.recoil.geehi[k] );
                                            h2_gamma_gamma_shortdec->Fill ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.geehi[k], dssd_corr[s_d_fr][s_d_ba].chain.recoil.geehi[j] );
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                break;

            case 2:   // Second decay generation

                decay.time = decay.ts - dssd_corr[s_d_fr][s_d_ba].chain.decay[0].ts;
                decay0_time = dssd_corr[s_d_fr][s_d_ba].chain.decay[0].ts - dssd_corr[s_d_fr][s_d_ba].chain.recoil.ts;
                dssd_corr[s_d_fr][s_d_ba].chain.decay[1] = decay;
                dssd_corr[s_d_fr][s_d_ba].status++;
                dssd_corr[s_d_fr][s_d_ba].chain.ndec = 2;

                if ( decay.time > 0 )
                {
                    logtime = log10 ( 10.0 * decay.time );
                    h2_e2t2log->Fill ( decay.en, logtime );
                }

                //if ( !pars.noHists )
                //{
                //h2_e2t2c1->Fill(decay.en,decay.time/T1COMP1);
                //h2_e2t2c2->Fill(decay.en,decay.time/T1COMP2);
                //h2_e2t2c3->Fill(decay.en,decay.time/T1COMP3);
                //h2_e2t2c4->Fill(decay.en,decay.time/T1COMP4);


                //if(decay.time < corr_time_short && dssd_corr[s_d_fr][s_d_ba].chain.decay[0].time  < corr_time_short)
                //h2_e1e2_short->Fill(decay.en, dssd_corr[s_d_fr][s_d_ba].chain.decay[0].en);
                //if(decay.time < corr_time &&  dssd_corr[s_d_fr][s_d_ba].chain.decay[0].time < corr_time)
                //h2_e1e2->Fill(decay.en, dssd_corr[s_d_fr][s_d_ba].chain.decay[0].en);
                //}

                break;

            case 3:   // Third decay generation

                decay.time = decay.ts - dssd_corr[s_d_fr][s_d_ba].chain.decay[1].ts;
                dssd_corr[s_d_fr][s_d_ba].chain.decay[2] = decay;
                dssd_corr[s_d_fr][s_d_ba].status++;
                dssd_corr[s_d_fr][s_d_ba].chain.ndec = 3;

                if ( decay.time > 0 )
                {
                    logtime = log10 ( 10.0 * decay.time );
                    if ( !pars.noHists ) h2_e3t3log->Fill ( decay.en, logtime );
                }

                //if ( !pars.noHists )
                //{
                //h2_e3t3c1->Fill(decay.en,decay.time/T1COMP1);
                //h2_e3t3c2->Fill(decay.en,decay.time/T1COMP2);
                //h2_e3t3c3->Fill(decay.en,decay.time/T1COMP3);
                //h2_e3t3c4->Fill(decay.en,decay.time/T1COMP4);

                //if(decay.time < corr_time_short && dssd_corr[s_d_fr][s_d_ba].chain.decay[1].time < corr_time_short)
                // h2_e2e3_short->Fill(decay.en, dssd_corr[s_d_fr][s_d_ba].chain.decay[1].en);
                //if(decay.time < corr_time &&  dssd_corr[s_d_fr][s_d_ba].chain.decay[1].time < corr_time)
                // h2_e2e3->Fill(decay.en, dssd_corr[s_d_fr][s_d_ba].chain.decay[1].en);
                //}

                break;

            case 4:   // Fourth decay generation
                dssd_corr[s_d_fr][s_d_ba].status++;
                dssd_corr[s_d_fr][s_d_ba].chain.ndec = 4;

                /*
                decay.time = decay.ts - dssd_corr[s_d_fr][s_d_ba].chain.decay[2].ts;
                dssd_corr[s_d_fr][s_d_ba].chain.decay[3] = decay;
                if (decay.time>0){
                   logtime=log10(10.0*decay.time);
                   if ( !pars.noHists ) h2_e4t4log->Fill(decay.en,logtime);
                }

                if ( !pars.noHists )
                {
                              h2_e4t4c1->Fill(decay.en,decay.time/T1COMP1);
                              h2_e4t4c2->Fill(decay.en,decay.time/T1COMP2);
                              h2_e4t4c3->Fill(decay.en,decay.time/T1COMP3);
                              h2_e4t4c4->Fill(decay.en,decay.time/T1COMP4);
                    }
                              */

                break;

            case 5:   // Fifth decay generation
                dssd_corr[s_d_fr][s_d_ba].status++;
                dssd_corr[s_d_fr][s_d_ba].chain.ndec = 5;
                break;

            default:
                dssd_corr[s_d_fr][s_d_ba].status++;
                dssd_corr[s_d_fr][s_d_ba].chain.ndec = 6;
                break;
            }
        } //end while

#endif

//<><><><><><><><>*
// Singles gammas *
//<><><><><><><><>*

        // printf("\nTest4, event number %i\n",pars.CurEvNo);

#if(1)

        if ( !pars.noHists )
        {
            for ( i = 0; i < ng; i++ )
            {
                if ( ( dgsEvt[i].tpe == GE ) && ( dgsEvt[i].flag == 0 ) ) // Check for clean GEs
                {
                    h2_all_gammas->Fill ( dgsEvt[i].ehi, dgsEvt[i].tid );

                    if ( ( dgsEvt[i].event_timestamp - t_first ) / 1E5 < 100000 )
                    {
                        h1_GE_rate->Fill ( ( dgsEvt[i].event_timestamp - t_first ) / 1E5 );
                    }
                }
            }
        }

#endif


//<><><><><><><><><><><><><>*
// Recoil-correlated gammas *
//<><><><><><><><><><><><><>*

#if(1)

//printf("\n\n1) Event number %i has s_r_fr %i, s_r_ba %i, ng %i",pars.CurEvNo,s_r_fr,s_r_ba,ng);
//printf("\nndssd: %i, cl: %f, cr: %f\n",ndssd,cl,cr);
        if ( !pars.noHists )
        {
            if ( s_r_fr != 0 && s_r_ba != 0 )
            {
                for ( i = 0; i < ng; i++ )
                {
                    if ( ( dgsEvt[i].tpe == GE ) && ( dgsEvt[i].flag == 0 ) ) // Check for clean GEs
                    {
                        h2_rec_gammas->Fill ( dgsEvt[i].ehi, dgsEvt[i].tid );
                        h2_xehi->Fill ( cratn, dgsEvt[i].ehi );

                        if ( dTgdssd > dtmin && dTgdssd < dtmax )
                        {
                            h2_xehig->Fill ( cratn, dgsEvt[i].ehi );
                        }
                    }
                }
            }

            if ( ndssd > 0 )
            {
                for ( i = 0; i < ng; i++ )
                {
                    if ( ( dgsEvt[i].tpe == GE ) && ( dgsEvt[i].flag == 0 ) ) // Check for clean GEs
                    {
                        h2_dssd_gammas->Fill ( dgsEvt[i].ehi, dgsEvt[i].tid );
                    }
                }
            }

            if ( cl != 0 || cr != 0 )
            {
                for ( i = 0; i < ng; i++ )
                {
                    if ( ( dgsEvt[i].tpe == GE ) && ( dgsEvt[i].flag == 0 ) ) // Check for clean GEs
                    {
                        h2_mcp_gammas->Fill ( dgsEvt[i].ehi, dgsEvt[i].tid );
                    }
                }
            }
        }

#endif

//<><><><><><><><><>*
// Print statements *
//<><><><><><><><><>*

        if ( pars.CurEvNo <= pars.NumToPrint )
        {
            printf ( "Print statements at end of BinDFMA\n" );

            for ( i = 0; i < 1; i++ )
            {
                printf ( "\n\n\nwe have %i gamma rays\n", ng );
                printf ( "%2i> ", i );
                printf ( "chan_id=%i; ", dgsEvt[i].chan_id );
                printf ( "board_id=%i; ", dgsEvt[i].board_id );
                printf ( "id =%i; ", dgsEvt[i].id );
                printf ( "tpe=%i; ", dgsEvt[i].tpe );
                printf ( "tid=%i; ", dgsEvt[i].tid );
                printf ( "EventTS=%llu; ", dgsEvt[i].event_timestamp );
                printf ( "ehi=%f ", dgsEvt[i].ehi );
                printf ( "\n\n\n" );
            }
        }

        // debug list the dssd events we found

        if ( pars.CurEvNo <= pars.NumToPrint )
            for ( i = 0; i < ndssd; i++ )
            {
                printf ( "we have %i DSSD event(s)\n", ndssd );
                printf ( "%2i> ", i );
                printf ( "chan_id=%i; ", dfmaEvt[i].chan_id );
                printf ( "board_id=%i; ", dfmaEvt[i].board_id );
                printf ( "id =%i; ", dfmaEvt[i].id );
                printf ( "tpe=%i; ", dfmaEvt[i].tpe );
                printf ( "tid=%i; ", dfmaEvt[i].tid );
                printf ( "LEDTS=%llu; ", dfmaEvt[i].LEDts );
                printf ( "ehi=%8i ", dfmaEvt[i].ehi );
                printf ( "\n\n\n\n" );
                fflush ( stdout );
            };

        // done

        if ( pars.CurEvNo <= pars.NumToPrint )
        {
            printf ( "exit BinDFMA\n" );
        }

        return ( 0 );
    }

#endif

    return ( 0 );
}

/**************************/
/*TRACE ANALYSIS FUNCTIONS*/
/**************************/

// smooth3ch filter
void smoothFilter3ch ( short int trace[], int trLen, short int smoothTrace2[] )
{
    int i;

    for ( i=5; i<trLen; i++ )
    {
        smoothTrace2[i]= ( trace[i-1] + 2*trace[i] + trace[i+1] ) /4;
    }
}

// smooth5ch filter
void smoothFilter5ch ( short int trace[], int trLen, short int smoothTrace2[] )
{
    int i;

    for ( i=5; i<trLen; i++ )
    {
        smoothTrace2[i]= ( trace[i-2] + 2*trace[i-1] + 4*trace[i] + 2*trace[i+1] + trace[i+2] ) /10;
    }
}

// smooth7ch filter
void smoothFilter7ch ( short int trace[], int trLen, short int smoothTrace2[] )
{
    int i;

    for ( i=5; i<trLen; i++ )
    {
        smoothTrace2[i]= ( trace[i-3] + 2*trace[i-2] + 4*trace[i-1] + 8*trace[i] + 4*trace[i+1] +2*trace[i+2] + trace[i+3] ) /22;
    }
}

// D filter
void dFilter ( short int trace[], int trLen, int dTrace[] )
{
    int i;

    for ( i=1; i<trLen; i++ )
    {
        dTrace[i]=trace[i]-trace[i-1];
    }
}

// D filter
void dFilterNEW ( short int trace[], int trLen, short int dTrace[] )
{
    int i;

    for ( i=1; i<trLen; i++ )
    {
        dTrace[i]=trace[i]-trace[i-5];
    }
}

void averaging ( short int trace[], int trLen, short int dTrace[] )
{
    int i;

    for ( i=1; i<trLen; i++ )
    {
        dTrace[i] = ( trace[i-2] + trace[i-1] + trace[i] + trace[i+1] + trace[i+2] ) /5;
    }
}

// DD filter
void ddFilter ( short int trace[], int trLen, int ddTrace[] )
{
    int i;

    for ( i=10; i<trLen-4; i++ )
    {
        ddTrace[i]=trace[i]-2*trace[i-1]+trace[i-2];
    }

}

// DD filter small signals
void ddFilterNEW ( short int trace[], int trLen, short int ddTrace[] )
{
    int i;

    for ( i=10; i<trLen-4; i++ )
    {
        ddTrace[i]=trace[i]-2*trace[i-5]+trace[i-10];
    }

}

// LE filter
void leFilter ( short int trace[], int trLen, int k, int leTrace[], int& t0, int& t1, int leThr )
{
    int i;
    t0 = 0;
    t1 = 0;

    for ( i=k; i<trLen; i++ )
    {
        leTrace[i]=trace[i]-trace[i-k];
        //if(i<100)printf("trace[%i]: %i, trace[%i]: %i, leTrace[%i]: %i\n",i,trace[i],i-k,trace[i-k],leTrace[i]);

        if ( ( leTrace[i]>leThr ) && ( leTrace[i-1]<leThr ) && ( t0>0 ) && ( t1==0 ) )
        {
            t1 = i;
        }
        if ( ( leTrace[i]>leThr ) && ( leTrace[i-1]< leThr ) && ( t0==0 ) )
        {
            t0 = i;
        }
    }
}

// trigger 1 on trace
void trigTrace ( int trace[], int trLen, int trStart, int trStop, int thr, int &t0 )
{
    int i;

    int safetyTrace = 0;
    if ( trStop < trLen ) safetyTrace = trStop;
    else safetyTrace = trLen;

    for ( i=trStart; i<safetyTrace; i++ )
    {
        if ( ( trace[i]>=thr ) && ( trace[i+1]<thr ) && ( t0==0 ) )
        {
            t0=i;
            break;
        }
    }

}


void negd2 ( int trace[], int trLen, int trStart, int trStop, int thr, int &numtrigs )
{
    int safetyTrace = 0;
    if ( trStop < trLen ) safetyTrace = trStop;
    else safetyTrace = trLen;

    int i;

    for ( i=trStart; i<safetyTrace; i++ )
    {
        if ( ( trace[i]>=thr ) && ( trace[i+1] < thr ) ) numtrigs++;
    }
}

// Trapezoidal filter
void trapFilter ( short int trace[], int trLen, int m, int k, int trapTrace[] )
{
    int i;
    for ( i=m+k+m; i<trLen; i++ ) trapTrace[i] = trapTrace[i-1] + ( trace[i]-trace[i-m] )- ( trace[i-m-k]-trace[i-m-k-m] );
}

// extract energies from PU traces
void GetPUEn ( int t1, int t0, int trace[], int trlen, float &e0, float &e1, int m, int k, int n, float trise )
{
    float ebl;
    float dt;
    int iw0, iw1;
    int i;

    if ( t1-t0>k ) // time separation longer than k
    {
        // integration windows
        if ( t1-t0-k>m ) iw0=m;
        else iw0=t1-t0-k; // 11-11 correction
        //if (iw0<0) iw0=1;
        if ( trlen-t1-k>m ) iw1=m;
        else iw1=trlen-t1-k; //11-11 correction
        //if (iw1<0) iw1=1;
        //if (iw1>iw0) iw1=iw0;

        // front energies
        e0=0;
        e1=0;
        for ( i=t0+k; i<t0+k+iw0; i++ )
        {
            e0=e0+trace[i]-trace[i-iw0-k];
        }
        for ( i=t1+k; i<t1+iw1+k; i++ )
        {
            e1=e1+trace[i]-trace[i-k-t1+t0-iw1];
        }

        e0=e0*float ( m ) /float ( iw0 ); // gain correction // 11-11 correction
        e1=e1*float ( m ) /float ( iw1 ); //  gain correction // 11-11 corrrection
        e1=e1-e0;
    }

    else     // t1-t0 < k
    {
        if ( t1-t0<=trise )
        {
            e0=0;
            e1=0;
            dt=t1-t0;
            // integration of the begining of 1st signal
            for ( i=t0; i<t1; i++ )
            {
                e0=e0+trace[i];
            }
            //printf("integral %f\n", e0);
            ebl=0;
            // base line inegration
            for ( i=t0-m; i<t0; i++ )
            {
                ebl=ebl+trace[i];
            }
            e0=e0-ebl*dt/float ( m ); // energy of 1st signal
            // integration of 1st and 2nd signal
            for ( i=t1+k; i<t1+m+k; i++ )
            {
                e1=e1+trace[i];
            }

            e1=e1-ebl; // energy sum of 1st and 2nd signal
            e0=2.0*trise*float ( m ) *e0/dt/dt; // energy of 1st signal
            e1=e1-e0; // enery of 2nd signal
        }

        else
        {
            e0=0;
            e1=0;
            dt=t1-t0-trise;
            // integration of the begining of 1st signal
            for ( i=t0+trise; i<t1; i++ )
            {
                e0=e0+trace[i];
            }
            //printf("integral %f\n", e0);
            ebl=0;
            // base line inegration
            for ( i=t0-m; i<t0; i++ )
            {
                ebl=ebl+trace[i];
            }
            e0=e0-ebl*dt/float ( m ); // energy of 1st signal
            // integration of 1st and 2nd signal
            for ( i=t1+k; i<t1+m+k; i++ )
            {
                e1=e1+trace[i];
            }

            e1=e1-ebl; // energy sum of 1st and 2nd signal
            e0=float ( m ) *e0/dt; // energy of 1st signal
            e1=e1-e0; // enery of 2nd signal
        }
    }
}
