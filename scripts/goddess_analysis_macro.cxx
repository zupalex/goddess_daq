#include <iostream>
#include <fstream>

#include "TROOT.h"
#include "TSystem.h"

void LinuxLibrariesLoader(string myPath)
{
  gSystem->Load(Form("%s/exec/libORRUBA.so", myPath.c_str()));
  gSystem->Load(Form("%s/exec/libGoddessStruct.so", myPath.c_str()));
}

void MacLibrariesLoader(string myPath)
{
  gSystem->Load(Form("%s/exec/libORRUBA.dylib", myPath.c_str()));
  gSystem->Load(Form("%s/exec/libGoddessStruct.dylib", myPath.c_str()));
}

void goddess_analysis_macro(string myPathToGoddessDaq)
{
  #if __APPLE__
  MacLibrariesLoader(myPathToGoddessDaq);  
  #elif __linux__
  LinuxLibrariesLoader(myPathToGoddessDaq);
  #elif __unix__ 
  LinuxLibrariesLoader(myPathToGoddessDaq);
  #endif
  
  gROOT->ProcessLine(Form(".L %s/exec/calibstrips.cxx++", myPathToGoddessDaq.c_str())); 
  gROOT->ProcessLine(Form(".L %s/exec/MakeEventLists.cxx++", myPathToGoddessDaq.c_str()));
}