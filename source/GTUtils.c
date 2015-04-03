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

#include "GTMerge.h"

extern STAT nstat;
extern GTEVENT Event[MAXCOINEV];
extern int nBadTestPat;
extern CONTROL control;
extern DGSEVENT DGSEvent[MAXCOINEV];
extern int tlkup[NCHANNELS];
extern int tid[NCHANNELS];
#if(USEZLIB==0)
extern off_t inData[MAXFILES];
//extern int inData[MAXFILES];
#endif
#if(USEZLIB==1)
extern gzFile zFile[MAXFILES];
#endif

extern int Counter;
extern unsigned int TEMP[MAXLENINTS];

extern int idlim_lo[MAXTPE];
extern int idlim_hi[MAXTPE];

#if (USEBREAD)
extern int *bread_buf[MAXFILES];
extern int bread_pos[MAXFILES];
extern int bread_bufsiz[MAXFILES];
#endif

#define DEBUG2 0



/*----------------------------------------------------------------------------*/

int
#if (USEZLIB == 0 )
bread (int in, int *val, int *pos, int *buf, int *bufsiz)
#endif
#if (USEZLIB == 1 )
bread (gzFile in, int *val, int *pos, int *buf, int *bufsiz)
#endif
{

  /* buffered inter read */

  /* declarations */

  int siz;

  /* read new buffer */

  if (*pos == 0)
    {

      /* read in a buffer of data */

//      printf ("in = %i, bufsiz=%i\n", in,*bufsiz*sizeof(int));

#if (USEZLIB == 0 )
      siz = read (in, (char *) buf, *bufsiz * sizeof (int));
//      printf ("read regular buffer of size %i\n", siz);
#endif
#if (USEZLIB == 1 )
      siz = gzread (in, (char *) buf, *bufsiz * sizeof (int));
//      printf ("read zipped buffer of size %i\n", siz);
#endif

      *bufsiz = siz / sizeof (int);

      if (siz <= 0)
        return (-1);

    };

  /* return a value */

  *val = *(buf + (*pos));
//  printf("pos= %i, *(buf+(*pos))=%i, *val=%i\n", *pos,*(buf+(*pos)),*val );
  (*pos)++;

  /* force buffer read the next time? */

  if (*pos == *bufsiz)
    *pos = 0;

  /* done, report we got 4 bytes like 'read' would */

  return (sizeof (int));

};

/*----------------------------------------------------------------------------*/

int
GTGetDiskEv (int FileNo, int storeNo)
{

  /* returns: */
  /*   0: success */
  /*   1: header */
  /*   2: read trouble; end of file */
  /*   5: bad test pattern */

  /* declarations */

  static int nn = 0, mm = 0;
  int siz, i, i1, k;
  unsigned int j;
  unsigned int testPattern = 0, bufPos = 0;
  unsigned int PMET[MAXLENINTS];
  unsigned int t1, t2, t3, t4;
  float r1;

#if(0)
  printf ("entered GTGetDiskEv, mm=%i\n", mm);
  mm++;
  if (mm >= 3)
    {
      printf ("debug stop in GTGetDiskEv\n");
      exit (0);
    };
#endif

  /* normally we have one EOE marks */
  /* when we start reading......... */
  /* note: no need to swap bytes... */

  Counter = 0;
  while (testPattern != EOE)
    {


#if (USEBREAD==0)

      /* simple unbuffered reads */

      siz = read (inData[FileNo], (char *) &testPattern, 4);

#else

      /* use buffered reads */

#if (USEZLIB == 0 )
      siz = bread (inData[FileNo], &testPattern, &bread_pos[FileNo], bread_buf[FileNo], &bread_bufsiz[FileNo]);
#endif
#if (USEZLIB == 1 )
      siz = bread (zFile[FileNo], &testPattern, &bread_pos[FileNo], bread_buf[FileNo], &bread_bufsiz[FileNo]);
#endif

#endif

#if(0)
      if (nn < 100 || 1)
        {
          printf ("nn=%i, siz=%i, testPattern=%u\n", nn, siz, testPattern);
          nn++;
        };
#endif


      if (siz != 4)
        {
          printf ("GTGetDisk error: siz=%i, expected 4\n", siz);
          return (2);
        };

      nstat.inbytes += siz;
      if (Counter < MAXLENINTS)
        TEMP[Counter] = testPattern;
      bufPos += siz / 4;
      Counter++;                //Counter = Event[storeNo]Length + 1
//      printf("Counter=%i\n", Counter);
    };

  if (Counter == 1)
    {
      printf ("** File Header or Double Event[storeNo] Header **\n");
      return (1);
    }

#if(DEBUG1)
  printf ("GTGetDiskEv: event # %i from file %i (store in %i)\n", nn, FileNo, storeNo);
#endif

#ifdef LINUX                    //Swap bytes
  for (i = 0; i < Counter - 1; i++)
    if (i < MAXLENINTS)
      {

       /* before 4 3 2 1 */
       /*        | | | | */
       /* after  1 2 3 4 */

        t1 = (TEMP[i] & 0x000000ff) << 24;
        t2 = (TEMP[i] & 0x0000ff00) << 8;
        t3 = (TEMP[i] & 0x00ff0000) >> 8;
        t4 = (TEMP[i] & 0xff000000) >> 24;
        PMET[i] = t1 + t2 + t3 + t4;
      }
#endif


  /* store header */

  for (i = 0; i < HDRLENINTS; i++)
    Event[storeNo].hdr[i] = PMET[i];

#if(0)

  /* debug print header */

  for (i = 0; i < HDRLENINTS; i++)
    {
      printf ("Event[storeNo].hdr[%2i] = 0x%8.8x; ", i, Event[storeNo].hdr[i]);
      j = 0x80000000;
      printf ("|");
      for (k = 0; k <= 31; k++)
        {
          if ((Event[storeNo].hdr[i] & j) == j)
            printf ("1");
          else
            printf ("0");
          j = j / 2;
          if ((k + 1) % 4 == 0)
            printf ("|");
        };
      printf ("\n");

    }
#endif


  /* extract the trace */

  Event[storeNo].traceLen = 0;
  for (i = HDRLENINTS; i < (Counter - 1); i++)
    if (i < MAXLENINTS)
      {
        i1 = 2 * (i - HDRLENINTS);
//        printf("i1=%i\n", i1);
        Event[storeNo].trace[i1] = (unsigned short int) (PMET[i] & 0x3fff);
        Event[storeNo].trace[i1 + 1] = (unsigned short int) ((PMET[i] >>= 16) & 0x3fff);
        Event[storeNo].traceLen += 2;
        assert (Event[storeNo].traceLen < MAXTRACELEN);
      }

#if(0)

  /* debug print trace */

  printf ("tracLen=%i, trace>\n", Event[storeNo].traceLen);
  for (i = 0; i < Event[storeNo].traceLen; i++)
    printf ("%3i: %i\n", i, Event[storeNo].trace[i]);
#endif


#if(0)
//  if (NprintEvNo <= NumToPrint)
  printf ("Event[storeNo].traceLen=%i, Event[storeNo].len=%i, Counter=%i\n", Event[storeNo].traceLen,
          Event[storeNo].len, Counter);
#endif
  Event[storeNo].pos = bufPos;

  /* print trace */

#if(0)
//  if (NprintEvNo <= NumToPrint)
  for (i = 0; i < Event[storeNo].traceLen; i++)
    printf ("Event[storeNo].trace[%4i]=0x%4.4x, %6i\n", i, Event[storeNo].trace[i], Event[storeNo].trace[i]);
#endif

  /* done */

  control.fileEventsRead[FileNo]++;
  nn++;
  control.filesiz[FileNo] += Counter * sizeof (int);
//if(1)exit(0);
//  printf("GTGetDiskEv returns 0\n");
  return (0);

}

/*----------------------------------------------------------------------------*/

int
GTEvdecompose (GTEVENT * Event, DGSEVENT * DGSEvent)
{

  /* declarations */

  unsigned int ui1, ui2, k;
  unsigned long long int ulli1, ulli2, i, j;
  static long long int ts1=0,ts2=0;
  static int nn=0;

  /* extract IDs */

//  DGSEvent->chan_id = (Event->hdr[0] & 0x0000001f);
  DGSEvent->chan_id = (Event->hdr[0] & 0x0000000f);
//  printf("0x%8.8x\n", (Event->hdr[0]&0xf8000000));
//  printf("0x%8.8x\n", (Event->hdr[0]&0xf8000000) >> 27);
//  DGSEvent->board_id = ((Event->hdr[0] & 0xf8000000) >> 27);

  DGSEvent->board_id= ((Event->hdr[0]>>= 4)&0xfff);
  
//DGSEvent->board_id= (Event->hdr[0]>> 4);

DGSEvent->id = DGSEvent->board_id * 10 + DGSEvent->chan_id;

  /* store the type and type ID */

  DGSEvent->tpe = tlkup[DGSEvent->id];
  DGSEvent->tid = tid[DGSEvent->id];

#if(0)
  printf ("DGSEvent->chan_id=%i; ", DGSEvent->chan_id);
  printf ("board_id=%i; ", DGSEvent->board_id);
  printf ("id =%i; ", DGSEvent->id);
  printf ("tpe=%i; ", DGSEvent->tpe);
  printf ("tid=%i\n", DGSEvent->tid);
#endif

  /* check the IDs */

  if (DGSEvent->tpe < 0 || DGSEvent->tpe >= MAXTPE)
    {
    printf("return 3\n");
    return (3);
    };
  if (DGSEvent->tid < idlim_lo[DGSEvent->tpe])
    {
    printf("return 4\n");
    return (4);
    };
  if (DGSEvent->tid > idlim_hi[DGSEvent->tpe])
    {
    printf("return 5, DGSEvent->tid=%i > idlim_hi[DGSEvent->tpe]=%i\n",DGSEvent->tid,idlim_hi[DGSEvent->tpe]);
  printf ("DGSEvent->chan_id=%i; ", DGSEvent->chan_id);
  printf ("board_id=%i; ", DGSEvent->board_id);
  printf ("id =%i; ", DGSEvent->id);
  printf ("tpe=%i; ", DGSEvent->tpe);
  printf ("tid=%i\n", DGSEvent->tid);
//if(1)exit(0);
    return (5);
    };
//  printf("passed checks\n");

  /* extract the energy */

  Event->PreE = Event->hdr[7] & 0x00ffffff;
//  printf("Event->PreE=0x%8.8x\n", Event->PreE);
  ui1 = Event->hdr[7] & 0xff000000;
  ui1 >>= 24;
//  printf("ui1=0x%8.8x\n",  ui1);
  ui2 = Event->hdr[8] & 0x0000ffff;
  ui2 <<= 8;
//  printf("ui2=0x%8.8x\n",  ui2);
//  printf("ui2+ui1=0x%8.8x\n",  ui1+ui2);
  Event->PostE = ui1 + ui2;
//  printf("Event->PostE=0x%8.8x\n", Event->PostE);

/*
  Event->rawE = Event->PostE - Event->PreE;
  DGSEvent->ehi = Event->rawE / 800;
*/

  Event->rawE = Event->PostE - Event->PreE;
  DGSEvent->ehi = Event->rawE / 100;  // ehi COMPRESSION!!!
  if (DGSEvent->ehi < 0)
    DGSEvent->ehi = -DGSEvent->ehi;


#if(0)
  printf ("DGSEvent->ehi=%i\n", DGSEvent->ehi);
  fflush (stdout);
#endif
//if(1)exit(0);

  /* extract the LED time stamp */

  DGSEvent->LEDts=0;
//  printf("Event->hdr[1]=0x%u; ", Event->hdr[1]);
  DGSEvent->LEDts = (unsigned long long int) Event->hdr[1];
  ulli1 = (unsigned long long int)(Event->hdr[2] & 0x0000ffff);
//  printf("ulli1=0x%llx; ", ulli1);
  ulli1 = (ulli1<<32);
//  printf("ulli1=0x%llx shifted\n", ulli1);
  DGSEvent->LEDts += ulli1;
#if(0)
      j = 0x8000000000000000;
      printf ("DGSEvent->LEDts  |");
      for (k = 0; k <= 63; k++)
        {
          if ((DGSEvent->LEDts & j) == j)
            printf ("1");
          else
            printf ("0");
          j = j / 2;
          if ((k + 1) % 4 == 0)
            printf ("|");
        };
      printf (" = %20lli, 0x%x", DGSEvent->LEDts, DGSEvent->LEDts);
      printf ("\n");
#endif

  /* extract the PEAK time stamp */
  
  DGSEvent->PEAKts = 0;
  ulli1 = (unsigned long long int)(Event->hdr[8]& 0xffff0000);
  ulli1 >>= 16;
#if(0)
      j = 0x8000000000000000;
      printf ("ulli1            |");
      for (k = 0; k <= 63; k++)
        {
          if ((ulli1 & j) == j)
            printf ("1");
          else
            printf ("0");
          j = j / 2;
          if ((k + 1) % 4 == 0)
            printf ("|");
        };
      printf ("\n");
#endif
  ulli2 = (unsigned long long int)Event->hdr[9];
  ulli2 <<= 16;

#if(0)
      j = 0x8000000000000000;
      printf ("ulli2            |");
      for (k = 0; k <= 63; k++)
        {
          if ((ulli2 & j) == j)
            printf ("1");
          else
            printf ("0");
          j = j / 2;
          if ((k + 1) % 4 == 0)
            printf ("|");
        };
      printf ("\n");
#endif

  DGSEvent->PEAKts = ulli1+ulli2;

#if(0)
      j = 0x8000000000000000;
      printf ("DGSEvent->PEAKts |");
      for (k = 0; k <= 63; k++)
        {
          if ((DGSEvent->PEAKts & j) == j)
            printf ("1");
          else
            printf ("0");
          j = j / 2;
          if ((k + 1) % 4 == 0)
            printf ("|");
        };
      printf ("\n");
#endif

  /* extract the CFD time stamp */

  DGSEvent->CFDts=0;
  ulli1 = (unsigned long long int)(Event->hdr[3] & 0xffff0000);
  ulli1 <<= 16;
  ulli2 = (unsigned long long int)Event->hdr[4];
  DGSEvent->CFDts = ulli1 + ulli2;

#if(0)
  printf ("DGSEvent->LEDts =%15lli; ", DGSEvent->LEDts);
  printf ("DT=%15lli;  ",DGSEvent->LEDts-ts1);
//  assert (DGSEvent->LEDts>=ts1);
  ts1=DGSEvent->LEDts;
  printf ("DGSEvent->PEAKts=%15lli; ", DGSEvent->PEAKts);
  printf ("DT=%15lli;",DGSEvent->PEAKts-ts2);
//  assert (DGSEvent->PEAKts>=ts2);
  ts2=DGSEvent->PEAKts;

//  printf ("DGSEvent->CFDts= %15lli; ", DGSEvent->CFDts);
  printf ("\n");
  nn++;
//  if (nn>300) exit(0);
#endif

#if(0)

  /* debug print trace */

  printf ("tracLen=%i, trace>\n", Event->traceLen);
  for (i = 0; i < Event->traceLen; i++)
    printf ("%3i: %i\n", i, Event->trace[i]);
#endif

 


  DGSEvent->traceLen = control.maxtracelen;
  if (Event->traceLen < control.maxtracelen)
    DGSEvent->traceLen = Event->traceLen;
  for (j = 0; j < DGSEvent->traceLen; j++)
    DGSEvent->trace[j] = Event->trace[j];  


//  if(1)exit(0);
//  printf("GTEvdecompose returns 0\n");
  return (0);

}
