#include "GoddessConfig.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "TClass.h"
#include "TMath.h"

#include "superX3.h"

/**
 *
 * \param[in] positionFile The configuration file specfying the central position 
 *  of each sector.
 * \param[in] configFile The configuration file specifying which detectors are where
 *  and their calibration parameters.
 *
 */
GoddessConfig::GoddessConfig(std::string positionFile, std::string configFile) {
	ReadPosition(positionFile);
	ReadConfig(configFile);
}	

GoddessConfig::~GoddessConfig() {
	for(auto iterator = chMap.begin(); iterator != chMap.end(); iterator++) {	
		delete (*iterator).second;
	}
}

/**The GODDESS configuration file specifies the silicon and ion/scint detector 
 * configuration. 
 *
 * For the silicon detectors the supported types are: superX3, BB10, and QQQ5. A 
 * typical configuration would read as follows:
 * superX3 1234-56 U0dE
 *  enCal p 0 1 2 3 
 *  enCal p 1 1 2 3 
 *  posCal resStrip 0 1 2 3 
 *
 * This configuration example defines a superX3 detector with serial number 1234-56
 * in the upstream position of sector 0 as a energy loss detector. Following this 
 * are a list of energy or position calibrations. Each calibration specifies the 
 * subtype of the channel, either p, n or resStrip for a superX3. This is followed 
 * by the subtype channel number and then the polynomial parameters. An arbitrary 
 * number of parameters can be listed in increasing order. 
 * \note The position calibration is only available for resStrip of superX3 
 *  detectors.
 *
 * Also supported, is an ion detector with a built-in scintillator. An example of a
 * typical configuration follows:
 * ion 5 4 2 3 
 *  enCal anode 0 0 1 2
 *  posCal scint 0 4 5 6
 *  timeCal scint 0 2 3 4
 *
 * This configuration defines an ion chamber with 5 anodes and 4 scintillator PMTs.
 * The energy loss signal is composed of the first two anodes and the residual 
 * energy is composed of the following 3 channels. There may be more anodes read out
 * then used in computing these two values. Following this is a list of calibration
 * parameters for energy, position and time. This calibration lines follow the same 
 * format as the silicons with subtypes or anode and scint.
 * \note The position and time calibration are only supported for the scint subtype.
 *
 *
 * \param[in] filename The configuration file to be read.
 */
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
		std::string type, serialNum = "", id;
		int daqCh;
		orrubaDet *det = 0;

		if (!(lineStream >> type)) {
			error = true;
			break;
		}
		//Report the one we are registering.
		std::cout << "Registering " << type << " detector: ";

		if (type == "ion") {
			int numAnode, numScint, numDE, numEres;
			if (!(lineStream >> numAnode >> numScint >> numDE >> numEres)) {
				error = true;
				break;
			}
			std::cout << " Anodes: " << numAnode << ", Scint PMTs: " << numScint;
			std::cout << " dE: " << numDE << " anodes, Eres " << numEres << "\n";

			ionChamber = new IonChamber(numAnode, numScint, numDE, numEres);
		}
		else {
			if (!(lineStream >> serialNum >> id >> daqCh)) {
				error = true;
				break;
			}

			std::cout << serialNum;
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

			SolidVector pos = GetPosVector(type, sector, depth, upStream);

			std::cout << ", Position: " << pos.x() << ", " << pos.y() << ", " << pos.z();
			std::cout << ", Angle: " << pos.RotZ() << "\n";

			//Construct object for the specified type.
			if (type == "superX3") {	
				superX3s.push_back(new superX3(serialNum,sector,depth,upStream,pos));
				det = superX3s.back();
			}	
			else if (type == "BB10") {	
				bb10s.push_back(new BB10(serialNum,sector,depth,upStream,pos));
				det = bb10s.back();
			}	
			else if (type == "QQQ5") {	
				qqq5s.push_back(new QQQ5(serialNum,sector,depth,upStream,pos));
				det = qqq5s.back();
			}
			else {
				std::cerr << "ERROR: Unknown detector type: " << type << "!\n";
				std::cerr << " Valid options: ion, superX3, BB10, QQQ5.\n";
				error = true;
				break;
			}
			if (IsInsertable(daqCh,type)) chMap[daqCh] = superX3s.back();
			else 
				std::cerr << "ERROR: Detector " << serialNum << " will not be unpacked!\n";
		}

		//Read calibration information.
		while (std::getline(mapFile, line).good()) {
			std::istringstream lineStream(line);
			std::string calType, subType;
			int detChannel;
			if (!(lineStream >> calType >> subType >> detChannel)) {
				mapFile.seekg(-line.length()-1,std::ios_base::cur);
				break;
			}

			//Determine the calibration type.
			bool posCal = false;
			bool timeCal = false;
			if (calType == "posCal") {
				if (!(type == "superX3" && subType == "resStrip") && !(type == "ion" && subType == "scint")) {
					std::cout << " WARNING: Ignoring position calibration specified for " << subType << "-type part of " << type << " detector " << serialNum << "\n";
					continue;
				}
				posCal = true;
			}
			else if (calType == "timeCal") {
				if (!(type == "ion" && subType == "scint")) {
					std::cout << " WARNING: Ignoring time calibration specified for " << subType << "-type part of " << type << " detector " << serialNum << "\n";
					continue;
				}
				timeCal = true;
			}
			else if (calType != "enCal") { 
				mapFile.seekg(-line.length()-1,std::ios_base::cur);
				break;
			}

			//Check that the subtype is valid
			bool nType = false;
			if (type == "ion") {
				if (subType != "scint" && subType != "anode") {
					std::cerr << " ERROR: Unknown subtype '" << subType << "'!\n";
					std::cerr << "  Valid options: anode, strip.\n";
					continue;
				}
			}
			else {
				if (subType == "n") nType = true;
				else if (subType != "p" && subType != "resStrip") {
					std::cerr << " ERROR: Unknown subtype '" << subType << "'!\n";
					std::cerr << "  Valid options: p, n, resStrip.\n";
					continue;
				}
			}

			//Get the calibration parameters
			float param;
			std::vector< float > calParams;
			while (lineStream >> param) calParams.push_back(param);

			//Print a line about the calibration function.
			if (posCal) std::cout << " Pos. ";
			else std::cout << " En. ";
			std::cout << subType << "-type channel " << detChannel << ", ";
			std::cout << calParams.size()-1 << " order calib.: "; 

			for (auto itr=calParams.begin(); itr != calParams.end(); ++itr) {
				if (itr != calParams.begin()) std::cout << ", ";
				std::cout << *itr;
			}
			std::cout << "\n";

			if (calParams.empty()) std::cerr << " WARNING: No calibration parameters specified for " << subType << "-type channel " << detChannel << ".\n";

			///Assign the parameters to their respective container.
			if (type == "ion") {	
				if (subType == "scint") {
					if (posCal) 
						std::cout << " WARNING: No support for scint position calibration!\n";	
					else if (timeCal) 
						ionChamber->SetScintTimeCalPars(detChannel, calParams);	
					else 
						ionChamber->SetScintEnCalPars(detChannel, calParams);	
				}
				else {
					ionChamber->SetAnodeEnCalPars(detChannel, calParams);	
				}
			}
			else {
				if (type == "superX3" && subType == "resStrip") {
					if (posCal) 
						superX3s.back()->SetStripPosCalibPars(detChannel, calParams);
					else superX3s.back()->SetStripEnCalibPars(detChannel, calParams);
				}
				else det->SetEnergyCalib(calParams, detChannel, nType);
			}

		}
	}

	//If we aborted for some reason and the file is good we must of had a parsing issue.
	if (mapFile.good()) {
		std::cerr << "ERROR: Unable to read configuration file! Failed on the following line:\n";
		std::cerr << line << "\n";
	}
}

void GoddessConfig::ReadPosition(std::string filename) {

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
SolidVector GoddessConfig::GetPosVector(std::string type, short sector, short depth, bool upStream) {
	static float barrelRadius = 3.750 * 25.4; //mm
	static float halfBarrelLength = (4.375 - 0.7) * 25.4; //mm
	SolidVector pos(0,0,0);

	//Compute position for barrel detectors.
	if (type == "superX3" || type == "BB10") {
		//The barrel is divided into twelve sectors with the zero sector in the 
		// positive vertical (y-axis) direction. Therefore the 0 sector is at 90 
		// degrees and we subtract for each additional sector and then offset by
		// two pi if less than zero.
		float azimuthal = TMath::PiOver2() - sector / 12. * TMath::TwoPi();
		if (azimuthal < 0) azimuthal += TMath::TwoPi();
		std::cout << azimuthal;
		//The rotation angle is also dependent on the sector with a rotation of zero 
		// in the zero sector.
		float rotZ = azimuthal + TMath::PiOver2();
		if (rotZ > TMath::TwoPi()) rotZ -= TMath::TwoPi();
		//The z position is oriented along the beam line and depends only on if the 
		// detector is upstream or downstream of the target.
		float z;
		if (upStream) z = halfBarrelLength / 2;
		else z = -halfBarrelLength / 2;
		//Set the computed x,y,z positions.
		pos.SetXYZ(barrelRadius * cos(azimuthal), barrelRadius * sin(azimuthal), z);
		pos.SetRotationZ(rotZ);

	}
	else if (type == "QQQ5") {
		float rotZ = -(3 * TMath::PiOver4()) + sector / 4. * TMath::TwoPi();
		pos.SetXYZ(0,0,halfBarrelLength);
		pos.SetRotationZ(rotZ);

	}

	return pos;
}
