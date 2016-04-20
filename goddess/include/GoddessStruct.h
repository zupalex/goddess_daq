#ifndef GODDESSSTRUCT_H
#define GODDESSSTRUCT_H

#include <vector>
#include "TVector3.h"
#include "SiDetEnStripInfo.h"

///Structure of gamma ray data from DGS
struct GamData {
	///The calibrated energy of the gamma.
	float en;
	///The type of gamma ray detector. GE=1, BGO=2
	int type;
	//The number of the gamma ray detector
	int num;
	///The time of the gamma relative to the beginning of the event.
	unsigned int time;
};

///Structure of silicon data from ORRUBA
struct SiData 
{ 
  ///The info about all the strips which fired and the energies collected by each of them for the dE layer.
  SiDetEnStripInfo dE;
  ///The info about all the strips which fired and the energies collected by each of them for the E1 layer.
  SiDetEnStripInfo E1;
  ///The info about all the strips which fired and the energies collected by each of them for the E2 layer.
  SiDetEnStripInfo E2;
  
  ///The unique ID of the telescope to allow an easier selection of a specific portion of the detection system.
  /** This ID is composed of 4 digits following this pattern 
   *  [1 for endcap / 2 for barrel] [1 for downstream / 2 for upstream] [sector numer]
   *   Examples: 
   *   -> QQQ5-UA would belong to telescope 1200 (1 for endcap)(2 for upstream)(00 for sector #0)
   *   -> SX3-D10 would belong to telescope 2110 (2 for barrel)(1 for downstream)(10 for sector #10)
   *   Note that the same result could be achieved using combined conditions over the members "isBarrel", "isUpstream" and "sector"
   */
  ushort telescopeID;

  ///The sector the silicon event occured. For the barrel detector: goes from 0 (90 degrees compared to beamline) to 11 clockwise. For QQQ5: A == 0, B == 1, C == 2, D == 3.
  ushort sector;
  ///The event interaction position.
  std::vector<TVector3> pos;
  ///True if in the barrel, otherwise occured in the endcap.
  bool isBarrel;
  ///True if upstream, otherwise occured downstream.
  bool isUpstream;
  ///The time of the particle relative to the beginning of the event.
  unsigned int time;
};





///Structure of the ion chamber data, including the scintillator.
struct IonData {
	///The energy deposited in the dE anodes.
	float dE;
	///The residaul energy in resE anodes.
	float resE;
	///The energy deposited in the E1 anodes.
	float E;
	///Energy deposited in the siPMTs.
	float scintE[4];
	///Timing of the siPMTs.
	float scintT[4];
	///The time of the ion relative to the beginning of the event.
	unsigned int time;
};
#endif
