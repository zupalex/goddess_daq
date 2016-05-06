#ifndef SIDETENSTRIPINFO_H
#define SIDETENSTRIPINFO_H

#include "PNPair.h"
#include "siDet.h"

///Support Class to make the sorting easier when storing the data relative to the Silicon Detectors in the root tree
class SiDetEnStripInfo
{
 private:

 public:
  ///Default constructor.
  SiDetEnStripInfo();
  
  ///Default destructor.
  virtual ~SiDetEnStripInfo();

  ///Clear the different class members of SiDetStripInfo
  void Clear();

  ///Retrieve the sum of the energies collected by all the strips which fired as well as the strip which collected the max energy for a specific detector. The front and back side and separated.
  void GetEnSumAndStripMax(bool isNType);

  void AddStripEnergyPair(siDet::ValueMap enMap, int strip_, bool isNType, std::string detType_, unsigned short depth_);

  void ReviveDeadStrip();

  ///The energies collected by the different strips which fired. Front and Back sides are separated and can be accessed using the "p" and "n" members respectively
  PNPair<float> e;

  ///The sum of the energies collected by the different strips which fired. Front and Back sides are separated and can be accessed using the "p" and "n" members respectively
  PNPair<float> eSum;
  
  ///The energies collected by the resistive strips in case of a SX3 type detector. "Near" and "Far" contacts can be accessed using the "p" and "n" members respectively
  PNPair<float> eRes;
  
  ///All the strips which fired. Front and Back sides are separated and can be accessed using the "p" and "n" members respectively
  PNPair<int> strip;

  ///The strip which collected the max energy. Front and Back sides are separated and can be accessed using the "p" and "n" members respectively
  PNPair<int> stripMax; 

  /// \cond This is just for ROOT and doesn't need to be documented
  ClassDef(SiDetEnStripInfo,1)
  /// \endcond
};

#endif
