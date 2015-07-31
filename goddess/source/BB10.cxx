#include "BB10.h"
#include <cmath>
#include <iostream>
#include "TMath.h"

ClassImp(BB10)

BB10::BB10(std::string serialNum, unsigned short sector, unsigned short depth, 
	bool upStream, SolidVector detPos) :
	orrubaDet(serialNum, sector, depth, upStream, detPos)
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

	//added in cornerPos vector which is located on the bottom (cable) left corner of silicon looking at the front face of detector
	// the cos(detPos.RotPhi()) will be +1 or -1 depending on if it is upstream or downstream respectively. Detectors are head to head in barrel
	SolidVector cornerPos(detPos.X() - 4 * pStripPitch * cos(detPos.RotZ()) * cos(detPos.RotPhi()),
		detPos.Y() - 4 * pStripPitch * sin(detPos.RotZ()) * cos(detPos.RotPhi()),
		detPos.Z() - halfLength * cos(detPos.RotPhi()),
		detPos.RotZ(),
		detPos.RotPhi());

	for (unsigned int strip=0; strip<=8; strip++) {			
		//compute the binning along the p and n type strip directions in mm.
		//strip 0 is furthest right strip, same side as corner position
		binsP[strip] = strip * pStripPitch;

		//The strips have x and y computed from detector vector projected onto the 
		//XY vector from the detector origin to the strip edge.
		// this vector needed the same treatment of cos(detPos.RotPhi()) to calculate the correct strip location upstream or downstream
		pStripEdgePos[strip].SetXYZ(cornerPos.X() + binsP[strip] * cos(cornerPos.RotZ())*cos(detPos.RotPhi()), 
			cornerPos.Y() + binsP[strip] * sin(cornerPos.RotZ())*cos(detPos.RotPhi()), 
			cornerPos.Z() + halfLength*cos(detPos.RotPhi()) );

		//Azimuthal angle 
		binsAzimuthal[strip] = TMath::RadToDeg() * pStripEdgePos[strip].Phi();
	}


#ifdef VERBOSE
	std::cout << serialNum << "\tcenter:\t";
	detPos.Print();
	std::cout << "\tcorner:\t";
	cornerPos.Print();
	std::cout << "\tpStrip:\t";
	pStripEdgePos[0].Print();
	std::cout << "\t       \t";
	pStripEdgePos[7].Print();
	std::cout << "\tnStrip:\t";
#endif


}

void BB10::Clear() {
	siDet::Clear();
	enPtype = 0;
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
void BB10::SetRawValue(unsigned int contact, bool nType, int rawValue) {
	siDet::SetRawValue(contact, nType, rawValue);

	if (!nType) {
		//Set the energy value only if the multiplicity is 1.
		if (GetContactMult(nType) == 1) 
			enPtype = GetCalEnergy(contact, nType);
		else enPtype = 0;
	}


}
