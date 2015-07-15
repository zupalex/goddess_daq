
#define PMODE 0644
#define LENSP 16384

#define USEBREAD 0
#define BREAD_BUFSIZE 16384
#define USEZLIB 0

#define MAXFILES 200
#define MAXCOINEV 500
#define DEBUG1 0
#define NCHANNELS 1000*11
#define MAXVALIDHIE 32000
#define NGE 110
#define MAXBOARDID 100000

#define TRUE 1
#define FALSE 0
#define NOTDEF -1


#define EOE          0xaaaaaaaa
#define HDRLENBYTES  28
//#define HDRLENINTS    7
#define HDRLENINTS  13  
#define HDRLENWORDS  2*HDRLENINTS
#define MAXLENINTS  519

#define LENEOVWORDS  2

#define MAXTRACELEN 8192
#define EOE          0xaaaaaaaa

#define CC_ID1 10      /* instead of 29 as default */
#define CC_ID2 11      /* instead of 39 as default */

/* instrument types */

#define NOTHING 0
#define GE 1
#define BGO 2
#define SIDE 3
#define AUX 4
#define DSSD 5
#define FP 6
#define XARRAY 7
#define CHICO2 8

#define MAXTPE 9
#define MAXTID 1000

/*---------------*/
/* single events */
/*---------------*/

typedef struct DGSEVENT
{
  short int               ehi;
  short int               id;
  unsigned short int      tpe, tid;
  unsigned short int      board_id;
  unsigned short int      chan_id;
  unsigned long long int  LEDts;
  unsigned long long int  CFDts;
  unsigned long long int  PEAKts;
  char                    flag;  
  short int               baseline;
  unsigned short int      traceLen;
  short int               trace[MAXTRACELEN];
}  DGSEVENT;

#define DGSEVENT_BASELEN sizeof(DGSEVENT)-MAXTRACELEN*sizeof(short int)

typedef struct GTEVENT
{
  /* raw */

  unsigned short int      len;
  short int               ehi;
  short int               id;
  short int               module;
  unsigned short int      tpe, tid;
  unsigned short int      board_id;
  unsigned short int      chan_id;
  unsigned long long int  LEDts;
  unsigned long long int  CFDts;
  unsigned long long int  PEAKts;
  char                    flag;  
  short int               baseline;
  int                     rawE;
  unsigned int            hdr[HDRLENINTS];
  unsigned short int      traceLen;
  short int               trace[MAXTRACELEN];
}  GTEVENT;

/*--------------------*/
/* coincidence events */
/*--------------------*/

typedef struct COINEV_struct
  {
  unsigned short int      len;
  unsigned char      lenclean;
  unsigned char      lendirty;
  unsigned char      lenbgo;
  unsigned char      lenaux;
  GTEVENT  GTEvent[MAXCOINEV];
  DGSEVENT DGSEvent[MAXCOINEV];
  } COINEV;


typedef struct CONTROL_struct
  {
  int nOpenFiles;
  int fileActive[MAXFILES];
  int fileEventsRead[MAXFILES];
  int filesiz[MAXFILES];
  int nwritten;
  int nwritten_chunk;
  int chunkno;
  int chunksiz;
  int nread;
  int nzeroehi;
  int minGE;
  int minFP;
  int minDSSD;
  int minmult;
  int TSlistelen;
  int noverflowehi;
  int CurEvNo;
  long long int dts_min;
  long long int dts_max;
  int suppressBadAtRead;
  int zzipout;
  int dtsfabort;
  int dtsbabort;
  long long int startTS_lo;
  long long int startTS_hi;
  int startTS;
  int TSlist_lo;
  int TSlist_hi;
  } CONTROL;


typedef struct stat_struct
  {
  long long int inbytes;
  long long int badid;
  long long int outbytes;
  unsigned int ge_hit[NGE+1];
  unsigned int ge_cs[NGE+1];
  int nTSjumprecover_f[MAXTID];
  int nTSjumprecover_b[MAXTID];
  unsigned int in_hit[MAXTPE][MAXTID];
  unsigned int out_hit[MAXTPE][MAXTID];
  long long int nbigbufreads;
  int nswaps;
  unsigned int id_hit[NCHANNELS];
  unsigned int GEBIds[30];
  long long int GEBlen[30];
  } MSTAT;


/*-----------------*/
/* header for file */
/*-----------------*/

typedef struct DGSHEADER_struct
  {
  unsigned int      id;
  char              date[19];
  unsigned int      nfiles;
  char              dummy;
  } DGSHEADER;




