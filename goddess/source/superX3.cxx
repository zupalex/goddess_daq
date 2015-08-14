#include "superX3.h"

#include <cmath>
#include <iostream>

#include "TMath.h"

superX3::superX3() {
	siDet::SetNumContacts(8,4);
	Clear();
}

superX3::superX3(std::string serial_Num, unsigned short sector_, unsigned short depth_,
	bool up_Stream, SolidVector position) :
	orrubaDet(serial_Num, sector_, depth_, up_Stream, position)
{
	siDet::SetNumContacts(8,4);
	ConstructBins();
	Clear();
}

superX3::~superX3() {}

/**Construct arrays of bin edges for ROOT histograms based on the provided position 
 * of the detector. Currently constructs binning in the n, p and z direction in mm
 * and in the azimuthal and polar direction in degrees.
 *
 *
 * \note Assumes that the p type strips lie parallel to the z-axis and the n type
 * strips are perpendicular to the cylindrical radial dimension rho.
 */
void superX3::ConstructBins () {
	float pStripPitch = 40.3 / 4; //mm
	float nStripPitch = 75 / 4; //mm

	//float detRotation = TMath::PiOver2() + detPos.Theta();
	if (fabs(detPos.RotZ() - (TMath::PiOver2() + detPos.Phi())) > 0.001) 
		std::cerr << "WARNING: Detector " << serialNum << " is not normal to the XY radial direction! " << detPos.RotZ() << " != " << TMath::PiOver2() + detPos.Phi() << "\n";

	SolidVector cornerPos(detPos.X() - 2 * pStripPitch * cos(detPos.RotZ()) * cos(detPos.RotPhi()),
		detPos.Y() - 2 * pStripPitch * sin(detPos.RotZ()) * cos(detPos.RotPhi()),
		detPos.Z() - 2 * nStripPitch * cos(detPos.RotPhi()),
		detPos.RotZ(),
		detPos.RotPhi());

	for (unsigned int strip=0; strip<=4; strip++) {			
		//compute the binning along the p and n type strip directions in mm.
		binsP[strip] = strip * pStripPitch;
		binsN[strip] = strip * nStripPitch;

		nStripEdgePos[strip].SetXYZ(
			cornerPos.X() + 2 * pStripPitch * cos(cornerPos.RotZ()) * cos(detPos.RotPhi()),
			cornerPos.Y() + 2 * pStripPitch * sin(cornerPos.RotZ()) * cos(detPos.RotPhi()),
			cornerPos.Z() + binsN[strip] * cos(detPos.RotPhi()));
		//The strips have x and y computed from detector vector projected onto the 
		//XY vector from the detector origin to the strip edge.
		pStripEdgePos[strip].SetXYZ(cornerPos.X() + binsP[strip] * cos(cornerPos.RotZ()) * cos(detPos.RotPhi()), 
			cornerPos.Y() + binsP[strip] * sin(cornerPos.RotZ()) * cos(detPos.RotPhi()), 
			cornerPos.Z() + 2 * nStripPitch * cos(detPos.RotPhi()));

		//Z Position of each nStrip edge
		binsZ[strip] = nStripEdgePos[strip].Z();
		//Azimuthal angle 
		binsAzimuthal[strip] = TMath::RadToDeg() * pStripEdgePos[strip].Phi();
		//Polar angle is computed in reverse as the higher strips are at smaller
		//	angles and the list must be in ascending order.
		binsPolar[4-strip] = TMath::RadToDeg() * nStripEdgePos[strip].Theta();
	}

	for (unsigned int strip=0;strip<4;strip++){
		binsPCenter[strip] = (binsP[strip] + binsP[strip+1]) / 2;
		binsNCenter[strip] = (binsN[strip] + binsN[strip+1]) / 2;
		binsZCenter[strip] = (binsZ[strip] + binsZ[strip+1]) / 2;
		binsAzimuthalCenter[strip] = (binsAzimuthal[strip] + binsAzimuthal[strip+1]) / 2;
	}

#ifdef VERBOSE
	std::cout << serialNum << "\tcenter:\t";
	detPos.Print();
	std::cout << "\tcorner:\t";
	cornerPos.Print();
	std::cout << "\tpStrip:\t";
	pStripEdgePos[0].Print();
	std::cout << "\t       \t";
	pStripEdgePos[4].Print();
	std::cout << "\tnStrip:\t";
	nStripEdgePos[0].Print();
	std::cout << "\t       \t";
	nStripEdgePos[4].Print();
#endif

}

void superX3::Clear() {
	siDet::Clear();

	enCalPstrip.clear();
	stripPosRaw.clear();
	stripPosCal.clear();
	for (int i=0;i<4;i++) {
		stripContactMult[i] = 0;
	}

	enPtype = 0;
	enNtype = 0;
	enCal = 0;

	eventPos.SetXYZ(0,0,0);
}

/**Both the raw and calibrated positions are calculated as well as the energy 
 * deposited in the strip. The energy is computed as 
 * \f$ E = N + \sum_i p_i * F^i \f$, where \f$N\f$ and \f$F\f$ are the near and far 
 * contacts respectively and \f$p_i\f$ is the ith coefficient of the polynomial 
 * calibration function.
 *
 * \note The event position is only computed in the p strip multiplicity is 1. The 
 * higher multiplicity events are not currently supported.
 *
 *	\param[in] strip The number of the strip to update the position.
 */
void superX3::UpdatePosition(int strip) {	
	//Compute the near and far contacts
	int nearContact = 2 * strip, farContact = 2 * strip + 1;
	if (strip > 1) {
		nearContact = 2 * strip + 1;
		farContact = 2 * strip;
	}

	//Check if both contacts have had there energy set.
	if (!ContactHit(nearContact, false) || !ContactHit(farContact, false)) return;

	float nearEnergy = GetCalEnergy(nearContact);
	float farEnergy = GetCalEnergy(farContact);

	//Compute the strip energy by 
	//E = N + p0 + p1 * F + p2 * F^2 ...
	float stripEnergy = nearEnergy ;
	for (size_t power = 0; power < parStripEnCal[strip].size(); power++)
		stripEnergy += parStripEnCal[strip].at(power) * pow(farEnergy,power);
	enCalPstrip[strip] = stripEnergy;

	//Compute the resistive strip position by
	// P_raw = (N - F) / E 
	// P_cal = p0 + p1 * P_raw + p2 * P_raw^2
	float stripPosRaw_ = (nearEnergy - farEnergy) / stripEnergy;
	float stripPosCal_ = 0.0;
	for (size_t power = 0; power < parPosCal[strip].size(); power++)
		stripPosCal_ += parPosCal[strip].at(power) * pow (stripPosRaw_, power);

	stripPosRaw[strip] = stripPosRaw_;
	stripPosCal[strip] = stripPosCal_;
	

	//Compute the event position.
	//If the p and n strip multiplicity is 1 we can easily find the position.
	if (enCalPstrip.size() == 1) {
		//We construct the event position from the following:
		//	X: The middle point of the pStrip
		//	Y: The middle point of the pStrip
		//	Z: The calibrated position reported from the resistive strip.
		float xValue = (pStripEdgePos[strip].X() + pStripEdgePos[strip + 1].X()) / 2.;
		float yValue = (pStripEdgePos[strip].Y() + pStripEdgePos[strip + 1].Y()) / 2.;
		eventPos.SetXYZ(xValue, yValue, stripPosCal_);
	}
	//Otherwise we just zero the vector.
	else eventPos.SetXYZ(0,0,0);
	
}


void superX3::SetStripPosCalibPars(int strip, std::vector<float> pars) {
	if (!ValidStrip(strip)) {
		fprintf(stderr,"ERROR: Unable to assign strip position calibration parameters!\n");
			return;
	}

	parPosCal[strip] = pars;
}

void superX3::SetStripEnCalibPars(int strip, std::vector<float> pars) {
	if (!ValidStrip(strip)) {
		fprintf(stderr,"ERROR: Unable to assign strip energy calibration parameters!\n");
			return;
	}

	parStripEnCal[strip] = pars;
}

/**
 * \param[in] strip The strip number to validate.
 *	\return True is strip exists.
 */
bool superX3::ValidStrip(int strip) {
	if (strip < 0 || strip > 3) {
		fprintf(stderr, "ERROR: Invalid strip specification for illegal strip %d in detector.\n",strip);
		return false;
	}
	return true;
}

/**This method is called when a contact energy is updated. We call the parent 
 * siDet::SetRawValue to handle storing the raw and calibrated value. If the update
 * was a p type contact we check if another p contact in the same strip has been set
 * and if so we make a call to compute the position the event occurred in the strip.
 *
 * \param[in] contact The number of the contact which was updated.
 *	\param[in] rawValue The raw contact value in channels.
 * \param[in] nType Whether the contact was n Type.
 */
void superX3::SetRawValue(unsigned int contact, bool nType, int rawValue) {
	if (!ValidContact(contact, nType)) {
		char type = 'p';
		if (nType) type = 'n';
		std::cerr << "ERROR: Unable to set raw value for Super X3 " << serialNum << " " << type << "-type contact: " << contact << "!\n";
		return;
	}

	//Call parent method to handle calibration.
	siDet::SetRawValue(contact, nType, rawValue);

	if (nType) {
		//Set the energy value only if the multiplicity is 1.
		enNtype += GetCalEnergy(contact, nType);
		enCal += GetCalEnergy(contact, nType);
	}
	else {
		//Determine which strip this contact is in.
		int strip = GetStrip(contact);
		stripContactMult[strip]++;

		//If more than one contact fired we can compute position and energy. 
		if (stripContactMult[strip] > 1) {
			UpdatePosition(GetStrip(contact));
			//Store the energy only if the multiplicity is one.
			enPtype += enCalPstrip[strip];
			enCal += GetCalEnergy(contact, nType);
		}
	}
}

int superX3::GetStrip(int contact) {
	int strip = contact / 2;
	return strip;
}


ClassImp(superX3)


