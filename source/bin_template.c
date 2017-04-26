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

#include "veto_pos.h"
#include "GEBSort.h"
#include "GTMerge.h"


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

extern PARS pars;


/*-----------------------------------------------------*/

int
sup_template ()
{
  /* declarations */

  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

  /* list what we have */

  printf (" we have define the following spectra:\n");

  pars.wlist = gDirectory->GetList ();
  pars.wlist->Print ();

  return(0);

}

/* ----------------------------------------------------------------- */

int
bin_template (GEB_EVENT * gebEvt)
{

  /* declarations */

  char str[128];
  int i;

  /* prototypes */

  int GebTypeStr (int type, char str[]);

if(1)return(0);

  if (pars.CurEvNo <= pars.NumToPrint)
    printf ("entered bin_template:\n");


  /* loop through the coincidence event and fish out GEB_TYPE_DGS data */

  for (i = 0; i < gebEvt->mult; i++)
    {

      if (gebEvt->ptgd[i]->type == GEB_TYPE_DGS)
        {

          if (pars.CurEvNo <= pars.NumToPrint)
            {
              GebTypeStr (gebEvt->ptgd[i]->type, str);
              printf ("bin_template, %2i> %2i, %s, TS=%lli\n", i, gebEvt->ptgd[i]->type, str,
                      gebEvt->ptgd[i]->timestamp);
            }

        };

    };

  /* done */

  if (pars.CurEvNo <= pars.NumToPrint)
    printf ("exit bin_template\n");

  return (0);

}
