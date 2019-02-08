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

#include "GEBSort.h"
#include "GTMerge.h"

#include "DGSProcessor.h"
#include "GretProcessor.h"
#include "GRProcessor.h"
#include "GODProcessor.h"

#include "GoddessConfig.h"
#include "GoddessStruct.h"

#include "GoddessData.h"

// In ProcessManagers.cxx
int GebTypeStr ( int type, char str[] );
void CheckNoArgs ( int required, int actual, char* str );
void UPDSSHMEM ( time_t& t1, time_t& t2, TMapFile* mfile, char* shareMemFile );

class SortManager
{
private:
    SortManager();

    static SortManager* s_instance;

public:
    virtual ~SortManager();

    static SortManager* sinstance();

    PARS* execParams;
    GoddessConfig* gConfig;

    std::ifstream inData;

    unsigned long long int totBytesRead;

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

    GebData* buffHeader[MAXCOINEV];
    char* buffData[MAXCOINEV];

    GEB_EVENT* gebEvt;
    GEB_EVENT* overflowGEBEv;

    DFMAEVENT dfmaEvt[MAXCOINEV];
    unsigned int numDGOD;

    AGODEVENT agodEvt[MAXCOINEV];
    unsigned int numAGOD;
    unsigned long long lastTS;

    GODProcessor* theGODProcessor;

    int* tlkup;
    int* tid;
    int ng;

    DGSProcessor* theDGSProcessor;
    GretProcessor* theGretProcessor;
    GRProcessor* theGRProcessor;
    DGSEVENT dgsEvt[MAXCOINEV];
    GRETEVENT gretset[MAXCOINEV];

    int ShowStatus();

    int GEBSort_read_chat ( char* name );
    int GEBacq ( char* ChatFileName );
    int GEBGetEv ( );

    bool GetWriteEventFlag();
    
//     int GammaProcessor;
//     float sphere_split;
};

#endif
