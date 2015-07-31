
/*--------------------------*/
/* user initialization code */
/* (may be empty..........) */
/*--------------------------*/

config = new GoddessConfig("goddess.position","goddess.config");

qqq5Vect = config->GetQQQ5s();
qqq5s = new TClonesArray("QQQ5",qqq5Vect.size());

for (int det = 0;det < qqq5Vect.size(); det++) {
	qqq5s->AddAt(qqq5Vect.at(det),det);
}

superX3Vect = config->GetSuperX3s();
superX3s = new TClonesArray("superX3",superX3Vect.size());
hEnRawSX3 = new TH1F**[superX3Vect.size()];

for (int detNum = 0; detNum < superX3Vect.size(); detNum++) {
	superX3* det = superX3Vect.at(detNum);
	superX3s->AddAt(superX3Vect.at(detNum),detNum);
	hEnRawSX3[detNum] = new TH1F*[12];
	for (int contact = 0; contact < 12; contact++) {
		bool nType = false;
		std::string typeString = "p-type";
		if (contact >= 8) {
			nType = true;
			typeString = "n-type";
		}
			hEnRawSX3[detNum][contact] = new TH1F(
				Form("hEnRawSX3_%d_%d",detNum,contact),
				Form("superX3 %s Raw Energy Det: %d Contact: %d %s",
					det->GetSerialNum().c_str(),detNum,contact,typeString.c_str()),
				1024,0,1024);
	}
}

bb10Vect = config->GetBB10s();
bb10s = new TClonesArray("BB10",bb10Vect.size());
for (int det = 0; det < bb10Vect.size(); det++) {
	bb10s->AddAt(bb10Vect.at(det),det);

}

orruba = new ORRUBA();

data.Branch("orruba","ORRUBA",&orruba);
data.Branch("sX3",&superX3s);
data.Branch("bb10",&bb10s);
data.Branch("qqq5",&qqq5s);

