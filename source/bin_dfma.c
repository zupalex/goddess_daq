#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TH1.h"
#include "TH2.h"
#include "TObjArray.h"
#include "TObject.h"
#include "TKey.h"
#include "TSystem.h"
#include "TCutG.h"
#include "TTree.h"
#include "gdecomp.h"
#include "limits.h"

#include "GEBSort.h"
#include "GTMerge.h"
#include "functions_dfma.h"

#include "ProcessManagers.h"

#define LEFT 1
#define RIGHT 2
#define ICDE 3
#define PPACDE 4
#define PPACDE2 5

#define DSSDTRLEN 10
#define MCPTRLEN 10

#define SICH 5
#define RCH  2
#define LCH  1

#define EnergyFromTrace 0
#define SmoothOrNot 1
#define WITHDGS 1

// Change DSSD tid info here->

#define FRTIDLOW 1  //1
#define FRTIDHIGH 160   //160
#define BATIDLOW 161    //161
#define BATIDHIGH 320   //320
#define NUMFR 160   //160
#define NUMBA 160   //160
#define NUMSTRIPS 320   //320

/* Doppler correction and Ge calibrations */

#define NGSGE 110
#define MAXNUMGE 15
#define MAXNUMDEC 4

struct strip_type
{
    int phystrip;
    int thr;
    float off;
    float gain;
    int baseline;
};

struct strip_type map_fr[NUMSTRIPS + 1];
struct strip_type map_ba[NUMSTRIPS + 1];


struct clover_map
{

    float gain;
    float off;

};

struct clover_map clmap[21];

struct recoil_pu_type
{

    short int trace[MAXTRACELEN];
    unsigned short int traceLen;
    int s_fr;
    int s_ba;
    unsigned long long int ts;

};



struct recoil_type
{

    unsigned long long int ts;
    int en;
    int pu;
    float left;
    float right;
    double x;
    int d2t0;
    int d2t1;
    int d2t2;
    int nge;
    long long int geehi[MAXNUMGE];
    unsigned long long int tge[MAXNUMGE];
    int getid[MAXNUMGE];
    int traceLen;
    short int trace_fr[1000];
    short int trace_ba[1000];

};

struct decay_type_new
{

    unsigned long long int ts;
    int en;
    int pu_fr;
    int pu_ba;
    unsigned long long int time;
    //unsigned short int traceLen;
    //int dssdbase;
    //short int trace[MAXTRACELEN];
    int traceLen;
    //short int trace[MAXTRACELEN];
    short int trace_fr[1000];
    short int trace_ba[1000];
    int nge;
    long long int geehi[MAXNUMGE];
    unsigned long long int tge[MAXNUMGE];
    int getid[MAXNUMGE];
    int fdecEv;
    int d2t0;
    int d2t1;
    int s_ba;
    int s_fr;
};

struct decay_type
{

    unsigned long long int ts;
    int en;
    int pu_fr;
    int pu_ba;
    unsigned long long int time;
    int traceLen;
    short int trace_fr[1000];
    short int trace_ba[1000];
    int nge;
    long long int geehi[MAXNUMGE];
    unsigned long long int tge[MAXNUMGE];
    int getid[MAXNUMGE];
    int d2t0;
    int d2t1;
    int d2t2;
};



struct focal_plane
{

    int left;
    int right;
    int icde;
    int ppacde;
    int ppacde2;
    unsigned long long int left_ts;
    unsigned long long int right_ts;
    unsigned long long int icde_ts;
    unsigned long long int ppacde_ts;
    unsigned long long int ppacde2_ts;


};



struct chain_type
{

    int s_fr;
    int s_ba;
    recoil_type recoil;
    int ndec;
    decay_type decay[4];//*4thGen
    int corr_type;//*26th Apr

};

struct pixel_type
{

    int status;
    chain_type chain;

};


struct clover_type
{

    int nge;
    int tid[MAXNUMGE];
    long long int ehi[MAXNUMGE];
    unsigned long long int tge[MAXNUMGE];

};

struct clover_type clover;


struct chain_type chain;
struct recoil_pu_type recoil_pu[100];
struct recoil_type recoil;
struct decay_type decay;
struct decay_type decay_fr;
struct decay_type decay_ba;
struct focal_plane fplane;
struct pixel_type dssd_corr[NUMFR + 1][NUMBA + 1];
struct pixel_type dssd_front[NUMFR + 1];
struct pixel_type dssd_back[NUMBA + 1];



//>>>>>>>>>>>>>>>>>>
// RING BUFFER CODE

#define RBUFFSIZE 5

#if(0)


struct dssd_type
{

    decay_type_new east;
    decay_type_new west;
    decay_type_new top;

};


struct dssd_buffer_type
{

    dssd_type event[RBUFFSIZE];
    int head;
    int tail;
    int count;

};

struct dssd_type dssd_event;
struct dssd_buffer_type dssd_buffer;

//struct global_type global;

struct dssd_buffer_type dssd_tree;



struct decay_type_new east;



struct decay_type_new empty;


struct recoil_buffer_type
{

    recoil_type recoil[RBUFFSIZE];
    int head;
    int tail;
    int count;
    int s_fr;
    int s_ba;

};

struct recoil_buffer_type recbuffer;

struct decay_buffer_type
{

    decay_type decay[RBUFFSIZE];
    int head;
    int tail;
    int count;
    int s_fr;
    int s_ba;
};

struct decay_buffer_type decbuffer;


struct pixel_buffer_type
{
    recoil_buffer_type recbuffer;
    decay_buffer_type decbuffer;
    int status_flag;
};

struct pixel_buffer_type pixel_buff[101][101];

struct pixel_buffer_type front_buff[101];
struct pixel_buffer_type back_buff[101];

//*********************************************************


void DSSDBuffer_put ( dssd_buffer_type* _this, dssd_type& c )
{

    dssd_type newrecoil = c;

    _this->head = ( ( _this->head ) % ( RBUFFSIZE ) + 1 );
    _this->event[ ( _this->head ) - 1] = newrecoil; // WAS _this->recoil[(_this->head)-1] = newrecoil;

    ++_this->count;

}

//*********************************************************

//**********************************************************

void RBuffer_init ( recoil_buffer_type* _this )
{
    //The following clears:
    //-> buf
    // -> head
    //  -> tail
    //  -> count
    //  and sets head = tail

    memset ( _this, 0, sizeof ( *_this ) );

}
//**********************************************************

void DBuffer_init ( decay_buffer_type* _this )
{
    //The following clears:
    //-> buf
    // -> head
    //  -> tail
    //  -> count
    //  and sets head = tail

    memset ( _this, 0, sizeof ( *_this ) );

}

//**********************************************************

int RBuffer_empty ( recoil_buffer_type* _this )
{
    // If the buffer is empty, returns 0. If not, returns 1.
    return ( 0 == _this->count );
}


//**********************************************************

int RBuffer_full ( recoil_buffer_type* _this )
{
    // If the buffer is NOT full, returns 0. If FULL, returns 1.
    return ( _this->count >= RBUFFSIZE );
}

//**********************************************************

void RBuffer_put ( recoil_buffer_type* _this, recoil_type& c )
{

    recoil_type newrecoil = c;

    _this->head = ( ( _this->head ) % ( RBUFFSIZE ) + 1 );
    _this->recoil[ ( _this->head ) - 1] = newrecoil; // WAS _this->recoil[(_this->head)-1] = newrecoil;

    ++_this->count;

}

//**********************************************************

void DBuffer_put ( decay_buffer_type* _this, decay_type& c )
{

    decay_type newrecoil = c;

    _this->head = ( ( _this->head ) % ( RBUFFSIZE ) + 1 );
    _this->decay[ ( _this->head ) - 1] = newrecoil; // WAS_this->decay[(_this->head)-1] = newrecoil;

    ++_this->count;

}

//**********************************************************

int lookback ( int P, int M )
{

    int f = 0;

    f = ( ( P + RBUFFSIZE - ( M % RBUFFSIZE ) ) % RBUFFSIZE );


    return f;

}

#endif


unsigned long long int ts, t_first, t_last, t_firstge;
int CurSubEvNo;
bool first = true;
bool firstge = true;

int RealCount = 0;
int trn1 = 0;
int trn2 = 0;
int TestTr1 = 0;
int TestTr2 = 0;
int TestTr3 = 0;
int TestTr4 = 0;
int leTr = 0;
int numDFMA = 0;
int numDGS = 0;
// external parameters

extern TFile* treef;//TREES...
extern TTree* tree;//TREES...

extern PARS Pars;
extern int ng;
extern DGSEVENT DGSEvent[MAXCOINEV];
extern int tlkup[NCHANNELS];
extern int tid[NCHANNELS];
extern int DFMAEvDecompose_v3 ( unsigned int* ev, int len, DFMAEVENT* DFMAEvent );
extern DFMAEVENT DFMAEvent[MAXCOINEV];

// DEFINE HISTOGRAMS HERE!!!! (LIKE USERDECLARE.H)

TH1D* h1_short_dec_en;
TH2F* h2_le_output_left;
TH2F* h2_rec_gammas;
TH2F* h2_all_gammas;
TH2F* h2_dssd_gammas;
TH2F* h2_mcp_gammas;
//TH2F *h2_te107_gammas;

//TH2F *h2_corr_gammas;
TH2F* h2_corr_gammas_short;
TH2F* h2_gamma_gamma_shortdec;
TH2F* h2_dTgdssd, *h2_dTgdssd_ch40;
TH2F* h2_dTgdssd_allge;
TH1D* h1_dTgl, *h1_dTgr;
TH2F* h2_stripfr_cxng, *h2_stripba_cxng, *h2_stripba_cxng_frontgated;

TH1D* h1_cl, *h1_cr, *h1_esi, *h1_cx, *h1_cxn;
TH2F* h2_clr;
TH1D* h1_clg, *h1_crg, *h1_cxg, *h1_cxng;
TH1D* h1_cx_en_g1, *h1_cxn_en_g1;
TH1D* h1_cx_en_g2, *h1_cxn_en_g2;

TH2F* h2_clrg;
TH2F* h2_clrg_int; //new
TH2F* h2_traceg;
TH2F* h2_traceg2;

TH1D* h1_ch0, *h1_ch1, *h1_ch2, *h1_ch3, *h1_ch4, *h1_ch5;

TH2F* h2_frba_t;


// DSSD stuff

TH2F* h2_dssd_en, *h2_dssd_en_raw;
TH2F* h2_dssd_fr_emax;
TH2F* h2_dssd_ba_emax;
TH2F* h2_d_fr_e, *h2_d_ba_e;
TH2F* h2_r_fr_e, *h2_r_ba_e;
TH2F* h2_dssd_rate, *h2_FP_rate;
//TH1D *h1_dssd_rate_1us;
TH1D* h1_dssd_rate_1D;
TH1D* h1_decay_rate;
TH1D* h1_GE_rate;
TH1D* h1_FP_rate_left, *h1_FP_rate_right;
TH2F* h2_tdssdmcp_fr, *h2_tdssdmcp_ba;
TH2F* h2_tdssdmcp_fr_r, *h2_tdssdmcp_ba_r;
TH1D* h1_ndssd;

TH2F* h2_lefttraces;
TH2F* h2_righttraces;

TH1D* h1_cl_int;
TH1D* h1_cr_int;
TH1D* h1_cxg_int;
TH1D* h1_cxng_int;
TH2F* h2_clr_int;
TH2F* h2_clrg_en;
TH2F* h2_clrg_en_g1, *h2_clrg_en_g2;
TH2F* h2_r_hitxy, *h2_d_hitxy, *h2_dssd_hitxy_phys;

TH2F* h2_dssd_fr_p2;
TH2F* h2_dssd_ba_p2;
TH2F* h2_clr_p2;
TH2F* h2_dssd_fr_p1;
TH2F* h2_dssd_ba_p1;
TH2F* h2_clr_p1;

TH2F* h2_dssd_traces_fr, *h2_dssd_traces_ba;

TH2F* h2_xehi, *h2_xehig;
TH2F* h2_x_d_fr_e, *h2_x_d_fr_e_short;

// Correlation histograms

TH2F* h2_e1t1log, *h2_e2t2log, *h2_e3t3log;
TH2F* h2_e1t1c1, *h2_e2t2c1, *h2_e3t3c1;
TH2F* h2_e1t1c2, *h2_e2t2c2, *h2_e3t3c2;
TH2F* h2_e1t1c3, *h2_e2t2c3, *h2_e3t3c3;
TH2F* h2_e1t1c4, *h2_e2t2c4, *h2_e3t3c4;

/*-----------------------------------------------------*/


int exit_dfma()
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
    Pars.f1->cd();
    treef->Close();
#endif

    return ( 0 );
}

/*-----------------------------------------------------*/

int
sup_dfma()
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

    i = strlen ( Pars.ROOTFile );

    while ( Pars.ROOTFile[i] != '/' )
    {
        i--;
    }

    for ( j = i + 1; j < static_cast<int> ( strlen ( Pars.ROOTFile ) ) ; j++ )
    {
        onechar[0] = Pars.ROOTFile[j];
        onechar[1] = '\0';
        strcat ( presortFile, onechar );
    }

    printf ( "ROOT file %s\n", Pars.ROOTFile );
    printf ( "presort file %s\n", presortFile );



#if(1)
    treef = new TFile ( presortFile, "recreate" );
    tree = new TTree ( "tree", "presorted data" );

    Pars.f1->cd();

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






//*******************
// MAP FILE map.dat *
//*******************

    char str[STRLEN];
    int i1, i2, i7, i8;
    FILE* fp;
    for ( i = 0; i < NCHANNELS; i++ )
    {
        tlkup[i] = NOTHING;
        tid[i] = NOTHING;
    };

    printf ( "MAPPING H\n" );

    fp = fopen ( "./map_gsfma322.dat", "r" );
// fp = fopen ("./map_LBNL_final4.dat", "r");
    if ( fp == NULL )
    {
        printf ( "need a map file to run\n" );
        printf ( "do - g++ mkMap_LBNL.c -o mkMap_LBNL; ./mkMap_LBNL > map_LBNL.dat\n" );
        exit ( 1 );
    };

    printf ( "\nmapping H\n" );
    i2 = fscanf ( fp, "\n%i %i %i %s", &i1, &i7, &i8, str );
    printf ( "%i %i %i %s\n", i1, i7, i8, str );
    while ( i2 == 4 )
    {
        tlkup[i1] = i7;
        tid[i1] = i8;
        i2 = fscanf ( fp, "\n%i %i %i %s", &i1, &i7, &i8, str );
        printf ( "%i %i %i %s\n", i1, i7, i8, str );
    };
    fclose ( fp );

    Pars.wlist = gDirectory->GetList();
    Pars.wlist->Print();



#endif

    return ( 0 );
}

/* ----------------------------------------------------------------- */

/* ----------------------------------------------------------------- */



int DFMAEvDecompose_v3 ( unsigned int* ev, int len, DFMAEVENT* theDFMAEvent )
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


    if ( Pars.CurEvNo <= Pars.NumToPrint )
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

    if ( Pars.CurEvNo <= Pars.NumToPrint )
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

    theDFMAEvent->tpe = tlkup[theDFMAEvent->id];
    theDFMAEvent->tid = tid[theDFMAEvent->id];

    if ( Pars.CurEvNo <= Pars.NumToPrint )
    {
        printf ( "chan_id = %i, board_id=%i, id=%i\n", theDFMAEvent->chan_id, theDFMAEvent->board_id, theDFMAEvent->id );
    }

    /* extract the energy */

    PRE_RISE_SUM = * ( ev + 7 ) & 0x00ffffff;

    if ( Pars.CurEvNo <= Pars.NumToPrint )
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

    if ( Pars.CurEvNo <= Pars.NumToPrint )
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

    if ( Pars.CurEvNo <= Pars.NumToPrint )
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

    if ( Pars.CurEvNo <= Pars.NumToPrint )
    {
        printf ( "exit DGSEvDecompose_v3:\n" );
    }

    return ( 0 );

}


//**********************************************************


int bin_dfma ( GEB_EVENT* GEB_event )
{

    if ( Pars.CurEvNo <= 100 ) //Pars.NumToPrint){
    {
        printf ( "entered bin_dfma:\n" );
    }

#if(1)

    //if(Pars.CurEvNo > 0){
    //if(((DFMAEvent[0].LEDts - t_first)/1E8)>30){
    if ( Pars.CurEvNo > 0 )
    {


        // printf("\nTest1, event number %i\n",Pars.CurEvNo);

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


        if ( Pars.CurEvNo <= Pars.NumToPrint )
        {
            printf ( "entered bin_dfma:\n" );
        }

        //*********************************************************************
        // loop through the coincidence event and fish out GEB_TYPE_DFMA data *
        //*********************************************************************

        ndfma = 0;
        ndssd = 0;
        nsubev = 0;
        nfp = 0;

        for ( i = 0; i < GEB_event->mult; i++ )
        {

            GebTypeStr ( GEB_event->ptgd[i]->type, strg );

            if ( GEB_event->ptgd[i]->type == 16 )
            {

                if ( Pars.CurEvNo <= Pars.NumToPrint )
                {
                    printf ( "bin_dfma, %2i> %2i, %s, TS=%lli\n", i, GEB_event->ptgd[i]->type, strg, GEB_event->ptgd[i]->timestamp );
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
            }

            //nsubev++;

        }

        if ( !Pars.noHists ) h1_ndssd->Fill ( ndssd );

        if ( DFMAEvent[0].LEDts != 0 )
        {
            numDFMA++;
        }
        if ( DGSEvent[0].event_timestamp != 0 )
        {
            numDGS++;
        }



        if ( first && ( numDFMA > 1000 ) && ( numDGS > 1000 ) )
        {
            t_first = DFMAEvent[0].LEDts;
            first = false;
        }
        if ( Pars.CurEvNo % 10000 == 0 )
        {
            printf ( "Processing event number %i with timestamp %llu. Time elapsed: %i seconds\n", Pars.CurEvNo, DFMAEvent[0].LEDts, int ( float ( DFMAEvent[0].LEDts - t_first ) / 1E8 ) );
        }


        if ( firstge )
        {
            t_firstge = DGSEvent[0].event_timestamp;
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

            if ( DFMAEvent[i].tpe == DSSD )
            {

                cal_e = .0;
                //cal_e = DFMAEvent[i].ehi;

                if ( !Pars.noHists ) h2_dssd_en_raw->Fill ( DFMAEvent[i].ehi, DFMAEvent[i].tid );

                // FRONT - selecting maximum energy strip and calibrating

                if ( DFMAEvent[i].tid < BATIDLOW )
                {
                    cal_e = double ( map_fr[DFMAEvent[i].tid].gain ) * ( DFMAEvent[i].ehi + double ( rand() ) / RAND_MAX - 0.5 ) + double ( map_fr[DFMAEvent[i].tid].off );

                    if ( cal_e > dssd_fr_emax )
                    {
                        dssd_fr_emax = cal_e;
                        dssd_fr_subev = i;
                    }
                }

                // BACK - selecting maximum energy strip and calibrating

                if ( DFMAEvent[i].tid > FRTIDHIGH )
                {
                    cal_e = double ( map_ba[DFMAEvent[i].tid].gain ) * ( DFMAEvent[i].ehi + double ( rand() ) / RAND_MAX - 0.5 ) + double ( map_ba[DFMAEvent[i].tid].off );

                    if ( cal_e > dssd_ba_emax )
                    {
                        dssd_ba_emax = cal_e;
                        dssd_ba_subev = i;
                    }
                }

                // UNCOMMENT THIS IF YOU WANT CALIBRATED ENERGIES PROPAGATED THROUGH REMAINDER OF CODE
                DFMAEvent[i].ehi = cal_e;


                if ( !Pars.noHists ) h2_dssd_en->Fill ( DFMAEvent[i].ehi, DFMAEvent[i].tid );
                if ( !Pars.noHists ) h2_dssd_rate->Fill ( ( DFMAEvent[i].LEDts - t_first ) / 1E8, DFMAEvent[i].tid );

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

            frba_t = double ( DFMAEvent[dssd_fr_subev].LEDts ) - double ( DFMAEvent[dssd_ba_subev].LEDts );
            if ( !Pars.noHists ) h2_frba_t->Fill ( frba_t, DFMAEvent[dssd_fr_subev].tid );
            if ( !Pars.noHists ) h2_frba_t->Fill ( frba_t, DFMAEvent[dssd_ba_subev].tid );

            tdssd_fr = DFMAEvent[dssd_fr_subev].LEDts;
            tdssd_ba = DFMAEvent[dssd_ba_subev].LEDts;


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

            switch ( DFMAEvent[i].tpe )
            {

                //-------------*
                // Focal Plane *
                //-------------*

            case FP:
                if ( !Pars.noHists )
                {
                    if ( DFMAEvent[i].tid == 1 )
                    {
                        h1_ch0->Fill ( DFMAEvent[i].ehi );
                    }
                    if ( DFMAEvent[i].tid == 2 )
                    {
                        h1_ch1->Fill ( DFMAEvent[i].ehi );
                    }
                    if ( DFMAEvent[i].tid == 3 )
                    {
                        h1_ch2->Fill ( DFMAEvent[i].ehi );
                    }
                    if ( DFMAEvent[i].tid == 4 )
                    {
                        h1_ch3->Fill ( DFMAEvent[i].ehi );
                    }
                    if ( DFMAEvent[i].tid == 5 )
                    {
                        h1_ch4->Fill ( DFMAEvent[i].ehi );
                    }
                    if ( DFMAEvent[i].tid == 6 )
                    {
                        h1_ch5->Fill ( DFMAEvent[i].ehi );
                    }
                }
                if ( DFMAEvent[i].tid == SICH && DFMAEvent[i].ehi > 0 )
                {
                    gate = 1;
                }


                // MWPC LEFT
                if ( DFMAEvent[i].tid == LCH && DFMAEvent[i].ehi > 0 )
                {

                    left_subev = i;

                    if ( !Pars.noHists ) h2_FP_rate->Fill ( ( DFMAEvent[i].LEDts - t_first ) / 1E8, DFMAEvent[i].tid );
                    if ( !Pars.noHists  && ( DFMAEvent[i].LEDts - t_first ) / 1E5 < 100000 )
                    {
                        h1_FP_rate_left->Fill ( ( DFMAEvent[i].LEDts - t_first ) / 1E5 );
                    }
                    tdssdmcp_fr = ( DFMAEvent[i].LEDts ) - ( tdssd_fr );
                    tdssdmcp_ba = ( DFMAEvent[i].LEDts ) - ( tdssd_ba );

                    if ( !Pars.noHists && tdssd_fr != 0 )
                    {
                        h2_tdssdmcp_fr->Fill ( tdssdmcp_fr, DFMAEvent[dssd_fr_subev].tid );
                    }
                    if ( !Pars.noHists && tdssd_ba != 0 )
                    {
                        h2_tdssdmcp_ba->Fill ( tdssdmcp_ba, DFMAEvent[dssd_ba_subev].tid - 160 );
                    }


                    cl = DFMAEvent[i].ehi;

                    /*


                    if (trn1<1000) {
                                for (jj=0;jj<1000;jj++) h2_traceg->Fill(trn1,jj,DFMAEvent[i].trace[jj]);
                                trn1=trn1+1;
                     }


                     // Trace integration >>>>>>>>>>

                                 // Firstly, find trigger point...



                                 for(j=0;j<1000;j++) letrace[j] = 0;
                                 let0 = 0;
                                 let1 = 0;

                                 leFilter(DFMAEvent[i].trace, 1000, 20, letrace, let0, let1, 200);


                                 if(leTr<100){
                                    for(j=0;j<1000;j++) h2_le_output_left->Fill(leTr,j,letrace[j]);
                                    leTr++;
                                 }


                     for(j=5;j<(5+l_num_bkgd);j++){
                        l_bkgdsum = l_bkgdsum + DFMAEvent[i].trace[j];
                     }

                     l_avgbkgd = (l_bkgdsum/l_num_bkgd);

                                 for(j=let0; j<(let0+l_num_sig); j++){
                        l_signal = l_signal + DFMAEvent[i].trace[j];
                     }

                     cl_int = l_signal - (l_avgbkgd*l_num_sig);

                                 */

                }

                // MWPC RIGHT
                if ( DFMAEvent[i].tid == RCH && DFMAEvent[i].ehi > 0 )
                {

                    right_subev = i;

                    if ( !Pars.noHists ) h2_FP_rate->Fill ( ( DFMAEvent[i].LEDts - t_first ) / 1E8, DFMAEvent[i].tid );
                    if ( !Pars.noHists && ( DFMAEvent[i].LEDts - t_first ) / 1E5 < 100000 )
                    {
                        h1_FP_rate_right->Fill ( ( DFMAEvent[i].LEDts - t_first ) / 1E5 );
                    }

                    tdssdmcp_fr_r = ( DFMAEvent[i].LEDts ) - ( tdssd_fr );
                    tdssdmcp_ba_r = ( DFMAEvent[i].LEDts ) - ( tdssd_ba );

                    if ( !Pars.noHists  && tdssd_fr != 0 )
                    {
                        h2_tdssdmcp_fr_r->Fill ( tdssdmcp_fr_r, DFMAEvent[dssd_fr_subev].tid );
                    }
                    if ( !Pars.noHists && tdssd_ba != 0 )
                    {
                        h2_tdssdmcp_ba_r->Fill ( tdssdmcp_ba_r, DFMAEvent[dssd_ba_subev].tid - 160 );
                    }

                    cr = DFMAEvent[i].ehi;

                    /*


                    if (trn2<1000) {
                    for (jj=0;jj<1000;jj++) h2_traceg2->Fill(trn2,jj,DFMAEvent[i].trace[jj]);
                    trn2=trn2+1;
                    }


                    // Trace integration>>>>>>>>>>
                    // Firstly, find trigger point...



                    for(j=0;j<1000;j++) letrace[j] = 0;
                    let0 = 0;
                    let1 = 0;

                    leFilter(DFMAEvent[i].trace, 1000, 20, letrace, let0, let1, 200);

                    for(j=5;j<(5+r_num_bkgd);j++){
                         r_bkgdsum = r_bkgdsum + DFMAEvent[i].trace[j];
                      }

                      r_avgbkgd = (r_bkgdsum/r_num_bkgd);

                                  for(j=let0;j<(let0+r_num_sig);j++){
                         r_signal = r_signal + DFMAEvent[i].trace[j];
                      }

                      cr_int = r_signal - (r_avgbkgd*r_num_sig);

                                  */

                    //>>>>>>>>>>>>>>>>>>>>>>>>>>

                }



                if ( DFMAEvent[i].tid == SICH && DFMAEvent[i].ehi > 0 )
                {
                    esi = DFMAEvent[i].ehi;
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

            if ( DGSEvent[i].tpe == GE && geone == 1000 )
            {
                geone = i;
            }

        }


        int ch40subev = 1000;


        for ( j = 0; j < ng; j++ )
        {
            if ( DGSEvent[j].tid == 40 && DGSEvent[j].tpe == GE && DGSEvent[j].flag == 0 )
            {
                ch40subev = j;
            }
        }



        if ( ch40subev < 1000 )
        {
            for ( i = 0; i < nsubev; i++ )
            {
                if ( DFMAEvent[i].tpe == DSSD )
                {
                    dTgdssd = double ( DGSEvent[ch40subev].event_timestamp ) - double ( DFMAEvent[i].LEDts );
                    if ( !Pars.noHists ) h2_dTgdssd_ch40->Fill ( dTgdssd, DFMAEvent[i].tid );
                }
            }
        }




        dTgdssd = 0.0;

        for ( i = 0; i < nsubev; i++ )
        {
            if ( ( DFMAEvent[i].LEDts > 0 ) && ( DFMAEvent[i].tpe == DSSD ) && ( ng > 0 ) )
            {

                dTgdssd = double ( DGSEvent[geone].event_timestamp ) - double ( DFMAEvent[i].LEDts );
                if ( !Pars.noHists ) h2_dTgdssd->Fill ( dTgdssd, DFMAEvent[i].tid );






                if ( dTgdssd > 1000 && Pars.CurEvNo < 10000 )
                {

                    //printf("\n\nDebugging!");
                    //printf("\nEvent number %i",Pars.CurEvNo);
                    //printf("\n DFMAEvent[%i].LEDts: %llu",i,DFMAEvent[i].LEDts);
                    //printf("\n DGSEvent[%i].LEDts: %llu",geone,DGSEvent[geone].event_timestamp);

                }


            }
        }


        dTgdssd = 0.0;


        if ( dssd_fr_emax > 0 )
        {

            for ( i = 0; i < ng; i++ )
            {
                if ( DGSEvent[i].tpe == GE && DGSEvent[i].flag == 0 )
                {
                    dTgdssd = double ( DGSEvent[i].event_timestamp ) - double ( DFMAEvent[dssd_fr_subev].LEDts );
                    if ( !Pars.noHists )  h2_dTgdssd_allge->Fill ( dTgdssd, DFMAEvent[dssd_fr_subev].tid );
                }
            }

        }





        double dTgl;
        dTgl = 0.0;
        double dTgr;
        dTgr = 0.0;

        if ( DFMAEvent[left_subev].LEDts > 0 )
        {
            dTgl = double ( DGSEvent[0].event_timestamp ) - double ( DFMAEvent[left_subev].LEDts );
            if ( !Pars.noHists ) h1_dTgl->Fill ( dTgl );
        }
        if ( DFMAEvent[right_subev].LEDts > 0 )
        {
            dTgr = double ( DGSEvent[0].event_timestamp ) - double ( DFMAEvent[right_subev].LEDts );
            if ( !Pars.noHists ) h1_dTgr->Fill ( dTgr );
        }






//<><><>*
// MWPC *
//<><><>*

        if ( !Pars.noHists )
        {
            h1_esi->Fill ( esi );

            if ( cl != 0 )
            {
                if ( TestTr1 < 100 )
                {
                    for ( i = 0; i < 1000; i++ )
                    {
                        h2_lefttraces->Fill ( TestTr1, i, DFMAEvent[left_subev].trace[i] );

                    }
                    TestTr1++;
                }
            }

            if ( cr != 0 )
            {
                if ( TestTr4 < 100 )
                {
                    for ( i = 0; i < 1000; i++ )
                    {
                        h2_righttraces->Fill ( TestTr4, i, DFMAEvent[right_subev].trace[i] );

                    }
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

            if ( !Pars.noHists ) h1_cl_int->Fill ( cl_int ); // new
            if ( !Pars.noHists ) h1_cr_int->Fill ( cr_int ); // new

            if ( !Pars.noHists ) h1_cl->Fill ( cl );
            if ( !Pars.noHists ) h1_cr->Fill ( cr );
            if ( !Pars.noHists ) h2_clr->Fill ( cl / 10, cr / 10 );
            if ( !Pars.noHists ) h2_clr_int->Fill ( cl_int / 50, cr_int / 50 ); //new

            if ( !Pars.noHists ) h1_cx->Fill ( crat );

            if ( !Pars.noHists ) h1_cxn->Fill ( cratn );

            if ( ndssd > 0 )
            {

                if ( !Pars.noHists ) h1_clg->Fill ( cl );
                if ( !Pars.noHists ) h1_crg->Fill ( cr );
                if ( !Pars.noHists ) h2_clrg->Fill ( cl, cr );

                if ( dssd_fr_emax > 4500 && dssd_ba_emax > 4500 ) // Gating on Alpha energies
                {

                    if ( !Pars.noHists ) h2_clrg_en->Fill ( cl, cr );

                    if ( !Pars.noHists && map_fr[DFMAEvent[dssd_fr_subev].tid].phystrip >= 20 && map_fr[DFMAEvent[dssd_fr_subev].tid].phystrip < 40 )
                    {
                        h2_clrg_en_g1->Fill ( cl, cr );
                        h1_cx_en_g1->Fill ( cl / cr * 1000.0 );
                        h1_cxn_en_g1->Fill ( cl / ( cl + cr ) * 1000.0 );
                    }

                    if ( Pars.noHists && map_fr[DFMAEvent[dssd_fr_subev].tid].phystrip >= 100 && map_fr[DFMAEvent[dssd_fr_subev].tid].phystrip < 120 )
                    {
                        h2_clrg_en_g2->Fill ( cl, cr );
                        h1_cx_en_g2->Fill ( cl / cr * 1000.0 );
                        h1_cxn_en_g2->Fill ( cl / ( cl + cr ) * 1000.0 );
                    }

                    if ( !Pars.noHists )
                    {
                        h2_stripfr_cxng->Fill ( map_fr[DFMAEvent[dssd_fr_subev].tid].phystrip, cratn );
                        h2_stripba_cxng->Fill ( map_ba[DFMAEvent[dssd_ba_subev].tid].phystrip, cratn );
                    }

                    if ( !Pars.noHists && map_fr[DFMAEvent[dssd_fr_subev].tid].phystrip > 74 && map_fr[DFMAEvent[dssd_fr_subev].tid].phystrip < 86 )
                    {

                        h2_stripba_cxng_frontgated->Fill ( map_ba[DFMAEvent[dssd_ba_subev].tid].phystrip, cratn );

                    }



                }

                if ( !Pars.noHists )
                {
                    h2_clrg_int->Fill ( cl_int / 50, cr_int / 50 ); //new

                    h1_cxg->Fill ( crat );
                    h1_cxng->Fill ( cratn );

                    h1_cxg_int->Fill ( crat_int ); //new
                    h1_cxng_int->Fill ( cratn_int ); //new
                }
            }

        }

        // printf("\nTest2, event number %i\n",Pars.CurEvNo);


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
                s_r_fr = DFMAEvent[dssd_fr_subev].tid;
                r_fr_ts = DFMAEvent[dssd_fr_subev].LEDts; // Take recoil timestamp from FRONT...
                r_fr_subev = dssd_fr_subev;
                r_fr_PU = DFMAEvent[dssd_fr_subev].pu;
                r_fr_trace_len = DFMAEvent[dssd_fr_subev].traceLen;
                r_fr_d2t0 = DFMAEvent[dssd_fr_subev].d2t0;
                r_fr_d2t1 = DFMAEvent[dssd_fr_subev].d2t1;
                r_fr_d2t2 = DFMAEvent[dssd_fr_subev].d2t2;
                for ( j = 0; j < DFMAEvent[dssd_fr_subev].traceLen; j++ )
                {
                    r_fr_trace[j] = DFMAEvent[dssd_fr_subev].trace[j];
                }

                //BACK
                r_ba_emax = dssd_ba_emax;
                s_r_ba = DFMAEvent[dssd_ba_subev].tid - NUMFR;
                r_ba_ts = DFMAEvent[dssd_ba_subev].LEDts; // Take recoil timestamp from BACK...
                r_ba_subev = dssd_ba_subev;
                r_ba_PU = DFMAEvent[dssd_ba_subev].pu;
                r_ba_trace_len = DFMAEvent[dssd_ba_subev].traceLen;
                r_ba_d2t0 = DFMAEvent[dssd_ba_subev].d2t0;
                r_ba_d2t1 = DFMAEvent[dssd_ba_subev].d2t1;
                r_ba_d2t2 = DFMAEvent[dssd_ba_subev].d2t2;
                for ( j = 0; j < DFMAEvent[dssd_ba_subev].traceLen; j++ )
                {
                    r_ba_trace[j] = DFMAEvent[dssd_ba_subev].trace[j];
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
                s_d_fr = DFMAEvent[dssd_fr_subev].tid;
                s_d_fr_phys = map_fr[DFMAEvent[dssd_fr_subev].tid].phystrip;
                d_fr_subev = dssd_fr_subev;
                d_fr_ts = DFMAEvent[dssd_fr_subev].LEDts;
                d_fr_PU = DFMAEvent[dssd_fr_subev].pu;
                d_fr_trace_len = DFMAEvent[dssd_fr_subev].traceLen;
                for ( jj = 0; jj < DFMAEvent[dssd_fr_subev].traceLen; jj++ )
                {
                    d_fr_trace[jj] = DFMAEvent[dssd_fr_subev].trace[jj];
                }

                //BACK
                d_ba_emax = dssd_ba_emax;
                s_d_ba = DFMAEvent[dssd_ba_subev].tid - NUMFR;
                s_d_ba_phys = map_ba[DFMAEvent[dssd_ba_subev].tid].phystrip;
                d_ba_ts = DFMAEvent[dssd_ba_subev].LEDts;     // Take decay timestamp from BACK...
                d_ba_subev = dssd_ba_subev;
                d_ba_PU = DFMAEvent[dssd_ba_subev].pu;
                d_fr_trace_len = DFMAEvent[dssd_ba_subev].traceLen;
                for ( jj = 0; jj < DFMAEvent[dssd_ba_subev].traceLen; jj++ )
                {
                    d_ba_trace[jj] = DFMAEvent[dssd_ba_subev].trace[jj];
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

        if ( !Pars.noHists )
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
                h2_dssd_hitxy_phys->Fill ( map_fr[DFMAEvent[dssd_fr_subev].tid].phystrip, map_ba[DFMAEvent[dssd_ba_subev].tid].phystrip );

                if ( TestTr3 < 100 )
                {
                    for ( i = 0; i < 1000; i++ )
                    {
                        h2_dssd_traces_fr->Fill ( TestTr3, i, DFMAEvent[dssd_fr_subev].trace[i] );
                        h2_dssd_traces_ba->Fill ( TestTr3, i, DFMAEvent[dssd_ba_subev].trace[i] );
                    }
                    TestTr3++;
                }
            }

            if ( dssd_fr_emax > 0 )
            {

                h2_dssd_fr_emax->Fill ( dssd_fr_emax, DFMAEvent[dssd_fr_subev].tid );
                h2_d_fr_e->Fill ( d_fr_emax, s_d_fr );
                h2_r_fr_e->Fill ( r_fr_emax, s_r_fr );


                if ( ( DFMAEvent[dssd_fr_subev].LEDts - t_first ) / 1E5 < 100000 )
                {
                    h1_dssd_rate_1D->Fill ( ( DFMAEvent[dssd_fr_subev].LEDts - t_first ) / 1E5 );
                    //h1_dssd_rate_1us->Fill((DFMAEvent[dssd_fr_subev].LEDts - t_first)/100);
                    if ( s_d_fr != 0 )
                    {
                        h1_decay_rate->Fill ( ( DFMAEvent[dssd_fr_subev].LEDts - t_first ) / 1E5 );
                    }

                }

            }

            if ( dssd_ba_emax > 0 )
            {

                h2_dssd_ba_emax->Fill ( dssd_ba_emax, DFMAEvent[dssd_ba_subev].tid - NUMFR );
                h2_d_ba_e->Fill ( d_ba_emax, s_d_ba );
                h2_r_ba_e->Fill ( r_ba_emax, s_r_ba );

            }
        }




        /*
        if(Pars.CurEvNo > 1925000){
        printf("\n\nEvent number: %i, r_fr_subev: %i, s_r_fr: %i, s_r_ba : %i, ng: %i\n",Pars.CurEvNo,r_fr_subev,s_r_fr,s_r_ba,ng);
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

            //if(Pars.CurEvNo > 1925000)printf("EvNum %i Are we in this loop?\n",Pars.CurEvNo);

            switch ( dssd_corr[s_r_fr][s_r_ba].status )
            {

            case 0:

                // no recoils so far

            case 1:

                // recoil last

                recoil.pu = DFMAEvent[r_fr_subev].pu;
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
                    if ( DGSEvent[i].tpe == GE && jj < MAXNUMGE )
                    {
                        recoil.geehi[jj] = DGSEvent[i].ehi;
                        recoil.tge[jj] = DGSEvent[i].event_timestamp;
                        recoil.getid[jj] = DGSEvent[i].tid;
                        jj++;
                    }
                }
#endif


                dssd_corr[s_r_fr][s_r_ba].status = 1;
                dssd_corr[s_r_fr][s_r_ba].chain.recoil = recoil;
                dssd_corr[s_r_fr][s_r_ba].chain.s_fr = s_r_fr;
                dssd_corr[s_r_fr][s_r_ba].chain.s_ba = s_r_ba;


                // if(Pars.CurEvNo > 1925800)printf("\nTest middle event number %i\n",Pars.CurEvNo);

                break;
            default: // there is chain already

                // store previous chain in the tree

                chain = dssd_corr[s_r_fr][s_r_ba].chain;
                chain.corr_type = 1;
                tree->Fill();

                // reset the pixel

                recoil.pu = DFMAEvent[r_fr_subev].pu;
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
                    if ( DGSEvent[i].tpe == GE && jj < MAXNUMGE )
                    {
                        recoil.geehi[jj] = DGSEvent[i].ehi;
                        recoil.tge[jj] = DGSEvent[i].event_timestamp;
                        recoil.getid[jj] = DGSEvent[i].tid;
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
                    if ( !Pars.noHists ) h2_e1t1log->Fill ( decay.en, logtime );

                }

                if ( !Pars.noHists ) h2_x_d_fr_e->Fill ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.x, decay.en );

                if ( logtime < 8 && logtime > 0 )
                {

                    if ( !Pars.noHists ) h2_x_d_fr_e_short->Fill ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.x, decay.en );

                    //if ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.nge > 0 )
                    //{
                    //for(j=0;j<dssd_corr[s_d_fr][s_d_ba].chain.recoil.nge;j++) h2_corr_gammas->Fill(decay.en,dssd_corr[s_d_fr][s_d_ba].chain.recoil.geehi[j]);
                    //}
                    if ( !Pars.noHists ) h1_short_dec_en->Fill ( decay.en );

                }

                if ( logtime < 8 && logtime > 0 )
                {

                    if ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.nge > 0 )
                    {

                        for ( j = 0; j < dssd_corr[s_d_fr][s_d_ba].chain.recoil.nge; j++ )
                        {
                            dTgdssd = double ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.tge[j] ) - double ( dssd_corr[s_d_fr][s_d_ba].chain.recoil.ts );
                            if ( !Pars.noHists && dTgdssd > dtmin && dTgdssd < dtmax )
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
                                        if ( !Pars.noHists && dTgdssd > dtmin && dTgdssd < dtmax )
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

                //if ( !Pars.noHists )
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
                    if ( !Pars.noHists ) h2_e3t3log->Fill ( decay.en, logtime );
                }

                //if ( !Pars.noHists )
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
                   if ( !Pars.noHists ) h2_e4t4log->Fill(decay.en,logtime);
                }

                if ( !Pars.noHists )
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

        // printf("\nTest4, event number %i\n",Pars.CurEvNo);

#if(1)

        if ( !Pars.noHists )
        {
            for ( i = 0; i < ng; i++ )
            {
                if ( ( DGSEvent[i].tpe == GE ) && ( DGSEvent[i].flag == 0 ) ) // Check for clean GEs
                {
                    h2_all_gammas->Fill ( DGSEvent[i].ehi, DGSEvent[i].tid );


                    if ( ( DGSEvent[i].event_timestamp - t_first ) / 1E5 < 100000 )
                    {
                        h1_GE_rate->Fill ( ( DGSEvent[i].event_timestamp - t_first ) / 1E5 );
                    }


                }
            }
        }

#endif


//<><><><><><><><><><><><><>*
// Recoil-correlated gammas *
//<><><><><><><><><><><><><>*



#if(1)

//printf("\n\n1) Event number %i has s_r_fr %i, s_r_ba %i, ng %i",Pars.CurEvNo,s_r_fr,s_r_ba,ng);
//printf("\nndssd: %i, cl: %f, cr: %f\n",ndssd,cl,cr);
        if ( !Pars.noHists )
        {
            if ( s_r_fr != 0 && s_r_ba != 0 )
            {



                for ( i = 0; i < ng; i++ )
                {



                    if ( ( DGSEvent[i].tpe == GE ) && ( DGSEvent[i].flag == 0 ) ) // Check for clean GEs
                    {
                        h2_rec_gammas->Fill ( DGSEvent[i].ehi, DGSEvent[i].tid );
                        h2_xehi->Fill ( cratn, DGSEvent[i].ehi );

                        if ( dTgdssd > dtmin && dTgdssd < dtmax )
                        {
                            h2_xehig->Fill ( cratn, DGSEvent[i].ehi );
                        }


                    }
                }
            }


            if ( ndssd > 0 )
            {



                for ( i = 0; i < ng; i++ )
                {
                    if ( ( DGSEvent[i].tpe == GE ) && ( DGSEvent[i].flag == 0 ) ) // Check for clean GEs
                    {
                        h2_dssd_gammas->Fill ( DGSEvent[i].ehi, DGSEvent[i].tid );
                    }
                }
            }


            if ( cl != 0 || cr != 0 )
            {

                for ( i = 0; i < ng; i++ )
                {
                    if ( ( DGSEvent[i].tpe == GE ) && ( DGSEvent[i].flag == 0 ) ) // Check for clean GEs
                    {
                        h2_mcp_gammas->Fill ( DGSEvent[i].ehi, DGSEvent[i].tid );
                    }
                }
            }
        }




#endif


//<><><><><><><><><>*
// Print statements *
//<><><><><><><><><>*

        if ( Pars.CurEvNo <= Pars.NumToPrint )
        {

            printf ( "Print statements at end of bin_dfma\n" );

            for ( i = 0; i < 1; i++ )
            {


                printf ( "\n\n\nwe have %i gamma rays\n", ng );
                printf ( "%2i> ", i );
                printf ( "chan_id=%i; ", DGSEvent[i].chan_id );
                printf ( "board_id=%i; ", DGSEvent[i].board_id );
                printf ( "id =%i; ", DGSEvent[i].id );
                printf ( "tpe=%i; ", DGSEvent[i].tpe );
                printf ( "tid=%i; ", DGSEvent[i].tid );
                printf ( "EventTS=%llu; ", DGSEvent[i].event_timestamp );
                printf ( "ehi=%f ", DGSEvent[i].ehi );
                printf ( "\n\n\n" );


            }
        }

        // debug list the dssd events we found

        if ( Pars.CurEvNo <= Pars.NumToPrint )
            for ( i = 0; i < ndssd; i++ )
            {
                printf ( "we have %i DSSD event(s)\n", ndssd );
                printf ( "%2i> ", i );
                printf ( "chan_id=%i; ", DFMAEvent[i].chan_id );
                printf ( "board_id=%i; ", DFMAEvent[i].board_id );
                printf ( "id =%i; ", DFMAEvent[i].id );
                printf ( "tpe=%i; ", DFMAEvent[i].tpe );
                printf ( "tid=%i; ", DFMAEvent[i].tid );
                printf ( "LEDTS=%llu; ", DFMAEvent[i].LEDts );
                printf ( "ehi=%8i ", DFMAEvent[i].ehi );
                printf ( "\n\n\n\n" );
                fflush ( stdout );
            };


        // done


        if ( Pars.CurEvNo <= Pars.NumToPrint )
        {
            printf ( "exit bin_dfma\n" );
        }

        return ( 0 );
    }



#endif

    return ( 0 );
}





//*******************************************************************************************************



#if(0)
void
SetBeta()
{

    /* declarations */

    int i;
    double d1;

    /*-------------------------------------*/
    /* find Doppler and aberration factors */
    /*-------------------------------------*/

    for ( i = 0; i < NGSGE; i++ )
    {
        //printf("det %3.3i-> ", i);
        d1 = angle[i] / 57.29577951;
        DopCorFac[i] = ( 1 - Pars.beta * cos ( d1 ) ) / sqrt ( 1 - Pars.beta * Pars.beta );
        //printf("dop cor fac: %6.4f; ", DopCorFac[i]);
        ACFac[i] = DopCorFac[i] * DopCorFac[i];
        //printf("aberration cor fac: %6.4f\n", ACFac[i]);

    };
    fflush ( stdout );



}

#endif
