#ifndef GEBSORT_H
#define GEBSORT_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TDirectory.h"

#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TObjArray.h"
#include "TObject.h"
#include "TKey.h"
#include "TSystem.h"
#include "TCutG.h"
#include "TTree.h"

#include "TMapFile.h"

#include "GTMerge.h"

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

#define STRLEN 256
#define MAXDETNO 200
#define MAXDETPOS 120
#define MAXCRYSTALNO 3
#define MAXNOSEG 9
#define NUMAGATAPOS 180
#define MAXMODNO 60

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

#define MAX_GEB_TYPE 20

using std::string;

class GoddessData; // forward declaration

class PARS
{
private:

public:
    PARS() {}
    virtual ~PARS() {}

    char ConfigFile[STRLEN];
    char GeomFile[STRLEN];

    std::string sx3EnAdjustFile;
    std::string qqq5EnAdjustFile;

    short noCalib;
    bool noMapping;
    bool noHists;
    bool noDoppler;
    unsigned short ignoreThresholds;
    unsigned short siDetailLvl;

    std::string userFilter;
    std::string triggerMode;
    std::ofstream cleanedMerged;

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
    TTree* mainTree;
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
    float target_x, target_y, target_z;
    float maxsnglintrE;
    float maxsnglintrEFOM;
    char AGATA_data_fn[512];
};

class GEB_EVENT
{
private:

public:
    GEB_EVENT()
    {
        ptgd.clear();
        ptinp.clear();

        maxGebs = MAXGEBS;
    }
    GEB_EVENT ( int maxGebs_ )
    {
        ptgd.clear();
        ptinp.clear();

        maxGebs = maxGebs_;
    }
    virtual ~GEB_EVENT() {}

    int maxGebs;
    std::vector<GebData*> ptgd;
    std::vector<char*> ptinp;
};

TH2F* mkTH2F ( char* str1, char* str2, int n1, double lo1, double hi1, int n2, double lo2, double hi2, bool recreate = false );
TH2F* make2D ( const char* txt, int xln, int xlo, int xhi, int yln, int ylo, int yhi );
TH1D* mkTH1D ( char* str1, char* str2, int nn, double lo, double hi, bool recreate = false );
TH1D* make1D ( const char* txt, int xln, int xlo, int xhi );

#endif
