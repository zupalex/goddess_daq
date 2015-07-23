
/*--------------------------*/
/* user initialization code */
/* (may be empty..........) */
/*--------------------------*/


for (int det = 0; det < numQQQ5; det++) {
	new(qqq5s[det]) QQQ5("name",TVector3(0,0,(4.375-0.7)*25.4),0);

}
for (int det = 0; det < numSX3; det++) {
	new(superX3s[det]) superX3("name",TVector3(43.3/2,7.85/2*25.4,0),0);
	for (int contact = 0; contact < 12; contact++) 
		hEnRawSX3[det][contact] = new TH1F(Form("hEnRawSX3_%d_%d",det,contact),Form("superX3 %s Raw Energy Det: %d Contact: %d","name",det,contact),1024,0,1024);
}
for (int det = 0; det < numBB10; det++) {
	new(bb10s[det]) BB10("name",TVector3(0,0,(4.375-0.7)*25.4),0);

}

orruba = new ORRUBA();

data.Branch("orruba","ORRUBA",&orruba);
data.Branch("sX3",&superX3s);
data.Branch("bb10",&bb10s);
data.Branch("qqq5",&qqq5s);

