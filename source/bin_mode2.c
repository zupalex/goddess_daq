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

float egamBinWidth;

/* pointers to ROOT spectra */
/* __spectra affected by TS+T0 time gating */

TH1D *CCsum;
TH1D *CCsum_nodop;
TH1D *CCadd;
TH1D *radius_all;
TH2F *CCe;
TH2F *ggCC;
TH2F *SMAP_allhits;
TH1D *hitpat;
TH1D *CCmult;
TH1D *rate_mode2;
TH2F *evsr_all;
TH2F *z_plot;
TH2F *seghitpat;
TH1D *T0;

TH2F *xy_plot_a;
TH2F *xy_plot_a_1;
TH2F *xy_plot_a_2;
TH2F *xy_plot_a_3;
TH2F *xy_plot_a_4;
TH2F *xy_plot_a_5;
TH2F *xy_plot_a_6;

TH2F *xy_plot_b;
TH2F *xy_plot_b_1;
TH2F *xy_plot_b_2;
TH2F *xy_plot_b_3;
TH2F *xy_plot_b_4;
TH2F *xy_plot_b_5;
TH2F *xy_plot_b_6;


/* parameters */

extern PARS Pars;
extern TH1D *ehi[MAXDETPOS + 1];

/* for mean polar and azimuth angles */

double pol[MAXDETPOS + 1];
double azi[MAXDETPOS + 1];
long long int ndethits[MAXDETPOS + 1];

unsigned int modhit[MAXGTMODNO + 1][MAXCRYSTALNO + 1];

extern unsigned int *veto_cube;

/* ----------------------------------------------------------------- */

int
sup_mode2 ()
{
  /* declarations */

  char str1[STRLEN], str2[STRLEN];
  float pi;
  int i, j, i1;

  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

  /* initialize */

  for (i = 0; i <= MAXDETPOS; i++)
    {
      pol[i] = 0;
      azi[i] = 0;
      ndethits[i] = 0;
    };

  for (i = 0; i <= MAXGTMODNO; i++);
  for (j = 0; j <= MAXCRYSTALNO; j++)
    modhit[i][j] = 0;

  /* define spectra */

  sprintf (str1, "T0");
  sprintf (str2, "T0");
  T0 = mkTH1D (str1, str2, 2048, 0, 150);
  sprintf (str1, "");
  T0->SetXTitle ("10 nsec");

  sprintf (str1, "hitpat");
  sprintf (str2, "hitpat");
  hitpat = mkTH1D (str1, str2, 200, 1, 200);
  sprintf (str1, "det number");
  hitpat->SetXTitle (str1);

  sprintf (str1, "CCmult");
  sprintf (str2, "CC Multiplicity");
  CCmult = mkTH1D (str1, str2, 21, 0, 20);
  sprintf (str1, "CCmult");
  CCmult->SetXTitle (str1);

  sprintf (str1, "CCsum");
  sprintf (str2, "CCsum");
  CCsum = mkTH1D (str1, str2, LONGLEN, 1, LONGLEN);
  sprintf (str1, "(keV)");
  CCsum->SetXTitle (str1);

  sprintf (str1, "CCsum_nodop");
  sprintf (str2, "CCsum_nodop");
  CCsum_nodop = mkTH1D (str1, str2, LONGLEN, 1, LONGLEN);
  sprintf (str1, "(keV)");
  CCsum_nodop->SetXTitle (str1);

  sprintf (str1, "CCadd");
  sprintf (str2, "CCadd");
  CCadd = mkTH1D (str1, str2, LONGLEN, 1, LONGLEN);
  sprintf (str1, "(keV)");
  CCadd->SetXTitle (str1);

  sprintf (str1, "radius_all");
  sprintf (str2, "radius (all points)");
  radius_all = mkTH1D (str1, str2, 4096, RMIN, RMAX);
  radius_all->SetXTitle (str1);

  sprintf (str1, "rate_mode2");
  sprintf (str2, "rate_mode2");
  rate_mode2 = mkTH1D (str1, str2, RATELEN, 0, RATELEN);
  rate_mode2->SetXTitle (str1);
  rate_mode2->SetXTitle ("minutes");
  rate_mode2->SetYTitle ("Hz");

  /* star map of GRETA */

  sprintf (str1, "evsr_all");
  sprintf (str2, "evsr_all");
  evsr_all = mkTH2F (str1, str2, MEDIUMLEN, RMIN, RMAX, MEDIUMLEN, 1, MEDIUMLEN);
  sprintf (str1, "energy");
  evsr_all->SetXTitle (str1);
  sprintf (str1, "radius");
  evsr_all->SetYTitle (str1);

  sprintf (str1, "SMAP_allhits");
  sprintf (str2, "SMAP_allhits");
  SMAP_allhits = mkTH2F (str1, str2, 720, -180, 180, 360, 0, 180);
  sprintf (str1, "Azimuth");
  SMAP_allhits->SetXTitle (str1);
  sprintf (str1, "Polar");
  SMAP_allhits->SetYTitle (str1);

  sprintf (str1, "seghitpat");
  sprintf (str2, "seghitpat");
  seghitpat = mkTH2F (str1, str2, 180, 1, 180, 38, 0, 37);
  sprintf (str1, "detector");
  seghitpat->SetXTitle (str1);
  sprintf (str1, "segment");
  seghitpat->SetYTitle (str1);

  sprintf (str1, "CCe");
  sprintf (str2, "CCe");
  CCe = mkTH2F (str1, str2, MAXDETPOS, 1, MAXDETPOS, LONGLEN, 1, LONGLEN);
  sprintf (str1, "crystal #");
  CCe->SetXTitle (str1);
  sprintf (str1, "cc energy");
  CCe->SetYTitle (str1);

  sprintf (str1, "ggCC");
  sprintf (str2, "CC gg matrix");
  ggCC = mkTH2F (str1, str2, Pars.GGMAX, 1, Pars.GGMAX, Pars.GGMAX, 1, Pars.GGMAX);
  sprintf (str1, "g1");
  ggCC->SetXTitle (str1);
  sprintf (str1, "g2");
  ggCC->SetYTitle (str1);

  sprintf (str1, "z_plot");
  sprintf (str2, "z_plot");
  z_plot = mkTH2F (str1, str2, NUMAGATAPOS + 1, 0, NUMAGATAPOS, 1000, 0, 100);
  sprintf (str1, "crystal");
  z_plot->SetXTitle (str1);
  sprintf (str1, "z values");
  z_plot->SetYTitle (str1);

  sprintf (str1, "xy_plot_a");
  sprintf (str2, "xy_plot_a");
  xy_plot_a = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_a->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_a->SetYTitle (str1);


  sprintf (str1, "xy_plot_a_1");
  sprintf (str2, "xy_plot_a_1");
  xy_plot_a_1 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_a_1->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_a_1->SetYTitle (str1);

  sprintf (str1, "xy_plot_a_2");
  sprintf (str2, "xy_plot_a_2");
  xy_plot_a_2 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_a_2->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_a_2->SetYTitle (str1);

  sprintf (str1, "xy_plot_a_3");
  sprintf (str2, "xy_plot_a_3");
  xy_plot_a_3 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_a_3->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_a_3->SetYTitle (str1);

  sprintf (str1, "xy_plot_a_4");
  sprintf (str2, "xy_plot_a_4");
  xy_plot_a_4 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_a_4->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_a_4->SetYTitle (str1);

  sprintf (str1, "xy_plot_a_5");
  sprintf (str2, "xy_plot_a_5");
  xy_plot_a_5 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_a_5->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_a_5->SetYTitle (str1);

  sprintf (str1, "xy_plot_a_6");
  sprintf (str2, "xy_plot_a_6");
  xy_plot_a_6 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_a_6->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_a_6->SetYTitle (str1);

  sprintf (str1, "xy_plot_b");
  sprintf (str2, "xy_plot_b");
  xy_plot_b = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_b->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_b->SetYTitle (str1);


  sprintf (str1, "xy_plot_b_1");
  sprintf (str2, "xy_plot_b_1");
  xy_plot_b_1 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_b_1->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_b_1->SetYTitle (str1);

  sprintf (str1, "xy_plot_b_2");
  sprintf (str2, "xy_plot_b_2");
  xy_plot_b_2 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_b_2->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_b_2->SetYTitle (str1);

  sprintf (str1, "xy_plot_b_3");
  sprintf (str2, "xy_plot_b_3");
  xy_plot_b_3 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_b_3->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_b_3->SetYTitle (str1);

  sprintf (str1, "xy_plot_b_4");
  sprintf (str2, "xy_plot_b_4");
  xy_plot_b_4 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_b_4->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_b_4->SetYTitle (str1);

  sprintf (str1, "xy_plot_b_5");
  sprintf (str2, "xy_plot_b_5");
  xy_plot_b_5 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_b_5->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_b_5->SetYTitle (str1);

  sprintf (str1, "xy_plot_b_6");
  sprintf (str2, "xy_plot_b_6");
  xy_plot_b_6 = mkTH2F (str1, str2, 120, -60, 60, 120, -60, 60);
  sprintf (str1, "crystal");
  xy_plot_b_6->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot_b_6->SetYTitle (str1);

  /* list what we have */

//  Pars.wlist = gDirectory->GetList ();
//  Pars.wlist->Print ();

  for (i = 1; i <= MAXDETPOS; i++)
    Pars.detpolang[i] = 0;
  printf ("MAXDETPOS=%i\n", MAXDETPOS);
  fflush (stdout);

}

/* ----------------------------------------------------------------- */

int
exit_mode2 (void)
{

/* declarations*/

  int i, j, mod, crystal, ncrystals = 0;
  double csum, mcsum, d1;
  int k, l, m;
  unsigned int ibad, icsum, nbadcubes = 0, ntotcubes = 0;
  FILE *fp;
  unsigned int ui1;

  /* normalize and report */

  printf ("\n");
  printf ("mean observed polar and azimuth angles\n");
  printf ("for the detectors we saw (module,crystal)\n");
  printf ("\n");
  printf ("------------------------------------------\n");

  for (i = 0; i <= MAXDETPOS; i++)
    if (ndethits[i] > 0)
      {
        ncrystals++;
        pol[i] /= ndethits[i];
        azi[i] /= ndethits[i];
        mod = i / 4;
        crystal = i - 4 * mod;
        printf ("mean pol/azi angle for detector %3i (%2i,%1i) are %7.2f/%7.2f\n", i, mod, crystal,
                pol[i] * 180.0 / M_PI, azi[i] * 180.0 / M_PI);
      };

  printf ("we found %i crystals\n", ncrystals);
  printf ("------------------------------------------\n");
  printf ("\n");

  if (Pars.vetoSpots)
    {

      printf ("we may have to evaluate as a fuction of Z\n");
      printf ("rather than base on mean from entire crystal...\n");
      printf ("\n");


      fp = fopen (Pars.vetoSpotsfn, "w");
      if (fp == NULL)
        {
          printf ("could not open %s\n", Pars.vetoSpotsfn);
          exit (1);
        }
      else
        printf ("open %s for bad cube listings\n", Pars.vetoSpotsfn);

      for (i = 0; i <= MAXGTMODNO; i++)
        for (j = 0; j <= MAXCRYSTALNO; j++)
          {

#if(0)
            csum = 0;
            for (k = 0; k <= VETO_NX; k++)
              for (l = 0; l <= VETO_NY; l++)
                for (m = 0; m <= VETO_NZ; m++)
                  csum += *(veto_cube + VETO_INDX (i, j, k, l, m));
#else
            csum = modhit[i][j];
#endif

            if (csum > 0)
              {
                mcsum = csum / VETO_NX / VETO_NY / VETO_NZ;
                printf ("mod %2i, crystal %1i, has %6.2f counts, mean %f, ", i, j, csum, (float) mcsum);
                fflush (stdout);
                ntotcubes += VETO_NX * VETO_NY * VETO_NZ;
              };

            if (csum > 0)
              {
                ibad = 0;

                /* now write out the bad cubes only */

                for (k = 0; k <= VETO_NX; k++)
                  for (l = 0; l <= VETO_NY; l++)
                    for (m = 0; m <= VETO_NZ; m++)
                      {
                        icsum = (int) (Pars.vetocutfac[m] * mcsum + 0.5);
                        if ((int) *(veto_cube + VETO_INDX (i, j, k, l, m)) > icsum)
                          {
                            ibad++;
                            nbadcubes++;
                            fprintf (fp, "%2i %2i %3i %3i %3i ; %3i\n", i, j, k, l, m,
                                     *(veto_cube + VETO_INDX (i, j, k, l, m)));
//                          fprintf (fp, "%5.1f %5.1f %5.1f\n", VETO_X_XEDNI (k), VETO_Y_XEDNI (l), VETO_Z_XEDNI (m));
                          };
                      };
                d1 = (double) ibad / VETO_NX / VETO_NY / VETO_NZ;
                printf ("there are %6i bad cubes; %6.2f%%\n", ibad, 100 * d1);
              };

          };
      fclose (fp);
      printf ("\n");
      printf ("bin_mode2: closed %s\n", Pars.vetoSpotsfn);
      printf ("\n");

      printf ("VETO_INDX (7, 3, 10, 10, 10) =%i\n", VETO_INDX (7, 3, 10, 10, 10));
      printf ("VETO_INDX (8, 0, 10, 10, 10) =%i\n", VETO_INDX (8, 0, 10, 10, 10));
      printf ("VETO_INDX (8, 1, 10, 10, 10) =%i\n", VETO_INDX (8, 1, 10, 10, 10));
      printf ("VETO_INDX (8, 2, 10, 10, 10) =%i\n", VETO_INDX (8, 2, 10, 10, 10));
      printf ("VETO_INDX (8, 3, 10, 10, 10) =%i\n", VETO_INDX (8, 3, 10, 10, 10));

      ui1 = MAXGTMODNO * MAXCRYSTALNO * VETO_NX * VETO_NY * VETO_NZ;
      printf ("max index...: %10u\n", ui1);
      printf ("ntotcubes...= %10i\n", ntotcubes);
      d1 = 100 * (double) nbadcubes / ntotcubes;
      printf ("nbadcubes...= %10i or %6.2f%%\n", nbadcubes, d1);
      printf ("VETO_X_D = %f\n", VETO_X_D);
      printf ("VETO_Y_D = %f\n", VETO_Y_D);
      printf ("VETO_Z_MAX = %f\n", VETO_Z_MAX);
      printf ("VETO_BINWIDTH = %f mm\n", VETO_BINWIDTH);
      printf ("VETO_NX = %i\n", VETO_NX);
      printf ("VETO_NY = %i\n", VETO_NY);
      printf ("VETO_NZ = %i\n", VETO_NZ);
      printf ("\n");
    };

  printf ("module hit statistics\n");
  for (i = 0; i <= MAXGTMODNO; i++)
    for (j = 0; j <= MAXCRYSTALNO; j++)
      if (modhit[i][j] > 0)
        printf ("module %3i, crystal %i was hit %10i times\n", i, j, modhit[i][j]);
  printf ("\n");

  return (0);
}

/* ----------------------------------------------------------------- */

int
bin_mode2 (GEB_EVENT * GEB_event)
{

  /* declarations */

  int i, j, crystalno, moduleno, detno, nn;
  float sX, sY, polAng, aziAng, rr, xx, yy, zz, r1, rmax, rmin;
  double d1, d2;
  char str[128];
  int GebTypeStr (int type, char str[]);
  float detDopFac, dp, addedEnergy = 0, r2, orig_seg_e;
  float RAD2DEG = 0.0174532925;
  float CCenergies[MAX_GAMMA_RAYS];
  static int firsttime = 1;
  static long long int t0;
  float polang[MAX_GAMMA_RAYS];
  float doppler_factor[MAX_GAMMA_RAYS];
  float xar[MAXCOINEV], yar[MAXCOINEV], zar[MAXCOINEV];
  int detectorPosition, crystalNumber;
  int i1, i2, i3;
  int nCCenergies, crystaltype;
  static int nperrors = 0;
  unsigned int ui1;

  CRYS_INTPTS *ptinp;
  GEBDATA *ptgd;

  /* prototypes */

  float findAzimuthFromCartesian (float, float, float);
  float findPolarFromCartesian (float, float, float, float *);

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered bin_mode2: %i/%i\n", Pars.CurEvNo, Pars.NumToPrint);

  if (Pars.requiretracked)
    {

      /* require tracked data before we bin mode2 data */

      i1 = 0;
      for (i = 0; i < GEB_event->mult; i++)
        if (GEB_event->ptgd[i]->type == GEB_TYPE_TRACK)
          i1++;
      if (i1 == 0)
        return (0);

    }


  /* main loop */

  addedEnergy = 0;
  for (i = 0; i < GEB_event->mult; i++)
    CCenergies[i] = 0;
  nCCenergies = 0;

  for (i = 0; i < GEB_event->mult; i++)
    {
      if (GEB_event->ptgd[i]->type == GEB_TYPE_DECOMP)
        {

          /* cast */

          ptinp = (CRYS_INTPTS *) GEB_event->ptinp[i];

          /* debug print */

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              GebTypeStr (GEB_event->ptgd[i]->type, str);
              printf ("bin_mode2, %2i> %2i, %s, TS=%lli\n", i, GEB_event->ptgd[i]->type, str,
                      GEB_event->ptgd[i]->timestamp);
            }

          /* extract IDs */

          crystalno = (ptinp->crystal_id & 0x0003);
          moduleno = ((ptinp->crystal_id & 0xfffc) >> 2);
          modhit[moduleno][crystalno] += ptinp->num;

          if (Pars.AGATA_data == 0)
            {
              detno = moduleno * 4 + crystalno;
              if (crystalno == 0 || crystalno == 2)
                crystaltype = 0;
              else
                crystaltype = 1;
            }
          else if (Pars.AGATA_data == 1)
            {
              detno = moduleno * 3 + crystalno;
              crystaltype = 0;
            };

          /* 2D segment hit matrix */

          for (j = 0; j < ptinp->num; j++)
            {
              seghitpat->Fill (detno, ptinp->intpts[j].seg, 1);
            };

          /* bin if enabled */

          if (Pars.enabled[detno])
            {
              /* mode 2 rate spectrum, x=minute, y=Hz */

              if (firsttime)
                {
                  firsttime = 0;
                  t0 = GEB_event->ptgd[i]->timestamp;
                };
              d1 = (double) (GEB_event->ptgd[i]->timestamp - t0);
              d1 /= 100000000;
              d1 /= 60;
              if (d1 > 0 && d1 < (double) RATELEN)
                rate_mode2->Fill (d1, 1 / 60.0);

              /* update veto_cube */

              if (Pars.vetoSpots)
                for (j = 0; j < ptinp->num; j++)
                  if (ptinp->intpts[j].e <= Pars.vetoecut)
                    {
                      i1 = VETO_X_INDEX (ptinp->intpts[j].x);
                      i2 = VETO_Y_INDEX (ptinp->intpts[j].y);
                      i3 = VETO_Z_INDEX (ptinp->intpts[j].z);

                      if (i1 >= 0 && i2 >= 0 && i3 >= 0)
                        if (i1 < VETO_NX && i2 < VETO_NY && i3 < VETO_NZ)
                          {
                            (*(veto_cube + VETO_INDX (moduleno, crystalno, i1, i2, i3)))++;
                          };
                    };

              /* make crystal z_plot and xy_plot */

              for (j = 0; j < ptinp->num; j++)
                {


                  if (Pars.AGATA_data == 0)
                    z_plot->Fill ((double) (moduleno * 4 + crystalno), (double) ptinp->intpts[j].z, 1.0);
                  else if (Pars.AGATA_data == 1)
                    z_plot->Fill ((double) (moduleno * 3 + crystalno), (double) ptinp->intpts[j].z, 1.0);

                  if (crystaltype == 0)
                    xy_plot_a->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                  else
                    xy_plot_b->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);

                  /* bin xy per segment slice along z. */
                  /* we bin the two crystal types on  */
                  /* top af one another for now */

                  if (Pars.AGATA_data == 0)
                    i1 = (ptinp->intpts[j].seg + 1) / 6 + 1;
                  else if (Pars.AGATA_data == 1)
                    i1 = ptinp->intpts[j].seg % 6 + 1;

                  switch (i1)
                    {
                    case 1:
                      if (crystaltype == 0)
                        xy_plot_a_1->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      else
                        xy_plot_b_1->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      break;
                    case 2:
                      if (crystaltype == 0)
                        xy_plot_a_2->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      else
                        xy_plot_b_2->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      break;
                    case 3:
                      if (crystaltype == 0)
                        xy_plot_a_3->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      else
                        xy_plot_b_3->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      break;
                    case 4:
                      if (crystaltype == 0)
                        xy_plot_a_4->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      else
                        xy_plot_b_4->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      break;
                    case 5:
                      if (crystaltype == 0)
                        xy_plot_a_5->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      else
                        xy_plot_b_5->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      break;
                    case 6:
                      if (crystaltype == 0)
                        xy_plot_a_6->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      else
                        xy_plot_b_6->Fill ((double) ptinp->intpts[j].x, (double) ptinp->intpts[j].y, 1.0);
                      break;
                    };

                };

              if (Pars.CurEvNo <= Pars.NumToPrint)
                {
                  printf ("* %i/%i, is GEB_TYPE_DECOMP: num=%i\n", i, GEB_event->mult, ptinp->num);
                  printf ("__detno: %i, module: %i, crystalNumber: %i\n", detno, moduleno, crystalno);
                }

              /* calibrate mode 2 CC data */

              ptinp->tot_e = ptinp->tot_e * Pars.CCcal_gain[detno] + Pars.CCcal_offset[detno];
              addedEnergy += ptinp->tot_e;
              CCenergies[nCCenergies] = ptinp->tot_e;
              if (Pars.CurEvNo <= Pars.NumToPrint)
                printf ("CCenergies[%i]=%f\n", nCCenergies, CCenergies[nCCenergies]);

              nCCenergies++;

              /* calibrate mode2 segment energies */

              for (j = 0; j < ptinp->num; j++)
                {
                  ptinp->intpts[j].e = ptinp->intpts[j].e * Pars.SEGcal_gain[detno][ptinp->intpts[j].seg]
                    + Pars.SEGcal_offset[detno][ptinp->intpts[j].seg];
                }

              /* quietly rescale all interaction energies to the CC energy */

              r1 = 0;
              for (j = 0; j < ptinp->num; j++)
                r1 += ptinp->intpts[j].e;
              for (j = 0; j < ptinp->num; j++)
                ptinp->intpts[j].e = ptinp->intpts[j].e / r1 * ptinp->tot_e;


              ptinp->tot_e = 0;
              for (j = 0; j < ptinp->num; j++)
                ptinp->tot_e += (ptinp->intpts[j].e);

              /* store original/calibrated segment energy for later use */

              orig_seg_e = ptinp->intpts[0].e;

              /* hit pattern */

              hitpat->Fill ((double) detno, 1);

              /* Now rotate into world coordinates */
              /* from now on x,y,z will be world coordinates */

              for (j = 0; j < ptinp->num; j++)
                {

                  if (Pars.nocrystaltoworldrot == 0)
                    {

                      if (Pars.AGATA_data == 0)
                        {

                          /* rotate into world coordinates first */
                          /* and make it cm rather than mm because */
                          /* crmat needs it in cm */

                          if (Pars.CurEvNo <= Pars.NumToPrint)
                            {
                              printf ("* %i: ", j);
                              printf ("%7.2f,%7.2f,%7.2f --> ", ptinp->intpts[j].x, ptinp->intpts[j].y,
                                      ptinp->intpts[j].z);
                            };

                          xx = ptinp->intpts[j].x / 10.0;
                          yy = ptinp->intpts[j].y / 10.0;
                          zz = ptinp->intpts[j].z / 10.0;


                          ptinp->intpts[j].x = Pars.crmat[moduleno][crystalno][0][0] * xx
                            + Pars.crmat[moduleno][crystalno][0][1] * yy
                            + Pars.crmat[moduleno][crystalno][0][2] * zz + Pars.crmat[moduleno][crystalno][0][3];

                          ptinp->intpts[j].y = Pars.crmat[moduleno][crystalno][1][0] * xx
                            + Pars.crmat[moduleno][crystalno][1][1] * yy
                            + Pars.crmat[moduleno][crystalno][1][2] * zz + Pars.crmat[moduleno][crystalno][1][3];

                          ptinp->intpts[j].z = Pars.crmat[moduleno][crystalno][2][0] * xx
                            + Pars.crmat[moduleno][crystalno][2][1] * yy
                            + Pars.crmat[moduleno][crystalno][2][2] * zz + Pars.crmat[moduleno][crystalno][2][3];

                          if (Pars.CurEvNo <= Pars.NumToPrint)
                            {
                              printf ("%7.2f,%7.2f,%7.2f\n", ptinp->intpts[j].x, ptinp->intpts[j].y,
                                      ptinp->intpts[j].z);
                            }

                        }
                      else if (Pars.AGATA_data == 1)
                        {
                          detno = moduleno * 3 + crystalno;


                          xx = ptinp->intpts[j].x;
                          yy = ptinp->intpts[j].y;
                          zz = ptinp->intpts[j].z;

                          ptinp->intpts[j].x =
                            Pars.rotxx[detno] * xx + Pars.rotxy[detno] * yy + Pars.rotxz[detno] * zz + Pars.TrX[detno];
                          ptinp->intpts[j].y =
                            Pars.rotyx[detno] * xx + Pars.rotyy[detno] * yy + Pars.rotyz[detno] * zz + Pars.TrY[detno];;
                          ptinp->intpts[j].z =
                            Pars.rotzx[detno] * xx + Pars.rotzy[detno] * yy + Pars.rotzz[detno] * zz + Pars.TrZ[detno];;


                          if (Pars.CurEvNo <= Pars.NumToPrint)
                            {
                              printf ("AG::x: %9.2f --> %9.2f\n", xx, ptinp->intpts[j].x);
                              printf ("AG::y: %9.2f --> %9.2f\n", yy, ptinp->intpts[j].y);
                              printf ("AG::z: %9.2f --> %9.2f\n", zz, ptinp->intpts[j].z);
                              r1 = xx * xx + yy * yy + zz * zz;
                              r1 = sqrtf (r1);
                              r2 = ptinp->intpts[j].x * ptinp->intpts[j].x
                                + ptinp->intpts[j].y * ptinp->intpts[j].y + ptinp->intpts[j].z * ptinp->intpts[j].z;
                              r2 = sqrtf (r2);
                              printf ("AG::radius %f --> %f\n", r1, r2);
                            }

                          ptinp->intpts[j].x /= 10;
                          ptinp->intpts[j].y /= 10;
                          ptinp->intpts[j].z /= 10;


                        }

                      else
                        {
                          /* no rotation case, just make it cm */

                          xx = ptinp->intpts[j].x / 10.0;
                          yy = ptinp->intpts[j].y / 10.0;
                          zz = ptinp->intpts[j].z / 10.0;

                        };

                    };

                };

              /* doppler correction (this is not the only way to do this!) */

              for (j = 0; j < ptinp->num; j++)
                {

                  rr =
                    (ptinp->intpts[j].x - Pars.target_x/10) * (ptinp->intpts[j].x - Pars.target_x/10) +
                    (ptinp->intpts[j].y - Pars.target_y/10) * (ptinp->intpts[j].y - Pars.target_y/10) +
                    (ptinp->intpts[j].z - Pars.target_z/10) * (ptinp->intpts[j].z - Pars.target_z/10);
                  rr = sqrtf (rr);

//printf("ptinp->intpts[j].x=%f, ptinp->intpts[j].x - Pars.target_x=%f\n",ptinp->intpts[j].x,ptinp->intpts[j].x - Pars.target_x);
//printf("ptinp->intpts[j].y=%f, ptinp->intpts[j].y - Pars.target_y=%f\n",ptinp->intpts[j].y,ptinp->intpts[j].y - Pars.target_y);
//printf("ptinp->intpts[j].z=%f, ptinp->intpts[j].z - Pars.target_z=%f\n",ptinp->intpts[j].z,ptinp->intpts[j].z - Pars.target_z);


                  if (rr > RMIN && rr < RMAX)
                    {
                      radius_all->Fill ((double) rr, 1);
                      if (ptinp->intpts[j].e > 0 && ptinp->intpts[j].e < MEDIUMLEN);
                        evsr_all->Fill ((double) rr, ptinp->intpts[j].e);
                    };

                  dp = ((ptinp->intpts[j].x - Pars.target_x/10) * Pars.beamdir[0] +
                        (ptinp->intpts[j].y - Pars.target_y/10) * Pars.beamdir[1] +
                        (ptinp->intpts[j].z - Pars.target_z/10) * Pars.beamdir[2]) / rr;

                  if (dp < -1.0)
                    dp = -1.0;
                  if (dp > 1.0)
                    dp = 1.0;
                  polang[j] = acosf (dp);

                  rr = 1.0 - Pars.beta * Pars.beta;
                  doppler_factor[j] = sqrt (rr) / (1.0 - Pars.beta * cos (polang[j]));

                  /* central contact energy matrix and total energy */
                  /* the 1.0/ptinp->num preserves the counts */

                  if (detno > 0 && detno < MAXDETPOS)
                    if (ptinp->tot_e > 0 && ptinp->tot_e < (LONGLEN+30))
                      {
                        CCsum_nodop->Fill (ptinp->tot_e, 1.0/ptinp->num);
                        CCsum->Fill ((double) ptinp->tot_e / doppler_factor[j], 1.0/ptinp->num );
                        CCe->Fill ((double) detno, (double) ptinp->tot_e/ doppler_factor[j],1.0/ptinp->num );
                      };

                };

              /* worldmap all hits */
              /* target position does not get into this */

              for (j = 0; j < ptinp->num; j++)
                {

                  polAng =
                    findPolarFromCartesian (ptinp->intpts[j].x, ptinp->intpts[j].y,
                                            ptinp->intpts[j].z, &rr);
                  aziAng =
                    findAzimuthFromCartesian (ptinp->intpts[j].x, ptinp->intpts[j].y,
                                              ptinp->intpts[j].z);

                  ndethits[detno]++;
                  pol[detno] += polAng;
                  azi[detno] += aziAng;


                  /* SMAP coordinates */

                  sX = aziAng * sinf (polAng) / RAD2DEG;
                  sY = polAng / RAD2DEG;        /* + 1.5; */

                  if (Pars.CurEvNo <= Pars.NumToPrint && 0)
                    {
                      printf ("%i [type %i] ", j, GEB_event->ptgd[i]->type);
                      printf ("e: %9.2f/%9.2f ", ptinp->intpts[j].e, ptinp->tot_e);
                      printf ("(%6.2f,%6.2f,%6.2f)cry --> ", xx, yy, zz);
                      printf ("(%6.2f,%6.2f,%6.2f)world(cm); ", ptinp->intpts[j].x, ptinp->intpts[j].y,
                              ptinp->intpts[j].z);
//                    printf (" sX,sY=%6.2f,%6.2f ", sX, sY);
                      printf ("\n");
                    };

                  /* update */

                  if (sX >= -180 && sX <= 180 && sY >= 0 && sY <= 180)
                    SMAP_allhits->Fill (sX, sY, 1);
                  else
                    {
                      if (nperrors < 10)
                        {
                          nperrors++;
                          printf ("error: sX,sY= ( %11.6f , %11.6f )\n", sX, sY);
//                          exit (1);
                        };
                    };


                };              /* for (j = 0; j < ptinp->num; j++) */



            };                  /* if (Pars.enabled[detno]) */

        };                      /* if (GEB_event->ptgd[i]->type == GEB_TYPE_DECOMP) */

    };                          /* for (i = 0; i < GEB_event->mult; i++) */



// if (Pars.CurEvNo >2000) exit(0);

  /* update added energy spectrum */

  CCadd->Fill ((double) addedEnergy, 1);

  /* fill the ggCC martrix */

  CCmult->Fill (nCCenergies, 1);
  if (nCCenergies >= Pars.multlo && nCCenergies <= Pars.multhi)
    for (i = 0; i < nCCenergies; i++)
      for (j = i + 1; j < nCCenergies; j++)
        {
          ggCC->Fill (CCenergies[i], CCenergies[j], 1.0);
          ggCC->Fill (CCenergies[j], CCenergies[i], 1.0);
        };

  /* done */

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("exit bin_mode2\n");

  return (0);

}
