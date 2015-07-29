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

class GoddessConfig {
	private:
		///Read the configuration file.
		void ReadConfig(std::string filename);
		///Read the position configuration.
		void ReadPosition(std::string filename);

		///Map of channels to silicon detector.
		std::map<short, orrubaDet*> chMap;

		std::vector<superX3*> superX3s;
		std::vector<BB10*> bb10s;
		std::vector<QQQ5*> qqq5s;
		IonChamber *ionChamber;
		
		///Check if this type of detector is insertable in the channel map at the specified postion.
		bool IsInsertable(int daqCh, std::string type);

	public:
		///Default constructor.
		GoddessConfig(std::string positionFile, std::string configFile);
		///Default destructor.
		~GoddessConfig();

		void GetMappedCh(short digitizerCh, orrubaDet* det, short &contactNum);

		///Parse and id string to get position details.
		bool ParseID(std::string id, short& sector, short& depth, bool& upStream);

		///Compute vector and rotation angle to detector.
		TVector3 GetPosVector(std::string type, short sector, short depth, bool upStream, float &angle); 
		

};

#endif
