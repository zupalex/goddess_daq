#include "orrubaDet.h"

orrubaDet::orrubaDet() {

}
orrubaDet::orrubaDet(std::string serialNum, unsigned short sector, 
	unsigned short depth, bool upStream, SolidVector position) :
	serialNum(serialNum), sector(sector), depth(depth), upStream(upStream),
	detPos(position)
{

}

orrubaDet::~orrubaDet() {

}

std::string orrubaDet::GetPosID() {
	std::string id;
	if (upStream) id.append("U");
	else id.append("D");
	
	id.append(std::to_string(sector));

	if (depth == 0) id.append("dE");
	else if (depth == 1) id.append("E1");
	else if (depth == 2) id.append("E2");
	
	return id;
}

void orrubaDet::SetDetector(std::string serialNum_, unsigned short sector_, unsigned short depth_, bool upStream_, SolidVector position_) {
	serialNum = serialNum_;
	sector = sector_;
	depth = depth_;
	upStream = upStream_;
	detPos = position_;
}

ClassImp(orrubaDet)

