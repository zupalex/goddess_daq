#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

using namespace std;


void PrintPlots(std::string outputDir, int num){

  TFile* curr = new TFile( Form("rootfiles/run%i.root",num) );

  TH2F* god_energy = (TH2F*) curr->Get("hists/dgod/god_en");
  TH2F* dtg_digital_god = (TH2F*) curr->Get("hists/dgod/dTg_god");
  
  TH2F* agod_energy = (TH2F*)curr->Get("hists/agod/agod_en");
  TH2F* dtg_analog_god = (TH2F*)curr->Get("hists/agod/dTg_agod");
 
  TH2F* dgs_ehi = (TH2F*)curr->Get("hists/dgs/EhiRaw");

  TCanvas* c = new TCanvas(Form("GODDESS_RUN%i",num),Form("GODDESS_RUN%i",num),800,800);
  c->Divide(2,2);
  c->cd(1);
  gPad->SetLogz();
  god_energy->Draw("colz");
  c->cd(2);
  gPad->SetLogz();
  agod_energy->Draw("colz");
  c->cd(3);
  gPad->SetLogz();
  dtg_digital_god->Draw("colz");
  c->cd(4);
  gPad->SetLogz();
  dtg_analog_god->Draw("colz");

  c->Print(Form("Plots_run%i.pdf(",num));

  c->Clear();
  c->Divide(1,1);
  gPad->SetLogz();
  dgs_ehi->Draw("colz");

  c->Print(Form("Plots_run%i.pdf)",num));


}
