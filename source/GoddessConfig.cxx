#include "GoddessConfig.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "TClass.h"
#include "TMath.h"

#include "superX3.h"

GoddessConfig::GoddessConfig(std::string filename) {
	ReadConfig(filename);
}	

GoddessConfig::~GoddessConfig() {
	for(auto iterator = chMap.begin(); iterator != chMap.end(); iterator++) {	
		delete (*iterator).second;
	}
}

void GoddessConfig::ReadConfig(std::string filename) {
	std::ifstream mapFile(filename);	

	if (!mapFile.good()) {
		std::cerr << "ERROR: Unable to read configuration file: " << filename << "\n";
		return;
	}

	std::string line;
	bool error = false;
	//Loop over every line in the config file
	while (!error && std::getline(mapFile, line).good()) {
		//Convert string to stream
		std::istringstream lineStream(line);
			
		//Read the first line for a detector block
		// This line specifies the following in order:
		// 	type serialNum x y z angle daqCh
		std::string type, serialNum, id;
		int daqCh;

		if (!(lineStream >> type >> serialNum >> id >> daqCh)) {
			error = true;
			break;
		}

		//Report the we successfully registered it
		std::cout << "Registering " << type << " detector: " << serialNum;
		std::cout << ", LocID: " << id;
		std::cout << ", DAQ: " << daqCh << "+\n";

		short sector, depth;
		bool upStream;
		if (!ParseID(id, sector, depth, upStream)) {
			std::cerr << "ERROR: Unable to parse position id: " << id << " of detector " << serialNum << "!\n";
			break;
		}
		std::cout << " Upstream: ";
		upStream ? std::cout << "U" : std::cout << "D";
		std::cout << ", Sector: " << sector << ", Depth: " << depth;

		float angle;
		TVector3 pos = GetPosVector(type, sector, depth, upStream, angle);

		std::cout << ", Position: " << pos.x() << ", " << pos.y() << ", " << pos.z();
		std::cout << ", Angle: " << angle << "\n";

		//Construct object for the specified type.
		if (type == "superX3") {	
			superX3s.push_back(new superX3(serialNum,sector,depth,upStream,pos,angle));

			if (IsInsertable(daqCh,type)) chMap[daqCh] = superX3s.back();
			else 
				std::cerr << "ERROR: Detector " << serialNum << " will not be unpacked!\n";
		}	
		else if (type == "BB10") {	
			bb10s.push_back(new BB10(serialNum,sector,depth,upStream,pos,angle));

			if (IsInsertable(daqCh,type)) chMap[daqCh] = bb10s.back();
			else 
				std::cerr << "ERROR: Detector " << serialNum << " will not be unpacked!\n";
		}	
		else if (type == "QQQ5") {	
			qqq5s.push_back(new QQQ5(serialNum,sector,depth,upStream,pos,angle));

			if (IsInsertable(daqCh,type)) chMap[daqCh] = qqq5s.back();
			else 
				std::cerr << "ERROR: Detector " << serialNum << " will not be unpacked!\n";
		}
		else {
			std::cerr << "ERROR: Unknown detector type: " << type << "!\n";
			error = true;
			break;
		}

		//Read calibration information.
		while (std::getline(mapFile, line).good()) {
			std::istringstream lineStream(line);
			std::string calType;
			if (!(lineStream >> calType)) break;
			bool posCal = false;
			if (calType == "posCal") posCal = true;
			else if (calType != "enCal") { 
				mapFile.seekg(-line.length()-1,std::ios_base::cur);
				break;
			}

			int contactNum;
			std::string contactType;
			if (!(lineStream >> contactType >> contactNum)) {
				std::cerr << "ERROR: Unable to read calibration information!\n";
				std::cerr << " " << line << "\n";
				continue;
			}

			bool nType = false;
			if (contactType == "n") nType = true;
			else if (contactType != "p") {
				std::cerr << "ERROR: Unknown contact type '" << contactType << "'!\n";
				continue;
			}

			float param;
			std::vector< float > calParams;
			while (lineStream >> param) calParams.push_back(param);

			//Print a line about the calibration function.
			if (posCal) std::cout << " Pos. ";
			else std::cout << " En. ";
			std::cout << contactType << "-type contact " << contactNum << ", ";
			std::cout << calParams.size()-1 << " order calib.: "; 

			for (auto itr=calParams.begin(); itr != calParams.end(); ++itr) {
				if (itr != calParams.begin()) std::cout << ", ";
				std::cout << *itr;
			}
			std::cout << "\n";

			if (calParams.empty()) std::cerr << " WARNING: No calibration parameters specified for " << contactType << "-type contact " << contactNum << ".\n";

		}
	}

	//If we aborted for some reason and the file is good we must of had a parsing issue.
	if (mapFile.good()) {
		std::cerr << "ERROR: Unable to read configuration file! Failed on the following line:\n";
		std::cerr << line << "\n";
	}
}

/**We check that at the insertion point the previous entry has sufficient 
 * channels prior to the insertion point of this detector. As well as, check that 
 * the inserted detector will have sufficienct channel space before the following 
 * detector.
 *
 * \param[in] daqCh The DAQ channel at which the detector would be inserted into the
 *  map.
 * \param[in] type The type of detector to insert into the map.
 * \return True if there is no overlapping channels already loaded in the map.
 */
bool GoddessConfig::IsInsertable(int daqCh, std::string type) {
	bool insertable = true;

	if (!chMap.empty()) {
		//Check the upper bound
		auto mapItr = chMap.upper_bound(daqCh);
		if (mapItr != chMap.end()) {
			//Get the number of channels needed for each detector.
			int numCh = 1;
			if (type == "superX3") numCh = 12;
			else if (type == "BB10") numCh = 8;
			else if (type == "QQQ5") numCh = 36;

			if (daqCh + numCh > mapItr->first) {
				std::cerr << "ERROR: Specified starting DAQ channel conflict with detector " << mapItr->second->GetSerialNum() << "\n";
				insertable = false;
			}
		}

		//Check the lower bound
		if (mapItr != chMap.begin()) {
			--mapItr;
			int numCh = 1;
			std::string checkType = mapItr->second->IsA()->GetName();
			if (checkType == "superX3") numCh = 12;
			else if (checkType == "BB10") numCh = 8;
			else if (checkType == "QQQ5") numCh = 36;

			std::cout << mapItr->first << " " << numCh << " " << daqCh << "\n";
			if (mapItr->first + numCh > daqCh) {
				std::cerr << "ERROR: Specified starting DAQ channel conflict with detector " << mapItr->second->GetSerialNum() << "\n";
				insertable = false;
			}
		}
	}

	return insertable;
}

void GoddessConfig::GetMappedCh(short daqCh, orrubaDet *det, short &contactNum) {

}

/**Parse the id string describing the detector position int he barrel. The id is 
 * composed of the following:
 * 1. U/D: Indicating whether the detector is up/down stream of the target.
 * 2. A-D or 0-11: Indicating which sector of the barrel the detector occupies, 
 * 	letters and numbers denote end cap and barrel sectors respectively.
 * 3. dE, E1, or E2: Indicating the depthe of the detector in the stack.
 *
 *	\param[in] id The id string to be parsed.
 *	\param[out] sector The sector occupied by the detector.
 *	\param[out] depth The depth of the detector.
 *	\param[out] upStream Flag indicating if the detector is upstream of the target.
 *	\return True if successfully parses the id string.
 *
 */
bool GoddessConfig::ParseID(std::string id, short& sector, short& depth, bool& upStream) {
	std::string subStr = id.substr(0,1);
	if (subStr == "U") upStream = true;
	else if (subStr == "D") upStream = false;
	else {
		std::cerr << "ERROR: Unexpected character '" << subStr << "' in up/down stream position of id!\n";
		return false;
	}

	bool barrel = false;
	subStr = id.substr(1,id.length() - 3);
	if (subStr == "A") sector = 0;
	else if (subStr == "B") sector = 1;
	else if (subStr == "C") sector = 2;
	else if (subStr == "D") sector = 3;
	else if (subStr.find_first_of("0123456789") != std::string::npos) {
		sector = std::stoi(subStr);
		if (sector > 11) {
			std::cerr << "ERROR: Unexpected sector '" << subStr << "' in id!\n";
			return false;
		}
		barrel = true;
	}
	else {
		std::cerr << "ERROR: Unexpected character '" << subStr << "' in sector position of id!\n";
		return false;
	}

	//The depth index position depends on whether we read out a barrel or end cap.
	subStr = id.substr(id.length() - 2, 2);
	if (subStr == "dE") depth = 0;
	else if (subStr == "E1") depth = 1;
	else if (subStr == "E2") depth = 2;
	else {
		std::cerr << "ERROR: Unexpected depth '" << subStr << "' in id!\n";
		return false;
	}

	return true;
}

/**Based on the detector type, sector, depth and whether the detector is up stream a
 * vector position is computed and the rotation angle of the detector is determined.
 * 
 * \param[in] type The detector type.
 * \param[in] sector The sector occupied by the detector.
 * \param[in] depth The depth of the detector in the stack.
 * \param[in] upStream Flag indicating if the detector is up stream of the target.
 * \param[out] angle The computed rotation angle.
 * \return A TVector3 pointing to the detector corner. 
 */
TVector3 GoddessConfig::GetPosVector(std::string type, short sector, short depth, bool upStream, float &angle) {
	static float barrelRadius = 3.750 * 25.4; //mm
	TVector3 pos(0,0,0);

	//Compute position for barrel detectors.
	if (type == "superX3" || type == "BB10") {
		//The barrel is divided into twelve sectors with the zero sector in the 
		// positive vertical (y-axis) direction. Therefore the 0 sector is at 90 
		// degrees and we subtract for each additional sector and then offset by
		// two pi if less than zero.
		float azimuthal = - TMath::PiOver2() + sector / 12. * TMath::TwoPi();
		if (azimuthal < 0) azimuthal += TMath::TwoPi();
		//The rotation angle is also dependent on the sector with a rotation of zero 
		// in the zero sector.
		angle = (1 - sector / 12.) * TMath::TwoPi();
		//The z position is oriented along the beam line and depends only on if the 
		// detector is upstream or downstream of the target.
		float z;
		if (upStream) z = 0;
		else z = 0;
		//Set the computed x,y,z positions.
		pos.SetXYZ(barrelRadius * sin(azimuthal), barrelRadius * cos(azimuthal), z);

	}
	else if (type == "QQQ5") {
		angle = -(3 * TMath::PiOver4()) + sector / 4. * TMath::TwoPi();
		pos.SetXYZ(0,0,(4.375-0.7)*25.4);
		
	}

	return pos;
}
