/*--------------------------*/
/* user initialization code */
/* (may be empty..........) */
/*--------------------------*/


TH1D *h1_hp;
h1_hp = mkTH1D((char *)"h1_hp",(char *)"h1_hp",1000,0,1000);

h1_trlen = mkTH1D((char *)"trlen",(char *)"trlen",4000,0,4000);

h1_t0 = mkTH1D((char *)"t0",(char *)"t0",1000,0,1000);
h1_t1 = mkTH1D((char *)"t1",(char *)"t1",1000,0,1000);
h1_dt = mkTH1D((char *)"dt",(char *)"dt",1000,0,1000);

h1_tr_hp = mkTH1D((char *)"tr_hp",(char *)"tr_hp",1000,0,1000);

h1_CurEvNo_DSSD_LEDts = mkTH1D((char *)"CurEvNo_DSSD_LEDts",(char *)"CurEvNo_DSSD_LEDts",1400000,0,1400000);
h1_CurEvNo_DSSD113_LEDts = mkTH1D((char *)"CurEvNo_DSSD113_LEDts",(char *)"CurEvNo_DSSD113_LEDts",1400000,0,1400000);
h2_CurEvNo_DSSD_LEDts = mkTH2F((char *)"CurEvNo_DSSD_LEDts2",(char *)"CurEvNo_DSSD_LEDts2",100000,0,100000,400,0,400);

h1_CurEvNo_XARRAY_LEDts = mkTH1D((char *)"CurEvNo_LEDts_XARRAY",(char *)"CurEvNo_LEDts_XARRAY",1400000,0,1400000);
h2_CurEvNo_XARRAY_LEDts = mkTH2F((char *)"CurEvNo_LEDts_XARRAY2",(char *)"CurEvNo_LEDts_XARRAY2",100000,0,100000,20,0,20);

h1_CurEvNo_FP_LEDts = mkTH1D((char *)"CurEvNo_LEDts_FP",(char *)"CurEvNo_LEDts_FP",1400000,0,1400000);
h2_CurEvNo_FP_LEDts = mkTH2F((char *)"CurEvNo_LEDts_FP2",(char *)"CurEvNo_LEDts_FP2",100000,0,100000,10,0,10);


// DSSD histograms


//h2_backside_traces_fr = mkTH2F((char *)"backside_traces_fr",(char *)"backside_traces_fr",1000,0,1000,1000,0,1000);
//h2_backside_traces_ba = mkTH2F((char *)"backside_traces_ba",(char *)"backside_traces_ba",1000,0,1000,1000,0,1000);
//h2_frontside_traces_fr = mkTH2F((char *)"frontside_traces_fr",(char *)"backside_traces_fr",1000,0,1000,1000,0,1000);
//h2_frontside_traces_ba = mkTH2F((char *)"frontside_traces_ba",(char *)"frontside_traces_ba",1000,0,1000,1000,0,1000);

h1_dssd_dT140 = mkTH1D((char *)"dssd_dT140",(char *)"dssd_dT140",100000,0,1000000);
h1_dssd_dT240 = mkTH1D((char *)"dssd_dT240",(char *)"dssd_dT240",100000,0,1000000);

h2_dssd_dTE140 = mkTH2F((char *)"dssd_dTE140",(char *)"dssd_dTE140",1000,0,100000,1000,0,100000);
h2_dssd_dTE240 = mkTH2F((char *)"dssd_dTE240",(char *)"dssd_dTE240",1000,0,100000,1000,0,100000);


h1_d_fr_emax = mkTH1D((char *)"d_fr_emax",(char *)"d_fr_emax",1000,0,100000);
h1_d_ba_emax = mkTH1D((char *)"d_ba_emax",(char *)"d_ba_emax",1000,0,100000);

h2_ehi_trE = mkTH2F((char *)"ehi_trE",(char *)"ehi_trE",2000,0,2000,10000,0,10000);

for (iQ = 0; iQ < NFS; iQ++ ){
 sprintf(gStr1,"h2_ehi_trapV_%i",iQ+1);
 h2_ehi_trapV[iQ] = mkTH2F(gStr1,gStr1,4000,0,4000,4000,0,4000);

 sprintf(gStr1,"h1_ehi_%i",iQ+1);
 h1_ehi[iQ] = mkTH1D(gStr1,gStr1,2000,200,2200);

 sprintf(gStr1,"h1_trapV_%i",iQ+1);
 h1_trapV[iQ] = mkTH1D(gStr1,gStr1,3200,300,3500);
}

h2_ge = mkTH2F((char *)"ge",(char *)"ge",10000,0,10000,200,0,200);
h2_genocalib = mkTH2F((char *)"genocalib",(char *)"genocalib",10000,0,100000,200,0,200);

h1_decay_generations = mkTH1D((char *)"decay_generations",(char *)"decay_generations",10,0,10);

h2_dssd_frbase_raw = mkTH2F((char *)"dssd_frbase_raw",(char *)"dssd_frbase_raw",2000,-10000,100000,500,0,500);
h2_dssd_babase_raw = mkTH2F((char *)"dssd_babase_raw",(char *)"dssd_babase_raw",2000,-10000,100000,500,0,500);

h2_fast_bl_fr = mkTH2F((char *)"fast_bl_fr",(char *)"fast_bl_fr",2000,-10000,10000,500,0,500);
h2_fast_bl_ba = mkTH2F((char *)"fast_bl_ba",(char *)"fast_bl_ba",2000,-10000,10000,500,0,500);


h1_decay_ediff = mkTH1D((char *)"decay_ediff",(char *)"decay_ediff",20000,-100000,100000);

h2_dssd_badfr = mkTH2F((char *)"dssd_badfr",(char *)"dssd_badfr",5000,0,5000,200,0,200);
h2_dssd_badba = mkTH2F((char *)"dssd_badba",(char *)"dssd_badba",5000,0,5000,200,0,200);

h1_fplane_stats = mkTH1D((char *)"fplane_stats",(char *)"fplane_stats",30,0,30);

h1_pu = mkTH1D((char *)"pu",(char *)"pu",30,0,30);
h1_pu_daq = mkTH1D((char *)"pu_daq",(char *)"pu_daq",30,0,30);

//h2_badtrace = mkTH2F((char *)"badtrace",(char *)"badtrace",1000,0,1000,1000,0,1000);

//h2_levsid = mkTH2F((char *)"levsid",(char *)"levsid",1000,0,1000,1000,0,1000);
//h2_recoil_pu_traces = mkTH2F((char *)"recoil_pu_traces",(char *)"recoil_pu_traces",1000,0,1000,1000,0,1000);
h2_recoil_pu_traces_fr = mkTH2F((char *)"recoil_pu_traces_fr",(char *)"recoil_pu_traces_fr",1000,0,1000,1000,0,1000);
h2_recoil_pu_traces_ba = mkTH2F((char *)"recoil_pu_traces_ba",(char *)"recoil_pu_traces_ba",1000,0,1000,1000,0,1000);
h2_dec_pu_traces_fr = mkTH2F((char *)"dec_pu_traces_fr",(char *)"dec_pu_traces_fr",1000,0,1000,1000,0,1000);
h2_dec_pu_traces_ba = mkTH2F((char *)"dec_pu_traces_ba",(char *)"dec_pu_traces_ba",1000,0,1000,1000,0,1000);
h2_fast_traces_fr = mkTH2F((char *)"fast_traces_fr",(char *)"fast_traces_fr",1000,0,1000,1000,0,1000);
h2_fast_traces_ba = mkTH2F((char *)"fast_traces_ba",(char *)"fast_traces_ba",1000,0,1000,1000,0,1000);
h2_dec1pu_corr_fr_traces =  mkTH2F((char *)"dec1pu_corr_fr_traces",(char *)"dec1pu_corr_fr_traces",1000,0,1000,1000,0,1000);
h2_dec1pu_corr_ba_traces =  mkTH2F((char *)"dec1pu_corr_ba_traces",(char *)"dec1pu_corr_ba_traces",1000,0,1000,1000,0,1000);
h2_dec2pu_corr_fr_traces =  mkTH2F((char *)"dec2pu_corr_fr_traces",(char *)"dec2pu_corr_fr_traces",1000,0,1000,1000,0,1000);
h2_dec2pu_corr_ba_traces =  mkTH2F((char *)"dec2pu_corr_ba_traces",(char *)"dec2pu_corr_ba_traces",1000,0,1000,1000,0,1000);

h2_pu_traces =  mkTH2F((char *)"putr",(char *)"putr",1000,0,1000,1000,0,1000);
h2_pud1_traces =  mkTH2F((char *)"pud1tr",(char *)"pud1tr",1000,0,1000,1000,0,1000);
h2_pud2_traces =  mkTH2F((char *)"pud2tr",(char *)"pud2tr",1000,0,1000,1000,0,1000);

//h2_recoil_corr_traces = mkTH2F((char *)"recoil_corr_traces",(char *)"recoil_corr_traces",100,0,100,1000,0,1000);
//h2_recoil_pu_traces_gated = mkTH2F((char *)"recoil_pu_traces_gated",(char *)"recoil_pu_traces_gated",1000,0,1000,1000,0,1000);
//h2_decay_pu_traces = mkTH2F((char *)"decay_pu_traces",(char *)"decay_pu_traces",1000,0,1000,1000,0,1000);

//h2_d_fr_traces_hie = mkTH2F((char *)"d_fr_traces_hie",(char *)"d_fr_traces_hie",1000,0,1000,1000,0,1000);
//h2_d_ba_traces_hie = mkTH2F((char *)"d_ba_traces_hie",(char *)"d_ba_traces_hie",1000,0,1000,1000,0,1000);

//h2_test_traces = mkTH2F((char *)"test_traces",(char *)"test_traces",1000,0,1000,1000,0,1000);


//si_trV = mkTH1D((char *)"testV",(char *)"testV",1000,0,1000);


h2_dssd_traces = mkTH2F((char *)"dssd_traces",(char *)"dssd_traces",1000,0,1000,1000,0,1000);
h2_dssd_traces_smooth = mkTH2F((char *)"dssd_traces_smooth",(char *)"dssd_traces_smooth",1000,0,1000,1000,0,1000);
h2_dssd_traces_le = mkTH2F((char *)"dssd_traces_le",(char *)"dssd_traces_le",1000,0,1000,1000,0,1000);
h2_dssd_traces_trapez = mkTH2F((char *)"dssd_traces_trapez",(char *)"dssd_traces_trapez",1000,0,1000,1000,0,1000);
h2_dssd_traces_delay = mkTH2F((char *)"dssd_traces_delay",(char *)"dssd_traces_delay",1000,0,1000,1000,0,1000);

h2_dssd_traces_raw = mkTH2F((char *)"dssd_traces_raw",(char *)"dssd_traces_raw",1000,0,1000,1000,0,1000);

//h2_dssd_Vtrap_tlinear = mkTH2F((char *)"dssd_V_vs_t",(char *)"dssd_V_vs_t",1000,-500,500,3000,0,3000);

for (iQ = 0; iQ < NX3FS; iQ++ ){
  sprintf(gStr1,"h2_V_vs_t%i",iQ+1);
  h2_dssd_Vtrap_tlinear[iQ] = mkTH2F(gStr1,gStr1,1600,-800,800,8000,0,4000);
}

/*
for (iQ = 0; iQ < 180; iQ++ ) {
  sprintf(gStr1,"ch_%03i_trace",iQ);
  h2_all_tr[iQ] = mkTH2F(gStr1,gStr1,1000,0,1000,1000,0,1000);

 }
*/
h2_dssd_deriv = mkTH2F((char *)"dssd_deriv",(char *)"dssd_deriv",1000,0,1000,1000,0,1000);
h2_dssd_derivt = mkTH2F((char *)"dssd_derivt",(char *)"dssd_derivt",1000,0,1000,1000,0,1000);

h1_dssd_traces_2d = mkTH1D((char *)"dssd_2d",(char *)"dssd_2d",1000,0,1000);

//h2_recoil_traces = mkTH2F((char *)"recoil_traces",(char *)"recoil_traces",1000,0,1000,100,0,100);
//h2_decay_traces = mkTH2F((char *)"decay_traces",(char *)"decay_traces",1000,0,1000,100,0,100);

//h1_ge_CFDs = mkTH1D((char *)"ge_CFDs",(char *)"ge_CFDs",2000,-1000,1000);

sprintf(str1,"dssd_fr_rawe");
sprintf(str2,"dssd_fr_rawe");
h2_dssd_fr_rawe = mkTH2F(str1,str2,10000,0,10000,500,0,500);

sprintf(str1,"dssd_ba_rawe");
sprintf(str2,"dssd_ba_rawe");
h2_dssd_ba_rawe = mkTH2F(str1,str2,10000,0,10000,500,0,500);

sprintf(str1,"dssd_en_raw");
sprintf(str2,"dssd_en_raw");
h2_dssd_en_raw = mkTH2F(str1,str2,10000,0,10000,500,0,500);

sprintf(str1,"r_hitxy");
sprintf(str2,"r_hitxy");
h2_r_hitxy = mkTH2F(str1,str2,200,0,200,200,0,200);

sprintf(str1,"r_fr_e");
sprintf(str2,"r_fr_e");
h2_r_fr_e = mkTH2F(str1,str2,1000,0,10000,200,0,200);

sprintf(str1,"r_ba_e");
sprintf(str2,"r_ba_e");
h2_r_ba_e = mkTH2F(str1,str2,1000,0,10000,200,0,200);

h2_r_fr_all = mkTH2F((char *)"r_fr_all",(char *)"r_fr_all",1000,0,10000,200,0,200);
h2_r_ba_all = mkTH2F((char *)"r_ba_all",(char *)"r_ba_all",1000,0,10000,200,0,200);

h1_recrate = mkTH1D((char *)"recrate",(char *)"recrate",500000,0,500000);
h1_recrate_ch = mkTH1D((char *)"recrate_ch",(char *)"recrate_ch",500000,0,500000);
h1_decrate = mkTH1D((char *)"decrate",(char *)"decrate",500000,0,500000);
h1_dssdrate_ch1 = mkTH1D((char *)"dssdrate_ch1",(char *)"dssdrate_ch1",500000,0,500000);
h1_dssdrate_ch2 = mkTH1D((char *)"dssdrate_ch2",(char *)"dssdrate_ch2",500000,0,500000);
h1_fprate = mkTH1D((char *)"fprate",(char *)"fprate",500000,0,500000);

h2_d_hitxy = mkTH2F((char *)"d_hitxy",(char *)"d_hitxy",200,0,200,200,0,200);
h2_d_fr_e = mkTH2F((char *)"d_fr_e",(char *)"d_fr_e",1000,0,10000,200,0,200);
h2_d_ba_e = mkTH2F((char *)"d_ba_e",(char *)"d_ba_e",1000,0,10000,200,0,200);
h2_d_fr_all = mkTH2F((char *)"d_fr_all",(char *)"d_fr_all",1000,0,10000,200,0,200);
h2_d_ba_all = mkTH2F((char *)"d_ba_all",(char *)"d_ba_all",1000,0,10000,200,0,200);
h2_d_fb = mkTH2F((char *)"d_fb",(char *)"d_fb",1000,0,10000,1000,0,10000);


// Focal plane histograms
h2_LvR = mkTH2F((char *)"LvR",(char *)"Left versus right",2500,0,5000,2500,0,5000);
h2_Lvppacde = mkTH2F((char *)"Lvppacde",(char *)"Left versus ppacde",2500,0,5000,5000,0,10000);
h2_Rvppacde = mkTH2F((char *)"Rvppacde",(char *)"Right versus ppacde",2500,0,5000,5000,0,10000);
h2_Rvppacde_rec = mkTH2F((char *)"Rvppacderec",(char *)"Right versus ppacde",2500,0,5000,5000,0,10000);
h2_R_corr_decayen = mkTH2F((char *)"R_corr_decayen",(char *)"R_corr_decayen",2500,0,5000,5000,0,20000);
h2_R_corr_decayen_lowE = mkTH2F((char *)"R_corr_decayen_lowE",(char *)"R_corr_decayen_lowE",2500,0,5000,5000,0,20000);

h2_xe1 = mkTH2F((char *)"xe1",(char *)"xe1",2500,0,5000,5000,0,20000);
h2_xe1_short = mkTH2F((char *)"xe1_short",(char *)"xe1_short",2500,0,5000,5000,0,20000);

h1_rl = mkTH1D((char *)"rl",(char *)"right+left",2000,0,8000);
h1_x = mkTH1D((char *)"x",(char *)"left-right",2000,-2000,2000);
h2_x_icde = mkTH2F((char *)"x_icde",(char *)"x_icde",2000,-2000,2000,1000,0,20000);
h2_x_ppacde = mkTH2F((char *)"x_ppacde",(char *)"x_ppacde",2000,-2000,2000,5000,0,10000);
h1_l = mkTH1D((char *)"l",(char *)"l",2500,0,5000);
h1_r = mkTH1D((char *)"r",(char *)"r",2500,0,5000);
h1_icde = mkTH1D((char *)"icde",(char *)"icde",2500,0,5000);
h1_ppacde = mkTH1D((char *)"ppacde",(char *)"ppacde",5000,0,10000);
h2_ppacde_icde = mkTH2F((char *)"ppacde_icde",(char *)"ppacde_icde",5000,0,10000,2500,0,5000);

h2_LvR_raw = mkTH2F((char *)"LvR_raw",(char *)"Left versus right raw",2500,0,5000,2500,0,5000);
h2_Lvppacde_raw = mkTH2F((char *)"Lvppacde_raw",(char *)"Left versus ppacde raw",2500,0,5000,5000,0,10000);
h2_Rvppacde_raw = mkTH2F((char *)"Rvppacde_raw",(char *)"Right versus ppacde raw",2500,0,5000,5000,0,10000);
h1_rl_raw = mkTH1D((char *)"rl_raw",(char *)"right+left raw",2000,0,8000);
h1_x_raw = mkTH1D((char *)"x_raw",(char *)"left-right rwa",2000,-2000,2000);
h2_x_icde_raw = mkTH2F((char *)"x_icde_raw",(char *)"x_icde raw",2000,-2000,2000,1000,0,20000);
h2_x_ppacde_raw = mkTH2F((char *)"x_ppacde_raw",(char *)"x_ppacde raw",2000,-2000,2000,5000,0,10000);  //Increased ppac de range
h1_l_raw = mkTH1D((char *)"l_raw",(char *)"l raw",2500,0,5000);
h1_r_raw = mkTH1D((char *)"r_raw",(char *)"r raw",2500,0,5000);
h1_icde_raw = mkTH1D((char *)"icde_raw",(char *)"icde raw",2500,0,5000);
h1_ppacde_raw = mkTH1D((char *)"ppacde_raw",(char *)"ppacde raw",5000,0,10000);
h1_ppacde2_raw = mkTH1D((char *)"ppacde2_raw",(char *)"ppacde2 raw",5000,0,10000);

h2_recoilen_tdssdppac = mkTH2F((char *)"recoilen_tdssdppac",(char *)"recoilen_tdssdppac",10000,0,10000,4000,-2000,2000);
h2_recoilen_icde = mkTH2F((char *)"recoilen_icde",(char *)"recoilen_icde",1000,0,1000,2500,0,5000);
h2_recoilen_tdssdppac_gated = mkTH2F((char *)"recoilen_tdssdppac_gated",(char *)"recoilen_tdssdppac_gated",10000,0,10000,4000,-2000,2000);
h2_recoilen_tdssdppac_fission = mkTH2F((char *)"recoilen_tdssdppac_fission",(char *)"recoilen_tdssdppac_fission",10000,0,10000,4000,-2000,2000);
h2_recoilen_tdssdppac_electrons = mkTH2F((char *)"recoilen_tdssdppac_electrons",(char *)"recoilen_tdssdppac_electrons",10000,0,10000,4000,-2000,2000);

h2_tdssdppac_raw = mkTH2F((char *)"tdssdppac_raw",(char *)"tdssdppac_raw",4000,-2000,2000,500,0,500);
h1_tgppac = mkTH1D((char *)"tgppac",(char *)"tgppac",4000,-2000,2000);
h2_tgppac_all = mkTH2F((char *)"tgppac_all",(char *)"tgppac_all",4000,-2000,2000,150,0,150);
h2_tdssdppac_ba = mkTH2F((char *)"tdssdppac_ba",(char *)"tdssdppac_ba",4000,-2000,2000,200,0,200);
//h2_x_ge = mkTH2F((char *)"x_ge",(char *)"x_ge",2000,-1000,1000,2000,0,2000);
//h2_x_ge2 = mkTH2F((char *)"x_ge2",(char *)"x_ge2",2000,-1000,1000,2000,0,2000);
//h2_tid_ge = mkTH2F((char *)"tid_ge",(char *)"tid_ge",120,0,120,10000,0,10000);
//h2_tid_gebase = mkTH2F((char *)"tid_gebase",(char *)"tid_gebase",120,0,120,2000,-10000,10000);
h2_tid_base_test = mkTH2F((char *)"tid_base_test",(char *)"tid_gebase with min baseline",120,0,120,2000,-10000,10000);
//h2_baseline_ehi = mkTH2F((char *)"tid_ge",(char *)"tid_ge with baseline cut",120,0,120,10000,0,10000);

//h2_d58ppacde = mkTH2F((char *)"d58ppacde",(char *)"d58ppacde",4096,0,4096,2048,0,2048);
h1_nge = mkTH1D((char *)"nge",(char *)"nge",1000,0,1000);

//h2_bgo_baseline = mkTH2F((char *)"bgo_baseline",(char *)"bgo baseline",120,0,120,10000,-10000,10000);


// Correlation histograms
h2_e1t1log = mkTH2F((char *)"e1t1log",(char *)"e1t1log",10000,0,100000,1000,0,100);
h2_e1t1log_fr = mkTH2F((char *)"e1t1log_fr",(char *)"e1t1log_fr",10000,0,100000,1000,0,100);
h2_e1t1log_ba = mkTH2F((char *)"e1t1log_ba",(char *)"e1t1log_ba",10000,0,100000,1000,0,100);
h2_e1t1c1 = mkTH2F((char *)"e1t1c1",(char *)"e1t1c1",2000,0,20000,1000,0,1000);
h2_e1t1c2 = mkTH2F((char *)"e1t1c2",(char *)"e1t1c2",2000,0,20000,1000,0,1000);
h2_e1t1c3 = mkTH2F((char *)"e1t1c3",(char *)"e1t1c3",2000,0,20000,1000,0,1000);
h2_e1t1c4 = mkTH2F((char *)"e1t1c4",(char *)"e1t1c4",2000,0,20000,1000,0,1000);
h2_e1e2 = mkTH2F((char *)"e1e2",(char *)"e1e2",2000,0,20000,2000,0,20000);
h2_e1e2_short = mkTH2F((char *)"e1e2_short",(char *)"e1e2_short",2000,0,20000,2000,0,20000);
h2_e2e3 = mkTH2F((char *)"e2e3",(char *)"e2e3",2000,0,2000,20000,0,20000);
h2_e2e3_short = mkTH2F((char *)"e2e3_short",(char *)"e2e3_short",2000,0,20000,2000,0,20000);

// Correlation histograms
h2_e2t2log = mkTH2F((char *)"e2t2log",(char *)"e2t2log",2000,0,2000,1000,0,100);
h2_e2t2c1 = mkTH2F((char *)"e2t2c1",(char *)"e2t2c1",2000,0,2000,1000,0,1000);
h2_e2t2c2 = mkTH2F((char *)"e2t2c2",(char *)"e2t2c2",2000,0,2000,1000,0,1000);
h2_e2t2c3 = mkTH2F((char *)"e2t2c3",(char *)"e2t2c3",2000,0,2000,1000,0,1000);
h2_e2t2c4 = mkTH2F((char *)"e2t2c4",(char *)"e2t2c4",2000,0,2000,1000,0,1000);

// Correlation histograms
h2_e3t3log = mkTH2F((char *)"e3t3log",(char *)"e3t3log",2000,0,2000,1000,0,100);
h2_e3t3c1 = mkTH2F((char *)"e3t3c1",(char *)"e3t3c1",2000,0,2000,1000,0,1000);
h2_e3t3c2 = mkTH2F((char *)"e3t3c2",(char *)"e3t3c2",2000,0,2000,1000,0,1000);
h2_e3t3c3 = mkTH2F((char *)"e3t3c3",(char *)"e3t3c3",2000,0,2000,1000,0,1000);
h2_e3t3c4 = mkTH2F((char *)"e3t3c4",(char *)"e3t3c4",2000,0,2000,1000,0,1000);


// GODDESS

// implement the left and right side of one strip
sprintf(gStr1,"trVal%i",1);
sprintf(gStrB,"%s[trackTr]/F",gStr1);
GDST->Branch(gStr1,&trVal1,gStrB);


for (iQ = 0; iQ < NX3FS; iQ++ ){
  // histos
  sprintf(gStr1,"si_lr%i",iQ+1);
  si_lr[iQ] = mkTH2F(gStr1,gStr1,1000,0,3000,1000,0,3000);

  sprintf(gStr1,"si_ep%i",iQ+1);
  si_ep[iQ] = mkTH2F(gStr1,gStr1,1000,-1,1,1000,0,5000);

  sprintf(gStr1,"si_lr_offline%i",iQ+1);
  si_lr_offline[iQ] = mkTH2F(gStr1,gStr1,1000,0,5000,1000,0,5000);

  sprintf(gStr1,"si_ep_offline%i",iQ+1);
  si_ep_offline[iQ] = mkTH2F(gStr1,gStr1,1000,-1,1,1000,0,5000);

  if ( iQ < 16 ) {
    sprintf(gStr1,"si_tr%i",iQ+1);
    si_tr[iQ] = mkTH2F(gStr1,gStr1,1000,0,20000,1000,0,20000);
    sprintf(gStr1,"si_tl%i",iQ+1);
    si_tl[iQ] = mkTH2F(gStr1,gStr1,1000,0,20000,1000,0,20000);
  }
  /*
  // for trees
  // left - right branches
  sprintf(gStr1,"gds_%sV_%i","X",iQ);
  sprintf(gStrB,"%s/D",gStr1);
  //  GDST->Branch(gStr1,&tSiXV[iQ],gStrB);

  sprintf(gStr1,"gds_%sV_%i","Y",iQ);
  sprintf(gStrB,"%s/D",gStr1);
  //  GDST->Branch(gStr1,&tSiYV[iQ],gStrB);

  // energy - position branches

  sprintf(gStr1,"gds_%sV_%i","ene",iQ);
  sprintf(gStrB,"%s/D",gStr1);
  //  GDST->Branch(gStr1,&tSiEne[iQ],gStrB);

  sprintf(gStr1,"gds_%sV_%i","pos",iQ);
  sprintf(gStrB,"%s/D",gStr1);
  //  GDST->Branch(gStr1,&tSiPos[iQ],gStrB);

  // backsides of the SX3 detectors

  if ( iQ < 16 ) {
    sprintf(gStr1,"gds_%sV_%i","bks",iQ);
    sprintf(gStrB,"%s/D",gStr1);
    //    GDST->Branch(gStr1,&tSiBks[iQ],gStrB);
  }
  */
}

TH2F *trlhp;
trlhp = mkTH2F((char *)"trlhp",(char *)"trlhp",100,0,100,100,0,100);
TH2F *h2ffcm;
h2ffcm = mkTH2F((char *)"h2ffcm",(char *)"h2ffcm",161,0,161,161,0,161);

TH2F *h2allcm;
h2allcm = mkTH2F((char *)"h2allcm",(char *)"h2allcm",320,0,320,320,0,320);


#if(0)
sprintf(str1,"gam_rec");
sprintf(str2,"gam_rec");
h1_gam_rec = mkTH1D(str1,str2,10000,0,10000);


h1_gam_rec_dec = mkTH1D((char *)"gam_rec_dec",(char *)"gam_rec_dec",10000,0,10000);
h2_gam_decayen_corr = mkTH2F((char *)"gam_decayen_corr",(char *)"gam_decayen_corr",1000,0,10000,5000,0,10000);

h1_gam = mkTH1D((char *)"gam",(char *)"gam",10000,0,10000);


sprintf(str1,"dssd_en_fr");
sprintf(str2,"dssd_en_fr");
h2_dssd_en_fr = mkTH2F(str1,str2,20000,0,20000,500,0,500);

sprintf(str1,"dssd_en_ba");
sprintf(str2,"dssd_en_ba");
h2_dssd_en_ba = mkTH2F(str1,str2,20000,0,20000,500,0,500);

sprintf(str1,"dssd_en_imp");
sprintf(str2,"dssd_en_imp");
h2_dssd_en_imp = mkTH2F(str1,str2,20000,0,20000,500,0,500);

sprintf(str1,"dssd_en_rec");
sprintf(str2,"dssd_en_rec");
h2_dssd_en_rec = mkTH2F(str1,str2,20000,0,20000,500,0,500);

sprintf(str1,"dssd_en_dec");
sprintf(str2,"dssd_en_dec");
h2_dssd_en_dec = mkTH2F(str1,str2,20000,0,20000,500,0,500);

sprintf(str1,"timeid");
sprintf(str2,"timeid");
h2_timeid = mkTH2F(str1,str2,1000,0,1000,10000,0,10000);

sprintf(str1,"clen");
sprintf(str2,"clen");
h1_clen = mkTH1D(str1,str2,200,0,200);


sprintf(str1,"leddiff");
sprintf(str2,"leddiff");
h1_leddiff = mkTH1D(str1,str2,10000,-10000,10000);


sprintf(str1,"cfddiff");
sprintf(str2,"cfddiff");
h1_cfddiff = mkTH1D(str1,str2,10000,-10000,10000);


sprintf(str1,"id");
sprintf(str2,"id");
h1_id = mkTH1D(str1,str2,10000,0,10000);

sprintf(str1,"tid");
sprintf(str2,"tid");
h1_tid = mkTH1D(str1,str2,1000,0,1000);

#endif

/*------------------------------------------------------*/
// Ge Calibration Stuff

float ehigain[NGE + 1], ehioffset[NGE + 1], ehi_base[NGE + 1], rr1, r2, r3, r4; 
FILE *fp1;
int i2, i3, i4;

 for (i = 0; i <= NGE; i++)
    {
      ehigain[i] = 1;
      ehioffset[i] = 0;
    };

  //fp1 = fopen ("./DGSSort/ehi2.cal", "r");
    fp1 = fopen ("MAP_FILES/ge1_test.map","r");
  if (fp1 != NULL)
    {
      printf ("found ehi.cal file\n");
//      getline(str,132,fp1);
//      fscanf (fp1, "%s\n", str);
//      fscanf (fp1, "%s\n", str);
//      fscanf (fp1, "%s\n", str);
//      fscanf (fp1, "%s\n", str);
      i2 = fscanf (fp1, "%d %d %f %f %d", &i1, &i3, &rr1, &r2, &i4);
      printf ("i2=%i\n", i2);
      while (i2 == 5)
        {
          if (i1 <= NGE && i1 > 0)
            {
              ehigain[i1] = r2/10;
              ehioffset[i1] = rr1;
	      ehi_base[i1] = i4;
              printf ("ge %3i, offset=%6.2f gain=%9.4f, baseline=%9.4f \n", i1, ehioffset[i1], ehigain[i1], ehi_base[i1]);
            };
          i2 = fscanf (fp1, "%d %d %f %f %d", &i1, &i3, &rr1, &r2, &i4);

        }
      fclose (fp1);
    }


/*-------------------------------------------------------*/






// MAPFILES FOR DSSD

FILE *fmap1;
char fmapname1[32];

int strip, thr, baseline;
float off, gain;

struct strip_type {
   int phystrip;
   int thr;
   float off;
   float gain;
   int baseline;
};


struct strip_type map_fr[321];
struct strip_type map_ba[321];

sprintf(fmapname1,"MAP_FILES/dssd_fr_test.map");
//sprintf(fmapname1,"MAP_FILES/dssd_fr_zero.map");
//sprintf(fmapname1,"MAP_FILES/dssd_fr_Sncalib_new.map");
//sprintf(fmapname1,"MAP_FILES/dssd_fr_Sncalib.map");
fmap1=fopen(fmapname1,"read");

if(fmap1==0) {
printf("Failed to read front mapfile...");

for(i=0;i<160;i++){

   //fscanf(fmap1,"%d %d %f %f %i", &strip, &thr, &off, &gain, &baseline);
   map_fr[i+1].thr = 0;
   map_fr[i+1].off = 0;
   map_fr[i+1].gain = 1; 
   map_fr[i+1].baseline = 0;
   
   //printf("map front strip %d has thr %d, offset %f and gain %f and baseline %i\n",strip,thr,off,gain,baseline);

}

}
if(fmap1!=0) 
{
printf("Read front mapfile ok... ");

for(i=0;i<161;i++){

   fscanf(fmap1,"%d %d %f %f %i", &strip, &thr, &off, &gain, &baseline);
   map_fr[strip].thr = thr;
   map_fr[strip].off = off;
   map_fr[strip].gain = gain; 
   map_fr[strip].baseline = baseline;
   
   printf("map front strip %d has thr %d, offset %f and gain %f and baseline %i\n",strip,thr,off,gain,baseline);

}

fclose(fmap1);

}



//sprintf(fmapname1,"MAP_FILES/dssd_ba_zero.map");
sprintf(fmapname1,"MAP_FILES/dssd_ba_test.map");
//sprintf(fmapname1,"MAP_FILES/dssd_ba_Sncalib_new.map");
//sprintf(fmapname1,"MAP_FILES/dssd_ba_Sncalib.map");

fmap1=fopen(fmapname1,"read");

if(fmap1==0) {
printf("Failed to read back mapfile...");

for(i=0;i<160;i++){

   //fscanf(fmap1,"%d %d %f %f %i", &strip, &thr, &off, &gain, &baseline);
   map_ba[i+1].thr = 0;
   map_ba[i+1].off = 0;
   map_ba[i+1].gain = 1; 
   map_ba[i+1].baseline = 0;
   
   //printf("map back strip %d has thr %d, offset %f and gain %f and baseline %i\n",strip,thr,off,gain,baseline);

}

}

if(fmap1!=0) 
{
printf("Read back mapfile ok... ");

for(i=0;i<161;i++){

   fscanf(fmap1,"%d %d %f %f %i", &strip, &thr, &off, &gain, &baseline);
   map_ba[strip].thr = thr;
   map_ba[strip].off = off;
   map_ba[strip].gain = gain; 
   map_ba[strip].baseline = baseline;

  printf("map back strip %d has thr %d, offset %f and gain %f and baseline %i\n",strip,thr,off,gain,baseline);

}

fclose(fmap1);

}




// Recoil en vs tof graphical cut

TFile *cutfile = new TFile ("mycut.root","read");
TCutG *impwin;
impwin = (TCutG *) cutfile->Get("cut1");






