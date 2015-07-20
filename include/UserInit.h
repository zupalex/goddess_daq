
/*--------------------------*/
/* user initialization code */
/* (may be empty..........) */
/*--------------------------*/

orruba = new ORRUBA();

sX3 = new superX3(TVector3(43.3/2,7.85/2*25.4,0),0);

data.Branch("orruba","ORRUBA",&orruba);
data.Branch("superX3","superX3",&sX3);
