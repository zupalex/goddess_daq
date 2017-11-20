#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <vector>
#include <random>
#include <cxxabi.h>

#include "TObject.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TMath.h"
#include "TF1.h"
#include "TEventList.h"
#include "TEntryList.h"
#include "TChain.h"

#include "TROOT.h"
#include "TFile.h"
#include "TDirectoryFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

bool AskForYesOrNO ( string messageToDisplay )
{
    char userInput[256];

    bool inputIsValid = false;

    bool aYes, aNo;

    while ( !inputIsValid )
    {
        inputIsValid = true;

        std::cout << messageToDisplay;
        std::cin >> userInput;

        aYes = ( strcmp ( userInput, "y" ) == 0 || strcmp ( userInput, "yes" ) == 0 );
        aNo = ( strcmp ( userInput, "n" ) == 0 || strcmp ( userInput, "no" ) == 0 );

        if ( ! ( aYes || aNo ) )
        {
            inputIsValid = false;
            std::cerr << "\nInvalid input. Answer [yes/y] or [no/n]\n" << std::endl;
        }
    }

    return aYes ? true : false;
}

std::vector<TEntryList*> entryLists;

std::map<string, TEntryList*> rEL;

TChain* myChain = new TChain();

void InitiateTChain()
{
    std::cout << "To use InitiateTChain function, please provide the following arguments:" << std::endl;
    std::cout << "1) The name of the tree." << std::endl;
    std::cout << "2) The suffix (if any) contained in the run names you want to chain." << std::endl;
    std::cout << "3) The list of runs you want to chain separated by comas" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "InitiateTChain(\"god\", \"_XeRuns\", 52, 54, 55, 56, 58, 59)" << std::endl;
    std::cout << "InitiateTChain(\"god\", \"\", 252, 253,254)" << std::endl;
    std::cout << std::endl;
    std::cout << "This will \"feed\" the TChain called \"myChain\" that you can then call to do whatever you want." << std::endl;

    return;
}

template<typename T> void InitiateTChain ( TChain* chain, string suffix, T runNbr )
{
    std::cout << "Adding " << Form ( "./run%d%s.root", ( int ) runNbr, suffix.c_str() ) << " to the TChain" << std::endl;

    chain->Add ( Form ( "./run%d%s.root", ( int ) runNbr, suffix.c_str() ) );

    return;
}

template<typename First, typename... Rest> void InitiateTChain ( TChain* chain, string suffix, First first, Rest... rest )
{
    std::cout << "Adding " << Form ( "./run%d%s.root", ( int ) first, suffix.c_str() ) << " to the TChain" << std::endl;

    InitiateTChain ( chain, suffix, rest... );

    chain->Add ( Form ( "./run%d%s.root", ( int ) first, suffix.c_str() ) );

    return;
}

template<typename First, typename... Rest> void InitiateTChain ( string treeName, string suffix, First first, Rest... rest )
{
    myChain = new TChain ( treeName.c_str() );

    InitiateTChain ( myChain, suffix, first, rest... );

    return;
}

void OpenEntryListsFile ( string fileName = "MyEntryLists.root" )
{
    string currPath = ( string ) gDirectory->GetPath();

    TFile *file = new TFile ( fileName.c_str() ,"read" );

    auto lOK = file->GetListOfKeys();

    std::vector<TEntryList> retEntryLists;

    retEntryLists.clear();

    gDirectory->cd ( currPath.c_str() );

    for ( int i = 0; i < lOK->GetSize(); i++ )
    {
        TEntryList* newRetEntList = ( TEntryList* ) ( file->Get ( lOK->At ( i )->GetName() )->Clone() );

        rEL[newRetEntList->GetName()] = newRetEntList;
    }

    file->Close();

    return;
}

void DumpCurrentEntryLists()
{
    string rootFileName = gDirectory->GetName();

    if ( rootFileName.find ( ".root" ) != std::string::npos )
    {
        std::cout << "Loaded \"TEntryList\"s valid for this run:" << std::endl;

        for ( auto itr = rEL.begin(); itr != rEL.end(); itr++ )
        {
            string eLName = itr->first;

            std::size_t foundRun = eLName.find ( ( rootFileName.substr ( 0, rootFileName.find ( ".root" ) ) ).c_str() );

            if ( foundRun != std::string::npos )
                std::cout << itr->first.substr ( 0, foundRun-1 ) << " (" << itr->second->GetN() << " entries)" << std::endl;
        }
    }
    else
    {
        std::cout << "No rootfile pre-loaded. \"Existing EntryList\"s are:" << std::endl;

        std::vector<string> allEL;
        allEL.clear();

        for ( auto itr = rEL.begin(); itr != rEL.end(); itr++ )
        {
            string eLName = itr->first;

            std::size_t foundRun = eLName.find ( "_run" );

            if ( foundRun != std::string::npos )
            {
                string listName = itr->first.substr ( 0, foundRun );

                if ( std::find ( allEL.begin(), allEL.end(), listName ) == allEL.end() )
                    allEL.push_back ( listName );
            }
        }

        for ( unsigned int n = 0; n < allEL.size(); n++ )
            std::cout << allEL[n] << std::endl;
    }

    return;
}

void ApplyEntryList ( TTree* tree, string entryListName )
{
    string rootFileName = gDirectory->GetName();

    string realELName = Form ( "%s_%s", entryListName.c_str(), ( rootFileName.substr ( 0, rootFileName.find ( ".root" ) ) ).c_str() );

    if ( rEL.find ( realELName ) != rEL.end() )
        tree->SetEntryList ( rEL[realELName.c_str()] );
    else
        std::cerr << "ERROR: Entry list specified do not exists!" << std::endl;


    return;
}

void ApplyEntryList ( TChain* chain, string entryListName )
{
    auto lOF = chain->GetListOfFiles();

    TEntryList* chainEntryList = new TEntryList ( entryListName.c_str(), entryListName.c_str() );

    for ( int i = 0; i < chain->GetNtrees(); i++ )
    {
        TFile* f = ( TFile* ) lOF->At ( i );

        string fName = f->GetTitle();

        fName = fName.substr ( fName.find ( "run" ) );

        string realELName = Form ( "%s_%s", entryListName.c_str(), ( fName.substr ( 0, fName.find ( ".root" ) ) ).c_str() );

        if ( rEL.find ( realELName ) != rEL.end() )
        {
            std::cout << "Found EntryList " << realELName << " for file " << fName << std::endl;
            chainEntryList->Add ( rEL[realELName.c_str()] );
        }
        else
            std::cerr << "ERROR: Entry list specified not found for " << fName << std::endl;
    }

    chain->SetEntryList ( chainEntryList );

    return;
}

TEntryList* MakeEntryList ( TTree* tree, string entryListName, string conds, string opts = "" )
{
    time_t startTime = time ( NULL );

    string rootFileName = gDirectory->GetName();

    entryListName += "_" + rootFileName.substr ( 0, rootFileName.find ( ".root" ) );

    tree->Draw ( Form ( ">>%s", entryListName.c_str() ),conds.c_str(), Form ( "entrylist %s", opts.c_str() ) );

    time_t endTime = time ( NULL );

    std::cout << std::endl;
    std::cout << "Operation performed in " << difftime ( endTime, startTime ) << " seconds" << std::endl;

    TEntryList* newEntList = ( TEntryList* ) gDirectory->Get ( entryListName.c_str() );

    entryLists.push_back ( newEntList );

    return newEntList;
}

TEntryList* MakeEntryList ( TChain* chain, string entryListName, string conds, string opts = "" )
{
    time_t startTime = time ( NULL );

    chain->Draw ( Form ( ">>%s", entryListName.c_str() ),conds.c_str(), Form ( "entrylist %s", opts.c_str() ) );

    time_t endTime = time ( NULL );

    std::cout << std::endl;
    std::cout << "Operation performed in " << difftime ( endTime, startTime ) << " seconds" << std::endl;

    TEntryList* chainEntryList = ( TEntryList* ) gDirectory->Get ( entryListName.c_str() );
    TList* eLList = chainEntryList->GetLists();

    for ( int i = 0; i < eLList->GetSize(); i++ )
    {
        TEntryList* newEntList = ( TEntryList* ) eLList->At ( i );

        string rootFileName = newEntList->GetFileName();

        string newELName = entryListName + "_" + rootFileName.substr ( rootFileName.find ( "/run" ) +1, rootFileName.find ( ".root" ) - ( rootFileName.find ( "/run" ) +1 ) );

        newEntList->SetName ( newELName.c_str() );

        entryLists.push_back ( newEntList );
    }

    return chainEntryList;
}

int DeleteEntryListFromFile ( TEntryList* list, TFile* file )
{
    if ( file->FindKey ( list->GetName() ) != NULL || file->FindObject ( list->GetName() ) != NULL )
    {
        string objToDelete = ( string ) list->GetName() + ";*";

        file->Delete ( objToDelete.c_str() );
    }
    else return -1;

    return 1;
}

int DeleteEntryListFromFile ( TEntryList* list, string fileName )
{
    TFile *file = new TFile ( fileName.c_str(), "UPDATE" );

    if ( file->IsOpen() )
    {
        return DeleteEntryListFromFile ( list, file );
    }
    else return -1;
}

int WriteEntryListToFile ( TEntryList* list, string fileName, string mode = "UPDATE" )
{
    string currPath = ( string ) gDirectory->GetPath();

    TFile *file = new TFile ( fileName.c_str(), mode.c_str() );

    if ( file->IsOpen() )
    {
        file->cd();

        if ( file->FindKey ( list->GetName() ) != NULL || file->FindObject ( list->GetName() ) != NULL )
        {
            std::cout << "A TEntryList with the same name has been found in the destination file..." << std::endl;
            if ( !AskForYesOrNO ( "Do you want to overwrite the TEventList already in the file?   " ) ) return 0;
            else
            {
                DeleteEntryListFromFile ( list, file );
            }
        }

        list->Write();

        file->Close();
    }
    else return -1;

    gDirectory->cd ( currPath.c_str() );

    return 1;
}

void WriteAllEntryListToFile ( string fileName = "MyEntryLists.root", string mode = "UPDATE" )
{
    if ( entryLists.size() > 0 )
        for ( unsigned short i = 0; i < entryLists.size(); i++ )
            WriteEntryListToFile ( entryLists[i], fileName, mode );
}

void EntryListsHelp()
{
    std::cout << "Help not written yet... Come back later..." << std::endl;
}

