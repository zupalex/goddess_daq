#ifndef BB10_H
#define BB10_H

#include "siDet.h"
#include "TVector3.h"

class BB10 : public siDet {
	private:
		///The detector position.
		TVector3 detPos; //!
		///The detector rotation around the z-axis.
		float detRotation;

		///Vector pointing to mid point of p type strip edge in mm.
		TVector3 pStripEdgePos[9]; //!

		///The bin edges along the p-type strips in mm.	
		Float_t binsP[9]; //!
		///The bin edges along the phi axis in radians.	
		Float_t binsAzimuthal[9]; //!

		///Energy of the p type side.
		Float_t enPtype;
	
		///Construct array of bin edges.
		void ConstructBins();

	public:
		///Default constructor.
		BB10(TVector3 pos, float rotationAngle);
		///Default destructor.
		virtual ~BB10() {};

		///Clear the stored values in this detector.
		void Clear();

		///Return the number of bins.
		int GetNumBins() {return 8;}
		///Return the bins boundaries of the dimension along the p type strips.
		float* GetPtypeBins() {return binsP;};
		///Return the bins along the azimuthal direction.
		float* GetAzimuthalBins() {return binsAzimuthal;};

		///Return the energy from the p type side.
		float GetPtypeEnergy() {return enPtype;};

		///Set the raw energy of the contact and compute the calibrated value.
		virtual void SetEnergy(unsigned int contact, int rawValue, bool nType = false);


	/// \cond This is just for ROOT and doesn't need to be documented
	ClassDef(BB10,1)
	/// \endcond
};

#endif
