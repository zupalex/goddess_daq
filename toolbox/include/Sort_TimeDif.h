#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "TMath.h"
#include "TPolyMarker.h"
#include "TSpectrum.h"
#include "goddess_analysis_macros.h"
#include "GoddessAnalysis.h"

using namespace std;

struct si_struct
{

    bool isBarrel;
    bool isUpstream;
    float sector_strip;
    double energy;
//     double angle;
    TVector3 pos;
    unsigned long long int time;
    float angle;
    int mult;
};

struct gam_struct
{

    vector<double> gam_energy;
    vector<int> crystal_num;
    unsigned long long int time;

};

