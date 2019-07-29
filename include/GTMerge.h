#ifndef GTMERGE_H
#define GTMERGE_H

#include <vector>
#include "GoddessToolbox.h"

#define PMODE 0644
#define LENSP 16384

#define USEBREAD 0
#define BREAD_BUFSIZE 16384
#define USEZLIB 0

#define MAXFILES 500
#define MAXCOINEV 500
#define DEBUG1 0
#define NCHANNELS 1000*11
#define MAXVALIDHIE 32000
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

#define NGE 110

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
#define SSD 9
#define CLOVER 10
#define SPARE 11

#define MAXTPE 12
#define MAXTID 1000

#define MAXBIGBUFSIZ 50000

#define STRLEN 256

//**** TO CHECK *************//

#define MAXDATASIZE 20000

#define SUMS_FILE   "precalc.dat"       /* file containing precalculated sums */

#define GRID_PTS   228254  /* CHANGEME number of grid points in the basis */
#define GRID_SEGS  37      /* number of signals calculated for each basis point */
#define MEAS_SEGS  37      /* number of signals measured for each event */
#define TIME_STEPS 50      /* number of time steps calculated/measured for each segment */
#define TOT_SEGS   36      /* total number of segments in crystal */
#define MAX_AGS    1000  /* CHANGEME max. no. of points in coarse grid for AGS */

#define MAX_SEGS   8              /* max. number of segments to take in events */
#define MAX_PARS   (8*MAX_SEGS+1) /* max. number of parameters to fit */
#define MAX_INTPTS (2*MAX_SEGS)   /* max. number of interaction points */
#define SSEG       TOT_SEGS       /* range of seg in basis grid */
#define SRAD       34             /* range of r in basis grid */
#define SPHI       13             /* range of phi in basis grid */
#define SZZZ       22             /* range of z in basis grid */

#define COAL_DIST_DEFAULT 2.0

#define GEB_TYPE_DECOMP         1
#define GEB_TYPE_RAW            2
#define GEB_TYPE_TRACK          3
#define GEB_TYPE_BGS            4
#define GEB_TYPE_S800_RAW       5
#define GEB_TYPE_NSCLnonevent   6
#define GEB_TYPE_GT_SCALER      7
#define GEB_TYPE_GT_MOD29       8
#define GEB_TYPE_S800PHYSDATA   9
#define GEB_TYPE_NSCLNONEVTS   10
#define GEB_TYPE_G4SIM         11
#define GEB_TYPE_CHICO         12
#define GEB_TYPE_DGS           14
#define GEB_TYPE_DGSTRIG       15
#define GEB_TYPE_DFMA          16
#define GEB_TYPE_PHOSWICH      17
#define GEB_TYPE_PHOSWICHAUX   18
#define GEB_TYPE_AGOD          19

#define MAX_GEB_TYPE           20
#define MAXINPUTS		16

// Binary data format

struct GebData
{
		int type; /* type of data following */
		int length;
		unsigned long long timestamp;
};


/*---------------*/
/* single events */
/*---------------*/

struct AGODEVENT
{
		std::vector<unsigned short> channels;
		std::vector<unsigned short> values;
		unsigned long long timestamp;
};

struct GRETHEADER
{
    int type;
    int crystal_id;
    int num; //number of interaction points from decomp
    float tot_e;
    int core_e[4];
    long long int timestamp;
    long long trig_time;
    float t0;
    float cfd;
    float chisq;
    float norm_chisq;
    float baseline;
    float prestep;
    float poststep;
    int pad; //non0 gives error type
    struct
    {
        float x;
        float y;
        float z;
        float e;
        int seg;
        float seg_ener;
    } inputs[MAXINPUTS];
};

struct GRETEVENT
{
		long long unsigned int timestamp;
		int quad;
		int crystal;
		float theta;
		float phi;
		float e;
		float x;
		float y;
		float z;
		float raw_e;
};

struct GRETHIT
{
    float x;
    float y;
    float z;
    float r;
    float theta;
    float phi;
    float e;
    int seg;
    float seg_ener;
    float raw_e;
    int quad;
    int crystal;
    int timestamp;
};

struct DGSEVENT
{
		float ehi;
		short int id;
		//short int               id_pw[MAXPW];
		unsigned short int tpe, tid;
		unsigned short int flag;
		unsigned short int board_id;
		unsigned short int chan_id;
		unsigned short int geo_addr;
		unsigned short int packet_length;

		unsigned short int header_type;
		unsigned short int event_type;
		unsigned short int header_length;

		unsigned long long int event_timestamp;
		unsigned long long int last_disc_timestamp;
		unsigned long long int peak_timestamp;

		unsigned short int timestamp_match_flag;
		unsigned short int external_disc_flag;
		unsigned short int cfd_valid_flag;
		unsigned short int pileup_only_flag;
		unsigned short int offset_flag;
		unsigned short int sync_error_flag;
		unsigned short int general_error_flag;

		unsigned short int peak_valid_flag;
		unsigned short int pileup_flag;

		int sampled_baseline;
		int cfd_sample_0;
		int cfd_sample_1;
		int cfd_sample_2;
		int pre_rise_energy;
		int post_rise_energy;

		unsigned short int m1_begin_sample;
		unsigned short int m1_end_sample;
		unsigned short int m2_begin_sample;
		unsigned short int m2_end_sample;
		unsigned short int peak_sample;
		unsigned short int base_sample;

		int baseline;

		unsigned short int traceLen;
		short int trace[MAXTRACELEN];

		//unsigned short int      A[MAXPW];
		//unsigned short int      B[MAXPW];
		//unsigned short int      C[MAXPW];
		//unsigned short int      T[MAXPW];

		unsigned long long int LEDts;
		unsigned long long int CFDts;
		//unsigned long long int  PEAKts;
		//char                    flag;
		//short int               baseline;
		//unsigned short int      traceLen;
		//short int               trace[MAXTRACELEN];
};

struct DFMAEVENT
{
		int ehi;    // WAS SHORT INT
		short int id;
		unsigned short int tpe, tid;
		unsigned short int board_id;
		unsigned short int chan_id;
		unsigned long long int LEDts;
		unsigned long long int CFDts;
		unsigned long long int PEAKts;
		char flag;
		char pu;
		int d2t0;
		int d2t1;
		int d2t2;
		unsigned long long int prevTS;
		int baseline;
		int postrisebeg;
		int prerisebeg;
		int postriseend;
		int preriseend;
		int peaksample;
		int basesample;
		int prerisesum;
		unsigned short int traceLen;
		short int trace[MAXTRACELEN];
};

/*--------------------*/
/* coincidence events */
/*--------------------*/

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
		unsigned long long int startTS_lo;
		unsigned long long int startTS_hi;
		int startTS;
		int TSlist_lo;
		int TSlist_hi;
} CONTROL;

typedef struct stat_struct
{
		long long int inbytes;
		long long int badid;
		long long int outbytes;
		unsigned int ge_hit[NGE + 1];
		unsigned int ge_cs[NGE + 1];
		int nTSjumprecover_f[MAXTID];
		int nTSjumprecover_b[MAXTID];
		unsigned int in_hit[MAXTPE][MAXTID];
		unsigned int out_hit[MAXTPE][MAXTID];
		int nbigbufreads;
		int nswaps;
		unsigned int id_hit[NCHANNELS];
} MSTAT;

/*-----------------*/
/* header for file */
/*-----------------*/

typedef struct DGSHEADER_struct
{
		unsigned int id;
		char date[19];
		unsigned int nfiles;
		char dummy;
} DGSHEADER;

#endif
