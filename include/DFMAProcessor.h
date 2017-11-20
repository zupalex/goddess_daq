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
#include "limits.h"

#include "GEBSort.h"
#include "GTMerge.h"

#include "SortManager.h"

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

//>>>>>>>>>>>>>>>>>>
// RING BUFFER CODE

#define RBUFFSIZE 5

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

struct StripType
{
    int phystrip;
    int thr;
    float off;
    float gain;
    int baseline;
};

struct CloverMap
{
    float gain;
    float off;
};

struct RecoilPUType
{
    short int trace[MAXTRACELEN];
    unsigned short int traceLen;
    int s_fr;
    int s_ba;
    unsigned long long int ts;
};

struct RecoilType
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

struct DecayType_new
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

struct DecayType
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

struct FocalPlane
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

struct ChainType
{

    int s_fr;
    int s_ba;
    RecoilType recoil;
    int ndec;
    DecayType decay[4];//*4thGen
    int corr_type;//*26th Apr

};

struct PixelType
{

    int status;
    ChainType chain;

};

struct CloverType
{
    int nge;
    int tid[MAXNUMGE];
    long long int ehi[MAXNUMGE];
    unsigned long long int tge[MAXNUMGE];
};

/// Class used to unpack and sort data recorded using the DFMA DAQ
class DFMAProcessor
{
private:

public:
    DFMAProcessor ( int* tlkup_, int* tid_, int* ng_, PARS* pars_ );

    ~DFMAProcessor() {}

    StripType map_fr[NUMSTRIPS + 1];
    StripType map_ba[NUMSTRIPS + 1];

    CloverMap clmap[21];

    CloverType clover;

    ChainType chain;
    RecoilPUType recoil_pu[100];
    RecoilType recoil;
    DecayType decay;
    DecayType decay_fr;
    DecayType decay_ba;
    FocalPlane fplane;
    PixelType dssd_corr[NUMFR + 1][NUMBA + 1];
    PixelType dssd_front[NUMFR + 1];
    PixelType dssd_back[NUMBA + 1];

    unsigned long long int ts, t_first, t_last, t_firstge;
    int CurSubEvNo;
    bool first = true;
    bool firstge = true;

    int RealCount;
    int trn1;
    int trn2;
    int TestTr1;
    int TestTr2;
    int TestTr3;
    int TestTr4;
    int leTr;
    int numDFMA;
    int numDGS;

    PARS* pars;

    TFile* treef;
    TTree* tree;

    // external parameters

    int* ng;
    int* tlkup;
    int* tid;

    DGSEVENT dgsEvt[MAXCOINEV];
    DFMAEVENT dfmaEvt[MAXCOINEV];

    int ExitDFMA();
    int SupDFMA();
    int DFMAEvDecompose_v3 ( unsigned int* ev, int len, DFMAEVENT* thedfmaEvt );
    int BinDFMA ( GEB_EVENT* gebEvt );
};

