#ifndef GEBSORT_H
#define GEBSORT_H

#include "TFile.h"
#include "gdecomp.h"
#include "veto_pos.h"
#include <string>


#define TAPE 0
#define NET  1
#define DISK 2
#define GEB  3

#define MEDIUMLEN 2048
#define LONGLEN 8000
#define SHORTLEN 1024
#define RBUFSIZE 500000
#define MAXPAYLOADSIZE 10000
#define STRLEN 256
#define MAXDETPOS 120
#define MAXCRYSTALNO 3
#define RATELEN 16384
#define DTBTEVLEN 1000
#define MAXNOSEG 9
#define MAXDETNO 200
#define NUMSEGS 36
#define MAXSEGNO MAXDETNO*NUMSEGS
#define RMIN 10
#define RMAX 35
#define MAXMODNO 60
#define MAXGTMODNO 30
#define NUMAGATAPOS 180

#define MAXGEBS 200

#define MAX_GAMMA_RAYS 40
#define GEB_BITS GEB_HEADER_BYTES*8

/* max values for # of bits */

#define M14BITS 0x3fff
#define M13BITS 0x1fffGEBSort.h
#define M12BITS 0x0fff
#define M11BITS 0x07ffGEBSort.h
#define M10BITS 0x03ff

/* basic spectra lengths */

#define L14BITS  M14BITS+1
#define L13BITS  M13BITS+1
#define L12BITS  M12BITS+1
#define L11BITS  M11BITS+1
#define L10BITS  M10BITS+1

typedef struct GEB_event {
    int mult;
    CRYS_INTPTS* ptinp[MAXGEBS];
    GEBDATA* ptgd[MAXGEBS];
} GEB_EVENT;

typedef struct PARS {
    char ConfigFile[STRLEN];
    short noCalib;
    bool noMapping;
    bool noHists;
    unsigned short ignoreThresholds;
    char ROOTFile[STRLEN];
    unsigned long long int nEvents;
    char ROOTFileOption[STRLEN];
    char GTSortInputFile[STRLEN];
    int UseShareMemFile;
    unsigned int StartMapAddress;
    char ShareMemFile[STRLEN];
    int InputSrc;
    int HaveRootFileName;
    int WeWereSignalled;
    int UseRootFile, SizeShareMemFile;
    int UpdateRootFile;
    char spname[STRLEN];
    unsigned int firstEvent;
    int GSudpPort;
    unsigned int NumToPrint;
    int DumpEvery;
    TFile* f1;
    TDirectory* histDir;
    TDirectory* treeDir;
    TList* wlist;
    unsigned long long int curTS;
    long long int dTS;
    long long int nbytes;
    unsigned int CurEvNo;
    char pHost[16];
    int grouping;
    int type;
    int enabled[MAXDETNO + 1];
    float CCcal_gain[MAXDETNO + 1];
    float CCcal_offset[MAXDETNO + 1];
    float SEGcal_gain[MAXDETPOS + 1][MAXCRYSTALNO + 1];
    float SEGcal_offset[MAXDETPOS + 1][MAXCRYSTALNO + 1];
    float timeout;
    float crmat[MAXDETPOS + 1][MAXCRYSTALNO + 1][4][4];
    float detpolang[MAXDETPOS + 1];
    float beta;
    int GGMAX;
    int modwrite;
    unsigned int tsnumwrites;
    float fomlo[MAXNOSEG];
    float fomhi[MAXNOSEG];
    int ndetlimlo;
    int ndetlimhi;
    float beamdir[3];
    int nocrystaltoworldrot;
    int multlo;
    int multhi;
    int requiretracked;
    float  modCCang[MAXMODNO + 1][4];
    float  modCCdopfac[MAXDETNO];
    int AGATA_data;
    double TrX[NUMAGATAPOS], TrY[NUMAGATAPOS], TrZ[NUMAGATAPOS];
    double rotxx[NUMAGATAPOS], rotxy[NUMAGATAPOS], rotxz[NUMAGATAPOS];
    double rotyx[NUMAGATAPOS], rotyy[NUMAGATAPOS], rotyz[NUMAGATAPOS];
    double rotzx[NUMAGATAPOS], rotzy[NUMAGATAPOS], rotzz[NUMAGATAPOS];
    int numgggates;
    int gg_gate_pos[100];
    int gg_gate_width[100];
    int vetoSpots;
    char vetoSpotsfn[128];
    float vetocutfac[VETO_NZ];
    float vetoecut;
    float target_x, target_y, target_z;
    float maxsnglintrE;
    float maxsnglintrEFOM;
    char AGATA_data_fn[512];
} PARS;


/* structure for the tracked gamma rays */
/* written to the output with geb ID GEB_TYPE_TRACK==3 */

typedef struct TRACKED_GAMMA_RAY {
    float esum;              /* gamma ray energy */
    int ndet;                /* number of interactions */
    float fom;               /* figure of merit */
    int   tracked;           /* 1==if tracked */
    long long int timestamp; /* timestap of first interaction point */
    float x0, y0, z0, e0;    /* first interaction point */
    float x1, y1, z1, e1;    /* second interaction point */
} TRACKED_GAMMA_RAY;

typedef struct TRACKED_GAMMA_HIT {
    int ngam;
    int pad;
    TRACKED_GAMMA_RAY gr[MAX_GAMMA_RAYS];
} TRACKED_GAMMA_HIT;


/* macros */

#define WRITEALLHISTS  \
    gDirectory->cd("root:/"); \
    wlist = gDirectory->GetList(); \
    if(!Pars.UpdateRootFile) f1= new TFile(Pars.RootFile, Pars.ROOTFileOption); \
    if (ComPressLevel>NOTDEF) f1->SetCompressionLevel(ComPressLevel); \
    printf("writing all spectra to [%s]\n", Pars.RootFile); \
    printf("be patient... "); \
    fflush(stdout); \
    t1 = time(NULL); \
    wlist->Write(0,TObject::kOverwrite); \
    t2 = time(NULL); \
    printf("DONE! on "); \
    time_stamp(); \
    printf("file size: %i, ",f1->GetSize()); \
    printf("compression level: %i, ",f1->GetCompressionLevel()); \
    printf("and factor: %f\n",f1->GetCompressionFactor()); \
    printf("uncompressed root file size: %f\n",f1->GetSize()*f1->GetCompressionFactor()); \
    printf("writeout time: %i seconds\n", t2 - t1); \
    printf("at %7.2f Mbytes/sec\n", (float) f1->GetSize() / (t2 - t1) / 1000000.0); \
    printf("on "); \
    time_stamp(); \
    fflush(stdout);

#define UPDSSHMEM \
    t1 = time(NULL); \
    mfile->Update(); \
    t2 = time(NULL); \
    printf("done! "); \
    printf("shared memory size: %i\n", mfile->GetSize()); \
    printf("update time: %li seconds ", t2 - t1); \
    printf("at %7.2f Mbytes/sec\n", (float) mfile->GetSize() / (t2 - t1) / 1000000.0); \
    printf("to mapfile [%s] on ",Pars.ShareMemFile); \
    time_stamp(); \
    fflush(stdout);


/* prototypes */

//  TH2F *mkTH2F (char *, char *, int , double , double , int , double , double );
//  TH1D *mkTH1D (char *, char *, int , double , double );

#endif
