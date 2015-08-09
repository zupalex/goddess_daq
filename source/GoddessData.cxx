#include "GoddessData.h"

//ROOT Headers
#include "TDirectory.h"

//GEBSort Headers
#include "gdecomp.h"

//GODDESS Headers
#include "Detector.h"

GoddessData::GoddessData(std::string configFilename)
{
	config = new GoddessConfig("goddess.position",configFilename);

	tree=new TTree("god","GODDESS Tree");

	enRawA = new TH2F("enRawA","Raw Analog Energies;Energy [Ch];Channel",4096,0,4096,400,0,400);
}

void GoddessData::Fill(std::vector<DGSEVENT> *dgsEvts, std::vector<DFMAEVENT> *dgodEvts, std::vector<AGODEVENT> *agodEvts)
{
	for (size_t i=0;i<agodEvts->size();i++) {
		AGODEVENT agodEvt = agodEvts->at(i);
		for (size_t j=0;j<agodEvt.values.size();j++) {
			short value = agodEvt.values[j];
			short channel = agodEvt.channels[j];
			enRawA->Fill(value,channel);

			Detector *det = config->SetRawValue(GEB_TYPE_AGOD, channel, value);
		}
	}
	//tree->Fill();
}

