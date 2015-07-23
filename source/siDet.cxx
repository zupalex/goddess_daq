#include "siDet.h"
#include <cmath>

siDet::siDet() {
	Clear();
}
siDet::~siDet() {

}


void siDet::Clear() {
	enRawP.assign(enRawP.size(),0);
	enRawN.assign(enRawN.size(),0);
	enCalP.assign(enCalP.size(),0);
	enCalN.assign(enCalN.size(),0);

	multP = 0;
	multN = 0;
}

void siDet::SetNumContacts(int pType, int nType/*=0*/) {
	enRawP.resize(pType,0);
	enRawN.resize(nType,0);
	enCalP.resize(pType,0);
	enCalN.resize(nType,0);

	parEnCalP.resize(pType);
	parEnCalN.resize(nType);
}

/**
 * \param[in] contact The contact number to validate.
 * \param[in] nType Whether the contact is nType.
 *	\return True is contact has been declared.
 */
bool siDet::ValidContact(unsigned int contact, bool nType/*=false*/) {
	size_t size = enRawP.size();
	if (nType) size = enRawN.size();
	if (contact >= size) { 
			fprintf(stderr, "ERROR: Contact specified, %u, is invalid!\n",contact);
			return false;
	}

	return true;
}


/**The raw value is stored and the calibrated value is computed based on the loaded
 * calibration parameters.
 *
 *	\param[in] contact The contact to be adjusted.
 *	\param[in] rawValue The raw contact value in channels.
 *	\param[in] nType Whether the contact selected is n type.
 */
void siDet::SetEnergy(int contact, int rawValue, bool nType/*=false*/) {
	if (!ValidContact(contact)) return;

	//Get pointer to the raw an calibrated storage location.
	float *enRaw, *enCal;
	std::vector<float> *parEnCal;
	if (nType) {
		enRaw = &(enRawN.at(contact));
		enCal = &(enCalN.at(contact));
		parEnCal = &(parEnCalN.at(contact));
	}
	else {
		enRaw = &(enRawP.at(contact));
		enCal = &(enCalP.at(contact));
		parEnCal = &(parEnCalP.at(contact));
	}

	//Assign raw value and compute calibrated value.
	*enRaw = rawValue;
	for (size_t power = 0; power < parEnCal->size(); power++)
		*enCal += parEnCal->at(power) * pow(*enRaw,power);
}

float siDet::GetCalEnergy(int contact, bool nType/*=false*/) {
	if (!ValidContact(contact, nType)) return 0;
	if (nType) return enCalN.at(contact);
	return enCalP.at(contact);
}
int siDet::GetContactMult(bool nType/*=false*/) {
	if (nType) return multN;
	return multP;
}
