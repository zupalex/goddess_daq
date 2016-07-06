#include "GoddessAnalysis.h"

GoddessAnalysis::GoddessAnalysis()
{
    defaultTreeName1 = "raw";
    defaultTreeName2 = "sorted";
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
    if(fileName.length() > 0) defaultFileName = fileName;
    if(treeName1.length() > 0) defaultTreeName1 = treeName1;
    if(treeName2.length() > 0) defaultTreeName2 = treeName2;
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

ClassImp ( GoddessAnalysis )
