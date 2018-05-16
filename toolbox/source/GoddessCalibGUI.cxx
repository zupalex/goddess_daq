#include "GoddessCalib.h"

void GoddessCalib::InitGUI()
{
	controlFrame = new TGMainFrame( gClient->GetRoot(), 200, 200);
	controlFrame->SetWindowName("Control Frame");
	controlFrame->SetName("Control Frame");

	controlFrame->Connect("Destroyed()", "GoddessCalib", sinstance(), "DeleteSInstance()");

	// ------ Creating Main Vertical Frame ------ //

	TGVerticalFrame* vertFrame = new TGVerticalFrame(controlFrame, 200, 200);
	vertFrame->SetName("Main Vertical Frame");
	vertFrame->SetBackgroundColor(0x4d004d);

	// ------ Adding the button to start the SX3 Calibration ------ //

	TGTextButton* sSX3CB = new TGTextButton(vertFrame, "Start SuperX3 Calib", "GoddessCalib::StartSX3EnCalib()");
	sSX3CB->SetName("Start SuperX3 Calib");
	sSX3CB->SetFont("-*-helvetica-medium-r-*-*-20-*-*-*-*-*-iso8859-1");

	vertFrame->AddFrame(sSX3CB, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 100, 100, 20, 30));

	// ------ Adding the input field for the name of the tree used in the user TChain ------ //

	TGHorizontalFrame* subTreeNameFrame = new TGHorizontalFrame(vertFrame, 200, 400);
	subTreeNameFrame->SetName("SubTreeNameFrame");
	subTreeNameFrame->SetBackgroundColor(0x4d004d);

	TGLabel* tNLabel = new TGLabel(subTreeNameFrame, "Tree Name:");
	tNLabel->SetBackgroundColor(0x4d004d);
	tNLabel->SetTextColor(0xffffff);

	TGTextEntry* treeNameIF = new TGTextEntry(subTreeNameFrame);
	treeNameIF->SetName("ChainTreeNameIF");
	treeNameIF->Resize(130, 22);
	treeNameIF->SetText("god");

	subTreeNameFrame->AddFrame(tNLabel, new TGLayoutHints(kLHintsCenterX, 20, 2, 0, 0));
	subTreeNameFrame->AddFrame(treeNameIF, new TGLayoutHints(kLHintsCenterX, 2, 20, 0, 0));

	vertFrame->AddFrame(subTreeNameFrame, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 0, 0, 0, 10));

	// ------ Adding the button to add a file to the user TChain ------ //

	TGHorizontalFrame* subChainFrame = new TGHorizontalFrame(vertFrame, 200, 400);
	subChainFrame->SetName("SubChainFrame");
	subChainFrame->SetBackgroundColor(0x4d004d);

	TGTextButton* aFTC = new TGTextButton(subChainFrame, "Add current file to TChain", "GoddessCalib::OnClickAddToChain()");
	aFTC->SetName("Add current file to chain");

	TGTextButton* resetC = new TGTextButton(subChainFrame, "Reset TChain", "GoddessCalib::OnClickResetChain()");
	resetC->SetName("Reset TChain");

	subChainFrame->AddFrame(aFTC, new TGLayoutHints(kLHintsCenterX, 20, 25, 0, 0));
	subChainFrame->AddFrame(resetC, new TGLayoutHints(kLHintsCenterX, 25, 20, 0, 0));

	vertFrame->AddFrame(subChainFrame, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 0, 0, 0, 30));

	// ------ Adding the first horizontal subframe for plotting the calib graphs ------ //

	TGHorizontalFrame* subHorzFrame1 = new TGHorizontalFrame(vertFrame, 200, 400);
	subHorzFrame1->SetName("SubHorizontalFrame1");
	subHorzFrame1->SetBackgroundColor(0x4d004d);

	TGTextButton* pEnCalB = new TGTextButton(subHorzFrame1, "Plot Energy Calib. Graphs", "GoddessCalib::OnClickPlotEnCalGraphs()");
	pEnCalB->SetName("Plot Energy Calib. Graphs");

	TGTextButton* pPosCalB = new TGTextButton(subHorzFrame1, "Plot Pos. Calib Graphs", "GoddessCalib::OnClickPlotPosCalGraphs()");
	pPosCalB->SetName("Plot Pos. Calib Graphs");

	subHorzFrame1->AddFrame(pEnCalB, new TGLayoutHints(kLHintsCenterX, 20, 25, 0, 0));
	subHorzFrame1->AddFrame(pPosCalB, new TGLayoutHints(kLHintsCenterX, 25, 20, 0, 0));

	vertFrame->AddFrame(subHorzFrame1, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 0, 0, 0, 20));

	// ------ Adding the second horizontal subframe for writing and updating the calib parameters ------ //

	TGHorizontalFrame* subHorzFrame2 = new TGHorizontalFrame(vertFrame, 200, 400);
	subHorzFrame2->SetName("SubHorizontalFrame2");
	subHorzFrame2->SetBackgroundColor(0x4d004d);

	TGTextButton* dCPB = new TGTextButton(subHorzFrame2, "Reload Prev. Cal. Params", "GoddessCalib::OnClickParamsButtons(\"Reload\")");
	dCPB->SetName("Reload Prev. Cal. Params");

	TGTextButton* wCPB = new TGTextButton(subHorzFrame2, "Write New Cal. Params", "GoddessCalib::OnClickParamsButtons(\"Write\")");
	wCPB->SetName("Write New Cal. Params");

	subHorzFrame2->AddFrame(dCPB, new TGLayoutHints(kLHintsCenterX, 20, 25, 0, 0));
	subHorzFrame2->AddFrame(wCPB, new TGLayoutHints(kLHintsCenterX, 20, 25, 0, 0));

	vertFrame->AddFrame(subHorzFrame2, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 0, 0, 20, 10));

	// ------ Adding text input field for the config file name ------ //

	TGTextButton* uCFB = new TGTextButton(vertFrame, "Update Goddess Config File", "GoddessCalib::OnClickUpdateConf()");
	uCFB->SetName("Update Goddess Config File");
	vertFrame->AddFrame(uCFB, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 0, 0, 40, 0));

	PopupInputfield(vertFrame, 400, 0, 12, "Config Full Name", new TGLayoutHints(kLHintsCenterX, 15, 15, 0, 20), false);

	// ------ Adding the button to retreive the info from the TLines ------ //

	TGHorizontalFrame* subHorzFrame3 = new TGHorizontalFrame(vertFrame, 200, 400);
	subHorzFrame3->SetName("SubHorizontalFrame3");
	subHorzFrame3->SetBackgroundColor(0x4d004d);

	TGTextButton* gCornerB = new TGTextButton(subHorzFrame3, "Save TLines Info", "GoddessCalib::OnClickGetLinesInfo()");
	gCornerB->SetName("Save TLines Info");

	TGLabel* eAlphaLabel = new TGLabel(subHorzFrame3, "Alpha Energy:");
	eAlphaLabel->SetBackgroundColor(0x4d004d);
	eAlphaLabel->SetTextColor(0xffffff);

	TGNumberEntryField* alphaEn1IF = new TGNumberEntryField(subHorzFrame3, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAPositive);
	alphaEn1IF->SetName("alphaEn1IF");
	alphaEn1IF->Resize(90, 22);

	subHorzFrame3->AddFrame(eAlphaLabel, new TGLayoutHints(kLHintsLeft, 10, 10, 5, 5));
	subHorzFrame3->AddFrame(alphaEn1IF, new TGLayoutHints(kLHintsLeft, 10, 30, 5, 5));

	subHorzFrame3->AddFrame(gCornerB, new TGLayoutHints(kLHintsRight, 10, 10, 5, 5));

	vertFrame->AddFrame(subHorzFrame3, new TGLayoutHints(kLHintsCenterX, 0, 0, 40, 20));

	// ------ Adding the button to perform the Position Calibration ------ //

	TGHorizontalFrame* subHorzFrame4 = new TGHorizontalFrame(vertFrame, 200, 400);
	subHorzFrame4->SetName("SubHorizontalFrame4");
	subHorzFrame4->SetBackgroundColor(0x4d004d);

	TGTextButton* gEdgeB = new TGTextButton(subHorzFrame4, "Get Strips Edges", "GoddessCalib::OnClickGetStripsEdges()");
	gEdgeB->SetName("Save TLines Info");

	TGLabel* fNameLabel = new TGLabel(subHorzFrame4, "File:");
	fNameLabel->SetBackgroundColor(0x4d004d);
	fNameLabel->SetTextColor(0xffffff);

	TGTextEntry* fNameIF = new TGTextEntry(subHorzFrame4);
	fNameIF->SetName("fNameIF");
	fNameIF->Resize(130, 22);
	fNameIF->SetText("Currently Opened File");

	subHorzFrame4->AddFrame(fNameLabel, new TGLayoutHints(kLHintsLeft, 10, 10, 5, 5));
	subHorzFrame4->AddFrame(fNameIF, new TGLayoutHints(kLHintsLeft, 10, 35, 5, 5));

	subHorzFrame4->AddFrame(gEdgeB, new TGLayoutHints(kLHintsRight, 10, 10, 5, 5));

	vertFrame->AddFrame(subHorzFrame4, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 70));

	// ------ Wraping everything in the main frame ------ //

	controlFrame->AddFrame(vertFrame);

	controlFrame->MapSubwindows();
	controlFrame->Resize(controlFrame->GetDefaultSize());
	controlFrame->MapWindow();
}

TGWindow* GoddessCalib::FindWindowByName(string winName)
{
	string altName = winName;

	while (altName.find(" ") != string::npos)
	{
		altName = altName.replace(altName.find(" "), 1, "");
	}

	auto winList = gClient->GetListOfWindows();

	for (int i = 0; i < winList->GetSize(); i++)
	{
		if (winList->At(i)->GetName() == winName || winList->At(i)->GetName() == altName) return (TGWindow*) winList->At(i);
	}

	return nullptr;
}

TGFrame* GoddessCalib::FindFrameByName(TGCompositeFrame* pFrame, string frameName)
{
//     cout << "Scanning Frame " << pFrame->GetName() << "\n";

	string altName = frameName;

	while (altName.find(" ") != string::npos)
	{
		altName = altName.replace(altName.find(" "), 1, "");
	}

	TIter nFrame(pFrame->GetList());

	TGFrameElement* frameEl;

	while ((frameEl = (TGFrameElement*) nFrame()))
	{
		if (frameEl->fFrame->GetName() == frameName || frameEl->fFrame->GetName() == altName) return frameEl->fFrame;

		if (frameEl->fFrame->InheritsFrom(TGCompositeFrame::Class()))
		{
			TGFrame* findInSubFrame = FindFrameByName((TGCompositeFrame*) frameEl->fFrame, frameName);

			if (findInSubFrame != NULL) return findInSubFrame;
		}
	}

	return nullptr;
}

void GoddessCalib::PopupInputfield(TGWindow* pWin, float width_, float heigth_, short textSize, string label, TGLayoutHints* layHints, bool isEditable)
{
	TGGroupFrame* newGroup = new TGGroupFrame(pWin, label.c_str(), kVerticalFrame);
	newGroup->SetTitlePos(TGGroupFrame::kLeft);
	newGroup->SetBackgroundColor(0x4d004d);
	newGroup->SetTextColor(0xffffff);

	TGTextEntry* newTextEntry = new TGTextEntry(newGroup);
	TGDimension defDim = newTextEntry->GetDefaultSize();
	newTextEntry->SetName(label.c_str());

	float newWidth = width_;
	float newHeigth = heigth_;

	if (newWidth == 0) newWidth = defDim.fWidth;
	if (newHeigth == 0) newHeigth = defDim.fHeight;

	newTextEntry->Resize(newWidth, newHeigth);

	newTextEntry->SetFont(Form("-*-helvetica-medium-r-*-*-%d-*-*-*-*-*-iso8859-1", textSize));

	if (!isEditable)
	{
		newTextEntry->SetState(kFALSE);
		newTextEntry->SetEditable(true);
		newTextEntry->SetBackgroundColor(0x330033);
	}

//     newTextEntry->ChangeOptions ( ( newTextEntry->GetOptions() & ~kVerticalFrame ) | ( newTextEntry->GetOptions() & ~kDoubleBorder ) | kTransientFrame );

	newGroup->AddFrame(newTextEntry);

	TGMainFrame* mainFrameTest = dynamic_cast<TGMainFrame*>(pWin);
	TGVerticalFrame* vertFrameTest = dynamic_cast<TGVerticalFrame*>(pWin);
	TGHorizontalFrame* horzFrameTest = dynamic_cast<TGHorizontalFrame*>(pWin);

	if (mainFrameTest != NULL)
	{
		mainFrameTest->AddFrame(newGroup, layHints);
	}
	else if (vertFrameTest != NULL)
	{
		vertFrameTest->AddFrame(newGroup, layHints);
	}
	else if (horzFrameTest != NULL)
	{

		horzFrameTest->AddFrame(newGroup, layHints);
	}

	return;
}

void GoddessCalib::ValidateParamsButtons(string mode)
{
	TGWindow* paramsWin = FindWindowByName(Form("%s Parameters", mode.c_str()));

	if (paramsWin != NULL)
	{
		TGMainFrame* mf = (TGMainFrame*) paramsWin->GetMainFrame();

		TGTextEntry* tE;

		string fileName;

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "File name IF"));
		fileName = tE->GetText();

		if (fileName.empty()) fileName = "Resistive_Strips_Calib_Params.txt";

		if (mode == "Reload") GoddessCalib::sinstance()->DumpFileToResCalMap(fileName);
		else if (mode == "Write") GoddessCalib::sinstance()->WriteResCalResults(fileName);

		GoddessCalib::FindWindowByName(Form("%s Parameters", mode.c_str()))->UnmapWindow();
	}
}

void GoddessCalib::OnClickParamsButtons(string mode)
{
	TGWindow* paramsWin = FindWindowByName(Form("%s Parameters", mode.c_str()));

	if (paramsWin == NULL)
	{
		TGMainFrame* reloadParamsMF = new TGMainFrame( gClient->GetRoot(), 200, 200);
		reloadParamsMF->SetName(Form("%s Parameters", mode.c_str()));
		reloadParamsMF->SetWindowName(Form("%s Parameters", mode.c_str()));

		TGCompositeFrame* mainVertFrame = new TGCompositeFrame(reloadParamsMF, 200, 200);
		mainVertFrame->SetLayoutManager(new TGColumnLayout(mainVertFrame, 20));

		TGCompositeFrame* firstFrame = new TGCompositeFrame(mainVertFrame, 200, 200);
		firstFrame->SetLayoutManager(new TGRowLayout(firstFrame, 10));

		TGLabel* inputFileLabel = new TGLabel(firstFrame, "File name:");
		TGTextEntry* inputFileIF = new TGTextEntry(firstFrame, "Resistive_Strips_Calib_Params.txt");
		inputFileIF->SetName("File name IF");
		inputFileIF->SetAlignment(kTextRight);

		firstFrame->AddFrame(inputFileLabel);
		firstFrame->AddFrame(inputFileIF);

		mainVertFrame->AddFrame(firstFrame);

		TGCompositeFrame* secondFrame = new TGCompositeFrame(mainVertFrame, 200, 200);
		secondFrame->SetLayoutManager(new TGRowLayout(secondFrame, 10));

		string processCmd = Form("GoddessCalib::ValidateParamsButtons(\"%s\")", mode.c_str());
		string cancelCmd = Form("GoddessCalib::FindWindowByName(\"%s Parameters\")->UnmapWindow();", mode.c_str());

		TGTextButton* processButton = new TGTextButton(secondFrame, "Process", processCmd.c_str());
		TGTextButton* cancelButton = new TGTextButton(secondFrame, "Cancel", cancelCmd.c_str());

		secondFrame->AddFrame(processButton);
		secondFrame->AddFrame(cancelButton);

		mainVertFrame->AddFrame(secondFrame);

		reloadParamsMF->AddFrame(mainVertFrame, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 20, 20, 20, 20));

		reloadParamsMF->MapSubwindows();
		reloadParamsMF->Resize(reloadParamsMF->GetDefaultSize());
		firstFrame->Resize(reloadParamsMF->GetDefaultSize());
		secondFrame->Resize(reloadParamsMF->GetDefaultSize());
		reloadParamsMF->MapWindow();
	}
	else if (!paramsWin->IsMapped())
	{
		paramsWin->MapSubwindows();
		paramsWin->MapWindow();
	}
}

void GoddessCalib::ValidatePlotEnCalGraphs()
{
	TGWindow* plotEnCalWin = FindWindowByName("Plot Energy Calibration Graphs");

	if (plotEnCalWin != NULL)
	{
		TGMainFrame* mf = (TGMainFrame*) plotEnCalWin->GetMainFrame();

		TGTextEntry* tE;
		TGCheckButton* cB;

		string treeName, entriesStr, sectorsStr, outFName;
		bool isUS;

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "Tree Name IF"));
		treeName = tE->GetText();

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "Entries IF"));
		entriesStr = tE->GetText();

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "Sectors IF"));
		sectorsStr = tE->GetText();

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "OutFile Name IF"));
		outFName = tE->GetText();

		if (!outFName.empty())
		{
			GoddessCalib::sinstance()->outFileName = outFName;
		}

		cB = dynamic_cast<TGCheckButton*>(FindFrameByName(mf, "Is Upstream CB"));
		isUS = cB->GetState();

		TChain* chain = GoddessCalib::sinstance()->calChain;

		if (chain == NULL)
		{
			cout << "User Chain is empty. Trying to use the current opened file: " << gDirectory->GetName() << "\n";

			TTree* testTree = dynamic_cast<TTree*>( gDirectory->FindObjectAny(treeName.c_str()));

			if (testTree != nullptr)
			{
				string completePTreePath = testTree->GetDirectory()->GetPath();
				if (completePTreePath[completePTreePath.length() - 1] != '/') completePTreePath += "/";
				completePTreePath += treeName;

				if (completePTreePath.find(":") != string::npos) completePTreePath.replace(completePTreePath.find(":"), 1, "");

				cout << "Found matching tree in current file: " << testTree->GetDirectory()->GetPath() << "\n";

				GoddessCalib::sinstance()->calTreeName = treeName;

				GoddessCalib::sinstance()->AddFileToChain(completePTreePath.c_str());
				chain = GoddessCalib::sinstance()->calChain;
			}
		}

		if (chain == NULL) return;

		long long int nentries = stoul(entriesStr);

		if (nentries == 0 || nentries > chain->GetEntries()) nentries = chain->GetEntries();

		GoddessCalib::sinstance()->PlotSX3ResStripsCalGraphsFromTree(chain, nentries, isUS, DecodeNumberString(sectorsStr));
	}
}

void GoddessCalib::OnClickPlotEnCalGraphs()
{
	TGWindow* plotEnCalWin = FindWindowByName("Plot Energy Calibration Graphs");

	if (plotEnCalWin == NULL)
	{
		string outFName = GoddessCalib::sinstance()->GetAutoOutFileName("Resistive_Strips_EnCal_Graphs");

		TGMainFrame* plotEnCalMF = new TGMainFrame( gClient->GetRoot(), 100, 200);
		plotEnCalMF->SetName("Plot Energy Calibration Graphs");
		plotEnCalMF->SetWindowName("Plot Energy Calibration Graphs");

		TGMatrixLayout* mLay = new TGMatrixLayout(plotEnCalMF, 6, 2, 10, 10);
		plotEnCalMF->SetLayoutManager(mLay);

		TGLabel* treeNameLabel = new TGLabel(plotEnCalMF, "Tree Name:");
		TGTextEntry* treeNameIF = new TGTextEntry(plotEnCalMF);
		treeNameIF->SetName("Tree Name IF");
		treeNameIF->SetAlignment(kTextRight);
		treeNameIF->SetText("sorted");

		plotEnCalMF->AddFrame(treeNameLabel);
		plotEnCalMF->AddFrame(treeNameIF);

		TGLabel* nEntriesLabel = new TGLabel(plotEnCalMF, "Entries to treat (0 == all):");
		TGNumberEntryField* nEntriesIF = new TGNumberEntryField(plotEnCalMF, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive);
		nEntriesIF->SetName("Entries IF");

		plotEnCalMF->AddFrame(nEntriesLabel);
		plotEnCalMF->AddFrame(nEntriesIF);

		TGLabel* isUpstreamLabel = new TGLabel(plotEnCalMF, "Upstream?");
		TGCheckButton* isUpstreamCheckB = new TGCheckButton(plotEnCalMF);
		isUpstreamCheckB->SetName("Is Upstream CB");

		plotEnCalMF->AddFrame(isUpstreamLabel);
		plotEnCalMF->AddFrame(isUpstreamCheckB);

		TGLabel* sectorsLabel = new TGLabel(plotEnCalMF, "Sectors to treat \ne.g. : \"1, 4-8, 10\"");
		TGTextEntry* sectorsIF = new TGTextEntry(plotEnCalMF);
		sectorsIF->SetName("Sectors IF");
		sectorsIF->SetAlignment(kTextRight);

		plotEnCalMF->AddFrame(sectorsLabel);
		plotEnCalMF->AddFrame(sectorsIF);

		TGLabel* outFileLabel = new TGLabel(plotEnCalMF, "Output File Name:");
		TGTextEntry* outFileIF = new TGTextEntry(plotEnCalMF);
		TGDimension defDim = outFileIF->GetDefaultSize();
		outFileIF->SetName("OutFile Name IF");
		outFileIF->SetAlignment(kTextRight);
		outFileIF->Resize(300, defDim.fHeight);
		outFileIF->SetText(outFName.c_str());

		plotEnCalMF->AddFrame(outFileLabel);
		plotEnCalMF->AddFrame(outFileIF);

		TGTextButton* processButton = new TGTextButton(plotEnCalMF, "Process", "GoddessCalib::ValidatePlotEnCalGraphs()");
		TGTextButton* cancelButton = new TGTextButton(plotEnCalMF, "Cancel", "GoddessCalib::FindWindowByName(\"Plot Energy Calibration Graphs\")->UnmapWindow();");

		plotEnCalMF->AddFrame(processButton);
		plotEnCalMF->AddFrame(cancelButton);

		plotEnCalMF->MapSubwindows();
		plotEnCalMF->Resize(plotEnCalMF->GetDefaultSize());
		plotEnCalMF->MapWindow();
	}
	else if (!plotEnCalWin->IsMapped())
	{
		plotEnCalWin->MapSubwindows();
		plotEnCalWin->MapWindow();

		string outFName = GoddessCalib::sinstance()->GetAutoOutFileName("Resistive_Strips_EnCal_Graphs");
		TGTextEntry* tE;
		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(plotEnCalWin, "OutFile Name IF"));
		tE->SetText(outFName.c_str());
	}
}

void GoddessCalib::ValidatePlotPosCalGraphs()
{
	TGWindow* plotPosCalWin = FindWindowByName("Plot Position Calibration Graphs");

	if (plotPosCalWin != NULL)
	{
		TGMainFrame* mf = (TGMainFrame*) plotPosCalWin->GetMainFrame();

		TGTextEntry* tE;
		TGCheckButton* cB;

		string treeName, sectorsStr, configName, outFName;
		long long int nentries;
		int nBinsX, nBinsY;
		float binMinX, binMaxX, binMinY, binMaxY;
		bool isUS;

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "Tree Name IF"));
		treeName = tE->GetText();

		if (treeName.empty()) return;

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "Entries IF"));
		nentries = std::stoul(tE->GetText());

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "NBinsX IF"));
		nBinsX = std::stoi(tE->GetText());

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "BinMinX IF"));
		binMinX = std::stof(tE->GetText());

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "BinMaxX IF"));
		binMaxX = std::stof(tE->GetText());

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "NBinsY IF"));
		nBinsY = std::stoi(tE->GetText());

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "BinMinY IF"));
		binMinY = std::stof(tE->GetText());

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "BinMaxY IF"));
		binMaxY = std::stof(tE->GetText());

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "Sectors IF"));
		sectorsStr = tE->GetText();

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "Config File IF"));
		configName = tE->GetText();

		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "OutFile Name IF"));
		outFName = tE->GetText();

		if (!outFName.empty())
		{
			GoddessCalib::sinstance()->outFileName = outFName;
		}

		cB = dynamic_cast<TGCheckButton*>(FindFrameByName(mf, "Is Upstream CB"));
		isUS = cB->GetState();

		DecodeNumberString(sectorsStr);

		TChain* chain = GoddessCalib::sinstance()->calChain;

		if (chain == NULL)
		{
			cout << "User Chain is empty. Trying to use the current opened file: " << gDirectory->GetName() << "\n";

			TTree* testTree = dynamic_cast<TTree*>( gDirectory->FindObjectAny(treeName.c_str()));

			if (testTree != nullptr)
			{
				string completePTreePath = testTree->GetDirectory()->GetPath();
				if (completePTreePath[completePTreePath.length() - 1] != '/') completePTreePath += "/";
				completePTreePath += treeName;

				if (completePTreePath.find(":") != string::npos) completePTreePath.replace(completePTreePath.find(":"), 1, "");

				cout << "Found matching tree in current file: " << testTree->GetDirectory()->GetPath() << "\n";

				GoddessCalib::sinstance()->calTreeName = treeName;

				GoddessCalib::sinstance()->AddFileToChain(completePTreePath.c_str());
				chain = GoddessCalib::sinstance()->calChain;
			}
		}

		if (chain == NULL) return;

		if (nentries == 0 || nentries > chain->GetEntries()) nentries = chain->GetEntries();

		GoddessCalib::sinstance()->DrawPosCalHistBatch(chain, isUS, nentries, nBinsX, binMinX, binMaxX, nBinsY, binMinY, binMaxY, "", DecodeNumberString(sectorsStr), configName);
	}
}

void GoddessCalib::OnClickPlotPosCalGraphs()
{
	TGWindow* plotPosCalWin = FindWindowByName("Plot Position Calibration Graphs");

	if (plotPosCalWin == NULL)
	{
		string outFName = GoddessCalib::sinstance()->GetAutoOutFileName("Resistive_Strips_PosCal_Graphs");

		Pixel_t bcol = 0xb3d1ff;

		TGMainFrame* plotPosCalMF = new TGMainFrame( gClient->GetRoot(), 100, 200);
		plotPosCalMF->SetName("Plot Position Calibration Graphs");
		plotPosCalMF->SetWindowName("Plot Position Calibration Graphs");
		plotPosCalMF->SetBackgroundColor(bcol);

//         TGMatrixLayout* mLay = new TGMatrixLayout ( plotPosCalMF, 7, 2, 10, 10 );
//         plotPosCalMF->SetLayoutManager ( new TGColumnLayout ( plotPosCalMF ) );

		TGCompositeFrame* firstFrame = new TGCompositeFrame(plotPosCalMF, 100, 100);
		firstFrame->SetLayoutManager(new TGMatrixLayout(firstFrame, 3, 2, 5, 5));
		firstFrame->SetBackgroundColor(bcol);

		TGLabel* treeNameLabel = new TGLabel(firstFrame, "Tree Name:");
		treeNameLabel->SetBackgroundColor(bcol);
		TGTextEntry* treeNameIF = new TGTextEntry(firstFrame);
		treeNameIF->SetName("Tree Name IF");
		treeNameIF->SetAlignment(kTextRight);
		treeNameIF->SetText("sorted");

		firstFrame->AddFrame(treeNameLabel);
		firstFrame->AddFrame(treeNameIF);

		TGLabel* nEntriesLabel = new TGLabel(firstFrame, "Entries to treat (0 == all):");
		nEntriesLabel->SetBackgroundColor(bcol);
		TGNumberEntryField* nEntriesIF = new TGNumberEntryField(firstFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive);
		nEntriesIF->SetName("Entries IF");
		nEntriesIF->SetNumber(0);

		firstFrame->AddFrame(nEntriesLabel);
		firstFrame->AddFrame(nEntriesIF);

		TGLabel* isUpstreamLabel = new TGLabel(firstFrame, "Upstream?");
		isUpstreamLabel->SetBackgroundColor(bcol);
		TGCheckButton* isUpstreamCheckB = new TGCheckButton(firstFrame);
		isUpstreamCheckB->SetName("Is Upstream CB");
		isUpstreamCheckB->SetBackgroundColor(bcol);

		firstFrame->AddFrame(isUpstreamLabel);
		firstFrame->AddFrame(isUpstreamCheckB);

		plotPosCalMF->AddFrame(firstFrame, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0));

		TGCompositeFrame* secondFrame = new TGCompositeFrame(plotPosCalMF, 100, 100);
		secondFrame->SetLayoutManager(new TGMatrixLayout(secondFrame, 2, 4, 5, 0));
		secondFrame->SetBackgroundColor(bcol);

		TGLabel* xAxisBinsLabel = new TGLabel(secondFrame, "X Axis Binning (nbins, min, max)");
		xAxisBinsLabel->SetBackgroundColor(bcol);

		TGNumberEntryField* nbinsXIF = new TGNumberEntryField(secondFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber);
		TGDimension defDim = nbinsXIF->GetDefaultSize();
		nbinsXIF->SetName("NBinsX IF");
		nbinsXIF->SetNumber(200);
		TGNumberEntryField* binMinXIF = new TGNumberEntryField(secondFrame, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber);
		binMinXIF->SetName("BinMinX IF");
		binMinXIF->SetNumber(-1);
		TGNumberEntryField* binMaxXIF = new TGNumberEntryField(secondFrame, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber);
		binMaxXIF->SetName("BinMaxX IF");
		binMaxXIF->SetNumber(1);

		nbinsXIF->Resize(100, defDim.fHeight);
		binMinXIF->Resize(100, defDim.fHeight);
		binMaxXIF->Resize(100, defDim.fHeight);

		secondFrame->AddFrame(xAxisBinsLabel);
		secondFrame->AddFrame(nbinsXIF);
		secondFrame->AddFrame(binMinXIF);
		secondFrame->AddFrame(binMaxXIF);

		TGLabel* yAxisBinsLabel = new TGLabel(secondFrame, "Y Axis Binning (nbins, min, max)");
		yAxisBinsLabel->SetBackgroundColor(bcol);

		TGNumberEntryField* nbinsYIF = new TGNumberEntryField(secondFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber);
		nbinsYIF->SetName("NBinsY IF");
		nbinsYIF->SetNumber(500);
		TGNumberEntryField* binMinYIF = new TGNumberEntryField(secondFrame, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber);
		binMinYIF->SetName("BinMinY IF");
		binMinYIF->SetNumber(0);
		TGNumberEntryField* binMaxYIF = new TGNumberEntryField(secondFrame, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber);
		binMaxYIF->SetName("BinMaxY IF");
		binMaxYIF->SetNumber(10);

		nbinsYIF->Resize(100, defDim.fHeight);
		binMinYIF->Resize(100, defDim.fHeight);
		binMaxYIF->Resize(100, defDim.fHeight);

		secondFrame->AddFrame(yAxisBinsLabel);
		secondFrame->AddFrame(nbinsYIF);
		secondFrame->AddFrame(binMinYIF);
		secondFrame->AddFrame(binMaxYIF);

		plotPosCalMF->AddFrame(secondFrame, new TGLayoutHints(kLHintsCenterX, 0, 0, 0, 0));

		TGCompositeFrame* thirdFrame = new TGCompositeFrame(plotPosCalMF, 100, 100);
		thirdFrame->SetLayoutManager(new TGRowLayout(thirdFrame, 10));
		thirdFrame->SetBackgroundColor(bcol);

		TGLabel* sectorsLabel = new TGLabel(thirdFrame, "Sectors to treat \ne.g. : \"1, 4-8, 10\"");
		sectorsLabel->SetBackgroundColor(bcol);
		TGTextEntry* sectorsIF = new TGTextEntry(thirdFrame);
		sectorsIF->SetName("Sectors IF");
		sectorsIF->SetAlignment(kTextRight);

		thirdFrame->AddFrame(sectorsLabel);
		thirdFrame->AddFrame(sectorsIF);

		plotPosCalMF->AddFrame(thirdFrame, new TGLayoutHints(kLHintsCenterX, 0, 0, 10, 0));

		TGCompositeFrame* fourthFrame = new TGCompositeFrame(plotPosCalMF, 100, 100);
		fourthFrame->SetLayoutManager(new TGRowLayout(fourthFrame, 10));
		fourthFrame->SetBackgroundColor(bcol);

		TGLabel* configFileLabel = new TGLabel(fourthFrame, "Config File to use: ");
		configFileLabel->SetBackgroundColor(bcol);
		TGTextEntry* configFileIF = new TGTextEntry(fourthFrame);
		configFileIF->SetName("Config File IF");
		configFileIF->SetAlignment(kTextRight);
		configFileIF->Resize(500, defDim.fHeight);

		fourthFrame->AddFrame(configFileLabel);
		fourthFrame->AddFrame(configFileIF);

		plotPosCalMF->AddFrame(fourthFrame, new TGLayoutHints(kLHintsCenterX, 0, 0, 10, 0));

		TGCompositeFrame* outFileFrame = new TGCompositeFrame(plotPosCalMF, 100, 100);
		outFileFrame->SetLayoutManager(new TGRowLayout(outFileFrame, 10));
		outFileFrame->SetBackgroundColor(bcol);

		TGLabel* outFileLabel = new TGLabel(outFileFrame, "Output File Name:");
		outFileLabel->SetBackgroundColor(bcol);
		TGTextEntry* outFileIF = new TGTextEntry(outFileFrame);
		outFileIF->SetName("OutFile Name IF");
		outFileIF->SetAlignment(kTextRight);
		outFileIF->Resize(500, defDim.fHeight);
		outFileIF->SetText(outFName.c_str());

		outFileFrame->AddFrame(outFileLabel);
		outFileFrame->AddFrame(outFileIF);

		plotPosCalMF->AddFrame(outFileFrame, new TGLayoutHints(kLHintsCenterX, 0, 0, 20, 0));

		TGCompositeFrame* fifthFrame = new TGCompositeFrame(plotPosCalMF, 100, 100);
		fifthFrame->SetLayoutManager(new TGRowLayout(fifthFrame, 10));
		fifthFrame->SetBackgroundColor(bcol);

		TGTextButton* processButton = new TGTextButton(fifthFrame, "Process", "GoddessCalib::ValidatePlotPosCalGraphs()");
		processButton->SetBackgroundColor(bcol);
		TGTextButton* cancelButton = new TGTextButton(fifthFrame, "Cancel", "GoddessCalib::FindWindowByName(\"Plot Position Calibration Graphs\")->UnmapWindow();");
		cancelButton->SetBackgroundColor(bcol);

		fifthFrame->AddFrame(processButton);
		fifthFrame->AddFrame(cancelButton);

		plotPosCalMF->AddFrame(fifthFrame, new TGLayoutHints(kLHintsCenterX, 0, 0, 20, 0));

		plotPosCalMF->MapSubwindows();
		plotPosCalMF->Resize(plotPosCalMF->GetDefaultSize());
		plotPosCalMF->MapWindow();
	}
	else if (!plotPosCalWin->IsMapped())
	{
		plotPosCalWin->MapSubwindows();
		plotPosCalWin->MapWindow();

		string outFName = GoddessCalib::sinstance()->GetAutoOutFileName("Resistive_Strips_PosCal_Graphs");
		TGTextEntry* tE;
		tE = dynamic_cast<TGTextEntry*>(FindFrameByName(plotPosCalWin, "OutFile Name IF"));
		tE->SetText(outFName.c_str());
	}
}

void GoddessCalib::OnClickUpdateConf()
{
	TGTextEntry* configNameIF = dynamic_cast<TGTextEntry*>(FindFrameByName(GoddessCalib::sinstance()->GetControlFrame(), "Config Full Name"));

//     if(configNameIF != NULL) cout << "Found the Input Field for the Config Name...\n";

	if (!configNameIF->IsEnabled())
	{
//         cout << "Config Name Input Field was disabled. Enabling it...\n";

		configNameIF->SetState(kTRUE);
		configNameIF->SetEnabled(kTRUE);
	}
	else
	{
		string confName = configNameIF->GetText();

		configNameIF->SetText("");

//         cout << "Config Name Input Field was enabled. Read " << confName << "\n";

		if (confName.length() > 0) GoddessCalib::sinstance()->UpdateParamsInConf(confName, "superX3");

		configNameIF->SetState(kFALSE);
		configNameIF->SetEnabled(kFALSE);
		configNameIF->SetBackgroundColor(0x330033);
	}

	return;
}

void GoddessCalib::OnClickGetLinesInfo()
{
	if (GoddessCalib::sinstance() == NULL) return;

	short isUS_ = -1, sect_ = -1, strip_ = -1;

	if ( gPad == NULL || gPad->GetCanvas() == NULL)
	{
		cerr << "No Canvas available!\n";
		cerr << "To get help with the energy calibration procedure, type GoddessCalib::EnCalibHelp()\n";
		return;
	}

	auto canPrimList = gPad->GetCanvas()->GetListOfPrimitives();

	TH2* gr;

	string grName_ = "";

	cout << canPrimList->GetSize() << endl;

	for (int i = 0; i < canPrimList->GetSize(); i++)
	{
		cout << canPrimList->At(i)->GetName() << endl;
		gr = dynamic_cast<TH2*>(canPrimList->At(i));

		if (gr != nullptr)
		{
			grName_ = gr->GetTitle();
			break;
		}
	}

	cout << grName_ << endl;

	if (gr != nullptr)
	{
		std::size_t sectPos = grName_.find("sector");
		std::size_t stripPos = grName_.find("strip");
		std::size_t separatorPos = grName_.find("_");

		if (stripPos == string::npos)
		{
			stripPos = grName_.find("Strip");
		}

		if (stripPos != string::npos) strip_ = std::stoi(grName_.substr(stripPos + 5));

		if (sectPos != string::npos && stripPos != string::npos && separatorPos != string::npos)
		{
			if (grName_[sectPos + 6] == 'U') isUS_ = 1;
			else if (grName_[sectPos + 6] == 'D') isUS_ = 0;
			else return;

			sect_ = std::stoi(grName_.substr(sectPos + 7, separatorPos));
		}
	}

	if (strip_ == -1)
	{
		cerr << esc << "[38;2;255;0;0m" << "The histogram used do not fulfill the title requirement! Did you produce it with the GoddessCalib class built-in functions?" << esc
				<< "[0m \n";
		cerr << esc << "[38;2;255;0;0m" << "To get help with the energy calibration procedure, type GoddessCalib::EnCalibHelp()" << esc << "[0m \n";
		cerr << esc << "[38;2;255;0;0m" << "The histogram name must at least finish with \"strip[n]\" where [n] is the strip number" << esc << "[0m \n";
		return;
	}

	TGNumberEntryField* alphaEnIF = (TGNumberEntryField*) FindWindowByName("alphaEn1IF");

	float alphaEn1 = std::stof(alphaEnIF->GetText());

	GoddessCalib::sinstance()->currRefEn1 = alphaEn1;

	if (isUS_ == -1 || sect_ == -1)
	{
		string calMapKey = grName_.substr(0, grName_.find("strip"));
		GoddessCalib::sinstance()->GetCornersCoordinates( gPad->GetCanvas(), calMapKey, (short unsigned int) strip_, GoddessCalib::sinstance()->currRefEn1);
	}
	else
	{
		GoddessCalib::sinstance()->GetCornersCoordinates( gPad->GetCanvas(), (bool) isUS_, (short unsigned int) sect_, (short unsigned int) strip_,
				GoddessCalib::sinstance()->currDetType, GoddessCalib::sinstance()->currRefEn1);
	}

	cout << endl;

	return;
}

void GoddessCalib::ValidateGetStripsEdges()
{
	if (GoddessCalib::sinstance() == NULL) return;

	TGTextEntry* fileNameIF = (TGTextEntry*) FindFrameByName(GoddessCalib::sinstance()->s_instance->GetControlFrame(), "fNameIF");

	string fileName = fileNameIF->GetText();

	TFile *input;

	if (fileName.length() > 0 || fileName == "Currently Opened File")
	{
		input = gDirectory->GetFile();
	}
	else
	{
		input = new TFile(fileName.c_str(), "read");
	}

	if (input == NULL || !input->IsOpen())
	{
		cerr << "Invalid input file name..." << endl;
		return;
	}

	cout << "Opened file: " << input->GetName() << "\n";

	TGNumberEntryField* alphaEn1IF = (TGNumberEntryField*) FindFrameByName(GoddessCalib::sinstance()->s_instance->GetControlFrame(), "alphaEn1IF");

	double alphaEn1 = alphaEn1IF->GetNumber();

	TGWindow* prompt = FindWindowByName("Get Strips Edges");

	TGCheckButton* drawResCB = (TGCheckButton*) FindFrameByName(prompt, "Draw Results CB");
	TGNumberEntryField* projWidthIF = (TGNumberEntryField*) FindFrameByName(prompt, "Proj Width IF");
	TGNumberEntryField* projThrIF = (TGNumberEntryField*) FindFrameByName(prompt, "Proj Threshold IF");

	TGTextEntry* sectorsIF = (TGTextEntry*) FindFrameByName(prompt, "Sectors GSE IF");

	TGNumberEntryField* projMinIF = (TGNumberEntryField*) FindFrameByName(prompt, "Proj Win Min IF");
	TGNumberEntryField* projMaxIF = (TGNumberEntryField*) FindFrameByName(prompt, "Proj Win Max IF");

	TGNumberEntryField* shiftGrNPointsIF = (TGNumberEntryField*) FindFrameByName(prompt, "Shift Graph NPoints IF");

	int projWidth_ = projWidthIF->GetNumber();
	double thr_ = projThrIF->GetNumber();

	double projWinMin_ = projMinIF->GetNumber();
	double projWinMax_ = projMaxIF->GetNumber();

	int shiftGrNPoints_ = shiftGrNPointsIF->GetNumber();

	bool doDraw = drawResCB->GetState();
	string sectorsList = sectorsIF->GetText();

	if (projWidthIF->IsEnabled() && projThrIF->IsEnabled()) GoddessCalib::sinstance()->GetStripsEdges(input, sectorsList, projWidth_, thr_, alphaEn1, shiftGrNPoints_, doDraw);
	else if (projMinIF->IsEnabled() && projMaxIF->IsEnabled()) GoddessCalib::sinstance()->GetStripsEdges(input, sectorsList, projWinMin_, projWinMax_, alphaEn1, thr_,
			shiftGrNPoints_, doDraw);

	return;
}

void GoddessCalib::SetGSEAutoMode()
{
	TGWindow* getStripsEdgesWin = FindWindowByName("Get Strips Edges");

	if (getStripsEdgesWin != NULL)
	{
		TGNumberEntryField* projWidthIF = (TGNumberEntryField*) FindFrameByName(getStripsEdgesWin, "Proj Width IF");

		TGNumberEntryField* projMinIF = (TGNumberEntryField*) FindFrameByName(getStripsEdgesWin, "Proj Win Min IF");
		TGNumberEntryField* projMaxIF = (TGNumberEntryField*) FindFrameByName(getStripsEdgesWin, "Proj Win Max IF");

		if (projWidthIF != nullptr)
		{
			projWidthIF->SetState(kTRUE);
			projWidthIF->SetEnabled(kTRUE);
			projWidthIF->SetBackgroundColor(0xffffff);
		}

		if (projMinIF != nullptr)
		{
			projMinIF->SetState(kFALSE);
			projMinIF->SetEnabled(kFALSE);
			projMinIF->SetBackgroundColor(0x424949);
		}

		if (projMaxIF != nullptr)
		{
			projMaxIF->SetState(kFALSE);
			projMaxIF->SetEnabled(kFALSE);
			projMaxIF->SetBackgroundColor(0x424949);
		}
	}
}

void GoddessCalib::SetGSEFixedWinMode()
{
	TGWindow* getStripsEdgesWin = FindWindowByName("Get Strips Edges");

	if (getStripsEdgesWin != NULL)
	{
		TGNumberEntryField* projWidthIF = (TGNumberEntryField*) FindFrameByName(getStripsEdgesWin, "Proj Width IF");

		TGNumberEntryField* projMinIF = (TGNumberEntryField*) FindFrameByName(getStripsEdgesWin, "Proj Win Min IF");
		TGNumberEntryField* projMaxIF = (TGNumberEntryField*) FindFrameByName(getStripsEdgesWin, "Proj Win Max IF");

		if (projWidthIF != nullptr)
		{
			projWidthIF->SetState(kFALSE);
			projWidthIF->SetEnabled(kFALSE);
			projWidthIF->SetBackgroundColor(0x424949);
		}

		if (projMinIF != nullptr)
		{
			projMinIF->SetState(kTRUE);
			projMinIF->SetEnabled(kTRUE);
			projMinIF->SetBackgroundColor(0xffffff);
		}

		if (projMaxIF != nullptr)
		{
			projMaxIF->SetState(kTRUE);
			projMaxIF->SetEnabled(kTRUE);
			projMaxIF->SetBackgroundColor(0xffffff);
		}
	}
}

void GoddessCalib::OnClickGetStripsEdges()
{
	TGWindow* getStripsEdgesWin = FindWindowByName("Get Strips Edges");

	if (getStripsEdgesWin == NULL)
	{
		TGMainFrame* getStripsEdgesMF = new TGMainFrame( gClient->GetRoot(), 100, 100);
		getStripsEdgesMF->SetName("Get Strips Edges");
		getStripsEdgesMF->SetWindowName("Get Strips Edges");

		getStripsEdgesMF->SetLayoutManager(new TGMatrixLayout(getStripsEdgesMF, 9, 2, 5, 5));

		TGTextButton* autoModeButton = new TGTextButton(getStripsEdgesMF, "Automatic Mode", "GoddessCalib::SetGSEAutoMode();");
		TGTextButton* fixedWinMode = new TGTextButton(getStripsEdgesMF, "Fixed Window Mode", "GoddessCalib::SetGSEFixedWinMode();");

		getStripsEdgesMF->AddFrame(autoModeButton);
		getStripsEdgesMF->AddFrame(fixedWinMode);

		// ----------------------------------- //

		TGLabel* projWidthLabel = new TGLabel(getStripsEdgesMF, "Proj. Width (bins):");
		TGNumberEntryField* projWidthIF = new TGNumberEntryField(getStripsEdgesMF, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive);
		projWidthIF->SetName("Proj Width IF");
		projWidthIF->SetNumber(20);

		getStripsEdgesMF->AddFrame(projWidthLabel);
		getStripsEdgesMF->AddFrame(projWidthIF);

		TGLabel* projThrLabel = new TGLabel(getStripsEdgesMF, "Max Detection Threshold:");
		TGNumberEntryField* projThrIF = new TGNumberEntryField(getStripsEdgesMF, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive);
		projThrIF->SetName("Proj Threshold IF");
		projThrIF->SetNumber(3.0);

		getStripsEdgesMF->AddFrame(projThrLabel);
		getStripsEdgesMF->AddFrame(projThrIF);

		// ----------------------------------- //

		TGLabel* projMinLabel = new TGLabel(getStripsEdgesMF, "Proj. Window Min:");
		TGNumberEntryField* projMinIF = new TGNumberEntryField(getStripsEdgesMF, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAPositive);
		projMinIF->SetName("Proj Win Min IF");
		projMinIF->SetNumber(5.5);
		projMinIF->SetState(kFALSE);
		projMinIF->SetEnabled(kFALSE);
		projMinIF->SetBackgroundColor(0x424949);

		getStripsEdgesMF->AddFrame(projMinLabel);
		getStripsEdgesMF->AddFrame(projMinIF);

		TGLabel* projMaxLabel = new TGLabel(getStripsEdgesMF, "Proj. Window Max:");
		TGNumberEntryField* projMaxIF = new TGNumberEntryField(getStripsEdgesMF, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAPositive);
		projMaxIF->SetName("Proj Win Max IF");
		projMaxIF->SetNumber(6.2);
		projMaxIF->SetState(kFALSE);
		projMaxIF->SetEnabled(kFALSE);
		projMaxIF->SetBackgroundColor(0x424949);

		getStripsEdgesMF->AddFrame(projMaxLabel);
		getStripsEdgesMF->AddFrame(projMaxIF);

		// ----------------------------------- //

		TGLabel* shiftGrNPointsLabel = new TGLabel(getStripsEdgesMF, "Energy shift vs. position correction :\n           number of points           ");
		TGNumberEntryField* shiftGrNPointsIF = new TGNumberEntryField(getStripsEdgesMF, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive);
		shiftGrNPointsIF->SetName("Shift Graph NPoints IF");
		shiftGrNPointsIF->SetNumber(60);

		getStripsEdgesMF->AddFrame(shiftGrNPointsLabel);
		getStripsEdgesMF->AddFrame(shiftGrNPointsIF);

		// ----------------------------------- //

		TGLabel* sectorsLabel = new TGLabel(getStripsEdgesMF, "Sectors to treat \ne.g. : \"1, 4-8, 10\"");
		TGTextEntry* sectorsGSEIF = new TGTextEntry(getStripsEdgesMF);
		sectorsGSEIF->SetName("Sectors GSE IF");
		sectorsGSEIF->SetAlignment(kTextRight);

		getStripsEdgesMF->AddFrame(sectorsLabel);
		getStripsEdgesMF->AddFrame(sectorsGSEIF);

		// ----------------------------------- //

		TGLabel* drawResultsLabel = new TGLabel(getStripsEdgesMF, "Draw Results?");
		TGCheckButton* drawResultsCB = new TGCheckButton(getStripsEdgesMF);
		drawResultsCB->SetName("Draw Results CB");
		drawResultsCB->SetState(kButtonDown);

		getStripsEdgesMF->AddFrame(drawResultsLabel);
		getStripsEdgesMF->AddFrame(drawResultsCB);

		// ----------------------------------- //

		TGTextButton* processButton = new TGTextButton(getStripsEdgesMF, "Process",
				"GoddessCalib::ValidateGetStripsEdges(); GoddessCalib::FindWindowByName(\"Get Strips Edges\")->UnmapWindow();");
		TGTextButton* cancelButton = new TGTextButton(getStripsEdgesMF, "Cancel", "GoddessCalib::FindWindowByName(\"Get Strips Edges\")->UnmapWindow();");

		getStripsEdgesMF->AddFrame(processButton);
		getStripsEdgesMF->AddFrame(cancelButton);

		getStripsEdgesMF->MapSubwindows();
		getStripsEdgesMF->Resize(getStripsEdgesMF->GetDefaultSize());
		getStripsEdgesMF->MapWindow();
	}
	else if (!getStripsEdgesWin->IsMapped())
	{
		getStripsEdgesWin->MapSubwindows();
		getStripsEdgesWin->MapWindow();
	}
}

void GoddessCalib::OnClickAddToChain()
{
	TGMainFrame* mf = (TGMainFrame*) GoddessCalib::sinstance()->s_instance->GetControlFrame();

	TGTextEntry* tE;

	string treeName;

	tE = dynamic_cast<TGTextEntry*>(FindFrameByName(mf, "ChainTreeNameIF"));
	treeName = tE->GetText();

	GoddessCalib::sinstance()->calTreeName = treeName;

	auto lOK = gDirectory->GetListOfKeys();

	if (lOK->GetSize() > 0)
	{
		for (int i = 0; i < lOK->GetSize(); i++)
		{
			TTree* testTree = (TTree*) lOK->At(i);

			if (testTree != nullptr)
			{
				if (testTree->GetName() == GoddessCalib::sinstance()->calTreeName)
				{
					string currentFileName = gDirectory->GetName();

					GoddessCalib::sinstance()->AddFileToChain(currentFileName);

					cout << "Successfully added " << currentFileName << " to the TChain. New TChain content:\n";

					GoddessCalib::sinstance()->calChain->ls();

					return;
				}
			}
		}
	}
	else
	{
		cerr << "gDirectory is empty!! Nothing to add to the TChain... \n";
		return;
	}

	cerr << "No matching tree has been found in the current active file...\n";
	return;
}

void GoddessCalib::OnClickResetChain()
{
	GoddessCalib::sinstance()->ResetChain();
	return;
}

