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

#if(0)
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
#endif

/* pointers to ROOT spectra */

TH1D *dgs_sumehi;
TH1D *dgshitpat;
TH1D *hitpatge;
TH1D *hitpatbgo;
extern TH1D *ehi[MAXDETPOS + 1];

/* parameters */

extern PARS Pars;
int tlkup[NCHANNELS];
int tid[NCHANNELS];


/*-----------------------------------------------------*/

int
sup_dgs ()
{
  /* declarations */

  char str1[STRLEN], str2[STRLEN], str[STRLEN];
  float pi;
  int i, i1, i2, i7, i8;
  FILE *fp;

  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

  sprintf (str1, "dgs_sumehi");
  dgs_sumehi = mkTH1D (str1, str1, LONGLEN, 1, LONGLEN);
  dgs_sumehi->SetXTitle (str1);

  printf ("\n");
  sprintf (str1, "dgshitpat");
  sprintf (str2, "DGS");
  dgshitpat = mkTH1D (str1, str2, NCHANNELS, 0, NCHANNELS);
  printf ("%s;", str1);
  printf ("\n");

  sprintf (str1, "hitpatGe");
  sprintf (str2, "DGS");
  hitpatge = mkTH1D (str1, str2, NGE, 0, NGE);
  printf ("%s;", str1);
  printf ("\n");

  sprintf (str1, "hitpatBGO");
  sprintf (str2, "DGS");
  hitpatbgo = mkTH1D (str1, str2, NGE, 0, NGE);
  printf ("%s;", str1);
  printf ("\n");

  /* list what we have */

//  printf (" we have define the following spectra:\n");

//  Pars.wlist = gDirectory->GetList ();
//  Pars.wlist->Print ();

  /* -------------------- */
  /* read in the map file */
  /* -------------------- */

  for (i = 0; i < NCHANNELS; i++)
    {
      tlkup[i] = NOTHING;
      tid[i] = NOTHING;
    };

  fp = fopen ("map.dat", "r");
  if (fp == NULL)
    {
      printf ("need a \"map.dat\" file to run\n");
      system("./mkMap > map.dat");
      printf("just made you one...\n");
      fp = fopen ("map.dat", "r");
      assert(fp != NULL);
     };

  printf ("\nmapping\n");

  i2 = fscanf (fp, "\n%i %i %i %s", &i1, &i7, &i8, str);
  printf ("%i %i %i %s\n", i1, i7, i8, str);
  while (i2 == 4)
    {
      tlkup[i1] = i7;
      tid[i1] = i8;
      i2 = fscanf (fp, "\n%i %i %i %s", &i1, &i7, &i8, str);
      printf ("%i %i %i %s\n", i1, i7, i8, str);
    };
  fclose (fp);


};

/* ----------------------------------------------------------------- */

int
DGSEvDecompose_v3 (unsigned int *ev, int len, DGSEVENT * DGSEvent)
{

  /* firmware circa 9/2013 */

  /* declarations */

  int i, k, i1;
  unsigned int ui0 = 0, ui1 = 0, ui2 = 0;
  unsigned int PRE_RISE_SUM = 0, POST_RISE_SUM = 0;
  int rawE;
  unsigned int t1 = 0, t2 = 0, t3 = 0, t4 = 0;
  unsigned long long int ulli1;


  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered DGSEvDecompose_v3:\n");

  /* swap the bytes */

  i = 0;
  while (i < len)
    {

      /* before 4 3 2 1 */
      /*        | | | | */
      /* after  1 2 3 4 */

      t1 = (*(ev + i) & 0x000000ff) << 24;
      t2 = (*(ev + i) & 0x0000ff00) << 8;
      t3 = (*(ev + i) & 0x00ff0000) >> 8;
      t4 = (*(ev + i) & 0xff000000) >> 24;
      *(ev + i) = t1 + t2 + t3 + t4;

      i++;
    }

  /* debug print */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    {
      printf ("event len=%i (%i bytes) >\n", len, len * sizeof (unsigned int));
      for (i = 0; i < len; i++)
        {
          printf ("%3i[doc: %3i]: %12u, 0x%8.8x; ", i, i + 1, *(ev + i), *(ev + i));
          ui0 = 0x80000000;
          printf ("|");
          for (k = 0; k <= 31; k++)
            {
              if ((*(ev + i) & ui0) == ui0)
                printf ("1");
              else
                printf ("0");
              ui0 = ui0 / 2;
              if ((k + 1) % 4 == 0)
                printf ("|");
            };
          printf ("\n");
        };
    };

  /* extract IDs */

  DGSEvent->chan_id = (*(ev + 0) & 0x0000000f);
  DGSEvent->board_id = ((*(ev + 0) >> 4) & 0xfff);
  DGSEvent->id = DGSEvent->board_id * 10 + DGSEvent->chan_id;

  /* store the type and type ID */

  DGSEvent->tpe = tlkup[DGSEvent->id];
  DGSEvent->tid = tid[DGSEvent->id];

  if (Pars.CurEvNo <= Pars.NumToPrint)
    {
      printf ("chan_id = %i, board_id=%i, id=%i\n", DGSEvent->chan_id, DGSEvent->board_id, DGSEvent->id);
    }

  /* extract the energy */

  PRE_RISE_SUM = *(ev + 7) & 0x00ffffff;

  if (Pars.CurEvNo <= Pars.NumToPrint)
    {
      printf ("PRE_RISE_SUM =  0x%8.8x %12i  ", PRE_RISE_SUM, PRE_RISE_SUM);
      ui0 = 0x80000000;
      printf ("|");
      for (k = 0; k <= 31; k++)
        {
          if ((PRE_RISE_SUM & ui0) == ui0)
            printf ("1");
          else
            printf ("0");
          ui0 = ui0 / 2;
          if ((k + 1) % 4 == 0)
            printf ("|");
        };
      printf ("\n");
    };

  ui1 = *(ev + 7) & 0xff000000;
  ui1 >>= 24;
  ui2 = *(ev + 8) & 0x0000ffff;
  ui2 <<= 8;
  POST_RISE_SUM = ui1 + ui2;

  if (Pars.CurEvNo <= Pars.NumToPrint)
    {
      printf ("POST_RISE_SUM = 0x%8.8x %12i  ", POST_RISE_SUM, POST_RISE_SUM);
      ui0 = 0x80000000;
      printf ("|");
      for (k = 0; k <= 31; k++)
        {
          if ((POST_RISE_SUM & ui0) == ui0)
            printf ("1");
          else
            printf ("0");
          ui0 = ui0 / 2;
          if ((k + 1) % 4 == 0)
            printf ("|");
        };
      printf ("\n");
    };

  /* note: POS/PRE_RISE_SUM only have 24 bits */
  /* so making ints of them is not a problem */

  rawE = (int) POST_RISE_SUM - (int) PRE_RISE_SUM;
  DGSEvent->ehi = rawE / 800;

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("rawE = 0x%8.8x %i, DGSEvent->ehi= %i\n", rawE, rawE, DGSEvent->ehi);

  /* extract the LED time stamp */

  DGSEvent->LEDts = 0;
  DGSEvent->LEDts = (unsigned long long int) *(ev + 1);
  ulli1 = (unsigned long long int) (*(ev + 2) & 0x0000ffff);
  ulli1 = (ulli1 << 32);
  DGSEvent->LEDts += ulli1;

  /* done */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit DGSEvDecompose_v3:\n");

  return (0);

}

/* ----------------------------------------------------------------- */

int
bin_dgs (GEB_EVENT * GEB_event)
{

  /* declarations */

  char str[128];
  int i, i1, ng;
  unsigned int ui1;
  DGSEVENT DGSEvent[MAXCOINEV];


  /* prototypes */

  int GebTypeStr (int type, char str[]);




  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered bin_dgs:\n");


  /* loop through the coincidence event and fish out DGS data */
  /* (gamma rays) count in ng */

  ng = 0;
  for (i = 0; i < GEB_event->mult; i++)
    {

      if (GEB_event->ptgd[i]->type == GEB_TYPE_DGS)
        {

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              GebTypeStr (GEB_event->ptgd[i]->type, str);
              printf ("bin_mode1, %2i> %2i, %s, TS=%lli\n", i, GEB_event->ptgd[i]->type, str,
                      GEB_event->ptgd[i]->timestamp);
            }

          DGSEvDecompose_v3 ((unsigned int *) GEB_event->ptinp[i], GEB_event->ptgd[i]->length / sizeof (unsigned int),
                             &DGSEvent[ng]);

          ng++;


          /* debug list the gamma rays we found */

          if (Pars.CurEvNo <= Pars.NumToPrint)
            for (i = 0; i < ng; i++)
              {
                printf ("we have %i gamma rays\n", ng);
                printf ("%2i> ", i);
                printf ("chan_id=%i; ", DGSEvent[i].chan_id);
                printf ("board_id=%i; ", DGSEvent[i].board_id);
                printf ("id =%i; ", DGSEvent[i].id);
                printf ("tpe=%i; ", DGSEvent[i].tpe);
                printf ("tid=%i; ", DGSEvent[i].tid);
                printf ("LEDTS=%llu; ", DGSEvent[i].LEDts);
                printf ("ehi=%8i ", DGSEvent[i].ehi);
                printf ("\n");
                fflush (stdout);
              };

          /* simple bin */

          for (i = 0; i < ng; i++)
            if (DGSEvent[i].tpe == GE)
              {
                if (DGSEvent[i].ehi > 0 && DGSEvent[i].ehi < LONGLEN)
                  {
                    dgs_sumehi->Fill ((double) DGSEvent[i].ehi, 1);
                    ehi[DGSEvent[i].tid]->Fill ((double) DGSEvent[i].ehi, 1);
                  };
              };

          if (DGSEvent[i].id > 0 && DGSEvent[i].id < NCHANNELS)
            dgshitpat->Fill ((double) DGSEvent[i].id, 1);

          if (DGSEvent[i].tpe == GE && !DGSEvent[i].flag)
            if (DGSEvent[i].tid > 0 && DGSEvent[i].tid < NGE)
              if (DGSEvent[i].ehi > 5)
                {
                  hitpatge->Fill ((double) DGSEvent[i].tid, 1);
//                            hpge[DGSEvent[i].tid]++;
                };

          if (DGSEvent[i].tpe == BGO)
            if (DGSEvent[i].tid > 0 && DGSEvent[i].tid < NGE)
              hitpatbgo->Fill ((double) DGSEvent[i].tid, 1);

        };

    };

  /* done */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit bin_dgs\n");

  return (0);

}
