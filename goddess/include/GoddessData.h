#ifndef GODDESSDATA_H
#define GODDESSDATA_H

#include "TH1F.h"
#include "TH2F.h"
#include "TClonesArray.h"
#include "TTree.h"

#include <vector>

class GoddessData {
	public:
	GoddessData(TClonesArray*,TClonesArray*,TClonesArray*);
	~GoddessData();
	
	private:
	// raw and calibrated energy spectrum for each channel
	vector<TH1F*> enRaw;
	vector<TH1F*> enCal;
	
	// summary spectrum (energy vs strip) for each detector
	vector<TH1F*> sumEnCal;
	vector<TH1F*> sumEnRaw;

	// number of hits/strip for each detector
	vector<TH2F*> hitPattern; 

	TTree* tree;

	TClonesArray* superX3s;
	TClonesArray* bb10s;
	TClonesArray* qqq5s;


};

#endif
