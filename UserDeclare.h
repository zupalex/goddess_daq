
/*-----------------------*/
/* user declaration code */
/* (may be empty.......) */
/*-----------------------*/

signed long long int t_first;
signed long long int t_FP_last, t_DSSD_last, t_DSSD113_last, t_XARRAY_last;

t_first = -1;

signed long long int t_last140, t_last240;
t_last140 = 0; t_last240 = 0;
signed long int E_last140, E_last240;
E_last140 = 0; E_last240 = 0;

int Max, counter_fr, counter_ba;
Max = 1000;
counter_fr = 0;
counter_ba = 0;

TH2F *h2_recoilen_tdssdppac_fission, *h2_recoilen_tdssdppac_electrons;

int Trdisp=0;

const int mrun=50; //m for trapezoid voltage in runs 185-188 (trace outputs)
const int krun=200; //k for trapezoid voltage in runs 185-188 (trace outputs)

//TH2F *h2_backside_traces_fr, *h2_backside_traces_ba, *h2_frontside_traces_fr, *h2_frontside_traces_ba;


int NumRTrToPr, NumRTr, NumDTrToPr, NumDTr;
NumRTrToPr = 100;
NumRTr = 0;
NumDTrToPr = 100;
NumDTr = 0;

int trmax, trcurr;
trmax = 1000;
trcurr = 0;

int PU_hiE_fr, NumPU_hiE_fr, PU_hiE_ba, NumPU_hiE_ba;
PU_hiE_fr = 0;
NumPU_hiE_fr = 0;
NumPU_hiE_ba = 1000;
PU_hiE_ba = 0;

int Tr, NumTr;
Tr = 0;
//NumTr = 20000;
//NumTr = 1000;
NumTr = 10;

int PU_recoil_fr, PU_recoil_ba, PU_dec_fr, PU_dec_ba, PU_decay, PU_gated, NumPU, fast_fr, fast_ba, dec1PuCorr, dec2PuCorr;
fast_fr=0;
fast_ba=0;
PU_recoil_fr = 0;
PU_recoil_ba = 0;
PU_dec_fr = 0;
PU_dec_ba = 0;
PU_decay = 0;
PU_gated = 0;
dec1PuCorr=0;
dec2PuCorr=0;
NumPU = 1000;

int hiEfr, NumhiEfr;
hiEfr = 0;
NumhiEfr = 1000;
int hiEba, NumhiEba;
hiEba = 0;
NumhiEba = 1000;
int NumBad, Bad;
Bad = 0;
NumBad = 1000;

TH1D *h1_CurEvNo_DSSD_LEDts,*h1_CurEvNo_DSSD113_LEDts;
TH2F *h2_CurEvNo_DSSD_LEDts;
TH1D *h1_CurEvNo_XARRAY_LEDts;
TH2F *h2_CurEvNo_XARRAY_LEDts;
TH1D *h1_CurEvNo_FP_LEDts;
TH2F *h2_CurEvNo_FP_LEDts;


TH1D *h1_dssd_dT140, *h1_dssd_dT240;
TH2F *h2_dssd_dTE140, *h2_dssd_dTE240;

TH1D *h1_d_fr_emax, *h1_d_ba_emax;

//TH2F *h2_badtrace;

//energy outputs
TH2F *h2_ehi_trE;

const int NFS = 32;

TH2F *h2_ehi_trapV[NFS];
TH1D *h1_ehi[NFS], *h1_trapV[NFS];
// end

TH2F *h2_ge, *h2_genocalib;

TH1D *h1_decay_generations;

TH2F *h2_dssd_frbase_raw, *h2_dssd_babase_raw;
TH2F *h2_fast_bl_fr, *h2_fast_bl_ba;

TH1D *h1_decay_ediff;

TH2F *h2_dssd_badfr, *h2_dssd_badba;

TH2F *h2_recoilen_tdssdppac, *h2_recoilen_icde;
TH2F *h2_recoilen_tdssdppac_gated;

//TH2F *h2_test_traces;

TH2F *h2_dssd_traces, *h2_dssd_traces_smooth, *h2_dssd_traces_le, *h2_dssd_traces_trapez, *h2_dssd_traces_raw, *h2_levsid, *h2_dssd_traces_delay;

TH2F *h2_dssd_deriv, *h2_dssd_derivt;

TH1D *h1_dssd_traces_2d;

TH2F *h2_dssd_Vtrap_tlinear[NFS];

//TH2F *h2_dssd_Vtrap_tlinear;

TH2F  *h2_recoil_pu_traces, *h2_recoil_pu_traces_fr, *h2_recoil_pu_traces_ba;
TH2F  *h2_dec_pu_traces_fr, *h2_dec_pu_traces_ba;
TH2F *h2_decay_pu_traces, *h2_recoil_pu_traces_gated, *h2_recoil_corr_traces;
TH2F *h2_fast_traces_fr, *h2_fast_traces_ba;
TH2F *h2_dec1pu_corr_fr_traces, *h2_dec1pu_corr_ba_traces, *h2_dec2pu_corr_fr_traces, *h2_dec2pu_corr_ba_traces;

TH1D *h1_tr_hp;

TH2F *h2_pu_traces, *h2_pud1_traces, *h2_pud2_traces;

//TH2F *h2_d_fr_traces_hie, *h2_d_ba_traces_hie;
//TH2F *h2_recoil_traces, *h2_decay_traces;
//TH1D *h1_ge_CFDs;
TH1D *h1_fplane_stats;
//TH1D *h1_gam;
TH1D *h1_recrate, *h1_recrate_ch, *h1_decrate, *h1_dssdrate_ch1, *h1_dssdrate_ch2, *h1_fprate;

TH2F *h2_dssd_fr_rawe, *h2_dssd_ba_rawe;

TH2F *h2_dssd_en_raw;

TH2F *h2_r_hitxy, *h2_r_fr_e, *h2_r_ba_e;
TH2F *h2_d_hitxy, *h2_d_fr_e, *h2_d_ba_e, *h2_d_fr_all, *h2_d_ba_all, *h2_r_fr_all, *h2_r_ba_all, *h2_d_fb;
TH1D *h1_tgppac;
TH2F *h2_tgppac_all;
TH2F *h2_tdssdppac_ba;
TH2F *h2_tdssdppac_raw;
//TH2F *h2_x_ge, *h2_x_ge2, *h2_tid_ge, *h2_tid_gebase;
#if(0)
TH2F *h2_dssd_en_fr, *h2_dssd_en_ba, *h2_dssd_en_imp, *h2_dssd_en_rec, *h2_dssd_en_dec;
TH2F *h2_timeid;

TH1D *h1_clen, *h1_leddiff, *h1_cfddiff, *h1_id, *h1_tid;

#endif
TH2F *h2_all_tr[180];

TH1D *h1_rl, *h1_x;
TH1D *h1_l, *h1_r;
TH2F *h2_LvR, *h2_Lvppacde, *h2_Rvppacde, *h2_Rvppacde_rec, *h2_x_icde, *h2_x_ppacde;
TH1D *h1_icde, *h1_ppacde, *h1_ppacde2_raw;
TH2F *h2_ppacde_icde;

TH2F *h2_R_corr_decayen, *h2_R_corr_decayen_lowE;
TH2F *h2_xe1, *h2_xe1_short;

TH1D *h1_rl_raw, *h1_x_raw;
TH1D *h1_l_raw, *h1_r_raw;
TH2F *h2_LvR_raw, *h2_Lvppacde_raw, *h2_Rvppacde_raw, *h2_x_icde_raw, *h2_x_ppacde_raw;
TH1D *h1_icde_raw, *h1_ppacde_raw;

TH2F *h2_e1t1log_fr, *h2_e1t1log_ba;
TH2F *h2_e1t1log, *h2_e1t1c1, *h2_e1t1c2, *h2_e1t1c3, *h2_e1t1c4, *h2_e1e2, *h2_e1e2_short, *h2_e2e3, *h2_e2e3_short;
TH2F *h2_e2t2log, *h2_e2t2c1, *h2_e2t2c2, *h2_e2t2c3, *h2_e2t2c4;
TH2F *h2_e3t3log, *h2_e3t3c1, *h2_e3t3c2, *h2_e3t3c3, *h2_e3t3c4;
//TH2F *h2_gam_decayen_corr;
//TH1D *h1_gam_rec, *h1_gam_rec_dec;
TH2F *h2_tid_base_test;
TH2F *h2_baseline_ehi;

TH1D *h1_pu, *h1_pu_daq;

TH1D *h1_nge;
//TH2F *h2_d58ppacde;

TH1D *h1_trlen, *h1_t0, *h1_t1, *h1_dt;


int NumPuType, PuType, PuTypeFills;
NumPuType = 100;
PuType = 0;
PuTypeFills = 0;

// GODDESS

int iQ;

const int NX3FS = 32;
const int NSX3BS = 16;

Float_t si_en_raw;
Float_t si_ps_raw;

TH2F *si_ep[NX3FS];
TH2F *si_lr[NX3FS];

TH2F *si_ep_offline[NX3FS];
TH2F *si_lr_offline[NX3FS];

TH2F *si_tl[16], *si_tr[16];

//TH1D *si_trV; //fault finding

char gStr1[40];
char gStrB[40];

TTree *GDST;

int trackTr;
Float_t trVal1[MAXTRACELEN];
Float_t trVal2[MAXTRACELEN];
Float_t trBksV[4][MAXTRACELEN];

struct recoil_pu_type {

   short int trace[MAXTRACELEN];
   unsigned short int traceLen;
   int s_fr;
   int s_ba;
   unsigned long long int ts;

};

recoil_pu_type recoil_pu[100];

struct recoil_type {

   unsigned long long int ts;
   long int en;
   int pu;
   int right;
   //long int nge;
   //long int ehi[5];   
   //unsigned long long int tge[5];
   //unsigned long long int tgeCFD[5];
   //int getid[5];
   //int baseline[5];
   signed long long int tof;
   //int dssdbase;
   //unsigned short int traceLen;
   //short int trace[MAXTRACELEN];

};

recoil_type recoil;

struct decay_type {

   unsigned long long int ts;
   int en;
   unsigned long long int time;
   unsigned short int traceLen;
   //int dssdbase;
   //short int trace[MAXTRACELEN];
};

decay_type decay;

struct focal_plane {

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

focal_plane fplane;

struct chain_type {

   int s_fr;
   int s_ba;
   recoil_type recoil;
   decay_type decay[4];

};

struct pixel_type {

   int status;
   chain_type chain;

};

pixel_type dssd_corr[161][161];
pixel_type dssd_front[161];
pixel_type dssd_back[161];


/*struct event {

   focal_plane fplane;
   dssd_type dssd_ev[10];
  



};

event Total;*/


/*struct dssd_type {

   int type;
   int en;
   int ts;

};*/

