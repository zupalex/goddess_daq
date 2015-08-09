#include "GEBSort.h"
#include "GTMerge.h"

#include "GoddessData.h"
#include "GoddessConfig.h"

extern DFMAEVENT DFMAEvent[MAXCOINEV];
extern DGSEVENT DGSEvent[MAXCOINEV];
extern AGODEVENT AGODEvent[MAXCOINEV];

extern unsigned int numAGOD;
extern int ng;

GoddessData *godData;

void sup_god() {
	godData = new GoddessData("goddess.config");
}

void bin_god (GEB_EVENT *){
	std::vector<AGODEVENT> AGODEvts;

	for (unsigned int i=0;i<numAGOD;i++) 
		AGODEvts.push_back(AGODEvent[i]);

	godData->Fill(NULL,NULL,&AGODEvts);
}
