#include "SiDetEnStripInfo.h"
#include "TMath.h"

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

  for(int en = 0; en < enToSum.size(); en++)
    {
      int sGroup = -10;

      if(stripsGroups.size() == 0)
	{
	  goto ADDNEWSTRIPSGROUP;
	}
      else
	{
	  for(int gr = 0; gr < stripsGroups.size(); gr++)
	    {
	      for(int st = 0; st < stripsGroups[gr].size(); st++)
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
	  sGroup = 0;    
      }
      
    DOSUM:
      if(sGroup >= 0)
	{
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
    }

  if(enSum.size() > 0)
    {
      for(int multG = 0; multG < enSum.size(); multG++)
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

ClassImp(SiDetEnStripInfo)
