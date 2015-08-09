#ifndef SIDET_H
#define SIDET_H

#include <vector>
#include "Detector.h"

///A class to handle basic silicon detectors.
/**
 * \author Karl Smith
 * \date July 2015
 */
class siDet : public Detector {
	private:
		///Vector of raw energies for p type contacts.
		std::vector<float> enRawP;
		///Vector of raw energies for n type contacts.
		std::vector<float> enRawN;
		///Vector of calibrated energies for p type contacts.
		std::vector<float> enCalP;
		///Vector of calibrated energies for n type contacts.
		std::vector<float> enCalN;

		///Multiplicity of p type contacts.
		unsigned int multP;
		///Multiplicity of n type contacts.
		unsigned int multN;

		///The calibration parameters for p type contacts
		std::vector<std::vector<float>> parEnCalP; //!
		///The calibration parameters for n type contacts
		std::vector<std::vector<float>> parEnCalN; //!

	public:
		///Default constructor.
		siDet();
		///Default destructor.
		virtual ~siDet();
		///Clear the stored detector values.
		void Clear();
		///Set the number of contacts on the silicon detector.
		void SetNumContacts(int pType, int nType = 0);

		///Check if specified contact has been declared.
		bool ValidContact(unsigned int contact, bool nType);
		
		///Set the raw energy of the channel.
		virtual void SetRawValue(unsigned int channel, int rawValue);
		///Set the raw energy of the contact and compute the calibrated value.
		virtual void SetRawValue(unsigned int contact, bool nType, int rawValue);

		///Get the calibrated energy of the contact specified.
		float GetCalEnergy(int contact, bool nType = false);
		///Return the number of fired contacts.
		int GetContactMult(bool nType = false);

		///Specify the polynomial calibration parameters of the specified contact.
		bool SetEnergyCalib(std::vector<float> par, int contact, bool nType = false);



	ClassDef(siDet,1)
	
};

#endif
