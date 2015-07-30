#include "orrubaDet.h"

orrubaDet::orrubaDet(std::string serialNum, unsigned short sector, 
	unsigned short depth, bool upStream, SolidVector position) :
	serialNum(serialNum), sector(sector), depth(depth), upStream(upStream),
	detPos(position)
{

}

orrubaDet::~orrubaDet() {

}

ClassImp(orrubaDet)

