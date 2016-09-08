#ifndef SORTMANAGER_H
#define SORTMANAGER_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#include <signal.h>
#include <time.h>
#include <stddef.h>
#include <zlib.h>

#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TH1.h"
#include "TDirectory.h"

#include "TH2.h"
#include "TObjArray.h"
#include "TObject.h"
#include "TKey.h"
#include "TSystem.h"
#include "TCutG.h"
#include "TTree.h"

#include "TMapFile.h"

#include "GEBSort.h"
#include "GTMerge.h"
#include "veto_pos.h"
// #include "ctk.h"

#include "GoddessStruct.h"
#include "gdecomp.h"

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

class PARS : public TObject
{
private:

public:
    PARS();
    virtual ~PARS();

    char ConfigFile[STRLEN];
    short noCalib;
    bool noMapping;
    bool noHists;
    unsigned short ignoreThresholds;
    unsigned short siDetailLvl;
    std::string userFilter;
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

    ClassDef ( PARS, 1 );
};

// struct GEBDATA
// {
//     int type; /* type of data following */
//     int length;
//     unsigned long long timestamp;
// };

class GEB_EVENT : public TObject
{
private:

public:
    GEB_EVENT();
    GEB_EVENT ( int maxGebs_ );
    virtual ~GEB_EVENT();

    int maxGebs;
    std::vector<GEBDATA*> ptgd;
    std::vector<char*> ptinp;

    ClassDef ( GEB_EVENT, 1 );
};

// In time_stamp.c
int time_stamp();

// In bin_dgs.c
int sup_dgs();
void getcal ( char* file );
int DGSEvDecompose_v3 ( unsigned int* ev, int len, DGSEVENT* theDGSEvent );
int bin_dgs ( GEB_EVENT* GEB_event );
void SetBeta();

// In bin_dgod.c
int DFMAEvDecompose_v3 ( unsigned int* ev, int len, DFMAEVENT* theDFMAEvent );
int sup_dgod();
int bin_dgod ( GEB_EVENT* GEB_event );

// In bin_agod.c
void AGODEvDecompose ( unsigned int* ev, int len, AGODEVENT* theAGODEvent );
void sup_agod();
int bin_agod ( GEB_EVENT* GEB_event );

// In bin_god.c
void sup_god();
int bin_god ( GEB_EVENT* gebEvt );

// In ProcessManagers.cxx
int GebTypeStr ( int type, char str[] );
void CheckNoArgs ( int required, int actual, char* str );
void UPDSSHMEM ( time_t& t1, time_t& t2, TMapFile* mfile, char* shareMemFile );
float findAzimuthFromCartesian ( float xx, float yy );
float findPolarFromCartesian ( float xx, float yy, float zz, float* rr );

TH2F* mkTH2F ( char* str1, char* str2, int n1, double lo1, double hi1, int n2, double lo2, double hi2 );
TH2F* make2D ( const char* txt, int xln, int xlo, int xhi, int yln, int ylo, int yhi );
TH1D* mkTH1D ( char* str1, char* str2, int nn, double lo, double hi );
TH1D* make1D ( const char* txt, int xln, int xlo, int xhi );

class SortManager : public TObject
{
private:
    SortManager();

    static SortManager* s_instance;

public:
    virtual ~SortManager();

    static SortManager* sinstance();

    PARS* execParams;

    std::ifstream inData;

    char CommandFileName[STRLEN];
    time_t tdmp;
    time_t tdmplast;

    double pol[MAXDETPOS + 1];
    double azi[MAXDETPOS + 1];
    long long int ndethits[MAXDETPOS + 1];

    int nn1, nn2, nn3, firsttime;
    long long int t0, TSprev;
    int nn, ii, nbadTS;

    static void signal_catcher ( int sigval );

    GEBDATA* buffHeader[MAXCOINEV];
    char* buffData[MAXCOINEV];

    GEB_EVENT* GEB_event;
    GEB_EVENT* overflowGEBEv;

    DFMAEVENT DFMAEvent[MAXCOINEV];
    unsigned int numDGOD;

    AGODEVENT AGODEvent[MAXCOINEV];
    unsigned int numAGOD;
    unsigned long long lastTS;

    DGSEVENT DGSEvent[MAXCOINEV];
    int tlkup[NCHANNELS];
    int tid[NCHANNELS];
    int ng;

    GoddessData* godData;

    int ShowStatus();

    int GEBSort_read_chat ( char* name );
    int GEBacq ( char* ChatFileName );
    int GEBGetEv ( );

    std::vector<GamData>* gammaDets;
    std::vector<SiDataBase>* siDets;
    std::vector<IonData>* ionChamber;

    void SetGamDets ( std::vector<GamData>* gd_ )
    {
        gammaDets = gd_;
    }

    void SetSiDets ( std::vector<SiDataBase>* sd_ )
    {
        siDets = sd_;
    }

    void SetIonChamber ( std::vector<IonData>* ic_ )
    {
        ionChamber = ic_;
    }

    bool GetWriteEventFlag();

    ClassDef ( SortManager, 1 );
};

#endif
