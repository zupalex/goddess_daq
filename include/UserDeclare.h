
/*-----------------------*/
/* user declaration code */
/* (may be empty.......) */
/*-----------------------*/

const char numSX3 = 8;
const char numBB10 = 4;
const char numQQQ5 = 12;
const char numDetectors = numSX3 + numBB10 + numQQQ5;

ORRUBA *orruba; 

GoddessConfig *config;

TClonesArray superX3s("superX3",numSX3);
TClonesArray qqq5s("QQQ5",numQQQ5);
TClonesArray bb10s("BB10",numBB10);

TTree data("data","Data");

TH1F* hEnRawSX3[numSX3][12];

