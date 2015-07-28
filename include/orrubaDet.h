#ifndef ORRUBADET_H
#define ORRUBADET_H

#include "siDet.h"
#include "TVector3.h"

///A class to handle basic ORRUBA silicon detectors.
/**This class builds on a basic siDet by adding detector position information and 
 * serial number.
 *
 * \author Karl Smith
 * \date July 2015
 */
class orrubaDet : public siDet {
	private:
		///Serial number assigned to the detector.
		const std::string serialNum;

	protected:
		///Which region of the barrel the detector is located.
		const unsigned short sector;
		///The number of detectors between this one and the target.
		const unsigned short depth;
		///Whether the detector is upstream or downstream of the target.
		const bool upStream;
		
		
		///The detector position.
		const TVector3 detPos; //!
		///The detector rotation around the z-axis.
		const float detRotation; //!
	
	public:
		///Default constructor.
		orrubaDet(std::string serialNum, unsigned short sector, unsigned short depth, 
			bool upStream, TVector3 position, float rotationAngle);
		///Default destructor.
		virtual ~orrubaDet();

		///Get the detector's serial number.
		std::string GetSerialNum() {return serialNum;}

	/// \cond This is just for ROOT and doesn't need to be documented
	ClassDef(orrubaDet,1);
	/// \endcond
};


#endif

