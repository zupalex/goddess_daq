#include "LiquidScint.h"
#include <iostream>

LiquidScint::LiquidScint() {

}

LiquidScint::LiquidScint(std::string desc) {
	SetDescription(desc);

}

LiquidScint::~LiquidScint() {

}

void LiquidScint::SetRawValue(unsigned int detectorChannel, bool secondaryType, unsigned int rawValue) {
  
    if(detectorChannel == 0) enRaw = rawValue;
    if(detectorChannel == 1) psdRaw = rawValue;
    if(detectorChannel == 2) tacRaw = rawValue;

    if(secondaryType) std::cerr << " No secondary type for Liquid Scint "<< std::endl; 
}

void LiquidScint::Clear() {

  enRaw = 0;
  psdRaw = 0;
  tacRaw = 0;

}

float LiquidScint::GetRawEnergy(){
  return enRaw;
}

float LiquidScint::GetRawPSD(){
  return psdRaw;
}

float LiquidScint::GetRawTAC(){
  return tacRaw;
}

ClassImp(LiquidScint)
