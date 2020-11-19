#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "TMath.h"
#include "goddess_analysis_macros.h"
#include "GoddessAnalysis.h"

using namespace std;

void CompareGammasphere ( string filename, string treename)
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
    
    size_t posroot = filename.find(".");
    
    string run_num = filename.substr(posroot-3, 3);
    
    size_t posn = run_num.find("n");
    
    if (posn > 10) 
    {
        run_num = run_num.substr(1);
    }
    
    string outFileName = "GSEfficRun" + run_num + ".root";

    int entries = tree->GetEntries();
    //std::cout <<"Entries in TTree:" << entries << std::endl;

     
    TFile* f = new TFile(outFileName.c_str(), "update");
    
    //TH1F* gs = new TH1F ( "GSEffic", "Gammasphere Efficiency", 6000, 0, 6000 );
    
    vector<GamData>* vectGamData = new vector<GamData>();
    tree->SetBranchAddress ( "gam", &vectGamData );
    
    int GS_detector = 26;
    
        for (int i = 20; i < GS_detector; i++)
        {
            string number = to_string(i);
            
            string name = "GammaEffic" + number;
            
            TH1F* gs = new TH1F ( name.c_str() , "Gammasphere Efficiency", 12000, 0, 6000 );
            
            
            for (int k = 0; k < entries; k++)
                {
                    tree->GetEntry ( k );
                    
                    if ( k%20000 == 0 )
                    {
                    cout << "Detector" << i << " : Treated entry sorted # " << k << " / " << entries << "\r" << flush;
                    }
                    
                    if (vectGamData->size() ==0){continue;}
                    
                    for(int g = 0; g < vectGamData->size(); g++)
                    {
                    GamData GSData = vectGamData->at(g);
                    if (GSData.type != 1) {continue;}

                    if(GSData.num != i){continue;}
                    if(GSData.num == i){gs->Fill(GSData.en);}
                    }
                    
                }
                
		gs->Write();
        
		cout << "Writing " << gs->GetName() << " to file..." << endl;
                
            //gs->Draw();
            
            }
          f-> Close();  
return;
}



void GSEnvsCt ( string filename, string treename, int fdetgs, int ldetgs, int histostop = 1500)
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
    
    size_t posroot = filename.find(".");
    
    string run_num = filename.substr(posroot-3, 3);
    
    size_t posn = run_num.find("n");
    
    if (posn > 10) 
    {
        run_num = run_num.substr(1);
    }
    
    string outFileName = "GSEfficRun" + run_num + ".root";

    int entries = tree->GetEntries();
    //std::cout <<"Entries in TTree:" << entries << std::endl;

     
    TFile* f = new TFile(outFileName.c_str(), "update");
    
    //TH1F* gs = new TH1F ( "GSEffic", "Gammasphere Efficiency", 6000, 0, 6000 );
    
    vector<GamData>* vectGamData = new vector<GamData>();
    tree->SetBranchAddress ( "gam", &vectGamData );
    
    int size_array = ldetgs-fdetgs;
    
    TH1F* hist_array[size_array];
    vector<float> vect_array[size_array];
    
    
    
    
    
    
        for (int i = fdetgs; i < ldetgs; i++)
        {
            string number = to_string(i);
            
            string name = "GammaEffic" + number;
            
            TH1F* gs = new TH1F ( name.c_str() , "Gammasphere Efficiency", 5500, 0, histostop );
            
            hist_array[i-fdetgs] = gs;
            
            vector<float> en_in_hist;
            //vect_array[i-fdetgs] = en_in_hist;
            
            
            
        }
            
        size_t ltime;
        size_t ftime;
        
        for (int k = 0; k < entries; k++)
            {
                tree->GetEntry ( k );
                    
                if ( k%20000 == 0 )
                {
                cout << "Treated entry sorted # " << k << " / " << entries << "\r" << flush;
                }
                
                if (vectGamData->size() ==0){continue;}
                
                for(int g = 0; g < vectGamData->size(); g++)
                {
                    GamData GSData = vectGamData->at(g); 
                    if (k == 0) 
                    {
                        if (g == 0)
                        {
                        cout<<"First count timestamp: "<<GSData.time<<endl;
                        ftime = (GSData.time);
                        }
                    }
                    if (k == entries-1) 
                    {
                        if (g == vectGamData->size() -1)
                        {
                        cout<<" "<<endl;
                        cout<<"Last count timestamp: "<<GSData.time<<endl;
                        ltime = GSData.time;
                        size_t totruntime = ltime - ftime;
                        cout<<"Total run time: "<<totruntime<<endl;
                        }
                    }
                    if (GSData.type != 1) {continue;}
                    for (int h = fdetgs; h <ldetgs; h++)
                    {
                        if (GSData.num == h) 
                        {
                            hist_array[h-fdetgs]->Fill(GSData.en/1.233-5.5898);
                        }
                    }
                    
                }
            }
            
            
            
            for (int d = fdetgs; d<ldetgs; d++)
            {
                /*string histoname = "GammaEffic" + to_string(d);
                if (f->FindObjectAny(histoname.c_str()))
                {
                    cout<<histoname<<endl;
                    string deleteobj = histoname + ";*";
                    f->Delete(deleteobj.c_str());
                }*/
                hist_array[d-fdetgs]->Write();
            }
            
            
          f-> Close();  
return;
}


double gaus_fit_func(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 33;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[3*i]*TMath::Exp(-pow(x[0]-par[3*i+1],2)/(2*pow(par[3*i+2],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final + par[ng*3]*x[0] + par[ng*3+1];
}

vector<double> get_params(string textfile)
{
    vector<double> params;
    
    ifstream peaksfile;
    
    peaksfile.open(textfile);
    
    if (!peaksfile)
    {
        cout<<"Unable to open data file."<<endl;
    }
    
    if (peaksfile)
    {
        cout<<"File open."<<endl;
    }
    
    for(string line; getline(peaksfile, line);)
    {
        
        istringstream in(line);
        string energy, height, mean;
        in>>energy>>height>>mean;
        
        double ht = atof(height.c_str());
        double en = atof(energy.c_str());
        double mn = atof(mean.c_str());
        
        params.push_back(ht);
        params.push_back(en);
        params.push_back(mn);
    }
    
    params.push_back(0);
    params.push_back(0);
    
    return params;
}

double gaus_fit_peak3(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 3;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[3*i]*TMath::Exp(-pow(x[0]-par[3*i+1],2)/(2*pow(par[3*i+2],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final + par[ng*3]*x[0] + par[ng*3+1];
}

double gaus_fit_peak3_nopol1(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 3;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[3*i]*TMath::Exp(-pow(x[0]-par[3*i+1],2)/(2*pow(par[3*i+2],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final;
}

double gaus_fit_peak2(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 2;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[3*i]*TMath::Exp(-pow(x[0]-par[3*i+1],2)/(2*pow(par[3*i+2],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final + par[ng*3]*x[0] + par[ng*3+1];
}

double gaus_fit_peak2_nopol1(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 2;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[3*i]*TMath::Exp(-pow(x[0]-par[3*i+1],2)/(2*pow(par[3*i+2],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final;
}

double gaus_fit_peak1(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 1;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[3*i]*TMath::Exp(-pow(x[0]-par[3*i+1],2)/(2*pow(par[3*i+2],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final + par[ng*3]*x[0] + par[ng*3+1];
}

double gaus_fit_peak1_nopol1(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 1;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[3*i]*TMath::Exp(-pow(x[0]-par[3*i+1],2)/(2*pow(par[3*i+2],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final;
}

float Gauss_integral(float amp, float sig)
{
   float integral =  TMath::Sqrt(2*3.14)*amp*sig;
   
   return integral;
}

vector<float> GSIntegralFit(TF1 *fit, int numpeaks, int xmin, int xmax)
{
    int totalparams = numpeaks*3;
    
    vector<float> vect_params;
    
    for (int i=0; i<totalparams; i++)
    {
        float param = fit->GetParameter(i);
        vect_params.push_back(param);
    }
    
    vector<float> integral_vals;
    
    for (int k=0; k<numpeaks; k++)
    {
        float amp = vect_params[k*3];
        float mean = vect_params[k*3+1];
        float sig = vect_params[k*3+2];
        cout<<"Amplitude: "<<amp<<endl;
        cout<<"Mean: "<<mean<<endl;
        cout<<"Sigma: "<<sig<<endl;
        float integral_val = Gauss_integral(amp,sig);
        integral_vals.push_back(integral_val);
        float TF1int = fit->Integral(xmin,xmax);
        cout<<"The integral value of peak "<<k<<" is: "<<integral_val<<endl;
        cout<<"TF1 integral value: "<<TF1int<<endl;
    }
    
    
    return integral_vals;
}

void GSEasyIntegral(TH1* histo, int peaknum, int xmin, int xmax, bool pol1 = 1)
{
    TF1* fit;   
    int param_num = peaknum*3+2;
    
    if (pol1 == 1)
    {
        if (peaknum == 1)
        {
            fit = new TF1("fit func", gaus_fit_peak1, xmin, xmax, param_num);  
        }
        if (peaknum == 2)
        {
            fit = new TF1("fit func", gaus_fit_peak2, xmin, xmax, param_num);
        }
        if (peaknum == 3)
        {
            fit = new TF1("fit func", gaus_fit_peak3, xmin, xmax, param_num);
        }
    }
    else
    {
        if (peaknum == 1)
        {
            fit = new TF1("fit func", gaus_fit_peak1_nopol1, xmin, xmax, param_num);  
        }
        if (peaknum == 2)
        {
            fit = new TF1("fit func", gaus_fit_peak2_nopol1, xmin, xmax, param_num);
        }
        if (peaknum == 3)
        {
            fit = new TF1("fit func", gaus_fit_peak3_nopol1, xmin, xmax, param_num);
        } 
    }
    int amplitude1;
    int mean1;
    int sig1;
    int amplitude2;
    int mean2;
    int sig2;
    int amplitude3;
    int mean3;
    int sig3;
    
    cout<<"Please enter the amplitude for the first peak:"<<endl;
    cin>>amplitude1;
    cout<<"Please enter the mean for the first peak:"<<endl;
    cin>>mean1;
    cout<<"Please enter the sigma for the first peak:"<<endl;
    cin>>sig1;
    if (peaknum>1)
    {
        cout<<"Please enter the amplitude for the second peak:"<<endl;
        cin>>amplitude2;
        cout<<"Please enter the mean for the second peak:"<<endl;
        cin>>mean2;
        cout<<"Please enter the sigma for the second peak:"<<endl;
        cin>>sig2;
    }
    if (peaknum>2)
    {
        cout<<"Please enter the amplitude for the third peak:"<<endl;
        cin>>amplitude3;
        cout<<"Please enter the mean for the third peak:"<<endl;
        cin>>mean3;
        cout<<"Please enter the sigma for the third peak:"<<endl;
        cin>>sig3;
    }
    
    int bg1=0;
    int bg2=0;
    
    if (pol1 == 1)
    {
    cout<<"Background slope is: "<<endl;
    cin>>bg1;
    cout<<"Background intersect is: "<<endl;
    cin>>bg2;
    }
    
    if (peaknum ==1)
    {
        fit->SetParameters(amplitude1,mean1,sig1,bg1,bg2);
    }
    else if (peaknum == 2)
    {
        fit->SetParameters(amplitude1,mean1,sig1,amplitude2,mean2,sig2,bg1,bg2);
    }
    else if (peaknum == 3)
    {
        fit->SetParameters(amplitude1,mean1,sig1,amplitude2,mean2,sig2,amplitude3,mean3,sig3,bg1,bg2);
    }
    else 
    {
        cout<<"God forbid you're trying to use this for more than 3 peaks.... start over and use your head. If you're using it for zero peaks, come find me. We need to discuss your career choices."<<endl;
    }
    
    histo->Fit(fit,"M","",xmin,xmax);
    vector<float> integral_vals = GSIntegralFit(fit,peaknum,xmin,xmax);
    
    
    //Uncertainties
    
    for (int i=0; i<integral_vals.size(); i++)
    {
     float error_from_integral = TMath::Sqrt(integral_vals[i]);
     cout<<"The error obtained from taking the sqrt of the integral is: "<<error_from_integral<<endl;
     
     double amp = fit->GetParameter(3*i);
     //double mean = fit->GetParameter(3*i+1);
     double sig = fit->GetParameter(3*i+2);
     
     double eamp = fit->GetParError(3*i);
     //double emean = fit->GetParError(3*i+1);
     double esig = fit->GetParError(3*i+2);
     
     double bg1 = fit->GetParameter((integral_vals.size()-1)*3+1);
     double bg2 = fit->GetParameter((integral_vals.size()-1)*3+2);
     double ebg1 = fit->GetParError((integral_vals.size()-1)*3+1);
     double ebg2 = fit->GetParError((integral_vals.size()-1)*3+2);
     
     float error_from_calc = TMath::Sqrt(2.0*TMath::Pi())*TMath::Sqrt(pow((eamp/amp),2.0)+pow((esig/sig),2))*integral_vals[i];
     
     float error_from_bg = TMath::Sqrt(pow((ebg1/bg1),2.0)+pow((ebg2/bg2),2.0));
     
     cout<<"The error obtained from our formula is: "<<error_from_calc<<endl;
     cout<<"The error from the background is: "<<error_from_bg<<endl;
     
     float combined_error = error_from_bg + error_from_calc; 
     
     cout<<"The total error is: "<< combined_error<<endl;
     
    }
    
    
    
    
    return;
}

void DrawErrors() {

   TCanvas *c1 = new TCanvas("c1","Gammasphere Efficiency",200,10,700,500);

   const Int_t n = 17;
   Float_t x[n]  = {121,244,344,367,411,444,488,563,586,779,867,964,1085,1113,1212,1292,1408};
   Float_t y[n]  = {1.000,1.086,0.9206,0.9170,0.8294,0.8704,0.7379,0.6934,0.5129,0.5957,0.5756,0.5343,0.4992,0.4878,0.6112,0.4725,0.4291};
   Float_t ex[n] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
   Float_t ey[n] = {0.0251,0.0514,0.0242,0.2100,0.0941,0.0855,0.3245,0.2556,0.2650,0.0291,0.0555,0.0344,0.0487,0.0300,0.0761,0.0542,0.0141};
   TGraphErrors *gr = new TGraphErrors(n,x,y,ex,ey);
   gr->SetTitle("Gammasphere Efficiency");
   gr->SetMarkerColor(4);
   gr->SetMarkerStyle(21);
   gr->Draw("ALP");

   c1->Update();
   
   return;
}
