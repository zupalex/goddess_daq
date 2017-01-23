#include "GoddessAnalysis.h"

GoddessAnalysis::GoddessAnalysis()
{
    defaultTreeName1 = "raw";
    defaultTreeName2 = "sorted";

    userTree = nullptr;
    userChain = nullptr;

    SetReacParameters();
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

    std::vector<std::string> listOfFiles = DecodeItemsToTreat ( inFile, "system" );

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

TGraph* SuperimposeCalculatedKinematics ( string input )
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
            float labAngle, labEnergy;

            iss >> labAngle >> labEnergy;

            dataPoints.push_back ( std::make_pair ( labAngle, labEnergy ) );
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

TH2F* MakeNewHist ( string name, string title, int nBinsX, double* binsX, unsigned int nBinsY, int minY, int maxY, bool addToSpecialList )
{
    TH2F* newHist = new TH2F ( name.c_str(), title.c_str(), nBinsX, binsX, nBinsY, minY, maxY );
    histsMap[name] = std::make_pair ( newHist, * ( new std::vector<GamData*> ) );

    if ( addToSpecialList ) specialHists.push_back ( name );

    return newHist;
}

void ListHistograms ( string match, unsigned int limit, unsigned int startAt )
{
    std::vector<string> matchingHists;
    matchingHists.clear();

    if ( match.empty() ) match = "*";

    matchingHists = DecodeItemsToTreat ( match, "root" );

    if ( limit == 0 ) limit = 20;

    unsigned int histAmount = gDirectory->GetList()->GetSize();

    unsigned int digitsNumber = 1;

    string userIn;

    while ( ( histAmount/  pow ( 10,digitsNumber ) ) >= 1 ) digitsNumber++;

    if ( startAt >= matchingHists.size() )
    {
        return;
    }

    cout << matchingHists.size() << " matching histograms...\n\n";

DisplayResults:

    for ( unsigned int i = startAt; i < std::min ( ( unsigned int ) matchingHists.size(), startAt+limit ); i++ )
    {
        cout << "-> Histogram #" << std::setw ( digitsNumber ) << i << " : " << matchingHists[i] << endl;
    }

    startAt += limit;

    if ( startAt >= matchingHists.size() )
    {
        return;
    }

UserPrompt:

    cout << "Continue = Enter / Quit = q : ";

    std::getline ( std::cin, userIn );

    if ( userIn == "q" || userIn == "quit" ) return;
    else if ( userIn.empty() ) goto DisplayResults;
    else
    {
        cerr << "Invalid input...\n";
        goto UserPrompt;
    }

    return;
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

TVector3 GetDetPos ( TTree* tree, bool isUpstream_, bool isBarrel_, int sector_, int verbose )
{
    string treeName = tree->GetName();
    string fileName = ( tree->GetCurrentFile() != nullptr ) ? tree->GetCurrentFile()->GetName() : "";

    if ( fileName.empty() ) return TVector3 ( 0, 0, 0 );

    TChain* c = new TChain ( treeName.c_str(), treeName.c_str() );

    c->Add ( fileName.c_str() );

    return GetDetPos ( c, isUpstream_, isBarrel_, sector_, verbose );
}














