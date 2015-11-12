#ifndef BB10_H
#define BB10_H

#include "orrubaDet.h"
#include "TVector3.h"

///Class designed to compute hit information on a Micron BB10 detector.
/**The BB10 is a single sided strip, rectangular, detector. It has eight strips.
 *
 * \author Karl Smith
 * \date July 2015
 */

class BB10 : public orrubaDet {
	private:
		///Vector pointing to mid point of p type strip edge in mm.
		TVector3 pStripEdgePos[9]; //!

		///The bin edges along the p-type strips in mm.	
		Float_t binsP[9]; //!
		///The bin edges along the phi axis in radians.	
		Float_t binsAzimuthal[9]; //!
		///The bin centers along the p-type strips in mm.	
		Float_t binsPCenter[8]; //!
		///The bin centers along the phi axis in radians.	
		Float_t binsAzimuthalCenter[8]; //!

		///Pair strip# and energy of the p type side.
		std::pair<int,float> enPtype;

		///The constructed position of the event.
		TVector3 eventPos; 

		///Construct array of bin edges.
		void ConstructBins();

	public:
		///Default constructor.
		BB10();
		BB10(std::string serialNum, unsigned short sector, unsigned short depth, 
			bool upStream, SolidVector detPos);
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
		///Return the bins centers of the dimension along the p type strips.
		float* GetPtypeBinCenters() {return binsPCenter;};
		///Return the bin centers along the azimuthal direction.
		float* GetAzimuthalBinCenters() {return binsAzimuthalCenter;};

		///Return the computed event position.
		TVector3 GetEventPosition() {return eventPos;};
		///Return a pair of (strip#,energy) from the p type side.
		std::pair<int,float> GetPtypeEnergy() {return enPtype;};
		///Returns 0, Needed to be added to be used globally in orrubaDet
		std::pair<int,float> GetNtypeEnergy() {return std::make_pair(0,0.0);};
		///Return the total energy deposited in the detector.
		float GetEnergy() {return enPtype.second;};
		
		///Set the raw energy of the contact and compute the calibrated value.
		virtual void SetRawValue(unsigned int contact, bool nType, int rawValue);


	/// \cond This is just for ROOT and doesn't need to be documented
	ClassDef(BB10,1)
	/// \endcond
};

#endif
