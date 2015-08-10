#include "GoddessData.h"

//ROOT Headers
#include "TDirectory.h"

//GEBSort Headers
#include "gdecomp.h"

//GODDESS Headers
#include "Detector.h"
#include "orrubaDet.h"
#include "QQQ5.h"

#include <iostream>

GoddessData::GoddessData(std::string configFilename)
{
	config = new GoddessConfig("goddess.position",configFilename);

	orruba = new ORRUBA();

	tree=new TTree("god","GODDESS Tree");
	tree->Branch("orruba",&orruba);

	// ORRUBA histograms
	// dorectories to keep things organized
	gDirectory->mkdir("ORRUBA");
	gDirectory->cd("ORRUBA");
	gDirectory->mkdir("analog");
	gDirectory->mkdir("digital");
	gDirectory->cd("analog");
	gDirectory->mkdir("cal");
	gDirectory->mkdir("raw");
	gDirectory->cd("/ORRUBA/digital");
	gDirectory->mkdir("raw");
	gDirectory->mkdir("cal");

	gDirectory->cd("/ORRUBA/analog/raw");
	enRawA = new TH2F("enRawA","Raw Analog Energies;Energy [Ch];Channel",4096,0,4096,400,0,400);

	gDirectory->cd("/ORRUBA/digital/raw");
	enRawD = new TH2F("enRawD","Raw Digital Energies;Energy [Ch];Channel",4096,0,4096,400,0,400);

	gDirectory->cd("/ORRUBA/analog/cal");
	enCalA = new TH2F("enCalA","Calibrated Analog Energies;Energy [MeV];Channel",4096,0,4096,400,0,400);

	gDirectory->cd("/ORRUBA/digital/cal");
	enCalD = new TH2F("enCalD","Calibrated Digital Energies;Energy [MeV];Channel",4096,0,4096,400,0,400);

	gDirectory->cd("/");

	TClonesArray *qqq5s=config->GetQQQ5s();
	int nqqq5s = qqq5s->GetEntries();

	gDirectory->mkdir("QQQ5");
	gDirectory->cd("QQQ5");
	gDirectory->mkdir("front");
	gDirectory->mkdir("back");
	gDirectory->cd("front");
	gDirectory->mkdir("raw");
	gDirectory->mkdir("cal");
	gDirectory->cd("/QQQ5/back");
	gDirectory->mkdir("raw");
	gDirectory->mkdir("cal");
	
	for (int i = 0; i < nqqq5s; i++) {
		const char* name = ((std::string)((QQQ5*)qqq5s->At(i))->GetPosID()).c_str();
		
		gDirectory->cd("/QQQ5/front/raw");
		QQQenRawFront.push_back(new TH2F(Form("QQQenRawFront_%s",name),Form("Raw QQQ5 %s energy per front strip;Energy [Ch];Channel",name), 4096,0,4096,32,0,32));

		gDirectory->cd("/QQQ5/front/cal");
		QQQenCalFront.push_back(new TH2F(Form("QQQenCalFront_%s",name),Form("Cal QQQ5 %s energy per front strip;Energy [MeV];Channel",name), 4096,0,4096,32,0,32));

		gDirectory->cd("/QQQ5/back/raw");
		QQQenRawBack.push_back(new TH2F(Form("QQQenRawBack_%s",name),Form("Raw QQQ5 %s energy per back strip;Energy [Ch];Channel",name), 4096,0,4096,4,0,4));

		gDirectory->cd("/QQQ5/back/cal");
		QQQenCalBack.push_back(new TH2F(Form("QQQenCalBack_%s",name),Form("Cal QQQ5 %s energy per back strip;Energy [MeV];Channel",name), 4096,0,4096,4,0,4));
	}
	gDirectory->cd("/");	
		
		
	
	
}

void GoddessData::Fill(std::vector<DGSEVENT> *dgsEvts, std::vector<DFMAEVENT> *dgodEvts, std::vector<AGODEVENT> *agodEvts)
{
	std::vector<orrubaDet*> firedDets;

	for (size_t i=0;i<agodEvts->size();i++) {
		AGODEVENT agodEvt = agodEvts->at(i);
		for (size_t j=0;j<agodEvt.values.size();j++) {
			short value = agodEvt.values[j];
			short channel = agodEvt.channels[j];
			enRawA->Fill(value,channel);

			std::pair<short, short> key = std::make_pair(GEB_TYPE_AGOD, channel);
			if (suppressCh[key]) continue;

			Detector *det = config->SetRawValue(GEB_TYPE_AGOD, channel, value);
			if (!det) {
				suppressCh[key] = true;
				continue;
			}	

			if (std::string(det->IsA()->GetName()) != "IonChamber")
				firedDets.push_back((orrubaDet*) det);
		}
	}


	//Build a total ORRUBA event
	float dE=0, E1=0, E2=0;
	TVector3 pos(0,0,0);
	std::string sector;
	bool valid = true;
	for (auto itr=firedDets.begin();itr!=firedDets.end(); ++itr) {
		orrubaDet* det = *itr;
		unsigned short depth = det->GetDepth();
		std::string detType = det->IsA()->GetName();
		std::string detPosID = det->GetPosID();
		std::string detPosSector = detPosID.substr(0, detPosID.length() - 3);

		if (sector.empty()) sector = detPosSector;
		//If we had detectors from different quadrants we need to abort.
		else if (sector != detPosSector) {
			valid = false;
			break;
		}

		if (depth == 0) {
			if (!dE) dE = det->GetEnergy();
		}
		else if (depth == 1) {
			if (!E1) E1 = det->GetEnergy();
		}
		else if (depth == 2) {
			if (!E2) E2 = det->GetEnergy();
		}
		if (detType == "superX3" || detType == "QQQ5") {
			//Save the position if a position has not been saved or
			//	if this is a QQQ5 dE layer.
			if (!pos.Mag2() || (detType == "QQQ5" && depth == 0))
				pos = det->GetEventPosition();
		}
	} 
	orruba->SetEvent(sector,dE,E1,E2,pos);
	//tree->Fill();
}

