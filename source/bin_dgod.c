//#include <stdlib.h>
//#include <stdio.h>
//#include <stddef.h>
//#include <math.h>
//#include <string.h>
//#include <assert.h>

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

extern DFMAEVENT DFMAEvent[MAXCOINEV];
int DFMAEvDecompose_v3 (unsigned int *ev, int len, DFMAEVENT * DFMAEvent); 

extern int tlkup[NCHANNELS];
extern int tid[NCHANNELS];

TH1D *h1_god_en;
TH2F *h2_god_en;

/*-----------------------------------------------------*/

int
sup_dgod ()
{
  /* declarations */

  char str1[STRLEN], str2[STRLEN];
  float pi;
  int i;

  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

  h1_god_en = mkTH1D((char *)"god_all_en",(char *)"god_all_en",16000,0,160000);
  h2_god_en = mkTH2F((char *)"god_en",(char *)"god_en",16000,0,160000,400,0,400);

  /* list what we have */

  printf (" we have define the following spectra:\n");

  Pars.wlist = gDirectory->GetList ();
  Pars.wlist->Print ();

};

/* ----------------------------------------------------------------- */

int
bin_dgod (GEB_EVENT * GEB_event)
{

  /* declarations */

  char str[128];
  int i;
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

        };

    };

  /* histogram incremantation */

  for (i=0;i<nsubev;i++) {

    if(DFMAEvent[i].tpe == DSSD){
      h2_god_en->Fill(DFMAEvent[i].ehi,DFMAEvent[i].tid);
      h1_god_en->Fill(DFMAEvent[i].ehi);
    }
  }

  /* done */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit bin_dgod\n");

  return (0);

}
