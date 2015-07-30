#ifndef GODDESSCONFIG_H
#define GODDESSCONFIG_H

#include <map>
#include <string>
#include "orrubaDet.h"
#include "TClonesArray.h"
#include "superX3.h"
#include "BB10.h"
#include "QQQ5.h"
#include "IonChamber.h"

#include "SolidVector.h"

class GoddessConfig {
	private:
		///Read the configuration file.
		void ReadConfig(std::string filename);
		///Read the position configuration.
		void ReadPosition(std::string filename);

		///Map of channels to silicon detector.
		std::map<short, Detector*> chMap;

		///Vector of registered superX3 detectors.
		std::vector<superX3*> superX3s;
		///Vector of registered BB10 detectors.
		std::vector<BB10*> bb10s;
		///Vector of registered QQQ5 detectors.
		std::vector<QQQ5*> qqq5s;
		///Pointer to the registered ion chamber.
		IonChamber *ionChamber;
		
		///Check if this type of detector is insertable in the channel map at the specified postion.
		bool IsInsertable(int daqCh, std::string type);

	public:
		///Default constructor.
		GoddessConfig(std::string positionFile, std::string configFile);
		///Default destructor.
		~GoddessConfig();

		///Gets the detector and contact number for the specified digitizer channel.
		short GetMappedCh(short digitizerCh);

		///Parse and id string to get position details.
		bool ParseID(std::string id, short& sector, short& depth, bool& upStream);

		///Compute vector and rotation angle to detector.
		SolidVector GetPosVector(const std::string type, const short sector, const short depth, const bool upStream); 
		

};

#endif
