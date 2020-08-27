#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "TH1.h"
#include "/mnt/d/Ubuntu/goddess_daq/goddess/include/GoddessStruct.h"
#include "/mnt/d/Ubuntu/goddess_daq/toolbox/include/GoddessAnalysis.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TRandom.h"
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include "goddess_analysis_macros.h"
#include "GoddessAnalysis.h"

double rel_q_value(double psi, double T3)
{
	double q_val;
	double M1 = 133.9053947*931.5, M2 = 1876.134, M3 = 938.766;
	double P1, P3;
	double E1, E3;
	double T1 = 1337.7; //after energy loss of beam through half of the target 3769.81

	E1 = T1 + M1;
	E3 = T3 + M3;
	P1 = sqrt((E1 * E1) - (M1 * M1));
	P3 = sqrt((E3 * E3) - (M3 * M3));

	q_val = M1 + M2 - M3 - sqrt((M1 * M1) + (M2 * M2) + (M3 * M3) + 2 * M2 * E1 - 2 * E3 * (E1 + M2) + 2 * P1 * P3 * cos(psi * (3.1415 / 180.0)));

	return q_val;
}

void CalibrationCheck(string run)
{
	/*File INPUT  */
	TFile* f = TFile::Open(run.c_str());
	//cout << "Rundgs" << endl;

/*get the trees*/
	TTree* data = (TTree*)f->Get("god");
	TLeaf* timeS = data->GetLeaf("timestamp");
	

/*Creating histograms to be filled later*/
	TH2D* Paw_graph = new TH2D("Paw_graph", "Ex vs. Pos", 200, -100, 100, 600, 0, 200);
	/*Upstream Sx3*/
	TH2D* CalibCheckus = new TH2D("CalibCheckus", "Calibration Check", 400, 70400, 70800, 1000, 0, 14);
	TH2D* CalibPosCheckus = new TH2D("CalibPosCheckus", "Calibration Position Check", 400, 70400, 70800, 1000, -200, 200);

	/*Downstream Sx3*/
	TH2D* CalibCheckds = new TH2D("CalibCheckds", "Calibration Check", 400, 38400, 38800, 1000, 0, 14);

	/*Upstream QQQ5*/
	TH2D* CalibCheckuq = new TH2D("CalibCheckuq", "Calibration Check", 400, 38400, 38800, 1000, 0, 14);


/* Read det branches from all target file and tree*/
	TTreeReader myReader("god", f);

	/*Variables definition*/
	float gamE;
	int gam;
	int gamnum;
	float qval;
	float Qvaltest;
	float TEST;
	float qvalue=4.13408;
	float Ex;
	double counter;
	double counter1;
	float blahwa;
	double siVecsize;
	double gamVecsize;
	int detid;
	double E11;
	double angle;
	double dE;
	double position;
	float mejec = 1.00782503223*931.5;
	float mbeam = 133.9053947*931.5;
	float mrecoil = 134.907228*931.5;
	unsigned long long int nentries = data->GetEntries(); //get all entries
	cout << "n entries " << nentries << endl;

	/*Specific GODDESS DAQ variables (Si information and Gammasphere info - from GoddessStruct and GoddessData or embeded via the sort code)*/
	std::vector<SiDataDetailed>* si; //declare the pointer
	si = new std::vector<SiDataDetailed>; // allocate memory to it
	data->SetBranchAddress("si", &si);
	vector<GamData>* vectGamData = new vector<GamData>();
	data->SetBranchAddress("gam", &vectGamData);
	vector<float>* timetest = new vector<float>();
	GoddessReacInfos* grinfo = new GoddessReacInfos();

	/*Looping over entries*/
	for (unsigned long long int i = 0; i < nentries / 10; i++) // Read ALL Entries
	{
		if (i % 1000000 == 0) std::cout << "Event: " << i << std::endl;
		data->GetEntry(i);
		siVecsize = si->size(); 
		gamVecsize = vectGamData->size();
		vector<float> esum(siVecsize);


		/*Analysis of Silicon branch and Gamma branch -  see details as code goes*/
		for (int j = 0; j < siVecsize; j++)
		{
			SiDataDetailed si_test = si->at(j);
			/*get the detector id*/
			detid = si_test.telescopeID;
			position = si_test.PosE1().z();
			angle = si_test.Angle(1);
			//cout << position << endl;
			//cout << position << endl;
			/*Get the energies, in a layer E1, E2 or dE from front and backsides as well as angle such as described below (from Alex's code)*/
			/*false == front or p-side && true == back or n-side */
			/*Angle access the angle of the event -- Angle is in the center of mass and in degrees*/
			E11 = si_test.ESumLayer(1, false);
			dE = si_test.ESumLayer(1, true);
			int Strip = si_test.StripMaxLayer(1, false) + (32 * detid);
			if ((70431 < Strip && Strip < 7070433)&&(position != 0 && angle != 0)) Paw_graph->Fill(position,E11);

		
		
			CalibCheckus->Fill(Strip, E11);
			CalibCheckuq->Fill(Strip, E11);
			CalibPosCheckus->Fill(Strip, position);
		}
	}	

	/*Line Calibration check at 5.8 MeV*/
	TLine* line = new TLine(-200,5.8133, 81000, 5.8133); // 492 Cf 5.8133
	line->SetLineColor(kBlack);
	TCanvas* posZ = new TCanvas("posZ", "Position test", 400, 300);
	posZ->cd();
	Paw_graph->Draw("colz");

	TCanvas* posi1 = new TCanvas("posi1", "Calibration test", 400, 300);
	posi1->cd();
	CalibPosCheckus->Draw("colz");
	//line->Draw("same");
	posi1->Print("ReadGetTTree_result/CalibrationPosCheck_Sx3.pdf");

	TCanvas* calib1 = new TCanvas("calib1", "Sx3 : Calibration test", 400, 300);
	calib1->cd();
	CalibCheckus->Draw("colz");
	line->Draw("same");
	calib1->Print("ReadGetTTree_result/CalibrationCheck_Sx3.pdf");
	TCanvas* calib2 = new TCanvas("calib2", "QQQ5 : Calibration test", 400, 300);
	calib2->cd();
	CalibCheckuq->Draw("colz");
	line->Draw("same");
	calib2->Print("ReadGetTTree_result/CalibrationCheck_QQQ5.pdf");

return;
}
