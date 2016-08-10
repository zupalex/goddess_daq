#include <iostream>
#include <fstream>

#include "TROOT.h"
#include "TSystem.h"

string localPathToGoddessDaq = "";

void LinuxLibrariesLoader ( string myPath )
{
    gSystem->Load ( Form ( "%s/exec/libORRUBA.so", myPath.c_str() ) );
    gSystem->Load ( Form ( "%s/exec/libGoddessStruct.so", myPath.c_str() ) );
    gSystem->Load ( Form ( "%s/exec/libGoddessAnalysis.so", myPath.c_str() ) );
}

void MacLibrariesLoader ( string myPath )
{
    std::ifstream expScript ( Form ( "%s/exec/exportToDYLD.sh", myPath.c_str() ) );

    if ( !expScript.is_open() )
    {
        std::ofstream newExpScript ( Form ( "%s/exec/exportToDYLD.sh", myPath.c_str() ) );

        newExpScript << "#!/bin/bash" << std::endl << std::endl;

        newExpScript << "export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:" << myPath << "/exec/" << std::endl;

        newExpScript.close();

        expScript.open ( Form ( "%s/exec/exportToDYLD.sh", myPath.c_str() ) );
    }

    gROOT->ProcessLine ( Form ( ".! . %s/exec/exportToDYLD.sh", myPath.c_str() ) );

    gSystem->AddDynamicPath ( Form ( "%s/exec", myPath.c_str() ) );

    gSystem->Load ( Form ( "%s/exec/libORRUBA.dylib", myPath.c_str() ) );
    gSystem->Load ( Form ( "%s/exec/libGoddessStruct.dylib", myPath.c_str() ) );
    gSystem->Load ( Form ( "%s/exec/libGoddessAnalysis.dylib", myPath.c_str() ) );
}

void StartCalibGODDESSDets()
{
    gROOT->ProcessLine ( Form ( ".L %s/exec/calibstrips.cxx++", localPathToGoddessDaq.c_str() ) );
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

    std::cout << std::endl;
    std::cout << "To load the TEntryList creation macros, type \"LoadMakeEventLists()\"" << std::endl;
    std::cout << "To get the list of the functions you can use, type \"EntryListsHelp()\"" << std::endl;
    std::cout << std::endl;
    std::cout << "To use the calibration macros, type \"GoddessCalib::StartCalib()\"" << std::endl;
    std::cout << "To get the list of the functions you can use, call \"GoddessCalib::Help()\"" << std::endl;
    std::cout << std::endl;
}
