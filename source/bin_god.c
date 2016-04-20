#include "GEBSort.h"
#include "GTMerge.h"

#include "GoddessData.h"
#include "GoddessConfig.h"

#include <iostream>
#include <string>

extern DFMAEVENT DFMAEvent[MAXCOINEV];
extern DGSEVENT DGSEvent[MAXCOINEV];
extern AGODEVENT AGODEvent[MAXCOINEV];

extern unsigned int numAGOD, numDGOD;
extern int ng;

extern PARS Pars;

GoddessData *godData;

void sup_god() {
  std::string testconf = Pars.ConfigFile;
  godData = new GoddessData(testconf);
  //godData = new GoddessData("goddess.config");
  //godData = new GoddessData("goddess_95Mo_runs200_to_352.config");
}

void bin_god (GEB_EVENT *gebEvt){
	std::vector<AGODEVENT> AGODEvts;
	std::vector<DFMAEVENT> DFMAEvts;
	std::vector<DGSEVENT> DGSEvts;

	for (unsigned int i=0;i<numAGOD;i++) 
		AGODEvts.push_back(AGODEvent[i]);

	for (unsigned int i=0;i<numDGOD;i++) 
		DFMAEvts.push_back(DFMAEvent[i]);

	for (int i=0;i<ng;i++) 
		DGSEvts.push_back(DGSEvent[i]);



	godData->Fill(gebEvt, &DGSEvts,&DFMAEvts,&AGODEvts);
}
