#include "GoddessData.h"

//ROOT Headers
#include "TDirectory.h"

//GEBSort Headers
#include "gdecomp.h"

//GODDESS Headers
#include "Detector.h"
#include "orrubaDet.h"

GoddessData::GoddessData(std::string configFilename)
{
	config = new GoddessConfig("goddess.position",configFilename);

	orruba = new ORRUBA();

	tree=new TTree("god","GODDESS Tree");
	tree->Branch("orruba",&orruba);

	// ORRUBA histograms
	// dorectories to keep things organized
	gDirectory->mkdir("ORRUBA");
	gDirectory->mkdir("ORRUBA/analog");
	gDirectory->cd("ORRUBA/analog");

	enRawA = new TH2F("enRawA","Raw Analog Energies;Energy [Ch];Channel",4096,0,4096,400,0,400);

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

