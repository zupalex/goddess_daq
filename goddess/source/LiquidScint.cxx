#include "LiquidScint.h"
#include <iostream>

LiquidScint::LiquidScint() {
  Clear();
}

LiquidScint::LiquidScint(std::string desc) {
  SetDescription(desc);
  Clear();
}

LiquidScint::~LiquidScint() {

}

void LiquidScint::SetRawValue(unsigned int detectorChannel, bool secondaryType, unsigned int rawValue) {
  
  if(secondaryType){
    std::cerr << " No secondary type for Liquid Scint "<< std::endl; 
    return;
  }
  
  if(detectorChannel == 0) enRaw = rawValue;
  else if(detectorChannel == 1) psdRaw = rawValue;
  else if(detectorChannel == 2){
    tacRaw = rawValue;
    //printf(" %s: %d \n",description.c_str(),rawValue);
  }
         
    

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
