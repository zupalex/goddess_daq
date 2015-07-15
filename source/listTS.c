
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

#include "gdecomp.h"

/*----------------------------------------------------------------------------*/

int
GebTypeStr (int type, char str[])
{
//   printf("got type %i, %i\n", type,GEB_TYPE_DECOMP);
  if (type == GEB_TYPE_DECOMP)
    sprintf (str, "GEB_TYPE_DECOMP      ");
  else if (type == GEB_TYPE_BGS)
    sprintf (str, "GEB_TYPE_BGS         ");
  else if (type == GEB_TYPE_RAW)
    sprintf (str, "GEB_TYPE_RAW         ");
  else if (type == GEB_TYPE_TRACK)
    sprintf (str, "GEB_TYPE_TRACK       ");
  else if (type == GEB_TYPE_S800_RAW)
    sprintf (str, "GEB_TYPE_S800_RAW    ");
  else if (type == GEB_TYPE_NSCLnonevent)
    sprintf (str, "GEB_TYPE_NSCLnonevent");
  else if (type == GEB_TYPE_GT_SCALER)
    sprintf (str, "GEB_TYPE_GT_SCALER   ");
  else if (type == GEB_TYPE_GT_MOD29)
    sprintf (str, "GEB_TYPE_GT_MOD29    ");
  else if (type == GEB_TYPE_S800PHYSDATA)
    sprintf (str, "GEB_TYPE_S800PHYSDATA");
  else if (type == GEB_TYPE_G4SIM)
    sprintf (str, "GEB_TYPE_G4SIM       ");
  else if (type == GEB_TYPE_CHICO)
    sprintf (str, "GEB_TYPE_CHICO       ");
  else if (type == GEB_TYPE_NSCLNONEVTS)
    sprintf (str, "GEB_TYPE_NSCLNONEVTS ");
  else if (type == GEB_TYPE_DGS)
    sprintf (str, "GEB_TYPE_DGS         ");
  else
    sprintf (str, "%i is unknown, maybe update 'GebTypeStr' function?", type);
//      printf("type: %s\n",str);

};

/*----------------------------------------------------------------------------*/

int
main (int argc, char **argv)
{

  /* declarations */

  off_t inData;
  int i1, i2, i, siz, nn = 0, nbadTS = 0, nevents = 0, ncevents = 0;
  char storage[100000];
  GEBDATA gebheader;
  long long int lastTS = 0, firstTS, dTS, dtwin;
  float r1;
  int ntype[100], tsize = 0;;
  char str[128];
  double d1, nsec;

  /* help */

  if (argc < 2)
    {
      printf ("use: listTS file ntoprint twin\n");
      printf ("eg:  listTS file 1000     300\n");
      exit (1);
    }

  /* init */

  i1 = atoi (argv[2]);
  dtwin = atoll (argv[3]);

  for (i = 0; i < 100; i++)
    ntype[i] = 0;

  /* open input file */

  inData = open (argv[1], O_RDONLY, 0);
  if (inData == -1)
    {
      printf ("could not open\"%s\"; quit\n", argv[1]);
      exit (1);
    }
  else
    printf ("input file \"%s\" is open, inData=%i\n", argv[1], inData);

  while (1)
    {
      siz = read (inData, (char *) &gebheader, sizeof (GEBDATA));
      tsize += siz;
//      printf ("[1]*%i bytes, siz=%i\n", tsize,siz);

      if (siz != sizeof (GEBDATA))
        {
          printf ("\n");
          printf ("** done **\n");
          printf ("\n");
          printf ("read %i headers/payloads\n", nn);
          printf ("first/last TS %lli/%lli\n", firstTS, lastTS);
          printf ("covered %lli ticks\n", lastTS - firstTS);
          r1 = 100.0 * (float) nbadTS / (float) nn;
          printf ("found %i bad/negative DTS or  %9.3f%% out of %i\n", nbadTS, r1,nn);

          i1 = 0;
          for (i = 0; i < 100; i++)
            i1 += ntype[i];

          printf ("\n type statistics \n");

          for (i = 0; i < 100; i++)
            if (ntype[i] > 0)
              {
                r1 = 100.0 * (float) ntype[i] / (float) i1;
                GebTypeStr (i, str);
                printf ("type %3i, %s, %10i, %9.3f%%\n", i, str, ntype[i], r1);
              };
          printf ("\n");
          printf ("I believe there are %i single and coincidence events\n", nevents);
          printf ("I believe there are %i coincidence events\n", ncevents);
          printf (".. based on a time window of %lli\n", dtwin);
          printf ("\n");

          d1 = (double) (lastTS - firstTS);
          d1 /= 100000000;
          printf ("that is %.1f seconds or ", (float) d1);
          nsec = d1;
          i1 = (unsigned int) d1 / 3600;
          d1 -= i1 * 3600;
          i2 = (unsigned int) d1 / 60;
          d1 -= i2 * 60;
          printf ("%ih%im%is\n", i1, i2, (int) d1);
          printf ("^^^^^ any timestamp jumps will upset this accounting\n");
          printf ("\n");


          exit (0);
        };
      if (nn == 0)
        {
          firstTS = gebheader.timestamp;
          lastTS = gebheader.timestamp;
        };
      nn++;
      if (gebheader.type > 0 && gebheader.type < 100)
        ntype[gebheader.type]++;
      dTS = gebheader.timestamp - lastTS;
      if (dTS > dtwin)
        nevents++;
      else if (dTS <= dtwin)
        ncevents++;

      siz = read (inData, (char *) storage, gebheader.length);
      tsize += siz;
//      printf ("[2]*%i bytes, siz=%i\n", tsize,siz);

      if (nn < i1 && dTS >= 0)
        {
          printf (" %6i: ", nn);
          GebTypeStr (gebheader.type, str);
          printf ("%s ", str);
          printf ("type=%2i; ", gebheader.type);
          printf ("length=%4i; ", gebheader.length);
          printf ("timestamp=%14lli; ", gebheader.timestamp);
          printf ("dTS=%8lli ", dTS);
          printf ("%i bytes", tsize);
          printf ("\n");
        };
      if (dTS < 0)
        {
          printf ("*%6i: ", nn);
          printf ("type=%2i; ", gebheader.type);
          printf ("length=%4i; ", gebheader.length);
          printf ("timestamp=%14lli; ", gebheader.timestamp);
          printf ("dTS=%8lli ", dTS);
          printf ("%i bytes", tsize);
          printf ("\n");
          nbadTS++;
        };

      lastTS = gebheader.timestamp;
    };


  /* done */

  exit (0);

}
