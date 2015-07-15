#include <stdlib.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <assert.h>

#include "ctk.h"
#include "GEBFilter.h"
#include "veto_pos.h"

#define MAXGTMODNO 30


off_t inData, outData;
unsigned int *veto_cube;
GEBMERGEPAR Pars;

/*--------------------------------------------------------*/

void
CheckNoArgs (int required, int actual, char *str)
{

  if (required < actual)
    {
      printf ("argument problem with chat option\n");
      printf ("--> %s\n", str);
      printf ("required # arguments: %i\n", required - 1);
      printf ("actual   # arguments: %i\n", actual - 1);
      printf ("Please fix and try again, quitting...\n");
      exit (1);
    };

}

/*---------------------------------------------------------------------------------*/

int
readChatFile (char *name)
{

  /* declarations */

  FILE *fp, *fp1;
  char *p, *pc, str[STRLEN], str1[STRLEN];
  int echo, nn = 0, nni = 0, nret;

  /* open chat file */

  if ((fp = fopen (name, "r")) == NULL)
    {
      printf ("error: could not open chat file: <%s>\n", name);
      exit (0);
    };
  printf ("chat file: <%s> open\n", name);
  printf ("\n");
  fflush (stdout);

  /* read content and act */

  pc = fgets (str, STRLEN, fp);

  while (pc != NULL)
    {
      if (echo)
        printf ("chat->%s", str);
      fflush (stdout);

      /* attemp to interpret the line */

      if ((p = strstr (str, "echo")) != NULL)
        {
          echo = 1;
          if (echo)
            printf ("will echo command lines\n");
        }
      else if (str[0] == 35)
        {
          /* '#' comment line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if (str[0] == 59)
        {
          /* ';' comment line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if (str[0] == 10)
        {
          /* empty line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if ((p = strstr (str, "nevents")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &Pars.nevents);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "vetocube")) != NULL)
        {
          nret = sscanf (str, "%s %s", str1, &Pars.vetocubefn);
          CheckNoArgs (nret, 2, str);
          Pars.vetocubes = 1;
          printf ("will read %s\n", Pars.vetocubefn);
          printf ("  and process veto_cube\n");
          printf ("\n");
        }
      else if ((p = strstr (str, "addT0")) != NULL)
        {
          printf ("will do TS --> TS*10+(long long int)(T0+0.5)\n");
          printf ("for mode2 data and just\n");
          printf ("        TS --> TS*10\n");
          printf ("for any other GEBHEader type\n");
          Pars.addT0 = 1;
        }
      else
        {

         /*-----------------------------*/
          /* chatscript read error point */
         /*-----------------------------*/

          printf ("line %2.2i in chat script, option :%s \n__not understood\n", nn, str);
          printf ("%i\n", str[0]);
          printf ("aborting\n");
          fflush (stdout);
          exit (0);
        };

      /* read next line in chat script */

      nn++;                     /* line counter */
      nni++;                    /* instruction counter */
      pc = fgets (str, STRLEN, fp);

    };

  /* done */

  fclose (fp);
  printf ("\n");
  printf ("chat file: <%s> closed\n", name);
  printf ("__processed %i sort instructions and %i lines\n", nni, nn);
  printf ("\n");

  return (0);

};

/*---------------------------------------------------------------------*/

int
main (int argc, char **argv)
{

  /* declarations */

  GEBDATA *ptgd;
  PAYLOAD *ptinp;
  int i, siz, ip, nb, j;
  long long int ngood = 0, nbad = 0;
  int haveTrack = 0;
  int nread = 0, nwrite = 0;
  int writeOK;
  int curev = 0;
  CRYS_INTPTS *cip, tmpcip;
  int bip[MAX_INTPTS];
  int i1, i2, i3, moduleno, crystalno;
  double d1;
  float ebad, esum, egood, ff;

  /* prototypes */

  int setup_veto_cube ();

  /* help */

  if (argc != 4)
    {
      printf ("use: %s chatfile infile outfile\n", argv[0]);
      exit (0);
    };

  /* initialize */

  bzero ((void *) &Pars, sizeof (GEBMERGEPAR));

  /* read chat file */

  readChatFile (argv[1]);

  /* allocate space for track structure */

  ptgd = (GEBDATA *) calloc (1, sizeof (GEBDATA));
  ptinp = (PAYLOAD *) calloc (1, sizeof (PAYLOAD));

  /* open data file */

  inData = 0;
  /*                        + name of data file */
  /*                        |                   */
  inData = open ((char *) argv[2], O_RDONLY);
  if (inData == 0)
    {
      printf ("could not open input data file %s, quit!\n", argv[2]);
      exit (1);
    };
  printf ("input  data file %s is open\n", argv[2]);

  /* open output file */

  outData = 0;
  /*                        + name of data file */
  /*                        |                   */
  outData = open ((char *) argv[3], O_WRONLY | O_CREAT | O_TRUNC, PMODE);
  if (outData == 0)
    {
      printf ("could not open output data file %s, quit!\n", argv[3]);
      exit (1);
    };
  printf ("output data file %s is open\n", argv[3]);

  /* setup */

  if (Pars.vetocubes)
    setup_veto_cube ();

  /* loop through the data and filter */

  while (1)
    {

      /* read GEB header and payload */

      siz = read (inData, (char *) ptgd, sizeof (GEBDATA));
      if (siz != sizeof (GEBDATA))
        goto errordone;

      siz = read (inData, (char *) ptinp, ptgd->length);
      if (siz != ptgd->length)
        goto errordone;

      nread++;

      writeOK = 1;


      /* T0 adjustment ? */

      if (Pars.addT0)
        {

          /* make the TS ns rather than 10 ns */
          /* add allow for mode2 T0 adjustment */

          if (curev < 100)
            printf ("TS= %lli, ", ptgd->timestamp);
          ptgd->timestamp *= 10;

          /* if mode2 data, add T0 to nearest ns */

          if (ptgd->type == GEB_TYPE_DECOMP)
            {

              /* cast */

              cip = (CRYS_INTPTS *) ptinp;
              if (curev < 100)
                printf ("cip->t0=%f --> ", cip->t0);

              /* add T0 to the nearest 1 ns */

              ptgd->timestamp += (long long int) (10.0 * cip->t0 + 0.5);

            };
          if (curev < 100)
            printf ("%lli\n", ptgd->timestamp);

        };

      /* remove bad interaction */

      if (Pars.vetocubes)
        if (ptgd->type == GEB_TYPE_DECOMP)
          {

            /* cast */

            cip = (CRYS_INTPTS *) ptinp;
            assert (cip->num < MAX_INTPTS);

            /* init marker array */

            for (ip = 0; ip < cip->num; ip++)
              bip[ip] = 0;


            /* loop over interactions */

            ebad = 0;
            esum = 0;
            for (ip = 0; ip < cip->num; ip++)
              {
//                printf ("[]%i %i\n", cip->num, tmpcip.num);
//                fflush (stdout);

                nb = 0;

                /* find vetocube index */

                crystalno = (cip->crystal_id & 0x0003);
                moduleno = ((cip->crystal_id & 0xfffc) >> 2);
                i1 = VETO_X_INDEX (cip->intpts[ip].x);
                i2 = VETO_Y_INDEX (cip->intpts[ip].y);
                i3 = VETO_Z_INDEX (cip->intpts[ip].z);

                /* be careful about accessing veto_cube. */
                /* An index could be off in long datasets */

                if (cip->num < MAX_INTPTS)
                  if (i1 < VETO_NX && i2 < VETO_NY && i3 < VETO_NZ)
                    if (i1 >= 0 && i2 >= 0 && i3 >= 0)
                      if (crystalno > -0 && crystalno <= MAXCRYSTALNO)
                        if (moduleno <= MAXGTMODNO && moduleno >= 0)
                          bip[ip] = *(veto_cube + VETO_INDX (moduleno, crystalno, i1, i2, i3));

                if (curev < 10)
                  {
                    printf ("%i: %6.2f %6.2f %6.2f --> bip=", ip, cip->intpts[ip].x, cip->intpts[ip].y,
                            cip->intpts[ip].z);
                    printf ("%i\n", bip[ip]);
                    fflush (stdout);
                  };

                /* accounting */

                esum += cip->intpts[ip].e;
                if (bip[ip] == 0)
                  ngood++;
                else
                  {
                    nb++;
                    nbad++;
                    ebad += cip->intpts[ip].e;
                  };
              };
            egood = esum - ebad;
            ff = ebad / egood;


            /* zap the bad interaction points */

            if (nb > 0)
              {
                bcopy ((char *) cip, (char *) &tmpcip, sizeof (CRYS_INTPTS));
//                printf ("[]%i %i\n", cip->num, tmpcip.num);
//                fflush (stdout);
                assert (tmpcip.num == cip->num);
                assert (tmpcip.num < MAX_INTPTS);

                /* transfer only valid points back */
                /* redistribute the bad energy */

                j = 0;
                assert (cip->num < MAX_INTPTS);
                for (ip = 0; ip < cip->num; ip++)
                  {
                    if (bip[ip] == 0)
                      {
                        if (curev < 10)
                          {
                            printf ("ip=%i\n", ip);
                            fflush (stdout);
                            printf ("tmpcip.intpts[ip].x=%f\n", tmpcip.intpts[ip].x);
                            fflush (stdout);
                            printf ("j=%i\n", j);
                            fflush (stdout);
                            printf ("cip->intpts[j].x=%f\n", cip->intpts[j].x);
                            fflush (stdout);
                          };

                        cip->intpts[j].x = tmpcip.intpts[ip].x;
                        cip->intpts[j].y = tmpcip.intpts[ip].y;
                        cip->intpts[j].z = tmpcip.intpts[ip].z;
                        cip->intpts[j].e = tmpcip.intpts[ip].e * (1 + ff);
                        cip->intpts[j].seg = tmpcip.intpts[ip].seg;
                        cip->intpts[j].seg_ener = tmpcip.intpts[ip].seg_ener;
                        j++;
                      };
                  };


                /* update number of interaction points */

                cip->num = j;

                /* prevent writeout if none survived */

                if (cip->num == 0)
                  writeOK = 0;

              };

          };



      /* filter */


      /* only repeat what fileter OKed */

      if (writeOK)
        {
          siz = write (outData, (char *) ptgd, sizeof (GEBDATA));
//          assert (siz == sizeof (GEBDATA));
          siz = write (outData, (char *) ptinp, ptgd->length);
//          assert (siz == ptgd->length);
          nwrite++;
        };

      /* are we done? */

      curev++;
      if (curev > Pars.nevents)
        goto done;

    };

  /* done */

errordone:
  printf ("GEBFilter: could not read payload\n");
  printf ("could not read more data\n");
done:

  if (Pars.vetocubes)
    {
      printf ("veto_cube:\n");
      d1 = 100.0 * (double) ngood / ((double) ngood + (double) nbad);
      printf ("ngood= %12lli %6.2f%%\n", ngood, d1);
      d1 = 100.0 * (double) nbad / ((double) ngood + (double) nbad);
      printf ("nbad= %12lli %6.2f%%\n", nbad, d1);
    };

  printf ("read  header/payloads: %10i\n", nread);
  printf ("wrote header/payloads: %10i\n", nwrite);

  exit (0);

}
