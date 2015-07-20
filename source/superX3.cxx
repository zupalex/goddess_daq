#include "superX3.h"
#include <cmath>
#include "TMath.h"

superX3::superX3(TVector3 pos, float rotationAngle) :
	detPos(pos),
	detRotation(rotationAngle)
{
	ConstructBins();
	Clear();
	siDet::SetNumContacts(8,4);
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

	for (int strip=0;strip<=4;strip++) {			
		//compute the binning along the p and n type strip directions in mm.
		binsP[strip] = strip * pStripPitch;
		binsN[strip] = strip * nStripPitch;

		nStripEdgePos[strip].SetXYZ(detPos.X() + 2 * pStripPitch, detPos.Y(), detPos.Z() + binsN[strip]);
		//The strips have x and y computed from detector vector projected onto the 
		//XY vector from the detector origin to the strip edge.
		pStripEdgePos[strip].SetXYZ(detPos.X() + binsP[strip] * cos(detRotation), detPos.Y() + binsP[strip] * sin(detRotation), detPos.Z() + 2 * nStripPitch);

		//Z Position of each nStrip edge
		binsZ[strip] = nStripEdgePos[strip].Z();
		//Azimuthal angle 
		binsAzimuthal[strip] = TMath::RadToDeg() * pStripEdgePos[strip].Phi();
		//Polar angle is computed in reverse as the higher strips are at smaller
		//	angles and the list must be in ascending order.
		binsPolar[4-strip] = TMath::RadToDeg() * nStripEdgePos[strip].Theta();
	}

}

void superX3::Clear() {
	siDet::Clear();
	for (int i=0;i<4;i++) {
		enCalPstrip[i] = 0;
		stripPosRaw[i] = -2;
		stripPosCal[i] = -1;
		stripContactMult[i] = 0;
	}

	pStripMult = 0;
	nStripMult = 0;

	eventPos.SetXYZ(0,0,0);
}

/**Both the raw and calibrated positions are calculated as well as the energy 
 * deposited in the strip. The energy is computed as 
 * \f$ E = N + \sum_i p_i * F^i \f$, where \f$N\f$ and \f$F\f$ are the near and far 
 * contacts respectively and \f$p_i\f$ is the ith coefficient of the polynomial 
 * calibration function.
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

	//Compute the strip energy by 
	//E = N + p0 + p1 * F + p2 * F^2 ...
	enCalPstrip[strip] = GetCalEnergy(nearContact);
	for (size_t power = 0; power < parStripEnCal[strip].size(); power++)
		enCalPstrip[strip] += parStripEnCal[strip].at(power) * pow(GetCalEnergy(farContact),power);

	//Compute the event position.
	//If the p and n strip multiplicity is 1 we can easily find the position.
	if (pStripMult == 1 && nStripMult == 1) {
		//We construct the event position from the following:
		//	X: The middle point of the pStrip
		//	Y: The middle point of the pStrip
		//	Z: The calibrated position reported from the resistive strip.
		float xValue = (pStripEdgePos[strip].X() + pStripEdgePos[strip + 1].X()) / 2.;
		float yValue = (pStripEdgePos[strip].Y() + pStripEdgePos[strip + 1].Y()) / 2.;
		eventPos.SetXYZ(xValue, yValue, enCalPstrip[strip]);
	}
	
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
 * \param[in] contact The strip number to validate.
 * \param[in] nType Whether the contact is n type.
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
 * siDet::SetEnergy to handle storing the raw and calibrated value. If the update
 * was a p type contact we check if another p contact in the same strip has been set
 * and if so we make a call to compute the position the event occurred in the strip.
 *
 * \param[in] contact The number of the contact which was updated.
 * \param[in] nType Whether the contact was n Type.
 */
void superX3::SetEnergy(unsigned int contact, int rawValue, bool nType) {
	if (!ValidContact(contact)) return;

	//Call parent method to handle calibration.
	siDet::SetEnergy(contact, rawValue, nType);

	if (nType) {

	}
	else {
		int strip = GetStrip(contact);
		stripContactMult[strip]++;
		if (stripContactMult[strip] > 1) UpdatePosition(GetStrip(contact));
	}
}

int superX3::GetStrip(int contact) {
	int strip = contact / 2;
	return strip;
}


ClassImp(superX3)


