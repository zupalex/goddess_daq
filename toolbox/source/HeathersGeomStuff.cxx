#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <math.h>
#include "TMath.h"
#include "TLegend.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include "goddess_analysis_macros.h"
#include "GoddessAnalysis.h"

using namespace std;

double gaus_fit_peak2(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 2;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2]+par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final + par[ng*2+1]*x[0] + par[ng*2+2];
}

double gaus_fit_peak2_nopol1(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 2;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2]+par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final;
}

double gaus_fit_peak3(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 3;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2]+par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final + par[ng*2+1]*x[0] + par[ng*2+2];
}


double gaus_fit_peak3_nopol1(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 3;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2]+par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final;
}


double gaus_fit_peak4(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 4;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2]+par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final + par[ng*2+1]*x[0] + par[ng*2+2];
}

double gaus_fit_peak4_nopol1(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 4;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2]+par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final;
}

double gaus_fit_peak5(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 5;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final + par[ng*2+1]*x[0] + par[ng*2+2];
}

double gaus_fit_peak5_nopol1(double *x, double *par)
{
    double gauss_final = par[1]*TMath::Exp(-pow(x[0]-(par[2]),2)/(2*pow(par[0],2)));
    
    int ng = 5;
    
    for (int i=1; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final;
}

double gaus_fit_peak6(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 6;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2]+par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final + par[ng*2+1]*x[0] + par[ng*2+2];
}

double gaus_fit_peak7_nopol1(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 7;
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2]+par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final;
}

double gaus_fit_peak7(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 7;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2]+par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final + par[ng*2+1]*x[0] + par[ng*2+2];
}

double gaus_fit_peak6_nopol1(double *x, double *par)
{
    double gauss_final = 0;
    
    int ng = 6;
    
    for (int i=0; i < ng; i++)
    {
        double gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2]+par[2*i+2]),2)/(2*pow(par[0],2)));
        
        gauss_final = gauss_final + gaus_piece;
    } 
    return gauss_final;
}

float Gauss_integral(float amp, float sig)
{
   float integral =  TMath::Sqrt(2*3.14)*amp*sig;
   
   return integral;
}
vector<float> GSIntegralFit(TF1 *fit, int numpeaks, float xmin, float xmax, float binwidth)
{
    int totalparams = numpeaks*2+3;
    
    vector<float> vect_params;
    
    for (int i=0; i<totalparams; i++)
    {
        float param = fit->GetParameter(i);
        vect_params.push_back(param);
    }
    
    vector<float> integral_vals;
    
    for (int k=0; k<numpeaks; k++)
    {
        float amp = vect_params[k*2+1];
        float mean = vect_params[k*2+2];
        float sig = vect_params[0];
	cout<<"Peak Number:"<<k+1<<endl;
        cout<<"Amplitude: "<<amp<<endl;
        cout<<"Mean: "<<mean<<endl;
        cout<<"Sigma: "<<sig<<endl;
        float integral_val = Gauss_integral(amp,sig)/binwidth;
        integral_vals.push_back(integral_val);
        float TF1int = fit->Integral(xmin,xmax);
        cout<<"The integral value of peak "<<k+1<<" is: "<<integral_val<<endl;
        cout<<"TF1 integral value: "<<TF1int<<endl;
	cout<<"------------------------------------------------"<<endl;
    }
    
    return integral_vals;
}

template<typename T1, typename T2> void ExEnFit(TH1 *histo, int peaknum, T1 xmin, T1 xmax, float width, T2 mean1, T2 amp1, T2 mean2=0, T2 amp2=0, T2 mean3=0, T2 amp3=0, T2 mean4=0, T2 amp4=0)
{
  bool pol1;
  cout<<"Enter 1 if you would like background in the fit. Enter 0 for no background in the fit."<<endl;
  cin>>pol1;
  
  float m;
  float b;
  if(pol1==1){cout<<"Enter m and b for the pol1. (0 and 0 are standard)"<<endl; cin>>m; cin>>b;}
  
  bool rel;
  cout<<"Enter 1 if you would like the peaks to be fit relative to each other. Enter 0 if you don't want to peaks to be relative to each other."<<endl;
  cin>>rel;
  
  
  TF1* fit;
  int npar = peaknum*2+3;
  
  TVirtualFitter::SetMaxIterations(5000);
  
  function<function<double(double*,double*)>(int,bool)> fit_function = [=](int npeaks, bool wpol1)->function<double(double*,double*)>{
    
    function<double(double*,double*)> ret_fit_func = [=](double *x, double *par)->double{
      
      double gauss_final = par[1]*TMath::Exp(-pow(x[0]-par[2],2)/(2*pow(par[0],2)));
      
      double gaus_piece;
      
      if (npeaks>1){
    
      for (int i=1; i < npeaks; i++)
      {
	if(rel==1){
	  gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-(par[2]+par[2*i+2]),2)/(2*pow(par[0],2)));}
	else {gaus_piece = par[2*i+1]*TMath::Exp(-pow(x[0]-par[2*i+2],2)/(2*pow(par[0],2)));}
        
	  gauss_final = gauss_final + gaus_piece;
      } 
      }
      if(wpol1==0){return gauss_final;}
      else {return gauss_final+par[npeaks*2+1]*x[0] + par[npeaks*2+2];}
    };
    return ret_fit_func;
  };
  
  //You could use only one function by directly capturing peaknum and pol1;
  
  fit = new TF1("fit_func",fit_function(peaknum,pol1),xmin,xmax,(pol1 ? npar : npar-2));
  
  if (peaknum == 2){
    if(pol1 == 1){fit->SetParameters(width,amp1,mean1,amp2,mean2,0,0);}
    else {fit->SetParameters(width,amp1,mean1,amp2,mean2);}
  fit->SetParLimits(2,mean1-0.1,mean1+0.1);
  fit->SetParLimits(4,mean2-0.1,mean2+0.1);
  
  fit->SetParLimits(1,0,100000);
  fit->SetParLimits(3,0,100000);
  } 
  else if (peaknum == 3){
    if(pol1 == 1){fit->SetParameters(width,amp1,mean1,amp2,mean2,amp3,mean3,0,0);}
    else {fit->SetParameters(width,amp1,mean1,amp2,mean2,amp3,mean3);}
  fit->SetParLimits(2,mean1-0.1,mean1+0.1);
  fit->SetParLimits(4,mean2-0.1,mean2+0.1);
  fit->SetParLimits(6,mean3-0.1,mean3+0.1);
  
  fit->SetParLimits(1,0,100000);
  fit->SetParLimits(3,0,100000);
  fit->SetParLimits(5,0,100000); 
  }
  else if (peaknum==4) {
  if (pol1==1) {fit->SetParameters(width,amp1,mean1,amp2,mean2,amp3,mean3,amp4,mean4,0,0);}
  else {fit->SetParameters(width,amp1,mean1,amp2,mean2,amp3,mean3,amp4,mean4);}
  
  fit->SetParLimits(2,mean1-0.1,mean1+0.1);
  fit->SetParLimits(4,mean2-0.1,mean2+0.1);
  fit->SetParLimits(6,mean3-0.1,mean3+0.1);
  fit->SetParLimits(8,mean4-0.1,mean4+0.1);
  
  fit->SetParLimits(1,0,100000);
  fit->SetParLimits(3,0,100000);
  fit->SetParLimits(5,0,100000);
  fit->SetParLimits(7,0,100000);
  }

  
  
  fit->SetParLimits(0,0.01,0.3);
  histo->Fit(fit,"M","",xmin,xmax);
  float binwidth = histo->GetXaxis()->GetBinWidth(2);

  vector<float> integral_vals = GSIntegralFit(fit,peaknum,xmin,xmax,binwidth); 
  
  for (int i=0; i<integral_vals.size(); i++)
    {
     float error_from_integral = TMath::Sqrt(integral_vals[i]);
     cout<<"The error obtained from taking the sqrt of the integral for peak number"<<i+1<<" is: "<<error_from_integral<<endl;
     
     double amp = fit->GetParameter(2*i+1);
     double sig = fit->GetParameter(0);
     
     double eamp = fit->GetParError(2*i+1);
     double esig = fit->GetParError(0);
     
     float error_from_calc = TMath::Sqrt(2.0*TMath::Pi())*TMath::Sqrt(pow((eamp/amp),2.0)+pow((esig/sig),2))*integral_vals[i];
     
     cout<<"The error obtained from our formula is: "<<error_from_calc<<endl;
     cout<<"------------------------------------------------------------------------"<<endl;
    }
  
  return;
}



template<typename T1, typename T2> void ExEnFit5(TH1D *histo,T1 xmin, T1 xmax, T2 mean1, T2 amp1, T2 mean2, T2 amp2, T2 mean3, T2 amp3, T2 mean4, T2 amp4, T2 mean5, T2 amp5, T2 width, bool bg = 1)
{
  TF1* fit;
  if(bg ==1){
    fit = new TF1("fit_func",gaus_fit_peak5,xmin,xmax,13);
  }
  else {fit = new TF1("fit_func", gaus_fit_peak5_nopol1,xmin,xmax,11);}
  
  fit->SetParameter(0,width);
  fit->SetParameter(1,amp1);
  fit->SetParameter(2,mean1);
  fit->SetParameter(3,amp2);
  fit->SetParameter(4,mean2);
  fit->SetParameter(5,amp3);
  fit->SetParameter(6,mean3);
  fit->SetParameter(7,amp4);
  fit->SetParameter(8,mean4);
  fit->SetParameter(9,amp5);
  fit->SetParameter(10,mean5);
  fit->SetParameter(11,0);
  fit->SetParameter(12,0);
  
  fit->SetParLimits(2,mean1-0.1,mean1+0.1);
  fit->SetParLimits(4,mean2-0.1,mean2+0.1);
  fit->SetParLimits(6,mean3-0.1,mean3+0.1);
  fit->SetParLimits(8,mean4-0.1,mean4+0.1);
  fit->SetParLimits(10,mean5-0.1,mean5+0.1);
  
  fit->SetParLimits(1,0,100000);
  fit->SetParLimits(3,0,100000);
  fit->SetParLimits(5,0,100000);
  fit->SetParLimits(7,0,100000);
  fit->SetParLimits(9,0,100000);
  
  fit->SetParLimits(0,0.1,0.3);
  
  histo->Fit(fit,"M","",xmin,xmax);

  vector<float> integral_vals = GSIntegralFit(fit,5,xmin,xmax); 
  
  return;
}

template<typename T1, typename T2> void ExEnFit6(TH1D *histo,T1 xmin, T1 xmax, T2 mean1, T2 amp1, T2 mean2, T2 amp2, T2 mean3, T2 amp3, T2 mean4, T2 amp4, T2 mean5, T2 amp5,T2 mean6, T2 amp6, T2 width, bool bg = 1)
{
  TF1* fit;
  
  if (bg == 1){
  fit = new TF1("fit_func",gaus_fit_peak6,xmin,xmax,13);
  }
  else {fit = new TF1("fit_func",gaus_fit_peak6_nopol1,xmin,xmax,15);}
  
  fit->SetParameter(0,width);
  fit->SetParameter(1,amp1);
  fit->SetParameter(2,mean1);
  fit->SetParameter(3,amp2);
  fit->SetParameter(4,mean2);
  fit->SetParameter(5,amp3);
  fit->SetParameter(6,mean3);
  fit->SetParameter(7,amp4);
  fit->SetParameter(8,mean4);
  fit->SetParameter(9,amp5);
  fit->SetParameter(10,mean5);
  fit->SetParameter(11,amp6);
  fit->SetParameter(12,mean6);
  if (bg == 0){
  fit->SetParameter(13,0);
  fit->SetParameter(14,0);
  }
  
  fit->SetParLimits(2,mean1-0.1,mean1+0.1);
  fit->SetParLimits(4,mean2-0.1,mean2+0.1);
  fit->SetParLimits(6,mean3-0.1,mean3+0.1);
  fit->SetParLimits(8,mean4-0.1,mean4+0.1);
  fit->SetParLimits(10,mean5-0.1,mean5+0.1);
  fit->SetParLimits(12,mean6-0.1,mean6+0.1);
  
  fit->SetParLimits(1,0,100000);
  fit->SetParLimits(3,0,100000);
  fit->SetParLimits(5,0,100000);
  fit->SetParLimits(7,0,100000);
  fit->SetParLimits(9,0,100000);
  fit->SetParLimits(11,0,100000);
  
  fit->SetParLimits(0,0.1,0.3);
  
  histo->Fit(fit,"M","",xmin,xmax);

  vector<float> integral_vals = GSIntegralFit(fit,6,xmin,xmax); 
  
  return;
}

template<typename T1, typename T2> void ExEnFit7(TH1D *histo,T1 xmin, T1 xmax, T2 mean1, T2 amp1, T2 mean2, T2 amp2, T2 mean3, T2 amp3, T2 mean4, T2 amp4, T2 mean5, T2 amp5,T2 mean6, T2 amp6,T2 mean7, T2 amp7, T2 width, bool bg = 1)
{
  TF1* fit;
  
  if (bg == 1){
  fit = new TF1("fit_func",gaus_fit_peak7,xmin,xmax,13);
  }
  else {fit = new TF1("fit_func",gaus_fit_peak7_nopol1,xmin,xmax,15);}
  
  fit->SetParameter(0,width);
  fit->SetParameter(1,amp1);
  fit->SetParameter(2,mean1);
  fit->SetParameter(3,amp2);
  fit->SetParameter(4,mean2);
  fit->SetParameter(5,amp3);
  fit->SetParameter(6,mean3);
  fit->SetParameter(7,amp4);
  fit->SetParameter(8,mean4);
  fit->SetParameter(9,amp5);
  fit->SetParameter(10,mean5);
  fit->SetParameter(11,amp6);
  fit->SetParameter(12,mean6);
  fit->SetParameter(13,amp7);
  fit->SetParameter(14,mean7);
  if (bg == 0){
  fit->SetParameter(15,0);
  fit->SetParameter(16,0);
  }
  
  fit->SetParLimits(2,mean1-0.1,mean1+0.1);
  fit->SetParLimits(4,mean2-0.1,mean2+0.1);
  fit->SetParLimits(6,mean3-0.1,mean3+0.1);
  fit->SetParLimits(8,mean4-0.1,mean4+0.1);
  fit->SetParLimits(10,mean5-0.1,mean5+0.1);
  fit->SetParLimits(12,mean6-0.1,mean6+0.1);
  fit->SetParLimits(14,mean7-0.1,mean7+0.1);
  
  fit->SetParLimits(1,0,100000);
  fit->SetParLimits(3,0,100000);
  fit->SetParLimits(5,0,100000);
  fit->SetParLimits(7,0,100000);
  fit->SetParLimits(9,0,100000);
  fit->SetParLimits(11,0,100000);
  fit->SetParLimits(13,0,100000);
  
  fit->SetParLimits(0,0.1,0.3);
  
  histo->Fit(fit,"M","",xmin,xmax);

  vector<float> integral_vals = GSIntegralFit(fit,7,xmin,xmax); 
  
  return;
}

template<typename T> void GetSectionsRootfile(TH2F* histo, string newfilename, T first_angle, T second_angle, T slice_size, float yaxis_0 = 20.0, float yaxis_f = 20.0, float slice_ymax = 100.0, bool bgsubtraction = 0, int coarse=15 )
{ 
  int number_slices = round((second_angle-first_angle)/slice_size);
  
  TFile* file = new TFile(newfilename.c_str(),"recreate");
  
  TH1* slice;
  TH1* rslice;
  
  TSpectrum* spec = new TSpectrum();
  TH1* bg;
  
  histo->GetYaxis()->SetRangeUser(yaxis_0,yaxis_f);
  
  for(int i=0; i<number_slices; i++)
  {
    T fangle = first_angle+slice_size*i;
    T langle = fangle+slice_size;
    
    int fbin = histo->GetXaxis()->FindBin(fangle);
    int lbin = histo->GetXaxis()->FindBin(langle);
    
    //cout<<"fbin "<<fbin<<" lbin "<<lbin<<endl;
    
    string slice_name = "slice" + to_string(i);
    string rebinned_slice_name = slice_name+"r";
    
    slice = histo->ProjectionY(slice_name.c_str(),fbin,lbin); 
    
    
    if (bgsubtraction == 0){
      bg = spec->Background(slice,coarse );
      slice->Add(bg,-1);
      slice->GetYaxis()->SetRangeUser(0.,slice_ymax);
    }
    
    rslice=slice->Rebin(4,rebinned_slice_name.c_str());
    
    slice->Write();
    rslice->Write();
    
  }
  
  file->Close();
  
  return;
}

	
void DrawAngleComp(vector<float> peakmeans,float first_angle, float second_angle, float slice_size)
{
  vector<vector<float>> peak_ints;
  vector<vector<float>> epeak_ints;
  float value;
  float eval;
  float range = -first_angle+second_angle;
  int num_slices = range/slice_size;
  vector<float> angles;
  vector<float> eangl;
  vector<float> int_v;
  vector<float>int_ev;
  vector<TGraph*> graphs;

  
  for (int i =0; i<peakmeans.size(); i++)
  {
    for(int p=0; p<num_slices;p++)
    {
    cout<<"Enter the "<<p<<" value for the peak at:"<<peakmeans[i]<<endl;
    cin>>value;
    cout<<"Enter the error associated with "<<value<<":"<<endl;
    cin>>eval;
    int_v.push_back(value);
    int_ev.push_back(eval);
    }
    peak_ints.push_back(int_v);
    epeak_ints.push_back(int_ev);
    int_v.clear();
    int_ev.clear();
  }
  
  for (int j = 0; j<num_slices; j++)
  {
    angles.push_back(first_angle + j*slice_size);
    eangl.push_back(0.5);
    
  }

  TCanvas *c1 = new TCanvas("c1","Angle Comparison Graph",200,10,700,500);
  auto legend = new TLegend(0.1,0.7,0.48,0.9);
  
  for (int h = 0; h<peakmeans.size();h++)
  {
    TGraphErrors *gr = new TGraphErrors(angles.size(),&angles[0],&peak_ints[h][0],&eangl[0],&epeak_ints[h][0]);
    graphs.push_back(gr);
  }
  
   graphs[0]->SetTitle("Angle Comparison Graph");
   graphs[0]->SetMarkerColor(1);
   graphs[0]->SetMarkerStyle(21);
   graphs[0]->Draw("ALP");
   char thing[20];
   const char lable0 = sprintf(thing,"%.2f angle entry",peakmeans[0]);
   legend->AddEntry(graphs[0],&lable0,"lep");
   legend->Draw();
  
  for (int l =1; l<graphs.size();l++)
  {
   graphs[l]->SetMarkerColor(l+1);
   graphs[l]->SetMarkerStyle(21);
   graphs[l]->Draw("LPsame");
   char thing2[20];
   const char lable = sprintf(thing2,"%.2f angle entry",peakmeans[l]);
   legend->AddEntry(graphs[l],&lable,"lep");
   legend->Draw();
   
  }

   c1->Update();
   
   return;
}





