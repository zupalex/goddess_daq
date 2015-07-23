#include "BB10.h"
#include <cmath>
#include "TMath.h"

ClassImp(BB10)

BB10::BB10(std::string serialNum, unsigned short sector, unsigned short depth, 
	TVector3 detPos, float rotationAngle) :
	orrubaDet(serialNum, sector, depth, detPos, rotationAngle)
{
	siDet::SetNumContacts(8,0);
	ConstructBins();
	Clear();
}

/**Construct arrays of bin edges for ROOT histograms based on the provided position 
 * of the detector. Currently constructs binning in the p and z direction in mm
 * and in the azimuthal and polar direction in degrees.
 *
 *
 * \note Assumes that the p type strips lie parallel to the z-axis and the detector
 * lies perpendicular to the cylindrical radial dimension rho.
 */
void BB10::ConstructBins () {
	float pStripPitch = 40.3 / 8; //mm
	float halfLength = 75.0 / 2; //mm

	for (unsigned int strip=0; strip<=8; strip++) {			
		//compute the binning along the p and n type strip directions in mm.
		binsP[strip] = strip * pStripPitch;

		//The strips have x and y computed from detector vector projected onto the 
		//XY vector from the detector origin to the strip edge.
		pStripEdgePos[strip].SetXYZ(detPos.X() + binsP[strip] * cos(detRotation), detPos.Y() + binsP[strip] * sin(detRotation), detPos.Z() + halfLength);

		//Azimuthal angle 
		binsAzimuthal[strip] = TMath::RadToDeg() * pStripEdgePos[strip].Phi();
	}
}

void BB10::Clear() {
	siDet::Clear();
	enPtype = 0;
}

/**This method is called when a contact energy is updated. We call the parent 
 * siDet::SetEnergy to handle storing the raw and calibrated value. If the update
 * was a p type contact we check if another p contact in the same strip has been set
 * and if so we make a call to compute the position the event occurred in the strip.
 *
 * \param[in] contact The number of the contact which was updated.
 *	\param[in] rawValue The raw contact value in channels.
 * \param[in] nType Whether the contact was n Type.
 */
void BB10::SetEnergy(unsigned int contact, int rawValue, bool nType) {
	siDet::SetEnergy(contact, rawValue, nType);

	if (!nType) {
		//Set the energy value only if the multiplicity is 1.
		if (GetContactMult(nType) == 1) 
			enPtype = GetCalEnergy(contact, nType);
		else enPtype = 0;
	}


}
