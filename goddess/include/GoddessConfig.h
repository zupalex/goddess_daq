#ifndef GODDESSCONFIG_H
#define GODDESSCONFIG_H

#include <map>
#include <string>
#include "orrubaDet.h"
#include "TClonesArray.h"
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

		///Pointer to array of registered superX3 detectors.
		TClonesArray *superX3s;
		///Pointer to array of registered BB10 detectors.
		TClonesArray *bb10s;
		///Pointer to array of registered QQQ5 detectors.
		TClonesArray *qqq5s;
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

		///Get a vector of all registered SuperX3s.
		TClonesArray* GetSuperX3s() {return superX3s;}
		///Get a vector of all registered BB10s.
		TClonesArray* GetBB10s() {return bb10s;}
		///Get a vector of all registered QQQ5s.
		TClonesArray* GetQQQ5s() {return qqq5s;}

};

#endif
