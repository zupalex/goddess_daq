#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "TMath.h"
#include "TPolyMarker.h"
#include "TSpectrum.h"
#include "goddess_analysis_macros.h"
#include "GoddessAnalysis.h"
#include "Sort_TimeDif.h"

using namespace std;


void CheckBSMapping ( string filename, string treename, int sector, bool isupstream, bool isbarrel, int bins = 400, float histostop=2500 )
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

    string sectorid;
    if ( sector-10<0 )
    {
        sectorid = "0" + to_string ( sector );
    }
    else
    {
        sectorid = to_string ( sector );
    }

    string outfilename = "Mapping_Backsides_Check_Detector_" + to_string ( isbarrel +1 ) + to_string ( isupstream +1 ) + sectorid + ".root";

    TFile* file = new TFile ( outfilename.c_str(),"update" );

    int entries = tree->GetEntries();

    TH2F* hist_array[12];


    for ( int g=0; g<12; g++ )
    {
        string name = "BacksideDetector" + to_string ( g );

        TH2F* histo = new TH2F ( name.c_str(),"Check Mapping for Backside",bins,0,histostop,bins,0,histostop );

        hist_array[g] = histo;

    }

    TH2F* histofull = new TH2F ( "NoBacksideDetector","Check Mapping for Backside", bins, 0, histostop, bins, 0, histostop );



    vector<SiDataDetailed>* SiData = new vector<SiDataDetailed>();
    tree->SetBranchAddress ( "si", &SiData );

    bool issector = false;
    int elementnum;

    for ( int entry=0; entry<entries; entry++ )
    {
        tree->GetEntry ( entry );

        if ( entry%20000 == 0 )
        {
            cout << "Treated entry sorted # " << entry << " / " << entries << "\r" << flush;
        }

        if ( SiData->size() ==0 )
        {
            continue;
        }

        for ( int r = 0; r<SiData->size(); r++ )
        {
            SiDataDetailed* sidatadetailed = &SiData->at ( r );

            if ( SiData->size() >1 && sidatadetailed->isBarrel == isbarrel && sidatadetailed->isUpstream == isupstream && sidatadetailed->sector == sector )
            {
                issector = true;
                elementnum = r;

                for ( int f = 0; f<sidatadetailed->E1.strip.n.size(); f++ )
                {
                    if ( sidatadetailed->E1.strip.n.at ( f ) == -1 )
                    {
                        histofull->Fill ( sidatadetailed->E1.en.n.at ( f ),sidatadetailed->E1.en.p.at ( f ) );
                    }
                }
            }
            //cout<<"Sector "<<sector<<" is in element: "<<elementnum<<endl;
            else
            {
                continue;
            }
        }


        if ( !issector ) continue;

        for ( int k=0; k<SiData->size(); k++ )
        {
            SiDataDetailed* sidatapt = &SiData->at ( k );

            if ( sidatapt->E1.strip.n.size() >sidatapt->E1.strip.p.size() )
            {
//                 cout<<"There is something in the backside of element: "<<k<<endl;

                for ( int u = 0; u<SiData->size(); u++ )
                {
                    if ( u==elementnum )
                    {
                        for ( int q = 0; q<sidatapt->E1.strip.n.size(); q++ )
                        {
//                      cout<<"For sector "<<sidatapt->sector<<" the strip number is listed as:"<<sidatapt->E1.strip.n.at ( q ) <<endl;
                            if ( sidatapt->E1.strip.n.at ( q ) == -1 )
                            {
                                hist_array[sidatapt->sector]->Fill ( sidatapt->E1.en.n.at ( q ),sidatapt->E1.en.p.at ( q ) );
                            }
                        }
                    }
                }
            }
        }


        issector = false;
    }

    for ( int p = 0; p<12; p++ )
    {
        hist_array[p]->Write();
    }

    file->Close();

    return;
}

double gaus_fit_peak ( double *x, double *par )
{

    double gauss= par[1]*TMath::Exp ( -pow ( x[0]- ( par[2] ),2 ) / ( 2*pow ( par[0],2 ) ) );

    return gauss;
}


void QQQ5Calib ( string filename, string newfilename, bool isupstream, bool is2015 )
{
    TFile* originalfile = new TFile ( filename.c_str(),"read" );


    TTree* sorted = ( TTree* ) originalfile->Get ( "sorted" );

    if ( sorted == NULL )
    {

        sorted = ( TTree* ) originalfile->Get ( "trees/sorted" );

        if ( sorted == NULL )
        {
            sorted = ( TTree* ) originalfile->Get ( "god" );
	    
	    if (sorted == NULL)
	    {
	      sorted = ( TTree* ) originalfile->Get ( "trees/god" );
	      
	      if (sorted == NULL) return;
	    }
	    
        }
    }

    TH2F* dig_hist1;
    TH2F* dig_hist2;
    TH2F* an_hist1;
    TH2F* an_hist2;

    if ( is2015 )
    {
        dig_hist1 = new TH2F ( "qqq5_hist_1","QQQ5 En vs Strip", 32,0,32,400,0,10 );
        dig_hist2 = new TH2F ( "qqq5_hist_2","QQQ5 En vs Strip", 32,0,32,400,0,10 );
        an_hist1 = new TH2F ( "qqq5_hist_3","QQQ5 En vs Strip", 32,0,32,400,0,10);
        an_hist2 = new TH2F ( "qqq5_hist_4","QQQ5 En vs Strip", 32,0,32,400,0,10 );
    }

    else if ( !is2015 )
    {
        dig_hist1 = new TH2F ( "qqq5_hist_1","QQQ5 En vs Strip", 32,0,32,400,200,5000 ); //these two are set up like the analog detectors
        dig_hist2 = new TH2F ( "qqq5_hist_2","QQQ5 En vs Strip", 32,0,32,400,200,5000 ); //they're only not named that way for ease of use with the 2015 campaign
        an_hist1 = new TH2F ( "qqq5_hist_3","QQQ5 En vs Strip", 32,0,32,400,200,5000 );
        an_hist2 = new TH2F ( "qqq5_hist_4","QQQ5 En vs Strip", 32,0,32,400,200,5000 );
    }

    vector<TH2F*> hists;

    int entries = sorted->GetEntries();

    vector<SiDataDetailed>* vectdata = new vector<SiDataDetailed>;
    sorted->SetBranchAddress ( "si",&vectdata );

    for ( int i = 0; i<entries; i++ )
    {
        sorted->GetEntry ( i );

        if ( i%2000==0 )
        {
            cout<<"Entries treated: "<<i<<"/"<<entries<<"\r"<<flush;
        }

        for ( int vdat = 0; vdat<vectdata->size(); vdat++ )
        {
            SiDataDetailed* dat = &vectdata->at ( vdat );

            if ( dat->isUpstream == isupstream && dat->isBarrel == 0 )
            {
                if ( dat->sector == 0 )
                {
                    for ( int striphit = 0; striphit<dat->E1.strip.p.size(); striphit++ )
                    {
                        dig_hist1->Fill ( dat->E1.strip.p.at ( striphit ),  dat->E1.en.p.at ( striphit ) );

                    }
                }

                else if ( dat->sector == 1 )
                {
                    for ( int striphit2 = 0; striphit2<dat->E1.strip.p.size(); striphit2++ )
                    {
                        dig_hist2->Fill ( dat->E1.strip.p.at ( striphit2 ), dat->E1.en.p.at ( striphit2 ) );

                    }
                }
                else if ( dat->sector == 2 )
                {
                    for ( int striphit3 = 0; striphit3<dat->E1.strip.p.size(); striphit3++ )
                    {
                        an_hist1->Fill ( dat->E1.strip.p.at ( striphit3 ), dat->E1.en.p.at ( striphit3 ) );

                    }
                }
                else if ( dat->sector == 3 )
                {
                    for ( int striphit4 = 0; striphit4<dat->E1.strip.p.size(); striphit4++ )
                    {
                        an_hist2->Fill ( dat->E1.strip.p.at ( striphit4 ), dat->E1.en.p.at ( striphit4 ) );

                    }
                }
            }


        }
    }

    TFile* newfile = new TFile ( newfilename.c_str(),"recreate" );

    dig_hist1->Write();
    dig_hist2->Write();
    an_hist1->Write();
    an_hist2->Write();

    newfile->Write();
    newfile->Close();



    return;
}

void QQQ5Project ( string originalfile, string newfilename, string isUpstream = "U" )
{
    TFile* file = new TFile ( originalfile.c_str(),"read" );
    TList* list = file->GetListOfKeys();
    if ( !list )
    {
        cout<<"No keys in file."<<endl;
    }

    TIter next ( list );
    TKey* key;
    TObject* obj;

    TH1* projection;
    TH2* hist;
    vector<TH1*> histvec;
    string slice;
    int b = 0;

    while ( ( key = ( TKey* ) next() ) )
    {
        obj = key->ReadObj();
        if ( obj->InheritsFrom ( "TH2" ) )
        {
            hist = ( TH2* ) obj;

            for ( int strip = 1; strip<33; strip++ )
            {
                if ( strip<10 ) slice = isUpstream + to_string ( b ) + "0"+ to_string ( strip );
                if ( strip>=10 ) slice = isUpstream + to_string ( b ) + to_string ( strip );

                projection = hist ->ProjectionY ( slice.c_str(),strip,strip );

                histvec.push_back ( projection );

            }
        }

        b++;
    }

    TFile* newfile = new TFile ( newfilename.c_str(),"recreate" );

    for ( int k = 0; k<histvec.size(); k++ )
    {
        histvec.at ( k )->Write();
    }

    newfile->Write();
    newfile->Close();

    return;
}


void QQQ5Fits ( string filename, string newfilename, bool isupstream = 1 )
{

    TFile* file = new TFile ( filename.c_str(),"read" );
    TList* list = file->GetListOfKeys();
    if ( !list )
    {
        cout<<"No keys in file."<<endl;
    }

    TIter next ( list );
    TKey* key;
    TObject* obj;

    TF1* fit_dig = new TF1 ( "fit_func",gaus_fit_peak,120000,180000,3 );
    TF1* fit_an = new TF1 ( "fit_func",gaus_fit_peak,2500,4000,3 );
    vector<Double_t> amps;
    vector<TH1*> peaksfound;
    //TSpectrum *spec;

    int b = 0;
    TH1* histo;
    TList *functions;
    TPolyMarker *pm;
    float amp;
    Double_t* ypeaks;
    Double_t* xpeaks;
    int name_prev = 0;
    string name_curr;
    int name_num;
    TH1D* notfound = new TH1D ( "empty","empty",400,0,10 );
    int skip=1;
    vector<float> afromfit;

    while ( ( key= ( TKey* ) next() ) )
    {
        obj = key->ReadObj();
        if ( obj->InheritsFrom ( "TH1" ) )
        {
            histo = ( TH1* ) obj;
            name_curr = ( string ) histo->GetName();
            name_curr = name_curr.substr ( name_curr.length()-1 );
            name_num = stoi ( name_curr );
            if ( name_num == 0 && name_prev == 9 )
            {
                skip=0;
            }
            //cout<<"prev: "<<name_prev<<" curr: "<<name_num<<endl;
            if ( name_num != name_prev+1 || skip==0 )
            {
                for ( int hm = 0; hm<name_num-name_prev+1; hm++ )
                {
                    peaksfound.push_back ( notfound );
                    afromfit.push_back ( 1 );
                    amps.push_back ( 1 );
                }
            }
            skip = 1;
            name_prev = name_num;
            histo->ShowPeaks ( 2,"",0.4 );
            functions = histo->GetListOfFunctions();
            pm= ( TPolyMarker* ) functions->FindObject ( "TPolyMarker" );
            if ( pm == NULL )
            {
                amps.push_back ( 1 );
                afromfit.push_back ( 1 );
                peaksfound.push_back ( histo );
                continue;
            }
            ypeaks = pm->GetY();
            xpeaks = pm->GetX();

            if ( xpeaks[0]<10000 )
            {
                fit_an->SetParameters ( 1,ypeaks[0],xpeaks[0] );
                histo->Fit ( fit_an,"M","",xpeaks[0]-50,xpeaks[0]+40 );
                afromfit.push_back ( fit_an->GetParameter ( 2 ) );
            }
            else if ( xpeaks[0]>10000 )
            {
                fit_dig->SetParameters ( 1,ypeaks[0],xpeaks[0] );
                histo->Fit ( fit_dig,"M","",xpeaks[0]-2000,xpeaks[0]+2000 );
                afromfit.push_back ( fit_dig->GetParameter ( 2 ) );
            }
            else
            {
                afromfit.push_back ( 1 );
            }

            histo->Draw();
            amps.push_back ( xpeaks[0] );
            peaksfound.push_back ( histo );

        }


        b++;
    }

    Double_t lamp1 = 1;
    Double_t lamp2 = 1;
    Double_t lamp3 = 1;
    Double_t lamp4 = 1;
    Double_t corr;

    for ( int n = 0; n<amps.size(); n++ )
    {
        if ( n<32 && amps[n]>lamp1 ) lamp1 = amps[n];
        if ( n<64 && amps[n]>lamp2 ) lamp2 = amps[n];
        if ( n<96 && amps[n]>lamp3 ) lamp3 = amps[n];
        if ( n<128 && amps[n]>lamp4 ) lamp4 = amps[n];
    }

    vector<Double_t> corrections;
    vector<int> todo;

    for ( int c = 0; c<amps.size(); c++ )
    {
        if ( amps[c] == 1 )
        {
            corrections.push_back ( 0 );
            continue;
        }
        corrections.push_back ( 5.813/amps[c] );
        if ( c<64 && corrections[c]>0.0001 ) todo.push_back ( c );
        if ( c>=64 && corrections[c]>0.01 ) todo.push_back ( c );
    }
    cout<<"Do these by hand: "<<endl;
    int sector;
    int strip;

    for ( int td = 0; td<todo.size(); td++ )
    {
        sector = todo[td]/32;
        strip = todo[td] - 32*sector;

        cout<<"Sector: "<<sector<<"    Strip: "<<strip<<endl;

    }

    TFile* newfile = new TFile ( newfilename.c_str(),"recreate" );

    for ( int ff = 0; ff<peaksfound.size(); ff++ )
    {
        peaksfound[ff]->Write();
    }

    newfile->Write();
    newfile->Close();

    ofstream txtfile;
    string txtname = "QQQ5_Calibrations_isupstream_"+to_string ( isupstream ) +".txt";
    txtfile.open ( txtname.c_str() );
    txtfile<<"Calibrations for the QQQ5"<<endl;
    txtfile<<"Detector,Mean_by_polymarker,Correction_by_polymarker,Mean_by_fit,Correction_by_fit"<<endl;
    float correction_fit;
    for ( int h = 0; h<amps.size(); h++ )
    {
        if ( afromfit[h] == 1 )
        {
            correction_fit = 1;
        }
        else correction_fit = 5.813/afromfit[h];
        txtfile<<peaksfound[h]->GetName() <<","<<amps[h]<<","<<corrections[h]<<","<<afromfit[h]<<","<<correction_fit<<endl;;
    }

    txtfile.close();

    return;
}

void QQQ5UpdateConfig ( string calib_file, string config_file )
{
    ifstream qqq5calib ( calib_file.c_str() );
    string line;

    int pos_val;
    string val;
    vector<string> vals;
    string isUpstream;
    vector<string> isU;
    string sector;
    vector<string> sectors;
    string strip;
    vector<string> strips;
    int b = 0;
    int sec;
    int str;

    while ( getline ( qqq5calib,line ) )
    {
        b++;
        if ( b < 3 ) continue;
        if ( line.substr ( 0,5 ) == "empty" )
        {
            if ( strip == "32" )
            {
                strip = "0";
                sec = stoi ( sector ) + 1;
            }
            else
            {
                sec = stoi ( sector );
                str = stoi ( strip ) + 1;
            }

            sectors.push_back ( to_string ( sec ) );
            strips.push_back ( to_string ( str ) );
            isU.push_back ( "E" );
            vals.push_back ( "1" );
            continue;
        }
        isUpstream = line.substr ( 0,1 );
        isU.push_back ( isUpstream );
        sector = line.substr ( 1,1 );
        sectors.push_back ( sector );
        strip = line.substr ( 2,2 );
        strips.push_back ( strip );
        pos_val = line.find_last_of ( "," );
        val = line.substr ( pos_val+1 );
        //cout<<isUpstream<<" "<<sector<<" "<<strip<<endl;
        vals.push_back ( val );

    }

    ifstream config ( config_file.c_str() );

    string det;
    int pos;
    string layer;
    bool flag=0;
    string encal;
    string v_config;
    int num;
    int snum;
    string replacement;
    int sstrip;

    int cpos = config_file.find ( ".config" );
    string csub = config_file.substr ( 0,cpos );
    csub = csub + "_new.config";

    ofstream fileout ( csub.c_str() );


    while ( getline ( config,line ) )
    {
        if ( flag == 0 )
        {
            det = line.substr ( 0,4 );

            if ( det == "QQQ5" )
            {
                pos = line.find ( "U" );
                if ( pos == -1 )
                {
                    pos = line.find ( "D" );
                    if ( pos == -1 )
                    {
                        cout<<"Not Upstream or Downstream. Error."<<endl;
                    }
                }
                isUpstream = line.substr ( pos,1 );
                sector = line.substr ( pos+1,1 );
                if ( sector == "A" ) sector = "0";
                else if ( sector == "B" ) sector ="1";
                else if ( sector == "C" ) sector = "2";
                else if ( sector == "D" ) sector = "3";
                else
                {
                    cout<<"Sector is not viable."<<endl;
                }

                layer = line.substr ( pos+3,2 );
                if ( layer == "E1" )
                {
                    flag = 1;
                }
            }
        }

        if ( flag == 1 )
        {
            encal = line.substr ( 0,7 );
            if ( encal == "enCal n" )
            {
                flag = 0;
            }
            if ( encal == "enCal p" )
            {

                strip = line.substr ( 8,2 );
                num = stoi ( strip ) +1;
                strip = to_string ( num );
                if ( stoi ( strip ) <10 )
                {
                    strip = "0"+strip.substr ( 0,1 );
                }

                num = line.find_last_of ( " " );
                v_config = line.substr ( num+1 );
                snum = stoi ( sector );
                sstrip = stoi ( strip );

                //cout<<"Strip from calib "<<strips[snum*32+s]<<" Strip from config "<<strip<<endl;
                if ( isU[snum*32+sstrip-1] == isUpstream && sectors[snum*32+sstrip-1] ==  sector  && strips[snum*32+sstrip-1] == strip )
                {
                    replacement = vals[snum*32+sstrip-1];
                    //cout<<line<<endl;
                    line.replace ( line.begin() +num+1,line.end(),replacement.begin(),replacement.end() );
                    //cout<<line<<endl;
                }
                else
                {
                    continue;
                    //cout<<strips[snum*32+sstrip-1]<<" "<<strip<<" "<<sectors[snum*32+sstrip-1]<<" "<<sector<<" "<<isUpstream<<" "<<isU[snum*32+sstrip-1]<<endl;
                }

            }
        }
        line += "\n";
        fileout<<line;
    }




    return;
}


void NearvFar ( string filenames, string newname, string treename, bool isbarrel, bool isupstream, bool coinbacksides = false, bool position = false )
{
    vector<string> runs = DecodeItemsToTreat ( filenames,"system" );

    TChain* chain = new TChain ( treename.c_str(),treename.c_str() );
    for ( int h = 0; h<runs.size(); h++ )
    {
        chain->Add ( runs[h].c_str() );
    }

    string newfilename = "NearvFar_tree_"+treename+"_withbacksides_" + to_string ( coinbacksides ) +"_"+to_string ( position ) +"_"+newname;

    TFile* newfile = new TFile ( newfilename.c_str(),"recreate" );

    int entries = chain->GetEntries();

    vector<TH2F*> vecthists;
    TH2F* hist;
    string histname;
    string histname2;
    vector<TH2F*> posvect;
    TH2F* poshist;


    for ( int i = 0; i<48; i++ )
    {
        int sector = i/4;
        int strip = i-sector*4;

        histname = "Energy_Sector"+to_string ( sector ) +"strip"+to_string ( strip );

        if ( treename=="god" )
        {
            hist = new TH2F ( histname.c_str(),"Near v. Far",400,0,200000,400,0,200000 );

            if ( position )
            {
                histname2 = "Position_Sector"+to_string ( sector ) +"strip"+to_string ( strip );
                poshist = new TH2F ( histname2.c_str(),"Position",400,-1,1,400,0,10 );
            }
        }
        if ( treename=="sorted" )
        {
            if ( sector>0 && sector<6 )
            {
                hist = new TH2F ( histname.c_str(),"Near v. Far", 800,0,250000,800,0,250000 );
            }
            else
            {
                hist = new TH2F ( histname.c_str(),"Near v. Far",400,0,2500,400,0,2500 );
            }
        }

        vecthists.push_back ( hist );
        posvect.push_back ( poshist );



    }

    vector<SiDataDetailed>* vectdata = new vector<SiDataDetailed>;
    chain->SetBranchAddress ( "si",&vectdata );

    for ( int entry = 0; entry<entries; entry++ )
    {
        chain->GetEntry ( entry );

        if ( entry%2000==0 )
        {
            cout<<"Entries treated: "<<entry<<"/"<<entries<<"\r"<<flush;
        }

        if ( vectdata->size() ==0 )
        {
            continue;
        }

        for ( int r = 0; r<vectdata->size(); r++ )
        {
            //cout<<"============================================="<<endl;
            //cout<<vectdata->size()<<endl;

            SiDataDetailed* sidat = &vectdata->at ( r );

            for ( int hist = 0; hist<48; hist++ )
            {
                int sectorid = hist/4;
                int stripid = hist - sectorid*4;

                if ( sidat->isBarrel == isbarrel && sidat->isUpstream == isupstream && sidat->sector == sectorid && sidat->E1.strip.p.size() >0 )
                {
// 		  cout<<"******************************************************"<<endl;
// 		  cout<<sectorid<<"is the sector"<<endl;
// 		  cout<<"strip.p "<<sidat->E1.strip.p.size()<<endl;
// 		  cout<<"strip.n "<<sidat->E1.strip.n.size()<<endl;
// 		  cout<<"en.p "<<sidat->E1.en.p.size()<<endl;
// 		  cout<<"en.n "<<sidat->E1.en.n.size()<<endl;

                    if ( coinbacksides == true )
                    {
                        if ( sidat->E1.strip.n.size() <=sidat->E1.strip.p.size() )
                        {
                            continue;
                        }
                    }

                    if ( sidat->E1.strip.p.size() !=sidat->E1.en.p.size() )
                    {
                        continue;
                    }

                    for ( int striphit = 0; striphit<sidat->E1.strip.p.size(); striphit++ )
                    {
// 		      cout<<"-----------------------------------------------------------------------"<<endl;
// 		      cout<<"Entry in E1.strip.p is "<<striphit<<endl;
                        if ( sidat->E1.strip.n.at ( striphit ) == -1 &&  sidat->E1.strip.p.at ( striphit ) == stripid )
                        {
// 			  cout<<"E1.strip.n at striphit is"<<sidat->E1.strip.n.at(striphit)<<endl;
// 			  cout<<"In strip number: "<<sidat->E1.strip.p.at(striphit)<<endl;
// 			  cout<<"energy n "<<sidat->E1.en.n.at(striphit)<<endl;
// 			  cout<<"energy p "<<sidat->E1.en.p.at(striphit)<<endl;

                            vecthists[hist]->Fill ( sidat->E1.en.n.at ( striphit ), sidat->E1.en.p.at ( striphit ) );
                            if ( position )
                            {
                                posvect[hist]->Fill ( ( sidat->E1.en.p.at ( striphit )-sidat->E1.en.n.at ( striphit ) ) / ( sidat->E1.en.n.at ( striphit ) + sidat->E1.en.p.at ( striphit ) ),sidat->E1.en.n.at ( striphit ) + sidat->E1.en.p.at ( striphit ) );
                            }
                        }
                    }
                }
            }

        }

    }

    for ( int k = 0; k<48; k++ )
    {
        vecthists[k]->Write();
        if ( position )
        {
            posvect[k]->Write();
        }

    }

    newfile->Close();

    return;
}


void ExvAngle ( string filenames, string namefile, string treename, vector<string> detectortype, vector<bool> isupstream, vector<bool> isbarrel, int layer, vector<int> sx3_strips_set = {49}, vector<int> qqq5_sec_set= {5}, bool back = false )
{
    vector<string> rootfiles = DecodeItemsToTreat ( filenames, "system" );

    if ( rootfiles.size() ==0 )
    {
        return;
    }

    TChain* chain = new TChain ( treename.c_str(),treename.c_str() );

    for ( int file =0; file < rootfiles.size(); file++ )
    {
        chain->Add ( rootfiles[file].c_str() );
    }

    string newfilename = "ExvAngle_"+namefile;

    TFile* newfile = new TFile ( newfilename.c_str(),"recreate" );

    int entries = chain->GetEntries();

    TH2F* sx3hist = new TH2F ( "SX3_hist","Excitation Energ v. Angle SX3",800,0,180,800,-40,40 );
    TH2F* qqq5hist = new TH2F ( "QQQ5_hist","Excitation Energy v. Angle QQQ5",800,0,180,800,-40,40 );
    TH2F* tothist = new TH2F ( "tot_hist","Excitation Energy v. Angle", 800,0,180,800,-40,40 );

    vector<SiDataDetailed>* vectordata = new vector<SiDataDetailed>;
    chain->SetBranchAddress ( "si", &vectordata );

    GoddessReacInfos* reacinfo = new GoddessReacInfos();

    TVector3 beamdir ( 0,0,1 );


    //     vector<GamData>* gam = new vector<GamData>;
    //     tree->SetBranchAddress("gam",&gam);

    for ( int entry = 0; entry<entries; entry++ )
    {
        chain->GetEntry ( entry );

        if ( entry%2000 ==0 )
        {
            cout<<"Entries treated: "<<entry<<"/"<<entries<<"\r"<<flush;
        }

        if ( vectordata->size() == 0 /*&& gam->size()>0*/ )
        {
            // 	  cout<<gam->size()<<endl;
            //             continue;
        }

        for ( int j = 0; j<vectordata->size(); j++ )
        {
            SiDataDetailed* sidata = &vectordata->at ( j );

            //cout<<"--------------------------------------------"<<endl;
            //cout<<vectordata->size()<<endl;

            for ( int t = 0; t<detectortype.size(); t++ )
            {
                if ( sidata->E1.strip.p.size() !=sidata->E1.en.p.size() )
                {
                    continue;
                }

                if ( detectortype.at ( t ) =="SX3" )
                {
                    if ( sx3_strips_set[0] == 49 )
                    {
                        continue;
                    }

                    for ( int sxthing = 0; sxthing<sx3_strips_set.size(); sxthing++ )
                    {
                        for ( int k = 0; k<48; k++ )
                        {
                            if ( k != sx3_strips_set[sxthing] )
                            {
                                continue;
                            }

                            int sectorid = k/4;
                            int stripid = k-sectorid*4;

                            for ( int sxthing =0; sxthing<sx3_strips_set.size(); sxthing++ )
                            {

                                if ( sidata->sector == sectorid && sidata->isUpstream == isupstream.at ( t ) && sidata->isBarrel == isbarrel.at ( t ) && sidata->E1.strip.p.size() >0 )
                                {
                                    for ( int pent = 0; pent<sidata->E1.strip.p.size(); pent++ )
                                    {
                                        if ( sidata->E1.strip.p.at ( pent ) == stripid )
                                        {

                                            //cout<<"========================================================"<<endl;

                                            // cout<<"energy "<<toten<<endl;
                                            float angle = sidata->Angle ( layer );
                                            float qval = SiDataBase::QValue ( reacinfo, sidata->ESumLayer ( 1, false ) * reacinfo->sX3EnGain, sidata->PosE1().Angle ( beamdir ) ); //95,755,2,1,96);
                                            //cout<<"angle "<<angle<<endl;
                                            float fqval = reacinfo->qValGsGs-qval;
                                            sx3hist->Fill ( angle, fqval );
                                            tothist->Fill ( angle,fqval );
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if ( detectortype.at ( t ) == "QQQ5" )
                {
                    if ( qqq5_sec_set[0] == 5 )
                    {
                        continue;
                    }

                    for ( int qthing = 0; qthing<qqq5_sec_set.size(); qthing++ )
                    {
                        for ( int y = 0; y<4; y++ )
                        {
                            if ( qqq5_sec_set[qthing] != y )
                            {
                                continue;
                            }

                            if ( sidata->sector == y && sidata->isUpstream == isupstream.at ( t ) && sidata->isBarrel==isbarrel.at ( t ) && sidata->E1.strip.p.size() >0 )
                            {
                                for ( int qent =0; qent<sidata->E1.strip.p.size(); qent++ )
                                {
                                    float qangle = sidata->Angle ( layer );
                                    float tenergy = SiDataBase::QValue ( reacinfo, sidata->ESumLayer ( 1, false ) * reacinfo->qqq5EnGain, sidata->PosE1().Angle ( beamdir ) );
                                    float fqqval = reacinfo->qValGsGs-tenergy;

                                    //cout<<reacinfo->qValGsGs<<endl;
                                    qqq5hist->Fill ( qangle,fqqval );
                                    tothist->Fill ( qangle, fqqval );
                                }
                            }

                        }
                    }
                }
                else
                {
                    continue;
                }




            }


        }
    }
    qqq5hist->Write();
    sx3hist->Write();
    tothist->Write();

    newfile->Close();




    return;
}

void ProtonEvAngle ( string filenames, string namefile, string treename, vector<string> detectortype, vector<bool> isupstream, int layer, bool back = false )
{
    vector<string> rootfiles = DecodeItemsToTreat ( filenames, "system" );

    if ( rootfiles.size() ==0 )
    {
        return;
    }

    TChain* chain = new TChain ( treename.c_str(),treename.c_str() );

    for ( int file =0; file < rootfiles.size(); file++ )
    {
        chain->Add ( rootfiles[file].c_str() );
    }

    string newfilename = "ProtonvAngle_"+namefile;

    TFile* newfile = new TFile ( newfilename.c_str(),"recreate" );

    int entries = chain->GetEntries();

    TH2F* hist;
    vector<TH2F*> vecthists2;
    vector<TH2F*> dsvecthists;
    string histname;
    vector<TH2F*> secthists;
    vector<TH2F*> dssecthists;
    TH2F* secthist;
    string histnamesect;
    string histnameqqq;
    TH2F* qqqhist;
    vector<TH2F*> qqqvect;
    vector<TH2F*> dsqqq5vect;
    string isup;

    for ( int c = 0; c<isupstream.size(); c++ )
    {

        for ( int q = 0; q<detectortype.size(); q++ )
        {
            if ( detectortype[q] == "SX3" )
            {
	      for ( int i = 0; i<48; i++ )
                {
                    int sector = i/4;
                    int strip = i-sector*4;

                    if ( isupstream[c] == 1 )
                    {
                        isup = "U";
                    }
                    else if ( isupstream[c] == 0 )
                    {
                        isup = "D";
                    }
                    else
                    {
                        cout<<"invalid upstream bool."<<endl;
                        return;
                    }

                    histname = "SX3_" +isup + "_Sector_"+to_string ( sector ) +"_Strip_"+to_string ( strip );
                    hist = new TH2F ( histname.c_str(),"Proton v. Angle SX3",800,0,170,1600,0,14 );

                    if ( isup == "U" )
                    {
                        vecthists2.push_back ( hist );
                    }
                    else if ( isup == "D" )
                    {
                        dsvecthists.push_back ( hist );
                    }

                    if ( i%4==0 )
                    {
                        histnamesect = "SX3_" +isup+"_Full_Sector_"+to_string ( sector );
                        secthist = new TH2F ( histnamesect.c_str(),"Proton v. Angle For a Full Sector SX3", 800,0,170,1600,0,14 );
                        if ( isup == "U" )
                        {
                            secthists.push_back ( secthist );
                        }
                        else if ( isup == "D" )
                        {
                            dssecthists.push_back ( secthist );
                        }
                    }
                }
            }

            if ( detectortype.at ( q ) == "QQQ5" )
            {
                for ( int qqq = 0; qqq<4; qqq++ )
                {

                    if ( isupstream[c] == 1 )
                    {
                        isup = "U";
                    }
                    else if ( isupstream[c] == 0 )
                    {
                        isup = "D";
                    }
                    else
                    {
                        cout<<"invalid upstream bool."<<endl;
                        return;
                    }
                    histnameqqq = "QQQ5_"+isup+"_Sector_"+to_string ( qqq );
                    qqqhist = new TH2F ( histnameqqq.c_str(),"Protov v. Angle QQQ5",800,0,170,400,0,14 );

                    if ( isup == "U" )
                    {
                        qqqvect.push_back ( qqqhist );
                    }
                    else if ( isup =="D" )
                    {
                        dsqqq5vect.push_back ( qqqhist );
                    }
                }
            }

            else
            {
                continue;
            }
        }
    }


//     vector<SiDataBase>* vectordata = new vector<SiDataBase>;
//     chain->SetBranchAddress ( "si", &vectordata );


    vector<si_struct>* vectordata = new vector<si_struct>;
    chain->SetBranchAddress("si",&vectordata);

    for ( int entry = 0; entry<entries; entry++ )
    {
        chain->GetEntry ( entry );

        if ( entry%2000 ==0 )
        {
            cout<<"Entries treated: "<<entry<<"/"<<entries<<"\r"<<flush;
        }

//         if ( vectordata->size() == 0 /*&& gam->size()>0*/ )
//         {
//             // 	  cout<<gam->size()<<endl;
// // 	  cout<<"No data."<<endl;
// //                          return;
//         }

        for ( int j = 0; j<vectordata->size(); j++ )
        {
            si_struct* sidata; // = &vectordata->at ( j );

            //cout<<"--------------------------------------------"<<endl;
            //cout<<vectordata->size()<<endl;

            for ( int iup = 0; iup<isupstream.size(); iup++ )
            {

                for ( int t = 0; t<detectortype.size(); t++ )
                {

                    if ( detectortype.at ( t ) =="SX3" )
                    {
                        for ( int k = 0; k<48; k++ )
                        {
                            int sectorid = k/4;
                            int stripid = k-sectorid*4;

                            if ( (int)sidata->sector_strip == sectorid && sidata->isUpstream == isupstream.at ( iup ) && sidata->isBarrel == 1 )
                            {
                                int strip = (sidata->sector_strip-sectorid)*100;
				if (strip == stripid)
                                {
//                                     if ( sidata->E1.strip.p.at ( pent ) == stripid )
//                                     {
//                                         cout<<"========================================================"<<endl;
                                        float toten = sidata-> energy;
//                                         cout<<"energy "<<toten<<endl;
                                        float angle = sidata->pos.Theta();
//                                         cout<<"angle "<<angle<<endl;

                                        if ( isupstream.at ( iup ) == 0 )
                                        {

                                            vecthists2[k]->Fill ( angle, toten );
                                        }
                                        else
                                        {
                                            dsvecthists[k]->Fill ( angle,toten );
                                        }
                                    }
//                                 }
                            }
                        }
                    }

//                     else if ( detectortype.at ( t ) == "QQQ5" )
//                     {
//                         for ( int y = 0; y<4; y++ )
//                         {
//                             if ( sidata->sector == y && sidata->isUpstream == isupstream.at ( iup ) && sidata->isBarrel== 0 && sidata->E1.strip.p.size() >0 )
//                             {
//                                 for ( int qent =0; qent<sidata->E1.strip.p.size(); qent++ )
//                                 {
//                                     float tenergy = sidata->ESumLayer ( layer,back );
//                                     float qangle = sidata->Angle ( layer );
//                                     if ( isupstream.at ( iup ) == 0 )
//                                     {
//                                         qqqvect[y]->Fill ( qangle,tenergy );
//                                     }
//                                     else
//                                     {
//                                         dsqqq5vect[y]->Fill ( qangle,tenergy );
//                                     }
//                                 }
//                             }
//
//                         }
//                     }
                    else
                    {
                        continue;
                    }
                }
            }



        }


    }

    TH2F* fullhist = new TH2F ( "fullprotonvangle", "Full Proton v. Angle",800,0,170,1600,0,14 );
    TH2F* dsfullhist = new TH2F ( "dsfullprotonvangle", "Full Proton v. Angle",800,0,170,1600,0,14 );
    TH2F* set = new TH2F ( "full_set","SX3 and QQQ5 Proton v. Angle", 800, 0,170, 1600,0,14 );


    for ( int p = 0; p<48; p++ )
    {
//         cout<<vecthists2.size() <<endl;
        vecthists2[p]->Write();
        dsvecthists[p]->Write();

//         cout<<"2"<<endl;

        fullhist->Add ( vecthists2[p] );
        dsfullhist->Add ( dsvecthists[p] );

//         cout<<"3"<<endl;
        int sector = p/4;

        secthists[sector]->Add ( vecthists2[p] );
        dssecthists[sector]->Add ( dsvecthists[p] );

//         cout<<"4"<<endl;
    }

    fullhist->Write();
//     cout<<"5"<<endl;
    dsfullhist->Write();
//     cout<<"6"<<endl;

    for ( int w = 0; w<secthists.size(); w++ )
    {
        secthists[w]->Write();
        dssecthists[w]->Write();
    }

//     for ( int hq =0; hq<qqqvect.size(); hq++ )
//     {
//         qqqvect[hq]->Write();
//         dsqqq5vect[hq]->Write();
//
//     }

//     for ( int sx =0; sx<48; sx++ )
//     {
//         set->Add ( vecthists2[sx] );
//         set->Add ( dsvecthists[sx] );
//     }

//     for ( int qq =0; qq<4; qq++ )
//     {
//         set->Add ( qqqvect[qq] );
//         set->Add ( dsqqq5vect[qq] );
//     }


    set->Write();



    newfile->Close();


    return;
}

// void FindPercentGauss(float amp, float mean, float width, float percent, bool right)
// {
//   float peramp = amp*percent;
//
//   float A = -2*pow(width,2)*log(percent*amp*TMath::Sqrt((2*TMath::Pi()*pow(width,2))));
//
//   complex<float> ac =
//
//   float x1 = (2*mean+TMath::Sqrt(4*pow(mean,2)-4*(pow(mean,2)-A)))/2;
//
//   float x2 = (2*mean-TMath::Sqrt(4*pow(mean,2)-4*(pow(mean,2)-A)))/2;
//
// //   if (right == true)
// //   {
// //     x = mean + abs(x);
// //   }
// //   else
// //   {
// //     x = mean - abs(x);
// //   }
//
//   cout<<x1<<" and "<<x2<<endl;
//
//   return;
// }



