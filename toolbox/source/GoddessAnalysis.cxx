#include "GoddessAnalysis.h"

GoddessAnalysis* gA;
unsigned int ghistcount = 0;

GoddessAnalysis::GoddessAnalysis()
{
    defaultTreeName1 = "raw";
    defaultTreeName2 = "sorted";

    userTree = nullptr;
    userChain = nullptr;

    reacInfo = new GoddessReacInfos();
    geomInfo = new GoddessGeomInfos();

    userTreeList.clear();

    AutoReadAndSetReacParameters();

    SetGainAdjust ( "" );
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

double GoddessAnalysis::GetEnergyShiftFromQVal ( double qValShift, double qValRef, double angle )
{
    TF1* qValEval = new TF1 ( "qValEval",
                              [&] ( double* x, double* p ) -> Double_t
    {
        ( void ) p;
        return SiDataBase::QValue ( reacInfo, x[0], angle );
    },
    -20, 20, 0 );

    double qValObs = qValRef*qValShift;

    double enRef = qValEval->GetX ( qValRef );
    double enObs = qValEval->GetX ( qValObs );

    return enObs/enRef;
}

double GoddessAnalysis::GetEnergyShiftFromEx ( double exShift, double exRef, double angle )
{
    TF1* exEval = new TF1 ( "exEval",
                            [&] ( double* x, double* p ) -> Double_t
    {
        ( void ) p;
        return reacInfo->qValGsGs - SiDataBase::QValue ( reacInfo, x[0], angle );
    },
    -20, 20, 0 );

    double exObs = exRef*exShift;

    double enRef = exEval->GetX ( exRef );
    double enObs = exEval->GetX ( exObs );

    return enObs/enRef;
}

void GoddessAnalysis::AutoReadAndSetReacParameters()
{
    GoddessReacInfos* rInfo_ = ( GoddessReacInfos* ) gDirectory->FindObjectAny ( "GoddessReac" );

    if ( rInfo_ != nullptr ) reacInfo = rInfo_;
    else SetReacParameters();
}

void GoddessAnalysis::SetReacParameters ( float beamMass_, float beamEk_, float targetMass_, float ejecMass_, float recoilMass_, float qValGsGs_ )
{
    reacInfo->beamEk = beamEk_;
    reacInfo->beamA = beamMass_;
    reacInfo->targetA = targetMass_;
    reacInfo->ejecA = ejecMass_;
    reacInfo->recoilA = recoilMass_;
    reacInfo->qValGsGs = qValGsGs_;
}

void GoddessAnalysis::SetBeamParameters ( float beamMass_, float beamEk_ )
{
    reacInfo->beamEk = beamEk_;
    reacInfo->beamA = beamMass_;
}

void GoddessAnalysis::SetBeamEk ( float beamEk_ )
{
    reacInfo->beamEk = beamEk_;
}

void GoddessAnalysis::SetBeamMass ( float beamMass_ )
{
    reacInfo->beamA = beamMass_;
}

void GoddessAnalysis::SetEjectileMass ( float ejecMass_ )
{
    reacInfo->ejecA = ejecMass_;
}

void GoddessAnalysis::SetQvalueGsToGs ( float qvalGsGs_ )
{
    reacInfo->qValGsGs = qvalGsGs_;
}

void GoddessAnalysis::SetRecoilMass ( float recoilMass_ )
{
    reacInfo->recoilA = recoilMass_;
}

void GoddessAnalysis::SetTargetMass ( float targetMass_ )
{
    reacInfo->targetA = targetMass_;
}

void GoddessAnalysis::SetGainAdjust ( string fName )
{
    if ( fName.empty() )
        for ( int i = 0; i < 500; i++ ) gainAdjusts[i] = 1.0;

    else
    {
        std::ifstream input ( fName.c_str() );

        string readLine;

        while ( std::getline ( input, readLine ) )
        {
            if ( readLine.empty() ) continue;

            if ( readLine[0] == '#' )
            {
                std::istringstream iss;
                string dummy;
                string detType, detNum;

                bool isUpstream, isBarrel;

                int sector;

                iss.str ( readLine );

                iss >> dummy >> detType >> detNum;

                if ( detType == "QQQ5" ) isBarrel = false;
                else if ( detType == "SX3" || detType == "SuperX3" || detType == "BB10" ) isBarrel = true;
                else
                {
                    cerr << "/!\\ Invalid detector type: " << detType << endl;
                    return;
                }

                if ( detNum[0] == 'U' ) isUpstream = true;
                else if ( detNum[0] == 'D' ) isUpstream = false;
                else
                {
                    cerr << "/!\\ Invalid detector identifier: " << detNum << endl;
                    return;
                }

                sector = std::stoi ( detNum.substr ( 1 ) );

                bool isFrontside = true;

//                 cout << "Found gain adjusts for detector " << detType << " " << detNum << endl;
//                 cout << "(isBarrel: " << ( isBarrel ? "true" : "false" ) << " / isUpstream: " << ( isUpstream ? "true" : "false" ) << ")" << endl;

                while ( std::getline ( input, readLine ) )
                {
                    if ( readLine.empty() ) continue;

                    if ( readLine.substr ( 0, 5 ) == "-----" ) break;
                    else if ( readLine == "front" || readLine == "back" )
                    {
                        isFrontside = ( readLine == "front" );
//                         cout << "Retrieving " << ( isFrontside ? "front side" : "back side" ) << " infos...\n";
                    }
                    else
                    {
                        std::size_t semiColonPos = readLine.find ( ":" );
                        std::size_t separatorPos = 0;

                        while ( semiColonPos != string::npos )
                        {
                            int strip = std::stoi ( readLine.substr ( separatorPos, semiColonPos-separatorPos ) );

                            separatorPos = readLine.find ( "/", semiColonPos );

                            double gain = std::stod ( readLine.substr ( semiColonPos+1, separatorPos-semiColonPos-1 ) );

                            if ( separatorPos != string::npos ) separatorPos++;
                            semiColonPos = readLine.find ( ":", separatorPos );

                            int globStripID = ToStripID ( isUpstream, isBarrel, isFrontside, sector, strip );

                            gainAdjusts[globStripID] = gain;

//                             cout << "Strip # " << strip << " (global ID: " << globStripID << " ) => Gain = " << gain << endl;
                        }
                    }
                }

//                 cout << "-----------------------------------------------------------\n";
            }
            else continue;
        }
    }
}

TH2F* GoddessAnalysis::DrawEnergyVsAngleSX3 ( TChain* chain, int nentries, std::string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts,
        bool isUpstream_, std::string strips, short unsigned int sector )
{
    return DrawEnergyVsAngleSX3 ( chain, nentries, hname, nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY, drawOpts, isUpstream_, strips, sector );
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

void GoddessAnalysis::PrintUserTreeList()
{
    if ( userTreeList.size() > 0 )
    {
        std::cout << "User Tree List content:\n";

        for ( unsigned int i = 0; i < userTreeList.size(); i++ )
        {
            std::cout << " * " << userTreeList.at ( i )->GetCurrentFile()->GetName() << std::endl;
        }
    }
    else
    {
        std::cout << "The User Tree List is empty...\n";
    }

    return;
}

void GoddessAnalysis::AddFileToTreat ( TFile* inFile, std::string treeName )
{
    userTree = ( TTree* ) inFile->FindObjectAny ( treeName.c_str() );

    if ( userTree == NULL )
    {
        std::cerr << treeName << ": tree not found in file " << inFile->GetName() << "...\n";
        return;
    }

    string fileName = inFile->GetName();
    string dirName = userTree->GetDirectory()->GetName();

    string finalTreeName = ( fileName == dirName ) ? treeName : ( dirName + "/" + treeName );

    if ( userChain == NULL )
    {
        std::cout << "Initializing the user TChain...\n";

        userChain = new TChain ( finalTreeName.c_str() );
    }

    std::cout << "Found tree " << finalTreeName << " in the file " << inFile->GetName() << "\n";

    if ( finalTreeName != ( std::string ) userChain->GetName() )
    {
        std::cerr << "Cannot add " << finalTreeName << " to the chain. Trees have different sctructure...\n";

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

//     for ( unsigned int i = 0; i < userTreeList.size(); i++ )
//     {
//         if ( userTree == userTreeList.at ( i ) )
//         {
//             std::cerr << "File already in the TChain... skipping it...\n";
//             return;
//         }
//     }
//
//     userTreeList.push_back ( userTree );
}

void GoddessAnalysis::AddFileToTreat ( std::string inFile, std::string treeName )
{
    std::vector<std::string> listOfFiles = DecodeItemsToTreat ( inFile, "system", true, true );

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
    std::vector<int> listOfHists = DecodeNumberString ( toSum, false );

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
    std::vector<int> listOfHists = DecodeNumberString ( toSum, false );

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

void DrawSum ( string toSum, TH1F*& dest )
{
    DrawSum<TH1F> ( toSum, dest );
}

void DrawSum ( string toSum, TH2F*& dest )
{
    DrawSum<TH2F> ( toSum, dest );
}

TGraph* DrawGraphFromDataset ( vector< double > xSet, vector< double > ySet )
{
    if ( xSet.size() > 0 && xSet.size() == ySet.size() )
    {
        TGraph* gr = new TGraph ( xSet.size() );

        for ( unsigned int i = 0; i < xSet.size(); i++ )
        {
            gr->SetPoint ( i, xSet[i], ySet[i] );
        }

        return gr;
    }
    else return nullptr;
}

TH2F* DrawCombinedKinematics ( string qqq5List, string sX3List )
{
    std::vector<string> qqq5HistsNames = DecodeItemsToTreat ( qqq5List, "root" );
    std::vector<string> sX3HistsNames = DecodeItemsToTreat ( sX3List, "root" );

    if ( qqq5HistsNames.size() == 0 && sX3HistsNames.size() == 0 ) return nullptr;

    int sX3NBinsX = 0;
    float sX3BinWidthX = 0;

    int sX3FstBinNum = 0;
    float sX3FstBinCenterX = 0;

    int sX3LstBinNum = 0;
    float sX3LstBinCenterX = 0;

    int sX3NBinsY = 0;
    float sX3FstBinCenterY = 0;
    float sX3LstBinCenterY = 0;

    if ( sX3HistsNames.size() > 0 )
    {
        TH2F* sX3RefHist = nullptr;

        auto sX3HistItr = sX3HistsNames.begin();

        while ( sX3RefHist == nullptr && sX3HistItr != sX3HistsNames.end() )
        {
            TObject* foundObj = gDirectory->FindObjectAny ( ( *sX3HistItr ).c_str() );

            sX3RefHist = dynamic_cast<TH2F*> ( foundObj );

            sX3HistItr++;
        }

        if ( sX3RefHist != nullptr )
        {
            sX3NBinsX = sX3RefHist->GetXaxis()->GetNbins();
            sX3NBinsY = sX3RefHist->GetYaxis()->GetNbins();

            sX3BinWidthX = sX3RefHist->GetXaxis()->GetBinWidth ( 0 );

            sX3FstBinCenterY = sX3RefHist->GetYaxis()->GetBinCenter ( sX3RefHist->GetYaxis()->GetFirst() );
            sX3LstBinCenterY = sX3RefHist->GetYaxis()->GetBinCenter ( sX3RefHist->GetYaxis()->GetLast() );

            int binNum = 0;

            while ( sX3FstBinCenterX == 0 )
            {
                for ( int i = 0; i < sX3NBinsY; i++ )
                {
                    double binContent = sX3RefHist->GetBinContent ( binNum, i );

                    if ( binContent > 0 )
                    {
                        sX3FstBinNum = binNum;
                        sX3FstBinCenterX = sX3RefHist->GetXaxis()->GetBinCenter ( binNum );
                        break;
                    }
                }

                binNum++;
            }

//             cout << "First angle coverd by the SX3: " << sX3FstBinCenterX << endl;

            binNum = sX3RefHist->GetXaxis()->GetLast();

            while ( sX3LstBinCenterX == 0 )
            {
                for ( int i = sX3NBinsY-1; i > 0; i-- )
                {
                    double binContent = sX3RefHist->GetBinContent ( binNum, i );

                    if ( binContent > 0 )
                    {
                        sX3LstBinNum = binNum;
                        sX3LstBinCenterX = sX3RefHist->GetXaxis()->GetBinCenter ( binNum );
                        break;
                    }
                }

                binNum--;
            }

//             cout << "Last angle ceverd by the SX3: " << sX3LstBinCenterX << endl;
        }
    }

    double qqq5BinLowEdgesX[33];

    if ( qqq5HistsNames.size() > 0 )
    {
        TH2F* qqq5RefHist = nullptr;

        auto qqq5HistItr = qqq5HistsNames.begin();

        while ( qqq5RefHist == nullptr && qqq5HistItr != qqq5HistsNames.end() )
        {
            TObject* foundObj = gDirectory->FindObjectAny ( ( *qqq5HistItr ).c_str() );

            qqq5RefHist = dynamic_cast<TH2F*> ( foundObj );

            qqq5HistItr++;
        }

        if ( qqq5RefHist != nullptr )
        {
            for ( int i = 0; i < 32; i++ )
            {
                qqq5BinLowEdgesX[i] = qqq5RefHist->GetXaxis()->GetBinCenter ( i ) - qqq5RefHist->GetXaxis()->GetBinWidth ( i ) /2.;
//             cout << "QQQ5 range low bin edge #" << i << " : " << qqq5BinLowEdgesX[i] << endl;
            }

            qqq5BinLowEdgesX[32] = qqq5RefHist->GetXaxis()->GetBinCenter ( 31 ) + qqq5RefHist->GetXaxis()->GetBinWidth ( 31 ) /2.;
//         cout << "QQQ5 range low bin edge #32 : " << qqq5BinLowEdgesX[32] << endl;
        }
    }

    std::vector<float> binEdgesVect;
    binEdgesVect.clear();

    binEdgesVect.push_back ( 0. );

    double firstRangeEnd = sX3BinWidthX > 0 ? ( sX3FstBinCenterX - sX3BinWidthX ) : ( qqq5HistsNames.size() > 0 ? qqq5BinLowEdgesX[0] : 180. );

    double nextBinWidth = sX3BinWidthX > 0 ? sX3BinWidthX : 1.;
    double nextBinEdge = nextBinWidth;


    while ( nextBinEdge < firstRangeEnd )
    {
        binEdgesVect.push_back ( nextBinEdge );
        nextBinEdge += nextBinWidth;
    }

    if ( sX3BinWidthX > 0 )
    {
        nextBinEdge = sX3FstBinCenterX - sX3BinWidthX/2.;

        double secondRangeEnd = qqq5HistsNames.size() > 0 ? qqq5BinLowEdgesX[0] - sX3BinWidthX : 180.;

        while ( nextBinEdge < secondRangeEnd )
        {
            binEdgesVect.push_back ( nextBinEdge );
            nextBinEdge += sX3BinWidthX;
        }
    }

    if ( qqq5HistsNames.size() > 0 )
    {
        for ( unsigned int i = 0; i < 33; i++ ) binEdgesVect.push_back ( qqq5BinLowEdgesX[i] );

        nextBinEdge = qqq5BinLowEdgesX[32] + nextBinWidth;

        while ( nextBinEdge < 180. )
        {
            binEdgesVect.push_back ( nextBinEdge );
            nextBinEdge += nextBinWidth;
        }
    }

//     cout << "Bin Edges Vector filled. Copying it to an array...\n";

    double* binEdges = new double[binEdgesVect.size()];

    for ( unsigned int i = 0; i < binEdgesVect.size(); i++ )
    {
        binEdges[i] = binEdgesVect[i];
    }

    for ( unsigned int i = 0; i < binEdgesVect.size(); i++ )
    {
        if ( i > 0 && binEdges[i] <= binEdges[i-1] ) cout << "WARNING!! Incorrect bin order here ----> Final Bin edge #" << i << " -> " <<  binEdges[i] << endl;
//         cout << "Final Bin edge #" << i << " -> " <<  binEdges[i] << endl;
    }

    TH2F* combinedHist = new TH2F ( "EvsA_combined", "Energy vs. Angle complete range", binEdgesVect.size()-1, binEdges, sX3NBinsY, sX3FstBinCenterY, sX3LstBinCenterY );

    for ( unsigned int i = 0; i < qqq5HistsNames.size(); i++ )
    {
        TObject* foundObj = gDirectory->FindObjectAny ( qqq5HistsNames[i].c_str() );

        TH2F* hist = dynamic_cast<TH2F*> ( foundObj );

        if ( hist != nullptr )
        {
            for ( int binNumX = 0; binNumX < hist->GetXaxis()->GetNbins(); binNumX++ )
            {
                for ( int binNumY = 0; binNumY < hist->GetYaxis()->GetNbins(); binNumY++ )
                {
                    combinedHist->Fill ( hist->GetXaxis()->GetBinCenter ( binNumX ), hist->GetYaxis()->GetBinCenter ( binNumY ), hist->GetBinContent ( binNumX, binNumY ) );
                }
            }
        }
    }

    for ( unsigned int i = 0; i < sX3HistsNames.size(); i++ )
    {
        TObject* foundObj = gDirectory->FindObjectAny ( sX3HistsNames[i].c_str() );

        TH2F* hist = dynamic_cast<TH2F*> ( foundObj );

        if ( hist != nullptr )
        {
            for ( int binNumX = 0; binNumX < hist->GetXaxis()->GetNbins(); binNumX++ )
            {
                for ( int binNumY = 0; binNumY < hist->GetYaxis()->GetNbins(); binNumY++ )
                {
                    combinedHist->Fill ( hist->GetXaxis()->GetBinCenter ( binNumX ), hist->GetYaxis()->GetBinCenter ( binNumY ), hist->GetBinContent ( binNumX, binNumY ) );
                }
            }
        }
    }

    return combinedHist;
}

TGraph* SuperimposeCalculatedKinematics ( string input, string xStr, string yStr, TVector3 beam_off, double barrel_radius )
{
    std::ifstream indata ( input.c_str() );

    if ( !indata.is_open() )
    {
        cerr << "Input files does not exist...\n";
        return nullptr;
    }

    string readLine;
    bool startFilling;

    std::vector<std::pair<float, float>> dataPoints;
    dataPoints.clear();

    while ( std::getline ( indata, readLine ) )
    {
        if ( readLine.empty() ) continue;

        std::istringstream iss;

        iss.str ( readLine );
        string readWord;

        if ( !startFilling )
        {
            iss >> readWord;

            if ( readWord == "Ejectile" ) startFilling = true;
            continue;
        }
        else
        {
            float ejecLabAngle, ejecLabEnergy, ejecCMAngle, recoilLabAngle, recoilLabEnergy;

            iss >> ejecLabAngle >> ejecLabEnergy >> ejecCMAngle >> recoilLabAngle >> recoilLabEnergy;

            float* xData;
            float* yData;

            if(beam_off.Mag() > 0)
            {
            	cout << "Warning: Shift for C>M> Angle not yet computed..." << endl;

            	int angle_adjust = 0;

            	if(ejecLabAngle>90) angle_adjust = 180;
            	else if(angle_adjust<-90) angle_adjust = -180;

            	ejecLabAngle = TMath::ATan( (beam_off.Y()-barrel_radius) / (barrel_radius/TMath::Tan(ejecLabAngle*TMath::DegToRad()) - beam_off.Z()) )*TMath::RadToDeg() + angle_adjust;
            	recoilLabAngle = TMath::ATan( (beam_off.Y()-barrel_radius) / (barrel_radius/TMath::Tan(recoilLabAngle*TMath::DegToRad()) - beam_off.Z()) )*TMath::RadToDeg() + angle_adjust;
            }

            if ( xStr == "Ejectile Lab Angle" ) xData = &ejecLabAngle;
            else if ( xStr == "Ejectile Lab Energy" ) xData = &ejecLabEnergy;
            else if ( xStr == "C.M. Angle" ) xData = &ejecCMAngle;
            else if ( xStr == "Recoil Lab Angle" ) xData = &recoilLabAngle;
            else if ( xStr == "Recoild Lab Energy" ) xData = &recoilLabEnergy;
            else
            {
                cerr << "Invalid input for the x axis..." << endl;
                return nullptr;
            }

            if ( yStr == "Ejectile Lab Angle" ) yData = &ejecLabAngle;
            else if ( yStr == "Ejectile Lab Energy" ) yData = &ejecLabEnergy;
            else if ( yStr == "C.M. Angle" ) yData = &ejecCMAngle;
            else if ( yStr == "Recoil Lab Angle" ) yData = &recoilLabAngle;
            else if ( yStr == "Recoild Lab Energy" ) yData = &recoilLabEnergy;
            else
            {
                cerr << "Invalid input for the y axis..." << endl;
                return nullptr;
            }

            dataPoints.push_back ( std::make_pair ( *xData, *yData ) );
        }
    }

    TGraph* calcKin = new TGraph ( dataPoints.size() );

    for ( unsigned int i = 0; i < dataPoints.size(); i++ )
    {
        calcKin->SetPoint ( i, dataPoints[i].first, dataPoints[i].second );
    }

    calcKin->Draw ( "sameC" );

    return calcKin;
}

void PrintProgress ( long long int maxEvents_, long long int currEvt, int fileNbr )
{
    if ( currEvt % 10000 == 0 )
    {
        std::cout << "Treated " << std::setw ( 11 ) << currEvt << " / " << std::setw ( 11 ) << std::left << maxEvents_;
        if ( fileNbr == -1 ) std::cout << " ( " << std::setw ( 5 ) << std::setprecision ( 2 ) << std::fixed << ( float ) currEvt/maxEvents_ * 100. << " % )\r" << std::flush;
        else std::cout << " ( " << std::setw ( 5 ) << std::setprecision ( 2 ) << std::fixed << ( float ) currEvt/maxEvents_ * 100. << " % / Current File: " << fileNbr<< ")\r" << std::flush;
    }
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

TH2F* MakeNewHist ( string name, string title, int nBinsX, double* binsX, unsigned int nBinsY, int minY, int maxY, bool addToSpecialList )
{
    TH2F* newHist = new TH2F ( name.c_str(), title.c_str(), nBinsX, binsX, nBinsY, minY, maxY );
    histsMap[name] = std::make_pair ( newHist, * ( new std::vector<GamData*> ) );

    if ( addToSpecialList ) specialHists.push_back ( name );

    return newHist;
}

TH3F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY, unsigned int nBinsZ, int minZ, int maxZ, bool addToSpecialList )
{
    TH3F* newHist = new TH3F ( name.c_str(), title.c_str(), nBinsX, minX, maxX, nBinsY, minY, maxY, nBinsZ, minZ, maxZ );
    histsMap[name] = std::make_pair ( newHist, * ( new std::vector<GamData*> ) );

    if ( addToSpecialList ) specialHists.push_back ( name );

    return newHist;
}

TH3F* MakeNewHist ( string name, string title, int nBinsX, double* binsX, unsigned int nBinsY, int minY, int maxY, unsigned int nBinsZ, int minZ, int maxZ, bool addToSpecialList )
{
    std::vector<double> binsY;
    std::vector<double> binsZ;

    for ( unsigned int i = 0; i < nBinsY+1; i++ )
    {
        binsZ.push_back ( minY + ( maxY-minY ) /nBinsY );
    }

    for ( unsigned int i = 0; i < nBinsZ+1; i++ )
    {
        binsZ.push_back ( minZ + ( maxZ-minZ ) /nBinsZ );
    }

    TH3F* newHist = new TH3F ( name.c_str(), title.c_str(), nBinsX, binsX, nBinsY, &binsY[0], nBinsZ, &binsZ[0] );
    histsMap[name] = std::make_pair ( newHist, * ( new std::vector<GamData*> ) );

    if ( addToSpecialList ) specialHists.push_back ( name );

    return newHist;
}

TH1D* DrawGodHist ( TH2F* source, string toDraw, string opt )
{
    vector<int> stripIDs = ToStripID ( toDraw );

    int nBins = source->GetYaxis()->GetNbins();

    double xMin = source->GetYaxis()->GetXmin();
    double xMax = source->GetYaxis()->GetXmax();

    TH1D* dest = new TH1D ( Form ( "ghist%d", ghistcount ), "hist", nBins, xMin, xMax );

    ghistcount++;

    for ( unsigned int i = 0; i < stripIDs.size(); i++ )
    {
        TH1D* py = source->ProjectionY ( Form ( "%s_py_%d", source->GetName(), stripIDs[i] ), stripIDs[i]+1, stripIDs[i]+1 );

        dest->Add ( py, 1 );

        py->Delete();
    }

    dest->Draw ( opt.c_str() );

    return dest;
}

TH2D* DrawGodHist ( TH3F* source, string toDraw, string opt )
{
    vector<int> stripIDs = ToStripID ( toDraw );

    int nBinsX = source->GetXaxis()->GetNbins();
    int nBinsY = source->GetYaxis()->GetNbins();

    double xMin = source->GetXaxis()->GetXmin();
    double xMax = source->GetXaxis()->GetXmax();

    double yMin = source->GetYaxis()->GetXmin();
    double yMax = source->GetYaxis()->GetXmax();

    TH2D* dest = new TH2D ( Form ( "ghist%d", ghistcount ), "hist", nBinsX, xMin, xMax, nBinsY, yMin, yMax );

    ghistcount++;

    for ( unsigned int i = 0; i < stripIDs.size(); i++ )
    {
        source->GetZaxis()->SetRangeUser ( stripIDs[i], stripIDs[i]+1 );

        TH2D* pxy = ( TH2D* ) source->Project3D ( "xy" );

        dest->Add ( pxy, 1 );

        pxy->Delete();
    }

    dest->Draw ( opt.c_str() );

    return dest;
}

TVector3 GetDetPos ( TChain* c, bool isUpstream_, bool isBarrel_, int sector_, int verbose )
{
    TVector3 detPos ( 0, 0, 0 );

    float qqq5FirstStripWidth = 2.55; // mm

    long long int entry = 0;

    std::vector<SiDataBase>* vectSiData = new std::vector<SiDataBase>;
    std::vector<SiDataDetailed>* vectSiDataD = new vector<SiDataDetailed>;

    if ( c->SetBranchAddress ( "si", &vectSiData ) == -1 )
    {
        vectSiData = nullptr;
        cout << "Trying vector<SiDataDetailed>* for the silicon branch instead...\n";

        if ( c->SetBranchAddress ( "si", &vectSiDataD ) == -1 )
        {
            cerr << "si branch is not recognized!\n";
            return TVector3 ( 0, 0, 0 );
        }

        cout << "vector<SiDataDetailed>* matches the silicon branch. Processing...\n";
    }

    while ( detPos.Mag() == 0 && entry < c->GetEntries() )
    {
        c->GetEntry ( entry );

        unsigned int vectSiSize = ( vectSiData != nullptr ) ? vectSiData->size() : vectSiDataD->size();

        if ( vectSiSize > 0 )
        {
            for ( unsigned int i = 0; i < vectSiSize; i++ )
            {
                SiDataBase siData = ( vectSiData != nullptr ) ? vectSiData->at ( i ) : vectSiDataD->at ( i );

                if ( siData.isUpstream == isUpstream_ && siData.isBarrel == isBarrel_ && siData.sector == sector_ && siData.MultLayer ( 1, false ) == 1 )
                {
                    int strip = siData.StripMaxLayer ( 1, false );

                    detPos = siData.PosE1();

                    if ( !siData.isBarrel )
                    {
                        float offsetFromFirstStrip = 0;

                        for ( int st = 0; st < strip; st++ )
                        {
                            offsetFromFirstStrip += ( qqq5FirstStripWidth - st*0.05 ) / 2. + ( qqq5FirstStripWidth - ( st+1 ) *0.05 ) / 2.;
                        }

                        TVector3 stripOffset ( 0, offsetFromFirstStrip, 0 );

                        stripOffset.SetPhi ( detPos.Phi() );

                        detPos -= stripOffset;

                        int sectorPhiMod[4] = {1, 0, 3, 2};

                        detPos.SetPhi ( sectorPhiMod[sector_] * 90*TMath::DegToRad() );

                        if ( verbose ) cout << "Found detector " << GetDetectorID ( isUpstream_, isBarrel_, sector_ ) << " strip #" << strip << " ( offset from 1st strip = " << offsetFromFirstStrip << " )\n";
                    }

                    break;
                }
            }
        }

        entry++;
    }

    return detPos;
}

TVector3 GetDetPos ( TChain* c, bool isUpstream_, bool isBarrel_, int sector_, int depth_,  int verbose )
{
    ( void ) depth_;

    TVector3 detPos ( 0, 0, 0 );

    float qqq5FirstStripWidth = 2.55; // mm

    long long int entry = 0;

    std::vector<SiDataBase>* vectSiData = new std::vector<SiDataBase>;
    std::vector<SiDataDetailed>* vectSiDataD = new vector<SiDataDetailed>;

    if ( c->SetBranchAddress ( "si", &vectSiData ) == -1 )
    {
        vectSiData = nullptr;
        cout << "Trying vector<SiDataDetailed>* for the silicon branch instead...\n";

        if ( c->SetBranchAddress ( "si", &vectSiDataD ) == -1 )
        {
            cerr << "si branch is not recognized!\n";
            return TVector3 ( 0, 0, 0 );
        }

        cout << "vector<SiDataDetailed>* matches the silicon branch. Processing...\n";
    }

    while ( detPos.Mag() == 0 && entry < c->GetEntries() )
    {
        c->GetEntry ( entry );

        unsigned int vectSiSize = ( vectSiData != nullptr ) ? vectSiData->size() : vectSiDataD->size();

        if ( vectSiSize > 0 )
        {
            for ( unsigned int i = 0; i < vectSiSize; i++ )
            {
                SiDataBase siData = ( vectSiData != nullptr ) ? vectSiData->at ( i ) : vectSiDataD->at ( i );

                if ( siData.isUpstream == isUpstream_ && siData.isBarrel == isBarrel_ && siData.sector == sector_ && siData.MultLayer ( 1, false ) == 1 )
                {
                    int strip = siData.StripMaxLayer ( 1, false );

                    detPos = siData.PosE1();

                    if ( !siData.isBarrel )
                    {
                        float offsetFromFirstStrip = 0;

                        for ( int st = 0; st < strip; st++ )
                        {
                            offsetFromFirstStrip += ( qqq5FirstStripWidth - st*0.05 ) / 2. + ( qqq5FirstStripWidth - ( st+1 ) *0.05 ) / 2.;
                        }

                        TVector3 stripOffset ( 0, offsetFromFirstStrip, 0 );

                        stripOffset.SetPhi ( detPos.Phi() );

                        detPos -= stripOffset;

                        int sectorPhiMod[4] = {1, 0, 3, 2};

                        detPos.SetPhi ( sectorPhiMod[sector_] * 90*TMath::DegToRad() );

                        if ( verbose ) cout << "Found detector " << GetDetectorID ( isUpstream_, isBarrel_, sector_ ) << " strip #" << strip << " ( offset from 1st strip = " << offsetFromFirstStrip << " )\n";
                    }

                    break;
                }
            }
        }

        entry++;
    }

    return detPos;
}

TVector3 GetDetPos ( TTree* tree, bool isUpstream_, bool isBarrel_, int sector_, int depth_, int verbose )
{
    string treeName = tree->GetName();
    string fileName = ( tree->GetCurrentFile() != nullptr ) ? tree->GetCurrentFile()->GetName() : "";

    if ( fileName.empty() ) return TVector3 ( 0, 0, 0 );

    TChain* c = new TChain ( treeName.c_str(), treeName.c_str() );

    c->Add ( fileName.c_str() );

    return GetDetPos ( c, isUpstream_, isBarrel_, sector_, depth_, verbose );
}

TVector3 GetDetPos ( GoddessGeomInfos* geomInfo, bool isUpstream_, bool isBarrel_, int sector_, int depth_, int verbose )
{
    ( void ) verbose;

    double halfBarrelLength = geomInfo->barrelLength/2.;
    double barrelRadius = geomInfo->barrelRadius;

    TVector3 qqq5RefPos = TVector3 ( 0, 0, halfBarrelLength ) + TVector3 ( 0, ( depth_ - 1 ) * geomInfo->endcapLayerSpacing, 0 ) + geomInfo->qqq5Offset;
    TVector3 sX3RefPos = TVector3 ( 0, barrelRadius, 0 ) + TVector3 ( 0, 0, ( depth_ - 1 ) * geomInfo->barrelLayerSpacing ) + geomInfo->superX3Offset;

    TVector3 detPosVect3 = isBarrel_ ? sX3RefPos : qqq5RefPos;
    SolidVector detPos;
    detPos.SetXYZ ( detPosVect3.X(), detPosVect3.Y(), detPosVect3.Z() );

    if ( isUpstream_ ) detPos.SetTheta ( TMath::Pi() - detPos.Theta() );

    double refPhi = isBarrel_ ? geomInfo->superX3RefPhi : geomInfo->qqq5RefPhi;
    double deltaPhi = isBarrel_ ? geomInfo->superX3DeltaPhi : geomInfo->qqq5DeltaPhi;

    double refRotZ = isBarrel_ ? geomInfo->superX3RefRotz : geomInfo->qqq5RefRotz;
    double deltaRotZ = isBarrel_ ? geomInfo->superX3DeltaRotz : geomInfo->qqq5DeltaRotz;

    detPos.SetPhi ( refPhi + sector_ * deltaPhi );
    detPos.SetRotationZ ( refRotZ + sector_ * deltaRotZ );

    detPos -= geomInfo->targetOffset;

    return detPos;
}

TVector3 GetStripPos ( GoddessGeomInfos* geomInfo, bool isUpstream_, bool isBarrel_, int sector_, int strip_, int depth_, int verbose )
{
    if ( !isBarrel_ )
    {
        TVector3 firstStripPos = GetDetPos ( geomInfo, isUpstream_, isBarrel_, sector_, depth_, verbose );
        double qqq5FstStripWidth = geomInfo->qqq5FirstStripWidth;
        double qqq5DeltaPitch = geomInfo->qqq5DeltaPitch;

        std::vector<double> qqq5BinsEdges;

        std::list<double> binsEdgesList;
        binsEdgesList.clear();

        TVector3 lowEdge = firstStripPos;
        double midDetPhi = lowEdge.Phi();

        TVector3 highEdge = lowEdge;

        for ( int i = 0; i < strip_+1; i++ )
        {
            lowEdge = highEdge;

            TVector3 toNextStrip ( 0, qqq5FstStripWidth - ( i-1 ) *qqq5DeltaPitch , 0 );
            toNextStrip.SetPhi ( midDetPhi );

            highEdge += toNextStrip;
        }

        return ( lowEdge+highEdge ) *0.5;
    }
    else return TVector3 ( 0, 0, 0 );
}

std::vector<double> GetBinsEdges ( GoddessGeomInfos* geomInfo, bool isUpstream_, bool isBarrel_, int sector_, int depth_, int verbose )
{
    TVector3 beamDir ( 0, 0, 1 );

    TVector3 firstStripPos = GetDetPos ( geomInfo, isUpstream_, isBarrel_, sector_, depth_, verbose );
    double qqq5FstStripWidth = geomInfo->qqq5FirstStripWidth;
    double qqq5DeltaPitch = geomInfo->qqq5DeltaPitch;

    std::vector<double> qqq5BinsEdges;

    std::list<double> binsEdgesList;
    binsEdgesList.clear();

    TVector3 lowEdge = firstStripPos;
    double midDetPhi = lowEdge.Phi();

    for ( int i = 0; i < 33; i++ )
    {
        if ( i>0 )
        {
            TVector3 toNextStrip ( 0, qqq5FstStripWidth - ( i-1 ) *qqq5DeltaPitch , 0 );
            toNextStrip.SetPhi ( midDetPhi );

            lowEdge += toNextStrip;
        }

        double thetaEdge = lowEdge.Angle ( beamDir ) * TMath::RadToDeg();

        binsEdgesList.push_back ( thetaEdge );

        for ( int j = 0; j < 4; j++ )
        {
            lowEdge.SetPhi ( midDetPhi - 3./16. * TMath::Pi() + j/8. * TMath::Pi() );

            thetaEdge = lowEdge .Angle ( beamDir ) * TMath::RadToDeg();

            binsEdgesList.push_back ( thetaEdge );
        }

        lowEdge.SetPhi ( midDetPhi );
    }

    binsEdgesList.unique ( CheckValProxFunc ( 0.05 ) );
    binsEdgesList.sort();

    qqq5BinsEdges.clear();

    if ( verbose ) cout << "Filling the final bins edges array of size " << binsEdgesList.size() <<" ...\n";

    int counter = 0;

    for ( auto listItr = binsEdgesList.begin(); listItr != binsEdgesList.end(); listItr++ )
    {
        qqq5BinsEdges.push_back ( *listItr );
        if ( verbose ) cout << "Bin #" << counter << " : " << qqq5BinsEdges[counter] << endl;

        counter++;
    }

    return qqq5BinsEdges;
}

void InsertGeomInfo ( TFile* file, GoddessGeomInfos* geomInfo_, bool overwriteIfExists )
{
    if ( !file->IsOpen() ) return;
    if ( overwriteIfExists )
    {
        file->cd ( "infos" );
        gDirectory->Delete ( "GoddessGeom;*" );
    }

    if ( !overwriteIfExists && file->FindObjectAny ( "GoddessGeom" ) ) return;

    if ( geomInfo_ != nullptr )
    {
        if ( !file->cd ( "infos" ) )
        {
            file->mkdir ( "infos" );
            file->cd ( "infos" );
        }

        geomInfo_->Write ( "GoddessGeom" );
        cout << "Written GoddessGeom to " << file->GetName() << endl;
    }

    file->Close();
}

void InsertGeomInfo ( string files, GoddessGeomInfos* geomInfo_, bool overwriteIfExists )
{
    vector<string> fileList = DecodeItemsToTreat ( files, "system" );

    for ( unsigned int i = 0; i < fileList.size(); i++ )
    {
        TFile* f = new TFile ( fileList[i].c_str(), "update" );

        if ( f->IsOpen() ) InsertGeomInfo ( f, geomInfo_, overwriteIfExists );

//         cout << "Treated: " << fileList[i] << endl;
    }
}

void InsertReacInfo ( TFile* file, GoddessReacInfos* reacInfo_, bool overwriteIfExists )
{
    if ( !file->IsOpen() ) return;
    if ( overwriteIfExists )
    {
        file->cd ( "infos" );
        gDirectory->Delete ( "GoddessReac;*" );
    }

    if ( !overwriteIfExists && file->FindObjectAny ( "GoddessReac" ) ) return;

    if ( reacInfo_ != nullptr )
    {
        if ( !file->cd ( "infos" ) )
        {
            file->mkdir ( "infos" );
            file->cd ( "infos" );
        }

        reacInfo_->Write ( "GoddessReac" );
        cout << "Written GoddessReac to " << file->GetName() << endl;
    }

    file->Close();
}

void InsertReacInfo ( string files, GoddessReacInfos* reacInfo_, bool overwriteIfExists )
{
    vector<string> fileList = DecodeItemsToTreat ( files, "system" );

    for ( unsigned int i = 0; i < fileList.size(); i++ )
    {
        TFile* f = new TFile ( fileList[i].c_str(), "update" );

        if ( f->IsOpen() ) InsertReacInfo ( f, reacInfo_, overwriteIfExists );

//         cout << "Treated: " << fileList[i] << endl;
    }
}

void WriteUserHists ( string outName )
{
    string mode = "recreate";

    TFile* outRootFile = new TFile ( outName.c_str(), "read" );

    int userChoice;

    if ( outRootFile->IsOpen() )
    {
        std::cout << "File " << outName << " already exists...\n";
        std::cout << "Would you like to overwrite it [1] or update it [2]? ";
        std::cin >> userChoice;

        if ( userChoice == 2 )
        {
            mode = "update";
        }
        else if ( std::cin.fail() || userChoice < 1 || userChoice > 2 )
        {
            std::cout << "Invalid input... aborting...\n";
            return;
        }

        outRootFile->Close();

        std::cout << "\n";
    }
    else
    {
        std::cout << "It has now been created...\n";
    }

    outRootFile = new TFile ( outName.c_str(), mode.c_str() );

    for ( auto itr = histsMap.begin(); itr != histsMap.end(); itr++ )
    {
        if ( std::distance ( histsMap.begin(), itr ) %10 == 0 )
        {
            std::cout << "Written " << std::distance ( histsMap.begin(), itr );
            std::cout << " out of " << std::distance ( histsMap.begin(), histsMap.end() ) << " histograms...\r" << std::flush;
        }

        TH1F* h1 = dynamic_cast<TH1F*> ( ( itr->second ).first );
        TH2F* h2 = dynamic_cast<TH2F*> ( ( itr->second ).first );

        if ( h1 != NULL )
        {
            h1->Write();
        }
        else if ( h2 != NULL )
        {
            h2->Write();
        }
    }

    std::cout << std::endl;

    outRootFile->Close();
}

void UpdateUserHists ( string outName )
{
    TFile* outRootFile = new TFile ( outName.c_str(), "update" );

    for ( auto itr = histsMap.begin(); itr != histsMap.end(); itr++ )
    {
        if ( std::distance ( histsMap.begin(), itr ) %10 == 0 )
        {
            std::cout << "Updated " << std::distance ( histsMap.begin(), itr );
            std::cout << " out of " << std::distance ( histsMap.begin(), histsMap.end() ) << " histograms...\r" << std::flush;
        }

        TH1F* h1 = dynamic_cast<TH1F*> ( ( itr->second ).first );
        TH2F* h2 = dynamic_cast<TH2F*> ( ( itr->second ).first );

        if ( h1 != NULL )
        {
            TH1F* toUpdate = ( TH1F* ) outRootFile->Get ( h1->GetName() );

            if ( toUpdate != nullptr )
            {
                h1->Add ( toUpdate, 1 );

                string nameCycle = ( string ) h1->GetName() + ";*";

                outRootFile->Delete ( nameCycle.c_str() );

                h1->Write();
            }
        }
        else if ( h2 != NULL )
        {
            TH2F* toUpdate = ( TH2F* ) outRootFile->Get ( h2->GetName() );

            if ( toUpdate != nullptr )
            {
                h2->Add ( toUpdate, 1 );

                string nameCycle = ( string ) h2->GetName() + ";*";

                outRootFile->Delete ( nameCycle.c_str() );

                h2->Write();
            }
        }
    }

    std::cout << std::endl;

    outRootFile->Close();
}

void ClearUserHists()
{
    for ( auto itr = histsMap.begin(); itr != histsMap.end(); itr++ )
    {
        TH1F* h1 = dynamic_cast<TH1F*> ( ( itr->second ).first );
        TH2F* h2 = dynamic_cast<TH2F*> ( ( itr->second ).first );

        if ( h1 != NULL )
        {
            h1->Reset();
        }
        else if ( h2 != NULL )
        {
            h2->Reset();
        }
    }
}













