#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <type_traits>
#include <cxxabi.h>

#include "TROOT.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TList.h"
#include "TLine.h"
#include "TGraph.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"
#include "TH2F.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TMath.h"
#include "TF1.h"
#include "TDirectoryFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

#include "../goddess/include/GoddessStruct.h"

#ifndef __GODDESSANAMACROSINCLUDES__
#define __GODDESSANAMACROSINCLUDES__

template<typename T> inline void DisplayListOfMapKeys(std::map<string, T> map_)
{
  for(auto itr = map_.begin(); itr != map_.end(); itr++)
  {
   std::cout <<  itr->first << std::endl;
  }
}

inline void DisplayListOfMapKeys(std::map<string, TH1> map_)
{
  DisplayListOfMapKeys<TH1>(map_);
}

template<typename T> inline void GetListOfSectorsToTreat ( std::vector<unsigned short>* sectorsList, T sector )
{
    std::cout << "adding " << sector << " to the list of sectors to treat..." << std::endl;

    sectorsList->push_back ( sector );
}

template<typename First, typename... Rest> inline void GetListOfSectorsToTreat ( std::vector<unsigned short>* sectorsList, First fstSector, Rest... otherSectors )
{
    GetListOfSectorsToTreat<unsigned short> ( sectorsList, fstSector );

    GetListOfSectorsToTreat ( sectorsList, otherSectors... );
}

inline int DecodeStripNumber(int st_)
{
  return st_%100;
}

#endif