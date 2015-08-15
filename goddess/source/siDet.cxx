#include "siDet.h"
#include <cmath>
#include <iostream>

siDet::siDet():
	numPtype(0), numNtype(0)
{
	Clear();
}
siDet::~siDet() {

}


void siDet::Clear() {
	enRawP.clear();
	enRawN.clear();
	enCalP.clear();
	enCalN.clear();
}

void siDet::SetNumContacts(int pType, int nType/*=0*/) {
	numPtype = pType;
	numNtype = nType;

	parEnCalP.resize(pType);
	parEnCalN.resize(nType);
}

/**
 * \param[in] contact The contact number to validate.
 * \param[in] nType Whether the contact is nType.
 *	\return True is contact has been declared.
 */
bool siDet::ValidContact(unsigned int contact, bool nType/*=false*/) {
	size_t size = numPtype;
	if (nType) size = numNtype;
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
void siDet::SetRawValue(unsigned int channel, unsigned int rawValue) {
	if (channel < numPtype)
		SetRawValue(channel,false,rawValue);
	else if (channel < numPtype + numNtype) 
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
void siDet::SetRawValue(unsigned int contact, bool nType, unsigned int rawValue) {
	if (!ValidContact(contact, nType)) return;

	//Get pointer to the raw an calibrated storage location.
	float *enRaw = 0, *enCal = 0;
	unsigned int threshold = 0;
	std::vector<float> *parEnCal;
	if (nType) {
		enRaw = &(enRawN[contact]);
		if (contact < threshN.size()) threshold = threshN.at(contact);
		if (rawValue > threshold) enCal = &(enCalN[contact]);
		parEnCal = &(parEnCalN[contact]);
	}
	else {
		enRaw = &(enRawP[contact]);
		if (contact < threshP.size()) threshold = threshP.at(contact);
		if (rawValue > threshold) enCal = &(enCalP[contact]);
		parEnCal = &(parEnCalP[contact]);
	}

	//Assign raw value and compute calibrated value.
	*enRaw = rawValue;
	if (rawValue > threshold) {
		*enCal = 0;
		for (size_t power = 0; power < parEnCal->size(); power++)
			*enCal += parEnCal->at(power) * pow(rawValue,power);
	}
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

bool siDet::SetThresholds(std::vector<int> thresholds, bool contactType/*=siDet::pType*/) {
	if (!ValidContact(thresholds.size()-1,contactType)) {
		std::cerr << "ERROR: Vector specified for thresholds was larger than number of contacts (" << thresholds.size() << ">" << GetNumChannels(contactType) << ")!\n";
		return false;
	}
	if (contactType == siDet::nType) threshN = thresholds;
	threshP = thresholds;
	return true;
}

int siDet::GetNumChannels(bool nType) {
	if (nType) return numNtype;
	return numPtype;
}

siDet::ValueMap siDet::GetRawEn(bool nType) {
	if (nType) return enRawN;
	return enRawP;
}

siDet::ValueMap siDet::GetCalEn(bool nType) {
	if (nType) return enCalN;
	return enCalP;
}

bool siDet::ContactHit(int contact, bool nType) {
	ValueMap *map = &enRawP;
	if (nType) map = &enRawN;
	
	if (map->find(contact) == map->end()) return false;
	return true;
}


ClassImp(siDet)
