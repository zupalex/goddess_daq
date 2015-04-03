
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




/*---------*/
/* globals */
/*---------*/

long __sysconf (int);

#ifdef SOLARIS
#include <strings.h>
#include <string.h>
#define CLK_TCK ((clock_t)_sysconf(3))
#endif

#ifdef LINUX
#include <string.h>
#define CLK_TCK ((__clock_t) __sysconf (2))
#endif

/* at the moment, the sorting seems to be on */
/* LEDts, so if you use CFDts we run into trouble */
/* with the code as is. Maybe if we use fsplit it works? */

#define TSTYPE LEDts
/* #define TSTYPE CFDts */

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
off_t inData[MAXFILES];

int Counter;
int TEMP[MAXLENINTS];


long long int oldFileTS[MAXCOINEV];
int whatFile[MAXCOINEV];
long long int curTS, firstTS;
COINEV CEvent;
int NprintEvNo = 0;
//int idOffset = 1000;
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


/*----------------------------------------------------------------------------*/

int
reportStats (struct tms *timesThen)
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
  printf ("statistics at current event no: %15i\n", control.CurEvNo);
  printf ("read attempts.................: %15i\n", control.nread);
  printf ("events written................: %15i\n", control.nwritten);
  printf ("current TS....................: %15lli\n", curTS);
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

  d1 = timesNow.tms_utime - timesThen->tms_utime;
  totalTime = d1;
  printf ("CPU time: user %9.3f sec, ", d1 / CLK_TCK);
  r1 = timesNow.tms_stime - timesThen->tms_stime;
  totalTime += d1;
  printf ("system  %9.3f sec, ", d1 / CLK_TCK);
  totalTime /= CLK_TCK;
  printf ("Total  %9.3f sec\n", totalTime);


  printf ("total reads...........=%10i\n", nreads);

  printf ("control.nzeroehi......=%10i; ", control.nzeroehi);
  r1 = 100.0 * (float) control.nzeroehi / (float) nreads;
  printf ("or %9.6f%%\n", r1);

  printf ("control.noverflowehi..=%10i; ", control.noverflowehi);
  r1 = 100.0 * (float) control.noverflowehi / (float) nreads;
  printf ("or %9.6f%%\n", r1);

  printf ("nstat.nTSjumprecover_f=%10i; ", nstat.nTSjumprecover_f);
  r1 = 100.0 * (float) nstat.nTSjumprecover_f / (float) nreads;
  printf ("or %9.6f%%\n", r1);

  printf ("nstat.nTSjumprecover_b=%10i; ", nstat.nTSjumprecover_b);
  r1 = 100.0 * (float) nstat.nTSjumprecover_b / (float) nreads;
  printf ("or %9.6f%%\n", r1);

  printf ("nstat.badid...........=%10lli; ", nstat.badid);
  r1 = 100.0 * (float) nstat.badid / (float) nreads;
  printf ("or %9.6f%%\n", r1);

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

  printf ("output chunk # %i (chunksiz=%i)\n", control.chunkno, control.chunksiz);

  printf ("^^----------------------------------------\n");
  printf ("\n");
  fflush (stdout);

  /* done */

  return (0);

};

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

/*----------------------------------------------------------------------------*/

int
main (int argc, char **argv)
{

  /* declarations */

  off_t outData;                /* input file */
  int nfiles = 0, maxNoEvents = 0, nPoolEvents = 0, siz;
  int i1, i2, i3, badRead;
  int i, j, st, nextTSpoolIndex, nn, argcoffset = 0;
  DGSHEADER dgsHeader[MAXFILES], outheader;
  long long int dt = 0, nextTS, oldts = 0, dTS, lltmp;
  float dtbtev[LENSP];
  float ehigain[NGE + 1], ehioffset[NGE + 1], r1, r2, rn;
  FILE *fp1, *fp3, *fp;
  char str[132], str1[512];
  unsigned int seed = 0;
  struct tms timesThen;
  int i7, i8, nGE, nFP, nDSSD, reportinterval;
  char *p, *pc;
  int echo = 0, nni, nret;

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


  /* open chat file */

  if ((fp = fopen (argv[1], "r")) == NULL)
    {
      printf ("error: could not open chat file: <%s>\n", argv[1]);
      exit (0);
    };
  printf ("chat file: <%s> open\n", argv[1]);
  printf ("\n");
  fflush (stdout);

  /* read chatfile content and act */

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
      else if ((p = strstr (str, "TSlistelen")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &control.TSlistelen);
          CheckNoArgs (nret, 2, str);
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
      else if ((p = strstr (str, "zerosuppress")) != NULL)
        {
          control.zerosuppress = 1;
          nret = sscanf (str, "%s %i", str1, &control.zerosuppresslim);
          CheckNoArgs (nret, 2, str);
          printf ("control.zerosuppresslim=%i\n", control.zerosuppresslim);
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
  fclose (fp);

  /* extract and repeat parameters */

  printf ("%s: will produce a max of %i events\n", argv[0], maxNoEvents);
  printf ("%s: will use dt of %lli\n", argv[0], dt);

  printf ("%s: will write combined data to file \"%s\"\n", argv[0], argv[2]);


  printf ("control.minmult=%i\n", control.minmult);
  printf ("control.minGE=%i\n", control.minGE);
  printf ("control.minFP=%i\n", control.minFP);
  printf ("control.minDSSD=%i\n", control.minDSSD);
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

  /* simple ts list file */

  TSlist = fopen ("TS.list", "w");

  /************************/
  /* open all input files */
  /************************/

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
        }


    };

#if (USEBREAD)

  for (i = 0; i < nfiles; i++)
    {
      bread_buf[i] = (int *) calloc (BREAD_BUFSIZE, sizeof (int));
      bread_pos[i] = 0;
      bread_bufsiz[i] = BREAD_BUFSIZE;
    };

#endif

  /*-------------*/
  /* output file */
  /*-------------*/


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

  /* write output header file */

  bzero ((char *) &outheader, sizeof (DGSHEADER));
  outheader.id = 1000;
  siz = write (outData, (char *) &outheader, sizeof (DGSHEADER));
  printf ("header written to output file\n");

  /*----------------------*/
  /* read in the map file */
  /*----------------------*/

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
  i2 = fscanf (fp, "\n%i %i %i", &i1, &i7, &i8);
  while (i2 == 3)
    {
      tlkup[i1] = i7;
      tid[i1] = i8;
      i2 = fscanf (fp, "\n%i %i %i", &i1, &i7, &i8);
    };
  fclose (fp);

  /*--------------------------------*/
  /* see if we have ge ehi cal file */
  /*--------------------------------*/

  for (i = 0; i <= NGE; i++)
    {
      ehigain[i] = 1;
      ehioffset[i] = 0;
    };

  fp1 = fopen ("ehi.cal", "r");
  if (fp1 != NULL)
    {
      printf ("found ehi.cal file\n");
//    getline(str,132,fp1);
      fscanf (fp1, "%s\n", str);
      fscanf (fp1, "%s\n", str);
      fscanf (fp1, "%s\n", str);
      fscanf (fp1, "%s\n", str);
      i2 = fscanf (fp1, "\n%i %f %f", &i1, &r1, &r2);
      printf ("i2=%i\n", i2);
      while (i2 == 3)
        {
          if (i1 <= NGE && i1 > 0)
            {
              ehigain[i1] = r2;
              ehioffset[i1] = r1;
              printf ("ge %3i, offset=%6.2f gain=%9.4f\n", i1, ehioffset[i1], ehigain[i1]);
            };
          i2 = fscanf (fp1, "\n%i %f %f", &i1, &r1, &r2);
        }
      fclose (fp1);
    }

  /* start timer */

  times ((struct tms *) &timesThen);


  /*----------------------------------------------*/
  /* read until we have filled our pool of events */
  /*----------------------------------------------*/

  nn = 0;
  for (i = 0; i < nPoolEvents; i++)
    {
      st = GTGetDiskEv (nn, i);
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
      if (st == 0)
        GTEvdecompose ((GTEVENT *) & Event[i], (DGSEVENT *) & DGSEvent[i]);
      oldFileTS[i] = DGSEvent[i].TSTYPE;
      whatFile[i] = nn;
      nn++;
      if (nn >= nfiles)
        nn = 0;
    };

  /* print first timestamps and check them */

  for (i = 0; i < nPoolEvents; i++)
    {

      if (DGSEvent[i].TSTYPE == (long long int) 0)
        printf ("ooops, file %3i has TS = %22lli, will re read\n", i, DGSEvent[i].TSTYPE);
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
            oldFileTS[i] = DGSEvent[i].TSTYPE;
          };

      };

      printf ("file %3i, first TS %22lli;\n", i, DGSEvent[i].TSTYPE);

    };

  printf ("\n");
  printf ("initial pool of events read (%i events)\n", nPoolEvents);
  reportStats (&timesThen);
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
  oldts = nextTS;
  firstTS = nextTS;

  printf ("\n");
  printf ("First TS=%lli, from file %i\n", nextTS, nextTSpoolIndex);


  /* let the first/lowest timestamp be the start of the first event */

  CEvent.len = 0;
  curTS = nextTS;
  memcpy ((char *) &CEvent.GTEvent[CEvent.len], (char *) &Event[nextTSpoolIndex], sizeof (GTEVENT));
  memcpy ((char *) &CEvent.DGSEvent[CEvent.len], (char *) &DGSEvent[nextTSpoolIndex], sizeof (DGSEVENT));
  CEvent.len++;

  /* ___ and read in a replacement */

  if (control.fileActive[whatFile[nextTSpoolIndex]])
    {

#include "GTMerge_readnew.h"

    };

  printf ("GTMerge: we are primed with first event...\n\n\n");


  for (i = 0; i < LENSP; i++)
    dtbtev[i] = 0;

  /*--------------------------------------------------------*/
  /* at this point we have the first member of the output.. */
  /* event defined and we have a pool of events to evaluate */
  /*--------------------------------------------------------*/

  while (control.nwritten < maxNoEvents)
    {

      NprintEvNo++;

      /*--------------------------------------------------*/
      /* find lowest time stamp of the candicates we have */
      /*--------------------------------------------------*/

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

      /* bin the dtbtev */

      i1 = (int) (nextTS - oldts);
      if (i1 >= 0 && i1 < LENSP)
        dtbtev[i1]++;
      oldts = nextTS;


      /* just to be sure */

      assert (nextTS >= curTS);

      /* if this timestamp is within the dt limits, include */
      /* and read in a replacement timestamp - else close and write */
      /* out current event and start a new one */

      if (nextTS <= (curTS + dt))
        {

          /* this gamma ray falls within current time window, so */
          /* include in current coincidence event */

          assert (CEvent.len < MAXCOINEV);
          assert (nextTSpoolIndex < MAXCOINEV);
          memcpy ((char *) &CEvent.GTEvent[CEvent.len], (char *) &Event[nextTSpoolIndex], sizeof (GTEVENT));
          memcpy ((char *) &CEvent.DGSEvent[CEvent.len], (char *) &DGSEvent[nextTSpoolIndex], sizeof (DGSEVENT));
          CEvent.len++;

          /* read in a replacement event */

          if (control.fileActive[whatFile[nextTSpoolIndex]])
            {

#include "GTMerge_readnew.h"

            }

          /* NOTE: we do not change curTS here */

        }
      else
        {

#if(0)
          for (i = 0; i < CEvent.len; i++)
            {
              GTPrintHeader (stdout, (GTEVENT *) & Event[i], (DGSEVENT *) & DGSEvent[i]);
              GTPrintEvent (stdout, (GTEVENT *) & CEvent.GTEvent[i]);
            };
          fflush (stdout);
#endif

          /* if we get here the event is complete, -- as in the next */
          /* gamma ray is in the next bucket outside current time window */

          cevent++;

          /* find and mark compton suppressed germaniums */

          for (i = 0; i < CEvent.len; i++)
            if (CEvent.DGSEvent[i].tpe == GE)
              {
                CEvent.DGSEvent[i].flag = 0;
                nstat.ge_hit[CEvent.DGSEvent[i].tid]++;
                for (j = 0; j < CEvent.len; j++)
                  if (i != j)
                    if (CEvent.DGSEvent[j].tpe == BGO && CEvent.DGSEvent[j].ehi > 5)
                      {
                        if ((CEvent.DGSEvent[i].id == (CEvent.DGSEvent[j].id + 5)))
                          {
                            //if((CEvent.DGSEvent[j].TSTYPE - CEvent.DGSEvent[i].TSTYPE) < 40){   // CHANGED
                            CEvent.DGSEvent[i].flag = 1;
                            nstat.ge_cs[CEvent.DGSEvent[i].tid]++;
                            //}
                          }
                      };
              };


          /* find the clean/dirty/bgo multiplicities we have now */

          CEvent.lenbgo = 0;
          CEvent.lendirty = 0;
          CEvent.lenclean = 0;
          for (i = 0; i < CEvent.len; i++)
            {
              if (CEvent.DGSEvent[i].tpe == BGO)
                CEvent.lenbgo++;
              else if (CEvent.DGSEvent[i].tpe == GE)
                {
                  if (CEvent.DGSEvent[i].flag)
                    CEvent.lendirty++;
                  else
                    CEvent.lenclean++;
                }
              else if (CEvent.DGSEvent[i].tpe == SIDE)
                {
                }
              else if (CEvent.DGSEvent[i].tpe == AUX)
                {
                }
              else if (CEvent.DGSEvent[i].tpe == DSSD)
                {
                }
              else if (CEvent.DGSEvent[i].tpe == FP)
                {
                }
              else if (CEvent.DGSEvent[i].tpe == XARRAY)
                {
                }
              else
                {
                  nstat.badid++;
//                  printf ("GTMerge: unidentified tID=%i ....\n", CEvent.DGSEvent[i].tpe);
                  CEvent.DGSEvent[i].tpe = NOTHING;
                  CEvent.DGSEvent[i].tid = 0;
//                  if (nstat.badid > 100)
//                    {
//                      printf ("too many bad IDs, quit after status report\n");
//                      reportStats (&timesThen);
//                      exit (1);
//                   };
                };
            };


          /* total event counter */

          control.CurEvNo++;

          /* find number of GS and FMA events */

          nGE = 0;
          nFP = 0;
          nDSSD = 0;
          for (i = 0; i < CEvent.len; i++)
            {
              if (CEvent.DGSEvent[i].tpe == GE)
                nGE++;
              if (CEvent.DGSEvent[i].tpe == FP)
                nFP++;
              if (CEvent.DGSEvent[i].tpe == DSSD)
                nDSSD++;
            }

          /* write out current event if the multiplicty requirements is OK */

          if (CEvent.lenclean >= control.minmult && nGE >= control.minGE && nFP >= control.minFP
              && nDSSD >= control.minDSSD)

            {
#if(0)
              printf ("control.minmult=%i\n", control.minmult);
              printf ("control.minGE=%i\n", control.minGE);
              printf ("control.minFP=%i\n", control.minFP);
              printf ("control.minDSSD=%i\n", control.minDSSD);
              printf ("CEvent.lenclean=%i\n", CEvent.lenclean);
              printf ("nGE=%i\n", nGE);
              printf ("nFP=%i\n", nFP);
              printf ("nDSSD=%i\n", nDSSD);
#endif

              /* energy calibration of germaniums */

              if (nGE > 0)
                for (i = 0; i < CEvent.len; i++)
                  if (CEvent.DGSEvent[i].tpe == GE)
                    {
//                    printf ("%i --> ", CEvent.GTEvent[i].ehi);
//                    printf (" CEvent.DGSEvent[i].tid=%i ", CEvent.DGSEvent[i].tid);
//                    printf (" ehigain[CEvent.DGSEvent[i].tid]=%f ", ehigain[CEvent.DGSEvent[i].tid]);
//                    printf (" ehioffset[CEvent.DGSEvent[i].tid]=%f ", ehioffset[CEvent.DGSEvent[i].tid]);
                      rn = (float) rand () / RAND_MAX;
//                    printf (" rn=%f ", rn);
                      r1 = (float) CEvent.DGSEvent[i].ehi + rn;
                      if (CEvent.DGSEvent[i].tid < NGE)
                        r1 = ehigain[CEvent.DGSEvent[i].tid] * r1 + ehioffset[CEvent.DGSEvent[i].tid];
                      else
                        r1 = 1;
                      CEvent.DGSEvent[i].ehi = (int) (r1 + 0.5);
//                    printf ("%i\n", CEvent.GTEvent[i].ehi);
                    };
              for (i = 0; i < CEvent.len; i++)
                if (CEvent.DGSEvent[i].tpe >= 0 && CEvent.DGSEvent[i].tpe < MAXTPE)
                  if (CEvent.DGSEvent[i].tid >= 0 && CEvent.DGSEvent[i].tid < MAXTID)
                    nstat.out_hit[CEvent.DGSEvent[i].tpe][CEvent.DGSEvent[i].tid]++;

              /* write coincidence header, unless we */
              /* are in the no coincidence mode */

              if (control.noCoinData == 0)
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

              /* write the coincidence data */

              for (i = 0; i < CEvent.len; i++)
                {
                  siz = write (outData, (char *) &CEvent.DGSEvent[i], DGSEVENT_BASELEN);
                  nstat.outbytes += siz;
//                printf("CEvent.DGSEvent[i].traceLen=%i\n",CEvent.DGSEvent[i].traceLen);
                  siz = write (outData, (char *) &CEvent.DGSEvent[i].trace[0],
                               CEvent.DGSEvent[i].traceLen * sizeof (short int));
                  nstat.outbytes += siz;
                };

              control.nwritten++;
              control.nwritten_chunk++;

#if(1)
              /* open a new chunk file? */

              if (control.nwritten_chunk > control.chunksiz)
                {

                  /* close old file */

                  close (outData);

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

                  printf ("control.chunkno=%i\n", control.chunkno);
                  printf ("control.nwritten_chunk=%i\n", control.nwritten_chunk);
                  printf ("outData=%i\n", (int) outData);

                  /* write output header file */

                  bzero ((char *) &outheader, sizeof (DGSHEADER));
                  outheader.id = 1000;
                  siz = write (outData, (char *) &outheader, sizeof (DGSHEADER));
                  printf ("header written to output file\n");

                };
#endif

              /* write some of the first good events to disk */

              if (control.nwritten < 30)
                {
                  //printf("Yes!\n");
                  sprintf (str, "GTMerge-cev_%6.6i.list", cevent);
                  fp3 = fopen (str, "w");
                  for (i = 0; i < CEvent.len; i++)
                    {
//                      GTPrintHeader (fp3, (GTEVENT *) & Event[i]);
                      GTPrintEvent (fp3, (GTEVENT *) & CEvent.GTEvent[i], (DGSEVENT *) & CEvent.DGSEvent[i]);
                    };
                  fprintf (fp3, "tot lenth: %i; clean: %i, dirty: %i, bgo: %i\n", CEvent.len, CEvent.lenclean,
                           CEvent.lendirty, CEvent.lenbgo);
                  fclose (fp3);
                }

//  printf("__output chunk # %i (chunksiz=%i)\n", control.chunkno, control.chunksiz);

            }
          else
            {
            };


          /* make start of new coincidence event */

          CEvent.len = 0;
          memcpy ((char *) &CEvent.GTEvent[CEvent.len], (char *) &Event[nextTSpoolIndex], sizeof (GTEVENT));
          memcpy ((char *) &CEvent.DGSEvent[CEvent.len], (char *) &DGSEvent[nextTSpoolIndex], sizeof (DGSEVENT));
          CEvent.len++;

          /* read in a replacement event */

          if (control.fileActive[whatFile[nextTSpoolIndex]])
            {

#include "GTMerge_readnew.h"

            };
          curTS = nextTS;

        };

    };

done:

  /* close all files */

  close (outData);
  for (i = 0; i < nfiles; i++)
    close (inData[i]);
  printf ("closed all files\n");
  printf ("\n");

  /* done */

  printf ("\n");
  printf ("hit statistics hit/comptonsuppres fraction\n");
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
  reportStats (&timesThen);

  printf ("\n");
  printf ("gtMerge is done\n");
  printf ("\n");

  printf ("\n");
  fflush (stdout);
  exit (0);

}
