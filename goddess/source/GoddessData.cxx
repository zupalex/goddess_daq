#include "GoddessData.h"

GoddessData::GoddessData(TClonesArray* superX3s_,TClonesArray*bb10s_,TClonesArray* qqq5s_) :
superX3s(superX3s_), bb10s(bb10s_), qqq5s(qqq5s_)
{
	tree=new TTree("goddess/tree");
	tree->Branch("sX3",superX3s);
	tree->Branch("bb10",bb10s);
	tree->Branch("qqq5",qqq5s);

}

void GoddessData::Fill(DGSEvent dgsevent,DFMAEvent dfmaevent,AGODEvent agodevent)
{
	tree->Fill();
}

