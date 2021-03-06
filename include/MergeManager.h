#ifndef MERGEMANAGER_H
#define MERGEMANAGER_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
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

#include "GTMerge.h"

#define payloadMaxSize 50000

using std::string;

class InDataInfo
{
private:

public:
    InDataInfo ( std::ifstream* istream_ );
    ~InDataInfo() {}

    int fileNum;
    string fileName;
    int type;
    long long unsigned int firstTimestamp;
    std::ifstream* istream;
    unsigned long long fileLength;
    int typical_payload_length;
};

class EVENT
{
private:

public:
    EVENT();
    ~EVENT();

    EVENT CopyContent ( const EVENT toCopy )
    {
        key = toCopy.key;

        gd->length = toCopy.gd->length;
        gd->timestamp = toCopy.gd->timestamp;
        gd->type = toCopy.gd->type;

        memcpy ( payload, toCopy.payload, gd->length );

        return *this;
    }

    GebData* gd;
    char* payload;
    int key;
};

void CheckNoArgs ( int required, int actual, string str );

bool CompareTimestamps ( EVENT* ev1, EVENT* ev2 );
// bool CompareTimestamps ( int ev1, int ev2 );

// In GEBMerge.cxx
EVENT* GTGetDiskEv ( InDataInfo* inFile, EVENT* bufEVENT, bool printInfo );
#if (USEZLIB == 0 )
int bread ( std::ifstream inFile, char *val, int *pos, int *buf, int *bufsiz );
#else
int bread ( gzFile in, int *val, int *pos, int *buf, int *bufsiz );
#endif


class MergeManager
{
private:
    MergeManager();

    static MergeManager* s_instance;

public:
    virtual ~MergeManager();

    static MergeManager* sinstance();

    unsigned long long int loopCounter;

    unsigned long long int readBytesCount;

    bool goBackToTop;

    unsigned int maxCoincEv;
    std::map<unsigned long long int, std::vector<EVENT*>*> Event;
    std::map<unsigned long long int, std::vector<std::pair<InDataInfo*, EVENT*>*>*> overflowEvent;

    unsigned int maxBigBufSize;
    std::vector<char*>* bigbuf;

    std::vector<InDataInfo*>* inData;
    std::ofstream outData;

    std::vector<int> pendingFiles;

    void RemoveFromInputList ( string input );

    int tlkup[NCHANNELS];
    int tid[NCHANNELS];

    std::pair<unsigned int, unsigned long long int> GetSizeAndBytesCount ( bool ofTyspe );

#if(USEZLIB==1)
    gzFile zFile[MAXFILES];
#endif
};

#endif
