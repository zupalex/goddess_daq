#include "TFile.h"
#include "TH2F.h"


#include "UserInclude.h"

#include "UserFunctions.h"

int main() {
	TFile f("test.root","RECREATE");
	printf("User Declare\n");
	#include "UserDeclare.h"
	printf("User Init\n");
	#include "UserInit.h"

	//TH2F *test = new TH2F("test","Test",4,((QQQ5*)qqq5s[0])->GetAzimuthalBins(),32,((QQQ5*)qqq5s[0])->GetRhoBins());

	printf("User Raw Ev\n");
	#include "UserRawEv.h"
	printf("User Good Ev\n");
	#include "UserGoodEv.h"
	printf("User PreCond\n");
	#include "UserPreCond.h"
	printf("User Ev\n");
	#include "UserEv.h"
	printf("User Exit\n");
	#include "UserExit.h"

//	for (int x=1;x<=test->GetNbinsX();x++) 
//		for (int y=1;y<=test->GetNbinsY();y++) 
//			test->SetBinContent(test->GetBin(x,y),test->GetBin(x,y));

	f.Print();
	f.Write();	
	f.Close();
}
