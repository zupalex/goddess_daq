#ifndef GODDESSDATA_H
#define GODDESSDATA_H

#include "GoddessConfig.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

#include "GTMerge.h"

#include <vector>

class GoddessData {
	public:
	GoddessData(std::string configFilename);
	~GoddessData();
	void Fill(std::vector<DGSEVENT> *dgsEvts, std::vector<DFMAEVENT> *dgodEvts, std::vector<AGODEVENT> *agodEvt);
	
	private:
	GoddessConfig *config;

	TH2F* enRawA;
	TH2F* enRawD;
	// raw and calibrated energy spectrum for each channel
	std::vector<TH1F*> enRaw;
	std::vector<TH1F*> enCal;
	
	// summary spectrum (energy vs strip) for each detector
	std::vector<TH1F*> sumEnCal;
	std::vector<TH1F*> sumEnRaw;

	// number of hits/strip for each detector
	std::vector<TH2F*> hitPattern; 

	TTree* tree;

	TClonesArray* superX3s;
	TClonesArray* bb10s;
	TClonesArray* qqq5s;


};

#endif
