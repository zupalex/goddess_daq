
/*-----------------*/
/* user event code */
/* (may be empty.) */
/*-----------------*/

// For new data
#define LEFT 1
#define RIGHT 2
#define ICDE 3
#define PPACDE 4
#define PPACDE2 5		// ONLY FOR RUN 87!!!!!
// For old data
//#define LEFT 3
//#define RIGHT 7
//#define ICDE 9


if(t_first < 0) t_first = CEvent.DGSEvent[0].LEDts;

// GS ENERGY CALIBRATION 

  unsigned short int GSMAP[500]={0};
  unsigned short int MODMAP[20]={0};

#include "GSMAP.h"
#include "GSII_angles.h"

double e, r;
int orig;

for (i = 0; i < CEvent.len; i++)
    {
     if (CEvent.DGSEvent[i].tpe==GE)  {
       

       r = (float) rand() / RAND_MAX - 0.5;
       e= CEvent.DGSEvent[i].ehi;
       orig = CEvent.DGSEvent[i].ehi;

  
       //e =e*ehigain[GSMAP[CEvent.DGSEvent[i].id -1000] ] + ehioffset[GSMAP[CEvent.DGSEvent[i].id -1000] ] + r;
       e =e*ehigain[CEvent.DGSEvent[i].tid] + ehioffset[CEvent.DGSEvent[i].tid] + r;
       if (e  < L14BITS)
	CEvent.DGSEvent[i].ehi  = (int) (e + 0.5);
       else
	 CEvent.DGSEvent[i].ehi =  INT_MAX;

       h2_genocalib->Fill(orig,CEvent.DGSEvent[i].tid);       
       h2_ge->Fill(e,CEvent.DGSEvent[i].tid);

     }

    }

// DOPPLER CORRECTION 

SetBeta();

for (i = 0; i < CEvent.len; i++)
    {
     if (CEvent.DGSEvent[i].tpe==GE)  {
       r = (float) rand() / RAND_MAX - 0.5;
       e= CEvent.DGSEvent[i].ehi;
       //e =e*DopCorFac[GSMAP[CEvent.DGSEvent[i].tid]] + r;
       e =e*DopCorFac[GSMAP[CEvent.DGSEvent[i].id -1000]] + r;
       if (e  < L14BITS)
	CEvent.DGSEvent[i].ehi  = (int) (e + 0.5);
       else
	CEvent.DGSEvent[i].ehi =  INT_MAX;
     }
    }

// ---------------------------------- //
// TRACE ANALYSIS
// including
// Pile up detection...

int m, k, lethr, baseoff, t0, t1;
// if these m an k are the same as on the digitizer we need to vary them each time.

m = 100;
k = 50;
lethr = 20; 
//baseoff = 6500;
baseoff = 0;

int trE, trhiE;

int trlen;

int smooth[MAXTRACELEN];
int le[MAXTRACELEN];
int trapez[MAXTRACELEN];
int pu[MAXTRACELEN];
int pud1[MAXTRACELEN];
int pud2[MAXTRACELEN];
int bsTest = 0;

for(i=0;i<MAXTRACELEN;i++){
   smooth[i] = 0;
   le[i] = 0;
   trapez[i] = 0;
}

  bsTest = 0;

for(j=0;j<CEvent.len;j++){ // trace loop

   trE = 0;
   trhiE = 0;
   trlen = 0;
   t0 = 0;
   t1 = 0;

   //PU h1_pu_daq->Fill(CEvent.DGSEvent[j].pu);

   //PU CEvent.DGSEvent[j].pu = 0;

  if(CEvent.DGSEvent[j].tpe == DSSD && CEvent.DGSEvent[j].tid == 74 ){
    bsTest = 1;
      // store trace
      //if ((CEvent.DGSEvent[j].tid>10)&&(CEvent.DGSEvent[j].tid<20)) {
      //if ((trmax-trmin)>0) {
      Trdisp++;
      //printf ("Trdisp %3i\n", Trdisp)
      if (Trdisp<NumTr) {
         for (i=0;i<CEvent.DGSEvent[j].traceLen;i++) h2_dssd_traces_raw->Fill(Trdisp,i,CEvent.DGSEvent[j].trace[i]);
      }
      //}

  }

    if (Trdisp<NumTr) {
      if ( CEvent.DGSEvent[j].tid > 72 && CEvent.DGSEvent[j].tid < 74 && bsTest ) {
	for ( i = 0; i < CEvent.DGSEvent[j].traceLen; i++ ) {
	  h2_dssd_traces_bs_raw->Fill(Trdisp,i,CEvent.DGSEvent[j].trace[i]);
	}
      }
    }




   if(CEvent.DGSEvent[j].tpe == 5){ // only DSSD

      //if (((Tr==0)||Tr==1)) Trdisp=0;

      Tr++;

      int trmin, trmax;
      trmin=0; trmax=0;
   
      trlen = CEvent.DGSEvent[j].traceLen;
      h1_trlen->Fill(trlen); 
      // find trace max and min
      for (i=0;i<trlen;i++) {
        if (CEvent.DGSEvent[j].trace[i]<trmin) trmin=CEvent.DGSEvent[j].trace[i];
        if (CEvent.DGSEvent[j].trace[i]>trmax) trmax=CEvent.DGSEvent[j].trace[i];
      }
     
/*
      // store trace
      //if ((CEvent.DGSEvent[j].tid>10)&&(CEvent.DGSEvent[j].tid<20)) {
      //if ((trmax-trmin)>0) {
      Trdisp++;
      //printf ("Trdisp %3i\n", Trdisp);
      if (Trdisp<NumTr) {
         for (i=0;i<trlen;i++) h2_dssd_traces_raw->Fill(Trdisp,i,CEvent.DGSEvent[j].trace[i]);
      }
      //}
*/

      // flip negative traces and add baseline offset...
      if(CEvent.DGSEvent[j].tid > 160){
         for (i=0;i<trlen;i++) {
            CEvent.DGSEvent[j].trace[i]=-CEvent.DGSEvent[j].trace[i];
         }
      }
    
      // add offset
      for (i=0;i<trlen;i++) {
         CEvent.DGSEvent[j].trace[i]=CEvent.DGSEvent[j].trace[i]+baseoff;
      }

      //fake pileup trace
            for (i=5;i<trlen;i++) {
	      pu[i]=CEvent.DGSEvent[j].trace[i]+CEvent.DGSEvent[j].trace[i-5];
      }

            for (i=6;i<trlen;i++) {
	      pud1[i]=pu[i]-pu[i-1];
      }

               for (i=7;i<trlen;i++) {
	      pud2[i]=pud1[i]-pud1[i-1];
      }

     if(Tr<NumTr){
        for(i=0;i<trlen;i++) h2_pu_traces->Fill(Tr,i,pu[i]);
        for(i=0;i<trlen;i++) h2_pud1_traces->Fill(Tr,i,pud1[i]);
        for(i=0;i<trlen;i++) h2_pud2_traces->Fill(Tr,i,pud2[i]);
      }
      

      // TESTING PILEUP DETECTION...
      
//      if(CurEvNo == 23 || CurEvNo == 51 || CurEvNo == 188){
//         for(i=430;i<trlen;i++) CEvent.DGSEvent[j].trace[i] = CEvent.DGSEvent[j].trace[i] + 1000;
//      }
      
     
      // store processed traces
      if(Tr<NumTr){
         for(i=0;i<trlen;i++) h2_dssd_traces->Fill(Tr,i,CEvent.DGSEvent[j].trace[i]);
      }

      // trace smoothing
      for (i=8;i<trlen;i++) {
	smooth[i]=(CEvent.DGSEvent[j].trace[i]+8*CEvent.DGSEvent[j].trace[i-1]+28*CEvent.DGSEvent[j].trace[i-2]+56*CEvent.DGSEvent[j].trace[i-3]+
		   +70*CEvent.DGSEvent[j].trace[i-4]+56*CEvent.DGSEvent[j].trace[i-5]+28*CEvent.DGSEvent[j].trace[i-6]+8*CEvent.DGSEvent[j].trace[i-7]+CEvent.DGSEvent[j].trace[i-8])/256;
      }

   // save  smooth filter output

     if(Tr<NumTr){
         for(i=8;i<trlen;i++) h2_dssd_traces_smooth->Fill(Tr,i,smooth[i]);
      }
 
      // LE filter
      for (i=k;i<trlen;i++) {  
         le[i]=smooth[i]-smooth[i-k];
         if(Tr<NumTr)// h2_levsid->Fill(Tr,i,le[i]);
         if ((le[i]>lethr)&&(le[i-1]<lethr)&&(t0>0)&&(t1==0)) {
            t1=i;
            h1_t1->Fill(t1);
         }
         if ((le[i]>lethr)&&(le[i-1]<lethr)&&(t0==0)) {
            t0=i;
            h1_t0->Fill(t0);
         }
      }

   // save  LE filter output

    if(Tr<NumTr){
         for(i=k;i<trlen;i++) h2_dssd_traces_le->Fill(Tr,i,le[i]);
      }

    
    // apply trapezoidal filter

    for(i=m+k+m;i<trlen;i++){
      trapez[i] = trapez[i-1] + (CEvent.DGSEvent[j].trace[i]-CEvent.DGSEvent[j].trace[i-m])-(CEvent.DGSEvent[j].trace[i-m-k]-CEvent.DGSEvent[j].trace[i-m-k-m]);
      }

    // save  trapezoidal filter output
   if(Tr<NumTr){
         for(i=m+k+m;i<trlen;i++) h2_dssd_traces_trapez->Fill(Tr,i,trapez[i]);
      }


      // Calculating energy from trace...
      for(i=200;i<300;i++){
        trE = trE + (CEvent.DGSEvent[j].trace[i+300])-(CEvent.DGSEvent[j].trace[i]);
      }

      //CEvent.DGSEvent[j].trhiE = 100*(CEvent.DGSEvent[j].trace[950] - CEvent.DGSEvent[j].trace[50])/80;
      //CEvent.DGSEvent[j].trhiE = trE/100.0;


 
      // SWITCH ENERGIES
      //CEvent.DGSEvent[j].ehi = trE/100;
     // ZERO ENERGIES if dealing with negative slope (PU)
      //if (trE<0) CEvent.DGSEvent[j].ehi = 0;

   }

   h1_dt->Fill(t1-t0);

   //PU if(t0 != 0 && t1 != 0 && (t1-t0>100))  CEvent.DGSEvent[j].pu = 1;


// Comparing energy extracted from trace to energy reported by DAQ...
   if(CEvent.DGSEvent[j].tpe == 5){
      h2_ehi_trE->Fill(CEvent.DGSEvent[j].ehi,(float(trE)/80));
   }



   if(CEvent.DGSEvent[j].ehi < 400 && trE > 1000){
      trcurr++;
      if(trcurr<trmax){
         //for(i=0;i<CEvent.DGSEvent[j].traceLen;i++) h2_test_traces->Fill(trcurr,i,CEvent.DGSEvent[j].trace[i]);
      }
   }
   
 } // end of trace loop




//------------------------------------------
// Variables for event loops
//------------------------------------------

long long int t_add;
t_add = 0;
fplane.left = 0;
fplane.right = 0;
fplane.icde = 0;
fplane.left_ts = 0;
fplane.right_ts = 0;
fplane.icde_ts = 0;
fplane.ppacde = 0;
fplane.ppacde_ts = 0;
recoil.en = 0;
recoil.ts = 0;
//recoil.nge = 0;
decay.ts = 0;
decay.time = 0;
decay.en = 0;
/*
for(i=0;i<15;i++){
   recoil.ehi[i] = 0;
   recoil.tge[i] = 0;
   recoil.getid[i] = 0;
}
*/

// Multiplicities

int mult_dssdfr, mult_dssdba;
mult_dssdfr = 0;
mult_dssdba = 0;

int icde, left, right, x, icde_min, icde_max, tgppac_min, tgppac_max;
icde = 0;
left = 0;
right = 0;
x = 0;
icde_min = 8400;
icde_max = 9000;
tgppac_min = 100;
tgppac_max = 140;

unsigned long long int recoil_win;
recoil_win = 600;   // Maximum TOF between PPAC-left and implant.

long int d_fr_emax, d_ba_emax;
int s_d_fr, s_d_ba;
long int r_fr_emax, r_ba_emax;
int  s_r_fr, s_r_ba;
int r_ba_subev, r_fr_subev, d_ba_subev, d_fr_subev;
unsigned long long int r_fr_ts, d_fr_ts, r_ba_ts, d_ba_ts;
int rec_PU, dec_PU, d_fr_PU, d_ba_PU, r_fr_PU, r_ba_PU; 

d_fr_PU=0;
d_ba_PU=0;
r_fr_PU=0;
r_ba_PU=0;
rec_PU=0;
dec_PU=0;
d_fr_emax = 0;
d_ba_emax = 0;
s_d_fr = 0;
s_d_ba = 0;
r_fr_emax = 0;
r_ba_emax = 0;
s_r_fr = 0;
s_r_ba = 0;
r_fr_ts = 0;
d_fr_ts = 0;
r_ba_ts = 0;
d_ba_ts = 0;
r_ba_subev = 0;
r_fr_subev = 0;
d_ba_subev = 0;
d_fr_subev = 0;

int nge;
nge = 0;


//******************************************
// Primary subevent loop
//******************************************

signed long long int fr_ts, ba_ts;
fr_ts = 0;
ba_ts = 0;

int hit;
hit = 0;


// GODDESS

int dssd_fr[161], dssd_ba[161];


for (i=0;i<160;i++) {
  dssd_fr[i]=0;
  dssd_ba[i]=0;
}

for (i=0;i<CEvent.len;i++) {

 

  switch (CEvent.DGSEvent[i].tpe) {
 
    //-------------\\
    // GERMANIUMS..\\
    //-------------\\

    case GE:

    break;


    //-------------\\
    // DSSD........\\
    //-------------\\

    case DSSD:       

          //if(CEvent.DGSEvent[i].tid == 310){// && hit == 0){

              h2_CurEvNo_DSSD_LEDts->Fill(CurEvNo,CEvent.DGSEvent[i].tid,CEvent.DGSEvent[i].LEDts - t_first);
              h1_CurEvNo_DSSD_LEDts->Fill(CEvent.DGSEvent[i].LEDts - t_DSSD_last);
              
              if (CEvent.DGSEvent[i].tid==113) {
               h1_CurEvNo_DSSD113_LEDts->Fill(CEvent.DGSEvent[i].LEDts - t_DSSD113_last);
               t_DSSD113_last=CEvent.DGSEvent[i].LEDts;
              }
              
              t_DSSD_last=CEvent.DGSEvent[i].LEDts;
              hit = 1;
           //}  

      switch (CEvent.DGSEvent[i].tid) {


        case 1 ... 160:

        //--------------\\
        // front strips.\\
        //--------------\\

           mult_dssdfr++;
           h2_dssd_fr_rawe->Fill(CEvent.DGSEvent[i].ehi,CEvent.DGSEvent[i].tid);
//           h2_dssd_en_raw->Fill(CEvent.DGSEvent[i].ehi/2,CEvent.DGSEvent[i].tid);
//           h2_dssd_en_raw->Fill(CEvent.DGSEvent[i].ehi/4,CEvent.DGSEvent[i].tid);	
           h2_dssd_en_raw->Fill(CEvent.DGSEvent[i].ehi,CEvent.DGSEvent[i].tid);	
           h2_dssd_frbase_raw->Fill(CEvent.DGSEvent[i].baseline,CEvent.DGSEvent[i].tid);
           fr_ts = CEvent.DGSEvent[i].LEDts;
           
//           if (CEvent.DGSEvent[i].tid<160) dssd_fr[CEvent.DGSEvent[i].tid-1]=CEvent.DGSEvent[i].ehi/2;
//           if (CEvent.DGSEvent[i].tid<160) dssd_fr[CEvent.DGSEvent[i].tid-1]=CEvent.DGSEvent[i].ehi/4;
           if (CEvent.DGSEvent[i].tid<160) dssd_fr[CEvent.DGSEvent[i].tid-1]=CEvent.DGSEvent[i].ehi;

        break;

        case 161 ... 320:

        //--------------\\
        // back strips..\\
        //--------------\\

	   mult_dssdba++;
           h2_dssd_ba_rawe->Fill(CEvent.DGSEvent[i].ehi,CEvent.DGSEvent[i].tid-160);
//           h2_dssd_en_raw->Fill(CEvent.DGSEvent[i].ehi/2,CEvent.DGSEvent[i].tid);
//           h2_dssd_en_raw->Fill(CEvent.DGSEvent[i].ehi/4,CEvent.DGSEvent[i].tid);
           h2_dssd_en_raw->Fill(CEvent.DGSEvent[i].ehi,CEvent.DGSEvent[i].tid);
           h2_dssd_babase_raw->Fill(CEvent.DGSEvent[i].baseline,CEvent.DGSEvent[i].tid-160);
           ba_ts = CEvent.DGSEvent[i].LEDts;

//           if (CEvent.DGSEvent[i].tid-160<160) dssd_ba[CEvent.DGSEvent[i].tid-160-1]=CEvent.DGSEvent[i].ehi/2;
//           if (CEvent.DGSEvent[i].tid-160<160) dssd_ba[CEvent.DGSEvent[i].tid-160-1]=CEvent.DGSEvent[i].ehi/4;
           if (CEvent.DGSEvent[i].tid-160<160) dssd_ba[CEvent.DGSEvent[i].tid-160-1]=CEvent.DGSEvent[i].ehi;

         break;
         default:
         break;
      }
    break;

    //-------------\\
    // Focal Plane \\
    //-------------\\

    case FP:

      h2_CurEvNo_FP_LEDts->Fill(CurEvNo,CEvent.DGSEvent[i].tid,CEvent.DGSEvent[i].LEDts - t_first);
      h1_CurEvNo_FP_LEDts->Fill(CEvent.DGSEvent[i].LEDts - t_FP_last);
      t_FP_last=CEvent.DGSEvent[i].LEDts;

      switch (CEvent.DGSEvent[i].tid) {
        case LEFT:
          fplane.left = CEvent.DGSEvent[i].ehi/10.0;
          fplane.left_ts = CEvent.DGSEvent[i].LEDts;
        break;
        case RIGHT:
          fplane.right = CEvent.DGSEvent[i].ehi/10.0;
          fplane.right_ts = CEvent.DGSEvent[i].LEDts;
        break;
        case ICDE:
          fplane.icde = CEvent.DGSEvent[i].ehi;
          fplane.icde_ts = CEvent.DGSEvent[i].LEDts;
        break;
        case PPACDE:
          fplane.ppacde = CEvent.DGSEvent[i].ehi;
          fplane.ppacde_ts = CEvent.DGSEvent[i].LEDts;
        break;
        case PPACDE2:
          fplane.ppacde2 = CEvent.DGSEvent[i].ehi;
          fplane.ppacde2_ts = CEvent.DGSEvent[i].LEDts;
        default:
        break;
      }   
    break;

    case XARRAY:

     h2_CurEvNo_XARRAY_LEDts->Fill(CurEvNo,CEvent.DGSEvent[i].tid,CEvent.DGSEvent[i].LEDts - t_first);
     h1_CurEvNo_XARRAY_LEDts->Fill(CEvent.DGSEvent[i].LEDts - t_XARRAY_last);
     t_XARRAY_last=CEvent.DGSEvent[i].LEDts;

    default:
    break;
  }


 } // end of primary subevent loop


// GODDESS

  for(i = 0; i < 32; i++){
   si_lr[i]->Fill(dssd_fr[i*2+32],dssd_fr[i*2+33]);

   si_en_raw = (Float_t)(dssd_fr[i*2+32]+dssd_fr[i*2+33]);
   si_ps_raw = ((Float_t)(dssd_fr[i*2+32]-dssd_fr[i*2+33]))/si_en_raw;

   si_ep[i]->Fill(si_ps_raw,si_en_raw);
   
   // assign tree values
   tSiXV[i] = (Double_t)dssd_fr[i*2+32];
   tSiYV[i] = (Double_t)dssd_fr[i*2+33];
   
   tSiEne[i] = tSiXV[i] + tSiYV[i];
   tSiPos[i] = (tSiXV[i] - tSiYV[i])/tSiEne[i];
   
   if ( i < 16 ) {
     tSiBks[i] = (Double_t)dssd_ba[i];
   }

  }

// fill the tree
GDST->Fill();
// stop filling the tree

  for(i = 0; i < 4; i++){
   si_tl[i]->Fill(dssd_ba[i],dssd_fr[i*2+80]);
   si_tr[i]->Fill(dssd_ba[i],dssd_fr[i*2+81]);
  }

   for(i = 4; i < 8; i++){
   si_tl[i]->Fill(dssd_ba[i],dssd_fr[(i-4)*2+88]);
   si_tr[i]->Fill(dssd_ba[i],dssd_fr[(i-4)*2+89]);
  }

   for(i = 8; i < 12; i++){
   si_tl[i]->Fill(dssd_ba[i],dssd_fr[(i-8)*2+64]);
   si_tr[i]->Fill(dssd_ba[i],dssd_fr[(i-8)*2+65]);
  }

   for(i = 12; i < 16; i++){
   si_tl[i]->Fill(dssd_ba[i],dssd_fr[(i-12)*2+72]);
   si_tr[i]->Fill(dssd_ba[i],dssd_fr[(i-12)*2+73]);
  }



for(i = 0; i < 16; i++) {
  for(j = 0; j < 32; j++) {   
    if ((dssd_ba[i]>0)&&(dssd_fr[64+j])>0) trlhp->Fill(i,j);    
  }
 }




/*

// Look at pileup traces...


int badflag;
badflag = 0;

for(i = 0; i < CEvent.len; i++){

   if(fplane.ppacde > 0 && CEvent.DGSEvent[i].tpe == 5 && 
      //PU CEvent.DGSEvent[i].pu == 1 && 
      CEvent.DGSEvent[i].traceLen == 1000){

         for(j=0;j<1000;j++) if(CEvent.DGSEvent[i].trace[j] == 0) badflag++;   

         if(badflag < 10){

            if(CEvent.DGSEvent[i].tid < 161 && CEvent.DGSEvent[i].trhiE > 4000){
               PU_recoil_fr++;
               if(PU_recoil_fr < NumPU){
                  for(k=0;k<CEvent.DGSEvent[i].traceLen;k++){
                    // h2_recoil_pu_traces_fr->Fill(PU_recoil_fr,k,CEvent.DGSEvent[i].trace[k]);
                  }
               }
            }

            if(CEvent.DGSEvent[i].tid > 160 && CEvent.DGSEvent[i].trhiE > 4000){
               PU_recoil_ba++;
               if(PU_recoil_ba < NumPU){
                  for(k=0;k<CEvent.DGSEvent[i].traceLen;k++){
                   //  h2_recoil_pu_traces_ba->Fill(PU_recoil_ba,k,CEvent.DGSEvent[i].trace[k]);
                  }
               }
            }
         }
   }  
}
*/

/*
for(i = 0; i < CEvent.len; i++){

  //PU h1_pu->Fill(CEvent.DGSEvent[i].pu);

  if (CEvent.DGSEvent[i].tpe == 5 
   //PU && CEvent.DGSEvent[i].pu==1
   ) {
   PU_recoil_fr++;
   if(PU_recoil_fr < NumPU){
    for(k=0;k<CEvent.DGSEvent[i].traceLen;k++){
      h2_recoil_pu_traces_fr->Fill(PU_recoil_fr,k,CEvent.DGSEvent[i].trace[k]);
    }
   }   
  }

}
*/

signed long long int t_dssdppac_raw;

t_dssdppac_raw = 0;


//*********************\\
// DSSD SUB-EVENT LOOP \\
//*********************\\

// (Only look at DSSD sub-events if you have a hit on front and back)


// selected rate spectra

for(i=0; i<CEvent.len; i++){
   if(CEvent.DGSEvent[i].tpe == DSSD && CEvent.DGSEvent[i].tid == 300) h1_dssdrate_ch1->Fill((CEvent.DGSEvent[i].LEDts - t_first)/1E8);
   if(CEvent.DGSEvent[i].tpe == DSSD && CEvent.DGSEvent[i].tid == 100) h1_dssdrate_ch2->Fill((CEvent.DGSEvent[i].LEDts - t_first)/1E8);
   if(CEvent.DGSEvent[i].tpe == FP && CEvent.DGSEvent[i].tid == 1) h1_fprate->Fill((CEvent.DGSEvent[i].LEDts - t_first)/1E8);
}



if((mult_dssdfr != 0) && (mult_dssdba != 0)){
  
   for(i=0;i<CEvent.len;i++){

      if(CEvent.DGSEvent[i].tpe == DSSD){


      if(CEvent.DGSEvent[i].tid == 140){
         h1_dssd_dT140->Fill(CEvent.DGSEvent[i].LEDts - t_last140);
         h2_dssd_dTE140->Fill(CEvent.DGSEvent[i].LEDts - t_last140,E_last140);

         t_last140 = CEvent.DGSEvent[i].LEDts;
         E_last140 = CEvent.DGSEvent[i].ehi;
      }
      if(CEvent.DGSEvent[i].tid == 240){
         h1_dssd_dT240->Fill(CEvent.DGSEvent[i].LEDts - t_last240);
         h2_dssd_dTE240->Fill(CEvent.DGSEvent[i].LEDts - t_last240,E_last240);

         t_last240 = CEvent.DGSEvent[i].LEDts;
         E_last240 = CEvent.DGSEvent[i].ehi;
      }


      if(fplane.icde_ts != 0){
         t_dssdppac_raw = CEvent.DGSEvent[i].LEDts - fplane.icde_ts;         
         h2_tdssdppac_raw->Fill(t_dssdppac_raw,CEvent.DGSEvent[i].tid); 
      }   

      //-----------\\
      // Recoils...\\
      //-----------\\
      


        if((fplane.left != 0) || (fplane.right != 0) || (fplane.icde != 0) || (fplane.ppacde != 0) || (fplane.left_ts != 0) || (fplane.right_ts != 0) || (fplane.icde_ts != 0) || (fplane.ppacde_ts != 0)){

         //---------------\\
         // FRONT RECOILS \\
         //---------------\\

	  
         if(CEvent.DGSEvent[i].tid < 161){// && CEvent.DGSEvent[i].baseline < map_fr[CEvent.DGSEvent[i].tid].baseline){
	 
    
           //compression
           CEvent.DGSEvent[i].ehi = CEvent.DGSEvent[i].ehi/8;

            // Implementing threshold...
            if(CEvent.DGSEvent[i].ehi < map_fr[CEvent.DGSEvent[i].tid].thr) CEvent.DGSEvent[i].ehi = 0;
            // Implementing gain and offset...
            if(CEvent.DGSEvent[i].ehi != 0){
               CEvent.DGSEvent[i].ehi = map_fr[CEvent.DGSEvent[i].tid].gain*(CEvent.DGSEvent[i].ehi + float(rand())/RAND_MAX-0.5) + map_fr[CEvent.DGSEvent[i].tid].off;
            }
            // Set negative energies to zero.
            if(CEvent.DGSEvent[i].ehi < 0) CEvent.DGSEvent[i].ehi = 0;

            h2_r_fr_all->Fill(CEvent.DGSEvent[i].ehi,CEvent.DGSEvent[i].tid);

            // Maximum energy selection...
            if(CEvent.DGSEvent[i].ehi > r_fr_emax){
               r_fr_emax = CEvent.DGSEvent[i].ehi;
               s_r_fr = CEvent.DGSEvent[i].tid;
               r_fr_ts = CEvent.DGSEvent[i].LEDts;	// Take recoil timestamp from FRONT...
               r_fr_subev = i;               
               //PU r_fr_PU = CEvent.DGSEvent[i].pu;
            }
	   
         }

	 
         //--------------\\
         // BACK RECOILS \\
         //--------------\\

         else if(CEvent.DGSEvent[i].tid > 160){// && CEvent.DGSEvent[i].baseline > map_ba[CEvent.DGSEvent[i].tid].baseline){
	   
           //compression
           CEvent.DGSEvent[i].ehi = CEvent.DGSEvent[i].ehi/8;

            // Implementing threshold...
            if(CEvent.DGSEvent[i].ehi < map_ba[CEvent.DGSEvent[i].tid].thr) CEvent.DGSEvent[i].ehi = 0;
            // Implementing gain and offset...
            if(CEvent.DGSEvent[i].ehi != 0){
               CEvent.DGSEvent[i].ehi = map_ba[CEvent.DGSEvent[i].tid].gain*(CEvent.DGSEvent[i].ehi + float(rand())/RAND_MAX-0.5) + map_ba[CEvent.DGSEvent[i].tid].off;
            }

            // Set negative energies to zero.
            if(CEvent.DGSEvent[i].ehi < 0) CEvent.DGSEvent[i].ehi = 0;

            h2_r_ba_all->Fill(CEvent.DGSEvent[i].ehi,CEvent.DGSEvent[i].tid-160);

            // Maximum energy selection...
            if(CEvent.DGSEvent[i].ehi > r_ba_emax){
               r_ba_emax = CEvent.DGSEvent[i].ehi;
               s_r_ba = CEvent.DGSEvent[i].tid - 160;
               r_ba_ts = CEvent.DGSEvent[i].LEDts;	// Take recoil timestamp from BACK...
	       r_ba_subev = i;
               //PU r_ba_PU = CEvent.DGSEvent[i].pu;
            }
	   

         } //back    
   
      }


	// DECAYS
	
      if((fplane.left == 0) && (fplane.right == 0) && (fplane.icde == 0) && (fplane.ppacde == 0) && (fplane.left_ts == 0) && (fplane.right_ts == 0) && (fplane.icde_ts == 0) && (fplane.ppacde_ts == 0)){
         //--------------
         // FRONT DECAYS 
         //-------------
         if(CEvent.DGSEvent[i].tid < 161){// && CEvent.DGSEvent[i].baseline < -(map_fr[CEvent.DGSEvent[i].tid].baseline)){

            // Implementing threshold...
            if(CEvent.DGSEvent[i].ehi < map_fr[CEvent.DGSEvent[i].tid].thr) CEvent.DGSEvent[i].ehi = 0;
            // Implementing gain and offset...
            if(CEvent.DGSEvent[i].ehi != 0){
               CEvent.DGSEvent[i].ehi = map_fr[CEvent.DGSEvent[i].tid].gain*(CEvent.DGSEvent[i].ehi + float(rand())/RAND_MAX-0.5) + map_fr[CEvent.DGSEvent[i].tid].off;
            }

            // Set negative energies to zero.
            if(CEvent.DGSEvent[i].ehi < 0) CEvent.DGSEvent[i].ehi = 0;

            h2_d_fr_all->Fill(CEvent.DGSEvent[i].ehi,CEvent.DGSEvent[i].tid);

            // Look at high energy traces...

            //if(CEvent.DGSEvent[i].ehi > 5000) printf("Test at current event %i",CurEvNo);

            if(CEvent.DGSEvent[i].ehi > 8000){
               hiEfr++;      
               if(hiEfr < NumhiEfr){
                 //for(j=0;j<CEvent.DGSEvent[i].traceLen;j++)  h2_d_fr_traces_hie->Fill(hiEfr,j,CEvent.DGSEvent[i].trace[j]);
               }
            }

            // Maximum energy selection...
            if((CEvent.DGSEvent[i].ehi > d_fr_emax)){
               d_fr_emax = CEvent.DGSEvent[i].ehi;
               s_d_fr = CEvent.DGSEvent[i].tid;
	       d_fr_subev = i;
               d_fr_ts = CEvent.DGSEvent[i].LEDts;
               //PU d_fr_PU = CEvent.DGSEvent[i].pu;
            }
            
         }

         //-------------
         // BACK DECAYS 
         //-------------

         else if(CEvent.DGSEvent[i].tid >160){// && CEvent.DGSEvent[i].baseline > map_ba[CEvent.DGSEvent[i].tid].baseline){

            // Implementing threshold...
            if(CEvent.DGSEvent[i].ehi < map_ba[CEvent.DGSEvent[i].tid].thr) CEvent.DGSEvent[i].ehi = 0;
            // Implementing gain and offset...
            if(CEvent.DGSEvent[i].ehi != 0){
               CEvent.DGSEvent[i].ehi = map_ba[CEvent.DGSEvent[i].tid].gain*(CEvent.DGSEvent[i].ehi + float(rand())/RAND_MAX-0.5) + map_ba[CEvent.DGSEvent[i].tid].off;
            }

            // Set negative energies to zero.
            if(CEvent.DGSEvent[i].ehi < 0) CEvent.DGSEvent[i].ehi = 0;

            h2_d_ba_all->Fill(CEvent.DGSEvent[i].ehi,CEvent.DGSEvent[i].tid-160);

            if(CEvent.DGSEvent[i].ehi > 8000){
               hiEba++;      
               if(hiEba < NumhiEba){
                // for(j=0;j<CEvent.DGSEvent[i].traceLen;j++) // h2_d_ba_traces_hie->Fill(hiEba,j,CEvent.DGSEvent[i].trace[j]);
               }
            }

            // Maximum energy selection
            if((CEvent.DGSEvent[i].ehi > d_ba_emax)){
               d_ba_emax = CEvent.DGSEvent[i].ehi;
               s_d_ba = CEvent.DGSEvent[i].tid-160;     
               d_ba_ts = CEvent.DGSEvent[i].LEDts;		// Take decay timestamp from BACK...
               d_ba_subev = i;
               //PU d_ba_PU = CEvent.DGSEvent[i].pu;
            }
         } 

      }

	

      } // tpe=DSSD

   } // end of DSSD loop

 } // multfr and multba


if(s_d_fr != 0) h1_d_fr_emax->Fill(d_fr_emax);
if(s_d_ba != 0) h1_d_ba_emax->Fill(d_ba_emax);

if ((d_fr_PU==1)||(d_ba_PU==1)) {
dec_PU=1; 
//printf("dec_PU\n");
}
if ((r_fr_PU==1)||(r_ba_PU==1)) {
rec_PU=1;
//printf("rec_PU\n");
}

// save PU TRACES

for(i = 0; i < CEvent.len; i++){

   if (CEvent.DGSEvent[i].tpe == 5 && CEvent.DGSEvent[i].traceLen == 1000){

     if ((rec_PU==1)&&(s_r_fr!=0)&&(s_r_ba!=0)) {
            if(CEvent.DGSEvent[i].tid == s_r_fr){

	      printf("h2_PU_recoil_fr\n");
               PU_recoil_fr++;
               if(PU_recoil_fr < NumPU){
                  for(k=0;k<CEvent.DGSEvent[i].traceLen;k++){
                    h2_recoil_pu_traces_fr->Fill(PU_recoil_fr,k,CEvent.DGSEvent[i].trace[k]);
                  }
               }
            }

            if(CEvent.DGSEvent[i].tid == s_r_ba+160){
	      printf("h2_PU_recoil_ba\n");
               PU_recoil_ba++;
               if(PU_recoil_ba < NumPU){
                  for(k=0;k<CEvent.DGSEvent[i].traceLen;k++){
                   h2_recoil_pu_traces_ba->Fill(PU_recoil_ba,k,CEvent.DGSEvent[i].trace[k]);
                  }
               }
            }
	    }

     if ((dec_PU==1)&&(s_d_fr!=0)&&(s_d_ba!=0)) {

            if(CEvent.DGSEvent[i].tid == s_d_fr){
	      printf("h2_PU_dec_fr\n");
               PU_dec_fr++;
               if(PU_dec_fr < NumPU){
                  for(k=0;k<CEvent.DGSEvent[i].traceLen;k++){
                    h2_dec_pu_traces_fr->Fill(PU_dec_fr,k,CEvent.DGSEvent[i].trace[k]);
                  }
               }
	    }
            

            if(CEvent.DGSEvent[i].tid == s_d_ba +160 ){
	      printf("h2_PU_dec_ba\n");
               PU_dec_ba++;
               if(PU_dec_ba < NumPU){
                  for(k=0;k<CEvent.DGSEvent[i].traceLen;k++){
                   h2_dec_pu_traces_ba->Fill(PU_dec_ba,k,CEvent.DGSEvent[i].trace[k]);
                  }
               }
            }
       }
            
   }  
}

//--------------------------------
// Focal plane
//--------------------------------

if(mult_dssdfr != 0 && mult_dssdba != 0){
   if(fplane.left == 0 && fplane.left_ts == 0 && fplane.right !=0 && fplane.icde != 0 && fplane.ppacde != 0) h1_fplane_stats->Fill(1);
   if(fplane.left != 0 && fplane.right ==0 && fplane.right_ts == 0 && fplane.icde != 0 && fplane.ppacde != 0) h1_fplane_stats->Fill(2);
   if(fplane.left != 0 && fplane.right !=0 && fplane.icde == 0 && fplane.icde_ts == 0 && fplane.ppacde != 0) h1_fplane_stats->Fill(3);
   if(fplane.left != 0 && fplane.right !=0 && fplane.icde != 0 && fplane.ppacde == 0 && fplane.ppacde_ts == 0) h1_fplane_stats->Fill(4);
   if(fplane.left == 0 && fplane.right ==0 && fplane.icde == 0 && fplane.ppacde == 0 && fplane.left_ts == 0 && fplane.right_ts ==0 && fplane.icde_ts == 0 && fplane.ppacde_ts == 0){
	 h1_fplane_stats->Fill(5);
   }
}

// Raw focal plane histograms...
h1_l_raw->Fill(fplane.left);
h1_r_raw->Fill(fplane.right);
h1_rl_raw->Fill(fplane.left+fplane.right);
h1_x_raw->Fill(fplane.left-fplane.right);
h1_icde_raw->Fill(fplane.icde);
h1_ppacde_raw->Fill(fplane.ppacde);
h1_ppacde2_raw->Fill(fplane.ppacde2);
h2_x_icde_raw->Fill(fplane.left-fplane.right,fplane.icde);
h2_LvR_raw->Fill(fplane.left,fplane.right);
h2_Lvppacde_raw->Fill(fplane.left,fplane.ppacde);
h2_Rvppacde_raw->Fill(fplane.right,fplane.ppacde);
h2_x_ppacde_raw->Fill(fplane.left-fplane.right,fplane.ppacde);

if((fplane.left + fplane.right > 1800) && (fplane.left + fplane.right < 2200)){

   h2_ppacde_icde->Fill(fplane.ppacde,fplane.icde);
   h1_l->Fill(fplane.left);
   h1_r->Fill(fplane.right);
   h1_rl->Fill(fplane.left+fplane.right);
   h1_x->Fill(fplane.left-fplane.right);
   h1_icde->Fill(fplane.icde);
   h1_ppacde->Fill(fplane.ppacde);
   h2_x_icde->Fill(fplane.left-fplane.right,fplane.icde);
   h2_LvR->Fill(fplane.left,fplane.right);
   h2_Lvppacde->Fill(fplane.left,fplane.ppacde);
   h2_Rvppacde->Fill(fplane.right,fplane.ppacde);
   h2_x_ppacde->Fill(fplane.left-fplane.right,fplane.ppacde);

}

h2_recoilen_icde->Fill(r_ba_emax,fplane.icde);

//--------------------------------
// Recoils
//--------------------------------

int badflag_fr;
badflag_fr = 0;
int badflag_ba;
badflag_ba = 0;

if(fplane.left_ts != 0 && fplane.right_ts != 0) t_add = fplane.left_ts + fplane.right_ts;
signed long long int t_dssdppac, t_dssdppac_fr, t_dssdppac_ba;
t_dssdppac = 0;
t_dssdppac_fr = 0;
t_dssdppac_ba = 0;

if(s_r_fr != 0 && s_r_ba != 0){
   // Checking PPAC-DSSD coincidences
   t_dssdppac = r_ba_ts - fplane.ppacde_ts;
   h2_tdssdppac_ba->Fill(t_dssdppac,s_r_ba);
   h2_recoilen_tdssdppac->Fill(r_ba_emax,t_dssdppac);

}

// front recoils
if(s_r_fr != 0){

   t_dssdppac_fr = r_fr_ts - fplane.ppacde_ts;

   //if(impwin->IsInside(r_fr_emax,t_dssdppac_fr) == 1){
      dssd_front[s_r_fr].status = 1;
      dssd_front[s_r_fr].chain.recoil.en = r_fr_emax;
      dssd_front[s_r_fr].chain.recoil.ts = r_fr_ts; 

/*
      if(CEvent.DGSEvent[r_fr_subev].pu == 1){

         for(i=0;i<CEvent.DGSEvent[r_fr_subev].traceLen; i++){ 
            if(CEvent.DGSEvent[r_fr_subev].trace[i] == 0) badflag_fr++; }

            if(badflag_fr < 10 && CEvent.DGSEvent[r_fr_subev].trhiE > 4000){
               PU_hiE_fr++;
               if(PU_hiE_fr < NumPU_hiE_fr){

                  for(i=0;i<CEvent.DGSEvent[r_fr_subev].traceLen;i++){

                     h2_recoil_pu_traces_fr->Fill(PU_hiE_fr,i,CEvent.DGSEvent[r_fr_subev].trace[i]);
                    if(i==1){
                       h2_recoil_pu_traces_fr->SetBinContent(i,0);
                       h2_recoil_pu_traces_fr->Fill(PU_hiE_fr,i,CEvent.DGSEvent[r_fr_subev].ehi);
                    }  
                  }
               }
            }
      }
*/

   //}

}

// back recoils
if(s_r_ba != 0){

   t_dssdppac_ba = r_ba_ts - fplane.ppacde_ts;

   //if(impwin->IsInside(r_ba_emax,t_dssdppac_ba) == 1){
      dssd_back[s_r_ba].status = 1;
      dssd_back[s_r_ba].chain.recoil.en = r_ba_emax;
      dssd_back[s_r_ba].chain.recoil.ts = r_ba_ts; 

//???
/*
      if(CEvent.DGSEvent[r_ba_subev].pu == 1){

         for(i=0;i<CEvent.DGSEvent[r_ba_subev].traceLen; i++){ 
            if(CEvent.DGSEvent[r_ba_subev].trace[i] == 0) badflag_ba++; }

            if(badflag_ba < 10 && CEvent.DGSEvent[r_ba_subev].trhiE > 4000){
               PU_hiE_ba++;
               if(PU_hiE_ba < NumPU_hiE_ba){
                 
                 for(i=0;i<CEvent.DGSEvent[r_ba_subev].traceLen;i++){
                    h2_recoil_pu_traces_ba->Fill(PU_hiE_ba,i,CEvent.DGSEvent[r_ba_subev].trace[i]);
                    if(i==1){
                       h2_recoil_pu_traces_ba->SetBinContent(i,0);
                       h2_recoil_pu_traces_ba->Fill(PU_hiE_ba,i,CEvent.DGSEvent[r_ba_subev].ehi);
                    } 
                 } 
               }
            }
      }
*/
   //}
}

// recoil correlation pre-process
if((s_r_fr != 0) && (s_r_ba != 0)){// && (impwin->IsInside(r_ba_emax,t_dssdppac) == 1)){

   h2_recoilen_tdssdppac_gated->Fill(r_ba_emax,t_dssdppac);

   //PU recoil.pu = CEvent.DGSEvent[r_ba_subev].pu;
   recoil.en = r_fr_emax;
   recoil.ts = r_fr_ts;

   h1_recrate->Fill((r_ba_ts - t_first)/1E8);
   if(s_r_ba == 100) h1_recrate_ch->Fill((r_ba_ts - t_first)/1E8);
   h2_r_hitxy->Fill(s_r_fr,s_r_ba);
   h2_r_fr_e->Fill(r_fr_emax,s_r_fr);
   h2_r_ba_e->Fill(r_ba_emax,s_r_ba);
   
   recoil.tof = t_dssdppac;
   recoil.right = fplane.right;

   if ((r_ba_emax>100)&&(r_ba_emax<400)&&(t_dssdppac>10)&&(t_dssdppac<20)) {
    h2_Rvppacde_rec->Fill(fplane.right,fplane.ppacde);   
   }


   dssd_corr[s_r_fr][s_r_ba].status = 1;
   dssd_corr[s_r_fr][s_r_ba].chain.recoil = recoil;

   if(PuType < NumPuType && recoil.pu == 1){
      recoil_pu[PuType].ts = recoil.ts;
      recoil_pu[PuType].s_fr = s_r_fr;
      recoil_pu[PuType].s_ba = s_r_ba;
      recoil_pu[PuType].traceLen = CEvent.DGSEvent[r_ba_subev].traceLen;
         for(i=0;i<recoil_pu[PuType].traceLen;i++){
            recoil_pu[PuType].trace[i] = CEvent.DGSEvent[r_ba_subev].trace[i];
         }
      PuType++;
   }
}

//--------------------------------
// Decays
//--------------------------------

int d_ediff;
d_ediff = 0;


if(s_d_fr != 0){
   if(dssd_front[s_d_fr].status > 0) dssd_front[s_d_fr].status++;
   dssd_front[s_d_fr].chain.decay[0].ts = d_fr_ts;
   dssd_front[s_d_fr].chain.decay[0].en = d_fr_emax;
}

if(s_d_ba != 0){
   if(dssd_front[s_d_ba].status > 0) dssd_back[s_d_ba].status++;
   dssd_back[s_d_ba].chain.decay[0].ts = d_ba_ts;
   dssd_back[s_d_ba].chain.decay[0].en = d_ba_emax;
}

// ???
if(d_fr_emax < 100 && d_ba_emax > 200 && s_d_fr != 0 && s_d_ba != 0){
    counter_ba++;
    if(counter_ba< Max){
       for(i=0;i<CEvent.DGSEvent[d_ba_subev].traceLen;i++){
          //h2_backside_traces_fr->Fill(counter_ba,i,CEvent.DGSEvent[d_fr_subev].trace[i]);
          //h2_backside_traces_ba->Fill(counter_ba,i,CEvent.DGSEvent[d_ba_subev].trace[i]);
          if(i==1){
            // h2_backside_traces_fr->SetBinContent(i,0);
            // h2_backside_traces_ba->SetBinContent(i,0);
            // h2_backside_traces_fr->Fill(counter_ba,i,CEvent.DGSEvent[d_fr_subev].ehi);
            // h2_backside_traces_ba->Fill(counter_ba,i,CEvent.DGSEvent[d_ba_subev].ehi);
          }
       }
    }
} 

// ???
if(d_ba_emax < 100 && d_fr_emax > 200 && s_d_fr != 0 && s_d_ba != 0){
    counter_fr++;

    if(counter_fr< Max){
       for(i=0;i<CEvent.DGSEvent[d_fr_subev].traceLen;i++){
         // h2_frontside_traces_fr->Fill(counter_fr,i,CEvent.DGSEvent[d_fr_subev].trace[i]);
         // h2_frontside_traces_ba->Fill(counter_fr,i,CEvent.DGSEvent[d_ba_subev].trace[i]);
          if(i==1){
           //  h2_frontside_traces_fr->SetBinContent(i,0);
           //  h2_frontside_traces_ba->SetBinContent(i,0);
           //  h2_frontside_traces_fr->Fill(counter_fr,i,CEvent.DGSEvent[d_fr_subev].ehi);
           //  h2_frontside_traces_ba->Fill(counter_fr,i,CEvent.DGSEvent[d_ba_subev].ehi);
          }
       }
    }
} 


// decay correlations pre-processing

if((s_d_fr != 0) && (s_d_ba != 0) && (s_r_fr == 0) && (s_r_ba == 0)){
 
 d_ediff = d_fr_emax - d_ba_emax;
 h1_decay_ediff->Fill(d_ediff);
 h2_d_fb->Fill(d_fr_emax,d_ba_emax);

   //if(abs(d_ediff) < 2000){

   // Look at pileup traces...
/*PU  
 if(CEvent.DGSEvent[d_ba_subev].pu == 1 && CEvent.DGSEvent[d_ba_subev].traceLen == 1000){
      PU_decay++;
      if(PU_decay < NumPU){
        // for(i=0;i<CEvent.DGSEvent[d_ba_subev].traceLen;i++) h2_decay_pu_traces->Fill(PU_decay,i,CEvent.DGSEvent[d_ba_subev].trace[i]);
      }
   }
*/

   decay.en = d_fr_emax;
   decay.ts = d_fr_ts;

   if(dssd_corr[s_d_fr][s_d_ba].status == 1) dssd_corr[s_d_fr][s_d_ba].chain.decay[0] = decay;
   if(dssd_corr[s_d_fr][s_d_ba].status == 2) dssd_corr[s_d_fr][s_d_ba].chain.decay[1] = decay;
   if(dssd_corr[s_d_fr][s_d_ba].status == 3) dssd_corr[s_d_fr][s_d_ba].chain.decay[2] = decay;
   if(dssd_corr[s_d_fr][s_d_ba].status > 3) dssd_corr[s_d_fr][s_d_ba].chain.decay[3] = decay;

   h1_decrate->Fill((d_ba_ts - t_first)/1E8);
   h2_d_hitxy->Fill(s_d_fr,s_d_ba);
   h2_d_fr_e->Fill(d_fr_emax,s_d_fr);
   h2_d_ba_e->Fill(d_ba_emax,s_d_ba);

   if(dssd_corr[s_d_fr][s_d_ba].status > 0) dssd_corr[s_d_fr][s_d_ba].status++;

   h1_decay_generations->Fill(dssd_corr[s_d_fr][s_d_ba].status);
   
  //} // ediff gate
}

//--------------------------------
// Decay Correlations
//--------------------------------

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

signed long long int tof;
tof = 0;

unsigned long long int corr_time, corr_time_short, decay0_time;
decay0_time = 0;
corr_time = 10E8; // 10 seconds
corr_time_short =  1E7; //100ms

// front side only correlations
if(s_d_fr != 0){
 if(dssd_front[s_d_fr].status == 2){
   decay.time = dssd_front[s_d_fr].chain.decay[0].ts - dssd_front[s_d_fr].chain.recoil.ts;
   if(decay.time > 0) h2_e1t1log_fr->Fill(dssd_front[s_d_fr].chain.decay[0].en,2.303*log10(decay.time));
 }
}
// back side only correlations
if(s_d_ba != 0){
  if(dssd_back[s_d_ba].status == 2){
     decay.time = dssd_back[s_d_ba].chain.decay[0].ts - dssd_back[s_d_ba].chain.recoil.ts;
     if(decay.time > 0) h2_e1t1log_ba->Fill(dssd_back[s_d_ba].chain.decay[0].en,2.303*log10(decay.time));
  }
}

// two-side correlations
if((s_d_fr != 0) && (s_d_ba != 0)){

   switch(dssd_corr[s_d_fr][s_d_ba].status){

      case 0:	// No recoil before - do nothing.
         break;
      case 1:   // Should never happen...
         break;
      case 2:   // First decay generation

         decay.time = decay.ts - dssd_corr[s_d_fr][s_d_ba].chain.recoil.ts;
      
         if (decay.time>0){
            logtime=2.303*log10(decay.time);
            h2_e1t1log->Fill(decay.en,logtime);
         }

    
	 // traces for wierd fast decays

  if (logtime<8&&(decay.en>300)&&(decay.en<400)) {
      
   for(i = 0; i < CEvent.len; i++){

   if (CEvent.DGSEvent[i].tpe == 5 && CEvent.DGSEvent[i].traceLen == 1000){
            

            if(CEvent.DGSEvent[i].tid == s_d_fr){
	      //printf("h2_PU_recoil_fr\n");
               fast_fr++;
                    h2_fast_bl_fr->Fill(CEvent.DGSEvent[i].baseline,s_d_fr);
               if(fast_fr < NumPU){
                  for(k=0;k<CEvent.DGSEvent[i].traceLen;k++){
                    h2_fast_traces_fr->Fill(fast_fr,k,CEvent.DGSEvent[i].trace[k]);

                  }
               }
	    }

            if(CEvent.DGSEvent[i].tid == s_d_ba+160){
	      //printf("h2_PU_recoil_ba\n");
               fast_ba++;
                    h2_fast_bl_ba->Fill(CEvent.DGSEvent[i].baseline,s_d_ba);
               if(fast_ba < NumPU){
                  for(k=0;k<CEvent.DGSEvent[i].traceLen;k++){
                   h2_fast_traces_ba->Fill(fast_ba,k,CEvent.DGSEvent[i].trace[k]);
                  }
               }
            }
	   

   }
 }
  }

         h2_e1t1c1->Fill(decay.en,decay.time/T1COMP1);
         h2_e1t1c2->Fill(decay.en,decay.time/T1COMP2);
         h2_e1t1c3->Fill(decay.en,decay.time/T1COMP3);
         h2_e1t1c4->Fill(decay.en,decay.time/T1COMP4);
    
         // save correlated decay pile-up events 1 gen

         if(logtime < 18) {

	   if((dec_PU==1) && (dec1PuCorr < NumPuType)){

	     for (i=0;i<CEvent.len;i++) {
               if (CEvent.DGSEvent[i].tpe==DSSD) {
               if (CEvent.DGSEvent[i].tid==s_d_fr) for(j=0;j<CEvent.DGSEvent[i].traceLen;i++) h2_dec1pu_corr_fr_traces->Fill(dec1PuCorr,j,CEvent.DGSEvent[i].trace[j]);
               if (CEvent.DGSEvent[i].tid==s_d_ba+160) for(j=0;j<CEvent.DGSEvent[i].traceLen;i++) h2_dec1pu_corr_ba_traces->Fill(dec1PuCorr,j,CEvent.DGSEvent[i].trace[j]); 
               dec1PuCorr++;
	       }
	     }
          }  

         }

        // Fission events...
        if(logtime < 15 && decay.en > 10000) h2_recoilen_tdssdppac_fission->Fill(dssd_corr[s_d_fr][s_d_ba].chain.recoil.en,dssd_corr[s_d_fr][s_d_ba].chain.recoil.tof);

        if(logtime < 15 && decay.en < 500) h2_recoilen_tdssdppac_electrons->Fill(dssd_corr[s_d_fr][s_d_ba].chain.recoil.en,dssd_corr[s_d_fr][s_d_ba].chain.recoil.tof);
         
        if(logtime < 13) h2_R_corr_decayen->Fill(dssd_corr[s_d_fr][s_d_ba].chain.recoil.right,decay.en);     
        

        if(logtime < 10) h2_R_corr_decayen_lowE->Fill(dssd_corr[s_d_fr][s_d_ba].chain.recoil.right,decay.en); 

        if (decay.time < corr_time_short)  h2_xe1_short->Fill(dssd_corr[s_d_fr][s_d_ba].chain.recoil.right,decay.en);
        if (decay.time < corr_time )	h2_xe1->Fill(dssd_corr[s_d_fr][s_d_ba].chain.recoil.right,decay.en);      

         break;

      case 3:   // Second decay generation

         decay.time = decay.ts - dssd_corr[s_d_fr][s_d_ba].chain.decay[0].ts;
         decay0_time = dssd_corr[s_d_fr][s_d_ba].chain.decay[0].ts - dssd_corr[s_d_fr][s_d_ba].chain.recoil.ts;

         if (decay.time>0){
            logtime=2.303*log10(decay.time);
            h2_e2t2log->Fill(decay.en,logtime);
         }
         h2_e2t2c1->Fill(decay.en,decay.time/T1COMP1);
         h2_e2t2c2->Fill(decay.en,decay.time/T1COMP2);
         h2_e2t2c3->Fill(decay.en,decay.time/T1COMP3);
         h2_e2t2c4->Fill(decay.en,decay.time/T1COMP4);

         if(decay.time < corr_time_short && decay0_time < corr_time_short)  h2_e1e2_short->Fill(decay.en, dssd_corr[s_d_fr][s_d_ba].chain.decay[0].en);
         if(decay.time < corr_time && decay0_time < corr_time)	            h2_e1e2->Fill(decay.en, dssd_corr[s_d_fr][s_d_ba].chain.decay[0].en);



        // save correlated decay pile-up events 2 gen

        if(logtime < 18) {

	   if((dec_PU==1) && (dec2PuCorr < NumPuType)){

	     for (i=0;i<CEvent.len;i++) {
               if (CEvent.DGSEvent[i].tid==DSSD) {
               if (CEvent.DGSEvent[i].tid==s_d_fr) for(j=0;j<CEvent.DGSEvent[i].traceLen;i++) h2_dec2pu_corr_fr_traces->Fill(dec2PuCorr,j,CEvent.DGSEvent[i].trace[j]);
               if (CEvent.DGSEvent[i].tid==s_d_ba+160) for(j=0;j<CEvent.DGSEvent[i].traceLen;i++) h2_dec2pu_corr_ba_traces->Fill(dec2PuCorr,j,CEvent.DGSEvent[i].trace[j]); 
               dec2PuCorr++;
	       }
	     }
          }  

         }


         break;

      case 4:   // Third decay generation

         decay.time = decay.ts - dssd_corr[s_d_fr][s_d_ba].chain.decay[1].ts;

         if (decay.time>0){
            logtime=2.303*log10(decay.time);
            h2_e3t3log->Fill(decay.en,logtime);
         }
         h2_e3t3c1->Fill(decay.en,decay.time/T1COMP1);
         h2_e3t3c2->Fill(decay.en,decay.time/T1COMP2);
         h2_e3t3c3->Fill(decay.en,decay.time/T1COMP3);
         h2_e3t3c4->Fill(decay.en,decay.time/T1COMP4);

       if(decay.time < corr_time_short && decay0_time < corr_time_short)  h2_e2e3_short->Fill(decay.en, dssd_corr[s_d_fr][s_d_ba].chain.decay[1].en);
       if(decay.time < corr_time && decay0_time < corr_time)	            h2_e2e3->Fill(decay.en, dssd_corr[s_d_fr][s_d_ba].chain.decay[1].en);


         break;

      case 5:   // Fourth decay generation
         break;
      case 6:   // Fifth decay generation
         break;
      default:
         break;

   }

}


