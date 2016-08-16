#ifndef GEBSORT_H
#define GEBSORT_H

#include "TFile.h"
#include "veto_pos.h"
#include <string>
#include <fstream>

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

#endif