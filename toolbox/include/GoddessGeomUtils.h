#ifndef __GODDESSGEOMUTILS__
#define __GODDESSGEOMUTILS__

#include "GoddessCalib.h"
#include "TSpectrum.h"

struct NewGeomPars
{
		bool overrideGeom;
		bool overrideReac;
		int computeEjectileELoss;
		int computeBeamELoss;

		void Init()
		{
			overrideGeom = false;
			overrideReac = false;
			computeBeamELoss = 1;
			computeEjectileELoss = 1;
		}
};

struct GeomAdjustData
{
		GoddessReacInfos* localReacInfo;

		std::pair<vector<double>, vector<double>> energyLossData;

		TVector3 qqq5sPos[2][4];
		TVector3 sX3sPos[2][12];

		vector<TVector3>* qqq5Offsets;
		vector<TVector3>* sx3Offsets;
		vector<TVector3>* targetOffsets;

		vector<string> sX3OffXStr, sX3OffYStr, sX3OffZStr, qqq5OffXStr, qqq5OffYStr, qqq5OffZStr, targetOffXStr, targetOffYStr, targetOffZStr;

		TVector3 beamDir;

		TVector3 targetLadderDir;

		bool isBarrel;
		bool isUpstream;
		float sector_strip;
		double initialEnergy;
		GoddessReacInfos* gri;
		TVector3* pos = new TVector3();

		unsigned int sector;
		int sectorID;

		unsigned int strip;
		int globStripID;

		vector<double>* gam_energy = new vector<double>();
		vector<int>* crystal_num = new vector<int>();
};

class GoddessGeomUtils: public GoddessAnalysis
{
	private:

	public:
		GoddessGeomUtils()
		{
			nGP.Init();

			bestGeom =
			{	TVector3 ( 0, 0, 0 ), TVector3 ( 0, 0, 0 ), TVector3 ( 0, 0, 0 )};
		}

		~GoddessGeomUtils()
		{}

		// ******************* Geometry Utilities ************************** //

		NewGeomPars nGP;
		GeomAdjustData gAD;

		SolidVector orrubaStripsPos[2][2][12][32];

		vector<TVector3> bestGeom;

		void SetNewGeomMode ( bool geomOR, bool reacOR, bool beamELoss, bool ejecELoss );
		void SetGeomOverride ( bool geomOR );
		void SetReacOverride ( bool reacOR );
		void SetComputeBeamELoss ( int beamELoss );
		void SetComputeEjecELoss ( int ejecELoss );

		map<string, TH2F*> hQval_NewGeom;
		map<string, TH2F*> hEx_NewGeom;

		map<string, TH2F*> hEpvsA_SX3_NewGeom;
		map<string, TH2F*> hEpvsA_QQQ5_NewGeom;

		map<string, TH2F*> hExvsA_SX3_NewGeom;

		map<string, TH2F*> hExvsGS_NewGeom;

		map<string, TH2F*> hQvalvsStrip_QQQ5_NewGeom;

		vector<float> lastQQQ5Offsets, lastSX3Offsets;
		
		TH2F* GammaGamma;

		// sector ID in this case are sector number + 4 if superX3 + 16 if upstream
		vector<int> ignoreSectorsList =
		{	0, 1, 2, 3, 4 + 0, 4 + 1, 4 + 2, 4 + 3, 4 + 6, 4 + 8, 4 + 9, 4 + 10, 4 + 11, 16 + 4 + 1, 16 + 4 + 2, 16 + 4 + 3, 16 + 4
			+ 4, 16 + 4 + 5, 16 + 4 + 6, 16 + 4 + 7, 16 + 4 + 8, 16 + 4 + 10};

		void SetIgnoreSectors(bool /*turn_off*/)
		{}

		void SetIgnoreSectors(bool turn_off, string ignoreStr);
		void SetIgnoreSectors(bool turn_off, char* ignoreStr);
		void SetIgnoreSectors(bool turn_off, const char* ignoreStr);
		template<typename T> void SetIgnoreSectors(bool turn_off, T sect);
		template<typename T, typename... R> void SetIgnoreSectors(bool turn_off, T sect, R... rest);

		void PrintOutStripsPositions ( );
		void FillStripsPositionsArray ( float qqq5OffX, float qqq5OffY, float QQQ5OffZ, float sX3OffX, float sX3OffY, float sX3OffZ );
		TVector3 GetFinalHitPosition ( int isUpstream_, int isBarrel_, int sector_, int strip_, float eNear_, float eFar_ );

		void WriteNewGeomGraphs ( string outFileName = "", string opts = "update" );

		void* RecalculateAngleAndQVal ( void* args );

		void GenerateGeomAdjustRootfile ( string filesname, string treename, long long int nEntries, string outfname );

		void GetQValWithNewGeometry ( string filename, string treeName, long long int nEntries, vector<TVector3> qqq5Offsets, vector<TVector3> sx3Offsets, vector<TVector3> targetOffsets,
				function<bool()> testfn = []() -> double
				{	return true;});

		void GetQValWithNewGeometry ( string filename, string treeName, long long int nEntries,
				int qqq5OffX, int qqq5OffY, int qqq5OffZ,
				int sX3OffX, int sX3OffY, int sX3OffZ,
				int targetOffX, int targetOffY, int targetOffZ,
				function<bool()> testfn = []() -> double
				{	return true;});

		void GetQValWithNewGeometry ( string filename, string treeName, long long int nEntries, string offsetsList,
				function<bool()> testfn = []() -> double
				{	return true;});

		void GetQValWithNewGeometry ( string filename, string treeName, long long int nEntries, int minQQQ5OffX, int maxQQQ5OffX, int minQQQ5OffY, int maxQQQ5OffY, int minQQQ5OffZ, int maxQQQ5OffZ, int stepQQQ5,
				int minSX3OffX, int maxSX3OffX, int minSX3OffY, int maxSX3OffY,int minSX3OffZ, int maxSX3OffZ, int stepSX3,
				int minTargetOffX, int maxTargetOffX, int minTargetOffY, int maxTargetOffY,int minTargetOffZ, int maxTargetOffZ, int stepTarget,
				function<bool()> testfn = []() -> double
				{	return true;});

		std::pair<double, double> FindPeakPos ( TH1* input, int nPeaks, double resolution = 1, double sigma = 2, double threshold = 0.05 );
		TGraph* FindKinematicsLines ( TH2* input, int projWidth = 1 );

		TF1* FindBestGeom ( string fName, string detStr );
		vector<TH2F*> FindBestGeomV2( string fName, string baseHistName = "", double pmin = 1, double pmax = 32, int step_iy = 200, int iy_min = -1000, int iy_max = 1000, int step_iz = 200, int iz_min = -1000, int iz_max = 1000 );
		vector<TH2F*> FindBestGeomV3( string fName, string baseHistName = "", double pmin = 1, double pmax = 32, int step_iy = 200, int iy_min = -1000, int iy_max = 1000, int step_iz = 200, int iz_min = -1000, int iz_max = 1000 );

		ClassDef ( GoddessGeomUtils, 1 )
	};

template<typename T> void GoddessGeomUtils::SetIgnoreSectors(bool turn_off, T sect)
{
	if (turn_off)
	{
		if (std::find(ignoreSectorsList.begin(), ignoreSectorsList.end(), (int) sect) == ignoreSectorsList.end()) ignoreSectorsList.push_back((int) sect);
	}
	else
	{
		auto sItr = std::find(ignoreSectorsList.begin(), ignoreSectorsList.end(), (int) sect);
		if (sItr != ignoreSectorsList.end()) ignoreSectorsList.erase(sItr);
	}
}

template<typename T, typename ... R> void GoddessGeomUtils::SetIgnoreSectors(bool turn_off, T sect, R ... rest)
{
	SetIgnoreSectors(turn_off, sect);
	SetIgnoreSectors(turn_off, rest...);
}

extern GoddessGeomUtils* gGU;

#endif
