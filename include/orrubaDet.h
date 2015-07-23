#ifndef ORRUBADET_H
#define ORRUBADET_H

#include "siDet.h"
#include "TVector3.h"

class orrubaDet : public siDet {
	private:
		///Serial number assigned to the detector.
		std::string serialNum;


	protected:
		///Which region of the barrel the detector is located.
		const unsigned short sector;
		///The number of detectors between this one and the target.
		const unsigned short depth;
		
		///The detector position.
		const TVector3 detPos; //!
		///The detector rotation around the z-axis.
		const float detRotation;
		
	public:
		orrubaDet(std::string serialNum, unsigned short sector, unsigned short depth, 
			TVector3 position, float rotationAngle);
};


#endif

