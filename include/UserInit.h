
/*--------------------------*/
/* user initialization code */
/* (may be empty..........) */
/*--------------------------*/

/*
config = new GoddessConfig("goddess.position","goddess.config");


superX3s = config->GetSuperX3s();
bb10s = config->GetBB10s();
qqq5s = config->GetQQQ5s();

hEnRaw = new TH1F**[superX3s->GetEntries()];

for (int detNum = 0; detNum < superX3s->GetEntries(); detNum++) {
	superX3* det = (superX3*)superX3s->At(detNum);

	hEnRaw[detNum] = new TH1F*[12];
	for (int contact = 0; contact < 12; contact++) {
		bool nType = false;
		std::string typeString = "p-type";
		if (contact >= 8) {
			nType = true;
			typeString = "n-type";
		}
		hEnRaw[detNum][contact] = new TH1F(
			Form("hEnRawSX3_%d_%d",detNum,contact),
			Form("superX3 %s Raw Energy Contact: %d %s",
				det->GetPosID().c_str(),contact,typeString.c_str()),
			1024,0,1024);
	}
}

orruba = new ORRUBA();

data.Branch("orruba","ORRUBA",&orruba);
data.Branch("sX3",superX3s);
data.Branch("bb10",bb10s);
data.Branch("qqq5",qqq5s);

*/

