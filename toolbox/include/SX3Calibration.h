#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "TMath.h"
#include "TPolyMarker.h"
#include "TSpectrum.h"
#include "goddess_analysis_macros.h"
#include "GoddessAnalysis.h"
#include "Sort_TimeDif.h"

class SX3Calibration
{
private:
public:

float GetSlope(pair<float,float> xs, pair<float,float> ys);
vector<pair<float,float>> ParallelGains(TH1D* histo);
void ParallelGains(vector<TH1D*> histos, float pedx, float pedy,int sector, int strip, bool isupstream);
//please only use two histograms at a time. One for the y-projection and one for the x-projection. Put the x-projection histogram first then the y-projection histogram in the vector.
void GainCorrection(string filename, string outfile, string treename, int sector, int strip, bool isupstream, string slopefile);
void PositionCalibration(string filenames, string tree, string outfile, int sector, int strip, vector<float> left_to_right_section, vector<float> y_sections, float alpha = 5.813);

// double quadratic_fit(double* x, double* par);
void PosCalCheck (string filenames, string treename, string outfile, vector<int> sectors_to_treat = {0,9,10,11});
void FitSmiles(TH2F* histo, int sector, int strip, bool isupstream, float x_0, float x_1);
void Braces(string filenames, string treename, int sector, int strip, bool isUpstream, float y_int, float par1, float par2);

struct params
{
  int sector;
  int strip;
  bool isupstream;
  int num_seg;
  float pedestal_y;
  float pedestal_x;
};

struct seg_params
{
  int segment;
  pair<float,float> coorl;
  pair<float,float> coorr;
  float slope;
  float area;
};

};

