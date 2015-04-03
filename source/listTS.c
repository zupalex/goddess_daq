
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
#define LONGLEN 16384

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
unsigned int TEMP[MAXLENINTS];


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

#if (USEBREAD)
int *bread_buf[MAXFILES];
int bread_pos[MAXFILES];
int bread_bufsiz[MAXFILES];
#endif

/* big buffer */

BIGBUF bigbuf;
#define FJUMP 100000000

/* -------------------------------------------------------------------------- */

int
reportStats (struct tms *timesThen, int nfiles)
{

  /* declarations */

  int i, nreads = 0, hh, mm, ss;
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
  printf ("total reads=%9i\n", nreads);
  d1 = (double) (curTS - firstTS);
  d1 *= 10e-9;
  hh = d1 / 3600;
  d1 -= hh * 3600;
  mm = d1 / 60;
  d1 -= mm * 60;
  ss = d1;
  printf ("elapsed time..................: %3.3ih%2.2im%2.2is, [t0=%lli]\n", hh, mm, ss, firstTS);
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
      printf ("nstat.nTSjumprecover_f[%i]=%10i; ", i, nstat.nTSjumprecover_f[i]);
      r1 = 100.0 * (float) nstat.nTSjumprecover_f[i] / (float) nreads;
      printf ("or %9.6f%%\n", r1);

      printf ("nstat.nTSjumprecover_b[%i]=%10i; ", i, nstat.nTSjumprecover_b[i]);
      r1 = 100.0 * (float) nstat.nTSjumprecover_b[i] / (float) nreads;
      printf ("or %9.6f%%\n", r1);

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

  int nfiles = 0,  nPoolEvents = 0, firsttime;
  unsigned long long int ntot = 0, nbad_b = 0, nbad_f = 0, nn, nsp=0;
  int i1, i2;
  int i, j, k, st, argcoffset = 0, readAsk_lo, readAsk_hi;
  long long int dt = 0, oldListTS, TSlo, TShi, startTS, nstart=0,spstartTS;
  long long int lowTS = LLONG_MAX, hiTS = -LLONG_MAX;
  float ehigain[NGE + 1], ehioffset[NGE + 1], r1;
  FILE *fp1, *fp;
  char str[132], fn_ehical[512], fn_toff[512], fnn[128];
  long long int toff[MAXTPE][MAXTID];
  unsigned int seed = 0;
  struct tms timesThen;
  int i7, i8;
  double d1, d2;
  DGSEVENT *tmpentry;
  float sp[LONGLEN], aa, bb;

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

  if (argc != 6)
    {
      printf ("use:  %s  lo hi TSlo TShi  file \n", argv[0]);
      exit (0);
    };

  /* initialize random number generator etc */

  readAsk_lo = atoi (argv[1]);
  readAsk_hi = atoi (argv[2]);
  TSlo = atoll (argv[3]);
  TShi = atoll (argv[4]);

  printf ("read/report from %i --> %i\n", readAsk_lo, readAsk_hi);
  printf ("TS range %lli --> %lli\n", TSlo, TShi);

  get_a_seed (&seed);
  srand (seed);
  bzero ((char *) &control, sizeof (CONTROL));
  bzero ((char *) &nstat, sizeof (STAT));
  
for (i = 0; i < LONGLEN; i++) sp[i]=0;

  for (i = 0; i < MAXTPE; i++)
    {
      idlim_hi[i] = 0;
      idlim_lo[i] = 0;
      for (j = 0; j < MAXTID; j++)
        {
          nstat.in_hit[i][j] = 0;
          nstat.out_hit[i][j] = 0;
        };
    };

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

  for (i = 0; i < MAXTPE; i++)
    {
      idlim_lo[i] = 0;
      idlim_hi[i] = 2000000;
    };


  /* offset for data file name reads, find  nfiles etc */

  argcoffset = 5;
  nfiles = (argc - argcoffset);
  printf ("%s: we have %i datafiles to combine\n", argv[0], nfiles);
  fflush (stdout);

  nPoolEvents = nfiles;
  printf ("%s: will keep a pool of %i events to combine from \n", argv[0], nPoolEvents);
  assert (nPoolEvents < MAXCOINEV);

  /* just to be sure */

  assert (nfiles == nPoolEvents);

  /* simple ts list file */

  TSlist = fopen ("TS.list", "w");

  /************************/
  /* open all input files */
  /************************/

#if(USEZLIB==0)
  for (i = 0; i < nfiles; i++)
    {
      i1 = i + argcoffset;
      strcpy (fnn, argv[i1]);
      inData[i] = open (argv[i1], O_RDONLY, 0);
      if (inData[i] < 0)
        {
          printf ("could not open input data file [%i] %s, quit!\n", i1, argv[i1]);
          exit (1);
        }
      else
        {
          printf ("%s: input data file \"%s\", number %i, is open\n", argv[0], argv[i1], i);
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
      i1 = i + argcoffset;
      zFile[i] = gzopen (argv[i1], "r");
      if (zFile[i] == NULL)
        {
          printf ("could not open input data file [%i] %s, quit!\n", i1, argv[i1]);
          exit (1);
        }
      else
        {
          printf ("%s: gzipped input data file \"%s\", number %i, is open\n", argv[0], argv[i1], i);
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
//  printf ("%i %i %i %s\n", i1, i7, i8, str);
  while (i2 == 4)
    {
      tlkup[i1] = i7;
      tid[i1] = i8;
      i2 = fscanf (fp, "\n%i %i %i %s", &i1, &i7, &i8, str);
//      printf ("%i %i %i %s\n", i1, i7, i8, str);
    };
  fclose (fp);


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

  /* simple read of data for debug purpose */

  assert (nfiles == 1);
  firsttime = 1;
  nn = 0;
  nbad_b = 0;
  nbad_f = 0;
  lowTS = LLONG_MAX;
  hiTS = (long long int) 0;
  for (i = 0; i < readAsk_hi; i++)
    {
      st = GTGetDiskEv (0, 0);
      if (st == 2 || st == 5)
        {
          printf ("failed to read more data from file %i\n", 0);
          printf ("read %10llu events\n", ntot);
          d1 = (double) nbad_f *100.0 / (double) ntot;
          printf ("     %10llu jumped forward   (%9.2f%%)\n", nbad_f, (float) d1);
          d1 = (double) nbad_b *100.0 / (double) ntot;
          printf ("     %10llu jumped backwards (%9.2f%%)\n", nbad_b, (float) d1);

          printf ("regular TS range %lli...", startTS);
          printf ("%lli\n", oldListTS);
          oldListTS -= startTS;
          d1 = (double) oldListTS / 100000000;
          printf ("__file covered %10.2f seconds\n", (float) d1);

          printf ("low/hi  TS range %lli...", lowTS);
          printf ("%lli\n", hiTS);
          hiTS -= lowTS;
          d1 = (double) hiTS / 100000000;
          printf ("__file covered %10.2f seconds\n", (float) d1);

          exit (0);
        };


      if (st == 0)
        {
          GTEvdecompose ((GTEVENT *) & Event[0], (DGSEVENT *) & DGSEvent[0]);
//          printf("1_listTS: DGSEvent[0].TSTYPE=%lli\n", DGSEvent[0].TSTYPE);

//          printf("%5i:  lowTS=%lli hiTS=%lli; TS=%lli\n", ntot, lowTS, hiTS,DGSEvent[0].TSTYPE );
          if (DGSEvent[0].TSTYPE > 0)
            {
              if (DGSEvent[0].TSTYPE > hiTS)
                {
//             printf("hi1\n");
                  hiTS = DGSEvent[0].TSTYPE;
                };
              if (DGSEvent[0].TSTYPE < lowTS)
                {
//             printf("hi2\n");
                  lowTS = DGSEvent[0].TSTYPE;
                }
            };
//          printf("%5i:  %lli %lli; TS=%lli\n", ntot, lowTS, hiTS, DGSEvent[0].TSTYPE );
//if(1)exit(0); 

          if (firsttime)
            {
              /* catch the first nonzero timestamp */

              ntot++;
              if (DGSEvent[0].TSTYPE > 0)
                firsttime = 0;
              ntot++;
              oldListTS = DGSEvent[0].TSTYPE;
              startTS = DGSEvent[0].TSTYPE;
              spstartTS = startTS;
              printf ("%s ", fnn);
              printf ("%7i:st=%i: TS=%20lli  ", i, st, DGSEvent[0].TSTYPE);
              printf ("START                    ");
              printf ("tpe=%i, tid=%i", DGSEvent[0].tpe, DGSEvent[0].tid);
              printf ("\n");
              fp1 = fopen ("x.cmd", "w");

            }
          else
            {
              /* examine interesting ones */

              ntot++;
              dt = (long long int) DGSEvent[0].TSTYPE - (long long int) oldListTS;

 if(nsp<20)
{
             i1 = (int) ntot -nstart;
              if (i1 < LONGLEN)
                {
                  d1 = (double) ((DGSEvent[0].TSTYPE - spstartTS));
                  d2 = (double)1000000000;
                  if (d1<d2 && d1>(double)-d2)
                    r1=(float)d1;
                  else if (d1<d2 && d1<-d2)
                    r1=-d2;
                  else if (d1>d2 && d1>-d2)
                    r1=d2;
                  sp[i1] = r1;
//                  printf (" ## %i, %f, %lli, %lli\n", i1, r1, DGSEvent[0].TSTYPE - startTS, startTS);
                }
              else
                {
                /* write current spectrum out */
          i1 = LONGLEN;
          nsp++;
          sprintf(str,"TSvsEv_%4.4i.spe", nsp);
          wr_spe (str, &i1, sp);
          nstart=(int) ntot;
          spstartTS=DGSEvent[0].TSTYPE;
          printf ("wrote %s\n",str);
          fflush (stdout);
};
                };

//             printf("2_listTS: DGSEvent[0].TSTYPE=%lli, dt=%lli\n", DGSEvent[0].TSTYPE, dt);

              if (dt < (long long int) 0)
                {
                  nbad_b++;
                  printf ("jump back    @evno %8lli, dt=%lli (old: %lli new: %lli)\n", ntot, dt, oldListTS,
                          DGSEvent[0].TSTYPE);
                  if ((int) ntot < LONGLEN)
                    {
                      fprintf (fp1, "cr %i\n", (int) ntot);
                    };
                };

              if (dt > (long long int) FJUMP)
                {
                  nbad_f++;
                  printf ("jump forward @evno %8lli, dt=%lli (old: %lli new: %lli)\n", ntot, dt, oldListTS,
                          DGSEvent[0].TSTYPE);
                };
#if(0)
              if (oldListTS >= TSlo && oldListTS <= TShi)
                if (i > readAsk_lo || dt < 0 || dt > 20000000)
//                if (i > readAsk_lo && DGSEvent[0].TSTYPE != (long long int) 0)
                  {
                    if (dt > (long long int) 4000)
                      printf ("\n");
                    printf ("%s ", fnn);
                    printf ("%7i:st=%i: TS=%20lli; ", i, st, DGSEvent[0].TSTYPE);
                    printf ("DT=%20lli ", dt);
                    printf ("or %8.0f msec  ", (float) (dt / 10000));
                    printf ("tpe=%2i, tid=%3i ", DGSEvent[0].tpe, DGSEvent[0].tid);
                    if (dt < 0 || dt > FJUMP)
                      printf ("ooopsie...");
                    printf ("\n");
                  };
#endif
            };

//          if (dt >= 0 && dt < FJUMP)
          if (DGSEvent[0].TSTYPE != (long long int) 0)
            oldListTS = DGSEvent[0].TSTYPE;
        };
      nn++;

    };

  printf ("total number of reads =%lli\n", ntot);
  d1 = 100 * (double) nbad_b / (double) ntot;
  printf ("nbad_b=%lli, %9.3f%%\n", nbad_b, d1);
  d1 = 100 * (double) nbad_f / (double) ntot;
  printf ("nbad_f=%lli, %9.3f%%\n", nbad_f, d1);
  printf ("\n");
  printf ("\n");
  printf ("\n");
  printf ("\n");
  printf ("\n");
  printf ("\n");
  fflush (stdout);
  exit (0);

}
