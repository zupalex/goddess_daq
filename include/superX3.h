#ifndef SUPERX3_H
#define SUPERX3_H

#include "siDet.h"
#include "TVector3.h"

///Class designed to compute hit information on a Micron Super X3 detector.
/**The Super X3 (http://www.micronsemiconductor.co.uk/pdf/cat.pdf) is a double sided
 * silicon strip detector with four strips on each side. The p-type side is position 
 * sensitive via a restive strip with contacts on each side of the strip. The 
 * detector has an active area of 40.3 x 75.0 mm^2.
 */
class superX3 : public siDet {
	private:
		///The detector position.
		TVector3 detPos; //!
		///The detector rotation around the z-axis.
		float detRotation;

		///Vector pointing to mid point of p type strip edge in mm.
		TVector3 pStripEdgePos[5]; //!
		///Vector pointing to mid point of n type strip edge in mm.
		TVector3 nStripEdgePos[5]; //!

		///The bin edges along the p-type strips in mm.	
		Float_t binsP[5]; //!
		///The bin edges along the n-type strips in mm.	
		Float_t binsN[5]; //!
		///The bin edges along the z-axis in mm.	
		Float_t binsZ[5]; //!
		///The bin edges along the phi axis in degrees.	
		Float_t binsAzimuthal[5]; //!
		///The bin edges along the phi axis in degrees.	
		Float_t binsPolar[5]; //!

		///Calibrated energy of p type strips.
		float enCalPstrip[4];
		///Calibrated energy of the p type side.
		float enPtype;
		///Calibrated energy of the n type side.
		float enNtype;

		///Computed raw position of event.
		float stripPosRaw[4];
		///Computed calibrated position of event.
		float stripPosCal[4];

		///The number of contacts fired on a strip.
		int stripContactMult[4];
		///The number of p type strips that fired.
		int multPstrip;

		///The constructed position of the event.
		TVector3 eventPos; 

		///Vector containing position calibration polynomial parameters.
		std::vector<float> parPosCal[4]; //!
		///Vector containing strip energy calibration polynomial parameters.
		std::vector<float> parStripEnCal[4]; //!

		///Construct array of bin edges.
		void ConstructBins();

	public:
		///Default constructor.
		superX3(std::string name, TVector3 detPos, Float_t detRotation);
		///Default destructor.
		virtual ~superX3();

		///Clear the stored values in this detector.
		void Clear();

		///Calculate the position on the resistive strip detectors.
		void UpdatePosition(int strip);

		///Assign a vector of float to the calibration parameters of the specified strip.
		void SetStripPosCalibPars(int strip,std::vector<float>);
		///Assign a vector of float to the calibration parameters of the specified strip.
		void SetStripEnCalibPars(int strip,std::vector<float>);

		///Return the strip in which the specified contact is attached.
		int GetStrip(int contact);

		///Return the number of bins.
		int GetNumBins() {return 4;}
		///Return the bins boundaries of the dimension along the p type strips in mm.
		float* GetPtypeBins() {return binsP;};
		///Return the bins boundaries of the dimension along the n type strips in mm.
		float* GetNtypeBins() {return binsN;};
		///Return the bins boundaries along the z (beam) direction in mm.
		float* GetZbins() {return binsZ;};
		///Return the bins along the azimuthal direction in degrees.
		float* GetAzimuthalBins() {return binsAzimuthal;};
		///Return the bins along the polar direction in degrees.
		float* GetPolarBins() {return binsPolar;};
		
		///Returns true if strip number is valid.
		bool ValidStrip(int strip);

		///Return the computed event position.
		TVector3 GetEventPosition() {return eventPos;};
		///Return the energy from the p type side.
		float GetPtypeEnergy() {return enPtype;};
		///Return the energy from the n type side.
		float GetNtypeEnergy() {return enNtype;};

		///Set the raw energy of the contact and compute the calibrated value.
		virtual void SetEnergy(unsigned int contact, int rawValue, bool nType = false);

		/// \cond This is just for ROOT and doesn't need to be documented
		ClassDef(superX3,1);
		/// \endcond
};


#endif
