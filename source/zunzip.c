
/* program to uncompress a file using zlib */
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
  char buf[BUFSIZE];
  gzFile zin;
  struct tms timesThen, timesNow;
  double totalTime, d1;
  float r1;

  /* open input file */

  in = open (argv[1], O_RDONLY);
  if (in <= 0)
    {
      printf ("could not open input file \"%s\"\n", argv[1]);
      exit (1);
    }
  else
    printf ("input file \"%s\" is open\n", argv[1]);
  zin = gzdopen (in, "r");

  /* check that there is no output file */

  out = open (argv[2], O_RDONLY);
  if (out != -1)
    {
      printf ("output file \"%s\" already exists, quit\n", argv[2]);
      exit (1);
    };

  /* open output file */

  out = open (argv[2], O_WRONLY | O_CREAT | O_TRUNC, PMODE);
  if (out <= 0)
    {
      printf ("could not open output file \"%s\"\n", argv[2]);
      exit (1);
    }
  else
    printf ("output file \"%s\" is open\n", argv[2]);

  /* start timer */

  times ((struct tms *) &timesThen);

  /* loop read/write */

  while (1)
    {

      osiz = gzread (zin, buf, BUFSIZE);
//      printf ("read buffer of size %i/%i\n", osiz, BUFSIZE);
      if (osiz <= 0)
        goto done;
      nbytes += osiz;

      osiz = write (out, buf, osiz);
//      printf ("wrote buffer of size %i/%i\n", osiz, BUFSIZE);


    };

  /* done */

done:


  gzclose (zin);
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

  exit (0);

}
