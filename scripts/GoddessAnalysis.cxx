#include "GoddessAnalysis.h"

GoddessAnalysis::GoddessAnalysis()
{
    currentTreeName1 = "raw";
    currentTreeName2 = "sorted";
}

GoddessAnalysis::GoddessAnalysis ( std::string filename ) : GoddessAnalysis()
{
    currentfilename = filename;
}

GoddessAnalysis::GoddessAnalysis ( std::string filename, std::string treename, std::string treename2 ) : GoddessAnalysis()
{
    currentfilename = filename;
    currentTreeName1 = treename;
    currentTreeName2 = treename2;
}


GoddessAnalysis::~GoddessAnalysis()
{

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
        bool read = false;

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
    DrawEnergyVsAngleSX3 ( chain, nentries, hname, nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY, drawOpts, isUpstream_, strips, sector );
}

void ReadDetectorID ( std::string DetectorID, bool* isUpstreamRes, unsigned short* numsector, bool* detectors, int* layernum, bool* side, unsigned short* sx3 )
{
    std::string str ( DetectorID );
    std::string IDtype;
    std::stringstream ss ( str );

    std::vector <std::string> ID;

    while ( ss >> IDtype )
        ID.push_back ( IDtype );

    std::string UorD = ID[1].substr ( 0,1 );
    std::string sector = ID[1].substr ( 1 );
    std::string currentTreeName1;
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
            *detectors=true;
            *layernum=1;
            *numsector = stoi ( sector );
            *isUpstreamRes = false;
            *sx3=1;

        }

        else if ( det == "SX3" )
        {
            *side = true;
            *numsector = stoi ( sector );
            *layernum=2;
            *detectors=true;
            *sx3=0;
            if ( UorD == "U" )
            {

                *isUpstreamRes = true;
                //cout << "Upstream ";
            }
            else
            {

                *isUpstreamRes = false;
                //cout << "Downstream ";
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
        int ulength = ID[1].length();
        std::string upstream = ID[1];
        /*
                if ( ID[1][0] == 'U' )
                {CheckMapping("run68.root","raw","sorted",81,88,true,"SX3 U3")
                    cout << "Upstream ";
                }
                if ( ID[1][0] == 'D' )
                {
                    cout << "Downstream ";
                }
                if ( ulength == 2 )

                    if ( ID[1][1] == 'A' )
                    {
                        cout << "Sector 0" << endl;
                    }
                    else if ( ID[1][1] == 'B' )
                    {
                        cout << "Sector 1" << endl;
                    }
                    else if ( ID[1][1] == 'C' )
                    {
                        cout << "Sector 2" << endl;
                    }
                    else if ( ID[1][1]== 'D' )
                    {
                        cout << "Sector 3" << endl;
                    }
                    else
                    {
                        cout << "Sector " << ID[1][1] << endl;
                    }
                }
                if ( ulength == 3 )
                {
                    cout << "Sector " << ID[1][1] << ID[1][2] << endl;
                }*/
        std::string detlayer = ID[2].substr ( 0 );
        std::string detside = ID[3].substr ( 0 );

        if ( UorD == "U" )
        {

            *isUpstreamRes = true;
            //cout << "Upstream ";
        }
        else
        {

            *isUpstreamRes = false;
            //cout << "Downstream ";
        }
        if ( sector == "A" )
        {
            *numsector = 0;
            //cout << "Sector 0" << endl;
        }
        else if ( sector == "B" )
        {
            *numsector = 1;
            //cout << "Sector 1" << endl;
        }
        else if ( sector == "C" )
        {
            *numsector = 2;
            //cout << "Sector 2" << endl;
        }
        else if ( sector == "D" )
        {
            *numsector = 3;
            //cout << "Sector 3" << endl;
        }
        else
        {
            *numsector = stoi ( sector );
            // cout << "Sector " << sector << endl;
        }

        if ( det == "SX3" || det == "BB10" )
        {
            *detectors=true;
        }
        else if ( det == "QQQ5" )
        {
            *detectors=false;
        }
        else
        {
            std::cout<<"Unknown detector named"<<std::endl;

            return;
        }
        if ( ID[2] == "dE" )
        {
            *layernum =1;
        }
        if ( ID[2] == "E1" )
        {
            *layernum =2;
        }
        if ( ID[2] == "E2" )
        {
            *layernum =3;
        }

        if ( detside == "p" )
        {
            *side = true;
        }

        if ( detside =="n" )
        {
            *side = false;
        }
        if ( det=="SX3" )
        {
            *sx3=0;
        }
        else
        {
            *sx3=1;
        }

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

//     if (Digital == true)
//     {
//       binnum = 7000;
//       starthere = 0;
//       endhere = 70000;
//     }
//     else
//     {
//       binnum = 400;
//       starthere = 0;
//       endhere = 4000;
//     }

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

//         for ( unsigned int m=0; m<randomnum.size(); m++ )
//     {
//         cout << ' ' << randomnum.at ( m );
//         cout << '\n';
//     }

    bool Upstream;
    unsigned short sectornum;
    bool front;
    int layer;
    bool detector;
    unsigned short sxx3;

    ReadDetectorID ( DetectorID, &Upstream, &sectornum, &detector, &layer, &front, &sxx3 );

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
            if ( treevar2->at ( w ).isUpstream != Upstream )
            {
                continue;
            }

            if ( treevar2->at ( w ).isBarrel != detector )
            {
                continue;
            }

            if ( treevar2->at ( w ).sector != sectornum )
            {
                continue;
            }

            if ( layer == 1 )
            {
                if ( front == true )
                {
                    sortedsivalue = treevar2->at ( w ).dE.en.p;


                }
                if ( front == false )
                {
                    sortedsivalue = treevar2->at ( w ).dE.en.n;


                }
            }
            if ( layer == 2 )
            {
                if ( front == true )
                {

                    sortedsivalue = treevar2->at ( w ).E1.en.p;

                    std::vector<float> valuen;

                    valuen = treevar2->at ( w ).E1.en.n;
                    int sizen = valuen.size();

                    if ( sxx3 == 0 )
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
                if ( front == false )
                {

                    std::vector<float> valuen;

                    valuen = treevar2->at ( w ).E1.en.n;
                    int sizen = valuen.size();
                    if ( sxx3 == 0 )
                    {
                        for ( int stripnevent=0; stripnevent<sizen; stripnevent++ )
                        {
                            if ( treevar2-> at ( w ).E1.strip.n[stripnevent]!= -1 )
                            {
                                sortedsivalue.push_back ( valuen[stripnevent] );
                            }

                        }
                    }
                    if ( sxx3 == 1 )
                    {
                        sortedsivalue = treevar2->at ( w ).E1.en.n;
                    }

                }
            }
            if ( layer == 3 )
            {
                if ( front == true )
                {
                    sortedsivalue = treevar2->at ( w ).E2.en.p;


                }
                if ( front == false )
                {
                    sortedsivalue = treevar2->at ( w ).E2.en.n;


                }
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

// void GoddessAnalysis::CheckMapping ( short unsigned int channel1, short unsigned int channel2, bool Digital, string DetectorID )
// {
//
//     TList* dirlist = gDirectory->GetListOfKeys();
//     string filename = dirlist->First()->GetName();
//
//     CheckMapping ( filename, currentTreeName1, currentTreeName2, channel1, channel2, Digital, DetectorID );
//
//
//
// }

void GoddessAnalysis::CheckMapping ( short unsigned int channel1, short unsigned int channel2, bool Digital, std::string DetectorID )
{
    if ( currentfilename.empty() )
    {
        if ( gDirectory->GetFile() ==NULL )
        {
            return;
        }
        else
        {
            currentfilename = gDirectory->GetFile()->GetName();
        }
    }

    CheckMapping ( currentfilename, currentTreeName1, currentTreeName2, channel1, channel2, Digital, DetectorID );



}




ClassImp ( GoddessAnalysis )
