#ifndef __GODDESSCALIB__
#define __GODDESSCALIB__

#include "GoddessAnalysis.h"
#include <stack>
#include "TSpectrum.h"
#include <pthread.h>
#include <mutex>
#include<condition_variable>

const int minModEndcaps = 40;
const int maxModEndcaps = 160;

extern vector<std::map<int, TH1F*>> hEn_QQQ5UA;
extern vector<std::map<int, TH1F*>> hEn_QQQ5UB;
extern vector<std::map<int, TH1F*>> hEn_QQQ5UC;
extern vector<std::map<int, TH1F*>> hEn_QQQ5UD;

extern vector<std::map<int, TH1F*>> hQVal_QQQ5UA;
extern vector<std::map<int, TH1F*>> hQVal_QQQ5UB;
extern vector<std::map<int, TH1F*>> hQVal_QQQ5UC;
extern vector<std::map<int, TH1F*>> hQVal_QQQ5UD;

TF1* FitQValGS(TH1* hist, vector<float> mean, float fwhm, float peakRatio, float minBound = 0, float maxBound = 0, bool verbose = false);
TF1* FitQVal(TH1* hist, vector<string> mean, float fwhm_min, float fwhm_max, float minBound, float maxBound, string mode = "TSpectrum", bool verbose = false);

extern map<string, string> stoppingPowerTable;

extern map<string, std::pair<float, float>> configCalPars;
void ReadConfigCalPars(string configFileName);

///Class providing tools to calibrate GODDESS detectors
class GoddessCalib : public GoddessAnalysis {
	private:
		GoddessCalib();

		static GoddessCalib* s_instance;
		TGMainFrame* controlFrame;
		pthread_t checkThread;

		void InitGUI();

	public:
		virtual ~GoddessCalib();

		static void RegisterClassForROOTSession()
		{
		}

		// ****************** Singleton stuffs ******************************* //

		static GoddessCalib* sinstance()
		{
			return s_instance;
		}

		static void DeleteSInstance();

		static GoddessCalib* StartCalib();
		static void StartSX3EnCalib(string detectorType = "SuperX3", double refEnergy1 = 5.813);

		// ****************** User Interface Interactions ******************** //

		string currDetType;
		double currRefEn1;

		static void OnClickParamsButtons(string mode);
		static void ValidateParamsButtons(string mode);

		static void OnClickUpdateConf();

		static void OnClickPlotEnCalGraphs();
		static void ValidatePlotEnCalGraphs();

		static void OnClickPlotPosCalGraphs();
		static void ValidatePlotPosCalGraphs();

		static void OnClickGetLinesInfo();
		static void OnClickGetStripsEdges();
		static void ValidateGetStripsEdges();

		static void OnClickAddToChain();
		static void OnClickResetChain();

		static void SetGSEAutoMode();
		static void SetGSEFixedWinMode();

		// ****************** user Interface Utilities ********************** //

		TGMainFrame* GetControlFrame()
		{
			return controlFrame;
		}

		static TGWindow* FindWindowByName(string winName);
		static TGFrame* FindFrameByName(TGCompositeFrame* pFrame, string frameName);

		template<typename T> static TGFrame* FindFrameByName(T* pFrame, string frameName)
		{
			TGCompositeFrame* tryCast = dynamic_cast<TGCompositeFrame*>(pFrame);

			if (tryCast != NULL) return FindFrameByName(tryCast, frameName);
			else return nullptr;
		}

		// ****************** Calibration Base ************************* //

		string calTreeName;
		TChain* calChain;
		void AddFileToChain(string fileName);
		void ResetChain();

		string outFileName;
		string GetAutoOutFileName(string baseName);

		// ******************** Calibration Utilities ********************** //

		map<string, map<unsigned short, vector<double>>> resStripsCalMap;
		map<string, vector<double>> endcapsStripsCalMap;

		map<string, TH2F*> resStripsEnCalGraphsMap;
		map<string, TH2F*> resStripsPosCalGraphsMap;
		map<string, TGraph*> enShiftVsPosGraphs;

		void InitializeCalMapKey(string mapKey, unsigned short strip);

		static void PlotSX3ResStripsCalGraphsFromTree();
		static void PlotSX3ResStripsCalGraphs();
		static void Help();
		static void EnCalibHelp();
		static void PosCalibHelp();

		void DrawSX3EnCalGraph(bool isUpstream, unsigned short sector, unsigned short strip);
		void DrawSX3PosCalHist(bool isUpstream, unsigned short sector, unsigned short strip, string drawOpts);

		TH2F* GetSX3EnCalGraph(bool isUpstream, unsigned short sector, unsigned short strip);
		TH2F* GetSX3PosCalHist(bool isUpstream, unsigned short sector, unsigned short strip);

		void GetCornersCoordinates(TCanvas* can, bool isUpstream, unsigned short sector, unsigned short strip, string detectorType = "SuperX3", double refEnergy1 = 5.813);
		void GetCornersCoordinates(TCanvas* can, string calMapKey, unsigned short strip, double refEnergy1 = 5.813);

		bool DumpFileToResCalMap(string fileName = "Resistive_Strips_Calib_Params.txt");

		void WriteResCalResults(string fileName = "Resistive_Strips_Calib_Params.txt", string mode = "update");

		bool UpdateParamsInConf(string configFile, string detType, bool invertContactMidDet = true, string mode = "protected");
		static void PopupInputfield(TGWindow* pWin, float width_, float heigth_, short textSize, string label, TGLayoutHints* layHints, bool isEditable);

		TH2F* PlotSX3ResStripCalGraph(TChain* chain, string varToPlot, unsigned short sector, unsigned short strip, string conditions);

		TH2F* PlotSX3ResStripCalGraph(TChain* chain, bool isUpstream, unsigned short sector, unsigned short strip);

		template<typename T> inline void PlotSX3ResStripsCalGraphs(TChain* chain, string varToPlot, string conditions, T sector);

		template<typename First, typename ... Rest> inline void PlotSX3ResStripsCalGraphs(TChain* chain, string varToPlot, string conditions, First fstSector, Rest ... otherSectors);

		template<typename First, typename ... Rest> inline void PlotSX3ResStripsCalGraphs(TChain* chain, bool isUpstream, First fstSector, Rest ... otherSectors);

		void PlotSX3ResStripsCalGraphsFromTree(TChain* chain, long int nentries, bool isUpstream_, vector<int> sectorsList);
		template<typename FirstSector, typename ... VarArgs> inline void PlotSX3ResStripsCalGraphsFromTree(TChain* chain, bool isUpstream_, long int nentries, FirstSector fstSector,
				VarArgs ... sectors);

		TGraph* DrawPosCalGraph(TChain* chain, bool isUpstream_, int nentries, unsigned short sector_, unsigned short strip_);

		template<typename T> inline void DrawPosCal(TChain* chain, bool isUpstream_, unsigned short sector_, unsigned short strip_, int nentries, T* drawResult);

		TH2F* DrawPosCalHist(TChain* chain, bool isUpstream_, int nentries, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts,
				unsigned short sector_, unsigned short strip_);

		map<string, TH2F*> DrawPosCalHistBatch(TChain* chain, bool isUpstream_, int nentries, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY,
				string drawOpts, unsigned short sector_, string configFileName);

		map<string, TH2F*> DrawPosCalHistBatch(TChain* chain, bool isUpstream_, int nentries, int nbinsX, float binMinX, float binMaxX, int nbinsY, float binMinY, float binMaxY,
				string drawOpts, vector<int> sectorsList, string configFileName);

		template<typename First, typename ... Rest> inline map<string, TH2F*> DrawPosCalHistBatch(TChain* chain, bool isUpstream_, int nentries, int nbinsX, int binMinX, int binMaxX,
				int nbinsY, int binMinY, int binMaxY, string drawOpts, First fstSector, Rest ... otherSectors, string configFileName);

		int GetPosCalEnBinMax(TH2F* input, double threshold);

		TH1D* GetPosCalProjX(TH2F* input, int projCenter, int projWidth);
		TH1D* GetPosCalProjY(TH2F* input, int projCenter, int projWidth);

		TF1* FitEdges(TH2F* input, int projCenterBin, int projWidth, bool fitRight = true, bool getParams = true, bool quietMode = false);
		std::tuple<TGraph*, vector<vector<float>>> GetEnergyShiftVsPosition(TH2F* input, int nPoints, float startPoint, float endPoint, double threshold, double peakPos);
		void GetStripsEdges(int projCenterBin, int projWidth, double peakPos = 5.813, double threshold = 3.0, int shiftGrNPoints = 60, bool drawResults = true);
		void GetStripsEdges(TH2F* input, int projCenterBin, int projWidth, double peakPos = 5.813, double threshold = 3.0, int shiftGrNPoints = 60, bool drawResults = true);
		void GetStripsEdges(TFile* input, string sectorsList, double projWinMin = 5.5, double projWinMax = 6.2, double peakPos = 5.813, double threshold = 3.0, int shiftGrNPoints = 60,
				bool drawResults = true);
		void GetStripsEdges(TFile* input, string sectorsList, int projWidth, double threshold, double peakPos = 5.813, int shiftGrNPoints = 60, bool drawResults = true);

		vector<float> GetOverlapPoints(TH2F* input, float xMin, float xMax, double threshold, bool jumpUp = true, bool printDebug = false);

		void GenerateGainAdjustfile(string filesname, string treename, long long int nEntries, string outfname, double minAdjust, double maxAdjust, double stepWidth);

		void WritePosCalHistsToFile();

		// ---------------------- QQQ5 stuffs -------------------------------- //

//     vector<double> AdjustQValSpectrum ( vector<std::map<int, TH1F*>>* hists, float peakPos, float fwhm, string crossSectionInput,
//                                         float minBound = 0, float maxBound = 0, int minModEndcaps_ = minModEndcaps, int maxModEndcaps_ = maxModEndcaps,
//                                         string betterFitMode = "chi2 < bestChi2 && sigma <= bestSigma && magn >= bestMagn && gaussIntegral >= bestGaussIntegral && rawIntegral >= bestRawIntegral" );

		void LoadInternalCalib(string fileName);

	ClassDef ( GoddessCalib, 1 )
};

extern GoddessCalib* gC;

template<typename T> void GoddessCalib::PlotSX3ResStripsCalGraphs(TChain* chain, string varToPlot, string conditions, T sector)
{
	PlotSX3ResStripCalGraph(chain, varToPlot, sector, 0, conditions);
	PlotSX3ResStripCalGraph(chain, varToPlot, sector, 1, conditions);
	PlotSX3ResStripCalGraph(chain, varToPlot, sector, 2, conditions);
	PlotSX3ResStripCalGraph(chain, varToPlot, sector, 3, conditions);

	return;
}

template<typename First, typename ... Rest> void GoddessCalib::PlotSX3ResStripsCalGraphs(TChain* chain, string varToPlot, string conditions, First fstSector, Rest ... otherSectors)
{
	if (std::is_same<decltype ( fstSector ), int>::value || (unsigned short) fstSector < 0)
	{
		PlotSX3ResStripsCalGraphs<unsigned short>(chain, varToPlot, conditions, fstSector);

		if (sizeof... ( otherSectors ) > 0)
		{
			PlotSX3ResStripsCalGraphs(chain, varToPlot, conditions, otherSectors...);
		}
	}
	else
	{
		cerr << "One of the sector specified has an invalid type! (expecting positive integer)" << endl;
	}

	return;
}

template<typename First, typename ... Rest> void GoddessCalib::PlotSX3ResStripsCalGraphs(TChain* chain, bool isUpstream, First fstSector, Rest ... otherSectors)
{
	string upstreamCond = isUpstream ? "si.isUpstream" : "!si.isUpstream";
	string cond = "si.isBarrel && " + upstreamCond;

	PlotSX3ResStripsCalGraphs<First, Rest...>(chain, "si.E1.en.n:si.E1.en.p", cond, fstSector, otherSectors...);

	return;
}

template<typename FirstSector, typename ... VarArgs> void GoddessCalib::PlotSX3ResStripsCalGraphsFromTree(TChain* chain, bool isUpstream_, long int nentries, FirstSector fstSector,
		VarArgs ... sectors)
{
	vector<int> sectorsList;

	GetListOfSectorsToTreat<FirstSector, VarArgs...>(&sectorsList, fstSector, sectors...);

	PlotSX3ResStripsCalGraphsFromTree(chain, nentries, isUpstream_, sectorsList);
}

template<typename T> void GoddessCalib::DrawPosCal(TChain* chain, bool isUpstream_, unsigned short sector_, unsigned short strip_, int nentries, T* drawResult)
{
	vector<SiDataDetailed>* siDataVect = new vector<SiDataDetailed>();
	siDataVect->clear();

	chain->SetBranchAddress("si", &siDataVect);

	if (nentries > chain->GetEntries() || nentries == 0) nentries = chain->GetEntries();

	for (int i = 0; i < nentries; i++)
	{
		chain->GetEntry(i);

		if (i % 10000 == 0) cout << "Treated " << i << " / " << nentries << " entries ( " << ((float) i) / ((float) nentries) * 100. << " % )\r" << std::flush;

		if (siDataVect->size() == 0) continue;

		for (unsigned short j = 0; j < siDataVect->size(); j++)
		{
			SiDataDetailed siData = siDataVect->at(j);

			if (siData.isBarrel && siData.isUpstream == isUpstream_ && siData.sector == sector_ && siData.E1.en.p.size() > 0 && siData.E1.en.n.size() > 0)
			{
				for (unsigned short k = 0; k < siData.E1.en.p.size(); k++)
				{
					if (siData.E1.strip.p[k] != strip_) continue;

					if (std::is_same<decltype ( drawResult ), TH2F*>::value)
					{
						((TH2F*) drawResult)->Fill((siData.E1.en.n[k] - siData.E1.en.p[k]) / (siData.E1.en.p[k] + siData.E1.en.n[k]), (siData.E1.en.p[k] + siData.E1.en.n[k]));
						break;
					}
					else if (std::is_same<decltype ( drawResult ), TGraph*>::value)
					{
						((TGraph*) drawResult)->SetPoint(((TGraph*) drawResult)->GetN(), (siData.E1.en.n[k] - siData.E1.en.p[k]) / (siData.E1.en.p[k] + siData.E1.en.n[k]),
								(siData.E1.en.p[k] + siData.E1.en.n[k]));
						break;
					}
					else return;
				}
			}
		}
	}

	cout << endl;
}

template<typename First, typename ... Rest> map<string, TH2F*> GoddessCalib::DrawPosCalHistBatch(TChain* chain, bool isUpstream_, int nentries, int nbinsX, int binMinX,
		int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, First fstSector, Rest ... otherSectors, string configFileName)
{
	vector<int> sectorsList;
	sectorsList.clear();

	GetListOfSectorsToTreat<First, Rest...>(&sectorsList, fstSector, otherSectors...);

	return DrawPosCalHistBatch(chain, isUpstream_, nentries, nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY, drawOpts, sectorsList, configFileName);
}

// ---------------------------------------- QQQ5 functions ----------------------------------------------- //

float GetRatioGSvsFirstEx(string inputName, float minAngle, float maxAngle);

#endif
