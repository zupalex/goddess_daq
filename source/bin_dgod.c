#include "gdecomp.h"
#include "GEBSort.h"
#include "GTMerge.h"

#include "TH1D.h"
#include "TH1F.h"
#include "TH2F.h"

typedef struct PAYLOAD
{
  char p[MAXDATASIZE];
} PAYLOAD;

typedef struct TRACK_STRUCT
{
  int n;
  GEBDATA *gd;
  PAYLOAD *payload;
} TRACK_STRUCT;


/* pointers to ROOT spectra */


/* parameters */

extern PARS Pars;

int DFMAEvDecompose_v3 (unsigned int *ev, int len, DFMAEVENT * DFMAEvent); 
extern DFMAEVENT DFMAEvent[MAXCOINEV];

extern int tlkup[NCHANNELS];
extern int tid[NCHANNELS];
extern DGSEVENT DGSEvent[MAXCOINEV];
extern int ng;
extern unsigned int numDGOD;

TH1D *h1_god_en;
TH2F *h2_god_en;
TH2F *h2_dTg_god;
TH2F *h2_g_god;

TH1D *h1_x, *h1_x_g;

/*-----------------------------------------------------*/

int
sup_dgod ()
{
  /* declarations */

	gDirectory->mkdir("dgod")->cd();
  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

  h1_god_en = mkTH1D((char *)"god_all_en",(char *)"god_all_en",16000,0,1600000);
  h2_god_en = mkTH2F((char *)"god_en",(char *)"god_en",16000,0,1600000,400,0,400);

  h2_dTg_god = mkTH2F((char *)"dTg_god",(char *)"dTg_god",4000,-2000,2000,400,0,400);

  h2_g_god  = mkTH2F((char *)"g_god",(char *)"g_god",4000,0,4000,4000,0,400000);

  h1_x = mkTH1D((char *)"x",(char *)"x",16000,0,16000);
  h1_x_g = mkTH1D((char *)"xg",(char *)"x",16000,0,16000);

	gDirectory->cd("/");

  /* list what we have */

  printf (" we have define the following spectra:\n");

  Pars.wlist = gDirectory->GetList ();
  Pars.wlist->Print ();

  return (0);

}

/* ----------------------------------------------------------------- */

int
bin_dgod (GEB_EVENT * GEB_event)
{

  /* declarations */

  char str[128];
  int i, j;
  int ndssd;
  int ndfma;
  int nfp;
  int nsubev;

  /* prototypes */

  int GebTypeStr (int type, char str[]);

//if(1)return(0);

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered bin_god:\n");

  ndfma = 0;
  ndssd = 0;
  nsubev = 0;
  nfp = 0;

	numDGOD = 0;


  /* loop through the coincidence event and fish out GEB_TYPE_DFMA data */

  for (i = 0; i < GEB_event->mult; i++)
    {

      if (GEB_event->ptgd[i]->type == 16)
        {

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              GebTypeStr (GEB_event->ptgd[i]->type, str);
              printf ("bin_template, %2i> %2i, %s, TS=%lli\n", i, GEB_event->ptgd[i]->type, str,
                      GEB_event->ptgd[i]->timestamp);
            }

          DFMAEvDecompose_v3 ((unsigned int *) GEB_event->ptinp[i], GEB_event->ptgd[i]->length / sizeof (unsigned int),
                             &DFMAEvent[nsubev]);

      if(DFMAEvent[nsubev].tpe == DSSD){
             ndssd++;
             ndfma++;
      }
      if(DFMAEvent[nsubev].tpe == FP){
             nfp++;
             ndfma++;
      }
           nsubev++;
				numDGOD++;

        };

    };

  // histogram incremantation 

  for (i=0;i<nsubev;i++) {

    if(DFMAEvent[i].tpe == DSSD){
      h2_god_en->Fill(DFMAEvent[i].ehi,DFMAEvent[i].tid);
      h1_god_en->Fill(DFMAEvent[i].ehi);
    }
  }

  // time differences 

  double dTg_god;
  dTg_god = 0.0;

  for(i=0;i<nsubev;i++){
   if((DFMAEvent[i].LEDts > 0) && (DFMAEvent[i].tpe == DSSD) && (ng > 0)){

        dTg_god = double(DGSEvent[0].event_timestamp) - double(DFMAEvent[i].LEDts);
        h2_dTg_god->Fill(dTg_god,DFMAEvent[i].tid);

     }
    }

 for(i=0;i<nsubev;i++){
  for(j=0;j<ng;j++){

    dTg_god = double(DGSEvent[j].event_timestamp) - double(DFMAEvent[i].LEDts);
    if ((DFMAEvent[i].tid==10)&&(dTg_god>260)&(dTg_god<290)) h2_g_god->Fill(DGSEvent[j].ehi,DFMAEvent[i].ehi);

   }
  }
 
 // start fma stuffs

 unsigned int left = 0;
 unsigned int right = 0;
 unsigned int lr = 0;
 unsigned int x = 0;

 for (i=0;i<nsubev;i++) {
   if(DFMAEvent[i].tpe == FP){
     DFMAEvent[i].ehi=DFMAEvent[i].ehi/30;
     if (DFMAEvent[i].tid==1) { left=DFMAEvent[i].ehi;}
     if (DFMAEvent[i].tid==2) { right=DFMAEvent[i].ehi; }
   }
    
 }

 if( (left>0)&&(right>0) ){
   x=left-right+8000;
   lr=left+right;
 }

 h1_x->Fill(x);

// Si-FMA 

 unsigned int left_g=0;
 unsigned int right_g=0;	

for(i=0;i<nsubev;i++){
  if((DFMAEvent[i].LEDts > 0) && (DFMAEvent[i].tpe == FP) ){
    
    for(j=0;j<nsubev;j++){    
      if  ((DFMAEvent[j].tpe == DSSD)&&(DFMAEvent[j].tid>0)&&(DFMAEvent[j].tid<107)) {
	dTg_god = double(DFMAEvent[j].LEDts) - double(DFMAEvent[i].LEDts);
	
	if ((dTg_god>130)&&(dTg_god<170)) {
	  if (DFMAEvent[i].tid==1) { left_g=DFMAEvent[i].ehi;}
	  if (DFMAEvent[i].tid==2) { right_g=DFMAEvent[i].ehi;}
	  // Fill 1d gated x spectrum
				if((left_g>0)&&(right_g>0)){	
				  x=(left_g-right_g+8000);
				  h1_x_g->Fill(x);
				}
	}
      }
    }
  }	
 }
 

 



  /* done */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit bin_dgod\n");

  return (0);

}
