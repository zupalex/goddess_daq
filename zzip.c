
/* program to compress a file using zlib */
/* directly. format will be different from */
/* gzip. program works on one file only */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <zlib.h>
#include <sys/times.h>

#define         PMODE 0644
#define BUFSIZE 163840

#include <string.h>
#define CLK_TCK ((__clock_t) __sysconf (2))

int
main (int argc, char **argv)
{

  /* declarations */

  int isiz, osiz;
  int in, out;
  long long int nbytes = 0;
  char buf[BUFSIZE], ofn[512], str[512];
  gzFile zout;
  struct tms timesThen, timesNow;
  double totalTime, d1;
  float r1;

  /* help */

  if (argc <= 1 || argc > 3)
    {
      printf ("use: %s infile outfile\n", argv[0]);
      printf ("use: %s infile\n", argv[0]);
      printf ("__if the output file is not specified, then the output \n");
      printf ("  file will be the input file name with a .z extension\n");
      exit (0);
    };


  /* open input file */

  in = open (argv[1], O_RDONLY);
  if (in <= 0)
    {
      printf ("could not open input file \"%s\"\n", argv[1]);
      exit (1);
    }
  else
    printf ("input file \"%s\" is open\n", argv[1]);

  if (argc == 2)
    sprintf (ofn, "%s.z", argv[1]);
  else
    strcpy (ofn, argv[2]);
  printf ("output file name is %s\n", ofn);

  /* check that there is no output file */

  out = open (ofn, O_RDONLY);
  if (out != -1)
    {
      printf ("output file \"%s\" already exists, quit\n", ofn);
      exit (1);
    };

  /* open output file */

  out = open (ofn, O_WRONLY | O_CREAT | O_TRUNC, PMODE);
  if (out <= 0)
    {
      printf ("could not open output file \"%s\"\n", ofn);
      exit (1);
    }
  else
    printf ("output file \"%s\" is open\n", ofn);
  zout = gzdopen (out, "w");

  /* start timer */

  times ((struct tms *) &timesThen);

  /* loop read/write */

  while (1)
    {

      isiz = read (in, buf, BUFSIZE);
//      printf ("read buffer of size %i/%i\n", isiz, BUFSIZE);
      if (isiz <= 0)
        goto done;
      nbytes += isiz;
      osiz = gzwrite (zout, buf, isiz);
//      printf ("wrote compressed buffer of size %i/%i\n", osiz, BUFSIZE);

    };

  /* done */

done:

  /* this will flush */
  gzclose (zout);

  close (in);
  close (out);

  printf ("read %lli bytes\n", nbytes);
  times ((struct tms *) &timesNow);
  d1 = timesNow.tms_utime - timesThen.tms_utime;
  totalTime = d1;
  printf ("CPU time: user %9.3f sec, ", d1 / CLK_TCK);
  r1 = timesNow.tms_stime - timesThen.tms_stime;
  totalTime += d1;
  printf ("system  %9.3f sec, ", d1 / CLK_TCK);
  totalTime /= CLK_TCK;
  printf ("Total  %9.3f sec\n", totalTime);

  d1 = nbytes / totalTime / 1024 / 1000;
  printf ("processed %9.3f MBytes/s\n", (float) d1);

  sprintf (str, "file %s; ls -l %s", argv[1], argv[1]);
  system (str);
  sprintf (str, "file %s; ls -l %s", ofn, ofn);
  system (str);

  exit (0);

}
