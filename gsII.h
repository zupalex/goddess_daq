/* $Id: gsII.h,v 1.1 2013/04/10 16:34:45 tl Exp $ */

#define  NBGO   15       /* no bgo detectors per ge +1     */
#define  NGE    150      /* no ge detectors                */
#define  MAXID  NGE-1    /* max id                         */
#define  REALDATAPOS 0  /* pos in buffer where real data starts */

/* max values for # of bits */

#define M14BITS 0x3fff 
#define M13BITS 0x1fff
#define M12BITS 0x0fff
#define M11BITS 0x07ff
#define M10BITS 0x03ff

/* basic spectra lengths */

#define L14BITS  M14BITS+1
#define L13BITS  M13BITS+1
#define L12BITS  M12BITS+1
#define L11BITS  M11BITS+1
#define L10BITS  M10BITS+1

#define DATASIZE 16384
#define WORDSIZE DATASIZE/2
#define NETSIZE 16390
#define MAXVALIDHIE 16127
#define TAPEHEADER 112
#define FILEHEADER  90
#define EOFSIZE      0
#define ERRORSIZE   -1
#define FILEMARK 3

#define CLEANGE 0
#define DIRTYGE 1
#define CLEANBGO 2

#define NERR 50

#define SUCCESS         0
#define GOODEV          0
#define GSIIERRNEWBUF   1
#define GSIIERRFILEHEAD 2
#define GSIIGETNEWBUFF  3
#define GSIITAPEHEAD    4
#define GSIIMULTOOLOW   5
#define GSIIERRSTART    6
#define GSIIERRSPACE    7
#define GSIIERRMAXSANE  8
#define GSIIERRBUFLEFT  9
#define GSIIERRBLOCK   10
#define GSIIERREBSIZE  11
#define GSIIREJECTEV   12
#define GSIIERRCONT    32

#define FERAERRNOHEAD  30
#define FERAERRNEWHEAD 31
#define FERAERRBADVSN  33
#define FERAERRWDS     34

#define SPLEN   8192
#define MAXSANE 60		/* max multiplicity before error return */
#define MAX_MATS 10
#define MAX_SEG  7
#define NBADMAX  20
#define M12 0x0fff


#define MAXFERATYPE 64   /* how many different types of FERAs? */
#define FERATYPEUNDEF 63 /* types are 0-63, default to max  */
#define FERATYPE0 0
#define FERATYPE1 1
#define FERATYPE2 2
#define FERATYPE3 3
#define FERATYPE4 4
#define FERATYPE5 5
#define FERATYPE6 6
#define FERATYPE7 7
#define FERATYPE8 8

#define  MAXVSN 512
#define  MAXFERACH 16

#define TAPE_REOPEN 10000000
#ifdef SUNOS
#define TAPE_REOPEN 10000
#endif

struct EVENT
{
  unsigned short int id;	/* detector id */
  unsigned short int gebit;	/* ge hit bit */
  unsigned short int bgohit;	/* bgo hitpattern */
  unsigned short int ehi;	/* high resolution ge energy */
  unsigned short int pu;	/* pileup bit */
  unsigned short int over;	/* overrange bit */
  unsigned short int tge;	/* germanium time */
  unsigned short int tc;	/* trap corrector */
  unsigned short int elo;	/* low resolution ge energy */
  unsigned short int eside;	/* side chan. energy for seg ge */
  unsigned short int tbgo;	/* bgo time */
  unsigned short int ebgo;	/* bgo energy */
  unsigned short int hs;	/* honeycomb suppress flag */
};

struct EVHDR
{

  unsigned short int len;	/* total length of event */
  unsigned short int len_clean;	/* no of clean ge */
  unsigned short int len_dirty;	/* no of dirty ge */
  unsigned short int len_bgo;	/* no of bgo-only */
  unsigned short int ttH;	/* usec-time high bits  */
  unsigned short int ttM;	/* usec-time medium bits */
  unsigned short int ttL;	/* usec-time low bits   */
  unsigned short int tac1;	/* tac1 */
  unsigned short int tac2;	/* tac2 (RF tac) */
  unsigned short int sumge;	/* ge summed energy */
  unsigned short int sumbgo;	/* bgo summed energy */
  unsigned short int mtm;	/* mtm word */
};
