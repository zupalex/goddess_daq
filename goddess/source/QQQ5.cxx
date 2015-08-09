#include "QQQ5.h"
#include <cmath>
#include <iostream>

#include "TMath.h"

QQQ5::QQQ5() {
	siDet::SetNumContacts(32,4);
	Clear();
}
/**We assuming the position provided for the detector is at the radial center of the
 * detector and the clockwise is aligned at the rotation angle. 
 */
QQQ5::QQQ5(std::string serialNum, unsigned short sector, unsigned short depth,
	bool upStream, SolidVector position) :
	orrubaDet(serialNum, sector, depth, upStream, position)
{
	siDet::SetNumContacts(32,4);
	ConstructBins();
	Clear();
}

QQQ5::~QQQ5() {}

/**Construct arrays of bin edges for ROOT histograms based on the provided position 
 * of the detector. Currently constructs binning in the n, p and z direction in mm
 * and in the azimuthal and polar direction in degrees.
 *
 *
 * \note Assumes that the detector lies in the XY plane with the p strip normals 
 * along the radial dimension rho.
 */
void QQQ5::ConstructBins () {
	float pStripRad[33] = 
		{25.2, 27.75, 30.25, 32.7, 35.1, 37.45, 39.75, 42.0,
		 44.2, 46.35, 48.45, 50.5, 52.5, 54.45, 56.35, 58.2,
		 60.0, 61.75, 63.45, 65.1, 66.7, 68.25, 69.75, 71.2,
		 72.6, 73.95, 75.25, 76.5, 77.7, 78.85, 79.95, 81.0, 82.0}; //mm
	
	//float nStripPitch = 90 / 4; //deg
	float nStripPitch = TMath::PiOver2() / 4;//rad

	float nTypeRadius = pStripRad[32] / 2;
	for (unsigned int strip=0; strip<=4; strip++) {			
		//compute the binning along the p and n type strip directions in mm.
		// strip 0 is furthest clockwise
		binsN[strip] = strip * nStripPitch; // recall actual channel numbers for backside strips go like 0,2,1,3

		nStripEdgePos[strip].SetXYZ(
			// added the offset 2*nStripPitch to compensate for detPos pointing to the center of the detector
			detPos.X() + nTypeRadius * cos(binsN[strip] + detPos.RotZ() - 2*nStripPitch),
			detPos.Y() + nTypeRadius * sin(binsN[strip] + detPos.RotZ() - 2*nStripPitch),		
			detPos.Z());

		//Azimuthal angle 
		binsAzimuthal[strip] = TMath::RadToDeg() * nStripEdgePos[strip].Phi();
	}

	//Compute the fraction of the radius in the x and y plane.
	float xFrac = cos(detPos.RotZ());//points to center of detector
	float yFrac = sin(detPos.RotZ());

	for (unsigned int strip=0; strip<=32; strip++) {			
		binsP[strip] = pStripRad[strip];

		//The strips have x and y computed from detector vector projected onto the 
		//XY vector from the detector origin to the strip edge.
		pStripEdgePos[strip].SetXYZ(
			detPos.X() + pStripRad[strip] * xFrac,
			detPos.Y() + pStripRad[strip] * yFrac,
			detPos.Z());

		binsRho[strip] = pStripEdgePos[strip].XYvector().Mod();
		int polarStrip = strip;
		if (detPos.Z() < 0) polarStrip = 32 - strip;
		binsPolar[polarStrip] = TMath::RadToDeg() * pStripEdgePos[strip].Theta();
	}

#ifdef VERBOSE
	std::cout << serialNum << "\tcenter:\t";
	detPos.Print();
	std::cout << "\tpStrip:\t";
	pStripEdgePos[0].Print();
	std::cout << "\t       \t";
	pStripEdgePos[31].Print();	
	std::cout << "\t       \t";
	pStripEdgePos[32].Print();
	std::cout << "\tnStrip:\t";
	nStripEdgePos[0].Print();
	std::cout << "\t       \t";
	nStripEdgePos[1].Print();
	std::cout << "\t       \t";
	nStripEdgePos[2].Print();
	std::cout << "\t       \t";
	nStripEdgePos[3].Print();
	std::cout << "\t       \t";
	nStripEdgePos[4].Print();
#endif

}

void QQQ5::Clear() {
	siDet::Clear();

	enPtype = 0;
	enNtype = 0;
	enCal = 0;

	eventPos.SetXYZ(0,0,0);
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
void QQQ5::SetRawValue(unsigned int contact, bool nType, int rawValue) {
	if (!ValidContact(contact, nType)) {;
		char type = 'p';
		if (nType) type = 'n';
		std::cerr << "ERROR: Unable to set raw value for QQQ5 " << serialNum << " " << type << "-type contact: " << contact << "!\n";
	}

	//Call parent method to handle calibration.
	siDet::SetRawValue(contact, nType, rawValue);

	if (nType) {
		enNtype += GetCalEnergy(contact, nType);
		enCal += GetCalEnergy(contact, nType);
	}
	else {
		enPtype += GetCalEnergy(contact, nType);
		enCal += GetCalEnergy(contact, nType);
	}
}


ClassImp(QQQ5)


