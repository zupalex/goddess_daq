#include "siDet.h"
#include <cmath>
#include <iostream>

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
 *	\param[in] channel The channel to be adjusted.
 *	\param[in] rawValue The raw contact value in channels.
 */
void siDet::SetRawValue(unsigned int channel, int rawValue) {
	if (channel < enRawP.size())
		SetRawValue(channel,false,rawValue);
	else if (channel < enRawP.size() + enRawN.size()) 
		SetRawValue(channel,true,rawValue);
	else
		std::cerr << "ERROR: Cannot set raw value for invalid channel: " << channel << "!\n";
}
/**The raw value is stored and the calibrated value is computed based on the loaded
 * calibration parameters.
 *
 *	\param[in] contact The contact to be adjusted.
 *	\param[in] rawValue The raw contact value in channels.
 *	\param[in] nType Whether the contact selected is n type.
 */
void siDet::SetRawValue(unsigned int contact, bool nType, int rawValue) {
	if (!ValidContact(contact, nType)) return;

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

/*
 * \return True if successful.
 */
bool siDet::SetEnergyCalib(std::vector<float> par, int contact, bool nType/*=false*/) {
	if (!ValidContact(contact,nType)) return false;
	if (nType) parEnCalP.at(contact) = par;
	else parEnCalP.at(contact) = par;
	return true;
}

int siDet::GetNumChannels(bool nType) {
	if (nType) return enRawN.size();
	return enRawP.size();
}


ClassImp(siDet)
