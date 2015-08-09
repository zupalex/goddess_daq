#ifndef ORRUBADET_H
#define ORRUBADET_H

#include "siDet.h"
#include "SolidVector.h"

///A class to handle basic ORRUBA silicon detectors.
/**This class builds on a basic siDet by adding detector position information and 
 * serial number.
 *
 * \author Karl Smith
 * \date July 2015
 */
class orrubaDet : public siDet {
	private:

	protected:
		///Serial number assigned to the detector.
		std::string serialNum;

		///Which region of the barrel the detector is located.
		unsigned short sector;
		///The number of detectors between this one and the target.
		unsigned short depth;
		///Whether the detector is upstream or downstream of the target.
		bool upStream;
		
		///The string form of the position ID.
		std::string posID;
		
		///The detector position defining the center of the active silicon area.
		SolidVector detPos; //!

		///Determines the position ID string.
		void SetPosID();
	
	public:
		///Default constructor.
		orrubaDet();
		orrubaDet(std::string serialNum, unsigned short sector, unsigned short depth, 
			bool upStream, SolidVector position);
		///Default destructor.
		virtual ~orrubaDet();

		///Get the detector's serial number.
		std::string GetSerialNum() {return serialNum;}

		std::string GetPosID() {return posID;};

		void SetDetector(std::string serialNum, unsigned short sector, unsigned short depth, bool upStream, SolidVector position);
		virtual void ConstructBins() = 0;

	/// \cond This is just for ROOT and doesn't need to be documented
	ClassDef(orrubaDet,1);
	/// \endcond
};


#endif

