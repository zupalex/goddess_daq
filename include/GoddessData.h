#ifndef GODDESSDATA_H
#define GODDESSDATA_H

#include "GoddessConfig.h"
#include "ORRUBA.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

#include "GTMerge.h"

#include <vector>
#include <map>
#include <utility>


class GoddessData {
	public:
	GoddessData(std::string configFilename);
	~GoddessData();
	void Fill(std::vector<DGSEVENT> *dgsEvts, std::vector<DFMAEVENT> *dgodEvts, std::vector<AGODEVENT> *agodEvt);
	
	private:
	GoddessConfig *config;

	ORRUBA *orruba;

	//Map of channels to suppress, This occurs if they were not found in the map.
	std::map<std::pair<short, short>, bool> suppressCh;

	TH2F* enRawA;
	TH2F* enRawD;
	TH2F* enCalA;
	TH2F* enCalD;
	
	// raw and calibrated energy spectrum for each channel
	// qqq5 per strip
	std::map<std::string,TH2F*> QQQenRawFront;
	std::map<std::string,TH2F*> QQQenCalFront;
	std::map<std::string,TH2F*> QQQenRawBack;
	std::map<std::string,TH2F*> QQQenCalBack;
	
	std::map<std::string,TH2F*> QQQHitPat;
	std::map<std::string,TH1F*> QQQFrontMult;
	std::map<std::string,TH1F*> QQQBackMult;



	// number of hits/strip for each detector
	std::vector<TH2F*> hitPattern; 

	TTree* tree;

	TClonesArray* superX3s;
	TClonesArray* bb10s;
	TClonesArray* qqq5s;


};

#endif
