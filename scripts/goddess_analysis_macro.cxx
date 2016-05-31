#include <iostream>
#include <fstream>

#include "TROOT.h"
#include "TSystem.h"

string localPathToGoddessDaq = "";

void LinuxLibrariesLoader ( string myPath )
{
    gSystem->Load ( Form ( "%s/exec/libORRUBA.so", myPath.c_str() ) );
    gSystem->Load ( Form ( "%s/exec/libGoddessStruct.so", myPath.c_str() ) );
}

void MacLibrariesLoader ( string myPath )
{
    gSystem->Load ( Form ( "%s/exec/libORRUBA.dylib", myPath.c_str() ) );
    gSystem->Load ( Form ( "%s/exec/libGoddessStruct.dylib", myPath.c_str() ) );
}

void StartCalibGODDESSDets()
{
    gROOT->ProcessLine ( Form ( ".L %s/exec/calibstrips.cxx++", localPathToGoddessDaq.c_str() ) );
    gROOT->ProcessLine ( Form ( ".L %s/exec/CalibResistiveStrips.cxx++", localPathToGoddessDaq.c_str() ) );
}

void LoadMakeEventLists()
{
    gROOT->ProcessLine ( Form ( ".L %s/exec/MakeEventLists.cxx++", localPathToGoddessDaq.c_str() ) );
}

void goddess_analysis_macro ( string myPathToGoddessDaq )
{
    localPathToGoddessDaq = myPathToGoddessDaq;

#if __APPLE__
    MacLibrariesLoader ( myPathToGoddessDaq );
#elif __linux__
    LinuxLibrariesLoader ( myPathToGoddessDaq );
#elif __unix__
    LinuxLibrariesLoader ( myPathToGoddessDaq );
#endif

    std::cout << "To load the calibration macros, type \"StartCalibGODDESSDets()\"" << std::endl;
    std::cout << "To load the TEbtryList creation macros, type \"LoadMakeEventLists()\"" << std::endl;
}
