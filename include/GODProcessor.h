#ifndef GODPROCESSOR_H
#define GODPROCESSOR_H

#include <iostream>
#include <string>

#include "TH1D.h"
#include "TH1F.h"
#include "TH2F.h"

#include "GEBSort.h"
#include "GTMerge.h"
#include "GRProcessor.h"

#include "GoddessData.h"

class GODProcessor
{
	private:

	public:
		GODProcessor(int* tlkup_, int* tid_, int* ng_, PARS* pars_, GoddessConfig* gConf_, unsigned int* numDGOD_, unsigned int* numAGOD_,
				unsigned long long* lastTS_);
		~GODProcessor()
		{
		}

		PARS* pars;
		GoddessConfig* gConf;

		GoddessData* godData;

		int* tlkup;
		int* tid;
		int* ng;
		unsigned int* numDGOD;
		unsigned int* numAGOD;
		unsigned long long* lastTS;

		int SupDGOD();
		int DGODEvDecompose(unsigned int* ev, int len, DFMAEVENT* thedfmaEvt);
		int BinDGOD(GEB_EVENT* gebEvt, DFMAEVENT* dfmaEvt, DGSEVENT* dgsEvt);
		int BinDGOD(GEB_EVENT* gebEvt, DFMAEVENT* dfmaEvt, GRProcessor::Gretina_Gamma_Ray* grEvt);

		void AGODEvDecompose(unsigned int* ev, int len, AGODEVENT* theagodEvt);
		void SupAGOD();
		int BinAGOD(GEB_EVENT* gebEvt, AGODEVENT* agodEvt, DGSEVENT* dgsEvt);
		int BinAGOD(GEB_EVENT* gebEvt, AGODEVENT* agodEvt, GRProcessor::Gretina_Gamma_Ray* grEvt);

		void SupGOD();
		int BinGOD(GEB_EVENT* gebEvt, AGODEVENT* agodEvt, DFMAEVENT* dfmaEvt, DGSEVENT* dgsEvt);
		int BinGOD(GEB_EVENT* gebEvt, AGODEVENT* agodEvt, DFMAEVENT* dfmaEvt, GRProcessor::Gretina_Gamma_Ray* grEvt);

};

#endif

