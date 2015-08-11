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

#include "GEBSort.h"
#include "GTMerge.h"

float egamBinWidth;

/* pointers to ROOT spectra */

TH2F *SMAP_allhits;
TH1D *hitpat;
TH1D *CCmult;
TH1D *CCsum;
TH1D *CCsum_s;
TH1D *CCadd;
TH1D *radius_all;
TH1D *rate_mode2;
TH2F *CCe;
TH2F *ggCC;
TH2F *evsr_all;
TH2F *z_plot;
TH2F *xy_plot;

/* parameters */

extern PARS Pars;
extern TH1D *ehi[MAXDETPOS + 1];

/* for mean polar and azimuth angles */

double pol[MAXDETPOS + 1];
double azi[MAXDETPOS + 1];
long long int ndethits[MAXDETPOS + 1];

/* ----------------------------------------------------------------- */

int
sup_mode2 ()
{
  /* declarations  else
     {
     printf ("Error: ascii AGATA_crmat.dat not found, quit\n");
     exit (1);
     };
   */

  char str1[STRLEN], str2[STRLEN];
  //float pi, il;//unused
  int i;

  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

  /* initialize */

  for (i = 0; i <= MAXDETPOS; i++)
    {
      pol[i] = 0;
      azi[i] = 0;
      ndethits[i] = 0;
    };

	gDirectory->mkdir("bin_mode2");
	gDirectory->cd("bin_mode2");

  /* define spectra */

  sprintf (str1, "hitpat");
  sprintf (str2, "hitpat");
  hitpat = mkTH1D (str1, str2, 200, 1, 200);
  sprintf (str1, "det number");
  hitpat->SetXTitle (str1);

  sprintf (str1, "CCmult");
  sprintf (str2, "CCmult");
  CCmult = mkTH1D (str1, str2, 21, 0, 20);
  sprintf (str1, "CCmult");
  CCmult->SetXTitle (str1);

  sprintf (str1, "CCsum");
  sprintf (str2, "CCsum");
  CCsum = mkTH1D (str1, str2, LONGLEN, 1, LONGLEN);
  sprintf (str1, "(keV)");
  CCsum->SetXTitle (str1);

  sprintf (str1, "CCsum_s");
  sprintf (str2, "CCsum_s");
  CCsum_s = mkTH1D (str1, str2, LONGLEN, 1, LONGLEN);
  sprintf (str1, "(keV)");
  CCsum_s->SetXTitle (str1);

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

  sprintf (str1, "CCe");
  sprintf (str2, "CCe");
  CCe = mkTH2F (str1, str2, MAXDETPOS, 1, MAXDETPOS, LONGLEN, 1, LONGLEN);
  sprintf (str1, "crystal #");
  CCe->SetXTitle (str1);
  sprintf (str1, "cc energy");
  CCe->SetYTitle (str1);

  sprintf (str1, "ggCC");
  sprintf (str2, "ggCC");
  ggCC = mkTH2F (str1, str2, Pars.GGMAX, 1, Pars.GGMAX, Pars.GGMAX, 1, Pars.GGMAX);
  sprintf (str1, "g1");
  ggCC->SetXTitle (str1);
  sprintf (str1, "g2");
  ggCC->SetYTitle (str1);

  sprintf (str1, "z_plot");
  sprintf (str2, "z_plot");
  z_plot = mkTH2F (str1, str2, NUMAGATAPOS+1, 0, NUMAGATAPOS, 1024, 0, 100);
  sprintf (str1, "crystal");
  z_plot->SetXTitle (str1);
  sprintf (str1, "z values");
  z_plot->SetYTitle (str1);

  sprintf (str1, "xy_plot");
  sprintf (str2, "xy_plot");
  xy_plot = mkTH2F (str1, str2, 1024, -60, 60, 1024, -60, 60);
  sprintf (str1, "crystal");
  xy_plot->SetXTitle (str1);
  sprintf (str1, "z values");
  xy_plot->SetYTitle (str1);

	gDirectory->cd("/");

  /* list what we have */

//  Pars.wlist = gDirectory->GetList ();
//  Pars.wlist->Print ();

  for (i = 1; i <= MAXDETPOS; i++)
    Pars.detpolang[i] = 0;
  printf ("MAXDETPOS=%i\n", MAXDETPOS);
  fflush (stdout);

  return (0);

}

/* ----------------------------------------------------------------- */

int
exit_mode2 (void)
{

/* declarations*/

  int i, mod, crystal;

/* normalize and report */

  printf ("\n");
  printf ("mean observed polar and azimuth angles\n");
  printf ("for the detectors we saw (module,crystal)\n");
  printf ("\n");
  printf ("------------------------------------------\n");

  for (i = 0; i <= MAXDETPOS; i++)
    if (ndethits[i] > 0)
      {
        pol[i] /= ndethits[i];
        azi[i] /= ndethits[i];
        mod = i / 4;
        crystal = i - 4 * mod;
        printf ("mean pol/azi angle for detector %3i (%2i,%1i) are %7.2f/%7.2f\n", i, mod, crystal, pol[i] * 57.2958,
                azi[i] * 57.2958);
      };

  printf ("------------------------------------------\n");
  printf ("\n");

  return (0); 

}

/* ----------------------------------------------------------------- */

int
bin_mode2 (GEB_EVENT * GEB_event)
{

  /* declarations */
  //int nn, rmax,rmin;//unused
  int i, j, crystalno, moduleno, detno;
  float sX, sY, polAng, aziAng, rr, xx, yy, zz, r1;
  //double d2;//unused
  double d1;
  char str[128];
  int GebTypeStr (int type, char str[]);
  //float detDpFac, orig_seg_e;//unused
  float dp, addedEnergy = 0, r2;
  float RAD2DEG = 0.0174532925;
  float CCenergies[MAX_GAMMA_RAYS];
  static int firsttime = 1;
  static long long int t0;
  float polang[MAX_GAMMA_RAYS];
  float doppler_factor[MAX_GAMMA_RAYS];
  //float xar[MAXCOINEV], yar[MAXCOINEV], zar[MAXCOINEV];//unused
  //int detectorPosition, crystalNumber;//unused
  int i1, ndecomp;
  int nCCenergies;
  static int nperrors = 0;

  CRYS_INTPTS *ptinp;
  //GEBDATA *ptgd;//unused

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


  addedEnergy = 0;
  ndecomp = 0;
  for (i = 0; i < GEB_event->mult; i++)
    CCenergies[i] = 0;
  nCCenergies = 0;
  for (i = 0; i < GEB_event->mult; i++)
    {
      if (GEB_event->ptgd[i]->type == GEB_TYPE_DECOMP)
        {
          ndecomp++;

          /* cast */

          ptinp = (CRYS_INTPTS *) GEB_event->ptinp[i];

          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              GebTypeStr (GEB_event->ptgd[i]->type, str);
              printf ("bin_mode2, %2i> %2i, %s, TS=%lli\n", i, GEB_event->ptgd[i]->type, str,
                      GEB_event->ptgd[i]->timestamp);
            }


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

          /* find basic info */

          crystalno = (ptinp->crystal_id & 0x0003);
          moduleno = ((ptinp->crystal_id & 0xfffc) >> 2);
          detno = moduleno * 4 + crystalno;

          /* make z_plot and xy_plot */

          for (j = 0; j < ptinp->num; j++)
            {
            if (Pars.AGATA_data==0)
              z_plot->Fill((double)(moduleno * 4 + crystalno),(double)ptinp->intpts[j].z,1.0);
            else if (Pars.AGATA_data==1)
              z_plot->Fill((double)(moduleno * 3 + crystalno),(double)ptinp->intpts[j].z,1.0);
             xy_plot->Fill((double)ptinp->intpts[j].x,(double)ptinp->intpts[j].y,1.0);
            };


          if (Pars.CurEvNo <= Pars.NumToPrint)
            {
              printf ("* %i/%i, is GEB_TYPE_DECOMP: num=%i\n", i, GEB_event->mult, ptinp->num);
              printf ("__detno: %i, module: %i, crystalNumber: %i\n", detno, moduleno, crystalno);
            }

          /* calibrate mode 2 CC data */

          ptinp->tot_e = ptinp->tot_e * Pars.CCcal_gain[detno] + Pars.CCcal_offset[detno];
          addedEnergy += ptinp->tot_e;
          CCenergies[nCCenergies++] = ptinp->tot_e;
          if (Pars.CurEvNo <= Pars.NumToPrint)
            printf ("CCenergies[%i]=%f\n", nCCenergies - 1, CCenergies[nCCenergies - 1]);

          /* calibrate mode2 segment energies */

          for (j = 0; j < ptinp->num; j++)
            {
//            printf("%3i,%3i: e=%7.1f --> ", detno,ptinp->intpts[j].seg,ptinp->intpts[j].e);
              ptinp->intpts[j].e = ptinp->intpts[j].e * Pars.SEGcal_gain[detno][ptinp->intpts[j].seg]
                + Pars.SEGcal_offset[detno][ptinp->intpts[j].seg];
//            printf("%7.1f (%7.3f,%7.3f)\n", ptinp->intpts[j].e,Pars.SEGcal_offset[detno][ptinp->intpts[j].seg],Pars.SEGcal_gain[detno][ptinp->intpts[j].seg]);
            }

          /* store original/calibrated segment energy for later use */

          //orig_seg_e = ptinp->intpts[0].e;//unused

          /* hit pattern */

          hitpat->Fill ((double) detno, 1);

          /* worldmap all hits */

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
                          printf ("%7.2f,%7.2f,%7.2f --> ", ptinp->intpts[j].x, ptinp->intpts[j].y, ptinp->intpts[j].z);
                        }

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
                          printf ("%7.2f,%7.2f,%7.2f\n", ptinp->intpts[j].x, ptinp->intpts[j].y, ptinp->intpts[j].z);
                        }

                    }
                  else if (Pars.AGATA_data == 1)
                    {
                      detno = moduleno * 3 + crystalno;


                      xx = ptinp->intpts[j].x;
                      yy = ptinp->intpts[j].y;
                      zz = ptinp->intpts[j].z;
//                      printf("nnn: %i,%f\n", moduleno * 3 + crystalno,(float)ptinp->intpts[j].z,1.0);

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

                }

              polAng = findPolarFromCartesian (ptinp->intpts[j].x, ptinp->intpts[j].y, ptinp->intpts[j].z, &rr);
              aziAng = findAzimuthFromCartesian (ptinp->intpts[j].x, ptinp->intpts[j].y, ptinp->intpts[j].z);

              ndethits[detno]++;
              pol[detno] += polAng;
              azi[detno] += aziAng;

              if (rr > RMIN && rr < RMAX)
                {
                  radius_all->Fill ((double) rr, 1);
		  //need to make sure if the following if statement controls the evsr Fill
                  if (ptinp->intpts[j].e > 0 && ptinp->intpts[j].e < MEDIUMLEN){};
                  evsr_all->Fill ((double) rr, ptinp->intpts[j].e);
                };

              /* SMAP coordinates */

              sX = aziAng * sinf (polAng) / RAD2DEG;
              sY = polAng / RAD2DEG;    /* + 1.5; */

              if (Pars.CurEvNo <= Pars.NumToPrint && 0)
                {
                  printf ("%i [type %i] ", j, GEB_event->ptgd[i]->type);
                  printf ("e: %9.2f/%9.2f ", ptinp->intpts[j].e, ptinp->tot_e);
                  printf ("(%6.2f,%6.2f,%6.2f)cry --> ", xx, yy, zz);
                  printf ("(%6.2f,%6.2f,%6.2f)world(cm); ", ptinp->intpts[j].x, ptinp->intpts[j].y, ptinp->intpts[j].z);
//                  printf (" sX,sY=%6.2f,%6.2f ", sX, sY);
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
            };

          /* simple dopler corrected sum of CC energies */

          CCsum_s->Fill (ptinp->tot_e / Pars.modCCdopfac[ptinp->crystal_id], 1);


          /* quietly rescale all interaction energies to the CC energy */

          r1 = 0;
          for (j = 0; j < ptinp->num; j++)
            r1 += ptinp->intpts[j].e;
          for (j = 0; j < ptinp->num; j++)
            ptinp->intpts[j].e = ptinp->intpts[j].e / r1 * ptinp->tot_e;

          /* doppler correction (this is not the only way to do this!) */
          /* and you must do the rescale above for it to work */

          for (j = 0; j < ptinp->num; j++)
            {
              rr =
                ptinp->intpts[j].x * ptinp->intpts[j].x + ptinp->intpts[j].y * ptinp->intpts[j].y +
                ptinp->intpts[j].z * ptinp->intpts[j].z;
              rr = sqrtf (rr);

              dp = (ptinp->intpts[j].x * Pars.beamdir[0] +
                    ptinp->intpts[j].y * Pars.beamdir[1] + ptinp->intpts[j].z * Pars.beamdir[2]) / rr;

              if (dp < -1.0)
                dp = -1.0;
              if (dp > 1.0)
                dp = 1.0;
              polang[j] = acosf (dp);

              rr = 1.0 - Pars.beta * Pars.beta;
              doppler_factor[j] = sqrt (rr) / (1.0 - Pars.beta * cos (polang[j]));

            };

          ptinp->tot_e = 0;
          for (j = 0; j < ptinp->num; j++)
            ptinp->tot_e += (ptinp->intpts[j].e / doppler_factor[j]);


          /* central contact energy matrix and total energy */

          if (detno > 0 && detno < MAXDETPOS)
            if (ptinp->tot_e > 0 && ptinp->tot_e < LONGLEN)
              {
                CCsum->Fill ((double) ptinp->tot_e, 1);
                CCe->Fill ((double) detno, (double) ptinp->tot_e, 1);
//                ehi[detno]->Fill ((double) ptinp->tot_e, 1);
              };

        };

    };

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
