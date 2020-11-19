
#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "TH1.h"
#include "goddess_daq/goddess/include/GoddessStruct.h"
#include "goddess_daq/toolbox/include/GoddessAnalysis.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TRandom.h"
#include <TTreeReader.h>
#include <TTreeReaderArray.h>


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

void Read_GetTTrees()
{
/*File INPUT  */
	TFile* f = TFile::Open("/mnt/d/run02052020.root");
	//cout << "Rundgs" << endl; 

/*get the trees*/
	TTree* data = (TTree*)f->Get("god");
	TLeaf* timeS = data->GetLeaf("timestamp");
	
/**/
	ofstream Harrysfile;
	Harrysfile.open("Harrysfile.txt");

/*Creating histograms to be filled later*/
	/* All Purpose*/
	TH1D* timefilters = new TH1D("timedisigams", "Time All DetID", 800, 0, 800);
	TH1D* a1 = new TH1D("a1", "Gate Gamma _ Plot protons 1", 2000, 0, 16);
	TH1D* a2 = new TH1D("a2", "Gate Gamma _ Plot protons 2", 2000, 0, 16);
	TH1D* ad = new TH1D("ad", "Gate Gamma _ Plot protons 3", 2000, 0, 16);
	TH2D* Pid = new TH2D("Pid", "E vs. dE", 400, 0, 20, 400, 0, 20);
	TH2D* PidSect1 = new TH2D("PidSect1", "dE vs. E for strip in barrel 1", 400, 0, 20, 400, 0, 20);
	TH2D* Pid2 = new TH2D("Pid2", "dE vs. E for strip in barrel 2", 400, 0, 20, 400, 0, 20);
	TH2D* ExvAngle = new TH2D("ExvAngle", "Ex vs. Angle", 600, 0, 200, 160, -0.5, 7);
	TH2D* Evpos = new TH2D("Evpos", "Ex vs. Pos", 600, 0, 200, 200, -100, 100);
	TH2D* GamMatrix = new TH2D("GamMatrix", "Gammas vs Ex : all detector", 350, -0.5, 7, 3500, 0, 3500);
	TH1D* GammaPlot = new TH1D("GammaPlot", "Gamma Plot", 2500, 0, 5000);
	TH1D* GammaOri = new TH1D("GammaOri", "Gamma Plot Original - All gammas", 2500, 0, 5000);
	TH1D* Copy = new TH1D("Copy", "Copy plot", 10000, 0, 10000);
	TH1D* gamdet = new TH1D("gamdet", "Detector gamma", 120, 0, 119);

	/*Upstream Sx3*/
	TH1D* f1us = new TH1D("f1us", "Time All DetID", 1000, 0, 1000);
	TH1D* f2us = new TH1D("f2us", "Sx3 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f5us = new TH1D("f5us", "Sx3 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f6us = new TH1D("f6us", "Sx3 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f7us = new TH1D("f7us", "Sx3 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f8us = new TH1D("f8us", "Sx3 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f9us = new TH1D("f9us", "Sx3 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f10us = new TH1D("f10us", "Sx3 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f4us = new TH1D("f4us", "Sx3 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f3us = new TH1D("f3us", "Y projection Gam vs Ex", 500, 0, 6000);
	TH1D* GamEus = new TH1D("GamEus", "X projection Gam vs Ex", 5000, 0, 6000);
	TH1D* Exus = new TH1D("Exus", "Y projection Gam vs Ex", 150, -2, 10);
	TH1D* Exus_O = new TH1D();
	TH2D* TotvAngleus = new TH2D("TotvAngleus", "Tot vs. Angle",600,0,200, 400, 0, 20);
	TH2D* ExvAngleus = new TH2D("ExvAngleus", "Ex vs. Angle", 600, 0, 200,160, -0.5, 7);
	TH2D* GamvsExus = new TH2D("GamvsExus", "Sx3 U0 - Strip 2 & 3: Gam vs. Ex", 350, -0.5, 7, 3500, 0, 3500);
	TH1D* Gamdd = new TH1D("Gamdd", "134Xe(d,d) Gam ", 2500, 0, 5000);
	TH1D* Gamdp = new TH1D("Gamdp", "134Xe(d,p) Gam ", 2500, 0, 5000);
	TH1D* Multiplicityus = new TH1D("Multiplicityus", " Gamma Multiplicity distribution", -1, 0, 9);

	/*Downstream Sx3*/
	TH2D* TotvAngleds = new TH2D("TotvAngleds", "Tot vs. Angle", 600, 0, 200, 400, 0, 20);
	TH2D* GamvsExds = new TH2D("GamvsExds", "Gam vs. Ex", 5000, 0, 5000, 4000, 0, 14);
	TH2D* ExvAngleds = new TH2D("ExvAngleds", "Ex vs. Angle", 600, 0, 200, 100, -1, 15);
	TH1D* f1ds = new TH1D("f1ds", "Time All DetID", 1000, 0, 1000);
	TH1D* f3ds = new TH1D("f3ds", "Y projection Gam vs Ex", 10000, 0, 7000);

	/*Upstream QQQ5*/
	TH2D* TotvAngleuq = new TH2D("TotvAngleuq", "Tot vs. Angle", 600, 0, 200, 400, 0, 20);
	TH2D* GamvsExuq = new TH2D("GamvsExuq", "QQQ5 D: Gam vs. Ex", 350, -0.5, 7, 3500, 0, 3500);
	TH2D* GamvsExuqdigi = new TH2D("GamvsExuqdigi", "QQQ5 A: Gam vs. Ex", 350, -0.5, 7, 3500, 0, 3500);
	TH2D* GamMatrixq5 = new TH2D("GamMatrixq5", "Gammas vs Ex : QQQ5 A & C", 350, -0.5, 7, 3500, 0, 3500);
	TH2D* ExvAngleuq = new TH2D("ExvAngleuq", "Ex vs. Angle", 600, 0, 200, 160, -0.5, 7);
	TH1D* f2uq = new TH1D("f2uq", "QQQ5 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f5uq = new TH1D("f5uq", "QQQ5 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f6uq = new TH1D("f6uq", "QQQ5 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f7uq = new TH1D("f7uq", "QQQ5 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f8uq = new TH1D("f8uq", "QQQ5 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f9uq = new TH1D("f9uq", "QQQ5 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f10uq = new TH1D("f10uq", "QQQ5 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f4uq = new TH1D("f4uq", "QQQ5 - Gammas - Ex gated", 2000, 0, 3500);
	TH1D* f3uq = new TH1D("f3uq", "Y projection Gam vs Ex", 10000, 0, 7000);
	TH1D* Gamdduq = new TH1D("Gamdd", "134Xe(d,d) Gam ", 2500, 0, 5000);
	TH1D* Gamdpuq = new TH1D("Gamdp", "134Xe(d,p) Gam ", 2500, 0, 5000);
	TH1D* Gamddud = new TH1D("Gamdd", "134Xe(d,d) Gam ", 2500, 0, 5000);
	TH1D* Gamdpud = new TH1D("Gamdp", "134Xe(d,p) Gam ", 2500, 0, 5000);

/* Read det branches from all target file and tree*/
	//TTreeReader myReader("god", f);

/*******************************************************************************************************************************************************/
/*                                                            Kinematic lines - 135Xe                                                                  */
/*******************************************************************************************************************************************************/
	//expected ground state kinematics
	ifstream gState_kinematics("KatKin.txt");
	

	string line;
	Int_t u = 0;
	Double_t xK0[190];
	Double_t yK0[190]; // Ground State
	Double_t yK1[190]; // 1st Ex
	Double_t yK2[190]; // 2nd Ex
	Double_t yK3[190]; // 3rd Ex
	Double_t yK4[190]; // 4th Ex
	Double_t yKl[190]; // last Ex before continuum

	while (getline(gState_kinematics, line))
	  {
		istringstream in(line);
		in >> xK0[u] >> yK0[u]>> yK1[u]>>yK2[u]>>yK3[u]>>yK4[u]>>yKl[u];
		u++;
	}
	TGraph* gStateKin = new TGraph(u, xK0, yK0);
	TGraph* gStateKin1 = new TGraph(u, xK0, yK1);
	TGraph* gStateKin2 = new TGraph(u, xK0, yK2);
	TGraph* gStateKin3 = new TGraph(u, xK0, yK3);
	TGraph* gStateKin4 = new TGraph(u, xK0, yK4);
	TGraph* gStateKinl = new TGraph(u, xK0, yKl);

	/*******************************************************************************************************************************************************/
/*                                                              Elastic Scattering                                                                     */
/*******************************************************************************************************************************************************/
	//expected ground state kinematics
	ifstream gState_elastics("elasticscat.txt");


	string lineS;
	Int_t v = 0;
	Double_t xE0[190];
	Double_t yE0[190]; // Ground State
	Double_t yE1[190]; // Ground State


	while (getline(gState_elastics, lineS))
	{
		istringstream in(lineS);
		in >> xE0[v] >> yE0[v] >> yE1[v];
		v++;
	}
	TGraph* gStateElas = new TGraph(v, xE0, yE0);
	TGraph* gStateElas1 = new TGraph(v, xE0, yE1);
	
	/*******************************************************************************************************************************************************/
	/*                                                               Variables definition                                                                  */
	/*******************************************************************************************************************************************************/
	int detid;
	int gamNum;
	int gamType;
	int gam;
	int gamnum;

	float gamE;
	float qval;
	float Qvaltest;
	float TEST;
	float Ex;
	float mejec = 1.00782503223 * 931.5;
	float mbeam = 133.9053947 * 931.5;
	float mrecoil = 134.907228 * 931.5;
	float qvalue = 4.13408;

	double siVecsize;
	double gamVecsize;
	double dE; double dE1;
	double E1_backside;
	double E2_backside;
	double E11;
	double E2; double E21;
	double E;
	double Etot; double Etot1;
	double EEE;
	double timeg;
	double timesi;
	double timediff;
	double timediffs;
	double time1;
	double time2;
	double time3;

	int counter;
	int counter1;
	int truth;
		
	unsigned long long int nentries = data->GetEntries(); //get all entries
	cout << "n entries " << nentries << endl;

	/*******************************************************************************************************************************************************/
	/*      Specific GODDESS DAQ variables (Si information and Gammasphere info - from GoddessStruct and GoddessData or embeded via the sort code)         */
	/*******************************************************************************************************************************************************/
	std::vector<SiDataDetailed>* si; //declare the pointer
	si = new std::vector<SiDataDetailed>; // allocate memory to it
	data->SetBranchAddress("si", &si);
	vector<GamData>* vectGamData = new vector<GamData>();
	data->SetBranchAddress("gam", &vectGamData);
	vector<float>* timetest = new vector<float>();
	GoddessReacInfos* grinfo = new GoddessReacInfos();

	/*******************************************************************************************************************************************************/
	/*                                                               Getting & Analising DATA                                                              */
	/*******************************************************************************************************************************************************/
	//cout << " Only multiplicity one for gamma event! " << endl;
	/*Looping over entries*/
	for (unsigned long long int i = 0; i < nentries; i++) // Read ALL Entries
	{
		//if (i % 1000000 == 0) std::cout << "Event: " << i << std::endl;
		//Progress update
		if (floor(1.0 * i / 100000) == 1.0 * i / 100000)
		{
			std::cout << i << "  [";
			double prog_pos = 30 * (1.0 * i / nentries);
			for (int h = 0; h < 30; h++)
			{
				if (h <= prog_pos) std::cout << "=";
				else std::cout << " ";
			}
			std::cout << "]" << int((1.0 * i / nentries) * 100.0) << "%";
		}
		std::cout << "\r";


		data->GetEntry(i);
		siVecsize = si->size();
		gamVecsize = vectGamData->size();
		vector<float> esum(siVecsize);

		/*Acces to time */
		for (int g = 0; g < gamVecsize; g++)
		{
			GamData GSData = vectGamData->at(g);
			EEE = GSData.en;
			timeg = GSData.time;
			gamE = (GSData.en) / 3.;
			gamNum = GSData.num;
			gamType = GSData.type;
			//GammaPlot->Fill(gamE);
			if (gamNum == 82) {
			       
			}
		}

		/*Analysis of Silicon branch and Gamma branch -  see details as code goes*/
		for (int j = 0; j < siVecsize; j++)
		{
			SiDataDetailed si_test = si->at(j);
			/*get the detector id*/
			detid = si_test.telescopeID;
			/*Get the energies, in a layer E1, E2 or dE from front and backsides as well as angle such as described below (from Alex's code)*/
			/*false == front or p-side && true == back or n-side */
			/*Angle access the angle of the event -- Angle is in the center of mass and in degrees*/
			dE = si_test.ESumLayer(0, false);
			E11 = si_test.ESumLayer(1, false);
			E1_backside = si_test.ESumLayer(1, true);
			E2 = si_test.ESumLayer(2, false);
			E2_backside = si_test.ESumLayer(2, true);
			double e1_paw = si_test.PosE1().Z();
			double angle1 = si_test.Angle(1);
			double angle2 = si_test.PosE1().Angle({ 0,0,1 });

			Etot = E11;// dE + E11 + E2;

			/*ID of the telescope. 4 digits number.
			1st: Barrel (2) or not (1)
			2nd: Upstream (2) or not (1)
			3rd and 4th: Sector number (from 00 to 11)
			*/
			int Strip = si_test.StripMaxLayer(1, false) + (32 * detid);



			/************************************************************************************************************/
			/*                                                 QQQ5 - Front                                             */
			/************************************************************************************************************/
			if (detid == 1200 ) {// &&(Strip!=38415 && Strip != 38429 && Strip != 38431)) {

				Qvaltest = rel_q_value(angle1, Etot);
				Ex = (qvalue - Qvaltest); // Calculates the excitation energy
				ExvAngleuq->Fill(angle1, Ex);
				TotvAngleus->Fill(angle1, Etot);
			

				for (int h = 0; h < gamVecsize; h++) { // This is if there is a gamma
					GamData GSData = vectGamData->at(h);
					timediffs = (double)vectGamData->at(h).time - (double)si->at(j).TimestampMaxLayer(1, false);
					timefilters->Fill(timediffs); // Will allow to filter gamma event

					gamdet->Fill(gamVecsize);


					// gamType == 1 turns ON the compton suppression       && (gamType == 1)
					if ((((timediffs > 400 && timediffs < 450))) && gamE > 100 && gamType == 1) {

						GamMatrixq5->Fill(Ex, gamE);
						GamMatrix->Fill(Ex, gamE);
						
						cout<<Ex<<endl;
						cout<<gamE<<endl;
						
						GamvsExuqdigi->Fill(Ex, gamE);
						counter += 1;
						
					}
				}
			}
			if (detid == 1203){// &&(Strip!=38415 && Strip != 38429 && Strip != 38431)) {

				Qvaltest = rel_q_value(angle1, Etot);
				Ex = (qvalue - Qvaltest); // Calculates the excitation energy
				ExvAngleuq->Fill(angle1, Ex);
				TotvAngleus->Fill(angle1, Etot);

				for (int h = 0; h < gamVecsize; h++) { // This is if there is a gamma
					GamData GSData = vectGamData->at(h);
					timediffs = (double)vectGamData->at(h).time - (double)si->at(j).TimestampMaxLayer(1, false);
					timefilters->Fill(timediffs); // Will allow to filter gamma event
					
					gamdet->Fill(gamVecsize);
					

					// gamType == 1 turns ON the compton suppression       && (gamType == 1)
					if ((((timediffs > 400 && timediffs < 450) ) ) && gamE > 100 && gamType==1) {

						GamMatrixq5->Fill(Ex, gamE);
						GamMatrix->Fill(Ex, gamE);

						GamvsExuq->Fill(Ex, gamE);
						counter1 += 1;
						
						
						// Loop through gamma rays for one event  -- Bit by Josh Hooker - UTK postdoc
						
						double gamE_h = (GSData.en) / 3.;
						int gamNum_h = GSData.num;
						GammaOri->Fill(gamE_h);
						//counter += 1;
						// Check if gamma energy is inside of the gate
							
						// Loop through gamma rays again
						for (int k = 0; k < gamVecsize; k++) {
							GamData GSData_k = vectGamData->at(k);
							double gamE_k = (GSData_k.en) / 3.;
							int gamNum_k = GSData_k.num;
							//counter1 += 1;
							if ((gamE_h > 278 && gamE_h < 298) && gamNum_k) {
								//if (j != k) continue; // don’t use the same gamma ray
								if (gamNum_h == gamNum_k) continue; // make sure it’s a different detector
								//truth += 1;
								GammaPlot->Fill(gamE_k);
							}
						}
						

						
						
							
						//if (gamE > 2198 && gamE < 2202) cout << " Proton Energy " << Ex << " Gamma Energy " << gamE << " Ge name " << gamNum << endl;
						if (Ex < 6.364) Gamdpuq->Fill(gamE);
						else Gamdduq->Fill(gamE);
						
						counter += 1;
							/*Protons gate - plotting gammas*/
						
						f4uq->Fill(gamE);
						if (gamType == 1) f2uq->Fill(gamE);

						//if (Ex > 1.5 && Ex < 2.0) f2uq->Fill(gamE);
						//if (Ex > 3.0 && Ex < 4.0) f4uq->Fill(gamE);
						//if (Ex >= 4.0 && Ex < 4.5) f4uq->Fill(gamE);
						if (Ex >= 4.5 && Ex < 4.7) f5uq->Fill(gamE);
						if (Ex >= 4.7 && Ex < 5.0) f7uq->Fill(gamE);
						if (Ex >= 5.0 && Ex < 5.4) f8uq->Fill(gamE);
						if (Ex >= 4.5 && Ex < 5.4) f9uq->Fill(gamE);
						if (Ex >= 2 && Ex < 2.7) f6uq->Fill(gamE);
						if (Ex >= -0.6 && Ex < 1.0) f10uq->Fill(gamE);
					}
				}

			} 
			
			/****************************************************************************************************************************************/
			/*                                                             Sx3                                                                      */
			/****************************************************************************************************************************************/

			// Upstream Sx3 - Front    || (70688 < Strip && Strip < 70691) || (70751 < Strip && Strip < 70756)
			// U0 (70400 < Strip && Strip < 70403) - Analog  - (timediffs > 410 && timediffs < 450)
			// U3 (70498 < Strip && Strip < 70500) - Digital - ((timediffs >65 && timediffs < 90) || (timediffs > 185 && timediffs < 215))
			if (Strip == 70401 || Strip == 70402) {
				double angle_new = 180 - (180/3.14159) * atan(  (89.713+6) / (abs(e1_paw)+6)   ); 
				Qvaltest = rel_q_value(angle_new, Etot);
				Ex = (qvalue - Qvaltest); // Calculates the excitation energy
				ExvAngle->Fill(angle_new, Ex);
				TotvAngleus->Fill(angle_new, Etot);
				Evpos->Fill(Etot, e1_paw);
				//GamvsExuq->Fill(E11, detid);
				//if (Etot > 3) Harrysfile << Etot << "  " << e1_paw << "\n";
				

				for (int h = 0; h < gamVecsize; h++) { // This is if there is a gamma
					GamData GSData = vectGamData->at(h);
					timediffs = (double)vectGamData->at(h).time - (double)si->at(j).TimestampMaxLayer(1, false);
					timefilters->Fill(timediffs); // Will allow to filter gamma event
					
									
					// gamType == 1 turns ON the compton suppression
					if ( ((timediffs > 400 && timediffs < 450)&&(gamType==1)) && gamE > 50) {
						GamvsExus->Fill(Ex, gamE);
						GamMatrix->Fill(Ex, gamE);
						if (gamVecsize == 1 || gamVecsize == 2 || gamVecsize == 3) {
							if (Ex < 6.364) Gamdp->Fill(gamE);
							/*Gamma gate - plotting protons*/
							if (gamE > 282 && gamE < 295) ad->Fill(Ex);
							if (gamE > 1500 && gamE < 1530) a2->Fill(Ex);
							if (gamE > 2100 && gamE < 2180) a1->Fill(Ex);
						}
						//counter1 += 1;
						/*Protons gate - plotting gammas*/
						
						if (Ex > 1.5 && Ex < 2) f2us->Fill(gamE);
						if (Ex > 3.0 && Ex < 4.2) f4us->Fill(gamE);
						if (Ex >= 4.5 && Ex < 5.4) f5us->Fill(gamE);
						if (Ex >= 5.3 && Ex < 5.6) f7us->Fill(gamE);
						if (Ex >= 5.6 && Ex < 6.3) f8us->Fill(gamE);
						if (Ex >= 2 && Ex < 2.7) f6us->Fill(gamE);
						if (Ex >= -0.4 && Ex < 1.0) f9us->Fill(gamE);
						else Gamdd->Fill(gamE);
					}
				}
			}
		}
	}
	cout << " QQQ5 A  " << counter <<"   QQQ5 D    "<< counter1 << endl;
	double lowlim = 0;
	double limy2 = 500;
	double limy1 = 500;
	double limy3 = 500;
	TLine* Paul = new TLine(0.288455, lowlim, 0.288455, limy1);
	TLine * Jon = new TLine(0.526551, lowlim, 0.526551, limy1);
	// The state discovered by Alex
	TLine * Mary = new TLine(2.040, lowlim, 2.040, limy2);
	TLine * Magda = new TLine(2.406, lowlim, 2.406, limy2);
	TLine * Jane = new TLine(2.836, lowlim, 2.836, limy2);
	TLine * Julie = new TLine(3.114, lowlim, 3.114, limy2);
	//Neutron separation energy
	TLine * Sn = new TLine(6.364, lowlim, 6.364, limy3);
	TLine* Marc = new TLine(6.364, 0, 6.364, 3500);

	TLine* Mia = new TLine(0, 0, 3.5, 3500);

	/***********************************************************************************************************************************************************/
	/*                                                                      Plots                                                                              */
	/***********************************************************************************************************************************************************/

	TCanvas* EvA = new TCanvas("EvA", "Etot vs. Angle", 400, 300);
	EvA->cd();
	TotvAngleus->Draw("colz");
	gStateKin->Draw("same");
	//gStateKin1->SetLineColor(kOrange+7);
	//gStateKin1->Draw("same");
	//gStateKin1->SetLineWidth(1);
	//gStateKin2->SetLineColor(kRed);
	//gStateKin2->Draw("same");
	//gStateKin3->SetLineColor(kViolet-5);
	//gStateKin3->Draw("same");
	//gStateKin4->Draw("same");
	//gStateKinl->Draw("same");
	/*TCanvas* EvAQ = new TCanvas("EvAQ", "QQQ5: Etot vs. Angle", 400, 300);
	EvAQ->cd();
	TotvAngleuq->Draw("colz");
	gStateKin->Draw("same");
	gStateKin1->SetLineColor(kOrange + 7);
	gStateKin1->Draw("same");
	gStateKin2->SetLineColor(kRed);
	gStateKin2->Draw("same");
	gStateKin3->SetLineColor(kViolet - 5);
	gStateKin3->Draw("same");*/

	/*TCanvas* thyme = new TCanvas("thyme", "Time difference", 400, 300);
	thyme->cd();
	timefilters->SetLineColor(kBlack);
	timefilters->Draw();*/
	//truth=timefilters->Integral(400, 450);
/*	cout << " Counter - QQQ5 event: " << counter << endl;
	cout << " Counter - Sx3 event: " << counter1 << endl;
	cout << " truth  " << truth << endl;
	//c1->Print("ReadGetTTree_result/Time_Difference.pdf");
	int integral1 = timefilters->Integral(400, 450);
	int integral2 = timefilters->Integral(70, 100);
	int integral3 = timefilters->Integral(185, 210);
	cout << integral2 + integral3 << "            " << integral1 << endl;*/

	/*TCanvas* Multiplicity = new TCanvas("Multiplicity", "Multiplicity Gamma", 400, 300);
	Multiplicity->cd();
	gamdet->SetLineColor(kBlack);
	gamdet->Draw();*/

	TCanvas* coincidencesgamma = new TCanvas("coincidencesgamma", "Gamma Coincidences", 400, 300);
	coincidencesgamma->cd();
	GammaPlot->SetLineColor(kBlack);
	//GammaOri->SetLineColor(kViolet);
	GammaPlot->Draw();
	//GammaOri->Draw("same");
	
	TCanvas* matrix1 = new TCanvas("Matrix1", "All upstream detectors : Gamma Energy vs. Excitation energy", 400, 300);
	matrix1->cd();
	Mia->SetLineColor(kPink + 9);
	Marc->SetLineColor(kViolet - 5);
	GamMatrix->Draw("COLZ");
	Marc->Draw("same");
	Mia->Draw("same");
	//matrix1->Print("ReadGetTTree_result/Gamma_vs_Ex_Sx3.pdf");

	TCanvas* matrix2 = new TCanvas("Matrix2", "Digital QQQ5 : Gamma Energy vs. Excitation energy", 400, 300);
	matrix2->cd();
	Mia->SetLineColor(kPink + 9);
	Marc->SetLineColor(kViolet - 5);
	GamvsExuqdigi->Draw("COLZ");
	Marc->Draw("same");
	Mia->Draw("same");
	//matrix2->Print("ReadGetTTree_result/Gamma_vs_Ex_Sx3.pdf");

	TCanvas* matrix3 = new TCanvas("Matrix3", "QQQ5 A & D : Gamma Energy vs. Excitation energy", 400, 300);
	matrix3->cd();
	Mia->SetLineColor(kPink + 9);
	Marc->SetLineColor(kViolet - 5);
	GamMatrixq5->Draw("COLZ");
	Marc->Draw("same");
	Mia->Draw("same");
	//matrix2->Print("ReadGetTTree_result/Gamma_vs_Ex_Sx3.pdf");
	
	

	TCanvas* c3 = new TCanvas("c3", "Sx3 U0 Strip 2 & 3 : Gamma Energy vs. Excitation energy", 400, 300);
	c3->cd();
	Mia->SetLineColor(kPink + 9);
	Marc->SetLineColor(kViolet - 5);
	GamvsExus->Draw("COLZ");
	Marc->Draw("same");
	Mia->Draw("same");
	//c3->Print("ReadGetTTree_result/Gamma_vs_Ex_Sx3.pdf");

	TCanvas* cgamExuq = new TCanvas("cgamExuq", "QQQ5 D : Gamma Energy vs. Excitation energy", 400, 300);
	cgamExuq->cd();
	Mia->SetLineColor(kPink + 9);
	Marc->SetLineColor(kViolet - 5);
	GamvsExuq->Draw("COLZ");
	Marc->Draw("same");
	Mia->Draw("same");

	/*TCanvas* gamm = new TCanvas("gamm", "Sx3: Gamma Plot from dd", 400, 300);
	gamm->cd();
	Gamdd->SetLineColor(kViolet - 5);
	Gamdd->Draw("same");
	gamm->Print("ReadGetTTree_result/gammaPlot.pdf");

	TCanvas* gammQ = new TCanvas("gammQ", "Sx3: Gamma Plot from dp", 400, 300);
	gammQ->cd();
	Gamdp->SetLineColor(kBlack);
	Gamdp->Draw();
	//Gamdduq->SetLineColor(kViolet - 5);
	//Gamdpuq->SetLineColor(kBlack);
	//Gamdpuq->Draw();
	//Gamdduq->Draw("same");


	TCanvas* gammuq = new TCanvas("gammuq", "QQQ5: Gamma Plot from dd", 400, 300);
	gammuq->cd();
	Gamdduq->SetLineColor(kViolet - 5);
	Gamdduq->Draw();
	
	TCanvas* gammuq1 = new TCanvas("gammuq1", "QQQ5: Gamma Plot from dp", 400, 300);
	gammuq1->cd();
	Gamdpuq->SetLineColor(kBlack);
	Gamdpuq->Draw();*/

	/*TCanvas* sx = new TCanvas("sx", "Ex - Sx3s", 400, 300);
	sx->cd();
	//f2uq->Draw();
	TH1D* py = ExvAngle->ProjectionY(); // where firstXbin = 0 and lastXbin = 9
	py->SetLineColor(kBlack);
	py->Draw();
	Paul->SetLineColor(kBlue);
	Jon->SetLineColor(kBlue);
	Paul->Draw("same");
	Jon->Draw("same");
	Mary->SetLineColor(kGreen+2);
	Magda->SetLineColor(kGreen+2);
	Jane->SetLineColor(kGreen+2);
	Julie->SetLineColor(kGreen+2);
	Mary->Draw("same");
	Magda->Draw("same");
	Jane->Draw("same");
	Julie->Draw("same");
	Sn->SetLineColor(kViolet-5);
	Sn->Draw("same");*/
	//Exus_O->Draw();
	//sx->Print("ReadGetTTree_result/Ex_Sx3.pdf");
	/*TCanvas* gating1 = new TCanvas("gating1", "Ex - p gated - QQQ5", 400, 300);
	gating1->cd();
	f5uq->SetLineColor(kRed);
	f5uq->Draw();*/
	/*TCanvas* gating2 = new TCanvas("gating2", "Ex - p gated - QQQ5", 400, 300);
	gating2->cd();
	f4uq->SetLineColor(kViolet);
	f4uq->Draw();
	f2uq->Draw("same");
	TCanvas* gating3 = new TCanvas("gating3", "Ex - p gated - QQQ5", 400, 300);
	gating3->cd();
	f2uq->SetLineColor(kBlue);
	f2uq->Draw();
	TCanvas* gating4 = new TCanvas("gating4", "Ex - p gated - QQQ5", 400, 300);
	gating4->cd();
	f6uq->SetLineColor(kGreen+2);
	f6uq->Draw();
	TCanvas* gating5 = new TCanvas("gating5", "Ex - p gated - QQQ5", 400, 300);
	gating5->cd();
	f7uq->SetLineColor(kBlack);
	f7uq->Draw();
	TCanvas* gating6 = new TCanvas("gating6", "Ex - p gated - QQQ5", 400, 300);
	gating6->cd();
	f8uq->SetLineColor(kPink+8);
	f8uq->Draw();
	TCanvas* gating7 = new TCanvas("gating7", "Ex - p gated - QQQ5", 400, 300);
	gating7->cd();
	f9uq->SetLineColor(kTeal+3);
	f9uq->Draw();
	TCanvas* gating8 = new TCanvas("gating8", "Ex - p gated - QQQ5", 400, 300);
	gating8->cd();
	f10uq->SetLineColor(kAzure -6);
	f10uq->Draw();


	TCanvas* gatinp1 = new TCanvas("gatinp1", "Ex - p gated - Sx3s", 400, 300);
	gatinp1->cd();
	f5us->SetLineColor(kRed);
	f5us->Draw("same");
	TCanvas* gatinp2 = new TCanvas("gatinp2", "Ex - p gated - Sx3s", 400, 300);
	gatinp2->cd();
	f4us->SetLineColor(kViolet);
	f4us->Draw("same");
	TCanvas* gatinp3 = new TCanvas("gatinp3", "Ex - p gated - Sx3s", 400, 300);
	gatinp3->cd();
	f2us->SetLineColor(kBlue);
	f2us->Draw("same");
	TCanvas* gatinp4 = new TCanvas("gatinp4", "Ex - p gated - Sx3s", 400, 300);
	gatinp4->cd();
	f6us->SetLineColor(kGreen+2);
	f6us->Draw("same");
	TCanvas* gatinp5 = new TCanvas("gatinp5", "Ex - p gated - Sx3s", 400, 300);
	gatinp5->cd();
	f7us->SetLineColor(kBlack);
	f7us->Draw("same");
	TCanvas* gatinp6 = new TCanvas("gatinp6", "Ex - p gated - Sx3s", 400, 300);
	gatinp6->cd();
	f8us->SetLineColor(kPink+8);
	f8us->Draw("same");
	TCanvas* gatinp7 = new TCanvas("gatinp7", "Ex - p gated - Sx3s", 400, 300);
	gatinp7->cd();
	f9us->SetLineColor(kTeal +3);
	f9us->Draw("same");
	*/

//	TCanvas* sxQQ = new TCanvas("sxQQ", "Ex - QQQ5s", 400, 300);
	//sxQQ->cd();
	//f2uq->Draw();
	//TH1D* pyq = ExvAngleuq->ProjectionY(); // where firstXbin = 0 and lastXbin = 9
	//pyq->SetLineColor(kBlack);
	//pyq->Draw();
	/*Paul->SetLineColor(kBlue);
	Jon->SetLineColor(kBlue);
	Paul->Draw("same");
	Jon->Draw("same");
	Mary->SetLineColor(kGreen + 2);
	Magda->SetLineColor(kGreen + 2);
	Jane->SetLineColor(kGreen + 2);
	Julie->SetLineColor(kGreen + 2);
	Mary->Draw("same");
	Magda->Draw("same");
	Jane->Draw("same");
	Julie->Draw("same");
	Sn->SetLineColor(kViolet - 5);
	Sn->Draw("same");*/

	/*TCanvas* compareOui = new TCanvas("compareOui", "Sx3 & QQQ5 singles", 400, 300);
	compareOui->cd();
	pyq->SetLineColor(kViolet);
	pyq->Draw();
	py->Draw("same");*/

	Harrysfile.close();

return;
}
