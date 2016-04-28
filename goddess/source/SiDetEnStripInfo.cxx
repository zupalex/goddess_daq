#include "TClass.h"
#include "TMath.h"
#include "orrubaDet.h"
#include "superX3.h"
#include "SiDetEnStripInfo.h"

SiDetEnStripInfo::SiDetEnStripInfo(){}
SiDetEnStripInfo::~SiDetEnStripInfo(){}

void SiDetEnStripInfo::Clear()
{
  e.Clear();
  eSum.Clear();
  eRes.Clear();
  strip.Clear();
  stripMax.Clear();
}

void SiDetEnStripInfo::GetEnSumAndStripMax(bool isNType)
{
  std::vector<int> sMax;
  std::vector<float> enMax;
  std::vector<float> enSum;

  sMax.clear();
  enMax.clear();
  enSum.clear();

  std::vector<float> enToSum = isNType ? e.n : e.p;
  std::vector<int> stripToTreat = isNType ? strip.n : strip.p;

  std::vector<std::vector<int>> stripsGroups;

  stripsGroups.clear(); 

  unsigned short sGroup = 0;

  for(unsigned short en = 0; en < enToSum.size(); en++)
    {
      if(stripsGroups.size() == 0)
	{
	  goto ADDNEWSTRIPSGROUP;
	}
      else
	{
	  for(unsigned short gr = 0; gr < stripsGroups.size(); gr++)
	    {
	      for(unsigned short st = 0; st < stripsGroups[gr].size(); st++)
		{
		  if(TMath::Abs(stripToTreat[en] -  stripsGroups[gr][st]) <= 1)
		    {
		      stripsGroups[gr].push_back(stripToTreat[en]);
		      sGroup = gr;
		      goto DOSUM;
		    }
		}
	    }  
	}

    ADDNEWSTRIPSGROUP:
      {
 	  std::vector<int> newStripsGroup;
	  newStripsGroup.clear();

	  newStripsGroup.push_back(stripToTreat[en]);
	  stripsGroups.push_back(newStripsGroup);
	  sGroup = stripsGroups.size()-1;    
      }
      
    DOSUM:
      if(sGroup == enSum.size())
	{
	  enSum.push_back(enToSum[en]);
	  enMax.push_back(enToSum[en]);
	  sMax.push_back(stripToTreat[en]);
	}
      else 
	{
	  enSum[sGroup] += enToSum[en];

	  if(enToSum[en] > enMax[sGroup])
	    {
	      enMax[sGroup] = enToSum[en];
	      sMax[sGroup] = stripToTreat[en];
	    }
	}
    }

  if(enSum.size() > 0)
    {
      for(unsigned short multG = 0; multG < enSum.size(); multG++)
	{
	  if(isNType)
	    {
	      eSum.n.push_back(enSum[multG]);
	      stripMax.n.push_back(sMax[multG]);
	    }
	  else
	    {
	      eSum.p.push_back(enSum[multG]);
	      stripMax.p.push_back(sMax[multG]);
	    }
	}
    }
}

void SiDetEnStripInfo::AddStripEnergyPair(siDet::ValueMap enMap, int strip_, bool isNType, std::string detType_, unsigned short depth_)
{
  //boolean which returns true if we are dealing with the resistive layer of a superX3 detector
  bool isSuperX3Res = (detType_ == "superX3" && depth_ == 1 && !isNType);

  std::vector<int> *strips = isNType ? &strip.n : &strip.p;

  std::vector<float> *energies = isNType ? &e.n : &e.p;

  float en_ = 0.0;
  int st_ = isSuperX3Res ? superX3::GetStrip(strip_) : strip_;
  
  if(!isSuperX3Res)
    {
      en_ = enMap.find(st_)->second;

      if(en_ > 0.0)
	{
	  strips->push_back(st_);
	  energies->push_back(en_);
	}
    }
  else
    {
      //Check if this strip has already been treated. This should not happen for anything other than the superX3 resistive layer since the input of 
      //this function is the number of the resistive strip itself and not the near/far contact. If the strip has already been treated, we don't go further
      //and stop the execution of this function here.
      if(std::find(strips->begin(), strips->end(), st_) != strips->end())
	{
	  return;
	}

      int nearStrip = superX3::GetNearContact(st_);
      int farStrip = superX3::GetFarContact(st_);
      
      float en_near = 0.0;
      float en_far = 0.0;
	  
      en_near = enMap.find(nearStrip)->second;
      en_far = enMap.find(farStrip)->second;	  
      
      en_ = en_near + en_far;
	  
      if(en_near > 0.0 || en_far > 0.0)
	{
	  strips->push_back(st_);
	  energies->push_back(en_);
	      
	  eRes.p.push_back(en_near > 0.0 ? en_near : -10); //If no energy, we push it to -10 to not pollute the histogram when plotting from 0.
	  eRes.n.push_back(en_far > 0.0 ? en_far : -10); //If no energy, we push it to -10 to not pollute the histogram when plotting from 0.      
	}
    }
}

ClassImp(SiDetEnStripInfo)
