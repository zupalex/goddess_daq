#include "QQQ5.h"
#include <cmath>
#include <iostream>

#include "TMath.h"

QQQ5::QQQ5()
{
	siDet::SetNumContacts(32, 4);
	Clear();
}
/**We assuming the position provided for the detector is at the radial center of the
 * detector and the clockwise is aligned at the rotation angle.
 */
QQQ5::QQQ5(std::string serial_Num, unsigned short sector_, unsigned short depth_, bool up_Stream, SolidVector position) :
		orrubaDet(serial_Num, sector_, depth_, up_Stream, position)
{
	siDet::SetNumContacts(32, 4);
	Clear();
}

QQQ5::~QQQ5()
{
}

/**Construct arrays of bin edges for ROOT histograms based on the provided position
 * of the detector. Currently constructs binning in the n, p and z direction in mm
 * and in the azimuthal and polar direction in degrees.
 *
 *
 * \note Assumes that the detector lies in the XY plane with the p strip normals
 * along the radial dimension rho.
 */
void QQQ5::ConstructBins()
{
//     float QQQ5_active_length = 56.8; // mm

	TVector3 firstStripOffset(0, firstStripWidth / 2., 0); // everything in mm

	TVector3 prevStripRefDetCenter = firstStripOffset;

	pStripCenterPos[0] = detPos.GetTVector3() + firstStripOffset;
//     cerr << "QQQ5 strip #0 : " << pStripCenterPos[0].Y() << endl;

	for (int i = 1; i < 32; i++)
	{
		float prevStripWidth = firstStripWidth - (i - 1) * deltaPitch;
		float currStripWidth = firstStripWidth - i * deltaPitch;

		TVector3 pStPosRefDetCenter = prevStripRefDetCenter + TVector3(0, (prevStripWidth + currStripWidth) / 2., 0);
		prevStripRefDetCenter = pStPosRefDetCenter;

		pStPosRefDetCenter.SetPhi(pStPosRefDetCenter.Phi() + detPos.RotZ());

		pStripCenterPos[i] = detPos.GetTVector3() + pStPosRefDetCenter;
//         cerr << "QQQ5 strip #" << i << " : " << pStripCenterPos[i].Y() << endl;
	}

//     for ( int i = 0; i < 4; i++ )
//     {
//         TVector3 nStPosRefDetCenter = firstStripOffset + TVector3 ( 0, QQQ5_active_length/2., 0 );
//
//         nStripCenterPos[i] = detPos.GetTVector3() + nStPosRefDetCenter;
//         nStripCenterPos[i].SetPhi ( nStripCenterPos[i].Phi() + ( 3 - 2*i ) /8. * TMath::PiOver4() );
//     }
}

void QQQ5::Clear()
{
	siDet::Clear();

	stripsP.clear();
	enRawP.clear();
	enCalP.clear();
	timeP.clear();

	stripsN.clear();
	enRawN.clear();
	enCalN.clear();
	timeN.clear();

	eventPos.SetXYZ(0, 0, 0);
}

void QQQ5::SetPosID()
{
	if (upStream)
	{
		posID.append("U");
	}
	else
	{
		posID.append("D");
	}

	static const char *sectorCode[4] = { "A", "B", "C", "D" };
	posID.append(sectorCode[sector]);

	posID.append("_");

	if (depth == 0)
	{
		posID.append("dE");
	}
	else if (depth == 1)
	{
		posID.append("E1");
	}
	else if (depth == 2)
	{
		posID.append("E2");
	}
}

void QQQ5::SetGeomParams(map<string, double> geomInfos_)
{
	firstStripWidth = geomInfos_["QQQ5 First Strip Width"];
	deltaPitch = geomInfos_["QQQ5 Delta Pitch"];
}

/**This method is called when a contact energy is updated. We call the parent
 * siDet::SetRawValue to handle storing the raw and calibrated value. If the update
 * was a p type contact we check if another p contact in the same strip has been set
 * and if so we make a call to compute the position the event occurred in the strip.
 *
 * \param[in] contact The number of the contact which was updated.
 *  \param[in] rawValue The raw contact value in channels.
 * \param[in] nType Whether the contact was n Type.
 */
void QQQ5::SetRawValue(unsigned int contact, bool nType, int rawValue, int ignThr)
{
	if (!ValidContact(contact, nType)) std::cerr << "ERROR: Unable to set raw value for QQQ5 " << serialNum << " " << (nType ? 'n' : 'p') << "-type contact: "
			<< contact << "!\n";

	//Call parent method to handle calibration.
	siDet::SetRawValue(contact, nType, rawValue, ignThr);
}

float QQQ5::GetEnSum(bool nType, bool calibrated, float pos)
{
	float enSum = 0.0;

	std::vector<float>* toSum;

	if (nType && calibrated) toSum = &enCalN;
	else if (nType && !calibrated) toSum = &enRawN;
	else if (!nType && calibrated) toSum = &enCalP;
	else if (!nType && !calibrated) toSum = &enRawP;
	else return 0.0;

	for (unsigned int i = 0; i < (*toSum).size(); i++)
	{
		enSum += toSum->at(i);
	}
	
	enSum += pos;

	return enSum;
}

void QQQ5::SortAndCalibrate(bool doCalibrate)
{
	siDet::ValueMap enPMap, enNMap;

	enPMap = GetRawEn(false);
	enNMap = GetRawEn(true);

	siDet::TimeMap tsPMap, tsNMap;

	tsPMap = GetTsMap(false);
	tsNMap = GetTsMap(true);

	for (auto stripItr = enPMap.begin(); stripItr != enPMap.end(); ++stripItr)
	{
		int st_ = stripItr->first;
		float en_ = stripItr->second;

		stripsP.push_back(st_);
		enRawP.push_back(en_);
		timeP.push_back(tsPMap[st_]);

		if (doCalibrate)
		{
			std::vector<std::vector<double>> calPars = GetEnParCal(false);

			enCalP.push_back(en_ * calPars[st_][1] + calPars[st_][0]);
		}
	}

	for (auto stripItr = enNMap.begin(); stripItr != enNMap.end(); ++stripItr)
	{
		int st_ = stripItr->first;
		float en_ = stripItr->second;

		stripsN.push_back(st_);
		enRawN.push_back(en_);
		timeN.push_back(tsNMap[st_]);

		if (doCalibrate)
		{
			std::vector<std::vector<double>> calPars = GetEnParCal(true);
			

			enCalN.push_back(en_ * calPars[st_][1] + calPars[st_][0]);
		}
	}
}

int QQQ5::GetContactMult(bool calibrated)
{
	if (calibrated) return enCalP.size() + enCalN.size();
	else return enRawP.size() + enRawN.size();
}

int QQQ5::GetContactMult(bool contactType, bool calibrated)
{
	if (contactType == siDet::nType)
	{
		if (calibrated) return enCalN.size();
		else return enRawN.size();
	}
	else
	{
		if (calibrated) return enCalP.size();
		else return enRawP.size();
	}
}

std::vector<float> QQQ5::GetHitsInfo(std::string info, std::vector<float>* dest)
{
	std::vector<float> request;

	if (info == "front energies raw") request = enRawP;
	else if (info == "back energies raw") request = enRawN;
	else if (info == "front energies cal") request = enCalP;
	else if (info == "back energies cal") request = enCalN;
	else std::cerr << "WARNING in QQQ5::GetHitsInfo -> requested member " << info << " does not exist!\n";

	if (dest != nullptr) *dest = request;

	return request;
}

float QQQ5::GetPosCh(bool calibrated, bool in_encal, float e_near, float e_far)
{
  float posch = 0;
  if (calibrated){
  posch = 0;
  }
  else posch =0;
  
  return posch;
}

float QQQ5::UpdatePosCh(float posch)
{
  float pos = posch*0;
  
  return pos;
}

std::vector<int> QQQ5::GetHitsInfo(std::string info, std::vector<int>* dest)
{
	std::vector<int> request;

	if (info == "front strips") request = stripsP;
	else if (info == "back strips") request = stripsN;
	else std::cerr << "WARNING in QQQ5::GetHitsInfo -> requested member " << info << " does not exist!\n";

	if (dest != nullptr) *dest = request;

	return request;
}

std::vector<long long unsigned int> QQQ5::GetHitsInfo(std::string info, std::vector<long long unsigned int>* dest)
{
	std::vector<long long unsigned int> request;

	if (info == "front timestamps") request = timeP;
	else if (info == "back timestamps") request = timeN;
	else std::cerr << "WARNING in QQQ5::GetHitsInfo -> requested member " << info << " does not exist!\n";

	if (dest != nullptr) *dest = request;

	return request;
}

void QQQ5::GetMaxHitInfo(int* stripMaxP, long long unsigned int* timeSampMaxP, int* stripMaxN, long long unsigned int* timeSampMaxN, bool calibrated)
{
	std::vector<float>* energiesN_;
	std::vector<float>* energiesP_;

	if (calibrated)
	{
		energiesN_ = &enCalN;
		energiesP_ = &enCalP;
	}
	else
	{
		energiesN_ = &enRawN;
		energiesP_ = &enRawP;
	}

	float enMax = 0.0;

	for (unsigned int i = 0; i < energiesP_->size(); i++)
	{
		if (energiesP_->at(i) > enMax)
		{
			if (stripMaxP != nullptr) *stripMaxP = stripsP.at(i);
			enMax = energiesP_->at(i);
			if (timeSampMaxP != nullptr) *timeSampMaxP = timeP.at(i);
		}
	}

	enMax = 0;

	for (unsigned int i = 0; i < energiesN_->size(); i++)
	{
		if (energiesN_->at(i) > enMax)
		{
			if (stripMaxN != nullptr) *stripMaxN = stripsN.at(i);
			enMax = energiesN_->at(i);
			if (timeSampMaxN != nullptr) *timeSampMaxN = timeN.at(i);
		}
	}
}

int QQQ5::GetMultiplicity(bool nType, bool calibrated)
{
	if (nType && calibrated) return enCalN.size();
	else if (nType && !calibrated) return enRawN.size();
	else if (!nType && calibrated) return enCalP.size();
	else if (!nType && !calibrated) return enRawP.size();
	else return 0;
}

void QQQ5::SetEnShiftVsPosGraph(std::string graphFileName)
{
	if (!graphFileName.empty())
	{
		TFile* grFile = new TFile(graphFileName.c_str(), "read");

		if (!grFile->IsOpen()) return;
	}
}

TVector3 QQQ5::GetEventPosition(bool calibrated)
{
	int pStripHit, nStripHit;
	GetMaxHitInfo(&pStripHit, nullptr, &nStripHit, nullptr, calibrated);

	TVector3 interactionPos = pStripCenterPos[pStripHit];

	if (nStripHit >= 0) interactionPos.SetPhi(interactionPos.Phi() - 3. / 16. * TMath::Pi() + nStripHit / 8. * TMath::Pi());

	return interactionPos;
}

ClassImp(QQQ5)

