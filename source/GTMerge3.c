#include <stdlib.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <assert.h>
#include <zlib.h>
#include <string.h>
#include <unistd.h>

/* ------- */
/* globals */
/* ------- */

#define CLK_TCK sysconf(_SC_CLK_TCK)

/* at the moment, the sorting seems to be on */
/* LEDts, so if you use CFDts we run into trouble */
/* with the code as is. Maybe if we use fsplit it works? */

#define TSTYPE LEDts
//#define TSTYPE CFDts 

#include "GTMerge.h"

#define MAXNFIX 10000

/* globals */

STAT nstat;
GTEVENT Event[MAXCOINEV];
int nBadTestPat = 0;
CONTROL control;
DGSEVENT DGSEvent[MAXCOINEV];

int tlkup[NCHANNELS];
int tid[NCHANNELS];
#if(USEZLIB==0)
off_t inData[MAXFILES];
//int inData[MAXFILES];
#endif
#if(USEZLIB==1)
gzFile zFile[MAXFILES];
#endif

int Counter;
int TEMP[MAXLENINTS];


long long int oldFileTS[MAXCOINEV];
int whatFile[MAXCOINEV];
long long int curTS, firstTS;
COINEV CEvent;
int NprintEvNo = 0;
int cevent = 0;
int NumToPrint = 10;
FILE *TSlist;
int nTSlist = 0;
int idlim_lo[MAXTPE];
int idlim_hi[MAXTPE];
long long int toff[MAXTPE][MAXTID];

#if (USEBREAD)
int *bread_buf[MAXFILES];
int bread_pos[MAXFILES];
int bread_bufsiz[MAXFILES];
#endif

/* big buffer */

BIGBUF bigbuf;

/* -------------------------------------------------------------------------- */

int
reportStats (struct tms *timesThen, int nfiles)
{

  /* declarations */

  int i,  nreads = 0, hh, mm, ss;
  float r1, r2;
  double d1;
  int time_stamp (void);
  struct tms timesNow;
  double totalTime;

  times ((struct tms *) &timesNow);

  /* print info */

  printf ("\n**----------------------------------------\n");
  for (i = 0; i < MAXFILES; i++)
    if (control.fileActive[i] || (control.fileEventsRead[i] > 0))
      {
        printf ("file %3i is ", i);
        if (control.fileActive[i])
          printf ("    active; ");
        else
          printf ("NOT active; ");
        printf ("%10i reads ", control.fileEventsRead[i]);
        nreads += control.fileEventsRead[i];
        printf ("; %10i bytes; ", control.filesiz[i]);
        r1 = control.filesiz[i] / 1024.0 / 1000000.0;
        printf ("(%7.3fGB) ", r1);
        if (control.fileActive[i])
          printf ("TS=%18lli", DGSEvent[i].TSTYPE);
        else
          printf (" -- completed");
        printf ("\n");

      };

  printf ("current TS: %12lli; ", curTS);
  printf ("current event no: %9i; ", control.CurEvNo);
  printf ("events written: %9i; ", control.nwritten);
  printf ("total reads=%9i;", nreads);
//  r1=(float)control.nwritten/(float)nreads;
//  printf(" frac=%f",r1);
  printf ("\n");
  d1 = (double) (curTS - firstTS);
  d1 *= 10e-9;
  hh = d1 / 3600;
  d1 -= hh * 3600;
  mm = d1 / 60;
  d1 -= mm * 60;
  ss = d1;
  printf ("elapsed time..................: %3.3ih%2.2im%2.2is, [t0=%lli, curTS=%lli]\n", hh, mm, ss, firstTS, curTS);
  time_stamp ();
  fflush (stdout);
  printf ("number of open files: %4i\n", control.nOpenFiles);

  d1 = timesNow.tms_utime - timesThen->tms_utime;
  totalTime = d1;
  printf ("CPU time: user %9.3f sec, ", d1 / CLK_TCK);
  r1 = timesNow.tms_stime - timesThen->tms_stime;
  totalTime += d1;
  printf ("system  %9.3f sec, ", d1 / CLK_TCK);
  totalTime /= CLK_TCK;
  printf ("Total  %9.3f sec\n", totalTime);


  printf ("control.nzeroehi......=%10i; ", control.nzeroehi);
  r1 = 100.0 * (float) control.nzeroehi / (float) nreads;
  printf ("or %9.6f%%\n", r1);

  printf ("control.noverflowehi..=%10i; ", control.noverflowehi);
  r1 = 100.0 * (float) control.noverflowehi / (float) nreads;
  printf ("or %9.6f%%\n", r1);

  for (i = 0; i < nfiles; i++)
    {
      if (nstat.nTSjumprecover_f[i] > 0)
        {
          printf ("nstat.nTSjumprecover_f[%i]=%10i; ", i, nstat.nTSjumprecover_f[i]);
          r1 = 100.0 * (float) nstat.nTSjumprecover_f[i] / (float) nreads;
          printf ("or %9.6f%%\n", r1);
        };

      if (nstat.nTSjumprecover_b[i] > 0)
        {
          printf ("nstat.nTSjumprecover_b[%i]=%10i; ", i, nstat.nTSjumprecover_b[i]);
          r1 = 100.0 * (float) nstat.nTSjumprecover_b[i] / (float) nreads;
          printf ("or %9.6f%%\n", r1);
        };

    };

  printf ("nstat.badid...........=%10lli; ", nstat.badid);
  r1 = 100.0 * (float) nstat.badid / (float) nreads;
  printf ("or %9.6f%%\n", r1);

  printf ("bigbuf fills..........= %10i\n", nstat.nbigbufreads);
  printf ("time order swaps......= %10lli\n", nstat.nswaps);

  r1 = nstat.inbytes / 1024.0 / 1000000.0;
  r2 = nstat.inbytes / 1024.0 / 1000.0 / totalTime;
  printf ("# bytes read...: %12lliB, %7.3fGB; %7.3fMB/sec; ", nstat.inbytes, r1, r2);
  printf ("%7.3fGB/hour, ", r2 * 3600. / 1000.);
  printf ("%3.1fTB/day\n", r2 * 3600. / 1000. / 1000. * 24);
  r1 = nstat.outbytes / 1024.0 / 1000000.0;
  r2 = nstat.outbytes / 1024.0 / 1000.0 / totalTime;
  printf ("# bytes written: %12lliB, %7.3fGB; %7.3fMB/sec\n", nstat.outbytes, r1, r2);

  r2 = (nstat.inbytes + nstat.outbytes) / 1024.0 / 1000.0 / totalTime;
  printf ("total IO rate: %7.3fMB/sec or %5.0fMbits/sec\n", r2, r2 * 8);

  r1 = (float) control.chunksiz / 1024000000.;
  printf ("output chunk # %i (chunksiz=%i bytes, or %6.3f GBytes)\n", control.chunkno, control.chunksiz, r1);

//  for (i = 0; i < NCHANNELS; i++)
//    if (nstat.id_hit[i] > 0)
//      printf ("id=%5i, had %i hits \n", i, nstat.id_hit[i]);

  printf ("^^----------------------------------------\n");
  printf ("\n");
  fflush (stdout);

  /* done */

  return (0);

}

/* ------------------------------------------------------ */

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

/* -------------------------------------------------------------------------- */

int
main (int argc, char **argv)
{

  /* declarations */

  off_t outData;                /* input file */
  gzFile zoutData;
  int nfiles = 0, maxNoEvents = 0, nPoolEvents = 0, siz, firsttime, nbad;
  int i1, i2, i3, i4, badRead, nfe, ii, njb = 0, njf = 0, k;
  int i, j, st, nextTSpoolIndex, nn, argcoffset = 0;
  DGSHEADER outheader;
  long long int dt = 0, nextTS, oldListTS, dTS, lltmp, oldTS;
  float dtbtev[LENSP];
  float ehigain[NGE + 1], ehioffset[NGE + 1], r1, r2, rn;
  FILE *fp1, *fp3, *fp;
  char str[132], str1[512], fn_ehical[512], fn_toff[512];
  unsigned int seed = 0;
  struct tms timesThen;
  int i7, i8, nGE = 0, nFP = 0, nDSSD = 0, nCHICO2 = 0, nBGO = 0, reportinterval, nGS = 0;
  char *p, *pc;
  int echo = 0, nni, nret, ncib = 0;
  double d1;
  DGSEVENT *tmpentry;

  /* prototypes */

  int wr_spe (char *, int *, float *);
  int printDgsHeader (DGSHEADER);
  int GTPrintEvent (FILE *, GTEVENT *, DGSEVENT *);
  int GTPrintHeader (FILE *, GTEVENT *);
  int get_a_seed (unsigned int *);
  int time_stamp (void);
  int GTGetDiskEv (int, int);
  int GTEvdecompose (GTEVENT *, DGSEVENT *);

  /* help */

  if (argc == 1)
    {
      printf ("use: gtMerge chatfile outfile     file1  file2  file3  file4 .....\n");
      printf ("eg., gtMerge gtmerge.chat c.gtd   t1.gtd t2.gtd t3.gtd t4.gtd\n");
      exit (0);
    };

  /* initialize random number generator etc */

  get_a_seed (&seed);
  srand (seed);
  bzero ((char *) &control, sizeof (CONTROL));
  bzero ((char *) &nstat, sizeof (STAT));

  control.dtsfabort = 5;
  control.dtsbabort = 5;

  // works to here...

  for (i = 0; i < MAXTPE; i++)
    {
      //      printf(" i is %i of MAXTPE of %i \n",i,MAXTPE);
      // works to here...
      idlim_hi[i] = 0;
      idlim_lo[i] = 0;
      for (j = 0; j < MAXTID; j++)
        {
//          nstat.in_hit[i][j] = 0;
//          nstat.out_hit[i][j] = 0;
//              /* output statistics */

          for (k = (ii - CEvent.len); k < ii; k++) {

            if (bigbuf.ev[k]->tpe >= 0 && bigbuf.ev[k]->tpe < MAXTPE)
              if (bigbuf.ev[k]->tid >= 0 && bigbuf.ev[k]->tid < MAXTID)
                nstat.out_hit[bigbuf.ev[k]->tpe][bigbuf.ev[k]->tid]++;
	  }
        };
    };
  // fails before this.

  for (i = 0; i < NCHANNELS; i++)
    nstat.id_hit[i] = 0;

  for (i = 0; i <= NGE; i++)
    {
      ehigain[i] = 1;
      ehioffset[i] = 0;
    };

  for (i = 0; i < MAXTPE; i++)
    for (j = 0; j < MAXTID; j++)
      toff[i][j] = 0;

  sprintf (fn_ehical, "none");
  sprintf (fn_toff, "none");

  /* open chat file */
  // fails before this...

  if ((fp = fopen (argv[1], "r")) == NULL)
    {
      printf ("error: could not open chat file: <%s>\n", argv[1]);
      exit (0);
    };
  printf ("chat file: <%s> open\n", argv[1]);
  printf ("\n");
  fflush (stdout);

  /* read chatfile content and act */
  // fails before this.

  pc = fgets (str, STRLEN, fp);

  nn = 0;
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
          fflush (stdout);

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
      else if ((p = strstr (str, "maxNoEvents")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &maxNoEvents);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "reportinterval")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &reportinterval);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "toff")) != NULL)
        {
          nret = sscanf (str, "%s %s", str1, fn_toff);
          CheckNoArgs (nret, 2, str);

          fp1 = fopen (fn_toff, "r");
          if (fp1 == NULL)
            {
              printf ("file %s not found, quit\n", fn_toff);
              exit (1);
            }
          else
            {
              i4 = fscanf (fp1, "\n%i %i %i", &i1, &i2, &i3);
              while (i4 == 3)
                {
                  assert (i1 >= 0);
                  assert (i1 < MAXTPE);
                  assert (i2 >= 0);
                  assert (i2 < MAXTID);
                  toff[i1][i2] = (long long int) i3;
                  printf ("toff[%i][%i]=%lli\n", i1, i2, toff[i1][i2]);
                  i4 = fscanf (fp1, "\n%i %i %i", &i1, &i2, &i3);
                }
              fclose (fp1);
            };
        }
      else if ((p = strstr (str, "toff")) != NULL)
        {
          nret = sscanf (str, "%s %s", str1, fn_toff);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "chunksiz")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.chunksiz);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "minmul")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.minmult);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "geehical")) != NULL)
        {
          nret = sscanf (str, "%s %s", str1, fn_ehical);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "timewindow")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &i1);
          CheckNoArgs (nret, 2, str);
          dt = (long long int) i1;
        }
      else if ((p = strstr (str, "minGE")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.minGE);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "minDSSD")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.minDSSD);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "minCHICO2")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.minCHICO2);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "minGS")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.minGS);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "minFP")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.minFP);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "maxtracelen")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.maxtracelen);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "bigbufsize")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &bigbuf.size);
          CheckNoArgs (nret, 2, str);
          assert (bigbuf.size <= MAXBIGBUFSIZ);
          r1 = (bigbuf.size * sizeof (DGSEVENT) + (bigbuf.size + 1) * sizeof (int)) / 1024.0 / 1000;
          printf ("sizeof(DGSEVENT)= %li\n", sizeof (DGSEVENT));
          printf ("will use a bigbuffer size of %i, or %7.3f MBytes\n", bigbuf.size, r1);
        }
      else if ((p = strstr (str, "wosize")) != NULL)
        {
          nret = sscanf (str, "%s %f", str1, &r1);
          r1=(r1/100.0*bigbuf.size);
          bigbuf.wosize=(int)r1;
          CheckNoArgs (nret, 2, str);
          printf ("will use a bigbuffer wosize of %i\n", bigbuf.wosize);
          assert (bigbuf.wosize <= bigbuf.size);
        }
      else if ((p = strstr (str, "startTS")) != NULL)
        {
          nret = sscanf (str, "%s %lli %lli ", str1, &control.startTS_lo, &control.startTS_hi);
          CheckNoArgs (nret, 3, str);
          printf ("startTS from %lli to %lli\n", control.startTS_lo, control.startTS_hi);
          control.startTS = 1;
        }
      else if ((p = strstr (str, "noCoinData")) != NULL)
        {
          nret = sscanf (str, "%s", str1);
          CheckNoArgs (nret, 1, str);
          control.noCoinData = 1;
          printf ("will not make coincidences, just pass through merged data\n");
        }
      else if ((p = strstr (str, "idlim")) != NULL)
        {
          nret = sscanf (str, "%s %i %i %i", str1, &i1, &i2, &i3);
          CheckNoArgs (nret, 4, str);
          idlim_lo[i1] = i2;
          idlim_hi[i1] = i3;
        }
      else if ((p = strstr (str, "zzipout")) != NULL)
        {
          nret = sscanf (str, "%s", str1);
          CheckNoArgs (nret, 1, str);
          control.zzipout = 1;
          printf ("will zzip output, %i\n", control.zzipout);
          fflush (stdout);

        }
      else if ((p = strstr (str, "TSlistelen")) != NULL)
        {
          nret = sscanf (str, "%s %i %i %i", str1, &control.TSlistelen, &control.TSlist_lo, &control.TSlist_hi);
          CheckNoArgs (nret, 4, str);
        }
      else if ((p = strstr (str, "dts_min")) != NULL)
        {
          nret = sscanf (str, "%s %lli", str1, &control.dts_min);
          CheckNoArgs (nret, 2, str);
          printf ("control.dts_min=%lli\n", control.dts_min);
        }
      else if ((p = strstr (str, "dts_max")) != NULL)
        {
          nret = sscanf (str, "%s %lli", str1, &control.dts_max);
          CheckNoArgs (nret, 2, str);
          printf ("control.dts_max=%lli\n", control.dts_max);
        }
      else if ((p = strstr (str, "dtsfabort")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.dtsfabort);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "dtsbabort")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.dtsbabort);
          CheckNoArgs (nret, 2, str);
        }
      else if ((p = strstr (str, "zerosuppress")) != NULL)
        {
          control.zerosuppress = 1;
          nret = sscanf (str, "%s %i", str1, &control.zerosuppresslim);
          CheckNoArgs (nret, 2, str);
          printf ("control.zerosuppresslim=%i\n", control.zerosuppresslim);
        }

      else
        {

          /* --------------------------- */
          /* chatscript read error point */
          /* --------------------------- */

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
  fclose (fp);

  /* extract and repeat parameters */

  printf ("%s: will produce a max of %i events\n", argv[0], maxNoEvents);
  printf ("%s: will use dt of %lli\n", argv[0], dt);

  printf ("%s: will write combined data to file \"%s\"\n", argv[0], argv[2]);


  printf ("control.minmult=%i\n", control.minmult);
  printf ("control.minGE=%i\n", control.minGE);
  printf ("control.minGS=%i\n", control.minGS);
  printf ("control.minFP=%i\n", control.minFP);
  printf ("control.minDSSD=%i\n", control.minDSSD);
  printf ("control.minCHICO2=%i\n", control.minCHICO2);
  printf ("CEvent.lenclean=%i\n", CEvent.lenclean);

  /* offset for data file name reads, find  nfiles etc */

  argcoffset = 3;
  nfiles = (argc - argcoffset);
  printf ("%s: we have %i datafiles to combine\n", argv[0], nfiles);
  fflush (stdout);

  nPoolEvents = nfiles;
  printf ("%s: will keep a pool of %i events to combine from \n", argv[0], nPoolEvents);
  assert (nPoolEvents < MAXCOINEV);

  /* just to be sure */

  assert (nfiles == nPoolEvents);

  /************************/
  /* open all input files */
  /************************/

#if(USEZLIB==0)
  for (i = 0; i < nfiles; i++)
    {
      nn = i + argcoffset;
      inData[i] = open (argv[nn], O_RDONLY, 0);
      if (inData[i] < 0)
        {
          printf ("could not open input data file [%i] %s, quit!\n", nn, argv[nn]);
          exit (1);
        }
      else
        {
          printf ("%s: input data file \"%s\", number %i, is open\n", argv[0], argv[nn], i);
          fflush (stdout);
          control.nOpenFiles++;
          control.fileActive[i] = 1;
#if(USEZLIB==1)
          zFile[i] = gzdopen (inData[i], "r");
#endif
        }
    };
#endif

#if(USEZLIB==1)
  for (i = 0; i < nfiles; i++)
    {
      nn = i + argcoffset;
      zFile[i] = gzopen (argv[nn], "r");
      if (zFile[i] == NULL)
        {
          printf ("could not open input data file [%i] %s, quit!\n", nn, argv[nn]);
          exit (1);
        }
      else
        {
          printf ("%s: gzipped input data file \"%s\", number %i, is open\n", argv[0], argv[nn], i);
          fflush (stdout);
          control.nOpenFiles++;
          control.fileActive[i] = 1;
        }
    };
#endif

#if (USEBREAD)

  for (i = 0; i < nfiles; i++)
    {
      bread_buf[i] = (int *) calloc (BREAD_BUFSIZE, sizeof (int));
      bread_pos[i] = 0;
      bread_bufsiz[i] = BREAD_BUFSIZE;
    };

#endif


  /* ----------- */
  /* output file */
  /* ----------- */


  outData = 0;
  /*                        + name of data file */
  /*                        |                   */
  sprintf (str, "%s_%3.3i", argv[2], control.chunkno);
  outData = open (str, O_WRONLY | O_CREAT | O_TRUNC, PMODE);

  if (outData == 0)
    {
      printf ("could not open output data file %s, quit!\n", str);
      exit (1);
    }
  else
    {
      printf ("%s: output data file \"%s\" is open\n", argv[0], str);
      fflush (stdout);
    };

  if (control.zzipout == 1)
    zoutData = gzdopen (outData, "w");

  /* write output header file */

  bzero ((char *) &outheader, sizeof (DGSHEADER));
  outheader.id = 1000;
  if (control.zzipout == 0)
    siz = write (outData, (char *) &outheader, sizeof (DGSHEADER));
  else
    siz = gzwrite (zoutData, (char *) &outheader, sizeof (DGSHEADER));

  printf ("header written to output file\n");

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
      exit (1);
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

  /* ------------------------------ */
  /* see if we have ge ehi cal file */
  /* ------------------------------ */

  fp1 = fopen (fn_ehical, "r");
  if (fp1 != NULL)
    {
      printf ("found %s ehi cal file\n", fn_ehical);
//      getline(str,&i1,fp1);
      for (i = 0; i < 4; i++)
        {
          fscanf (fp1, "%s", str);
          printf ("%s ", str);
        }
      i2 = fscanf (fp1, "\n%i %f %f", &i1, &r1, &r2);
      while (i2 == 3)
        {
          if (i1 <= NGE && i1 > 0)
            {
              ehigain[i1] = r2;
              ehioffset[i1] = r1;
              printf ("ge %3i, offset=%8.2f gain=%9.4f\n", i1, ehioffset[i1], ehigain[i1]);
            };
          i2 = fscanf (fp1, "\n%i %f %f", &i1, &r1, &r2);
        }
      fclose (fp1);
    }

  /* start timer */

  times ((struct tms *) &timesThen);

  /* allocate memory for bigbuf */

  i1 = 0;
  for (i = 0; i <= bigbuf.size; i++)
    {
      bigbuf.ev[i] = (DGSEVENT *) calloc (sizeof (DGSEVENT), 1);
      i1 += sizeof (DGSEVENT);
    };
  printf ("allocated %i bytes for bigbuf events\n", i1);
  tmpentry = (DGSEVENT *) calloc (sizeof (DGSEVENT), 1);

#if(0)
  /* simple read of data for debug purpose */

  assert (nfiles == 1);
  firsttime = 1;
  nn = 0;
  nbad = 0;
  for (i = 0; i < 10000000; i++)
    {
      st = GTGetDiskEv (0, 0);
      if (st == 2 || st == 5)
        {
          printf ("failed to read more data from file %i\n", 0);
          printf ("read %10i events, %5i jumped (%9.3f%%)\n", nn, nbad, (float) nbad * 100 / nn);
          exit (0);
        };
      if (st == 0)
        GTEvdecompose ((GTEVENT *) & Event[0], (DGSEVENT *) & DGSEvent[0]);
      if (firsttime)
        {
          firsttime = 0;
          oldListTS = DGSEvent[0].TSTYPE;
          printf ("%6i:st=%i: DGSEvent[%i].TSTYPE=%20lli; \n", i, st, 0, DGSEvent[0].TSTYPE);
        }
      else
        {
          dt = DGSEvent[0].TSTYPE - oldListTS;
          if ((dt > control.dts_max) || (dt < control.dts_min))
            {
              nbad++;
              printf ("%6i:st=%i: DGSEvent[%i].TSTYPE=%20lli; ", i, st, 0, DGSEvent[0].TSTYPE);
              printf ("DT=%20lli, oldTS=%20lli\n", dt, oldListTS);
            };
          oldListTS = DGSEvent[0].TSTYPE;
        };
      nn++;

    };
  if (1)
    exit (0);
#endif

  if (control.startTS)
    {
      /* pre-read until we hit the the start interval TS */

      nn = 0;
      for (i = 0; i < nPoolEvents; i++)
        {
          for (j = 0; j < 10000000; j++)
            {
              st = GTGetDiskEv (nn, i);
              if (st == 0)
                {





                  GTEvdecompose ((GTEVENT *) & Event[i], (DGSEVENT *) & DGSEvent[i]);



//      printf("%5i: ", j);
                  dTS = DGSEvent[i].TSTYPE - oldTS;
//      printf("TS=%20lli dTS=%10lli\n",DGSEvent[i].TSTYPE, dTS);
                  if (DGSEvent[i].TSTYPE > control.startTS_lo && DGSEvent[i].TSTYPE < control.startTS_hi)
                    break;
                  oldTS = DGSEvent[i].TSTYPE;
                }
            }
          nn++;
          printf ("found reset at TS=%20lli for PoolEvents=%i after %i reads\n", DGSEvent[i].TSTYPE, i, j);
        };

    };



  /* -------------------------------------------- */
  /* read until we have filled our pool of events */
  /* -------------------------------------------- */

  printf ("filling pool\n");
  nn = 0;
  for (i = 0; i < nPoolEvents; i++)
    {
      printf ("attempting to read event... i=%i ", i);
      st = GTGetDiskEv (nn, i);
      printf ("st=%i\n", st);
      if (st != 0)
        {
          printf ("1: GTGetDiskEv returned %i\n", st);
        }
      if (st == 2 || st == 5)
        {
          printf ("failed to read more data from %i\n", nn);
          exit (0);
        };
      while (st != 0 && st != 2 && st != 5)
        {
          st = GTGetDiskEv (nn, i);
          if (st != 0)
            {
              printf ("2: GTGetDiskEv returned %i\n", st);
            }
        }
      if (st == 0) {



 #if(0)

  /* debug print trace */

  printf ("Event tracLen=%i, trace>\n", Event[i].traceLen);
  for (k = 0; k < Event[i].traceLen; k++)
    printf ("%3i: %i\n", k, Event[i].trace[k]);

  #endif

        GTEvdecompose ((GTEVENT *) & Event[i], (DGSEVENT *) & DGSEvent[i]);

 #if(0)

  /* debug print trace */

  printf ("DGSEvent tracLen=%i, trace>\n", DGSEvent[i].traceLen);
  for (k = 0; k < DGSEvent[i].traceLen; k++)
    printf ("%3i: %i\n", k, DGSEvent[i].trace[k]);

  #endif

}

      oldFileTS[i] = DGSEvent[i].TSTYPE;
      printf ("DGSEvent[%i].TSTYPE=%lli\n", i, DGSEvent[i].TSTYPE);
      whatFile[i] = nn;
      nn++;
      if (nn >= nfiles)
        nn = 0;
    };

  /* print first timestamps and check them */

  for (i = 0; i < nPoolEvents; i++)
    {
      printf ("file %3i, first TS %22lli;\n", i, DGSEvent[i].TSTYPE);
    };
//if(1)exit(0);

  for (i = 0; i < nPoolEvents; i++)
    {

      if (DGSEvent[i].TSTYPE == (long long int) 0)
        printf ("ooops, file %3i has TS = %22lli, will re-read\n", i, DGSEvent[i].TSTYPE);
      {

        while (DGSEvent[i].TSTYPE == (long long int) 0)
          {
            st = GTGetDiskEv (i, i);
            if (st != 0)
              {
                printf ("1: GTGetDiskEv returned %i\n", st);
              }
            if (st == 2 || st == 5)
              {
                printf ("failed to read more data from %i\n", nn);
                exit (0);
              };
            while (st != 0 && st != 2 && st != 5)
              {
                st = GTGetDiskEv (i, i);
                if (st != 0)
                  {
                    printf ("2: GTGetDiskEv returned %i\n", st);
                  }
              }
            if (st == 0)
              GTEvdecompose ((GTEVENT *) & Event[i], (DGSEVENT *) & DGSEvent[i]);
            printf ("__DGSEvent[%i].TSTYPE=%lli\n", i, DGSEvent[i].TSTYPE);
            oldFileTS[i] = DGSEvent[i].TSTYPE;
          };

      };
      d1 = (double) DGSEvent[i].TSTYPE / 100000;
      printf ("file %3i, first TS %22lli or %.3f msec;\n", i, DGSEvent[i].TSTYPE, (float) d1);

  
 

    };

  printf ("\n");
  printf ("initial pool of events read (%i events)\n", nPoolEvents);
  reportStats (&timesThen, nfiles);
  fflush (stdout);




  /* find the very first time stamp */

  nextTS = ULLONG_MAX;
  nextTSpoolIndex = -1;
  for (i = 0; i < nPoolEvents; i++)
    {
#if(DEBUG1)
      printf ("%lli ", DGSEvent[i].TSTYPE);
#endif
      if (DGSEvent[i].TSTYPE < nextTS)
        {
          nextTS = DGSEvent[i].TSTYPE;
          nextTSpoolIndex = i;
        };
    };
  firstTS = nextTS;

  printf ("\n");
  printf ("First TS=%lli, from file %i\n", nextTS, nextTSpoolIndex);

  /* -------------------------- */
  /* fill bigbuf the first time */
  /* -------------------------- */

  bigbuf.truesize = 0;
  while (bigbuf.truesize < bigbuf.size)
    {

      /* ------------------------------------------------ */
      /* find lowest time stamp of the candicates we have */
      /* ------------------------------------------------ */

      nextTS = ULLONG_MAX;
      nextTSpoolIndex = -1;
      for (i = 0; i < nPoolEvents; i++)
        {
          if (DGSEvent[i].TSTYPE < nextTS)
            {
              nextTS = DGSEvent[i].TSTYPE;
              nextTSpoolIndex = i;
            };
        };

      /* -------------- */
      /* copy to bigbuf */
      /* -------------- */

      memcpy ((char *) bigbuf.ev[bigbuf.truesize], (char *) &DGSEvent[nextTSpoolIndex], sizeof (DGSEVENT));
//      printf ("__fill: bigbuf.ev[%i]->TSTYPE-curTS=%lli\n", bigbuf.truesize, bigbuf.ev[bigbuf.truesize]->TSTYPE-curTS);
      bigbuf.truesize++;

      /* --------------------------- */
      /* read in a replacement event */
      /* --------------------------- */

      if (control.fileActive[whatFile[nextTSpoolIndex]])
        {

#include "GTMerge_readnew.h"

        }

    };
  printf ("GTMerge: big buf is now completely primed with %i events...\n", bigbuf.truesize);
  nstat.nbigbufreads++;
  printf ("first fill: bigbuf.ev[%4i]->TSTYPE-curTS=%lli\n", 0, bigbuf.ev[0]->TSTYPE);
  printf ("last  fill: bigbuf.ev[%4i]->TSTYPE-curTS=%lli\n", bigbuf.truesize - 1,
          bigbuf.ev[bigbuf.truesize - 1]->TSTYPE);
  d1 = (double) (bigbuf.ev[bigbuf.truesize - 1]->TSTYPE - bigbuf.ev[0]->TSTYPE);
  d1 /= 1000000000;
  printf ("bigbuf covers %f seconds\n", (float) d1);
  printf ("\n\n");
#if(0)
  /* debug print the events in bigbuf */

  for (i = 0; i < bigbuf.truesize; i++)
    GTPrintEvent2 (stdout, i, bigbuf.ev[i]);
  if (1)
    exit (0);
#endif

  for (i = 0; i < LENSP; i++)
    dtbtev[i] = 0;

  /* ---------------------------------------------- */
  /* at this point we have a filled bigbuf. we are  */
  /* now going to process it and refill it until we  */
  /* have no data left to process */
  /* ---------------------------------------------- */

  while (1)
    {
      bigbuf.nused = 0;

#if(0)
      printf ("--------\n");
      printf ("start of loop\n");
      printf ("bigbuf.truesize=%i\n", bigbuf.truesize);
      printf ("bigbuf.nused=%i\n", bigbuf.nused);
      printf ("bigbuf.size=%i\n", bigbuf.size);
      fflush (stdout);
#endif

#if(0)
      printf ("\nbeginning buffer\n");
      printf ("bigbuf.size=%i, bigbuf.nused=%i, (bigbuf.size - bigbuf.nused)=%i\n", bigbuf.size, bigbuf.nused,
              (bigbuf.size - bigbuf.nused));
      i1 = 10;
      i2 = bigbuf.nused;
#include "pbuf.h"
#endif


      /* apply the time offsets */

      for (i = 0; i < bigbuf.truesize; i++)
        bigbuf.ev[i]->TSTYPE += toff[bigbuf.ev[i]->tpe][bigbuf.ev[i]->tid];

      /* now time order */
      /* bigbuf by swapping pointers. */
      /* Simple 'bubble sort' now, maybe */
      /* use better later */

      /* Note: it might be tempting to say:  */
      /* just sort from bigbuf.nused and up, but  */
      /* we might read in something that  */
      /* happened in the past so we cannot  */
      /* do that! That would be against what  */
      /* bigbuf was installed for in the  */
      /* first place */

      for (i = 0; i < bigbuf.truesize; i++)
        for (j = i + 1; j < bigbuf.truesize; j++)
          {
            if (bigbuf.ev[j]->TSTYPE < bigbuf.ev[i]->TSTYPE)
              {
//                printf ("swap %i and %i\n", i, j);
//                printf ("swap: TS[%i]=%lli < TS[%i]=%lli\n", i,bigbuf.ev[j]->TSTYPE, j,bigbuf.ev[i]->TSTYPE);

                /* swap the pointers */

                tmpentry = bigbuf.ev[j];
                bigbuf.ev[j] = bigbuf.ev[i];
                bigbuf.ev[i] = tmpentry;

                nstat.nswaps++;

              };
          };
//      printf ("nstat.nbigbufreads=%i, nstat.nswaps=%i\n", nstat.nbigbufreads, nstat.nswaps);

#if(0)
      printf ("\nafter sort\n");
      printf ("bigbuf.size=%i, bigbuf.nused=%i, (bigbuf.size - bigbuf.nused)=%i\n", bigbuf.size, bigbuf.nused,
              (bigbuf.size - bigbuf.nused));
      i1 = 10;
      i2 = bigbuf.nused;
#include "pbuf.h"
#endif

#if(0)
      /* check sanity of buffer */

      oldTS = bigbuf.ev[1]->TSTYPE;
      for (i = 1; i < bigbuf.truesize; i++)
        {
          assert (bigbuf.ev[i]->TSTYPE >= bigbuf.ev[i - 1]->TSTYPE);
          assert (bigbuf.ev[i]->TSTYPE > 0);
          dTS = bigbuf.ev[i]->TSTYPE - oldTS;
          oldTS = bigbuf.ev[i]->TSTYPE;
          if (dTS > 4000)
            printf ("\n");
          printf ("_%5i: TS=%20lli, dTS=%10lli, tpe=%i\n", i, bigbuf.ev[i]->TSTYPE, dTS, bigbuf.ev[i]->tpe);
        }
      exit (1);
#endif


      /* update simple TS.list file */

      if (nstat.nbigbufreads >= control.TSlist_lo && nstat.nbigbufreads <= control.TSlist_hi)
        {
          sprintf (str, "TS.list_%3.3i", nstat.nbigbufreads);
          TSlist = fopen (str, "w");
          nTSlist = 0;
          fprintf (TSlist, "TS dump of buffer # %i\n\n", nstat.nbigbufreads);

          if (nTSlist < control.TSlistelen)
            {

//        fprintf (TSlist, "read # %5i> ", control.nread);

//          fprintf (TSlist, "(%10lli,", (long long int)bigbuf.ev[i]->.LEDts);
//          fprintf (TSlist, "%10lli)", oldFileTS[nextTSpoolIndex]);

              for (i = 0; (i < bigbuf.truesize) && (nTSlist < control.TSlistelen); i++)
                {

                  dTS = bigbuf.ev[i]->TSTYPE - oldListTS;
                  if (dTS > dt)
                    fprintf (TSlist, "\n");

                  fprintf (TSlist, "%6i: ", i);
                  if (bigbuf.ev[i]->tpe == GE)
                    fprintf (TSlist, "germanium");
                  else if (bigbuf.ev[i]->tpe == BGO)
                    fprintf (TSlist, "bgo......");
                  else if (bigbuf.ev[i]->tpe == SIDE)
                    fprintf (TSlist, "side.....");
                  else if (bigbuf.ev[i]->tpe == AUX)
                    fprintf (TSlist, "aux......");
                  else if (bigbuf.ev[i]->tpe == DSSD)
                    fprintf (TSlist, "DSSD.....");
                  else if (bigbuf.ev[i]->tpe == FP)
                    fprintf (TSlist, "FP.......");
                  else if (bigbuf.ev[i]->tpe == XARRAY)
                    fprintf (TSlist, "XARRAY...");
                  else if (bigbuf.ev[i]->tpe == CHICO2)
                    fprintf (TSlist, "CHICO2...");
                  else
                    fprintf (TSlist, "what?....");

                  fprintf (TSlist, "/ch# %3i ", bigbuf.ev[i]->tid);

                  fprintf (TSlist, "TS=%10lli;  ", bigbuf.ev[i]->TSTYPE);
                  fprintf (TSlist, " ehi= %5i; ", bigbuf.ev[i]->ehi);

                  if (nTSlist == 0)
                    oldListTS = bigbuf.ev[i]->TSTYPE;
                  else
                    {
                      fprintf (TSlist, "dTS=%10lli; ", dTS);
                      oldListTS = bigbuf.ev[i]->TSTYPE;
                    }
                  fprintf (TSlist, "\n");
                  fflush (TSlist);
                  nTSlist++;

                };

            };
          fclose (TSlist);
        };


      /* write statistics in the beginning */

      if (nstat.nbigbufreads < 20 )
        {
          d1 = (double) (bigbuf.ev[bigbuf.truesize - 1]->TSTYPE - bigbuf.ev[0]->TSTYPE);
          d1 /= 100000;
          printf ("buf# %3i, bigbuf covers %9.3f ms; ", nstat.nbigbufreads, (float) d1);
          printf ("from %20lli(%3i/%3i) to ", bigbuf.ev[0]->TSTYPE, bigbuf.ev[0]->tpe, bigbuf.ev[0]->tid);
          printf ("%20lli(%3i/%3i)\n", bigbuf.ev[bigbuf.truesize - 1]->TSTYPE, bigbuf.ev[bigbuf.truesize - 1]->tpe,
                  bigbuf.ev[bigbuf.truesize - 1]->tid);
          fflush (stdout);
        };


      /* make coincidence events out of what we have */
      /* in the first part of bigbuf */

      ncib = 0;
      ii = 0;
      curTS = bigbuf.ev[ii]->TSTYPE;
      if (nstat.nbigbufreads < 20 )
{
      printf ("making coincidences from %i to %i in buffer\n", ii, bigbuf.wosize);
      printf ("first curTS=%lli\n", curTS);
};
      ii++;
      CEvent.len = 1;
      while (ii < bigbuf.wosize)
        {

//          printf ("__ii=%i, TS=%lli\n", ii, bigbuf.ev[ii]->TSTYPE);

          while ((bigbuf.ev[ii]->TSTYPE - curTS) < dt)
            {

#if(0)
              printf ("bigbuf.ev[%i]->TSTYPE=%lli; tp=%2i ", ii, bigbuf.ev[ii]->TSTYPE, bigbuf.ev[i]->tpe);
              printf ("{%lli, %lli}, DT=%lli\n", bigbuf.ev[ii]->TSTYPE, curTS, bigbuf.ev[ii]->TSTYPE - curTS);
              fflush (stdout);
#endif

              ii++;
//              printf ("_:ii=%i, TS=%lli\n", ii, bigbuf.ev[ii]->TSTYPE);
              CEvent.len++;
//if(ii>10)exit(0);
//              printf ("dt=%lli, CEvent.len=%i\n", dt, CEvent.len);


            };

#if(0)
          /* write this event out for debug */

          printf ("=======______=======\n");
          printf ("CEvent.len=%i, ii=%i\n", CEvent.len, ii);
          for (i = (ii - CEvent.len); i < ii; i++)
            {
              printf ("__TS(%i)=%lli, tpe/tid=%i/%i, DT=%lli\n", i, bigbuf.ev[i]->TSTYPE, bigbuf.ev[i]->tpe,
                      bigbuf.ev[i]->tid, bigbuf.ev[i]->TSTYPE - curTS);
            }
          if (ii > 100)
            exit (0);
#endif

          assert (CEvent.len > 0);

          /*---------------------------------------------*/
          /* write these hits out as a coincidence event */
          /*---------------------------------------------*/

          /* find and mark compton suppressed germaniums */

          for (i = (ii - CEvent.len); i < ii; i++)
            if (bigbuf.ev[i]->tpe == GE)
              {
                bigbuf.ev[i]->flag = 0;
                nstat.ge_hit[bigbuf.ev[i]->tid]++;
                for (j = (ii - CEvent.len); j < ii; j++)
                  if (i != j)
                    if (bigbuf.ev[j]->tpe == BGO && bigbuf.ev[j]->ehi > 5)
                      {
                        if (bigbuf.ev[i]->tid == bigbuf.ev[j]->tid)
                          {
                            bigbuf.ev[i]->flag = 1;
                            nstat.ge_cs[bigbuf.ev[i]->tid]++;
//                                  printf("%i/%i suppressed ",bigbuf.ev[j]->tpe,bigbuf.ev[j]->tid);
//                                  printf("%i/%i; ",bigbuf.ev[i]->tpe,bigbuf.ev[i]->tid);
//                                  printf("TS: %lli/%lli\n",bigbuf.ev[j]->LEDts,bigbuf.ev[i]->LEDts);
                          }

                      };
              };


          CEvent.lenbgo = 0;
          CEvent.lendirty = 0;
          CEvent.lenclean = 0;
          for (i = (ii - CEvent.len); i < ii; i++)
            {

              nstat.id_hit[bigbuf.ev[i]->id]++;

              if (bigbuf.ev[i]->tpe == BGO)
                CEvent.lenbgo++;
              else if (bigbuf.ev[i]->tpe == GE)
                {
                  if (bigbuf.ev[i]->flag)
                    CEvent.lendirty++;
                  else
                    CEvent.lenclean++;
                }
              else if (bigbuf.ev[i]->tpe == SIDE)
                {
                }
              else if (bigbuf.ev[i]->tpe == AUX)
                {
                }
              else if (bigbuf.ev[i]->tpe == DSSD)
                {
                }
              else if (bigbuf.ev[i]->tpe == FP)
                {
                }
              else if (bigbuf.ev[i]->tpe == XARRAY)
                {
                }
              else if (bigbuf.ev[i]->tpe == CHICO2)
                {
                }
              else
                {
                  nstat.badid++;
                  printf ("GTMerge: unidentified tID=%i ....\n", bigbuf.ev[i]->tpe);
                  bigbuf.ev[i]->tpe = NOTHING;
                  bigbuf.ev[i]->tid = 0;
//                  if (nstat.badid > 100)
//                    {
//                      printf ("too many bad IDs, quit after status report\n");
//                      reportStats (&timesThen,nfiles);
//                      exit (1);
//                   };
                };
            };

//          printf ("ii=%i  ", ii);
//          printf ("CEvent.len=%i\n", CEvent.len);

          /* total event counter */

          control.CurEvNo++;

          /* write some statistics now and then */

          if ((control.CurEvNo % reportinterval) == 0)
            reportStats (&timesThen, nfiles);

          bigbuf.nused += CEvent.len;

          /* find number of GS, FMA, DSSD, CHICO2 etc events */

          nGE = 0;
          nBGO = 0;
          nFP = 0;
          nDSSD = 0;
          nCHICO2 = 0;
          nGS = 0;
          for (i = (ii - CEvent.len); i < ii; i++)
            {
//              printf ("_&ii=%i, TS=%lli\n", i, bigbuf.ev[i]->TSTYPE);
              if (bigbuf.ev[i]->tpe == GE)
                nGE++;
              if (bigbuf.ev[i]->tpe == GE || bigbuf.ev[i]->tpe == BGO)
                nGS++;
              if (bigbuf.ev[i]->tpe == BGO)
                nBGO++;
              if (bigbuf.ev[i]->tpe == FP)
                nFP++;
              if (bigbuf.ev[i]->tpe == DSSD)
                nDSSD++;
              if (bigbuf.ev[i]->tpe == CHICO2 && bigbuf.ev[i]->tid == 1)
                nCHICO2++;
            }
///          printf ("nGE=%i nFP=%i nDSSD=%i nCHICO2=%i nBGO=%i\n", nGE, nFP, nDSSD, nCHICO2, nBGO);

//              if (CEvent.len > 1 && nCHICO2>0) exit (0);

          /* input statistics */

          for (i = (ii - CEvent.len); i < ii; i++)
            if (bigbuf.ev[i]->tpe >= 0 && bigbuf.ev[i]->tpe < MAXTPE)
              if (bigbuf.ev[i]->tid >= 0 && bigbuf.ev[i]->tid < MAXTID)
                nstat.in_hit[bigbuf.ev[i]->tpe][bigbuf.ev[i]->tid]++;

          if (CEvent.lenclean >= control.minmult && nGE >= control.minGE && nFP >= control.minFP
              && nDSSD >= control.minDSSD && nCHICO2 >= control.minCHICO2 && nGS >= control.minGS)

            {
              cevent++;
              ncib++;
//if(1)exit(0);
              if (cevent < 20)
                {
                  printf ("\n** cevent=%i\n", cevent);
                  printf ("  CEvent.lenclean=%i ", CEvent.lenclean);
                  printf ("nGE=%i ", nGE);
                  printf ("nGS=%i ", nGS);
                  printf ("nFP=%i ", nFP);
                  printf ("nDSSD=%i ", nDSSD);
                  printf ("nCHICO2=%i", nCHICO2);
                  printf ("\n");
                  for (i = (ii - CEvent.len); i < ii; i++)
                    {
                      printf ("__TS(%i)=%lli, tpe/tid=%i/%i, DT=%lli, flag=%i\n", i, bigbuf.ev[i]->TSTYPE,
                              bigbuf.ev[i]->tpe, bigbuf.ev[i]->tid, bigbuf.ev[i]->TSTYPE - curTS, bigbuf.ev[i]->flag);
                    }
                  printf ("\n");
                  //                 if (cevent == 5)
                  //                   exit (0);
                };

              /* energy calibration of germaniums */

              if (nGE > 0)
                for (i = (ii - CEvent.len); i < ii; i++)
                  if (bigbuf.ev[i]->tpe == GE)
                    {
//                    printf ("%i --> ", CEvent.GTEvent[i].ehi);
//                    printf (" bigbuf.ev[i]->tid=%i ", bigbuf.ev[i]->tid);
//                    printf (" ehigain[bigbuf.ev[i]->tid]=%f ", ehigain[bigbuf.ev[i]->tid]);
//                    printf (" ehioffset[bigbuf.ev[i]->tid]=%f ", ehioffset[bigbuf.ev[i]->tid]);
                      rn = (float) rand () / RAND_MAX;
//                    printf (" rn=%f ", rn);
                      r1 = (float) bigbuf.ev[i]->ehi + rn;
                      if (bigbuf.ev[i]->tid < NGE)
                        r1 = ehigain[bigbuf.ev[i]->tid] * r1 + ehioffset[bigbuf.ev[i]->tid];
                      else
                        r1 = 1;
                      bigbuf.ev[i]->ehi = (int) (r1 + 0.5);
//                    printf ("%i\n", CEvent.GTEvent[i].ehi);
                    };

              /* output statistics */

              for (i = (ii - CEvent.len); i < ii; i++)
                if (bigbuf.ev[i]->tpe >= 0 && bigbuf.ev[i]->tpe < MAXTPE)
                  if (bigbuf.ev[i]->tid >= 0 && bigbuf.ev[i]->tid < MAXTID)
                    nstat.out_hit[bigbuf.ev[i]->tpe][bigbuf.ev[i]->tid]++;

              /* write event header out */

              if (control.noCoinData == 0)
                {

                  if (control.zzipout == 0)
                    {
                      siz = write (outData, (char *) &CEvent.len, sizeof (unsigned short int));
                      nstat.outbytes += siz;
                      siz = write (outData, (char *) &CEvent.lenclean, sizeof (unsigned char));
                      nstat.outbytes += siz;
                      siz = write (outData, (char *) &CEvent.lendirty, sizeof (unsigned char));
                      nstat.outbytes += siz;
                      siz = write (outData, (char *) &CEvent.lenbgo, sizeof (unsigned char));
                      nstat.outbytes += siz;
                    }
                  else
                    {
                      siz = gzwrite (zoutData, (char *) &CEvent.len, sizeof (unsigned short int));
                      nstat.outbytes += siz;
                      siz = gzwrite (zoutData, (char *) &CEvent.lenclean, sizeof (unsigned char));
                      nstat.outbytes += siz;
                      siz = gzwrite (zoutData, (char *) &CEvent.lendirty, sizeof (unsigned char));
                      nstat.outbytes += siz;
                      siz = gzwrite (zoutData, (char *) &CEvent.lenbgo, sizeof (unsigned char));
                      nstat.outbytes += siz;
                    };

                };

              /* write event data out */

              if (control.zzipout == 0)
                {
                  for (i = (ii - CEvent.len); i < ii; i++)
                    {
                      siz = write (outData, (char *) bigbuf.ev[i], DGSEVENT_BASELEN);
                      nstat.outbytes += siz;
                      siz =
                        write (outData, (char *) &bigbuf.ev[i]->trace[0], bigbuf.ev[i]->traceLen * sizeof (short int));

 #if(1)

  /* debug print trace */

int kk;

  //printf ("bigbuf tracLen=%i\n", bigbuf.ev[i]->traceLen);
  for (kk = 0; kk < bigbuf.ev[i]->traceLen; kk++)
    printf ("%3i: %i\n", kk, bigbuf.ev[i]->trace[kk]);

  #endif

                      nstat.outbytes += siz;
                    };
                }
              else
                {
                  for (i = (ii - CEvent.len); i < ii; i++)
                    {
                      siz = gzwrite (zoutData, (char *) bigbuf.ev[i], DGSEVENT_BASELEN);
                      nstat.outbytes += siz;
                      siz =
                        gzwrite (zoutData, (char *) &bigbuf.ev[i]->trace[0],
                                 bigbuf.ev[i]->traceLen * sizeof (short int));
                      nstat.outbytes += siz;
                    };
                };

              /* write some of the first good events to */
              /* disk files for humans to inspect */

              if (control.nwritten < 30)
                {
                  sprintf (str, "GTMerge-cev_%6.6i.list", cevent);
                  fp3 = fopen (str, "w");
                  for (i = (ii - CEvent.len); i < ii; i++)
                    {
                      GTPrintEvent (fp3, (GTEVENT *) & bigbuf.ev[i], (DGSEVENT *) bigbuf.ev[i]);
                    };
                  fprintf (fp3, "tot lenth: %i; clean: %i, dirty: %i, bgo: %i\n", CEvent.len, CEvent.lenclean,
                           CEvent.lendirty, CEvent.lenbgo);
                  fclose (fp3);
                }
//if(1)exit(0);

              control.nwritten++;
              control.nwritten_chunk++;

              /* are we done? */

              if (control.nwritten >= maxNoEvents)
                {
                  printf ("\n\n**max number of events = %i reached; quit\n\n\n", control.nwritten);
                  goto done;
                };

              /* open a new chunk file? */

              if (control.nwritten_chunk > control.chunksiz)
                {

                  /* close old file */

                  close (outData);
                  if (control.zzipout == 1)
                    gzclose (zoutData);

                  /* open new file with next chunk number */

                  control.chunkno++;
                  outData = 0;
                  /*                        + name of data file */
                  /*                        |                   */
                  sprintf (str, "%s_%3.3i", argv[2], control.chunkno);
                  outData = open (str, O_WRONLY | O_CREAT | O_TRUNC, PMODE);

                  control.nwritten_chunk = 0;

                  if (outData == 0)
                    {
                      printf ("could not open output data file %s, quit!\n", str);
                      exit (1);
                    }
                  else
                    {
                      printf ("%s: output data file \"%s\" is open\n", argv[0], str);
                      fflush (stdout);
                    };

                  if (control.zzipout == 1)
                    zoutData = gzdopen (outData, "w");

                  printf ("control.chunkno=%i\n", control.chunkno);
                  printf ("control.nwritten_chunk=%i\n", control.nwritten_chunk);
                  printf ("outData=%i\n", (int) outData);

                  /* write output header file */

                  bzero ((char *) &outheader, sizeof (DGSHEADER));
                  outheader.id = 1000;
                  if (control.zzipout == 1)
                    siz = write (outData, (char *) &outheader, sizeof (DGSHEADER));
                  else
                    siz = gzwrite (zoutData, (char *) &outheader, sizeof (DGSHEADER));
                  printf ("header written to output file\n");

                };


//  printf("__output chunk # %i (chunksiz=%i)\n", control.chunkno, control.chunksiz);

            }
          else
            {
//            printf("event not written\n");
            };



          /* start of new event */

          curTS = bigbuf.ev[ii]->TSTYPE;
          ii++;
          CEvent.len = 1;
//         printf ("bottom: advanced curTS to %lli, CEvent.len now %i\n", curTS, CEvent.len);

        }

      /* update dtbtev for the event we used */

      for (i = 1; i < bigbuf.nused; i++)
        {
          i1 = (int) (bigbuf.ev[i]->TSTYPE - bigbuf.ev[i - 1]->TSTYPE);
          if (i1 < LENSP && i1 > 0)
            dtbtev[i1]++;
        }

      /* --------------------------  */
      /* we have processed bigbuf    */
      /* used first 'bigbuf.nused' elements */
      /* now we will replenish it    */
      /* --------------------------  */

      if (nstat.nbigbufreads < 20 )
        printf ("we made %4i coincidence events in this buffer (%i)\n", ncib, nstat.nbigbufreads);

#if(0)
      printf ("made all coincidences\n");
      printf ("bigbuf.truesize=%i\n", bigbuf.truesize);
      printf ("bigbuf.nused=%i\n", bigbuf.nused);
      printf ("bigbuf.size=%i\n", bigbuf.size);
      fflush (stdout);
#endif

#if(0)
      printf ("\nbefore refill\n");
      printf ("bigbuf.size=%i, bigbuf.nused=%i, (bigbuf.size - bigbuf.nused)=%i\n", bigbuf.size, bigbuf.nused,
              (bigbuf.size - bigbuf.nused));
      i1 = 10;
      i2 = bigbuf.nused;
#include "pbuf.h"
#endif


      /* ... but first we move the top of */
      /* the remaining buffer down, so we have */
      /* to sort less; it should be faster than having */
      /* the sort do the same job in a double loop */

      i = 0;
      for (j = bigbuf.nused; j < bigbuf.size; j++)
        {
          /* swap the pointers */

          tmpentry = bigbuf.ev[j];
          bigbuf.ev[j] = bigbuf.ev[i];
          bigbuf.ev[i] = tmpentry;

          i++;
        }
#if(0)
      printf ("copied end of buffer to start of buffer %i..%i to %i...%i\n", bigbuf.nused, bigbuf.size, 0, i);
      printf ("bigbuf.truesize=%i\n", bigbuf.truesize);
      printf ("bigbuf.nused=%i\n", bigbuf.nused);
      printf ("bigbuf.size=%i\n", bigbuf.size);
      fflush (stdout);
#endif

#if(0)
      /* debug print the buffer */

      printf ("\nafter moving top to bottom \n");
      printf ("bigbuf.size=%i, bigbuf.nused=%i, (bigbuf.size - bigbuf.nused)=%i\n", bigbuf.size, bigbuf.nused,
              (bigbuf.size - bigbuf.nused));
      i1 = 10;
      i2 = bigbuf.size - bigbuf.nused;
#include "pbuf.h"
#endif

      /* TBD */

      bigbuf.truesize = bigbuf.size - bigbuf.nused;

#if(0)
      printf ("ready to refil buffer\n");
      printf ("bigbuf.truesize=%i\n", bigbuf.truesize);
      printf ("bigbuf.nused=%i\n", bigbuf.nused);
      printf ("bigbuf.size=%i\n", bigbuf.size);
      fflush (stdout);
#endif

//      printf ("bigbuf.truesize=%i\n", bigbuf.truesize);
//      fflush(stdout);
//if(1)exit(0);

      i1 = bigbuf.size - bigbuf.nused;
      i2 = bigbuf.size;
      bigbuf.truesize == i1;
//      printf ("refill into %i...%i\n", i1, i2 - 1);

      for (nfe = i1; nfe < i2; nfe++)
        {

          /* ------------------------------------------------ */
          /* find lowest time stamp of the candicates we have */
          /* ------------------------------------------------ */

          nextTS = ULLONG_MAX;
          nextTSpoolIndex = -1;
          for (i = 0; i < nPoolEvents; i++)
            {
              if (DGSEvent[i].TSTYPE < nextTS)
                {
                  nextTS = DGSEvent[i].TSTYPE;
                  nextTSpoolIndex = i;
                };
            };

          /* -------------- */
          /* copy to bigbuf */
          /* -------------- */

          memcpy ((char *) bigbuf.ev[nfe], (char *) &DGSEvent[nextTSpoolIndex], sizeof (DGSEVENT));
          bigbuf.truesize++;
          /* --------------------------- */
          /* read in a replacement event */
          /* --------------------------- */

          if (control.fileActive[whatFile[nextTSpoolIndex]])
            {
#include "GTMerge_readnew.h"
            }

        };

#if(0)
      /* debug print the buffer */

      printf ("\nafter refill \n");
      printf ("bigbuf.size=%i, bigbuf.nused=%i, (bigbuf.size - bigbuf.nused)=%i\n", bigbuf.size, bigbuf.nused,
              (bigbuf.size - bigbuf.nused));
      i1 = 10;
      i2 = bigbuf.size - bigbuf.nused;
#include "pbuf.h"
#endif
//      if (1) exit (0);

      nstat.nbigbufreads++;
//    printf("%i: bigbuf.truesize=%i,bigbuf.size=%i \n",nstat.nbigbufreads,bigbuf.truesize, bigbuf.size);
//   reportStats (&timesThen,nfiles);

#if(0)
      printf ("bottom of loop\n");
      printf ("bigbuf.truesize=%i\n", bigbuf.truesize);
      printf ("bigbuf.nused=%i\n", bigbuf.nused);
      printf ("bigbuf.size=%i\n", bigbuf.size);
      fflush (stdout);
#endif

    };



done:

  /* close all files */

  close (outData);
  for (i = 0; i < nfiles; i++)
#if(USEZLIB==0)
    close (inData[i]);
#endif
#if(USEZLIB==1)
  gzclose (zFile[i]);
#endif

  printf ("closed all files\n");
  printf ("\n");

  /* done */

  printf ("\n");
  printf ("hit statistics hit/Compton-suppress fraction\n");
  printf ("\n");
  for (i = 0; i <= NGE; i++)
    if (nstat.ge_hit[i] > 0)
      {
        r1 = 100.0 * (float) nstat.ge_cs[i] / (float) nstat.ge_hit[i];
        printf ("%3i: %10i %10i %5.1f%%\n", i, nstat.ge_hit[i], nstat.ge_cs[i], r1);
      };

  printf ("\n");
  printf ("tpe:tid  in and out statistics\n");
  printf ("\n");
  for (i = 0; i < MAXTPE; i++)
    for (j = 0; j < MAXTID; j++)
      if (nstat.in_hit[i][j] > 0)
        {
          if (i == BGO)
            printf ("BGO    :");
          else if (i == GE)
            printf ("GE     :");
          else if (i == SIDE)
            printf ("SIDE  :");
          else if (i == AUX)
            printf ("AUX    :");
          else if (i == DSSD)
            printf ("DSSD   :");
          else if (i == FP)
            printf ("FP     :");
          else if (i == XARRAY)
            printf ("XARRAY :");
          else if (i == CHICO2)
            printf ("CHICO2 :");
          else
            printf ("?????? :");
          printf ("tID= %3i :> ", j);

          printf ("in %6i ", nstat.in_hit[i][j]);
          printf ("out %6i ", nstat.out_hit[i][j]);

          r1 = 100 * (float) nstat.out_hit[i][j] / (float) nstat.in_hit[i][j];
          printf ("in/out fraction %9.4f %%; ", r1);

          r1 = 100 * (float) nstat.out_hit[i][j] / control.nwritten;
          printf ("coin event fraction %9.4f %%\n", r1);

        };

  i = LENSP;
  wr_spe ("dtbtev.spe", &i, dtbtev);
  printf ("wrote \"dtbtev.spe\"\n");
  printf ("\n");
  reportStats (&timesThen, nfiles);

  printf ("\n");
  r1 = (bigbuf.size + 1) * sizeof (DGSEVENT);
  r1 /= 1024000000;
  printf ("used big buf of size %9.3f GBytes\n", r1);
  printf ("\n");
  printf ("\n");
  printf ("wrote %i events to::\n", control.nwritten);
  fflush (stdout);
//sprintf (str, "%s_%3.3i", argv[2], control.chunkno);
  sprintf (str, "file %s_*; ls -l %s_*", argv[2], argv[2]);
//  printf("executing \"%s\"", str);
  system (str);
  fflush (stdout);
  printf ("\n");
  fflush (stdout);

  printf ("\n");
  printf ("Boniva Sancta ;-) >> GTMerge3 did not crash << !!\n");
  printf ("\n");
  printf ("\n");

#if(0)
  printf ("sizeof(GTEVENT)=%i\n", sizeof (GTEVENT));
  printf ("sizeof(DGSEVENT)=%i\n", sizeof (DGSEVENT));
  printf ("DGSEVENT_BASELEN=%i\n", DGSEVENT_BASELEN);
#endif

  printf ("\n");
  fflush (stdout);
  exit (0);

}
