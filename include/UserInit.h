
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
for (int det = 0; det < superX3Vect.size(); det++) {
	superX3s->AddAt(superX3Vect.at(det),det);
//	for (int contact = 0; contact < 12; contact++) 
//		hEnRawSX3[det][contact] = new TH1F(Form("hEnRawSX3_%d_%d",det,contact),Form("superX3 %s Raw Energy Det: %d Contact: %d","name",det,contact),1024,0,1024);
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

