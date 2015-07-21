#ifndef SIDET_H
#define SIDET_H

#include "TObject.h"
#include <vector>

class siDet : public TObject {
	private:
		///Name assigned to the detector.
		std::string name;
		///Vector of raw energies for p type contacts.
		std::vector<float> enRawP;
		///Vector of raw energies for n type contacts.
		std::vector<float> enRawN;
		///Vector of calibrated energies for p type contacts.
		std::vector<float> enCalP;
		///Vector of calibrated energies for n type contacts.
		std::vector<float> enCalN;

		///Multiplicity of p type contacts.
		int multP;
		///Multiplicity of n type contacts.
		int multN;

		///The calibration parameters for p type contacts
		std::vector<std::vector<float>> parEnCalP; //!
		///The calibration parameters for n type contacts
		std::vector<std::vector<float>> parEnCalN; //!

	public:
		///Default constructor.
		siDet(std::string detName = "");
		///Default destructor.
		virtual ~siDet();
		///Clear the stored detector values.
		void Clear();
		///Set the number of contacts on the silicon detector.
		void SetNumContacts(int pType, int nType = 0);

		///Check if specified contact has been declared.
		bool ValidContact(unsigned int contact, bool nType = false);
		
		///Set the raw energy of the contact and compute the calibrated value.
		virtual void SetEnergy(int contact, int rawValue, bool nType = false);

		///Get the calibrated energy of the contact specified.
		float GetCalEnergy(int contact, bool nType = false);
		
		///Return the number of fired contacts.
		int GetContactMult(bool nType = false);


	ClassDef(siDet,1)
	
};

#endif
