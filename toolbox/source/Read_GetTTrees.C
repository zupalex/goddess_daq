#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "TH1.h"
#include "GoddessStruct.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TRandom.h"
#include <TTreeReader.h>
#include <TTreeReaderArray.h>

void Read_GetTTrees()
{
    /*File INPUT  */
    TFile* f = TFile::Open ( "/mnt/f/Goddess/95Mo/run357.root" );

    /*get the trees*/
    TTree* data = ( TTree* ) f->Get ( "god" );
    data->Print();
    TLeaf* timeS = data->GetLeaf ( "timestamp" );


    /*Creating histograms to be filled later*/
    TH1D* f1 = new TH1D ( "f1", "Time All DetID", 1000, -500, 500 );
    TH1D* f2 = new TH1D ( "f2", "Ex", 5000, -100, 2000000 );
    TH1D* f3 = new TH1D ( "f3", "Y projection Gam vs Ex", 4000, 0, 500000 );
    TH1D* a1 = new TH1D ( "a1", "Time difference", 359, 0, 360 );
    TH1D* a2 = new TH1D ( "a2", "Test angle E2", 359, 0, 360 );
    TH1D* ad = new TH1D ( "ad", "Test angle dE", 359, 0, 360 );
    TH2D* Pid = new TH2D ( "Pid", "E vs. dE",400, 0, 500000, 400, 0, 600000 );
    TH2D* PidSect1 = new TH2D ( "PidSect1", "dE vs. E for strip in barrel 1", 4000, 0, 500000, 4000, 0, 600000 );
    TH2D* Pid2 = new TH2D ( "Pid2", "dE vs. E for strip in barrel 2", 400, 0, 500000, 400, 0, 600000 );
    TH2D* TotvAngle = new TH2D ( "TotvAngle", "Tot vs. Angle",600,0,170, 100, 0, 500000 );
    TH2D* GamvsEx = new TH2D ( "GamvsEx", "Gam vs. Ex", 900, 2800, 400000, 2500, 2800, 400000 );
    /* Read det branches from all target file and tree*/
    TTreeReader myReader ( "god", f );

    /*To get all the events*/
    unsigned long long int nentries=data->GetEntries();
    cout << "n entries " << nentries << endl;

    std::vector<SiDataDetailed>* si; //declare the pointer
    si = new std::vector<SiDataDetailed>; // allocate memory to it
    //int gam;
    float gamE;
    int gam;
    int gamnum;
    float qval;
    float qvalue=4.13408;
    float Ex;
    long long int counter;
    long long int counter1;

    double siVecsize;
    double gamVecsize;
    int detid;
    //float eSumsi;
    double dE;
    double dE1;
    double de_angle;
    double E1;
    double E11;
    double e1_angle;
    double E2;
    double E21;
    double e2_angle;
    double E;
    double Etot;
    double Etot1;
    double angle;
    double EEE;
    double timeg;
    double timesi;
    double timinou;
    double timediff;
    data->SetBranchAddress ( "si", &si );
    vector<GamData>* vectGamData = new vector<GamData>();
    data->SetBranchAddress ( "gam", &vectGamData );
    cout << "test" << endl;


    /*Looping over entries*/
    for ( unsigned long long int i = 0; i < nentries / 10; i++ )
    {
        if ( i % 1000000 == 0 ) std::cout << "Event: " << i << std::endl;
        data->GetEntry ( i );
        siVecsize = si->size();
        gamVecsize = vectGamData->size();

        /*Analysis of gamma branch*/
        for ( int g = 0; g < gamVecsize; g++ )
        {
            GamData GSData = vectGamData->at ( g );
            EEE = GSData.en;
            timeg = GSData.time;
            f2->Fill ( timeg );
            //cout << timeg << endl;
        }
        //if (detid == 1100){
        for ( int g = 0; g < gamVecsize; g++ )
        {
            for ( int j = 0; j < siVecsize; j++ )
            {
                timediff = ( double ) vectGamData->at ( g ).time - ( double ) si->at ( j ).TimestampMaxLayer ( 0, false );
                a1->Fill ( timediff );
            }

        }
        //}
        //if (detid == 2100) {
        for ( int g = 0; g < gamVecsize; g++ )
        {
            for ( int j = 0; j < siVecsize; j++ )
            {
                timesi = ( double ) vectGamData->at ( g ).time - ( double ) si->at ( j ).TimestampMaxLayer ( 0, false );
                f1->Fill ( timesi );
            }

        }
        //}
        /*Analysis of silicon branch*/

        for ( int j = 0; j < siVecsize; j++ )
        {
            SiDataDetailed si_test = si->at ( j );
            //bool isUpstream = (int)si_test.isUpstream;
            //bool isBarrel = (int)si_test.isBarrel;
            /*get the detector id*/
            detid = si_test.telescopeID;
            /*false == front or p-side && true == back or n-side */
            /*Angle access the angle of the event -- Angle is in the center of mass and in degrees*/
            dE = si_test.ESumLayer ( 0, false );
            de_angle = si_test.Angle ( 0 );
            E1 = si_test.ESumLayer ( 1, false );
            double E1_backside = si_test.ESumLayer ( 1, true );
            e1_angle = si_test.Angle ( 1 );
            E2 = si_test.ESumLayer ( 2, false );
            double E2_backside = si_test.ESumLayer ( 2, true );
            e2_angle = si_test.Angle ( 2 );
            angle = e1_angle;
            E = E1 + E2;
            Etot = dE + E1 + E2;

            if ( detid>=2200 && si_test.E1.en.p.size() >0 )
            {
                for ( int val = 0; val<si_test.E1.en.p.size(); val++ )
                {
                    if ( si_test.E1.strip.n.at ( val ) == -1 )
                    {
		      cout<<"Detector: "<<detid<<" sector: "<<si_test.sector<<" strip: "<<si_test.E1.strip.p.at(val)<<" energy: "<<si_test.E1.en.p.at(val)<<endl;
                    }
                }
            }

            //a1->Fill(e1_angle);
            a2->Fill ( e2_angle );
            ad->Fill ( de_angle );

            //if (si_test.isUpstream) std::cout << E1 << '\t' << E1_backside << '\t' << E2 << '\t' << E2_backside << std::endl;

            //f1->Fill(detid);
            if ( ( E != 0 ) && ( dE != 0 ) && ( si_test.isUpstream ) && ( si_test.isBarrel ) )
            {
                //cout << "telescopte ID " << detid << endl;
                Pid->Fill ( E,dE ); //cout << E1 << endl;
            }
            else if ( ( Etot != 0 ) && ( detid == 2104 ) && ( si_test.isUpstream ) && ( si_test.isBarrel ) )
            {
                PidSect1->Fill ( dE, E );
            }
            else if ( ( Etot != 0 ) && ( detid == 2103 ) && ( si_test.isUpstream ) && ( si_test.isBarrel ) )
            {
                Pid2->Fill ( dE, E );
            }
            if ( ( Etot > 0 ) && ( si_test.isUpstream ) && ( si_test.isBarrel ) )
            {
                TotvAngle->Fill ( angle, Etot );
                if ( e1_angle != 0 )
                {
                    qval = si_test.QValue ( 133.9053947, 1337.7, 2.01410177812, 1.00782503223, 134.907228 );
                    //cout << qval << endl;
                    Ex = ( qval - qvalue );
                    //cout << Ex << endl;
                    f2->Fill ( Ex );
                }
                //((Ex > 3000) && (Ex < 2500))&&
                if ( ( Ex>3000 ) && ( si_test.isUpstream ) && ( si_test.isBarrel ) && ( ( ( timediff > 70 ) && ( timediff < 100 ) ) || ( ( timediff > 180 ) && ( timediff < 205 ) ) ) )
                {
                    counter += 1;
                    for ( int h = 0; h < gamVecsize; h++ )
                    {

                        //f1->Fill(timeg - timesi);
                        EEE = vectGamData->at ( h ).en;
                        //E11 = EEE * 0.0001;
                        GamvsEx->Fill ( EEE, Ex );
                        counter1 += 1;


                    }
                }

            }

        }



    }
    cout << "It seems all good" << endl;

    cout << counter << endl;
    cout << "Ouiiiiiiii" << endl;
    cout << counter1 << endl;
    TCanvas* EvA = new TCanvas ( "EvA", "Etot vs. Angle", 400, 300 );
    EvA->cd();
    TotvAngle->Draw ( "colz" );
    /*TCanvas* c1 = new TCanvas("c1", "Excitation Energy", 400, 300);
    c1->cd();
    f2->Draw();
    TCanvas* c2 = new TCanvas("c2", "Gamma Energy vs. Excitation energy", 400, 300);
    c2->cd();
    GamvsEx->Draw("COLZ");
    TCanvas* c3 = new TCanvas("c3", "Projection Y", 400, 300);
    c3->cd();
    f3= TotvAngle->ProjectionY("Y Projection",50,80);
    f3->Draw();
    TCanvas* c4 = new TCanvas("c4", "Time difference", 400, 300);
    c4->cd();
    a1->Draw();*/

    //TTreeReaderArray<double> Egam(myReader, "gam/gam.en");//mmh

    //cout << "boo is " << Egam[0] << endl;
    /*
    TH1F* AddBack = new TH1F("AddBack", "AddBack", ABstep, 0, ABmax);
    TH1F* Sum = new TH1F("Det", "Det", Sstep, 0, Smax);

    TRandom* random = new TRandom;

    TTreeReader myReader("data", f);
    TTreeReaderArray<UShort_t> detector(myReader, "adc");



    TF1  *f1 = new TF1("f1","gaus",9100,9200);
    TF1  *f2 = new TF1("f2","gaus",4410,4470);
    TCanvas *bou =new TCanvas("bou","bou",0,0,600,600);
    bou->cd();
    bou->SetLogy();
    AddBack->Draw("same");
    AddBack->Fit("f1","","",9100,9200); double t=f1->Integral(9100,9200);cout<<t<<endl;
    AddBack->Fit("f2","","",4410,4470); double tt=f2->Integral(4410,4470);cout<<tt<<endl;
    Sum->SetLineColor(kPink);
    Sum->Draw("same");*/
//Sum->Fit("f1","","",9100,9200); double g=f1->Integral(9100,9200);cout<<g<<endl;
//Sum->Fit("f2","","",4410,4470);double gg=f2->Integral(4410,4470);cout<<gg<<endl;
    /*
    int t = Sum->Integral(9220,9320);
    int a = Sum->Integral(9205,9215);int g = Sum->Integral(9325,9335);
     cout<<t<<","<<a<<","<<g<<endl;
    int z = AddBack->Integral(9220,9320);
    int x = AddBack->Integral(9205,9215);int c = AddBack->Integral(9325,9335);
     cout<<z<<","<<x<<","<<c<<endl;*/

    return;
}
