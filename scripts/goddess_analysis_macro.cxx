#include <iostream>
#include <fstream>

#include "TROOT.h"
#include "TSystem.h"

void LinuxLibrariesLoader()
{
  gSystem->Load("./libORRUBA.so");
  gSystem->Load("./libGoddessStruct.so");
}

void MacLibrariesLoader()
{
  gSystem->Load("./libORRUBA.dylib");
  gSystem->Load("./libGoddessStruct.dylib");
}

void goddess_analysis_macro()
{
  #if __APPLE__
  MacLibrariesLoader();  
  #elif __linux__
  LinuxLibrariesLoader();
  #elif __unix__ 
  LinuxLibrariesLoader();
  #endif
  
  gROOT->ProcessLine(".L ./calibstrips.cxx++"); 
  gROOT->ProcessLine(".L ./MakeEventLists.cxx++");
}