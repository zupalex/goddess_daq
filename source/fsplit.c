
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

#include "GTMerge.h"

/*---------*/
/* globals */
/*---------*/

STAT nstat;
GTEVENT Event[MAXCOINEV];
int nBadTestPat;
CONTROL control;
DGSEVENT DGSEvent[MAXCOINEV];
int tlkup[NCHANNELS];
int tid[NCHANNELS];
off_t inData[MAXFILES];
off_t outData[MAXBOARDID];

/* original buffer read */

int Counter;
int TEMP[MAXLENINTS];

int idlim_lo[MAXTPE];
int idlim_hi[MAXTPE];
#if (USEBREAD)
int *bread_buf[MAXFILES];
int bread_pos[MAXFILES];
int bread_bufsiz[MAXFILES];
#endif

/*----------------------------------------------------------------------------*/

int
main (int argc, char **argv)
{

  /* declarations */

  int i, nn, st, bID, cID = 0, siz, nfiles=1;
  int IDs[MAXBOARDID], nreads = 0;
  char str[512];
  unsigned long long int oldTS = 0;
  int GTGetDiskEv (int, int);
  int GTEvdecompose (GTEVENT *, DGSEVENT *);

  /* hello */

  if (argc != 2)
    {
      printf ("use: %s <file>\n", argv[0]);
      exit (1);
    };

  /* initialize */

  for (i = 0; i < MAXBOARDID; i++)
    IDs[i] = 0;

#if (USEBREAD)

  for (i = 0; i < nfiles; i++)
    {
      bread_buf[i] = (int *) calloc (BREAD_BUFSIZE, sizeof (int));
      bread_pos[i] = 0;
      bread_bufsiz[i] = BREAD_BUFSIZE;
    };

#endif

  /* open input file */

  i = 0;
  nn = 1;
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

  /* read until there is no data left */

  while (1)
    {

      /* read event */

      st = GTGetDiskEv (0, 0);
      control.nread++;
//      printf ("st=%i\n", st);
//      fflush (stdout);

      while (st != 0 && st != 2 && st != 5)
        {
          st = GTGetDiskEv (0, 0);
          control.nread++;
          printf (".st=%i\n", st);
          fflush (stdout);
          if (st != 0)
            {
              printf ("2: GTGetDiskEv returned %i\n", st);
            }
        }


      /* check if we are done */

      if (st == 2 || st == 5)
        {
          printf ("failed to read more data from %i\n", nn);

          /* hit stat */

          for (i = 0; i < MAXBOARDID; i++)
            if (IDs[i] > 0)
              printf ("ID %5i: %6i\n", i, IDs[i]);

          printf ("succesfully read %i events\n", nreads);

          exit (0);
        };

      /* decompose */

      nreads++;
      bID = -1;
      if (st == 0)
        {
          GTEvdecompose ((GTEVENT *) & Event[0], (DGSEVENT *) & DGSEvent[0]);
          bID = DGSEvent[0].id / 10;
          cID = DGSEvent[0].id - 10 * bID;
        };

      /* check if board file is open */

      if (outData[bID] == 0)
        {
          sprintf (str, "%s_b%i", argv[1], bID);
          outData[bID] = open (str, O_WRONLY | O_CREAT | O_TRUNC, PMODE);
          if (outData[bID] == 0)
            {
              printf ("could not open output data file %s, quit!\n", str);
              exit (1);
            };
          printf ("board ID= %4i, %s,outData[]=%i\n", bID, str, (int) outData[bID]);
        };

      /* write event to this board file */

      IDs[bID]++;

//      for (j=0;j<Counter;j++)
//        printf("%i\n", TEMP[i]);

      siz = write (outData[bID], (char *) &TEMP[0], 4 * Counter);
      if (siz != 4 * Counter)
        printf ("write error\n");

      if (control.nread < 1000)
        {
          printf ("board ID= %4i, ch=%2i, TS=%15lli, dTS=%15lli, siz=%i\n", bID, cID, DGSEvent[0].LEDts,
                  (long long int) DGSEvent[0].LEDts - oldTS, siz);
          oldTS = (long long int) DGSEvent[0].LEDts;
        };

      fflush (stdout);
    }



  /* done */

  printf ("\n");
  printf ("%s done\n", argv[0]);
  printf ("\n");
  exit (0);

}
