#include "GoddessData.h"

//ROOT Headers
#include "TDirectory.h"

//GEBSort Headers
#include "gdecomp.h"

//GODDESS Headers
#include "Detector.h"
#include "orrubaDet.h"
#include "QQQ5.h"
#include "superX3.h"

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
	gDirectory->mkdir("multiplicity");
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
		QQQenRawFront.emplace(name,new TH2F(Form("QQQenRawFront_%s",name),Form("Raw QQQ5 %s energy per front strip;Energy [Ch];Channel",name), 4096,0,4096,32,0,32));

		gDirectory->cd("/QQQ5/front/cal");
		QQQenCalFront.emplace(name,new TH2F(Form("QQQenCalFront_%s",name),Form("Cal QQQ5 %s energy per front strip;Energy [MeV];Channel",name), 4096,0,4096,32,0,32));

		gDirectory->cd("/QQQ5/back/raw");
		QQQenRawBack.emplace(name,new TH2F(Form("QQQenRawBack_%s",name),Form("Raw QQQ5 %s energy per back strip;Energy [Ch];Channel",name), 4096,0,4096,4,0,4));

		gDirectory->cd("/QQQ5/back/cal");
		QQQenCalBack.emplace(name,new TH2F(Form("QQQenCalBack_%s",name),Form("Cal QQQ5 %s energy per back strip;Energy [MeV];Channel",name), 4096,0,4096,4,0,4));

		gDirectory->cd("/QQQ5/multiplicity");
		QQQHitPat.emplace(name,new TH2F(Form("QQQHitPat_%s",name),Form("QQQ5 Hit Pattern %s;Front [strip];Back [strip]",name),32,0,32,4,0,4));
		QQQFrontMult.emplace(name,new TH1F(Form("QQQFrontMult_%s",name),Form("QQQ5 %s multiplicity;multiplicity",name),20,0,20));
		QQQBackMult.emplace(name,new TH1F(Form("QQQBackMult_%s",name),Form("QQQ5 %s multiplicitymultiplicity",name),20,0,20));
	}
	gDirectory->cd("/");	



	gDirectory->mkdir("sX3");
	gDirectory->cd("sX3");

	// super X 3s 
	TClonesArray* sx3s = config->GetSuperX3s();
	int nsx3s = sx3s->GetEntries();
	std::cout << nsx3s << "\n";
	for (int i=0;i< nsx3s;i++) {
		const char* name = ((std::string)((superX3*)sx3s->At(i))->GetPosID()).c_str();
		sX3stripEnRaw.emplace(name, new TH2F(Form("sX3stripEnRaw%s",name),Form("SuperX3 strip raw energy vs strip %s;energy [ch];strip", name),4096,0,4096, 8, 0,8));
		sX3stripEnCal.emplace(name, new TH2F(Form("sX3stripEnCal%s",name),Form("SuperX3 strip cal energy vs strip %s;energy [keV];strip", name),4096,0,4096, 8, 0,8));
		sX3backEnRaw.emplace(name, new TH2F(Form("sX3backEnRaw%s",name),Form("SuperX3 back raw energy vs strip %s;energy [ch];strip", name),4096,0,4096, 4, 0,4));
		sX3backEnCal.emplace(name, new TH2F(Form("sX3backEnCal%s",name),Form("SuperX3 back cal energy vs strip %s;energy [keV];strip", name),4096,0,4096, 4, 0,4));
		//sX3near_far.emplace(name, new TH2F(Form("sX3near_far_%s",name),Form("SuperX3 Near vs Far %s;Near;Far", name),4096,0,4096,4096,0,4096));
		//sX3enCal_posRaw.emplace(name, new TH2F(Form("sX3posRaw_enCal_%s",name),Form("SuperX3 Pos Raw vs Cal Energy %s;position [ch];Energy[MeV]", name),4096,0,4096,4096,0,4096));
		//sX3enCal_posCal.emplace(name, new TH2F(Form("sX3posCal_enCal_%s",name),Form("SuperX3 Pos Cal vs Cal Energy %s;position [cm];Energy[MeV]", name),4096,0,4096,4096,0,4096));
		sX3HitPat.emplace(name,new TH2F(Form("sX3HitPat_%s",name),Form("SuperX3 Hit Pattern %s;Front [strip];Back [strip]",name),8,0,8,4,0,4));
	}
		
	gDirectory->cd("/");
		
	
	
}

void GoddessData::Fill(std::vector<DGSEVENT> *dgsEvts, std::vector<DFMAEVENT> *dgodEvts, std::vector<AGODEVENT> *agodEvts) {
	std::vector<Detector*> firedDets;
	std::vector<orrubaDet*> siDets;

	// getting data from analog events
	for (size_t i=0;i<agodEvts->size();i++) {
		AGODEVENT agodEvt = agodEvts->at(i);

		for (size_t j=0;j<agodEvt.values.size();j++) {
			short value = agodEvt.values[j];
			short channel = agodEvt.channels[j];

			enRawA->Fill(value,channel);

			std::pair<short, short> key = std::make_pair(GEB_TYPE_AGOD, channel);
			if (suppressCh.find(key) != suppressCh.end()) continue;

			Detector *det = config->SetRawValue(GEB_TYPE_AGOD, channel, value);
			if (!det) {
				suppressCh[key] = true;
				continue;
			}	

			std::string detectorType = det->IsA()->GetName();
			if (detectorType != "Ion Chamber") {
				firedDets.push_back((orrubaDet*) det);
			} 
		}
	}

	// getting data from digital events	
	for (size_t i=0;i<dgodEvts->size();i++) {
		DFMAEVENT dgodEvt = dgodEvts->at(i);
		short value=dgodEvt.ehi;
		short channel=dgodEvt.tid;

		enRawD->Fill(value,channel);

		std::pair<short,short> key=std::make_pair(GEB_TYPE_DFMA,channel);
		if (suppressCh.find(key) != suppressCh.end()) {
			continue;
		}

		Detector *det=config->SetRawValue(GEB_TYPE_DFMA,channel,value);
		if (!det) {
			suppressCh[key]=true;
			continue;
		}

		firedDets.push_back(det);
		orrubaDet* siDet = dynamic_cast<orrubaDet*>(det);
		if (siDet) {
			siDets.push_back(siDet);
		}
	}

	// loop over fired detectors
	for (auto itr=siDets.begin();itr!=siDets.end(); ++itr) {
		orrubaDet* det = *itr;
		std::string detPosID = det->GetPosID();
		std::string detType = det->IsA()->GetName();

		// front=false (p-type = (!n-type))
		siDet::ValueMap frontRawEn = det->GetRawEn(false);
		siDet::ValueMap frontCalEn = det->GetRawEn(false);
		siDet::ValueMap backRawEn = det->GetRawEn(true);
		siDet::ValueMap backCalEn = det->GetCalEn(true);

		if (detType == "QQQ5") {
			for (auto itr=frontRawEn.begin(); itr!=frontRawEn.end();++itr) {
				QQQenRawFront[detPosID]->Fill(itr->second, itr->first);
				//QQQFrontMult[detPosID]->Fill(itr->first);
				QQQFrontMult[detPosID]->Fill(frontRawEn.size());
			}
			for (auto itr=frontCalEn.begin(); itr!=frontCalEn.end();++itr) {
				QQQenCalFront[detPosID]->Fill(itr->second, itr->first);
			}
			for (auto itr=backRawEn.begin(); itr!=backRawEn.end();++itr) {
				QQQenRawBack[detPosID]->Fill(itr->second, itr->first);
				QQQBackMult[detPosID]->Fill(itr->first);
			}
			for (auto itr=backCalEn.begin(); itr!=backCalEn.end();++itr) {
				QQQenCalBack[detPosID]->Fill(itr->second, itr->first);
			}

			for (auto itrFront=frontRawEn.begin();itrFront!=frontRawEn.end();++itrFront) {
				for (auto itrBack=backRawEn.begin();itrBack!=backRawEn.end();++itrBack) {
					QQQHitPat[detPosID]->Fill(itrFront->first,itrBack->first);
				}
			}
		}

		if (detType == "superX3") {
			for (auto itr=frontRawEn.begin(); itr!=frontRawEn.end();++itr) {
				sX3stripEnRaw[detPosID]->Fill(itr->second, itr->first);
			}
			for (auto itr=frontCalEn.begin(); itr!=frontCalEn.end();++itr) {
				sX3stripEnCal[detPosID]->Fill(itr->second, itr->first);
			}
			for (auto itr=backRawEn.begin(); itr!=backRawEn.end();++itr) {
				sX3backEnRaw[detPosID]->Fill(itr->second,itr->first);
			}
			for (auto itr=backCalEn.begin(); itr!=backCalEn.end();++itr) {
				sX3backEnCal[detPosID]->Fill(itr->second,itr->first);
			}
			// hit pattern
			for (auto itrFront=frontRawEn.begin();itrFront!=frontRawEn.end();++itrFront) {
				for (auto itrBack=backRawEn.begin();itrBack!=backRawEn.end();++itrBack) {
					sX3HitPat[detPosID]->Fill(itrFront->first,itrBack->first);	
				}
			}
		}
	}


	//Build a total ORRUBA event
	float dE=0, E1=0, E2=0;
	TVector3 pos(0,0,0);
	std::string sector;
	bool valid = true;
	for (auto itr=siDets.begin();itr!=siDets.end(); ++itr) {
		orrubaDet* det = *itr;
		std::string detPosID = det->GetPosID();
		unsigned short depth = det->GetDepth();
		std::string detType = det->IsA()->GetName();
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
	if (valid) {
		orruba->SetEvent(sector,dE,E1,E2,pos);
		tree->Fill();
	}

	//We clear everything here since we know what was actually fired.
	for (auto itr = firedDets.begin(); itr != firedDets.end(); ++itr) {
		(*itr)->Clear();
	}
}

