#include "LiquidScint.h"

LiquidScint::LiquidScint() {

}

LiquidScint::LiquidScint(std::string desc) {
	SetDescription(desc);

}

LiquidScint::~LiquidScint() {

}

<<<<<<< Updated upstream
void LiquidScint::SetRawValue(unsigned int detectorChannel, bool secondaryType, unsigned int rawValue) {
=======
void LiquindScint::SetDetID(){
  if (smallType) posID.append("2x2");
  else posID.append("4x6");

}

void LiquidScint::SetRawValue(unsigned int detectorChannel, bool smallType, int rawValue) {
  
  float *enRaw, *psdRaw, *tacRaw;
  if(smallType){
    enRaw
  }




  if(detectorChannel < numChannels)
    SetRawValue(detectorChannel,true,rawValue);
  else if(detectorChannel > numChannels + numChannels)
    SetRawValue(detectorChannel,false,rawValue);
  else 
    std::cerr << "ERROR: cannot set raw value for individual channel: " << detectorChannel << "!\n"; 

  if(secondaryType) {
    largeType += GetEnergy(detectorChannel,rawValue); 
  }
  else {
    smallType += GetEnergy(detectorChannel,rawValue);
  }
>>>>>>> Stashed changes

}

void LiquidScint::Clear() {


}

ClassImp(LiquidScint)
