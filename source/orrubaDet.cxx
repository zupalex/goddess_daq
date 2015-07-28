#include "orrubaDet.h"

orrubaDet::orrubaDet(std::string serialNum, unsigned short sector, 
	unsigned short depth, bool upStream, TVector3 position, float rotationAngle) :
	serialNum(serialNum), sector(sector), depth(depth), upStream(upStream),
	detPos(position), detRotation(rotationAngle)
{

}

orrubaDet::~orrubaDet() {

}

ClassImp(orrubaDet)

