#include "GoddessAnalysis.h"

std::vector<unsigned short> DecodeSectorsString ( std::string sectorsString, bool verbose )
{
    std::vector<unsigned short> sectorsNum;

    sectorsNum.clear();

    while ( sectorsString.length() > 0 )
    {
        size_t foundComa = sectorsString.find ( "," );
        size_t foundDash = sectorsString.find ( "-" );

        std::string sub1 = "";
        std::string sub2 = "";

        if ( foundComa != std::string::npos && foundComa < foundDash )
        {
            sub1 = sectorsString.substr ( 0, foundComa );
        }
        else if ( foundDash != std::string::npos )
        {
            sub1 = sectorsString.substr ( 0, foundDash );
            sub2 = sectorsString.substr ( foundDash+1, foundComa );
        }
        else
            sub1 = sectorsString;

        if ( sub1.length() > 0 )
        {
            while ( sub1.find ( " " ) != std::string::npos )
            {
                sub1.replace ( sub1.find ( " " ), 1, "" );
            }

            sectorsNum.push_back ( stoi ( sub1 ) );
        }
        if ( sub2.length() > 0 )
        {
            while ( sub1.find ( " " ) != std::string::npos )
            {
                sub1.replace ( sub1.find ( " " ), 1, "" );
            }

            int lowerBound = stoi ( sub1 );
            int upperBound = stoi ( sub2 );

            for ( int i = lowerBound+1; i < upperBound; i++ )
            {
                sectorsNum.push_back ( i );
            }
        }

        if ( foundComa != std::string::npos || foundDash != std::string::npos )
        {
//                 std::cout << "sectorsString was " << sectorsString;
            sectorsString.replace ( 0, sub2.length() > 0 ? foundDash+1 : foundComa+1, "" );
//                 std::cout << " ... is now " << sectorsString << "\n";
        }
        else
        {
            sectorsString.clear();
        }
    }

    if ( verbose )
    {
        std::cout << "Decoded the sectors string:\n";

        for ( unsigned int i = 0; i <sectorsNum.size(); i++ )
        {
            std::cout << "-> " << sectorsNum[i] << "\n";
        }
    }

    return sectorsNum;
}

std::vector<std::string> DecodeTags ( std::string tagsStr )
{
    std::vector<std::string> tags;

    std::size_t startPos = tagsStr.find_first_not_of ( " ," );;

    std::size_t separator = tagsStr.find_first_of ( " ,", startPos );

    std::string newTag;

    while ( separator != std::string::npos && startPos != std::string::npos )
    {
        newTag = tagsStr.substr ( startPos, separator - startPos );

        tags.push_back ( newTag );

        startPos = tagsStr.find_first_not_of ( " ,", separator );
        separator = tagsStr.find_first_of ( " ,", startPos );
    }

    if ( startPos != std::string::npos )
    {
        newTag = tagsStr.substr ( startPos, tagsStr.length() - startPos );

        tags.push_back ( newTag );
    }

    return tags;
}

std::vector<std::string> GetDirContent ( std::string dirName, std::string fileExt, std::string mustHaveAll, std::string cantHaveAny, std::string mustHaveOneOf, std::string startWith )
{
    DIR* toScan = opendir ( dirName.c_str() );
    dirent* dirEntry = new dirent;

    std::vector<std::string> fileList, mustAllTags, cantTags, mustOneOfTags;

    if ( !mustHaveAll.empty() )
    {
        mustAllTags = DecodeTags ( mustHaveAll );
    }

    if ( !cantHaveAny.empty() )
    {
        cantTags = DecodeTags ( cantHaveAny );
    }

    if ( !mustHaveOneOf.empty() )
    {
        mustOneOfTags = DecodeTags ( mustHaveOneOf );
    }

    while ( ( dirEntry = readdir ( toScan ) ) != NULL )
    {
        std::string entName = dirEntry->d_name;

        if ( startWith.empty() || ( !startWith.empty() && entName.find ( startWith.c_str() ) == 0 ) )
        {
            if ( fileExt.empty() || ( !fileExt.empty() && entName.find ( fileExt.c_str() ) != std::string::npos ) )
            {
                bool mustAllFlag = true, cantFlag = false, mustOneOfFlag = ( mustOneOfTags.size() > 0 ? false : true );

                if ( mustAllTags.size() > 0 )
                {
                    for ( unsigned int i = 0; i < mustAllTags.size(); i++ )
                    {
                        if ( entName.find ( mustAllTags[i].c_str() ) == std::string::npos ) mustAllFlag = false;
                    }
                }

                if ( cantTags.size() > 0 )
                {
                    for ( unsigned int i = 0; i < cantTags.size(); i++ )
                    {
                        if ( entName.find ( cantTags[i].c_str() ) != std::string::npos ) cantFlag = true;
                    }
                }

                if ( mustOneOfTags.size() > 0 )
                {
                    for ( unsigned int i = 0; i < mustOneOfTags.size(); i++ )
                    {
                        if ( entName.find ( mustOneOfTags[i].c_str() ) != std::string::npos ) mustOneOfFlag = true;
                    }
                }

                if ( mustAllFlag && !cantFlag && mustOneOfFlag ) fileList.push_back ( entName );
            }
        }
    }

    return fileList;
}

std::vector<std::string> DecodeFilesToTreat ( std::string filesStr )
{
    std::vector<std::string> files;

    std::size_t startPos = filesStr.find_first_not_of ( "*[]" );
    std::size_t special = filesStr.find_first_of ( "*[]", startPos );

    std::vector<std::string> fNameParts;
    std::vector<std::tuple<std::size_t, char, std::size_t>> sChars;

    std::string fStartWith;
    std::string fMustHaveAll;
    std::string fMustHaveOneOf;

    if ( special == std::string::npos )
    {
        files.push_back ( filesStr );
    }
    else
    {
        if ( startPos == 0 ) fStartWith = filesStr.substr ( 0, special );

        while ( special != std::string::npos )
        {
            fNameParts.push_back ( filesStr.substr ( startPos, special - startPos ) );

//             std::cout << "new name part: " << filesStr.substr ( startPos, special - startPos ) << "\n";

            sChars.push_back ( std::make_tuple ( special, filesStr[special], startPos ) );

//             std::cout << "separator: " << filesStr[special] << " @ " << special << "\n";

            startPos = filesStr.find_first_not_of ( "*[]", special );
            special = filesStr.find_first_of ( "*[]", startPos );
        }

        if ( startPos != std::string::npos )
        {
            fNameParts.push_back ( filesStr.substr ( startPos, filesStr.length() ) );

            fMustHaveAll += filesStr.substr ( startPos, filesStr.length() );
            fMustHaveAll += " ";

//             std::cout << "new must all tag: " << filesStr.substr ( startPos, filesStr.length() ) << "\n";

//             std::cout << "new name part: " << filesStr.substr ( startPos, special - startPos ) << "\n";
        }

        for ( unsigned int i = 0; i < sChars.size(); i++ )
        {
            if ( std::get<1> ( sChars[i] ) == '*' )
            {
                fMustHaveAll += filesStr.substr ( std::get<2> ( sChars[i] ), std::get<0> ( sChars[i] ) - std::get<2> ( sChars[i] ) );
                fMustHaveAll += " ";

//                 std::cout << "new must all tag: " << filesStr.substr ( std::get<2> ( sChars[i] ), std::get<0> ( sChars[i] ) - std::get<2> ( sChars[i] ) ) << "\n";
            }

            else if ( std::get<1> ( sChars[i] ) == '[' )
            {
                if ( i == sChars.size()-1 || std::get<1> ( sChars[i+1] ) != ']' )
                {
                    std::cerr << "Failed to decode the files input...\n";

                    return files;
                }
                else
                {
                    std::string toDecode = filesStr.substr ( std::get<0> ( sChars[i] ) +1, std::get<0> ( sChars[i+1] ) - std::get<0> ( sChars[i] ) - 1 );

                    std::vector<unsigned short> filesNbr = DecodeSectorsString ( toDecode, false );

                    std::string baseStr = filesStr.substr ( std::get<2> ( sChars[i] ), std::get<0> ( sChars[i] ) - std::get<2> ( sChars[i] ) );

                    for ( unsigned int j = 0; j < filesNbr.size(); j++ )
                    {
                        fMustHaveOneOf += Form ( "%s%d", baseStr.c_str(), filesNbr[j] );
                        fMustHaveOneOf += " ";

//                         std::cout << "new must one of tag: " << Form ( "%s%d", baseStr.c_str(), filesNbr[j] ) << "\n";
                    }

                    i++;
                }
            }

            else if ( std::get<1> ( sChars[i] ) == ']' )
            {
                std::cerr << "Failed to decode the files input...\n";

                return files;
            }
        }

        files = GetDirContent ( "./", "", fMustHaveAll, "", fMustHaveOneOf, fStartWith );
    }

    return files;
}

vector< string > SplitString ( string toSplit, string splitter )
{
    std::size_t startPos = 0;
    std::size_t comaPos = toSplit.find ( splitter.c_str() );

    vector<string> splitString;
    splitString.clear();

    while ( comaPos != string::npos )
    {
        splitString.push_back ( toSplit.substr ( startPos, comaPos- startPos ) );

        startPos = comaPos+splitter.length();
        comaPos = toSplit.find ( splitter.c_str(), startPos );
    }

    splitString.push_back ( toSplit.substr ( startPos ) );

    return splitString;
}

string FindAndReplaceInString ( string input, string toReplace, string substitute, unsigned int nTimes, int startIndex )
{
    int repSize = toReplace.length();

    vector<std::size_t> foundPosList;

    std::size_t foundPos = input.find ( toReplace.c_str() );

    while ( foundPos != string::npos )
    {
        foundPosList.push_back ( foundPos );

        foundPos = input.find ( toReplace.c_str(), foundPos+1 );
    }

    if ( nTimes == 0 ) nTimes = foundPosList.size();

    nTimes = std::min ( nTimes, ( unsigned int ) ( foundPosList.size()-startIndex ) );

    string newString = input;

    for ( unsigned int i = startIndex; i < startIndex+nTimes; i++ )
    {
        newString.replace ( foundPosList[i], repSize, substitute );
    }

    return newString;
}

GoddessAnalysis::GoddessAnalysis()
{
    defaultTreeName1 = "raw";
    defaultTreeName2 = "sorted";

    userTree = nullptr;
    userChain = nullptr;
}

GoddessAnalysis::GoddessAnalysis ( std::string filename ) : GoddessAnalysis()
{
    defaultFileName = filename;
}

GoddessAnalysis::GoddessAnalysis ( std::string filename, std::string treename, std::string treename2 ) : GoddessAnalysis()
{
    defaultFileName = filename;
    defaultTreeName1 = treename;
    defaultTreeName2 = treename2;
}

GoddessAnalysis::~GoddessAnalysis ()
{

}

void GoddessAnalysis::SetDefaultFileAndTrees ( std::string fileName, std::string treeName1, std::string treeName2 )
{
    if ( fileName.length() > 0 ) defaultFileName = fileName;
    if ( treeName1.length() > 0 ) defaultTreeName1 = treeName1;
    if ( treeName2.length() > 0 ) defaultTreeName2 = treeName2;
}

std::vector<unsigned short> GoddessAnalysis::GetStripsListToTreat ( std::string strips )
{
    std::vector<unsigned short> stripsList;
    stripsList.clear();

    if ( strips == "all" )
    {
        for ( int i = 0; i < 4; i++ )
            stripsList.push_back ( i );
    }
    else
    {
        std::istringstream stripsss;
        stripsss.clear();
        stripsss.str ( strips );

        unsigned short st_;
//         bool read = false;

        while ( stripsss >> st_ )
        {
            stripsList.push_back ( st_ );
        }
    }

    return stripsList;
}

void GoddessAnalysis::SetReacParameters ( float beamMass_, float beamEk_, float targetMass_, float ejecMass_, float recoilMass_, float qValGsGs_ )
{
    beamEk = beamEk_;
    beamMass = beamMass_;
    targetMass = targetMass_;
    ejecMass = ejecMass_;
    recoilMass = recoilMass_;
    qvalGsGs = qValGsGs_;
}

void GoddessAnalysis::SetBeamParameters ( float beamMass_, float beamEk_ )
{
    beamEk = beamEk_;
    beamMass = beamMass_;
}

void GoddessAnalysis::SetBeamEk ( float beamEk_ )
{
    beamEk = beamEk_;
}

void GoddessAnalysis::SetBeamMass ( float beamMass_ )
{
    beamMass = beamMass_;
}

void GoddessAnalysis::SetEjectileMass ( float ejecMass_ )
{
    ejecMass = ejecMass_;
}

void GoddessAnalysis::SetQvalueGsToGs ( float qvalGsGs_ )
{
    qvalGsGs = qvalGsGs_;
}

void GoddessAnalysis::SetRecoilMass ( float recoilMass_ )
{
    recoilMass = recoilMass_;
}

void GoddessAnalysis::SetTargetMass ( float targetMass_ )
{
    targetMass = targetMass_;
}

TH2F* GoddessAnalysis::DrawEnergyVsAngleSX3 ( TChain* chain, int nentries, std::string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts,
        bool isUpstream_, std::string strips, short unsigned int sector )
{
    return DrawEnergyVsAngleSX3 ( chain, nentries, hname, nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY, drawOpts, isUpstream_, strips, sector );
}

void ReadDetectorID ( std::string DetectorID, bool* isUpstream, unsigned short* numsector, bool* isBarrel, int* layernum, bool* side, bool* isSX3 )
{
    std::string str ( DetectorID );
    std::string IDtype;
    std::stringstream ss ( str );

    std::vector <std::string> ID;

    while ( ss >> IDtype )
        ID.push_back ( IDtype );

    std::string UorD = ID[1].substr ( 0,1 );
    std::string sector = ID[1].substr ( 1 );
    std::string defaultTreeName1;
    std::string det = ID[0].substr ( 0 );
    std::cout << "Entered:" << std::endl;
    for ( unsigned int l=0; l<ID.size(); l++ )
    {
        std::cout << ' ' << ID.at ( l );
        std::cout << '\n';
    }

    if ( ID.size() == 2 )

    {
        if ( det == "BB10" )
        {

            *side = true;
            *isBarrel=true;
            *layernum=1;
            *numsector = stoi ( sector );
            *isUpstream = false;
            *isSX3=false;

        }

        else if ( det == "SX3" )
        {
            *side = true;
            *numsector = stoi ( sector );
            *layernum=2;
            *isBarrel=true;
            *isSX3=true;
            if ( UorD == "U" )
            {
                *isUpstream = true;
                std::cout << "Upstream ";
            }
            else
            {
                *isUpstream = false;
                std::cout << "Downstream ";
            }
        }

        else
        {
            return;
        }
    }

    else if ( ID.size() == 4 )
    {
        std::cout << "Entered:" << std::endl;
        for ( unsigned int l=0; l<ID.size(); l++ )
        {
            std::cout << ' ' << ID.at ( l );
            std::cout << '\n';
        }
//         int ulength = ID[1].length();
        std::string upstream = ID[1];

        std::string detlayer = ID[2].substr ( 0 );
        std::string detside = ID[3].substr ( 0 );

        *isUpstream = ( UorD == "U" ? true : false );

        if ( sector == "A" ) *numsector = 0;
        else if ( sector == "B" ) *numsector = 1;
        else if ( sector == "C" ) *numsector = 2;
        else if ( sector == "D" ) *numsector = 3;
        else *numsector = stoi ( sector );

        *isSX3 = false;
        *isBarrel = true;

        if ( det == "SX3" || det == "sx3" || det == "superX3" || det == "SuperX3" || det == "SUPERX3" )  *isSX3 = true;
        else if ( det == "BB10" || det == "bb10" ) *isSX3 = false;
        else if ( det == "QQQ5" || det == "qqq5" ) *isBarrel = false;
        else
        {
            std::cout<<"Unknown detector named"<<std::endl;
            return;
        }

        if ( ID[2] == "dE" ) *layernum = 1;
        else if ( ID[2] == "dE" ) *layernum = 2;
        else if ( ID[2] == "dE" ) *layernum = 3;

        if ( detside == "p" ) *side = true;
        else if ( detside == "n" ) *side = false;
    }

    else
    {
        std::cout << "Incorrect Input for:" << DetectorID << std::endl;
    }
}


void GoddessAnalysis::CheckMapping ( std::string filename, std::string treename, std::string treename2, unsigned short channel1, unsigned short channel2, bool Digital, std::string DetectorID )
{
    TFile* rootfile = new TFile ( filename.c_str() );

    TTree* tree = ( TTree* ) rootfile->Get ( treename.c_str() );

    if ( tree == NULL )
    {

        tree = ( TTree* ) rootfile->Get ( ( ( std::string ) "trees/"+treename ).c_str() );

        if ( tree == NULL )
        {
            return;
        }
    }

    int entries = tree->GetEntries();

    std::cout <<"Entries in TTree:" << entries << std::endl;

    ORRUBARawData* treevar = new ORRUBARawData();

    tree->SetBranchAddress ( "si", &treevar );

    int binnum = Digital ? 2000 : 400; // It's the same as the if/else statement below!
    int starthere = 0;
    int endhere = Digital ? 400000 : 4000;

    TH1F* grraw = new TH1F ( "RawDataValue","Check Mapping", binnum, starthere, endhere );

    if ( treename == "raw" )
    {
        for ( int i = 0; i < entries; i++ )
        {
            tree->GetEntry ( i );

            int sizeraw = treevar->data.size();

            if ( i%20000 == 0 )
            {
                std::cout << "Treated entry raw # " << i << " / " << entries << "\r" << std::flush;
            }

            for ( unsigned short j = 0; j < sizeraw; j++ )
            {
                unsigned short channelnumber = treevar->data.at ( j ).channel;

                if ( treevar->isDigital.at ( j ) == !Digital )
                {
                    continue;
                }
                if ( channel1 <= channelnumber && channelnumber <= channel2 )
                {
                    unsigned long int sivalue = treevar->data.at ( j ).value;

                    grraw->Fill ( sivalue );
                }
            }
        }

    }

    srand ( std::time ( NULL ) );
    int rr =0;
    int randomcol;
    std::vector <int> randomnum;
    while ( rr<2 )
    {
        randomcol = rand() % 9 +1;

        if ( randomnum.empty() || randomnum[0] != randomcol )
        {
            randomnum.push_back ( randomcol );
            rr++;
        }
    }

    bool Upstream;
    unsigned short sectornum;
    bool front;
    int layer;
    bool isBarrel_;
    bool isSX3;

    ReadDetectorID ( DetectorID, &Upstream, &sectornum, &isBarrel_, &layer, &front, &isSX3 );

    TTree* tree2 = ( TTree* ) rootfile->Get ( treename2.c_str() );

    if ( tree2 == NULL )
    {

        tree2 = ( TTree* ) rootfile->Get ( ( ( std::string ) "trees/"+treename2 ).c_str() );

        if ( tree2 == NULL )
        {
            return;
        }
    }

    int entries2 = tree2->GetEntries();

    std::vector<SiDataDetailed>* treevar2 = new std::vector<SiDataDetailed>;

    tree2->SetBranchAddress ( "si", &treevar2 );

    TH1F* grsorted = new TH1F ( "SortedDataValue","Sorted Data Value", binnum, starthere, endhere );

    for ( int r =0; r <entries2; r++ )
    {
        tree2->GetEntry ( r );

        int sizesorted = treevar2->size();

        std::vector<float> sortedsivalue;

        if ( r%20000 == 0 )
        {
            std::cout << "Treated entry sorted # " << r << " / " << entries2 << "\r" << std::flush;
        }

        for ( int w=0; w<sizesorted; w++ )
        {
            if ( treevar2->at ( w ).isUpstream != Upstream || treevar2->at ( w ).isBarrel != isBarrel_ || treevar2->at ( w ).sector != sectornum )
            {
                continue;
            }

            if ( layer == 1 )
            {
                if ( front == true ) sortedsivalue = treevar2->at ( w ).dE.en.p;
                else sortedsivalue = treevar2->at ( w ).dE.en.n;
            }

            else if ( layer == 2 )
            {
                if ( front == true )
                {
                    sortedsivalue = treevar2->at ( w ).E1.en.p;

                    std::vector<float> valuen;

                    valuen = treevar2->at ( w ).E1.en.n;
                    int sizen = valuen.size();

                    if ( isSX3 )
                    {
                        for ( int stripnevent=0; stripnevent<sizen; stripnevent++ )
                        {
                            if ( treevar2-> at ( w ).E1.strip.n[stripnevent]== -1 )
                            {
                                sortedsivalue.push_back ( valuen[stripnevent] );
                            }

                        }
                    }
                }
                else
                {
                    std::vector<float> valuen;

                    valuen = treevar2->at ( w ).E1.en.n;
                    int sizen = valuen.size();

                    if ( isSX3 )
                    {
                        for ( int stripnevent=0; stripnevent<sizen; stripnevent++ )
                        {
                            if ( treevar2-> at ( w ).E1.strip.n[stripnevent]!= -1 ) sortedsivalue.push_back ( valuen[stripnevent] );
                        }
                    }

                    if ( !isSX3 ) sortedsivalue = treevar2->at ( w ).E1.en.n;
                }
            }

            else if ( layer == 3 )
            {
                if ( front == true ) sortedsivalue = treevar2->at ( w ).E2.en.p;
                else sortedsivalue = treevar2->at ( w ).E2.en.n;
            }

            int sizefront = sortedsivalue.size();
            for ( int q=0; q<sizefront; q++ )
            {
                grsorted->Fill ( sortedsivalue.at ( q ) );
            }
        }
    }

    grraw->Draw();
    grsorted->Draw ( "same" );

    grraw->SetFillStyle ( 3005 );
    grsorted->SetFillStyle ( 3004 );

    grraw->SetLineColor ( randomnum[0] );
    grraw->SetFillColor ( randomnum[0] );

    grsorted->SetLineColor ( randomnum[1] );
    grsorted->SetFillColor ( randomnum[1] );

    std::cout << std::endl;

    return;
}

void GoddessAnalysis::CheckMapping ( short unsigned int channel1, short unsigned int channel2, bool Digital, std::string DetectorID )
{
    if ( defaultFileName.empty() )
    {
        if ( gDirectory->GetFile() ==NULL )
        {
            return;
        }
        else
        {
            defaultFileName = gDirectory->GetFile()->GetName();
        }
    }

    CheckMapping ( defaultFileName, defaultTreeName1, defaultTreeName2, channel1, channel2, Digital, DetectorID );
}

// -------------------------- Core User Analysis Macro Functions --------------------------- //

void GoddessAnalysis::AddFileToTreat ( TFile* inFile, std::string treeName )
{
    userTree = ( TTree* ) inFile->FindObjectAny ( treeName.c_str() );

    if ( userTree == NULL )
    {
        std::cerr << treeName << ": tree not found in file " << inFile->GetName() << "...\n";
        return;
    }

    std::cout << "Found tree " << treeName << " in the file " << inFile->GetName() << "\n";

    if ( userChain == NULL )
    {
        std::cout << "User TChain did not exist... Creating it...\n";

        userChain = new TChain ( treeName.c_str() );
    }

    if ( treeName != ( std::string ) userChain->GetName() )
    {
        std::cerr << "Cannot add " << treeName << " to the chain. Trees have different sctructure...\n";

        return;
    }

    auto lOF = userChain->GetListOfFiles();

    if ( lOF != NULL && lOF->GetSize() > 0 )
    {
        for ( int i = 0; i < lOF->GetSize(); i++ )
        {
            if ( lOF->At ( i ) != NULL && ( ( std::string ) inFile->GetName() ) == ( ( std::string ) lOF->At ( i )->GetTitle() ) )
            {
                std::cerr << "File already in the TChain... skipping it...\n";
                return;
            }
        }
    }

    userChain->Add ( inFile->GetName() );
}

void GoddessAnalysis::AddFileToTreat ( std::string inFile, std::string treeName )
{
//     TFile* inRootFile = new TFile ( inFile.c_str(), "read" );
//
//     if ( inRootFile == NULL )
//     {
//         std::cerr << "Unabled to open the root file " << inFile << "\n";
//
//         return;
//     }
//
//     AddFileToTreat ( inRootFile, treeName );
//
//     return;

    std::vector<std::string> listOfFiles = DecodeFilesToTreat ( inFile );

    for ( unsigned int i = 0; i < listOfFiles.size(); i++ )
    {
        TFile* inRootFile = new TFile ( listOfFiles[i].c_str(), "read" );

        if ( inRootFile == NULL )
        {
            std::cerr << "Unabled to open the root file " << listOfFiles[i] << "\n";
        }
        else AddFileToTreat ( inRootFile, treeName );
    }

    return;
}

void GoddessAnalysis::AddFileToTreat ( std::vector<std::string> inFile, std::string treeName )
{
    for ( unsigned int i = 0; i < inFile.size(); i++ )
    {
        TFile* inRootFile = new TFile ( inFile[i].c_str(), "read" );

        if ( inRootFile == NULL )
        {
            std::cerr << "Unabled to open the root file " << inFile[i] << "\n";
        }
        else AddFileToTreat ( inRootFile, treeName );
    }

    return;
}


ClassImp ( GoddessAnalysis )



// ----------------- User Analysis Macros Stuffs ------------------------- //

std::map<string, std::pair<TObject*, std::vector<GamData*>>> histsMap;
std::vector<string> specialHists;

void ResetHistsStates ( bool all )
{
    if ( all )
    {
        for ( auto itr = histsMap.begin(); itr != histsMap.end(); itr++ )
        {
            ( itr->second ).second.clear();
        }
    }

    else
    {
        for ( unsigned int i = 0; i < specialHists.size(); i++ )
        {
            histsMap[specialHists[i]].second.clear();
        }
    }
}

bool CheckHistState ( string histName, GamData* gD )
{
    return ( std::find ( histsMap[histName].second.begin(), histsMap[histName].second.end(), gD ) != histsMap[histName].second.end() );
}

bool CheckHistState ( TObject* hist, GamData* gD )
{
    string histName = ( string ) hist->GetName();

    return CheckHistState ( histName, gD );
}

void AddToHistState ( string histName, GamData* gD )
{
    histsMap[histName].second.push_back ( gD );

    return;
}

void AddToHistState ( TObject* hist, GamData* gD )
{
    string histName = ( string ) hist->GetName();

    AddToHistState ( histName, gD );

    return;
}

void AddHists ( TH1F* h1, TH1F* h2 )
{
    AddHists<TH1F> ( h1, h2 );
}

void AddHists ( TH2F* h1, TH2F* h2 )
{
    AddHists<TH2F> ( h1, h2 );
}

TH1F* DrawSum ( TH1F* h1, TH1F* h2, bool cloneFirst, bool doDraw )
{
    TH1F* hSum;

    if ( cloneFirst )
    {
        hSum = ( TH1F* ) h1->Clone();
    }
    else
    {
        hSum = h1;
    }

    hSum->Add ( h2,1 );

    if ( doDraw ) hSum->Draw();

    return hSum;
}

TH1F* DrawSum ( TH1F** hists, string toSum )
{
    std::vector<unsigned short> listOfHists = DecodeSectorsString ( toSum, false );

    TH1F* hSum;

    if ( listOfHists.size() > 0 )
    {
        hSum = ( TH1F* ) hists[listOfHists[0]]->Clone();

        for ( unsigned int i = 1; i < listOfHists.size(); i++ )
        {
            hSum->Add ( hists[listOfHists[i]], 1 );
        }

        hSum->Draw();

        return hSum;
    }

    return nullptr;
}

TH2F* DrawSum ( TH2F** hists, string toSum )
{
    std::vector<unsigned short> listOfHists = DecodeSectorsString ( toSum, false );

    TH2F* hSum;

    if ( listOfHists.size() > 0 )
    {
        hSum = ( TH2F* ) hists[listOfHists[0]]->Clone();

        for ( unsigned int i = 1; i < listOfHists.size(); i++ )
        {
            hSum->Add ( hists[listOfHists[i]], 1 );
        }

        hSum->Draw ( "colz" );

        return hSum;
    }

    return nullptr;
}

TH1F* DrawSum ( TH1F** hists )
{
    if ( *hists != nullptr )
    {
        TH1F* hSum = dynamic_cast<TH1F*> ( *hists );

        if ( hSum != nullptr )
        {
            int counter = 1;

            while ( * ( hists+counter ) != nullptr )
            {
                TH1F* hAdd = dynamic_cast<TH1F*> ( * ( hists+counter ) );

                if ( hAdd != nullptr )
                {
                    hSum->Add ( hAdd );
                }

                counter++;
            }

            return hSum;
        }
    }

    return nullptr;
}

void PrintProgress ( long long int maxEvents_, long long int currEvt )
{
    if ( currEvt % 10000 == 0 )
    {
        std::cout << "Treated " << std::setw ( 11 ) << currEvt << " / " << std::setw ( 11 ) << std::left << maxEvents_;
        std::cout << " ( " << std::setw ( 5 ) << std::setprecision ( 2 ) << std::fixed << ( float ) currEvt/maxEvents_ * 100. << " % )\r" << std::flush;
    }
}

void PrintHistsMapContent()
{
    std::cout << "List of Histograms: \n\n";

    for ( auto itr = histsMap.begin(); itr != histsMap.end(); itr++ )
    {
        std::cout << "-> " << itr->first << "\n";
    }

    return;
}

TH1F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX, bool addToSpecialList )
{
    TH1F* newHist = new TH1F ( name.c_str(), title.c_str(), nBinsX, minX, maxX );
    histsMap[name] = std::make_pair ( newHist, * ( new std::vector<GamData*> ) );

    if ( addToSpecialList ) specialHists.push_back ( name );

    return newHist;
}

TH2F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY, bool addToSpecialList )
{
    TH2F* newHist = new TH2F ( name.c_str(), title.c_str(), nBinsX, minX, maxX, nBinsY, minY, maxY );
    histsMap[name] = std::make_pair ( newHist, * ( new std::vector<GamData*> ) );

    if ( addToSpecialList ) specialHists.push_back ( name );

    return newHist;
}
