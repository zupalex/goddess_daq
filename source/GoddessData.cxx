#include "GoddessData.h"

//ROOT Headers
#include "TDirectory.h"
#include "TFile.h"

//GEBSort Headers
#include "gdecomp.h"

//GODDESS Headers
#include "Detector.h"
#include "orrubaDet.h"
#include "QQQ5.h"
#include "superX3.h"
#include "BB10.h"

#include <iostream>

GoddessData::GoddessData(std::string configFilename)
{
	config = new GoddessConfig("goddess.position",configFilename);

	orruba = new ORRUBA();
	firedDets = new std::vector<Detector*>;

	gDirectory->pwd();
	TFile *f = gDirectory->GetFile();
	if (!f) {
		std::cerr << "ERROR: Not in a ROOT File?\n";
		return;
	}
	f->cd("/trees");
	tree=new TTree("god","GODDESS Tree");
	tree->Branch("orruba",&orruba);
	tree->Branch("det",&firedDets);

	f->cd("/hists");
	gDirectory->pwd();
	// ORRUBA histograms
	// dorectories to keep things organized
	TDirectory *dirOrruba = gDirectory->mkdir("orruba");
	dirOrruba->cd();
	TDirectory *dirRaw = gDirectory->mkdir("raw");
	dirRaw->cd();
	enRawA = new TH2F("enRawA","Raw Analog Energies;Energy [Ch];Channel",512,0,4096,400,0,400);
	enRawD = new TH2F("enRawD","Raw Digital Energies;Energy [Ch];Channel",512,0,4096,400,0,400);
	enCalA = new TH2F("enCalA","Calibrated Analog Energies;Energy [MeV];Channel",512,0,4096,400,0,400);
	enCalD = new TH2F("enCalD","Calibrated Digital Energies;Energy [MeV];Channel",512,0,4096,400,0,400);


	TClonesArray *qqq5s=config->GetQQQ5s();
	TClonesArray* sx3s = config->GetSuperX3s();
	TClonesArray* bb10s =config->GetBB10s();
	//TClonesArray* ics = config->GetIonCounter();
	int nqqq5s = qqq5s->GetEntries();

	f->cd("hists");
	TDirectory *dirQQQ5 = gDirectory->mkdir("qqq5");

	for (int i = 0; i < nqqq5s; i++) {
		const char* name = ((std::string)((QQQ5*)qqq5s->At(i))->GetPosID()).c_str();
		const char* dir=Form("%s",name);
		dirQQQ5->cd();
		TDirectory *dirDet = gDirectory->mkdir(dir);
		dirDet->cd();
		gDirectory->mkdir("en")->cd();

		QQQenRawFront.emplace(name,new TH2F(Form("QQQenRawFront_%s",name),Form("Raw QQQ5 %s energy per front strip;Energy [Ch];Channel",name), 512,0,4096,32,0,32));

		QQQenCalFront.emplace(name,new TH2F(Form("QQQenCalFront_%s",name),Form("Cal QQQ5 %s energy per front strip;Energy [MeV];Channel",name), 512,0,4096,32,0,32));

		QQQenRawBack.emplace(name,new TH2F(Form("QQQenRawBack_%s",name),Form("Raw QQQ5 %s energy per back strip;Energy [Ch];Channel",name), 512,0,4096,4,0,4));

		QQQenCalBack.emplace(name,new TH2F(Form("QQQenCalBack_%s",name),Form("Cal QQQ5 %s energy per back strip;Energy [MeV];Channel",name), 512,0,4096,4,0,4));

		dirDet->cd();
		gDirectory->mkdir("mult")->cd();

		QQQHitPat.emplace(name,new TH2F(Form("QQQHitPat_%s",name),Form("QQQ5 Hit Pattern %s;Front [strip];Back [strip]",name),32,0,32,4,0,4));
		QQQFrontMult.emplace(name,new TH1F(Form("QQQFrontMult_%s",name),Form("QQQ5 %s multiplicity;multiplicity",name),20,0,20));
		QQQBackMult.emplace(name,new TH1F(Form("QQQBackMult_%s",name),Form("QQQ5 %s multiplicitymultiplicity",name),20,0,20));
	}
	gDirectory->cd("/hists");

	TDirectory *dirSX3 = gDirectory->mkdir("sx5");

	// super X 3s 
	int nsx3s = sx3s->GetEntries();
	std::cout << nsx3s << "\n";
	for (int i=0;i< nsx3s;i++) {
		const char* name = ((std::string)((superX3*)sx3s->At(i))->GetPosID()).c_str();
		const char* dir=Form("%s",name);
		dirSX3->cd();
		TDirectory *dirDet = gDirectory->mkdir(dir);
		dirDet->cd();

		gDirectory->mkdir("en")->cd();
		sX3stripEnRaw.emplace(name, new TH2F(Form("sX3stripEnRaw%s",name),
			Form("SuperX3 strip raw energy vs strip %s;energy [ch];strip", name),512,0,4096, 8, 0,8));

		sX3stripEnCal.emplace(name, new TH2F(Form("sX3stripEnCal%s",name),
			Form("SuperX3 strip cal energy vs strip %s;energy [keV];strip", name),512,0,4096, 8, 0,8));

		sX3backEnRaw.emplace(name, new TH2F(Form("sX3backEnRaw%s",name),
			Form("SuperX3 back raw energy vs strip %s;energy [ch];strip", name),512,0,4096, 4, 0,4));

		sX3backEnCal.emplace(name, new TH2F(Form("sX3backEnCal%s",name),
			Form("SuperX3 back cal energy vs strip %s;energy [keV];strip", name),512,0,4096, 4, 0,4));

		dirDet->cd();
		gDirectory->mkdir("pos")->cd();

		sX3nearFar[name] = new TH2F*[4];
		sX3posRaw_enRaw[name] = new TH2F*[4];
		sX3posRaw_enCal[name] = new TH2F*[4];
		sX3posCal_enCal[name] = new TH2F*[4];

		for (int i=0; i < 4; i++) {
			sX3posRaw_enCal[name][i] = new TH2F(Form("sX3posRaw_enCal_%s_%i",name,i),
				Form("super X3 raw position vs cal energy %s_%i",name,i),512, -1,1,512,0,4096);

			sX3posCal_enCal[name][i] = new TH2F(Form("sX3posCal_enCal_%s_%i",name,i),
				Form("super X3 cal position vs cal energy %s_%i",name,i),512, -1,1,512,0,4096);

			sX3posRaw_enRaw[name][i] = new TH2F(Form("sX3posRaw_enRaw_%s_%i",name,i),
				Form("super X3 raw position vs raw energy %s_%i",name,i),512, -1,1,512,0,4096);

			sX3nearFar[name][i] = new TH2F(Form("sX3nearFar_%s_%i",name,i),
				Form("sX3 near strip vs far strip %s %i", name,i), 512,0,4096,512,0,4096);
		}

		dirDet->cd();
		gDirectory->mkdir("mult")->cd();

		sX3HitPat.emplace(name,new TH2F(Form("sX3HitPat_%s",name),
			Form("SuperX3 Hit Pattern %s;Front [strip];Back [strip]",name),8,0,8,4,0,4));

		sX3frontMult.emplace(name,new TH1F(Form("sX3FrontMult_%s",name),
			Form("super X3 %s multiplicity;multiplicity",name),20,0,20));

		sX3backMult.emplace(name,new TH1F(Form("sX3BackMult_%s",name),
			Form("super X3 %s multiplicity;multiplicity",name),20,0,20));


	}



	gDirectory->cd("/hists");
	TDirectory *dirBB10 = gDirectory->mkdir("bb10");

	int nbb10s = bb10s->GetEntries();
	for (int i = 0; i < nbb10s; i++) {
		const char* name = ((std::string)((BB10*)bb10s->At(i))->GetPosID()).c_str();
		dirBB10->cd();
		gDirectory->mkdir(name)->cd();
		gDirectory->mkdir("en")->cd();


	}
		
	gDirectory->cd("/hists");
		
	
	
}

void GoddessData::Fill(std::vector<DGSEVENT> *dgsEvts, std::vector<DFMAEVENT> *dgodEvts, std::vector<AGODEVENT> *agodEvts) {
	//Map of channels to suppress, This occurs if they were not found in the map.
	static std::map<std::pair<short, short>, bool> suppressCh;

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

			firedDets->push_back(det);
			orrubaDet* siDet = dynamic_cast<orrubaDet*>(det);
			if (siDet) {
				siDets.push_back(siDet);
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

		firedDets->push_back(det);
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
		siDet::ValueMap frontCalEn = det->GetCalEn(false);
		siDet::ValueMap backRawEn = det->GetRawEn(true);
		siDet::ValueMap backCalEn = det->GetCalEn(true);

		if (detType == "QQQ5") {
			for (auto itr=frontRawEn.begin(); itr!=frontRawEn.end();++itr) {
				QQQenRawFront[detPosID]->Fill(itr->second, itr->first);
				QQQFrontMult[detPosID]->Fill(frontRawEn.size());
			}
			for (auto itr=frontCalEn.begin(); itr!=frontCalEn.end();++itr) {
				QQQenCalFront[detPosID]->Fill(itr->second, itr->first);
			}
			for (auto itr=backRawEn.begin(); itr!=backRawEn.end();++itr) {
				QQQenRawBack[detPosID]->Fill(itr->second, itr->first);
				QQQBackMult[detPosID]->Fill(backRawEn.size());
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
			superX3 *sx3= (superX3*) det;
			for (auto itr=frontRawEn.begin(); itr!=frontRawEn.end();++itr) {
				
				sX3stripEnRaw[detPosID]->Fill(itr->second, itr->first);
				sX3frontMult[detPosID]->Fill(frontRawEn.size());
			}
			//for loop over 8 contacts/4 strips
			for (int i=0;i<4;i++) {
				if ((frontRawEn.find(2*i)!=frontRawEn.end()) && (frontRawEn.find(2*i+1)!=frontRawEn.end())) {
					sX3posRaw_enRaw[detPosID][i]->Fill(sx3->GetStripPosRaw()[i],frontRawEn[2*i]+frontRawEn[2*i+1]);
					sX3nearFar[detPosID][i]->Fill(frontRawEn[2*i], frontRawEn[2*i+1]);
				}
			}

			
			for (auto itr=frontCalEn.begin(); itr!=frontCalEn.end();++itr) {
				sX3stripEnCal[detPosID]->Fill(itr->second, itr->first);
			}
			//for loop over 8 contacts/4 strips
			for (int i=0;i<4;i++) {
				if ((frontCalEn.find(2*i)!=frontCalEn.end()) && (frontCalEn.find(2*i+1)!=frontCalEn.end())) {
					sX3posRaw_enCal[detPosID][i]->Fill(sx3->GetStripPosRaw()[i],frontCalEn[2*i]+frontCalEn[2*i+1]);
					sX3posCal_enCal[detPosID][i]->Fill(sx3->GetStripPosCal()[i],frontCalEn[2*i]+frontCalEn[2*i+1]);
					sX3nearFar[detPosID][i]->Fill(frontCalEn[2*i], frontCalEn[2*i+1]);
				}
			}

			for (auto itr=backRawEn.begin(); itr!=backRawEn.end();++itr) {
				sX3backEnRaw[detPosID]->Fill(itr->second,itr->first);
				sX3backMult[detPosID]->Fill(backRawEn.size());
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
		if (detType=="Ion Counter") {
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
	}
	tree->Fill();

	//We clear everything here since we know what was actually fired.
	for (auto itr = firedDets->begin(); itr != firedDets->end(); ++itr) {
		(*itr)->Clear();
	}
	firedDets->clear();
}

