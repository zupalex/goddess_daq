#include "orrubaDet.h"

orrubaDet::orrubaDet() {

}
orrubaDet::orrubaDet(std::string serialNum, unsigned short sector, 
	unsigned short depth, bool upStream, SolidVector position) :
	serialNum(serialNum), sector(sector), depth(depth), upStream(upStream),
	detPos(position)
{
	SetPosID();

}

orrubaDet::~orrubaDet() {

}

void orrubaDet::SetPosID() {
	if (upStream) posID.append("U");
	else posID.append("D");
	
	posID.append(std::to_string((long long int)sector));

	posID.append("-");

	if (depth == 0) posID.append("dE");
	else if (depth == 1) posID.append("E1");
	else if (depth == 2) posID.append("E2");
}

void orrubaDet::SetDetector(std::string serialNum_, unsigned short sector_, unsigned short depth_, bool upStream_, SolidVector position_) {
	serialNum = serialNum_;
	sector = sector_;
	depth = depth_;
	upStream = upStream_;
	detPos = position_;

	ConstructBins();
	SetPosID();

}

ClassImp(orrubaDet)

