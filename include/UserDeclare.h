
/*-----------------------*/
/* user declaration code */
/* (may be empty.......) */
/*-----------------------*/

const int numSX3 = 8;
const int numBB10 = 4;
const int numQQQ5 = 12;
const int numDetectors = numSX3 + numBB10 + numQQQ5;

ORRUBA *orruba; 

TClonesArray superX3s("superX3",numSX3);
TClonesArray qqq5s("QQQ5",numQQQ5);
TClonesArray bb10s("BB10",numBB10);

TTree data("data","Data");

TH1F* hEnRawSX3[numSX3][12];

