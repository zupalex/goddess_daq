#include "GoddessCalib.h"

GoddessCalib* GoddessCalib::s_instance = 0;
GoddessCalib* gC;

char esc ( 27 );

GoddessCalib::GoddessCalib() : GoddessAnalysis()
{
    nGP.Init();

    calChain = nullptr;

    controlFrame = new TGMainFrame ( gClient->GetRoot(),200,200 );
    controlFrame->SetWindowName ( "Control Frame" );
    controlFrame->SetName ( "Control Frame" );

    controlFrame->Connect ( "Destroyed()", "GoddessCalib", sinstance(), "DeleteSInstance()" );

    // ------ Creating Main Vertical Frame ------ //

    TGVerticalFrame* vertFrame = new TGVerticalFrame ( controlFrame, 200, 200 );
    vertFrame->SetName ( "Main Vertical Frame" );
    vertFrame->SetBackgroundColor ( 0x4d004d );

    // ------ Adding the button to start the SX3 Calibration ------ //

    TGTextButton* sSX3CB = new TGTextButton ( vertFrame, "Start SuperX3 Calib", "GoddessCalib::StartSX3EnCalib()" );
    sSX3CB->SetName ( "Start SuperX3 Calib" );
    sSX3CB->SetFont ( "-*-helvetica-medium-r-*-*-20-*-*-*-*-*-iso8859-1" );

    vertFrame->AddFrame ( sSX3CB, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 100, 100, 20, 30 ) );

    // ------ Adding the input field for the name of the tree used in the user TChain ------ //

    TGHorizontalFrame* subTreeNameFrame = new TGHorizontalFrame ( vertFrame, 200, 400 );
    subTreeNameFrame->SetName ( "SubTreeNameFrame" );
    subTreeNameFrame->SetBackgroundColor ( 0x4d004d );

    TGLabel* tNLabel = new TGLabel ( subTreeNameFrame, "Tree Name:" );
    tNLabel->SetBackgroundColor ( 0x4d004d );
    tNLabel->SetTextColor ( 0xffffff );

    TGTextEntry* treeNameIF = new TGTextEntry ( subTreeNameFrame );
    treeNameIF->SetName ( "ChainTreeNameIF" );
    treeNameIF->Resize ( 130, 22 );
    treeNameIF->SetText ( "god" );

    subTreeNameFrame->AddFrame ( tNLabel, new TGLayoutHints ( kLHintsCenterX, 20, 2, 0, 0 ) );
    subTreeNameFrame->AddFrame ( treeNameIF, new TGLayoutHints ( kLHintsCenterX, 2, 20, 0, 0 ) );

    vertFrame->AddFrame ( subTreeNameFrame, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 0, 0, 0, 10 ) );

    // ------ Adding the button to add a file to the user TChain ------ //

    TGHorizontalFrame* subChainFrame = new TGHorizontalFrame ( vertFrame, 200, 400 );
    subChainFrame->SetName ( "SubChainFrame" );
    subChainFrame->SetBackgroundColor ( 0x4d004d );

    TGTextButton* aFTC = new TGTextButton ( subChainFrame, "Add current file to TChain", "GoddessCalib::OnClickAddToChain()" );
    aFTC->SetName ( "Add current file to chain" );

    TGTextButton* resetC = new TGTextButton ( subChainFrame, "Reset TChain", "GoddessCalib::OnClickResetChain()" );
    resetC->SetName ( "Reset TChain" );

    subChainFrame->AddFrame ( aFTC, new TGLayoutHints ( kLHintsCenterX, 20, 25, 0, 0 ) );
    subChainFrame->AddFrame ( resetC, new TGLayoutHints ( kLHintsCenterX, 25, 20, 0, 0 ) );

    vertFrame->AddFrame ( subChainFrame, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 0, 0, 0, 30 ) );

    // ------ Adding the first horizontal subframe for plotting the calib graphs ------ //

    TGHorizontalFrame* subHorzFrame1 = new TGHorizontalFrame ( vertFrame, 200, 400 );
    subHorzFrame1->SetName ( "SubHorizontalFrame1" );
    subHorzFrame1->SetBackgroundColor ( 0x4d004d );

    TGTextButton* pEnCalB = new TGTextButton ( subHorzFrame1, "Plot Energy Calib. Graphs", "GoddessCalib::OnClickPlotEnCalGraphs()" );
    pEnCalB->SetName ( "Plot Energy Calib. Graphs" );

    TGTextButton* pPosCalB = new TGTextButton ( subHorzFrame1, "Plot Pos. Calib Graphs", "GoddessCalib::OnClickPlotPosCalGraphs()" );
    pPosCalB->SetName ( "Plot Pos. Calib Graphs" );

    subHorzFrame1->AddFrame ( pEnCalB, new TGLayoutHints ( kLHintsCenterX, 20, 25, 0, 0 ) );
    subHorzFrame1->AddFrame ( pPosCalB, new TGLayoutHints ( kLHintsCenterX, 25, 20, 0, 0 ) );

    vertFrame->AddFrame ( subHorzFrame1, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 0, 0, 0, 20 ) );

    // ------ Adding the second horizontal subframe for writing and updating the calib parameters ------ //

    TGHorizontalFrame* subHorzFrame2 = new TGHorizontalFrame ( vertFrame, 200, 400 );
    subHorzFrame2->SetName ( "SubHorizontalFrame2" );
    subHorzFrame2->SetBackgroundColor ( 0x4d004d );

    TGTextButton* dCPB = new TGTextButton ( subHorzFrame2, "Reload Prev. Cal. Params", "GoddessCalib::OnClickParamsButtons(\"Reload\")" );
    dCPB->SetName ( "Reload Prev. Cal. Params" );

    TGTextButton* wCPB = new TGTextButton ( subHorzFrame2, "Write New Cal. Params", "GoddessCalib::OnClickParamsButtons(\"Write\")" );
    wCPB->SetName ( "Write New Cal. Params" );

    subHorzFrame2->AddFrame ( dCPB, new TGLayoutHints ( kLHintsCenterX, 20, 25, 0, 0 ) );
    subHorzFrame2->AddFrame ( wCPB, new TGLayoutHints ( kLHintsCenterX, 20, 25, 0, 0 ) );

    vertFrame->AddFrame ( subHorzFrame2, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 0, 0, 20, 10 ) );

    // ------ Adding text input field for the config file name ------ //

    TGTextButton* uCFB = new TGTextButton ( vertFrame, "Update Goddess Config File", "GoddessCalib::OnClickUpdateConf()" );
    uCFB->SetName ( "Update Goddess Config File" );
    vertFrame->AddFrame ( uCFB, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 0, 0, 40, 0 ) );

    PopupInputfield ( vertFrame, 400, 0, 12, "Config Full Name", new TGLayoutHints ( kLHintsCenterX, 15, 15, 0, 20 ), false );

    // ------ Adding the button to retreive the info from the TLines ------ //

    TGHorizontalFrame* subHorzFrame3 = new TGHorizontalFrame ( vertFrame, 200, 400 );
    subHorzFrame3->SetName ( "SubHorizontalFrame3" );
    subHorzFrame3->SetBackgroundColor ( 0x4d004d );

    TGTextButton* gCornerB = new TGTextButton ( subHorzFrame3, "Save TLines Info", "GoddessCalib::OnClickGetLinesInfo()" );
    gCornerB->SetName ( "Save TLines Info" );

    TGLabel* eAlphaLabel = new TGLabel ( subHorzFrame3, "Alpha Energy:" );
    eAlphaLabel->SetBackgroundColor ( 0x4d004d );
    eAlphaLabel->SetTextColor ( 0xffffff );

    TGNumberEntryField* alphaEn1IF = new TGNumberEntryField ( subHorzFrame3, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAPositive );
    alphaEn1IF->SetName ( "alphaEn1IF" );
    alphaEn1IF->Resize ( 90, 22 );

    subHorzFrame3->AddFrame ( eAlphaLabel, new TGLayoutHints ( kLHintsLeft, 10, 10, 5, 5 ) );
    subHorzFrame3->AddFrame ( alphaEn1IF, new TGLayoutHints ( kLHintsLeft, 10, 30, 5, 5 ) );

    subHorzFrame3->AddFrame ( gCornerB, new TGLayoutHints ( kLHintsRight, 10, 10, 5, 5 ) );

    vertFrame->AddFrame ( subHorzFrame3, new TGLayoutHints ( kLHintsCenterX, 0, 0, 40, 20 ) );

    // ------ Adding the button to perform the Position Calibration ------ //

    TGHorizontalFrame* subHorzFrame4 = new TGHorizontalFrame ( vertFrame, 200, 400 );
    subHorzFrame4->SetName ( "SubHorizontalFrame4" );
    subHorzFrame4->SetBackgroundColor ( 0x4d004d );

    TGTextButton* gEdgeB = new TGTextButton ( subHorzFrame4, "Get Strips Edges", "GoddessCalib::OnClickGetStripsEdges()" );
    gEdgeB->SetName ( "Save TLines Info" );

    TGLabel* fNameLabel = new TGLabel ( subHorzFrame4, "File:" );
    fNameLabel->SetBackgroundColor ( 0x4d004d );
    fNameLabel->SetTextColor ( 0xffffff );

    TGTextEntry* fNameIF = new TGTextEntry ( subHorzFrame4 );
    fNameIF->SetName ( "fNameIF" );
    fNameIF->Resize ( 130, 22 );
    fNameIF->SetText ( "Currently Opened File" );

    subHorzFrame4->AddFrame ( fNameLabel, new TGLayoutHints ( kLHintsLeft , 10, 10, 5, 5 ) );
    subHorzFrame4->AddFrame ( fNameIF, new TGLayoutHints ( kLHintsLeft , 10, 35, 5, 5 ) );

    subHorzFrame4->AddFrame ( gEdgeB, new TGLayoutHints ( kLHintsRight , 10, 10, 5, 5 ) );

    vertFrame->AddFrame ( subHorzFrame4, new TGLayoutHints ( kLHintsCenterX, 0, 0, 0, 70 ) );

    // ------ Wraping everything in the main frame ------ //

    controlFrame->AddFrame ( vertFrame );

    controlFrame->MapSubwindows();
    controlFrame->Resize ( controlFrame->GetDefaultSize() );
    controlFrame->MapWindow();

//     controlFrame->GetL

    currDetType = "";
    currRefEn1 = 0.0;

    SetReacParameters();

    cout << "You can use the newly create \"gC\" object to access the GoddessCalib class functions\n";
}

GoddessCalib::~GoddessCalib()
{

}

void GoddessCalib::DeleteSInstance()
{
    delete s_instance;
    s_instance = nullptr;
}

GoddessCalib* GoddessCalib::StartCalib()
{
    if ( s_instance == NULL )
        s_instance = new GoddessCalib();

    gC = s_instance;

    return s_instance;
}

TGWindow* GoddessCalib::FindWindowByName ( string winName )
{
    string altName = winName;

    while ( altName.find ( " " ) != string::npos )
    {
        altName = altName.replace ( altName.find ( " " ), 1, "" );
    }

    auto winList = gClient->GetListOfWindows();

    for ( int i = 0; i < winList->GetSize(); i++ )
    {
        if ( winList->At ( i )->GetName() == winName || winList->At ( i )->GetName() == altName ) return ( TGWindow* ) winList->At ( i );
    }

    return nullptr;
}

TGFrame* GoddessCalib::FindFrameByName ( TGCompositeFrame* pFrame, string frameName )
{
//     cout << "Scanning Frame " << pFrame->GetName() << "\n";

    string altName = frameName;

    while ( altName.find ( " " ) != string::npos )
    {
        altName = altName.replace ( altName.find ( " " ), 1, "" );
    }

    TIter nFrame ( pFrame->GetList() );

    TGFrameElement* frameEl;

    while ( ( frameEl = ( TGFrameElement* ) nFrame() ) )
    {
        if ( frameEl->fFrame->GetName() == frameName || frameEl->fFrame->GetName() == altName ) return frameEl->fFrame;

        if ( frameEl->fFrame->InheritsFrom ( TGCompositeFrame::Class() ) )
        {
            TGFrame* findInSubFrame = FindFrameByName ( ( TGCompositeFrame* ) frameEl->fFrame, frameName );

            if ( findInSubFrame != NULL ) return findInSubFrame;
        }
    }

    return nullptr;
}

void GoddessCalib::PopupInputfield ( TGWindow* pWin, float width_, float heigth_, short textSize, string label, TGLayoutHints* layHints, bool isEditable )
{
    TGGroupFrame* newGroup = new TGGroupFrame ( pWin, label.c_str(), kVerticalFrame );
    newGroup->SetTitlePos ( TGGroupFrame::kLeft );
    newGroup->SetBackgroundColor ( 0x4d004d );
    newGroup->SetTextColor ( 0xffffff );

    TGTextEntry* newTextEntry = new TGTextEntry ( newGroup );
    TGDimension defDim = newTextEntry->GetDefaultSize();
    newTextEntry->SetName ( label.c_str() );

    float newWidth = width_;
    float newHeigth = heigth_;

    if ( newWidth == 0 ) newWidth = defDim.fWidth;
    if ( newHeigth == 0 ) newHeigth = defDim.fHeight;

    newTextEntry->Resize ( newWidth, newHeigth );

    newTextEntry->SetFont ( Form ( "-*-helvetica-medium-r-*-*-%d-*-*-*-*-*-iso8859-1", textSize ) );

    if ( !isEditable )
    {
        newTextEntry->SetState ( kFALSE );
        newTextEntry->SetEditable ( true );
        newTextEntry->SetBackgroundColor ( 0x330033 );
    }

//     newTextEntry->ChangeOptions ( ( newTextEntry->GetOptions() & ~kVerticalFrame ) | ( newTextEntry->GetOptions() & ~kDoubleBorder ) | kTransientFrame );

    newGroup->AddFrame ( newTextEntry );

    TGMainFrame* mainFrameTest = dynamic_cast<TGMainFrame*> ( pWin );
    TGVerticalFrame* vertFrameTest = dynamic_cast<TGVerticalFrame*> ( pWin );
    TGHorizontalFrame* horzFrameTest = dynamic_cast<TGHorizontalFrame*> ( pWin );

    if ( mainFrameTest != NULL )
    {
        mainFrameTest->AddFrame ( newGroup, layHints );
    }
    else if ( vertFrameTest != NULL )
    {
        vertFrameTest->AddFrame ( newGroup, layHints );
    }
    else if ( horzFrameTest != NULL )
    {

        horzFrameTest->AddFrame ( newGroup, layHints );
    }

    return;
}

string GoddessCalib::GetAutoOutFileName ( string baseName )
{
    string currPath = ( string ) gSystem->pwd();

//     string rootFileName = currPath + "/Resistive_Strips_EnCal_Graphs_";

    string rootFileName = baseName;

    if ( calChain != nullptr )
    {
        if ( calChain->GetNtrees() == 1 )
        {
            string treeFName = calChain->GetFile()->GetName();

            std::size_t begRunName = treeFName.find ( "run" );
            std::size_t endRunName = treeFName.find ( "_", begRunName );

            if ( begRunName != string::npos && endRunName != string::npos ) rootFileName += "_" + treeFName.substr ( begRunName, endRunName-begRunName ) + ".root";
            else rootFileName += "_" + treeFName;
        }
        else if ( calChain->GetNtrees() >= 2 ) rootFileName += "_chain.root";
    }

    else
    {
        auto lOK = gDirectory->GetListOfKeys();

        if ( lOK != nullptr && lOK->GetSize() > 0 )
        {
            for ( int i = 0; i < lOK->GetSize(); i++ )
            {
//                 cout << lOK->At ( i )->GetName() << endl;

                TTree* testTree = ( TTree* ) lOK->At ( i );

                if ( testTree != nullptr )
                {
                    string treeFName = gDirectory->GetName();

                    std::size_t begRunName = treeFName.find ( "run" );
                    std::size_t endRunName = treeFName.find ( "_", begRunName );

                    if ( begRunName != string::npos && endRunName != string::npos ) rootFileName += "_" + treeFName.substr ( begRunName, endRunName-begRunName ) + ".root";
                    else rootFileName += "_" + treeFName;

                    break;
                }
            }
        }
    }

    outFileName = rootFileName;

    return rootFileName;
}

void GoddessCalib::ValidateParamsButtons ( string mode )
{
    TGWindow* paramsWin = FindWindowByName ( Form ( "%s Parameters", mode.c_str() ) );

    if ( paramsWin != NULL )
    {
        TGMainFrame* mf = ( TGMainFrame* ) paramsWin->GetMainFrame();

        TGTextEntry* tE;

        string fileName;

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "File name IF" ) );
        fileName = tE->GetText();

        if ( fileName.empty() ) fileName = "Resistive_Strips_Calib_Params.txt";

        if ( mode == "Reload" ) GoddessCalib::sinstance()->DumpFileToResCalMap ( fileName );
        else if ( mode == "Write" ) GoddessCalib::sinstance()->WriteResCalResults ( fileName );

        GoddessCalib::FindWindowByName ( Form ( "%s Parameters", mode.c_str() ) )->UnmapWindow();
    }
}

void GoddessCalib::OnClickParamsButtons ( string mode )
{
    TGWindow* paramsWin = FindWindowByName ( Form ( "%s Parameters", mode.c_str() ) );

    if ( paramsWin == NULL )
    {
        TGMainFrame* reloadParamsMF = new TGMainFrame ( gClient->GetRoot(), 200, 200 );
        reloadParamsMF->SetName ( Form ( "%s Parameters", mode.c_str() ) );
        reloadParamsMF->SetWindowName ( Form ( "%s Parameters", mode.c_str() ) );

        TGCompositeFrame* mainVertFrame = new TGCompositeFrame ( reloadParamsMF, 200, 200 );
        mainVertFrame->SetLayoutManager ( new TGColumnLayout ( mainVertFrame, 20 ) );

        TGCompositeFrame* firstFrame = new TGCompositeFrame ( mainVertFrame, 200, 200 );
        firstFrame->SetLayoutManager ( new TGRowLayout ( firstFrame, 10 ) );

        TGLabel* inputFileLabel = new TGLabel ( firstFrame, "File name:" );
        TGTextEntry* inputFileIF = new TGTextEntry ( firstFrame, "Resistive_Strips_Calib_Params.txt" );
        inputFileIF->SetName ( "File name IF" );
        inputFileIF->SetAlignment ( kTextRight );

        firstFrame->AddFrame ( inputFileLabel );
        firstFrame->AddFrame ( inputFileIF );

        mainVertFrame->AddFrame ( firstFrame );

        TGCompositeFrame* secondFrame = new TGCompositeFrame ( mainVertFrame, 200, 200 );
        secondFrame->SetLayoutManager ( new TGRowLayout ( secondFrame, 10 ) );

        string processCmd = Form ( "GoddessCalib::ValidateParamsButtons(\"%s\")", mode.c_str() );
        string cancelCmd = Form ( "GoddessCalib::FindWindowByName(\"%s Parameters\")->UnmapWindow();", mode.c_str() );

        TGTextButton* processButton = new TGTextButton ( secondFrame, "Process", processCmd.c_str() );
        TGTextButton* cancelButton = new TGTextButton ( secondFrame, "Cancel", cancelCmd.c_str() );

        secondFrame->AddFrame ( processButton );
        secondFrame->AddFrame ( cancelButton );

        mainVertFrame->AddFrame ( secondFrame );

        reloadParamsMF->AddFrame ( mainVertFrame, new TGLayoutHints ( kLHintsCenterX| kLHintsCenterY, 20, 20, 20, 20 ) );

        reloadParamsMF->MapSubwindows();
        reloadParamsMF->Resize ( reloadParamsMF->GetDefaultSize() );
        firstFrame->Resize ( reloadParamsMF->GetDefaultSize() );
        secondFrame->Resize ( reloadParamsMF->GetDefaultSize() );
        reloadParamsMF->MapWindow();
    }
    else if ( !paramsWin->IsMapped() )
    {
        paramsWin->MapSubwindows();
        paramsWin->MapWindow();
    }
}

void GoddessCalib::ValidatePlotEnCalGraphs()
{
    TGWindow* plotEnCalWin = FindWindowByName ( "Plot Energy Calibration Graphs" );

    if ( plotEnCalWin != NULL )
    {
        TGMainFrame* mf = ( TGMainFrame* ) plotEnCalWin->GetMainFrame();

        TGTextEntry* tE;
        TGCheckButton* cB;

        string treeName, entriesStr, sectorsStr, outFName;
        bool isUS;

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Tree Name IF" ) );
        treeName = tE->GetText();

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Entries IF" ) );
        entriesStr = tE->GetText();

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Sectors IF" ) );
        sectorsStr = tE->GetText();

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "OutFile Name IF" ) );
        outFName = tE->GetText();

        if ( !outFName.empty() )
        {
            GoddessCalib::sinstance()->outFileName =  outFName;
        }

        cB = dynamic_cast<TGCheckButton*> ( FindFrameByName ( mf, "Is Upstream CB" ) );
        isUS = cB->GetState();

        TChain* chain = GoddessCalib::sinstance()->calChain;

        if ( chain == NULL )
        {
            cout << "User Chain is empty. Trying to use the current opened file: " << gDirectory->GetName() << "\n";

            TTree* testTree = dynamic_cast<TTree*> ( gDirectory->FindObjectAny ( treeName.c_str() ) );

            if ( testTree != nullptr )
            {
                string completePTreePath = testTree->GetDirectory()->GetPath();
                if ( completePTreePath[completePTreePath.length()-1] != '/' ) completePTreePath += "/";
                completePTreePath += treeName;

                if ( completePTreePath.find ( ":" ) != string::npos ) completePTreePath.replace ( completePTreePath.find ( ":" ), 1, "" );

                cout << "Found matching tree in current file: " << testTree->GetDirectory()->GetPath() << "\n";

                GoddessCalib::sinstance()->calTreeName = treeName;

                GoddessCalib::sinstance()->AddFileToChain ( completePTreePath.c_str() );
                chain = GoddessCalib::sinstance()->calChain;
            }
        }

        if ( chain == NULL ) return;

        long long int nentries = stoul ( entriesStr );

        if ( nentries == 0 || nentries > chain->GetEntries() ) nentries = chain->GetEntries();

        GoddessCalib::sinstance()->PlotSX3ResStripsCalGraphsFromTree ( chain, nentries, isUS, DecodeNumberString ( sectorsStr ) );
    }
}

void GoddessCalib::OnClickPlotEnCalGraphs()
{
    TGWindow* plotEnCalWin = FindWindowByName ( "Plot Energy Calibration Graphs" );

    if ( plotEnCalWin == NULL )
    {
        string outFName = GoddessCalib::sinstance()->GetAutoOutFileName ( "Resistive_Strips_EnCal_Graphs" );

        TGMainFrame* plotEnCalMF = new TGMainFrame ( gClient->GetRoot(),100, 200 );
        plotEnCalMF->SetName ( "Plot Energy Calibration Graphs" );
        plotEnCalMF->SetWindowName ( "Plot Energy Calibration Graphs" );

        TGMatrixLayout* mLay = new TGMatrixLayout ( plotEnCalMF, 6, 2, 10, 10 );
        plotEnCalMF->SetLayoutManager ( mLay );

        TGLabel* treeNameLabel = new TGLabel ( plotEnCalMF, "Tree Name:" );
        TGTextEntry* treeNameIF = new TGTextEntry ( plotEnCalMF );
        treeNameIF->SetName ( "Tree Name IF" );
        treeNameIF->SetAlignment ( kTextRight );
        treeNameIF->SetText ( "sorted" );

        plotEnCalMF->AddFrame ( treeNameLabel );
        plotEnCalMF->AddFrame ( treeNameIF );

        TGLabel* nEntriesLabel = new TGLabel ( plotEnCalMF, "Entries to treat (0 == all):" );
        TGNumberEntryField* nEntriesIF = new TGNumberEntryField ( plotEnCalMF, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive );
        nEntriesIF->SetName ( "Entries IF" );

        plotEnCalMF->AddFrame ( nEntriesLabel );
        plotEnCalMF->AddFrame ( nEntriesIF );

        TGLabel* isUpstreamLabel = new TGLabel ( plotEnCalMF, "Upstream?" );
        TGCheckButton* isUpstreamCheckB = new TGCheckButton ( plotEnCalMF );
        isUpstreamCheckB->SetName ( "Is Upstream CB" );

        plotEnCalMF->AddFrame ( isUpstreamLabel );
        plotEnCalMF->AddFrame ( isUpstreamCheckB );

        TGLabel* sectorsLabel = new TGLabel ( plotEnCalMF, "Sectors to treat \ne.g. : \"1, 4-8, 10\"" );
        TGTextEntry* sectorsIF = new TGTextEntry ( plotEnCalMF );
        sectorsIF->SetName ( "Sectors IF" );
        sectorsIF->SetAlignment ( kTextRight );

        plotEnCalMF->AddFrame ( sectorsLabel );
        plotEnCalMF->AddFrame ( sectorsIF );

        TGLabel* outFileLabel = new TGLabel ( plotEnCalMF, "Output File Name:" );
        TGTextEntry* outFileIF = new TGTextEntry ( plotEnCalMF );
        TGDimension defDim = outFileIF->GetDefaultSize();
        outFileIF->SetName ( "OutFile Name IF" );
        outFileIF->SetAlignment ( kTextRight );
        outFileIF->Resize ( 300, defDim.fHeight );
        outFileIF->SetText ( outFName.c_str() );

        plotEnCalMF->AddFrame ( outFileLabel );
        plotEnCalMF->AddFrame ( outFileIF );

        TGTextButton* processButton = new TGTextButton ( plotEnCalMF, "Process", "GoddessCalib::ValidatePlotEnCalGraphs()" );
        TGTextButton* cancelButton = new TGTextButton ( plotEnCalMF, "Cancel", "GoddessCalib::FindWindowByName(\"Plot Energy Calibration Graphs\")->UnmapWindow();" );

        plotEnCalMF->AddFrame ( processButton );
        plotEnCalMF->AddFrame ( cancelButton );

        plotEnCalMF->MapSubwindows();
        plotEnCalMF->Resize ( plotEnCalMF->GetDefaultSize() );
        plotEnCalMF->MapWindow();
    }
    else if ( !plotEnCalWin->IsMapped() )
    {
        plotEnCalWin->MapSubwindows();
        plotEnCalWin->MapWindow();

        string outFName = GoddessCalib::sinstance()->GetAutoOutFileName ( "Resistive_Strips_EnCal_Graphs" );
        TGTextEntry* tE;
        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( plotEnCalWin, "OutFile Name IF" ) );
        tE->SetText ( outFName.c_str() );
    }
}

void GoddessCalib::ValidatePlotPosCalGraphs()
{
    TGWindow* plotPosCalWin = FindWindowByName ( "Plot Position Calibration Graphs" );

    if ( plotPosCalWin != NULL )
    {
        TGMainFrame* mf = ( TGMainFrame* ) plotPosCalWin->GetMainFrame();

        TGTextEntry* tE;
        TGCheckButton* cB;

        string treeName, sectorsStr, configName, outFName;
        long long int nentries;
        int nBinsX, nBinsY;
        float binMinX, binMaxX,binMinY, binMaxY;
        bool isUS;

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Tree Name IF" ) );
        treeName = tE->GetText();

        if ( treeName.empty() ) return;

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Entries IF" ) );
        nentries = std::stoul ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "NBinsX IF" ) );
        nBinsX = std::stoi ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "BinMinX IF" ) );
        binMinX = std::stof ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "BinMaxX IF" ) );
        binMaxX = std::stof ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "NBinsY IF" ) );
        nBinsY = std::stoi ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "BinMinY IF" ) );
        binMinY = std::stof ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "BinMaxY IF" ) );
        binMaxY = std::stof ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Sectors IF" ) );
        sectorsStr = tE->GetText();

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Config File IF" ) );
        configName = tE->GetText();

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "OutFile Name IF" ) );
        outFName = tE->GetText();

        if ( !outFName.empty() )
        {
            GoddessCalib::sinstance()->outFileName =  outFName;
        }

        cB = dynamic_cast<TGCheckButton*> ( FindFrameByName ( mf, "Is Upstream CB" ) );
        isUS = cB->GetState();

        DecodeNumberString ( sectorsStr );

        TChain* chain = GoddessCalib::sinstance()->calChain;

        if ( chain == NULL )
        {
            cout << "User Chain is empty. Trying to use the current opened file: " << gDirectory->GetName() << "\n";

            TTree* testTree = dynamic_cast<TTree*> ( gDirectory->FindObjectAny ( treeName.c_str() ) );

            if ( testTree != nullptr )
            {
                string completePTreePath = testTree->GetDirectory()->GetPath();
                if ( completePTreePath[completePTreePath.length()-1] != '/' ) completePTreePath += "/";
                completePTreePath += treeName;

                if ( completePTreePath.find ( ":" ) != string::npos ) completePTreePath.replace ( completePTreePath.find ( ":" ), 1, "" );

                cout << "Found matching tree in current file: " << testTree->GetDirectory()->GetPath() << "\n";

                GoddessCalib::sinstance()->calTreeName = treeName;

                GoddessCalib::sinstance()->AddFileToChain ( completePTreePath.c_str() );
                chain = GoddessCalib::sinstance()->calChain;
            }
        }

        if ( chain == NULL ) return;

        if ( nentries == 0 || nentries > chain->GetEntries() ) nentries = chain->GetEntries();

        GoddessCalib::sinstance()->DrawPosCalHistBatch ( chain, isUS, nentries, nBinsX, binMinX, binMaxX, nBinsY, binMinY, binMaxY, "", DecodeNumberString ( sectorsStr ), configName );
    }
}

void GoddessCalib::OnClickPlotPosCalGraphs()
{
    TGWindow* plotPosCalWin = FindWindowByName ( "Plot Position Calibration Graphs" );

    if ( plotPosCalWin == NULL )
    {
        string outFName = GoddessCalib::sinstance()->GetAutoOutFileName ( "Resistive_Strips_PosCal_Graphs" );

        Pixel_t bcol = 0xb3d1ff;

        TGMainFrame* plotPosCalMF = new TGMainFrame ( gClient->GetRoot(),100, 200 );
        plotPosCalMF->SetName ( "Plot Position Calibration Graphs" );
        plotPosCalMF->SetWindowName ( "Plot Position Calibration Graphs" );
        plotPosCalMF->SetBackgroundColor ( bcol );

//         TGMatrixLayout* mLay = new TGMatrixLayout ( plotPosCalMF, 7, 2, 10, 10 );
//         plotPosCalMF->SetLayoutManager ( new TGColumnLayout ( plotPosCalMF ) );

        TGCompositeFrame* firstFrame = new TGCompositeFrame ( plotPosCalMF, 100, 100 );
        firstFrame->SetLayoutManager ( new TGMatrixLayout ( firstFrame, 3, 2, 5, 5 ) );
        firstFrame->SetBackgroundColor ( bcol );

        TGLabel* treeNameLabel = new TGLabel ( firstFrame, "Tree Name:" );
        treeNameLabel->SetBackgroundColor ( bcol );
        TGTextEntry* treeNameIF = new TGTextEntry ( firstFrame );
        treeNameIF->SetName ( "Tree Name IF" );
        treeNameIF->SetAlignment ( kTextRight );
        treeNameIF->SetText ( "sorted" );

        firstFrame->AddFrame ( treeNameLabel );
        firstFrame->AddFrame ( treeNameIF );

        TGLabel* nEntriesLabel = new TGLabel ( firstFrame, "Entries to treat (0 == all):" );
        nEntriesLabel->SetBackgroundColor ( bcol );
        TGNumberEntryField* nEntriesIF = new TGNumberEntryField ( firstFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive );
        nEntriesIF->SetName ( "Entries IF" );
        nEntriesIF->SetNumber ( 0 );

        firstFrame->AddFrame ( nEntriesLabel );
        firstFrame->AddFrame ( nEntriesIF );

        TGLabel* isUpstreamLabel = new TGLabel ( firstFrame, "Upstream?" );
        isUpstreamLabel->SetBackgroundColor ( bcol );
        TGCheckButton* isUpstreamCheckB = new TGCheckButton ( firstFrame );
        isUpstreamCheckB->SetName ( "Is Upstream CB" );
        isUpstreamCheckB->SetBackgroundColor ( bcol );

        firstFrame->AddFrame ( isUpstreamLabel );
        firstFrame->AddFrame ( isUpstreamCheckB );

        plotPosCalMF->AddFrame ( firstFrame, new TGLayoutHints ( kLHintsCenterX, 0, 0, 0, 0 ) );

        TGCompositeFrame* secondFrame = new TGCompositeFrame ( plotPosCalMF, 100, 100 );
        secondFrame->SetLayoutManager ( new TGMatrixLayout ( secondFrame, 2, 4, 5, 0 ) );
        secondFrame->SetBackgroundColor ( bcol );

        TGLabel* xAxisBinsLabel = new TGLabel ( secondFrame, "X Axis Binning (nbins, min, max)" );
        xAxisBinsLabel->SetBackgroundColor ( bcol );

        TGNumberEntryField* nbinsXIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber );
        TGDimension defDim = nbinsXIF->GetDefaultSize();
        nbinsXIF->SetName ( "NBinsX IF" );
        nbinsXIF->SetNumber ( 200 );
        TGNumberEntryField* binMinXIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber );
        binMinXIF->SetName ( "BinMinX IF" );
        binMinXIF->SetNumber ( -1 );
        TGNumberEntryField* binMaxXIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber );
        binMaxXIF->SetName ( "BinMaxX IF" );
        binMaxXIF->SetNumber ( 1 );

        nbinsXIF->Resize ( 100, defDim.fHeight );
        binMinXIF->Resize ( 100, defDim.fHeight );
        binMaxXIF->Resize ( 100, defDim.fHeight );

        secondFrame->AddFrame ( xAxisBinsLabel );
        secondFrame->AddFrame ( nbinsXIF );
        secondFrame->AddFrame ( binMinXIF );
        secondFrame->AddFrame ( binMaxXIF );

        TGLabel* yAxisBinsLabel = new TGLabel ( secondFrame, "Y Axis Binning (nbins, min, max)" );
        yAxisBinsLabel->SetBackgroundColor ( bcol );

        TGNumberEntryField* nbinsYIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber );
        nbinsYIF->SetName ( "NBinsY IF" );
        nbinsYIF->SetNumber ( 500 );
        TGNumberEntryField* binMinYIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber );
        binMinYIF->SetName ( "BinMinY IF" );
        binMinYIF->SetNumber ( 0 );
        TGNumberEntryField* binMaxYIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber );
        binMaxYIF->SetName ( "BinMaxY IF" );
        binMaxYIF->SetNumber ( 10 );

        nbinsYIF->Resize ( 100, defDim.fHeight );
        binMinYIF->Resize ( 100, defDim.fHeight );
        binMaxYIF->Resize ( 100, defDim.fHeight );

        secondFrame->AddFrame ( yAxisBinsLabel );
        secondFrame->AddFrame ( nbinsYIF );
        secondFrame->AddFrame ( binMinYIF );
        secondFrame->AddFrame ( binMaxYIF );

        plotPosCalMF->AddFrame ( secondFrame, new TGLayoutHints ( kLHintsCenterX, 0, 0, 0, 0 ) );

        TGCompositeFrame* thirdFrame = new TGCompositeFrame ( plotPosCalMF, 100, 100 );
        thirdFrame->SetLayoutManager ( new TGRowLayout ( thirdFrame, 10 ) );
        thirdFrame->SetBackgroundColor ( bcol );

        TGLabel* sectorsLabel = new TGLabel ( thirdFrame, "Sectors to treat \ne.g. : \"1, 4-8, 10\"" );
        sectorsLabel->SetBackgroundColor ( bcol );
        TGTextEntry* sectorsIF = new TGTextEntry ( thirdFrame );
        sectorsIF->SetName ( "Sectors IF" );
        sectorsIF->SetAlignment ( kTextRight );

        thirdFrame->AddFrame ( sectorsLabel );
        thirdFrame->AddFrame ( sectorsIF );

        plotPosCalMF->AddFrame ( thirdFrame, new TGLayoutHints ( kLHintsCenterX, 0, 0, 10, 0 ) );

        TGCompositeFrame* fourthFrame = new TGCompositeFrame ( plotPosCalMF, 100, 100 );
        fourthFrame->SetLayoutManager ( new TGRowLayout ( fourthFrame, 10 ) );
        fourthFrame->SetBackgroundColor ( bcol );

        TGLabel* configFileLabel = new TGLabel ( fourthFrame, "Config File to use: " );
        configFileLabel->SetBackgroundColor ( bcol );
        TGTextEntry* configFileIF = new TGTextEntry ( fourthFrame );
        configFileIF->SetName ( "Config File IF" );
        configFileIF->SetAlignment ( kTextRight );
        configFileIF->Resize ( 500, defDim.fHeight );

        fourthFrame->AddFrame ( configFileLabel );
        fourthFrame->AddFrame ( configFileIF );

        plotPosCalMF->AddFrame ( fourthFrame, new TGLayoutHints ( kLHintsCenterX, 0, 0, 10, 0 ) );

        TGCompositeFrame* outFileFrame = new TGCompositeFrame ( plotPosCalMF, 100, 100 );
        outFileFrame->SetLayoutManager ( new TGRowLayout ( outFileFrame, 10 ) );
        outFileFrame->SetBackgroundColor ( bcol );

        TGLabel* outFileLabel = new TGLabel ( outFileFrame, "Output File Name:" );
        outFileLabel->SetBackgroundColor ( bcol );
        TGTextEntry* outFileIF = new TGTextEntry ( outFileFrame );
        outFileIF->SetName ( "OutFile Name IF" );
        outFileIF->SetAlignment ( kTextRight );
        outFileIF->Resize ( 500, defDim.fHeight );
        outFileIF->SetText ( outFName.c_str() );

        outFileFrame->AddFrame ( outFileLabel );
        outFileFrame->AddFrame ( outFileIF );

        plotPosCalMF->AddFrame ( outFileFrame, new TGLayoutHints ( kLHintsCenterX, 0, 0, 20, 0 ) );

        TGCompositeFrame* fifthFrame = new TGCompositeFrame ( plotPosCalMF, 100, 100 );
        fifthFrame->SetLayoutManager ( new TGRowLayout ( fifthFrame, 10 ) );
        fifthFrame->SetBackgroundColor ( bcol );

        TGTextButton* processButton = new TGTextButton ( fifthFrame, "Process", "GoddessCalib::ValidatePlotPosCalGraphs()" );
        processButton->SetBackgroundColor ( bcol );
        TGTextButton* cancelButton = new TGTextButton ( fifthFrame, "Cancel", "GoddessCalib::FindWindowByName(\"Plot Position Calibration Graphs\")->UnmapWindow();" );
        cancelButton->SetBackgroundColor ( bcol );

        fifthFrame->AddFrame ( processButton );
        fifthFrame->AddFrame ( cancelButton );

        plotPosCalMF->AddFrame ( fifthFrame, new TGLayoutHints ( kLHintsCenterX, 0, 0, 20, 0 ) );

        plotPosCalMF->MapSubwindows();
        plotPosCalMF->Resize ( plotPosCalMF->GetDefaultSize() );
        plotPosCalMF->MapWindow();
    }
    else if ( !plotPosCalWin->IsMapped() )
    {
        plotPosCalWin->MapSubwindows();
        plotPosCalWin->MapWindow();

        string outFName = GoddessCalib::sinstance()->GetAutoOutFileName ( "Resistive_Strips_PosCal_Graphs" );
        TGTextEntry* tE;
        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( plotPosCalWin, "OutFile Name IF" ) );
        tE->SetText ( outFName.c_str() );
    }
}

void GoddessCalib::OnClickUpdateConf()
{
    TGTextEntry* configNameIF = dynamic_cast<TGTextEntry*> ( FindFrameByName ( GoddessCalib::sinstance()->GetControlFrame(), "Config Full Name" ) );

//     if(configNameIF != NULL) cout << "Found the Input Field for the Config Name...\n";

    if ( !configNameIF->IsEnabled() )
    {
//         cout << "Config Name Input Field was disabled. Enabling it...\n";

        configNameIF->SetState ( kTRUE );
        configNameIF->SetEnabled ( kTRUE );
    }
    else
    {
        string confName = configNameIF->GetText();

        configNameIF->SetText ( "" );

//         cout << "Config Name Input Field was enabled. Read " << confName << "\n";

        if ( confName.length() > 0 ) GoddessCalib::sinstance()->UpdateParamsInConf ( confName, "superX3" );

        configNameIF->SetState ( kFALSE );
        configNameIF->SetEnabled ( kFALSE );
        configNameIF->SetBackgroundColor ( 0x330033 );
    }

    return;
}

void GoddessCalib::OnClickGetLinesInfo ()
{
    if ( GoddessCalib::sinstance() == NULL ) return;

    short isUS_ = -1, sect_ = -1, strip_ = -1;

    if ( gPad == NULL || gPad->GetCanvas() == NULL )
    {
        cerr << "No Canvas available!\n";
        cerr << "To get help with the energy calibration procedure, type GoddessCalib::EnCalibHelp()\n";
        return;
    }

    auto canPrimList = gPad->GetCanvas()->GetListOfPrimitives();

    TH2F* gr;

    string grName_ = "";

    for ( int i = 0; i < canPrimList->GetSize(); i++ )
    {
        gr = dynamic_cast<TH2F*> ( canPrimList->At ( i ) );

        if ( gr != nullptr )
        {
            grName_ = gr->GetTitle();
            break;
        }
    }

    if ( gr != nullptr )
    {
        std::size_t sectPos = grName_.find ( "sector" );
        std::size_t stripPos = grName_.find ( "strip" );
        std::size_t separatorPos = grName_.find ( "_" );

        if ( sectPos != string::npos && stripPos != string::npos && separatorPos != string::npos )
        {
            if ( grName_[sectPos+6] == 'U' ) isUS_ = 1;
            else if ( grName_[sectPos+6] == 'D' ) isUS_ = 0;
            else return;

            sect_ = std::stoi ( grName_.substr ( sectPos+7, separatorPos ) );
            strip_ = std::stoi ( grName_.substr ( stripPos+5 ) );
        }
    }

    if ( isUS_ == -1 || sect_ == -1 || strip_ == -1 )
    {
        cerr << esc << "[38;2;255;0;0m" << "The graph used do not fulfill the title requirement! Did you produce it with the GoddessCalib class built-in functions?" << esc << "[0m \n";
        cerr << esc << "[38;2;255;0;0m" << "To get help with the energy calibration procedure, type GoddessCalib::EnCalibHelp()" << esc << "[0m \n";
        return;
    }

    TGNumberEntryField* alphaEnIF = ( TGNumberEntryField* ) FindWindowByName ( "alphaEn1IF" );

    float alphaEn1 = std::stof ( alphaEnIF->GetText() );

    GoddessCalib::sinstance()->currRefEn1 = alphaEn1;

    GoddessCalib::sinstance()->GetCornersCoordinates ( gPad->GetCanvas(), ( bool ) isUS_, ( short unsigned int ) sect_, ( short unsigned int ) strip_, GoddessCalib::sinstance()->currDetType, GoddessCalib::sinstance()->currRefEn1 );

    cout << endl;

    return;
}

void GoddessCalib::ValidateGetStripsEdges()
{
    if ( GoddessCalib::sinstance() == NULL ) return;

    TGTextEntry* fileNameIF = ( TGTextEntry* ) FindFrameByName ( GoddessCalib::sinstance()->s_instance->GetControlFrame(), "fNameIF" );

    string fileName = fileNameIF->GetText();

    TFile *input;

    if ( fileName.length() > 0 || fileName == "Currently Opened File" )
    {
        input = gDirectory->GetFile();
    }
    else
    {
        input = new TFile ( fileName.c_str(), "read" );
    }

    if ( input == NULL || !input->IsOpen() )
    {
        cerr << "Invalid input file name..." << endl;
        return;
    }

    cout << "Opened file: " << input->GetName() << "\n";

    TGNumberEntryField* alphaEn1IF = ( TGNumberEntryField* ) FindFrameByName ( GoddessCalib::sinstance()->s_instance->GetControlFrame(), "alphaEn1IF" );

    double alphaEn1 = alphaEn1IF->GetNumber();

    TGWindow* prompt = FindWindowByName ( "Get Strips Edges" );

    TGCheckButton* drawResCB = ( TGCheckButton* ) FindFrameByName ( prompt, "Draw Results CB" );
    TGNumberEntryField* projWidthIF = ( TGNumberEntryField* ) FindFrameByName ( prompt, "Proj Width IF" );
    TGNumberEntryField* projThrIF = ( TGNumberEntryField* ) FindFrameByName ( prompt, "Proj Threshold IF" );

    TGTextEntry* sectorsIF = ( TGTextEntry* ) FindFrameByName ( prompt, "Sectors GSE IF" );

    TGNumberEntryField* projMinIF = ( TGNumberEntryField* ) FindFrameByName ( prompt, "Proj Win Min IF" );
    TGNumberEntryField* projMaxIF = ( TGNumberEntryField* ) FindFrameByName ( prompt, "Proj Win Max IF" );

    TGNumberEntryField* shiftGrNPointsIF = ( TGNumberEntryField* ) FindFrameByName ( prompt, "Shift Graph NPoints IF" );

    int projWidth_ = projWidthIF->GetNumber();
    double thr_ = projThrIF->GetNumber();

    double projWinMin_ = projMinIF->GetNumber();
    double projWinMax_ = projMaxIF->GetNumber();

    int shiftGrNPoints_ = shiftGrNPointsIF->GetNumber();

    bool doDraw = drawResCB->GetState();
    string sectorsList = sectorsIF->GetText();

    if ( projWidthIF->IsEnabled() && projThrIF->IsEnabled() ) GoddessCalib::sinstance()->GetStripsEdges ( input, sectorsList, projWidth_, thr_, alphaEn1, shiftGrNPoints_, doDraw );
    else if ( projMinIF->IsEnabled() && projMaxIF->IsEnabled() ) GoddessCalib::sinstance()->GetStripsEdges ( input, sectorsList, projWinMin_, projWinMax_, alphaEn1, thr_, shiftGrNPoints_, doDraw );

    return;
}

void GoddessCalib::SetGSEAutoMode()
{
    TGWindow* getStripsEdgesWin = FindWindowByName ( "Get Strips Edges" );

    if ( getStripsEdgesWin != NULL )
    {
        TGNumberEntryField* projWidthIF = ( TGNumberEntryField* ) FindFrameByName ( getStripsEdgesWin, "Proj Width IF" );

        TGNumberEntryField* projMinIF = ( TGNumberEntryField* ) FindFrameByName ( getStripsEdgesWin, "Proj Win Min IF" );
        TGNumberEntryField* projMaxIF = ( TGNumberEntryField* ) FindFrameByName ( getStripsEdgesWin, "Proj Win Max IF" );

        if ( projWidthIF != nullptr )
        {
            projWidthIF->SetState ( kTRUE );
            projWidthIF->SetEnabled ( kTRUE );
            projWidthIF->SetBackgroundColor ( 0xffffff );
        }

        if ( projMinIF != nullptr )
        {
            projMinIF->SetState ( kFALSE );
            projMinIF->SetEnabled ( kFALSE );
            projMinIF->SetBackgroundColor ( 0x424949 );
        }

        if ( projMaxIF != nullptr )
        {
            projMaxIF->SetState ( kFALSE );
            projMaxIF->SetEnabled ( kFALSE );
            projMaxIF->SetBackgroundColor ( 0x424949 );
        }
    }
}

void GoddessCalib::SetGSEFixedWinMode()
{
    TGWindow* getStripsEdgesWin = FindWindowByName ( "Get Strips Edges" );

    if ( getStripsEdgesWin != NULL )
    {
        TGNumberEntryField* projWidthIF = ( TGNumberEntryField* ) FindFrameByName ( getStripsEdgesWin, "Proj Width IF" );

        TGNumberEntryField* projMinIF = ( TGNumberEntryField* ) FindFrameByName ( getStripsEdgesWin, "Proj Win Min IF" );
        TGNumberEntryField* projMaxIF = ( TGNumberEntryField* ) FindFrameByName ( getStripsEdgesWin, "Proj Win Max IF" );

        if ( projWidthIF != nullptr )
        {
            projWidthIF->SetState ( kFALSE );
            projWidthIF->SetEnabled ( kFALSE );
            projWidthIF->SetBackgroundColor ( 0x424949 );
        }

        if ( projMinIF != nullptr )
        {
            projMinIF->SetState ( kTRUE );
            projMinIF->SetEnabled ( kTRUE );
            projMinIF->SetBackgroundColor ( 0xffffff );
        }

        if ( projMaxIF != nullptr )
        {
            projMaxIF->SetState ( kTRUE );
            projMaxIF->SetEnabled ( kTRUE );
            projMaxIF->SetBackgroundColor ( 0xffffff );
        }
    }
}

void GoddessCalib::OnClickGetStripsEdges()
{
    TGWindow* getStripsEdgesWin = FindWindowByName ( "Get Strips Edges" );

    if ( getStripsEdgesWin == NULL )
    {
        TGMainFrame* getStripsEdgesMF = new TGMainFrame ( gClient->GetRoot(),100, 100 );
        getStripsEdgesMF->SetName ( "Get Strips Edges" );
        getStripsEdgesMF->SetWindowName ( "Get Strips Edges" );

        getStripsEdgesMF->SetLayoutManager ( new TGMatrixLayout ( getStripsEdgesMF, 9, 2, 5, 5 ) );

        TGTextButton* autoModeButton = new TGTextButton ( getStripsEdgesMF, "Automatic Mode", "GoddessCalib::SetGSEAutoMode();" );
        TGTextButton* fixedWinMode = new TGTextButton ( getStripsEdgesMF, "Fixed Window Mode", "GoddessCalib::SetGSEFixedWinMode();" );

        getStripsEdgesMF->AddFrame ( autoModeButton );
        getStripsEdgesMF->AddFrame ( fixedWinMode );

        // ----------------------------------- //

        TGLabel* projWidthLabel = new TGLabel ( getStripsEdgesMF, "Proj. Width (bins):" );
        TGNumberEntryField* projWidthIF = new TGNumberEntryField ( getStripsEdgesMF, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive );
        projWidthIF->SetName ( "Proj Width IF" );
        projWidthIF->SetNumber ( 20 );

        getStripsEdgesMF->AddFrame ( projWidthLabel );
        getStripsEdgesMF->AddFrame ( projWidthIF );

        TGLabel* projThrLabel = new TGLabel ( getStripsEdgesMF, "Max Detection Threshold:" );
        TGNumberEntryField* projThrIF = new TGNumberEntryField ( getStripsEdgesMF, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive );
        projThrIF->SetName ( "Proj Threshold IF" );
        projThrIF->SetNumber ( 3.0 );

        getStripsEdgesMF->AddFrame ( projThrLabel );
        getStripsEdgesMF->AddFrame ( projThrIF );

        // ----------------------------------- //

        TGLabel* projMinLabel = new TGLabel ( getStripsEdgesMF, "Proj. Window Min:" );
        TGNumberEntryField* projMinIF = new TGNumberEntryField ( getStripsEdgesMF, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAPositive );
        projMinIF->SetName ( "Proj Win Min IF" );
        projMinIF->SetNumber ( 5.5 );
        projMinIF->SetState ( kFALSE );
        projMinIF->SetEnabled ( kFALSE );
        projMinIF->SetBackgroundColor ( 0x424949 );

        getStripsEdgesMF->AddFrame ( projMinLabel );
        getStripsEdgesMF->AddFrame ( projMinIF );

        TGLabel* projMaxLabel = new TGLabel ( getStripsEdgesMF, "Proj. Window Max:" );
        TGNumberEntryField* projMaxIF = new TGNumberEntryField ( getStripsEdgesMF, -1, 0, TGNumberFormat::kNESReal, TGNumberFormat::kNEAPositive );
        projMaxIF->SetName ( "Proj Win Max IF" );
        projMaxIF->SetNumber ( 6.2 );
        projMaxIF->SetState ( kFALSE );
        projMaxIF->SetEnabled ( kFALSE );
        projMaxIF->SetBackgroundColor ( 0x424949 );

        getStripsEdgesMF->AddFrame ( projMaxLabel );
        getStripsEdgesMF->AddFrame ( projMaxIF );

        // ----------------------------------- //

        TGLabel* shiftGrNPointsLabel = new TGLabel ( getStripsEdgesMF, "Energy shift vs. position correction :\n           number of points           " );
        TGNumberEntryField* shiftGrNPointsIF = new TGNumberEntryField ( getStripsEdgesMF, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive );
        shiftGrNPointsIF->SetName ( "Shift Graph NPoints IF" );
        shiftGrNPointsIF->SetNumber ( 60 );

        getStripsEdgesMF->AddFrame ( shiftGrNPointsLabel );
        getStripsEdgesMF->AddFrame ( shiftGrNPointsIF );

        // ----------------------------------- //

        TGLabel* sectorsLabel = new TGLabel ( getStripsEdgesMF, "Sectors to treat \ne.g. : \"1, 4-8, 10\"" );
        TGTextEntry* sectorsGSEIF = new TGTextEntry ( getStripsEdgesMF );
        sectorsGSEIF->SetName ( "Sectors GSE IF" );
        sectorsGSEIF->SetAlignment ( kTextRight );

        getStripsEdgesMF->AddFrame ( sectorsLabel );
        getStripsEdgesMF->AddFrame ( sectorsGSEIF );

        // ----------------------------------- //

        TGLabel* drawResultsLabel = new TGLabel ( getStripsEdgesMF, "Draw Results?" );
        TGCheckButton* drawResultsCB = new TGCheckButton ( getStripsEdgesMF );
        drawResultsCB->SetName ( "Draw Results CB" );
        drawResultsCB->SetState ( kButtonDown );

        getStripsEdgesMF->AddFrame ( drawResultsLabel );
        getStripsEdgesMF->AddFrame ( drawResultsCB );

        // ----------------------------------- //

        TGTextButton* processButton = new TGTextButton ( getStripsEdgesMF, "Process", "GoddessCalib::ValidateGetStripsEdges(); GoddessCalib::FindWindowByName(\"Get Strips Edges\")->UnmapWindow();" );
        TGTextButton* cancelButton = new TGTextButton ( getStripsEdgesMF, "Cancel", "GoddessCalib::FindWindowByName(\"Get Strips Edges\")->UnmapWindow();" );

        getStripsEdgesMF->AddFrame ( processButton );
        getStripsEdgesMF->AddFrame ( cancelButton );

        getStripsEdgesMF->MapSubwindows();
        getStripsEdgesMF->Resize ( getStripsEdgesMF->GetDefaultSize() );
        getStripsEdgesMF->MapWindow();
    }
    else if ( !getStripsEdgesWin->IsMapped() )
    {
        getStripsEdgesWin->MapSubwindows();
        getStripsEdgesWin->MapWindow();
    }
}

void GoddessCalib::StartSX3EnCalib ( string detectorType, double refEnergy1 )
{
    if ( s_instance == NULL ) return;

    s_instance->currDetType = detectorType;
    s_instance->currRefEn1 = refEnergy1;

    if ( gPad == NULL || gPad->GetCanvas() == NULL ) new TBrowser();

    gStyle->SetLineWidth ( 2 );
    gStyle->SetLineColor ( 2 );

//     gStyle->SetMarkerColor ( 4 );
//     gStyle->SetMarkerSize ( 2 );
//     gStyle->SetMarkerStyle ( 3 );

    TGNumberEntryField* alphaEnIF = ( TGNumberEntryField* ) FindWindowByName ( "alphaEn1IF" );

    alphaEnIF->SetText ( Form ( "%.3f", refEnergy1 ) );
}

void GoddessCalib::OnClickAddToChain()
{
    TGMainFrame* mf = ( TGMainFrame* ) GoddessCalib::sinstance()->s_instance->GetControlFrame();

    TGTextEntry* tE;

    string treeName;

    tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "ChainTreeNameIF" ) );
    treeName = tE->GetText();

    GoddessCalib::sinstance()->calTreeName = treeName;

    auto lOK = gDirectory->GetListOfKeys();

    if ( lOK->GetSize() > 0 )
    {
        for ( int i = 0; i < lOK->GetSize(); i++ )
        {
            TTree* testTree = ( TTree* ) lOK->At ( i );

            if ( testTree != nullptr )
            {
                if ( testTree->GetName() == GoddessCalib::sinstance()->calTreeName )
                {
                    string currentFileName = gDirectory->GetName();

                    GoddessCalib::sinstance()->AddFileToChain ( currentFileName );

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

void GoddessCalib::AddFileToChain ( string fileName )
{
    if ( calChain == nullptr ) calChain = new TChain ( calTreeName.c_str(), calTreeName.c_str() );

    calChain->Add ( fileName.c_str() );

    return;
}

void GoddessCalib::ResetChain()
{
    calChain = nullptr;

    return;
}

void GoddessCalib::ReadConfigCalPars ( string configFileName )
{
    std::ifstream config;
    config.open ( configFileName.c_str(), std::ios::in );

    if ( !config.is_open() )
    {
        cerr << "Failed to open file: " << configFileName << " ...\n";
        return;
    }

    string readLine, dummy, detType, detID, lineID, stripType;
    int stripNbr;
    float offset, slope;
    float leftEdge, rightEdge;
    std::istringstream iss;

    while ( std::getline ( config, readLine ) )
    {
        if ( readLine.empty() || readLine == "\n" ) continue;

        iss.clear();
        iss.str ( readLine );

        if ( readLine.find ( "QQQ5" ) != string::npos || readLine.find ( "superX3" ) != string::npos )
        {
            iss >> detType >> dummy >> detID;

            detID = detID.substr ( 0, detID.find ( "-" ) );

            if ( detType == "superX3" ) detType = "SuperX3";
        }
        else
        {
            iss >> lineID;

            if ( lineID == "enCal" )
            {
                iss >> stripType >> stripNbr;

                if ( detType == "QQQ5" )
                {
                    if ( stripType == "p" ) stripType = "front";
                    else stripType = "back";
                }
                else if ( detType == "SuperX3" )
                {
                    if ( stripType == "resStrip" ) stripType = "front";
                    else if ( stripType == "n" ) stripType = "back";
                    else
                    {
                        if ( stripNbr == 0 || stripNbr == 2 || stripNbr == 5 || stripNbr == 7 ) stripType = "near";
                        else stripType = "far";

                        stripNbr /= 2;
                    }
                }

                iss >> offset >> slope;

                string detKey = detType + " " + detID + " " + stripType + " " + Form ( "%d", stripNbr );

                configCalPars[detKey] = std::make_pair ( offset, slope );
            }

            else if ( lineID == "posCal" )
            {
                iss >> stripType >> stripNbr >> leftEdge >> rightEdge;

                string detKey = detType + " " + detID + " position " + Form ( "%d", stripNbr );

                configCalPars[detKey] = std::make_pair ( leftEdge, rightEdge );
            }
        }
    }

    return;
}

void GoddessCalib::InitializeCalMapKey ( string mapKey, unsigned short strip )
{
    auto itr = resStripsCalMap.find ( mapKey );

    if ( itr == resStripsCalMap.end() )
    {
        cout << "No entry found for " << mapKey << " strip #" << strip << " in current session's map. Initializing it with -100" << endl;

        for ( int i = 0; i < 6; i++ )
        {
            resStripsCalMap[mapKey][strip].push_back ( -100 );
        }
    }
    else
    {
        auto itrStr = resStripsCalMap[mapKey].find ( strip );

        if ( itrStr == resStripsCalMap[mapKey].end() )
        {
            cout << "No entry found for " << mapKey << " strip #" << strip << ". Initializing it with -100" << endl;

            for ( int i = 0; i < 6; i++ )
            {
                resStripsCalMap[mapKey][strip].push_back ( -100 );
            }
        }
    }
}

void GoddessCalib::GetCornersCoordinates ( TCanvas* can, bool isUpstream, unsigned short sector, unsigned short strip, string detectorType, double refEnergy1 )
{
    string calMapKey = detectorType;
    calMapKey += isUpstream ? " U" : " D";
    calMapKey += std::to_string ( sector );

    TList* list = can->GetListOfPrimitives();

    int listSize = list->GetSize();

    TLine* negLine = 0;
    TLine* topLine = 0;
    TLine* botLine = 0;

    double tempSlope = 0;
    TLine* tempLine = 0;

    for ( int i = 0; i < listSize; i++ )
    {
        TObject* child = list->At ( i );

        TLine* line = dynamic_cast<TLine*> ( child );

        if ( line == nullptr ) continue;

        double x1 = line->GetX1();
        double y1 = line->GetY1();
        double x2 = line->GetX2();
        double y2 = line->GetY2();

        double slope = ( y2 - y1 ) / ( x2 - x1 );

//         cout << "Found a line in the list at position #" << i << endl;
//         cout << "X1 : " << x1 << "   /   Y1 : " << y1 << endl;
//         cout << "X2 : " << x2 << "   /   Y2 ; " << y2 << endl;
//
//         cout << "Slope = " << slope << endl;

        if ( slope < 0 ) negLine = line;
        else
        {
            if ( tempLine == nullptr )
            {
                tempSlope = slope;
                tempLine = line;
            }
            else
            {
                if ( slope > tempSlope )
                {
                    topLine = line;
                    botLine = tempLine;
                }
                else
                {
                    botLine = line;
                    topLine = tempLine;
                }
            }
        }
    }

    // ---------------- Let's find the inteserction between top and bottom ---------------- //

    double slopeTop = -100, slopeBot = -10, slopeNeg = -10;
    double offTop = -100, offBot = -100, offNeg = -100;

    if ( topLine != nullptr )
    {
        slopeTop = ( topLine->GetY2() - topLine->GetY1() ) / ( topLine->GetX2() - topLine->GetX1() );
        offTop = ( topLine->GetX1() * topLine->GetY2() - topLine->GetX2() * topLine->GetY1() ) / ( topLine->GetX1() - topLine->GetX2() );
    }

    if ( botLine != nullptr )
    {
        slopeBot = ( botLine->GetY2() - botLine->GetY1() ) / ( botLine->GetX2() - botLine->GetX1() );
        offBot = ( botLine->GetX1() * botLine->GetY2() - botLine->GetX2() * botLine->GetY1() ) / ( botLine->GetX1() - botLine->GetX2() );
    }

    if ( negLine != nullptr )
    {
        slopeNeg = ( negLine->GetY2() - negLine->GetY1() ) / ( negLine->GetX2() - negLine->GetX1() );
        offNeg = ( negLine->GetX1() * negLine->GetY2() - negLine->GetX2() * negLine->GetY1() ) / ( negLine->GetX1() - negLine->GetX2() );
    }

    double xIntersect = -100, yIntersect = -100;

    if ( topLine != nullptr && botLine != nullptr )
    {
        xIntersect = ( offTop - offBot ) / ( slopeBot - slopeTop );
        yIntersect = slopeTop * xIntersect + offTop;
    }

    double nearCorrFactor = -100;
    double energyCalCoeff = -100;

    if ( negLine != nullptr ) nearCorrFactor = -slopeNeg;

    InitializeCalMapKey ( calMapKey, strip );

    vector<double>* calRes = &resStripsCalMap[calMapKey][strip];

    xIntersect = ( xIntersect == -100 ? calRes->at ( 0 ) : xIntersect );
    yIntersect = ( yIntersect == -100 ? calRes->at ( 1 ) : yIntersect );
    nearCorrFactor = ( nearCorrFactor == -100 ? calRes->at ( 2 ) : nearCorrFactor );

    calRes->at ( 0 ) = xIntersect;
    calRes->at ( 1 ) = yIntersect;
    calRes->at ( 2 ) = nearCorrFactor;

    if ( negLine != nullptr && calRes->at ( 0 ) != -100 && calRes->at ( 1 ) != -100 && calRes->at ( 2 ) != -100 )
    {
        energyCalCoeff = refEnergy1 / ( negLine->GetX1() * calRes->at ( 2 ) - calRes->at ( 0 ) + negLine->GetY1() - calRes->at ( 1 ) );
    }

    energyCalCoeff = ( energyCalCoeff == -100 ? calRes->at ( 3 ) : energyCalCoeff );

    cout << "Intersection between top and bottom at ( " << xIntersect << " ; " << yIntersect << " )" << endl;
    cout << "Correction factor for the near strip = " << nearCorrFactor << endl;
    cout << "Slope for energy calibration = " << energyCalCoeff << endl;

    calRes->at ( 3 ) = energyCalCoeff;

    return;
}

bool GoddessCalib::DumpFileToResCalMap ( string fileName )
{
    std::ifstream readFile ( fileName.c_str() );

    if ( !readFile.is_open() )
    {
        cerr << "Failed to open file " << fileName << " for previous calibration reading" << endl;
        return false;
    }

    string dump;

    string lineID, detID, stripID, dummy;

    cout << "Reading " << fileName << "..." << endl;

    while ( std::getline ( readFile, dump ) )
    {
        if ( dump.empty() ) continue;

        std::istringstream readLine;
        readLine.str ( dump );

        readLine >> lineID;

        if ( lineID == "SuperX3" )
        {
            readLine >> detID;

            detID = lineID + " " + detID;

            int posInFile = readFile.tellg();

            cout << "Found " << detID << " at position " << posInFile << " in input file..." << endl;

            std::getline ( readFile, dump );

            readLine.clear();
            readLine.str ( dump );

            readLine >> lineID;
        }


        auto itr = resStripsCalMap.find ( detID );

        if ( lineID == "Res." )
        {
            readLine >> dummy >> stripID;

            unsigned short stripNbr = std::stoul ( stripID.substr ( 1 ) );

            double xinter, yinter, slope_gm, slope_encal, pos_left_edge, pos_right_edge;

            readLine >> xinter >> yinter >> slope_gm >> slope_encal >> pos_left_edge >> pos_right_edge;

            InitializeCalMapKey ( detID, stripNbr );

            cout << "Read the following values for strip #" << stripNbr << ": " << xinter << " / " <<  yinter << " / " << slope_gm << " / " << slope_encal << " / " << pos_left_edge << " / " << pos_right_edge << endl;

            vector<double>* readCal = &resStripsCalMap[detID][stripNbr];

            if ( readCal->at ( 0 ) == -100 ) readCal->at ( 0 ) = xinter == -100 ? readCal->at ( 0 ) : xinter;
            if ( readCal->at ( 1 ) == -100 ) readCal->at ( 1 ) = yinter == -100 ? readCal->at ( 1 ) : yinter;
            if ( readCal->at ( 2 ) == -100 ) readCal->at ( 2 ) = slope_gm == -100 ? readCal->at ( 2 ) : slope_gm;
            if ( readCal->at ( 3 ) == -100 ) readCal->at ( 3 ) = slope_encal == -100 ? readCal->at ( 3 ) : slope_encal;
            if ( readCal->at ( 4 ) == -100 ) readCal->at ( 4 ) = pos_left_edge == -100 ? readCal->at ( 4 ) : pos_left_edge;
            if ( readCal->at ( 5 ) == -100 ) readCal->at ( 5 ) = pos_right_edge == -100 ? readCal->at ( 5 ) : pos_right_edge;

            cout << "New values: " << readCal->at ( 0 ) << " / " <<  readCal->at ( 1 ) << " / " << readCal->at ( 2 ) << " / " << readCal->at ( 3 ) << " / " << readCal->at ( 4 ) << " / " << readCal->at ( 5 ) << endl;
        }
    }

    return true;
}

void GoddessCalib::WriteResCalResults ( string fileName, string mode )
{
    if ( mode == "update" )
    {
        if ( DumpFileToResCalMap ( fileName.c_str() ) ) remove ( fileName.c_str() );
    }

    std::ofstream outStream ( fileName.c_str() );

    outStream << "Columns are: \"Strip #\"     \"Offset X\"     \"Offset Y\"     \"Slope (gain match)\"    \"Slope (energy calibration)\"     \"Left Edge Relative Pos.\"     \"Right edge Relative Pos.\"\n";

    for ( auto itr = resStripsCalMap.begin(); itr != resStripsCalMap.end(); itr++ )
    {
        outStream << "\n" << itr->first << "\n";

        for ( auto stripItr = itr->second.begin(); stripItr != itr->second.end(); stripItr++ )
        {
            outStream << "Res. Strip #" << stripItr->first << "   " << stripItr->second[0] << "   " << stripItr->second[1] << "   " << stripItr->second[2] << "   " << stripItr->second[3];
            outStream << "   " << stripItr->second[4] << "   " << stripItr->second[5] << "\n";
        }
    }

    auto time_now = std::chrono::system_clock::now();

    auto ctime_now = std::chrono::system_clock::to_time_t ( time_now );

    string timeAndDate = std::ctime ( &ctime_now );

    std::size_t spacePos, charPos;

    spacePos = timeAndDate.find_first_of ( " \b\n\r\t\v" );

    charPos = timeAndDate.find_first_not_of ( " \b\n\r\t\v", spacePos );
    spacePos = timeAndDate.find_first_of ( " \b\n\r\t\v", charPos );

    string month = timeAndDate.substr ( charPos, spacePos - charPos );

    charPos = timeAndDate.find_first_not_of ( " \b\n\r\t\v", spacePos );
    spacePos = timeAndDate.find_first_of ( " \b\n\r\t\v", charPos );

    string dayNum = timeAndDate.substr ( charPos, spacePos - charPos );

    charPos = timeAndDate.find_first_not_of ( " \b\n\r\t\v", spacePos );
    spacePos = timeAndDate.find_first_of ( " \b\n\r\t\v", charPos );

    string timeOfDay = timeAndDate.substr ( charPos, spacePos - charPos );
    timeOfDay = FindAndReplaceInString ( timeOfDay, ":", "_" );

    charPos = timeAndDate.find_first_not_of ( " \b\n\r\t\v", spacePos );
    spacePos = timeAndDate.find_first_of ( " \b\n\r\t\v", charPos );

    string year = timeAndDate.substr ( charPos, spacePos - charPos );

//     string graphFileName = "EnShift_vs_Pos_" + dayNum + "_" + month + "_" + year + "_at_" + timeOfDay + ".root";
    string graphFileName = "EnShift_vs_Pos_" + dayNum + "_" + month + "_" + year + ".root";

    TFile* grFile = new TFile ( graphFileName.c_str(), "update" );

    for ( auto itr = enShiftVsPosGraphs.begin(); itr != enShiftVsPosGraphs.end(); itr++ )
    {
        grFile->cd();

        if ( grFile->FindObjectAny ( itr->first.c_str() ) != nullptr )
        {
            string toDelete = itr->first + ";*";

            grFile->Delete ( toDelete.c_str() );
        }

        if ( !isnan ( itr->second->GetXaxis()->GetXmin() ) && !isnan ( itr->second->GetXaxis()->GetXmax() ) )
        {
            if ( itr->first.find ( "_jump_at_" ) != string::npos )
            {
                if ( grFile->FindObjectAny ( "jumps" ) == nullptr ) grFile->mkdir ( "jumps" );

                grFile->cd ( "/jumps" );
            }

            itr->second->Write();
        }
    }

    grFile->Close();

    return;
}

bool GoddessCalib::UpdateParamsInConf ( string configFile, string detType, bool invertContactMidDet, string mode )
{
    std::ifstream readFile ( configFile.c_str() );

    if ( !readFile.is_open() )
    {
        cerr << "Failed to open input file " << configFile << endl;
        return false;
    }

    string path = configFile.substr ( 0, configFile.find_last_of ( "/" ) );

    int foundExt = configFile.find ( "." );

    string newName = configFile;
    newName.insert ( foundExt, "_new" );

    std::ofstream outStream ( newName.c_str() );

    if ( !outStream.is_open() )
    {
        cerr << "Failed to open output file " << configFile << endl;
        return false;
    }

    string dump;

    string lineID, detID, stripID, dummy;

    cout << "Reading " << configFile << "..." << endl;

    while ( std::getline ( readFile, dump ) )
    {
        std::istringstream readLine;
        readLine.str ( dump );

        readLine >> lineID;

        if ( lineID != detType )
        {
            outStream << dump << "\n";
            continue;
        }

        else
        {
            outStream << dump << "\n";

            readLine >> dummy >> detID;

//             int posInFile = readFile.tellg();

            cout << "Found a " << detType << " entry: " << detID << endl;

            std::getline ( readFile, dump );
            outStream << dump << "\n";

            if ( detType == "superX3" || detType == "QQQ5" )
            {
                std::getline ( readFile, dump );
                outStream << dump << "\n";
            }

            if ( detType == "superX3" )
            {
                string detKey = "SuperX3 " + detID.substr ( 0,detID.find ( "-" ) );

                if ( resStripsCalMap.find ( detKey ) != resStripsCalMap.end() )
                {
                    for ( int i =0; i < 4; i++ )
                    {
                        if ( resStripsCalMap[detKey].find ( i ) != resStripsCalMap[detKey].end() )
                        {
                            cout << "Replacing the parameters for strip #" << i << " with the new values..." << endl;

                            float offNear, offFar, slopeNear, slopeFar, offEnCal, slopeEnCal, pos_left_edge, pos_right_edge;

                            std::getline ( readFile, dump );
                            readLine.clear();
                            readLine.str ( dump );

                            if ( i < 2 || !invertContactMidDet )
                            {
                                readLine >> dummy >> dummy>> dummy >> offNear >> slopeNear;

                                offNear = resStripsCalMap[detKey][i][0] != -100 ? resStripsCalMap[detKey][i][0] : offNear;
                                slopeNear = resStripsCalMap[detKey][i][2] != -100 ? resStripsCalMap[detKey][i][2] : slopeNear;

                                std::getline ( readFile, dump );
                                readLine.clear();
                                readLine.str ( dump );

                                readLine >> dummy >> dummy>> dummy >> offFar >> slopeFar;

                                offFar = resStripsCalMap[detKey][i][1] != -100 ? resStripsCalMap[detKey][i][1] : offFar;

                                outStream << "enCal p " << i*2 << " " << offNear << " " << slopeNear << "\n";
                                outStream << "enCal p " << i*2 + 1 << " " << offFar << " " << slopeFar << "\n";
                            }
                            else
                            {
                                readLine >> dummy >> dummy>> dummy >> offFar >> slopeFar;

                                offFar = resStripsCalMap[detKey][i][1] != -100 ? resStripsCalMap[detKey][i][1] : offFar;

                                std::getline ( readFile, dump );
                                readLine.clear();
                                readLine.str ( dump );

                                readLine >> dummy >> dummy>> dummy >> offNear >> slopeNear;

                                offNear = resStripsCalMap[detKey][i][0] != -100 ? resStripsCalMap[detKey][i][0] : offNear;
                                slopeNear = resStripsCalMap[detKey][i][2] != -100 ? resStripsCalMap[detKey][i][2] : slopeNear;

                                outStream << "enCal p " << i*2 << " " << offFar << " " << slopeFar << "\n";
                                outStream << "enCal p " << i*2 + 1 << " " << offNear << " " << slopeNear << "\n";
                            }

                            std::getline ( readFile, dump );
                            readLine.clear();
                            readLine.str ( dump );

                            readLine >> dummy >> dummy>> dummy >> offEnCal >> slopeEnCal;

                            slopeEnCal = resStripsCalMap[detKey][i][3] != -100 ? resStripsCalMap[detKey][i][3] : slopeEnCal;

                            outStream << "enCal resStrip " << i << " " << offEnCal << " " << slopeEnCal << "\n";

                            std::getline ( readFile, dump );
                            readLine.clear();
                            readLine.str ( dump );

                            readLine >> dummy >> dummy>> dummy >> pos_left_edge >> pos_right_edge;

                            pos_left_edge = resStripsCalMap[detKey][i][4] != -100 ? resStripsCalMap[detKey][i][4] : pos_left_edge;
                            pos_right_edge = resStripsCalMap[detKey][i][5] != -100 ? resStripsCalMap[detKey][i][5] : pos_right_edge;

                            outStream << "posCal resStrip " << i << " " << pos_left_edge << " " << pos_right_edge << "\n";
                        }
                        else
                        {
                            for ( int j =0; j < 4; j++ )
                            {
                                std::getline ( readFile, dump );
                                outStream << dump << "\n";
                            }
                        }
                    }
                }
                else
                {
                    for ( int i = 0; i < 16; i++ )
                    {
                        std::getline ( readFile, dump );
                        outStream << dump << "\n";
                    }
                }


                for ( int i = 0; i < 4; i++ )
                {
                    std::getline ( readFile, dump );
                    outStream << dump << "\n";
                }
            }
            else if ( detType == "QQQ5" )
            {
                string detKey = "QQQ5 " + detID.substr ( 0,detID.find ( "-" ) );

                if ( endcapsStripsCalMap.find ( detKey ) != endcapsStripsCalMap.end() )
                {
                    for ( int i = 0; i < 32; i++ )
                    {
                        float offset, slope;

                        std::getline ( readFile, dump );
                        readLine.clear();
                        readLine.str ( dump );

                        readLine >> dummy >> dummy>> dummy >> offset >> slope;

                        outStream << "enCal p " << i << " " << offset << " " << slope * ( endcapsStripsCalMap[detKey][i]/100. ) << "\n";
                    }
                }
                else
                {
                    for ( int i = 0; i < 16; i++ )
                    {
                        std::getline ( readFile, dump );
                        outStream << dump << "\n";
                    }
                }

                for ( int i = 0; i < 4; i++ )
                {
                    std::getline ( readFile, dump );
                    outStream << dump << "\n";
                }
            }
        }
    }

    readFile.close();
    outStream.close();

    if ( mode == "overwrite" )
    {
        remove ( configFile.c_str() );
        rename ( newName.c_str(), configFile.c_str() );
    }

    return true;
}

TH2F* GoddessCalib::PlotSX3ResStripCalGraph ( TChain* chain, string varToPlot, unsigned short sector, unsigned short strip, string conditions )
{
//     cout<<"button pushed"<<endl;

    std::size_t upStreamCondPos = conditions.find ( "si.isUpstream" );

    string upStreamCond = "U";

    if ( upStreamCondPos == string::npos )
    {
        cerr << "isUpstream condition missing..." << endl;
        return 0;
    }

    if ( conditions[upStreamCondPos-1] == '!' ) upStreamCond = "D";

    cout << "Plotting sector #" << sector << " strip #" << strip << "..." << endl;

    chain->Draw ( Form ( "%s", varToPlot.c_str() ), Form ( "%s && si.sector == %d && si.E1.strip.p@.size() > 0 && si.E1.strip.p == %d", conditions.c_str(), sector, strip ), "colz" );

    int binnum;
    int starthere;
    int endhere;

    if ( sector < 6 && sector != 0 )
    {
        binnum = 2000;
        starthere = 0;
        endhere = 150000;
    }
    if ( sector > 6 || sector == 0 )
    {
        binnum = 400;
        starthere = 0;
        endhere = 2000;
    }

    TH2F* TH2Fplot = new TH2F ( "SX3EnCal","SX3 Energy Calibrations", binnum, starthere, endhere, binnum, starthere, endhere );
    //TH2Fplot = ( TH2F* ) gPad->GetPrimitive ( "Graph" );

    if ( TH2Fplot == nullptr ) return 0;

    string gName = Form ( "sector%s%d_strip%d", upStreamCond.c_str(), sector, strip );

    TH2Fplot->SetName ( gName.c_str() );
    TH2Fplot->SetTitle ( gName.c_str() );

    TFile* f = new TFile ( outFileName.c_str(), "update" );

    if ( f->FindKey ( gName.c_str() ) != nullptr || f->FindObject ( gName.c_str() ) != nullptr )
    {
        string objToDelete = gName + ";*";
        f->Delete ( objToDelete.c_str() );
    }

    f->cd();

    TH2Fplot->Write();

    f->Close();

    resStripsEnCalGraphsMap[Form ( "sector %s%d strip %d", upStreamCond.c_str(), sector, strip )] = TH2Fplot;

    return TH2Fplot;
}

void GoddessCalib::PlotSX3ResStripsCalGraphsFromTree ( TChain* chain, long int nentries, bool isUpstream_, vector< int > sectorsList )
{
    cout << sectorsList.size() << " sectors will be treated..." << endl;

    int sizeOfSectors = sectorsList.size();

    //     int numberOfGraphs = sizeOfSectors*4;

    for ( int i =0; i < sizeOfSectors; i++ )
    {
        for ( int j =0; j < 4; j++ )
        {
            string grID = Form ( "sector%s%d_strip%d", isUpstream_ ? "U" : "D", sectorsList[i], j );

            cout << "Creating graph " << grID << endl;

            int binnum;
            int starthere;
            int endhere;

            if ( sectorsList[i] < 6 && sectorsList[i] != 0 )
            {
                binnum = 2000;
                starthere = 0;
                endhere = 150000;
            }
            if ( sectorsList[i] > 6 || sectorsList[i] == 0 )
            {
                binnum = 400;
                starthere = 0;
                endhere = 2000;
            }

            TH2F* TH2Fplot = new TH2F ( "SX3EnCal","SX3 Energy Calibrations", binnum, starthere, endhere, binnum, starthere, endhere );

            TH2Fplot->SetName ( grID.c_str() );
            TH2Fplot->SetTitle ( grID.c_str() );

            resStripsEnCalGraphsMap[Form ( "sector %s%d strip %d", isUpstream_ ? "U" : "D", sectorsList[i], j )] = TH2Fplot;

            //             cout << "Stored graph in the TH2F map..." << endl;
        }
    }

    if ( nentries == 0 || nentries > chain->GetEntries() ) nentries = chain->GetEntries();

    cout << nentries << " entries will be treated" <<endl;

    vector<SiDataDetailed>* siInfo = new vector<SiDataDetailed>();
    siInfo->clear();

    cout << "Preparing the readout of the tree..." << endl;

    chain->SetBranchAddress ( "si", &siInfo );

    //     cout << "Linked the \"si\" branch to a SiDataDetailed object..." << endl;

    for ( int i = 0; i < nentries; i++ )
    {
        chain->GetEntry ( i );

        if ( i%10000 == 0 )
        {
            cout << "Treated " << std::setw ( 9 ) << i << " / " << nentries;
            std::cout << " entries ( " << std::fixed << std::setprecision ( 1 ) << std::setw ( 5 ) << ( float ) i/nentries * 100. << "% )\r" << std::flush;
        }

        if ( siInfo->size() == 0 ) continue;

        for ( unsigned short j = 0; j < siInfo->size(); j++ )
        {
            int sectorNbr = -1;

            for ( int k = 0; k < sizeOfSectors; k++ )
            {
                if ( siInfo->at ( j ).sector == sectorsList[k] )
                {
                    sectorNbr = siInfo->at ( j ).sector;
                    break;
                }
            }

            if ( sectorNbr == -1 ) continue;

            if ( siInfo->at ( j ).isBarrel && siInfo->at ( j ).isUpstream == isUpstream_ && siInfo->at ( j ).E1.en.p.size() > 0 )
            {
                for ( unsigned short st = 0; st < siInfo->at ( j ).E1.en.p.size(); st++ )
                {
                    string grID = Form ( "sector %s%d strip %d", isUpstream_ ? "U" : "D", sectorNbr, siInfo->at ( j ).E1.strip.p[st] );

                    TH2F* gr = resStripsEnCalGraphsMap[grID];

                    gStyle->SetPalette ( kRainBow );

                    gr->Fill ( siInfo->at ( j ).E1.en.p[st] , siInfo->at ( j ).E1.en.n[st] ) ;
                }
            }
        }
    }

    cout << endl;

    TFile* f = new TFile ( outFileName.c_str(), "update" );

    f->cd();

    for ( auto itr = resStripsEnCalGraphsMap.begin(); itr != resStripsEnCalGraphsMap.end(); itr++ )
    {
        TH2F* gr = itr->second;

        if ( f->FindKey ( gr->GetName() ) != NULL || f->FindObject ( gr->GetName() ) != NULL )
        {
            string objToDelete = gr->GetName();
            objToDelete += ";*";
            f->Delete ( objToDelete.c_str() );
        }

        cout << "Writing " << gr->GetName() << " to file..." << endl;

        gr->Write();
    }

    f->Close();

    return;
}

TGraph* GoddessCalib::DrawPosCalGraph ( TChain* chain, bool isUpstream_, int nentries, unsigned short sector_, unsigned short strip_ )
{
    TGraph* newGraph = new TGraph();

    DrawPosCal ( chain, isUpstream_, sector_,strip_, nentries, newGraph );

    newGraph->Draw ( "AP" );

    return newGraph;
}

TH2F* GoddessCalib::DrawPosCalHist ( TChain* chain, bool isUpstream_, int nentries, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY,
                                     string drawOpts, unsigned short sector_, unsigned short strip_ )
{
    string hname = Form ( "hPosCal_sector%s%d_strip%d", isUpstream_ ? "U" : "D", sector_, strip_ );

    string isUpstreamID = isUpstream_ ? "U" : "D";
    string hKey = Form ( "%s%d_%d", isUpstreamID.c_str(), sector_, strip_ );
    resStripsPosCalGraphsMap[hKey.c_str()] = new TH2F ( hname.c_str(), hname.c_str(), nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY );

    DrawPosCal ( chain, isUpstream_, sector_,strip_, nentries, resStripsPosCalGraphsMap[hKey.c_str()] );

    resStripsPosCalGraphsMap[hKey.c_str()]->Draw ( drawOpts.c_str() );

    return resStripsPosCalGraphsMap[hKey.c_str()];
}

std::map<string, TH2F*> GoddessCalib::DrawPosCalHistBatch ( TChain* chain, bool isUpstream_, int nentries,
        int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, unsigned short sector_, string configFileName )
{
    return DrawPosCalHistBatch ( chain, isUpstream_, nentries,nbinsX,binMinX, binMaxX, nbinsY, binMinY, binMaxY, drawOpts, sector_, configFileName );
}

std::map<string, TH2F*> GoddessCalib::DrawPosCalHistBatch ( TChain* chain, bool isUpstream_, int nentries,
        int nbinsX, float binMinX, float binMaxX, int nbinsY, float binMinY, float binMaxY, string drawOpts, vector< int > sectorsList, string configFileName )
{
    drawOpts = ""; // to avoid useless warning about that variable not being used for the moment...

    string isUpstreamID = isUpstream_ ? "U" : "D";

    for ( unsigned short i = 0; i < sectorsList.size(); i++ )
    {
        for ( int j = 0; j < 4; j++ )
        {
            string hname = Form ( "%s%d_%d", isUpstreamID.c_str(), sectorsList[i], j );

            cout << "Initializing graph for sector " << isUpstreamID << sectorsList[i] << " strip " << j << " and storing it in the map at key \"" << hname << "\"" << endl;

            resStripsPosCalGraphsMap[hname.c_str()] = new TH2F ( hname.c_str(), hname.c_str(), nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY );
        }
    }

    vector<SiDataDetailed>* siDataVect = new vector<SiDataDetailed>();
    siDataVect->clear();

    chain->SetBranchAddress ( "si", &siDataVect );

    if ( nentries > chain->GetEntries() || nentries == 0 ) nentries = chain->GetEntries();

    if ( !configFileName.empty() )
    {
        ReadConfigCalPars ( configFileName );
    }

//     string qqq5Ids[4] = {"A", "B", "C", "D"};

    for ( int i = 0; i < nentries; i++ )
    {
        chain->GetEntry ( i );

        if ( i%10000 == 0 )
        {
            cout << "Treated " << std::setw ( 9 ) << i << " / " << nentries;
            cout << " entries ( " << std::fixed << std::setprecision ( 1 ) << std::setw ( 5 ) << ( float ) i/nentries * 100. << " % )\r" << std::flush;
        }

        if ( siDataVect->size() == 0 ) continue;

        for ( unsigned short j = 0; j < siDataVect->size(); j++ )
        {
            SiDataDetailed siData = siDataVect->at ( j );

            if ( siData.isBarrel && siData.isUpstream == isUpstream_ && siData.E1.en.p.size() > 0 && siData.E1.en.n.size() > 0 )
            {
                int sect = -1;

                for ( unsigned short sec = 0; sec < sectorsList.size(); sec++ )
                {
                    if ( siData.sector == sectorsList[sec] )
                    {
                        sect = siData.sector;
                        break;
                    }
                }

                if ( sect == -1 ) continue;

                if ( configFileName.empty() )
                {
                    for ( unsigned short k = 0; k < siData.E1.en.p.size(); k++ )
                    {
                        TH2F* hh = resStripsPosCalGraphsMap[Form ( "%s%d_%d", isUpstreamID.c_str(), sect, siData.E1.strip.p[k] )];
                        hh->Fill ( ( siData.E1.en.p[k] - siData.E1.en.n[k] ) / ( siData.E1.en.p[k] + siData.E1.en.n[k] ), ( siData.E1.en.p[k] + siData.E1.en.n[k] ) );
                    }
                }
                else
                {
                    string detID = siData.isUpstream ? "U" : "D";

                    string detKey = Form ( "SuperX3 %s%d", detID.c_str(), siData.sector );

                    for ( unsigned short k = 0; k < siData.E1.en.p.size(); k++ )
                    {
                        int strip = siData.E1.strip.p[k];

                        float offNear, slopeNear, offFar, slopeFar, enCalPar;

                        string calParKey = detKey + " front " + Form ( "%d", strip );

                        enCalPar = configCalPars[calParKey].second;

                        calParKey = detKey + " near " + Form ( "%d", strip );

                        offNear = configCalPars[calParKey].first;
                        slopeNear = configCalPars[calParKey].second;

                        calParKey = detKey + " far " + Form ( "%d", strip );

                        offFar = configCalPars[calParKey].first;
                        slopeFar = configCalPars[calParKey].second;

                        float nearCalEn = siData.E1.en.p[k] * slopeNear - offNear ;
                        float farCalEn = siData.E1.en.n[k] * slopeFar - offFar;

                        float totCalEn = ( nearCalEn + farCalEn ) * enCalPar;

                        TH2F* hh = resStripsPosCalGraphsMap[Form ( "%s%d_%d", isUpstreamID.c_str(), sect, strip )];
                        hh->Fill ( ( nearCalEn - farCalEn ) / ( nearCalEn + farCalEn ), totCalEn );
                    }
                }
            }
        }
    }

    cout << endl;

    WritePosCalHistsToFile ( );

    return resStripsPosCalGraphsMap;
}

void GoddessCalib::DrawSX3EnCalGraph ( bool isUpstream, short unsigned int sector, short unsigned int strip )
{
    string isUpstreamID = isUpstream ? "U" : "D";
    string hKey = Form ( "sector %s%d strip %d", isUpstreamID.c_str(), sector, strip );

    auto itr = resStripsEnCalGraphsMap.find ( hKey );

    if ( itr != resStripsEnCalGraphsMap.end() )
    {
        itr->second->Draw ( "AP" );
    }
    else
        cerr << "This graph has not been generated yet!" << endl;
}

void GoddessCalib::DrawSX3PosCalHist ( bool isUpstream, short unsigned int sector, short unsigned int strip, string drawOpts )
{
    string isUpstreamID = isUpstream ? "U" : "D";
    string hKey = Form ( "%s%d_%d", isUpstreamID.c_str(), sector, strip );

    auto itr = resStripsPosCalGraphsMap.find ( hKey );

    if ( itr != resStripsPosCalGraphsMap.end() )
    {
        itr->second->Draw ( drawOpts.c_str() );
    }
    else
        cerr << "This graph has not been generated yet!" << endl;
}

void GoddessCalib::WritePosCalHistsToFile ( )
{
    TFile* f = new TFile ( outFileName.c_str(), "update" );

    for ( auto itr = resStripsPosCalGraphsMap.begin(); itr != resStripsPosCalGraphsMap.end(); itr++ )
    {
        if ( f->FindKey ( itr->second->GetName() ) != nullptr || f->FindObject ( itr->second->GetName() ) != nullptr )
        {
            string objToDelete = itr->second->GetName();
            objToDelete += ";*";
            f->Delete ( objToDelete.c_str() );
        }

        f->cd();

        itr->second->Write();
    }

    f->Close();

    return;
}

TH2F* GoddessCalib::GetSX3EnCalGraph ( bool isUpstream, short unsigned int sector, short unsigned int strip )
{
    string isUpstreamID = isUpstream ? "U" : "D";
    string hKey = Form ( "sector %s%d strip %d", isUpstreamID.c_str(), sector, strip );

    auto itr = resStripsEnCalGraphsMap.find ( hKey );

    if ( itr != resStripsEnCalGraphsMap.end() )
    {
        return itr->second;
    }
    else
    {
        cerr << "This graph has not been generated yet!" << endl;
        return 0;
    }
}

TH2F* GoddessCalib::GetSX3PosCalHist ( bool isUpstream, short unsigned int sector, short unsigned int strip )
{
    string isUpstreamID = isUpstream ? "U" : "D";
    string hKey = Form ( "%s%d_%d", isUpstreamID.c_str(), sector, strip );

    auto itr = resStripsPosCalGraphsMap.find ( hKey );

    if ( itr != resStripsPosCalGraphsMap.end() )
    {
        return itr->second;
    }
    else
    {
        cerr << "This graph has not been generated yet!" << endl;
        return 0;
    }
}

int GoddessCalib::GetPosCalEnBinMax ( TH2F* input, double threshold )
{
    int fstBin = input->GetXaxis()->GetFirst();
    int lstBin = input->GetXaxis()->GetLast();

    int binMax = 0;

    TH1D* proj = input->ProjectionY ( ( ( string ) "projY_" + input->GetName() ).c_str(), fstBin, lstBin );

    proj->GetXaxis()->SetRangeUser ( threshold, proj->GetBinCenter ( proj->GetXaxis()->GetLast() ) );

    binMax = proj->GetMaximumBin();

//     cout << "Found the max at " << proj->GetXaxis()->GetBinCenter ( binMax );
//     cout << " (Range: " << proj->GetXaxis()->GetBinCenter (proj->GetXaxis()->GetFirst()) << " - " << proj->GetXaxis()->GetBinCenter (proj->GetXaxis()->GetLast()) << ")\n";

    return binMax;
}

TH1D* GoddessCalib::GetPosCalProjX ( TH2F* input, int projCenter, int projWidth )
{
    string projName = Form ( "projX_%s_%0.3f_%0.3f", input->GetName(), input->GetYaxis()->GetBinCenter ( projCenter - projWidth/2. ), input->GetYaxis()->GetBinCenter ( projCenter + projWidth/2. ) );
    string projTitle = Form ( "projX %s [%0.3f to %0.3f]", input->GetName(), input->GetYaxis()->GetBinCenter ( projCenter - projWidth/2. ), input->GetYaxis()->GetBinCenter ( projCenter + projWidth/2. ) );

    TH1D* proj = input->ProjectionX ( projName.c_str(), projCenter - projWidth/2, projCenter + projWidth/2 );
    proj->SetTitle ( projTitle.c_str() );

    return proj;
}

TH1D* GoddessCalib::GetPosCalProjY ( TH2F* input, int projCenter, int projWidth )
{
    string projName = Form ( "projY_%s_%0.3f_%0.3f", input->GetName(), input->GetXaxis()->GetBinCenter ( projCenter - projWidth/2 ), input->GetXaxis()->GetBinCenter ( projCenter + projWidth/2 ) );
    string projTitle = Form ( "projY %s [%0.3f to %0.3f]", input->GetName(), input->GetXaxis()->GetBinCenter ( projCenter - projWidth/2 ), input->GetXaxis()->GetBinCenter ( projCenter + projWidth/2 ) );

    TH1D* proj = input->ProjectionY ( projName.c_str(), projCenter - projWidth/2, projCenter + projWidth/2 );
    proj->SetTitle ( projTitle.c_str() );

    return proj;
}


Double_t flatTopGaussLeft ( Double_t *x, Double_t *par )
{
    if ( x[0] > par[1] )
    {
        return par[3] * ( x[0] - par[1] ) + par[0];
    }
    else
    {
        return par[0] * TMath::Exp ( -pow ( x[0] - par[1],2 ) / pow ( 2 * par[2],2 ) );
    }
}

Double_t flatTopGaussRight ( Double_t *x, Double_t *par )
{
    if ( x[0] < par[1] )
    {
        return par[3] * ( x[0] - par[1] ) + par[0];
    }
    else
    {
        return par[0] * TMath::Exp ( -pow ( x[0] - par[1],2 ) / pow ( 2 * par[2],2 ) );
    }
}

Double_t flatTopGauss ( Double_t *x, Double_t *par )
{
    if ( x[0] < par[1] )
    {
        return par[0] * TMath::Exp ( -pow ( x[0] - par[1],2 ) / pow ( 2 * par[2],2 ) );
    }
    else if ( x[0] > par[4] )
    {
        return par[3] * TMath::Exp ( -pow ( x[0] - par[4],2 ) / pow ( 2 * par[2],2 ) );
    }
    else
    {
        return ( par[3] - par[0] ) / ( par[4] - par[1] ) * ( x[0] - par[1] ) + ( par[0] );
    }
}

TF1* GoddessCalib::FitEdges ( TH2F* input, int projCenterBin, int projWidth, bool fitRight, bool getParams, bool quietMode )
{
    TH1D* projX = GetPosCalProjX ( input, projCenterBin, projWidth );
    projX->GetXaxis()->SetRange ( projX->GetXaxis()->GetFirst() +1, projX->GetXaxis()->GetLast()-1 );

    int binMaxX = projX->GetMaximumBin();

    double maxContent = projX->GetBinContent ( binMaxX );

    int startBin = fitRight ? ( projX->GetXaxis()->GetLast() - 1 ) : ( projX->GetXaxis()->GetFirst() + 1 );
    double startBinContent = maxContent;

    int binShoulder = -1;

    int windowWidth = projX->GetNbinsX() / 20;

    int localMaxBin = fitRight ? ( projX->GetXaxis()->GetLast() - 1 ) : ( projX->GetXaxis()->GetFirst() + 1 );
    double localMaxContent = projX->GetBinContent ( localMaxBin );

    while ( fitRight ? ( startBin > projX->GetXaxis()->GetFirst() + 1 ) : ( startBin < projX->GetXaxis()->GetLast() - 1 ) )
    {
        double prevBinContent = startBinContent;

        int nextBin = fitRight ? ( startBin - 1 ) : ( startBin + 1 );
        double nextBinContent = projX->GetBinContent ( nextBin );

        if ( nextBinContent > localMaxContent )
        {
            localMaxBin = nextBin;
            localMaxContent = nextBinContent;
        }

        if ( binShoulder != -1 && ( nextBinContent-prevBinContent ) /prevBinContent <= 0.1 ) break;

        if ( prevBinContent > maxContent/6. && ( nextBinContent-prevBinContent ) /prevBinContent > 0.1 )
        {
            binShoulder = nextBin;
        }

        if ( fitRight ) startBin--;
        else startBin++;

        startBinContent = projX->GetBinContent ( startBin );
    }

    if ( !quietMode ) cout << "Found the " << ( fitRight ? "right" : "left" ) << " shoulder at bin #" << binShoulder << " (value = " << projX->GetBinCenter ( binShoulder ) << ")" << endl;

    TF1 *fitfunc = new TF1 ( Form ( "myfit_left_%s",input->GetName() ), ( fitRight ? flatTopGaussRight : flatTopGaussLeft ), projX->GetBinCenter ( binShoulder - 2*windowWidth ), projX->GetBinCenter ( binShoulder + windowWidth ), 4 );

    if ( binShoulder != -1 )
    {
        fitfunc->SetParameter ( 0, projX->GetBinContent ( binShoulder ) );
        fitfunc->SetParameter ( 1, projX->GetBinCenter ( binShoulder ) );
        fitfunc->SetParameter ( 2, 2*projX->GetBinWidth ( binShoulder ) );

        fitfunc->SetParameter ( 3, 0 );

        projX->Fit ( fitfunc, "QRMN" );

        float edge = fitfunc->GetParameter ( 1 ) - TMath::Sqrt ( -2*pow ( fitfunc->GetParameter ( 2 ),2 ) * TMath::Log ( 0.7 ) );

        if ( !quietMode ) cout << "Found the " << ( fitRight ? "right" : "left" ) << " strip edge at " << edge << endl;

        string hname = input->GetName();
        string calMapKey = "SuperX3 " + hname.substr ( 0, hname.find ( "_" ) );

        int stripNbr = std::stoi ( hname.substr ( hname.find ( "_" ) + 1 ) );

        if ( getParams )
        {
            InitializeCalMapKey ( calMapKey, stripNbr );

            if ( fitRight ) resStripsCalMap[calMapKey][stripNbr][5] = edge;
            else resStripsCalMap[calMapKey][stripNbr][4] = edge;
        }
    }
    return fitfunc;
}

vector< float > GoddessCalib::GetOverlapPoints ( TH2F* input, float xMin, float xMax, double threshold, bool jumpUp, bool printDebug )
{
    int xMinBin = input->GetXaxis()->FindBin ( xMin );
    int xMaxBin = input->GetXaxis()->FindBin ( xMax );

    int projWidthX = xMaxBin - xMinBin;

    int projBinX = xMinBin + projWidthX/2;

    TH1D* projY = GetPosCalProjY ( input, projBinX, projWidthX );

    double lastBinCenter = projY->GetBinCenter ( projY->GetXaxis()->GetLast() );

    projY->GetXaxis()->SetRangeUser ( threshold, lastBinCenter );

    if ( printDebug )
    {
        TCanvas* c1 = new TCanvas();
        c1->cd();
        TH1D* h1 = ( TH1D* ) projY->Clone();
        h1->Draw();
    }

    float binCenterMax1 = projY->GetBinCenter ( projY->GetMaximumBin() );

    if ( printDebug ) cout << "Found first maximum at " << binCenterMax1 << "...\n";

    TF1* simpleGauss = new TF1 ( "simpleGauss", "[0] * TMath::Exp ( -pow ( x - [1],2 ) / pow ( 2 * [2],2 ) )", threshold, lastBinCenter );

    simpleGauss->SetParameters ( 10, binCenterMax1, 0.2 );

    projY->Fit ( simpleGauss, "QRMN", "", binCenterMax1 - 0.5, binCenterMax1 + 0.5 );

    float projYSigma = TMath::Abs ( simpleGauss->GetParameter ( 2 ) );

    if ( printDebug )
    {
        cout << "Result of the fasty fit:\n";
        cout << "Amplitude: " << simpleGauss->GetParameter ( 0 ) << endl;
        cout << "Mean: " << simpleGauss->GetParameter ( 1 ) << endl;
        cout << "Sigma: " << simpleGauss->GetParameter ( 2 ) << endl;
    }

    projY->GetXaxis()->SetRangeUser ( threshold, binCenterMax1 - 5*projYSigma );

    if ( printDebug )
    {
        TCanvas* c2 = new TCanvas();
        c2->cd();
        TH1D* h2 = ( TH1D* ) projY->Clone();
        h2->Draw();
    }

    float binCenterMaxLow = projY->GetBinCenter ( projY->GetMaximumBin() );
    float binContentMaxLow = projY->GetBinContent ( projY->GetMaximumBin() );

    if ( printDebug ) cout << "Found temporary max on the left of the first max at " << binCenterMaxLow << " ( content = " << binContentMaxLow << " )\n";

    projY->GetXaxis()->SetRangeUser ( binCenterMax1 + 5*projYSigma, lastBinCenter );

    if ( printDebug )
    {
        TCanvas* c3 = new TCanvas();
        c3->cd();
        TH1D* h3 = ( TH1D* ) projY->Clone();
        h3->Draw();
    }

    float binCenterMaxHigh = projY->GetBinCenter ( projY->GetMaximumBin() );
    float binContentMaxHigh = projY->GetBinContent ( projY->GetMaximumBin() );

    if ( printDebug ) cout << "Found temporary max on the right of the first max at " << binCenterMaxHigh << " ( content = " << binContentMaxHigh << " )\n";

    float binCenterMax2 = binCenterMaxLow;

    if ( binContentMaxHigh > binContentMaxLow ) binCenterMax2 = binCenterMaxHigh;

    float xleft, xright, yleft, yright; // left and right are referring to the edge detection and not the actual position in the plot

    if ( jumpUp )
    {
        yleft = std::max ( binCenterMax1, binCenterMax2 );
        yright = std::min ( binCenterMax1, binCenterMax2 );
    }
    else
    {
        yleft = std::min ( binCenterMax1, binCenterMax2 );
        yright = std::max ( binCenterMax1, binCenterMax2 );
    }

    if ( printDebug ) cout << "Detected a jump from " << yright << " to " << yleft << " ...\n";

    int projWidthY = 6*projYSigma / input->GetYaxis()->GetBinWidth ( 0 );

    TF1* lfit = FitEdges ( input, input->GetYaxis()->FindBin ( yleft ), projWidthY, false, false, true );

    if ( printDebug )
    {
        TCanvas* c4 = new TCanvas();
        c4->cd();
        TH1D* h4 = ( TH1D* ) GetPosCalProjX ( input, input->GetYaxis()->FindBin ( yleft ), projWidthY )->Clone();
        h4->Draw();
        lfit->Draw ( "same" );
    }

    TF1* rfit = FitEdges ( input, input->GetYaxis()->FindBin ( yright ), projWidthY, true, false, true );

    if ( printDebug )
    {
        TCanvas* c5 = new TCanvas();
        c5->cd();
        TH1D* h5 = ( TH1D* ) GetPosCalProjX ( input, input->GetYaxis()->FindBin ( yright ), projWidthY )->Clone();
        h5->Draw();
        rfit->Draw ( "same" );
    }

    xleft = lfit->GetParameter ( 1 ) - 4*TMath::Abs ( lfit->GetParameter ( 2 ) );
    xright = rfit->GetParameter ( 1 ) + 4*TMath::Abs ( rfit->GetParameter ( 2 ) );

    if ( printDebug ) cout << "The jumping points are ( " << xright << " , " << yright << " ) and ( " << xleft << " , " << yleft << " )\n";

    float eNear1, eFar1, eNear2, eFar2;

    // Now we recover the Energy near and Energy Far corresponding to these 2 points to get the equation of the line separating the 2 regimes
    // REMINDER:
    // x = (Enear - Efar) / ( Enear + Efar) -------------- y = Enear + Efar
    // so
    // ===? (xy + y) / 2 = Enear
    // ===> -(xy - y) / 2 = Efar

    eNear1 = ( xleft * yleft + yleft ) / 2.;
    eFar1 = - ( xleft * yleft - yleft ) / 2.;

    eNear2 = ( xright * yright + yright ) / 2.;
    eFar2 = - ( xright * yright - yright ) / 2.;

    float eNearMin = std::min ( eNear1, eNear2 );
    float eNearMax = std::max ( eNear1, eNear2 );

    return {xright, yright, xleft, yleft, eNearMin, ( eNearMin == eNear1 ? eFar1 : eFar2 ), eNearMax, ( eNearMax == eNear1 ? eFar1 : eFar2 ) };
}

std::tuple<TGraph*,vector<vector<float>>> GoddessCalib::GetEnergyShiftVsPosition ( TH2F* input, int nPoints, float startPoint, float endPoint, double threshold, double peakPos )
{
    if ( input->GetEntries() == 0 ) return std::make_tuple<TGraph*,vector<vector<float>>> ( nullptr, {} );

    string graphName = "SuperX3_" + ( string ) input->GetName();

    float lastBinCenter = input->GetYaxis()->GetBinCenter ( input->GetYaxis()->GetLast() );

    enShiftVsPosGraphs[graphName] = new TGraph ( nPoints );
    enShiftVsPosGraphs[graphName]->SetName ( graphName.c_str() );
    enShiftVsPosGraphs[graphName]->SetTitle ( Form ( "Energy Shift vs. Position for %s", graphName.c_str() ) );

    float projWidth = ( endPoint-startPoint ) / nPoints;

    TH1D* projY = 0;

    float recenter = ( startPoint + endPoint ) / 2.;
    float normalize = endPoint - startPoint;

    vector<vector<float>> overlapCoords;
    overlapCoords.clear();

    vector<float> tempOC;
    tempOC.clear();

    for ( int i = 0; i < nPoints; i++ )
    {
        int projBin = input->GetXaxis()->FindBin ( startPoint + projWidth * ( i + 1/2. ) );

        projY = GetPosCalProjY ( input, projBin, projWidth );

        projY->GetXaxis()->SetRangeUser ( threshold, lastBinCenter );

        int binMax = projY->GetMaximumBin();

        float relPos = startPoint + projWidth * ( i + 1/2. );

        float shiftCoeff = peakPos / projY->GetBinCenter ( binMax );

        enShiftVsPosGraphs[graphName]->SetPoint ( i, ( relPos-recenter ) / normalize, shiftCoeff );

        if ( i > 0 )
        {
            double prevX, prevY;

            enShiftVsPosGraphs[graphName]->GetPoint ( i-1, prevX, prevY );

            if ( shiftCoeff - prevY >= 0.1 ) tempOC = GetOverlapPoints ( input, relPos - 0.2, relPos + 0.2, threshold, false );
            else if ( shiftCoeff - prevY <= -0.1 ) tempOC = GetOverlapPoints ( input, relPos - 0.2, relPos + 0.2, threshold, true );

            if ( tempOC.size() > 0 )
            {
                tempOC.push_back ( ( tempOC[0]-recenter ) / normalize );
                tempOC.push_back ( ( tempOC[2]-recenter ) / normalize );

                string overlapGrName = graphName + ( string ) Form ( "_jump_at_%0.3f", tempOC[8] + 0.001 );

                enShiftVsPosGraphs[overlapGrName] = new TGraph ( 2 );

                enShiftVsPosGraphs[overlapGrName]->SetPoint ( 0, tempOC[4], tempOC[5] );
                enShiftVsPosGraphs[overlapGrName]->SetPoint ( 1, tempOC[6], tempOC[7] );

                enShiftVsPosGraphs[overlapGrName]->SetName ( overlapGrName.c_str() );
                enShiftVsPosGraphs[overlapGrName]->SetTitle ( Form ( "Regime switch equation for %s", graphName.c_str() ) );

                overlapCoords.push_back ( tempOC );

                tempOC.clear();
            }
        }
    }

    if ( isnan ( enShiftVsPosGraphs[graphName]->GetXaxis()->GetXmin() ) && isnan ( enShiftVsPosGraphs[graphName]->GetXaxis()->GetXmax() ) ) return std::make_tuple<TGraph*,vector<vector<float>>> ( nullptr, {} );

    return std::make_tuple ( enShiftVsPosGraphs[graphName], overlapCoords );
}

void GoddessCalib::GetStripsEdges ( TH2F* input, int projCenterBin, int projWidth, double peakPos, double threshold, int shiftGrNPoints, bool drawResults )
{
    string hname = input->GetName();

    cout << "Retreiving the edges of sector " << hname.substr ( 0, hname.find ( "_" ) ) << " strip #" << hname.substr ( hname.find ( "_" ) +1 ) << " ..." << endl;

    TF1* lfit = FitEdges ( input, projCenterBin, projWidth, false );
    TF1* rfit = FitEdges ( input, projCenterBin, projWidth, true );

    float leftEdge = lfit->GetParameter ( 1 ) - TMath::Sqrt ( -2*pow ( lfit->GetParameter ( 2 ),2 ) * TMath::Log ( 0.7 ) );
    float rightEdge = rfit->GetParameter ( 1 ) - TMath::Sqrt ( -2*pow ( rfit->GetParameter ( 2 ),2 ) * TMath::Log ( 0.7 ) );

    std::tuple<TGraph*, vector<vector<float>>> eShiftVsPosRes = GetEnergyShiftVsPosition ( input, shiftGrNPoints, leftEdge, rightEdge, threshold, peakPos );

    TGraph* enShiftGraph = std::get<0> ( eShiftVsPosRes );
    vector<vector<float>> overlapCoords = std::get<1> ( eShiftVsPosRes );

    if ( drawResults )
    {
        TCanvas* newCanvas = new TCanvas ( Form ( "c_%s", hname.c_str() ) );

        newCanvas->Divide ( 3, 1 );

        newCanvas->GetPad ( 1 )->cd();

        input->Draw ( "colz" );

        newCanvas->GetPad ( 2 )->cd();

        GetPosCalProjX ( input, projCenterBin, projWidth )->Draw();
        lfit->Draw ( "same" );
        rfit->Draw ( "same" );

        if ( enShiftGraph != nullptr )
        {
            enShiftGraph->SetMarkerSize ( 2 );
            enShiftGraph->SetMarkerColor ( 4 );
            enShiftGraph->SetMarkerStyle ( 5 );

            if ( overlapCoords.size() > 0 )
            {
                for ( unsigned int i = 0; i < overlapCoords.size(); i++ )
                {
                    newCanvas->GetPad ( 1 )->cd();

                    TMarker* m1 = new TMarker();
                    m1->SetMarkerStyle ( 3 );
                    m1->SetMarkerSize ( 2 );
                    m1->SetMarkerColor ( 2 );

                    m1->SetX ( overlapCoords[i][0] );
                    m1->SetY ( overlapCoords[i][1] );

                    m1->Draw();

                    TMarker* m2 = new TMarker();
                    m2->SetMarkerStyle ( 3 );
                    m2->SetMarkerSize ( 2 );
                    m2->SetMarkerColor ( 2 );

                    m2->SetX ( overlapCoords[i][2] );
                    m2->SetY ( overlapCoords[i][3] );

                    m2->Draw();

                    cout << "*** Searching for points between " << std::min ( overlapCoords[i][8], overlapCoords[i][9] ) << " and " << std::max ( overlapCoords[i][8], overlapCoords[i][9] ) << " ***\n";

                    int realPointNum = 0;

                    for ( int j = 0; j < enShiftGraph->GetN(); j++ )
                    {
                        double pos, en;

                        enShiftGraph->GetPoint ( j, pos, en );

                        if ( pos >= std::min ( overlapCoords[i][8], overlapCoords[i][9] ) && pos <= std::max ( overlapCoords[i][8], overlapCoords[i][9] ) )
                        {
                            cout << "Removing point #" << realPointNum << " with coordinates ( " << pos << " , " << en << " )\n";

                            enShiftGraph->RemovePoint ( j );

                            j--;
                        }

                        realPointNum++;
                    }

                    cout << "**************************\n";
                }
            }

            newCanvas->GetPad ( 3 )->cd();

            enShiftGraph->Draw ( "ALP" );
        }
    }

    return;
}

void GoddessCalib::GetStripsEdges ( int projCenterBin, int projWidth, double peakPos, double threshold, int shiftGrNPoints, bool drawResults )
{
    for ( auto itr = resStripsPosCalGraphsMap.begin(); itr != resStripsPosCalGraphsMap.end(); itr++ )
    {
        GetStripsEdges ( itr->second, projCenterBin, projWidth, peakPos, threshold, shiftGrNPoints, drawResults );
    }

    return;
}

void GoddessCalib::GetStripsEdges ( TFile* input, string sectorsList, double projWinMin, double projWinMax, double peakPos, double threshold, int shiftGrNPoints, bool drawResults )
{
    vector<int> sectors = DecodeNumberString ( sectorsList );

    auto lOK = input->GetListOfKeys();

    for ( int i = 0; i < lOK->GetSize(); i++ )
    {
        TH2F* hist = dynamic_cast<TH2F*> ( input->Get ( lOK->At ( i )->GetName() ) );

        if ( hist != nullptr )
        {
            string hname = hist->GetName();

            if ( ! ( hname[0] == 'U' || hname[0] == 'D' ) && ! ( hname.find ( "_" ) == 2 || hname.find ( "_" ) == 3 ) ) continue;

            unsigned int sect = std::stoi ( hname.substr ( 1, hname.find_first_of ( "_" ) - 1 ) );

            if ( std::find ( sectors.begin(), sectors.end(), sect ) == sectors.end() ) continue;

            double binWidth = hist->GetYaxis()->GetBinWidth ( 0 );

            int projWidth = ( projWinMax-projWinMin ) / binWidth;

            int projCenterBin = hist->GetYaxis()->FindBin ( projWinMin + ( projWinMax-projWinMin ) /2. );

            GetStripsEdges ( hist, projCenterBin, projWidth, peakPos, threshold, shiftGrNPoints, drawResults );
        }
    }

    return;
}

void GoddessCalib::GetStripsEdges ( TFile* input, string sectorsList, int projWidth, double threshold, double peakPos, int shiftGrNPoints, bool drawResults )
{
    vector<int> sectors = DecodeNumberString ( sectorsList );

    auto lOK = input->GetListOfKeys();

    for ( int i = 0; i < lOK->GetSize(); i++ )
    {
        TH2F* hist = dynamic_cast<TH2F*> ( input->Get ( lOK->At ( i )->GetName() ) );

        if ( hist != nullptr )
        {
            string hname = hist->GetName();

            if ( ! ( hname[0] == 'U' || hname[0] == 'D' ) && ! ( hname.find ( "_" ) == 2 || hname.find ( "_" ) == 3 ) ) continue;

            unsigned int sect = std::stoi ( hname.substr ( 1, hname.find_first_of ( "_" ) - 1 ) );

            if ( std::find ( sectors.begin(), sectors.end(), sect ) == sectors.end() ) continue;

            int binMaxY = GetPosCalEnBinMax ( hist, threshold );

            GetStripsEdges ( hist, binMaxY, projWidth, peakPos, threshold, shiftGrNPoints, drawResults );
        }
    }

    return;
}

// --------------------------------------------- QQQ5 functions ---------------------------------- //

vector<std::map<int, TH1F*>> hEn_QQQ5UA;
vector<std::map<int, TH1F*>> hEn_QQQ5UB;
vector<std::map<int, TH1F*>> hEn_QQQ5UC;
vector<std::map<int, TH1F*>> hEn_QQQ5UD;

vector<std::map<int, TH1F*>> hQVal_QQQ5UA;
vector<std::map<int, TH1F*>> hQVal_QQQ5UB;
vector<std::map<int, TH1F*>> hQVal_QQQ5UC;
vector<std::map<int, TH1F*>> hQVal_QQQ5UD;

TH2F* hEn_vs_strip_QQQ5UA;
TH2F* hEn_vs_strip_QQQ5UB;
TH2F* hEn_vs_strip_QQQ5UC;
TH2F* hEn_vs_strip_QQQ5UD;

TH2F* hQval_vs_strip_QQQ5UA;
TH2F* hQval_vs_strip_QQQ5UB;
TH2F* hQval_vs_strip_QQQ5UC;
TH2F* hQval_vs_strip_QQQ5UD;

TH2F* hEn_vs_strip_QQQ5UA_mod;
TH2F* hEn_vs_strip_QQQ5UB_mod;
TH2F* hEn_vs_strip_QQQ5UC_mod;
TH2F* hEn_vs_strip_QQQ5UD_mod;

TH2F* hQval_vs_strip_QQQ5UA_mod;
TH2F* hQval_vs_strip_QQQ5UB_mod;
TH2F* hQval_vs_strip_QQQ5UC_mod;
TH2F* hQval_vs_strip_QQQ5UD_mod;

void GoddessCalib::LoadInternalCalib ( string fileName )
{
    std::ifstream input ( fileName.c_str(), std::ios_base::in );

    if ( !input.is_open() )
    {
        cerr << "Unable to open file: " << fileName << "\n";

        return;
    }

    string dump;

    string readWord1, sectorID, readWord2, readWord3;

    std::istringstream readLine;

    std::getline ( input, dump );

    readLine.str ( dump );

    readLine >> readWord1 >> sectorID >> readWord2 >> readWord3;

    if ( readWord1 != "QQQ5" || readWord2 != "Internal" || readWord3 != "Calib" )
    {
        cerr << "File used as an input doesn't have the proper format (check out the template)...\n";

        return;
    }

    string mapID = "QQQ5 " + sectorID;

    vector<double> newCoeffs;

    for ( int i = 0; i < 32; i++ )
    {
        newCoeffs.push_back ( 100 );
    }

    while ( std::getline ( input, dump ) )
    {
        if ( dump.empty() || dump.find_first_of ( "0123456789" ) == string::npos ) continue;

        double coeff;
        int stripNbr;

        readLine.clear();

        readLine.str ( dump );
        readLine >> stripNbr >> coeff;

//         cout << "Read: " << stripNbr << "    " << coeff << "\n";

        if ( stripNbr >= 0 && stripNbr <= 31 ) newCoeffs[stripNbr] = coeff;
        else
        {
            cerr << "Warning: strip number " << stripNbr << " doesn't exists!\n";
        }
    }

    endcapsStripsCalMap[mapID] = newCoeffs;

    return;
}

float GetRatioGSvsFirstEx ( string inputName, float minAngle, float maxAngle )
{
    std::ifstream input;
    input.open ( inputName.c_str(), std::ios::in );

    if ( !input.is_open() )
    {
        cerr << "Failed to open file: " << inputName << " ...\n";
        return 0.0;
    }

    map<float, double> gsMap, firstExMap;
    map<float, double>* buffMap = 0;

    string readLine;
    std::istringstream iss;

    float angle;
    double crossSection;

    while ( std::getline ( input, readLine ) )
    {
        if ( readLine.empty() ) continue;

        if ( readLine.find ( "Ground State" ) != string::npos )
        {
            buffMap = &gsMap;
            continue;
        }
        else if ( readLine.find ( "First Excited State" ) != string::npos )
        {
            buffMap = &firstExMap;
            continue;
        }

        if ( readLine.find_first_of ( "0123456789" ) == string::npos ) continue;

        iss.clear();
        iss.str ( readLine );

        iss >> angle >> crossSection;

//         cout << angle << "    /    " << crossSection << "\n";

        if ( buffMap != nullptr ) ( *buffMap ) [angle] = crossSection;
    }

    TGraph* gsGraph = new TGraph ( gsMap.size() );

    int counter = 0;

    for ( auto itr = gsMap.begin(); itr != gsMap.end(); itr++ )
    {
        gsGraph->SetPoint ( counter, itr->first, itr->second );

        counter++;
    }

    TGraph* fstExGraph = new TGraph ( firstExMap.size() );

    counter = 0;

    for ( auto itr = firstExMap.begin(); itr != firstExMap.end(); itr++ )
    {
        fstExGraph->SetPoint ( counter, itr->first, itr->second );

        counter++;
    }

//     gsGraph->Draw ( "AP" );
//     fstExGraph->Draw ( "same" );

    auto gsMinIndexItr = gsMap.upper_bound ( minAngle );
    gsMinIndexItr--;

    auto gsMaxIndexItr = gsMap.upper_bound ( maxAngle );
    gsMaxIndexItr--;

    int gsMinIndex = std::distance ( gsMap.begin(), gsMinIndexItr );
    int gsMaxIndex = std::distance ( gsMap.begin(), gsMaxIndexItr );

    cout << "Integrating the Ground State TGraph between index " << gsMinIndex << " ( " << gsMinIndexItr->first << " ) and " << gsMaxIndex << " ( " << gsMaxIndexItr->first << " ) ...\n";

    double gsIntegral = gsGraph->Integral ( gsMinIndex, gsMaxIndex );

    auto fstExMinIndexItr = firstExMap.upper_bound ( minAngle );
    fstExMinIndexItr--;

    auto fstExMaxIndexItr = firstExMap.upper_bound ( maxAngle );
    fstExMaxIndexItr--;

    int fstExMinIndex = std::distance ( firstExMap.begin(), fstExMinIndexItr );
    int fstExMaxIndex = std::distance ( firstExMap.begin(), fstExMaxIndexItr );

    cout << "Integrating the First Excited State TGraph between index " << fstExMinIndex << " ( " << fstExMinIndexItr->first << " ) and " << fstExMaxIndex << " ( " << fstExMaxIndexItr->first << " ) ...\n";

    double fstExIntegral = fstExGraph->Integral ( fstExMinIndex, fstExMaxIndex );

    double ratio = fstExIntegral/gsIntegral;

    return ratio;
}

TH1F* GoddessCalib::AddAllStrips ( vector<std::map<int, TH1F*>>* hists, int modCoeff_ )
{
    TH1F* hSum = ( TH1F* ) ( hists->at ( 0 ) ) [modCoeff_]->Clone();

    if ( hSum == nullptr )
    {
        cerr << "Histograms required have not been initialized!...\n";
        return nullptr;
    }

    for ( int i = 1; i < 32; i++ )
    {
        hSum->Add ( ( hists->at ( i ) ) [modCoeff_] );
    }

    return hSum;
}

TH1F* GoddessCalib::AddAllStrips ( vector<std::map<int, TH1F*>>* hists, double modCoeffs[32] )
{
    TH1F* hSum = ( TH1F* ) ( hists->at ( 0 ) ) [modCoeffs[0]]->Clone();

    if ( hSum == nullptr )
    {
        cerr << "Histograms required have not been initialized!...\n";
        return nullptr;
    }

    for ( int i = 1; i < 32; i++ )
    {
        hSum->Add ( ( hists->at ( i ) ) [modCoeffs[i]] );
    }

    return hSum;
}

TH1F* GoddessCalib::AddAllStrips ( vector<std::map<int, TH1F*>>* hists, vector<double> modCoeffs )
{
    TH1F* hSum = ( TH1F* ) ( hists->at ( 0 ) ) [modCoeffs[0]]->Clone();

    if ( hSum == nullptr )
    {
        cerr << "Histograms required have not been initialized!...\n";
        return nullptr;
    }

    for ( int i = 1; i < 32; i++ )
    {
        hSum->Add ( ( hists->at ( i ) ) [modCoeffs[i]] );
    }

    return hSum;
}

TH1F* GoddessCalib::AddAllStrips ( vector< std::map< int, TH1F* > >* hists, string sector )
{
    vector<double>* coeffMap = &endcapsStripsCalMap[ ( "QQQ5 " + sector )];

    return AddAllStrips ( hists, *coeffMap );
}

TF1* GoddessCalib::FitQVal ( TH1F* hist, vector< string > mean, float fwhm_min, float fwhm_max, float minBound, float maxBound, bool verbose )
{
    if ( mean.size() == 0 ) return nullptr;

    string funcFormula = "[0] + [1] * x + [3] * TMath::Exp ( -pow ( x - [4],2 ) / pow ( 2 * [2],2 ) )";

    vector<float> meansF;
    float minMean, maxMean;

    int* dependantFrom = new int[mean.size()];

    if ( mean.size() > 0 )
    {
        dependantFrom[0] = -1;
        meansF.push_back ( std::stof ( mean[0].substr ( 5 ) ) );

        minMean = meansF[0];
        maxMean = meansF[0];

        for ( unsigned int i = 1; i < mean.size(); i++ )
        {
            string newMeanStr = mean[i].substr ( 5 );

            bool isDependant = false;
            dependantFrom[i] = -1;

            std::size_t oBracketPos = newMeanStr.find_first_of ( "[" );

            if ( oBracketPos != string::npos )
            {
                isDependant = true;

                std::size_t cBracketPos = newMeanStr.find_first_of ( "]" );

                if ( cBracketPos == string::npos || cBracketPos < oBracketPos ) return nullptr;

                dependantFrom[i] = std::stoi ( newMeanStr.substr ( oBracketPos+1, cBracketPos - oBracketPos - 1 ) );

                meansF.push_back ( std::stof ( newMeanStr.substr ( cBracketPos+1 ) ) );
            }
            else meansF.push_back ( std::stoi ( newMeanStr ) );

            float newMean = dependantFrom[i] == -1 ? meansF[i] : meansF[dependantFrom[i]] + meansF[i];

            if ( newMean < minMean ) minMean = newMean;
            else if ( newMean > maxMean ) maxMean = newMean;

            if ( dependantFrom[i] == -1 ) funcFormula += ( string ) Form ( " + [%d] * TMath::Exp ( -pow ( x - [%d],2 ) / pow ( 2 * [2],2 ) )", 5 + ( i-1 ) * 2, 6 + ( i-1 ) * 2 );
            else funcFormula += ( string ) Form ( " + [%d] * TMath::Exp ( -pow ( x - ([%d]+[%d]),2 ) / pow ( 2 * [2],2 ) )", 5 + ( i-1 ) * 2, 4 + dependantFrom[i] * 2, 6 + ( i-1 ) * 2 );
        }
    }
    else return nullptr;

    if ( verbose )
    {
        cout << "Final Fit Formula:\n";
        cout << funcFormula << endl;
        cout << "***********************************************************************\n";
    }

    TF1* fitFunc = new TF1 ( "fitFunc", funcFormula.c_str(), -20, 20 );

    float fitMin, fitMax;

    float fwhm_mean = ( fwhm_min+fwhm_max ) /2.;

    if ( minBound == 0 ) fitMin = minMean - fwhm_mean*3;
    else fitMin = minBound;

    if ( maxBound == 0 ) fitMax = maxMean + fwhm_mean*3;
    else fitMax = maxBound;

    fitFunc->SetParameter ( 2, fwhm_mean );
    fitFunc->SetParLimits ( 2, fwhm_min, fwhm_max );

    fitFunc->SetParameter ( 0, 1 );
    fitFunc->SetParameter ( 1, 0.1 );

    for ( unsigned int i = 0; i < mean.size(); i++ )
    {
        int parNum = 3 + 2*i;

        fitFunc->SetParameter ( parNum, 10 );
        fitFunc->SetParameter ( parNum+1, meansF[i] );

        fitFunc->SetParLimits ( parNum, 0, 1e6 );
        if ( verbose ) cout << "Setting limits for parameter #" << parNum+1 << endl;
        if ( dependantFrom[i] == -1 )
        {
            fitFunc->SetParLimits ( parNum+1, meansF[i]-0.4, meansF[i]+0.4 );

            if ( verbose ) cout << "Min Bound = " << meansF[i]-0.4 << " / Max Bound = " << meansF[i]+0.4 << endl;
        }
        else
        {
            fitFunc->SetParLimits ( parNum+1, meansF[i] - 0.1, meansF[i] + 0.1 );

            if ( verbose ) cout << "Min Bound = " << meansF[i]-0.1 << " / Max Bound = " << meansF[i]+0.1 << endl;
        }
    }

    hist->Fit ( fitFunc, "Q", "", fitMin, fitMax );

    if ( minBound == 0 || maxBound == 0 )
    {
        minMean = fitFunc->GetParameter ( 4 + 2 * ( mean.size()-1 ) );
        maxMean = fitFunc->GetParameter ( 4 );

        for ( unsigned int i = 0; i < mean.size(); i++ )
        {
            if ( minMean > fitFunc->GetParameter ( 4 + 2 * i ) ) minMean = fitFunc->GetParameter ( 4 + 2 * i );
            else if ( maxMean < fitFunc->GetParameter ( 4 + 2 * i ) ) maxMean = fitFunc->GetParameter ( 4 + 2 * i );
        }

        fitMin = minBound == 0 ? ( minMean - 3 * fitFunc->GetParameter ( 2 ) ) : minBound;
        fitMax = maxBound == 0 ? ( maxMean + 10 * fitFunc->GetParameter ( 2 ) ) : maxBound;

        hist->Fit ( fitFunc, "Q", "", fitMin, fitMax );
    }

    if ( verbose )
    {
        cout << "Fit Results: \n";
        for ( unsigned int i = 0; i < mean.size(); i++ )
        {
            cout << "Peak #" << std::setw ( 2 ) << std::left << i << " : ";
            if ( dependantFrom[i] == -1 ) cout << fitFunc->GetParameter ( 4 + 2*i ) << " MeV\n";
            else
            {
                cout << fitFunc->GetParameter ( 4 + dependantFrom[i]*i ) + fitFunc->GetParameter ( 4 + 2*i ) << " MeV ( Depends from Peak #" << dependantFrom[i] << " : ";
                cout << " energy difference = " << fabs ( fitFunc->GetParameter ( 4 + 2*i ) );
                cout << " / Ratio = " << fitFunc->GetParameter ( 3 + 2*i ) / fitFunc->GetParameter ( 3 + dependantFrom[i]*i ) << " )\n";
            }
        }
        cout << "Sigma = " << fitFunc->GetParameter ( 2 ) << " MeV\n";
    }

    return fitFunc;
}

TF1* GoddessCalib::FitQValGS ( TH1F* hist, vector<float> mean, float fwhm, float peakRatio, float minBound, float maxBound, bool verbose )
{
    if ( mean.size() == 0 ) return nullptr;

    string funcFormula = "[0] + [1] * x + [4] * TMath::Exp ( -pow ( x - [5],2 ) / pow ( 2 * [3],2 ) ) + [4]*[2] * TMath::Exp ( -pow ( x - ([5]-0.288),2 ) / pow ( 2 * [3],2 ) )";

    float minMean = mean[0], maxMean = mean[0];

    if ( mean.size() > 0 )
    {
        for ( unsigned int i = 1; i < mean.size(); i++ )
        {
            if ( mean[i] < minMean ) minMean = mean[i];
            else if ( mean[i] > maxMean ) maxMean = mean[i];

            funcFormula += ( string ) Form ( " + [%d] * TMath::Exp ( -pow ( x - [%d],2 ) / pow ( 2 * [3],2 ) )", 6 + ( i-1 ) * 2, 7 + ( i-1 ) * 2 );
        }
    }

    TF1* fitFunc = new TF1 ( "fitFunc", funcFormula.c_str(), -20, 20 );

    float fitMin, fitMax;

    if ( minBound == 0 ) fitMin = minMean - fwhm;
    else fitMin = minBound;

    if ( maxBound == 0 ) fitMax = maxMean + fwhm*3;
    else fitMax = maxBound;

    fitFunc->SetParameter ( 2, peakRatio );
    fitFunc->SetParLimits ( 2, 0, 1 );

    fitFunc->SetParameter ( 0, 1 );
    fitFunc->SetParameter ( 1, 0.1 );

    fitFunc->SetParameter ( 3, fwhm );
    fitFunc->SetParLimits ( 3, 0.01, 0.1 );

    for ( unsigned int i = 0; i < mean.size(); i++ )
    {
        int parNum = 4 + 2*i;

        fitFunc->SetParameter ( parNum, 10 );
        fitFunc->SetParameter ( parNum+1, mean[i] );

        fitFunc->SetParLimits ( parNum, 0, 1e6 );
    }

    hist->Fit ( fitFunc, "Q", "", fitMin, fitMax );

    if ( minBound == 0 || maxBound == 0 )
    {
        minMean = fitFunc->GetParameter ( 5 + 2 * ( mean.size()-1 ) );
        maxMean = fitFunc->GetParameter ( 5 );

        for ( unsigned int i = 0; i < mean.size(); i++ )
        {
            if ( minMean > fitFunc->GetParameter ( 5 + 2 * i ) ) minMean = fitFunc->GetParameter ( 5 + 2 * i );
            else if ( maxMean < fitFunc->GetParameter ( 5 + 2 * i ) ) maxMean = fitFunc->GetParameter ( 5 + 2 * i );
        }

        fitMin = minBound == 0 ? ( minMean - 3 * fitFunc->GetParameter ( 3 ) ) : minBound;
        fitMax = maxBound == 0 ? ( maxMean + 10 * fitFunc->GetParameter ( 3 ) ) : maxBound;

        hist->Fit ( fitFunc, "Q", "", fitMin, fitMax );
    }

    if ( verbose )
    {
        cout << "Fit Results for the G.S. and 1st Exited State: \n";
        cout << std::setw ( 16 ) << std::left << "G.S. @ " << fitFunc->GetParameter ( 5 ) << " MeV\n";
//         cout << std::setw ( 16 ) << std::left << "1st Ex. State @ " << fitFunc->GetParameter ( 7 ) << " MeV\n";
//         cout << "Energy Difference = " << fabs ( fitFunc->GetParameter ( 7 ) - fitFunc->GetParameter ( 5 ) ) << " MeV\n";
        cout << "Sigma = " << fitFunc->GetParameter ( 3 ) << " MeV\n";
        cout << "Peak Ratio = " << fitFunc->GetParameter ( 2 ) << " MeV\n";
    }

    return fitFunc;
}

TH2F* GoddessCalib::GetQvalVsStrip ( vector<std::map<int, TH1F*>>* src, TH2F* dest, int modCoeff_ )
{
    dest->Reset();

    for ( int i = 0; i < 32; i++ )
    {
        unsigned int nBins = ( src->at ( i ) ) [modCoeff_]->GetNbinsX();

        for ( unsigned int j = 0; j < nBins; j++ )
        {
            dest->Fill ( i, ( src->at ( i ) ) [modCoeff_]->GetBinCenter ( j ), ( src->at ( i ) ) [modCoeff_]->GetBinContent ( j ) );
        }
    }

    return dest;
}

TH2F* GoddessCalib::GetQvalVsStrip ( vector<std::map<int, TH1F*>>* src, TH2F* dest, double modCoeffs[32] )
{
    dest->Reset();

    for ( int i = 0; i < 32; i++ )
    {
        unsigned int nBins = ( src->at ( i ) ) [modCoeffs[i]]->GetNbinsX();

        for ( unsigned int j = 0; j < nBins; j++ )
        {
            dest->Fill ( i, ( src->at ( i ) ) [modCoeffs[i]]->GetBinCenter ( j ), ( src->at ( i ) ) [modCoeffs[i]]->GetBinContent ( j ) );
        }
    }

    return dest;
}

TH2F* GoddessCalib::GetQvalVsStrip ( vector<std::map<int, TH1F*>>* src, TH2F* dest, vector<double> modCoeffs )
{
    dest->Reset();

    for ( int i = 0; i < 32; i++ )
    {
        unsigned int nBins = ( src->at ( i ) ) [modCoeffs[i]]->GetNbinsX();

        for ( unsigned int j = 0; j < nBins; j++ )
        {
            dest->Fill ( i, ( src->at ( i ) ) [modCoeffs[i]]->GetBinCenter ( j ), ( src->at ( i ) ) [modCoeffs[i]]->GetBinContent ( j ) );
        }
    }

    return dest;
}

TH2F* GoddessCalib::GetQvalVsStrip ( vector< std::map< int, TH1F* > >* src, TH2F* dest, string sector )
{
    vector<double>* coeffMap = &endcapsStripsCalMap[ ( "QQQ5 " + sector )];

    return GetQvalVsStrip ( src, dest, *coeffMap );
}

void GoddessCalib::InitSiEnergySpectra ( unsigned int nBins, int binMin, int binMax )
{
    for ( int i = 0; i < 32; i++ )
    {
        float modCoeff = minModEndcaps;

        std::map<int, TH1F*> newMapEntryA, newMapEntryB, newMapEntryC, newMapEntryD;

        while ( modCoeff <= maxModEndcaps )
        {
            newMapEntryA[modCoeff] = new TH1F ( Form ( "hEn_QQQ5_UA_s%d_mod%f", i, modCoeff ), Form ( "Si Energy QQQ5 UA strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
            newMapEntryB[modCoeff] = new TH1F ( Form ( "hEn_QQQ5_UB_s%d_mod%f", i, modCoeff ), Form ( "Si Energy QQQ5 UB strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
            newMapEntryC[modCoeff] = new TH1F ( Form ( "hEn_QQQ5_UC_s%d_mod%f", i, modCoeff ), Form ( "Si Energy QQQ5 UC strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
            newMapEntryD[modCoeff] = new TH1F ( Form ( "hEn_QQQ5_UD_s%d_mod%f", i, modCoeff ), Form ( "Si Energy QQQ5 UD strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );

            modCoeff += 1;
        }

        hEn_QQQ5UA.push_back ( newMapEntryA );
        hEn_QQQ5UB.push_back ( newMapEntryB );
        hEn_QQQ5UC.push_back ( newMapEntryC );
        hEn_QQQ5UD.push_back ( newMapEntryD );
    }

    hEn_vs_strip_QQQ5UA = new TH2F ( "hEn_vs_strip_QQQ5UA", "Si Energy vs. Strip# QQQ5 UA", 32, 0, 32, nBins, binMin, binMax );
    hEn_vs_strip_QQQ5UB = new TH2F ( "hEn_vs_strip_QQQ5UB", "Si Energy vs. Strip# QQQ5 UB", 32, 0, 32, nBins, binMin, binMax );
    hEn_vs_strip_QQQ5UC = new TH2F ( "hEn_vs_strip_QQQ5UC", "Si Energy vs. Strip# QQQ5 UC", 32, 0, 32, nBins, binMin, binMax );
    hEn_vs_strip_QQQ5UD = new TH2F ( "hEn_vs_strip_QQQ5UD", "Si Energy vs. Strip# QQQ5 UD", 32, 0, 32, nBins, binMin, binMax );

    hEn_vs_strip_QQQ5UA_mod = new TH2F ( "hEn_vs_strip_QQQ5UA_mod", "Si Energy vs. Strip# QQQ5 UA mod", 32, 0, 32, nBins, binMin, binMax );
    hEn_vs_strip_QQQ5UB_mod = new TH2F ( "hEn_vs_strip_QQQ5UB_mod", "Si Energy vs. Strip# QQQ5 UB mod", 32, 0, 32, nBins, binMin, binMax );
    hEn_vs_strip_QQQ5UC_mod = new TH2F ( "hEn_vs_strip_QQQ5UC_mod", "Si Energy vs. Strip# QQQ5 UC mod", 32, 0, 32, nBins, binMin, binMax );
    hEn_vs_strip_QQQ5UD_mod = new TH2F ( "hEn_vs_strip_QQQ5UD_mod", "Si Energy vs. Strip# QQQ5 UD mod", 32, 0, 32, nBins, binMin, binMax );

    return;
}

void GoddessCalib::InitQValSpectra ( unsigned int nBins, int binMin, int binMax )
{
    for ( int i = 0; i < 32; i++ )
    {
        float modCoeff = minModEndcaps;

        std::map<int, TH1F*> newMapEntryA, newMapEntryB, newMapEntryC, newMapEntryD;

        while ( modCoeff <= maxModEndcaps )
        {
            newMapEntryA[modCoeff] = new TH1F ( Form ( "hQVal_QQQ5_UA_s%d_mod%f", i, modCoeff ), Form ( "Q-Value QQQ5 UA strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
            newMapEntryB[modCoeff] = new TH1F ( Form ( "hQVal_QQQ5_UB_s%d_mod%f", i, modCoeff ), Form ( "Q-Value QQQ5 UB strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
            newMapEntryC[modCoeff] = new TH1F ( Form ( "hQVal_QQQ5_UC_s%d_mod%f", i, modCoeff ), Form ( "Q-Value QQQ5 UC strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
            newMapEntryD[modCoeff] = new TH1F ( Form ( "hQVal_QQQ5_UD_s%d_mod%f", i, modCoeff ), Form ( "Q-Value QQQ5 UD strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );

            modCoeff += 1;
        }

        hQVal_QQQ5UA.push_back ( newMapEntryA );
        hQVal_QQQ5UB.push_back ( newMapEntryB );
        hQVal_QQQ5UC.push_back ( newMapEntryC );
        hQVal_QQQ5UD.push_back ( newMapEntryD );
    }

    hQval_vs_strip_QQQ5UA = new TH2F ( "hQval_vs_strip_QQQ5UA", "Q-Value vs. Strip# QQQ5 UA", 32, 0, 32, nBins, binMin, binMax );
    hQval_vs_strip_QQQ5UB = new TH2F ( "hQval_vs_strip_QQQ5UB", "Q-Value vs. Strip# QQQ5 UB", 32, 0, 32, nBins, binMin, binMax );
    hQval_vs_strip_QQQ5UC = new TH2F ( "hQval_vs_strip_QQQ5UC", "Q-Value vs. Strip# QQQ5 UC", 32, 0, 32, nBins, binMin, binMax );
    hQval_vs_strip_QQQ5UD = new TH2F ( "hQval_vs_strip_QQQ5UD", "Q-Value vs. Strip# QQQ5 UD", 32, 0, 32, nBins, binMin, binMax );

    hQval_vs_strip_QQQ5UA_mod = new TH2F ( "hQval_vs_strip_QQQ5UA_mod", "Q-Value vs. Strip# QQQ5 UA mod", 32, 0, 32, nBins, binMin, binMax );
    hQval_vs_strip_QQQ5UB_mod = new TH2F ( "hQval_vs_strip_QQQ5UB_mod", "Q-Value vs. Strip# QQQ5 UB mod", 32, 0, 32, nBins, binMin, binMax );
    hQval_vs_strip_QQQ5UC_mod = new TH2F ( "hQval_vs_strip_QQQ5UC_mod", "Q-Value vs. Strip# QQQ5 UC mod", 32, 0, 32, nBins, binMin, binMax );
    hQval_vs_strip_QQQ5UD_mod = new TH2F ( "hQval_vs_strip_QQQ5UD_mod", "Q-Value vs. Strip# QQQ5 UD mod", 32, 0, 32, nBins, binMin, binMax );

    return;
}

void GoddessCalib::GenerateEnergyHistPerStrip ( TChain* chain )
{
    if ( geomInfo == nullptr )
    {
        cerr << "Geometry information missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    if ( reacInfo == nullptr )
    {
        cerr << "Reaction information missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    InitSiEnergySpectra ( 200, 0, 10 );
    InitQValSpectra ( 500,-15,10 );

    vector<SiDataBase>* siData = new vector<SiDataBase>;

    chain->SetBranchAddress ( "si", &siData );

    vector<std::map<int, TH1F*>>* hQValPerSector[4];

    hQValPerSector[0] = &hQVal_QQQ5UA;
    hQValPerSector[1] = &hQVal_QQQ5UB;
    hQValPerSector[2] = &hQVal_QQQ5UC;
    hQValPerSector[3] = &hQVal_QQQ5UD;

    vector<std::map<int, TH1F*>>* hEnPerSector[4];

    hEnPerSector[0] = &hEn_QQQ5UA;
    hEnPerSector[1] = &hEn_QQQ5UB;
    hEnPerSector[2] = &hEn_QQQ5UC;
    hEnPerSector[3] = &hEn_QQQ5UD;

    float angle, modCoeff;
    int sector, strip, mult;
    double siEn, newEn, newQval;

    for ( long long int i = 0; i < chain->GetEntries(); i++ )
    {
        if ( i%10000 == 0 )
        {
            cout << "Treated " << std::setw ( 9 ) << i << " / " << std::setw ( 9 ) << chain->GetEntries();
            cout << " ( " << std::setw ( 5 ) << std::fixed << std::setprecision ( 2 ) << ( float ) i/chain->GetEntries() *100. << "% )\r" << std::flush;
        }

        chain->GetEntry ( i );

        for ( unsigned int j = 0; j < siData->size(); j++ )
        {
            angle = siData->at ( j ).Angle ( 1 );
//             qval = siData->at ( j ).QValue ( massBeam, beamEk, massTarget, massEjec );

//             exEn = -qval + qValGsGs;

            sector = siData->at ( j ).sector;
            strip = siData->at ( j ).StripMaxLayer ( 1, false );
            mult = siData->at ( j ).MultLayer ( 1, false );

            siEn = siData->at ( j ).ESumLayer ( 1, false );

            if ( angle != 0 && siEn > 0 && mult == 1 )
            {
                if ( siData->at ( j ).isUpstream && !siData->at ( j ).isBarrel )
                {
                    modCoeff = minModEndcaps;

                    while ( modCoeff <= maxModEndcaps )
                    {
                        newEn = siEn* ( modCoeff/100. );

                        newQval = ( 1+reacInfo->ejecA/reacInfo->recoilA ) * ( newEn ) - ( 1 - reacInfo->beamA/reacInfo->recoilA ) * ( reacInfo->beamEk )
                                  - 2 * TMath::Sqrt ( reacInfo->beamA*reacInfo->ejecA* ( newEn ) * ( reacInfo->beamEk ) ) / reacInfo->recoilA * TMath::Cos ( angle * TMath::Pi() / 180. );

                        ( hEnPerSector[sector]->at ( strip ) ) [modCoeff]->Fill ( newEn );

                        ( hQValPerSector[sector]->at ( strip ) ) [modCoeff]->Fill ( newQval );

                        modCoeff += 1;
                    }
                }
            }
        }
    }

    cout << "\n";

    GetQvalVsStrip ( &hQVal_QQQ5UA, hQval_vs_strip_QQQ5UA );
    GetQvalVsStrip ( &hQVal_QQQ5UB, hQval_vs_strip_QQQ5UB );
    GetQvalVsStrip ( &hQVal_QQQ5UC, hQval_vs_strip_QQQ5UC );
    GetQvalVsStrip ( &hQVal_QQQ5UD, hQval_vs_strip_QQQ5UD );

    return;
}

vector<double> GoddessCalib::AdjustQValSpectrum ( vector<std::map<int, TH1F*>>* hists, float peakPos, float fwhm, string crossSectionInput,
        float minBound, float maxBound, int minModEndcaps_, int maxModEndcaps_, string betterFitMode )
{
    vector<double> finalMods;

    double testMods[32];

    for ( int i = 0; i < 32; i++ )
    {
        finalMods.push_back ( 100 );
    }

    TH1F* sum = AddAllStrips ( hists, finalMods );

    sum->Draw();

    // ---------------------

    double peakRatio = GetRatioGSvsFirstEx ( crossSectionInput, 120, 160 );

    TF1* firstFitFunc = FitQValGS ( sum, {peakPos, peakPos-2}, fwhm, peakRatio, minBound, maxBound );

    double firstChi2, firstMagn, firstMean, firstSigma, firstIntegral, firstHistIntegral, firstOffset, firstSlope;

    firstMagn = firstFitFunc->GetParameter ( 4 );
    firstMean = firstFitFunc->GetParameter ( 5 );
    firstSigma = firstFitFunc->GetParameter ( 3 );

    firstOffset = firstFitFunc->GetParameter ( 0 );
    firstSlope = firstFitFunc->GetParameter ( 1 );

    firstChi2 = firstFitFunc->GetChisquare();

    firstIntegral = firstMagn * TMath::Sqrt ( 2*TMath::Pi() ) * firstSigma;

    double totIntegral = firstFitFunc->Integral ( firstMean - 3*firstSigma, firstMean + 3*firstSigma );
    double bkgrIntegral = 6*firstSigma * ( firstSlope*firstMean + firstOffset ) ;
    firstHistIntegral = totIntegral - bkgrIntegral;

    // ------------------------

    TF1* bestFitFunc = ( TF1* ) firstFitFunc->Clone();

    double bestChi2, bestMagn, bestMean, bestSigma, bestIntegral, bestHistIntegral, bestOffset, bestSlope;

    bestMagn = firstMagn;
    bestMean = firstMean;
    bestSigma = firstSigma;

    bestOffset = firstOffset;
    bestSlope = firstSlope;

    bestChi2 = firstChi2;

    bestIntegral = firstIntegral;

    bestHistIntegral = firstHistIntegral;

    // -------------------------

    TF1* fitFunc;

    double chi2, magn, mean, sigma, integral, histIntegral, offset, slope;

    // -------------------------

    auto linkMap = MakeLinkMap ( "chi2 bestChi2 firstChi2 sigma bestSigma firstSigma magn bestMagn firstMagn gaussIntegral bestGaussIntegral firstGaussIntegral rawIntegral bestRawIntegral firstRawIntegral",
                                 chi2, bestChi2, firstChi2, sigma, bestSigma, firstSigma, magn, bestMagn, firstMagn, integral, bestIntegral, firstIntegral, histIntegral, bestHistIntegral, firstHistIntegral );

    cout << "\n";

    for ( int i = 0; i < 32; i++ )
    {
        std::cout << "Adjusted " << std::setw ( 2 ) << i << " / 32 strips...\r" << std::flush;

        for ( int j = 0; j < 32; j++ )
        {
            testMods[j] = finalMods[j];
        }

        int modCoeff = minModEndcaps_;

        while ( modCoeff <= maxModEndcaps_ )
        {
            testMods[i] = modCoeff;

            TH1F* testSum = AddAllStrips ( hists, testMods );

            fitFunc = FitQValGS ( testSum, {peakPos, peakPos-2}, fwhm, peakRatio, minBound, maxBound );

            magn = fitFunc->GetParameter ( 4 );
            mean = fitFunc->GetParameter ( 5 );
            sigma = fitFunc->GetParameter ( 3 );

            offset = fitFunc->GetParameter ( 0 );
            slope = fitFunc->GetParameter ( 1 );

            chi2 = fitFunc->GetChisquare();

            totIntegral = fitFunc->Integral ( mean - 3*sigma, mean + 3*sigma );
            bkgrIntegral = 6*sigma * ( slope*mean + offset ) ;
            histIntegral = totIntegral - bkgrIntegral;

            integral = magn * TMath::Sqrt ( 2*TMath::Pi() ) * sigma;

            bool goodFit = false;

            goodFit = StringFormulaComparator<double> ( betterFitMode, &linkMap );

            if ( goodFit ) finalMods[i] = modCoeff;

            if ( chi2 < bestChi2 && sigma <= bestSigma && magn >= bestMagn && integral >= bestIntegral && histIntegral >= bestHistIntegral )
            {
                bestFitFunc = ( TF1* ) fitFunc->Clone();

                bestMagn = magn;
                bestMean = mean;
                bestSigma = sigma;

                bestOffset = offset;
                bestSlope = slope;

                bestChi2 = chi2;

                bestIntegral = integral;

                bestHistIntegral = histIntegral;
            }

            modCoeff++;
        }
    }

    cout << "\n";

    for ( int i = 0; i < 8; i++ )
    {
        cout << std::setw ( 2 ) << i << ": " << std::setw ( 6 ) << finalMods[i] << "   |   ";
        cout << std::setw ( 2 ) << i+8 << ": " << std::setw ( 6 ) << finalMods[i+8] << "   |   ";
        cout << std::setw ( 2 ) << i+16 << ": " << std::setw ( 6 ) << finalMods[i+16] << "   |   ";
        cout << std::setw ( 2 ) << i+24 << ": " << std::setw ( 6 ) << finalMods[i+24] << "\n";
    }

    cout << "\n";

    if ( hists == &hQVal_QQQ5UA ) endcapsStripsCalMap["QQQ5 UA"] = finalMods;
    else if ( hists == &hQVal_QQQ5UB ) endcapsStripsCalMap["QQQ5 UB"] = finalMods;
    else if ( hists == &hQVal_QQQ5UC ) endcapsStripsCalMap["QQQ5 UC"] = finalMods;
    else if ( hists == &hQVal_QQQ5UD ) endcapsStripsCalMap["QQQ5 UD"] = finalMods;
    else
    {
        cerr << "Map used as an input is not recognized. Results are not stored...\n";
    }

    return finalMods;
}

// -------------------------------- Position Reconstruction ----------------------------------------- //

void GoddessCalib::PrintOutStripsPositions ( )
{
    cout << "QQQ5 Upstream Strips Positions:\n";

    for ( int sect = 0; sect < 4; sect++ )
    {
        cout << " ** Sector " << sect << " **\n";

        for ( int st = 0; st < 32; st++ )
        {
            cout << "   -> Strip " << st << " : ( " << orrubaStripsPos[1][0][sect][st].X() << " , " <<  orrubaStripsPos[1][0][sect][st].Y() << " , " << orrubaStripsPos[1][0][sect][st].Z() << " )\n";
        }
    }

    cout << "\n\nSuperX3 Upstream Strips Positions:\n";

    for ( int sect = 0; sect < 12; sect++ )
    {
        cout << " ** Sector " << sect << " **\n";

        for ( int st = 0; st < 4; st++ )
        {
            cout << "   -> Strip " << st << " : ( " << orrubaStripsPos[1][1][sect][st].X() << " , " <<  orrubaStripsPos[1][1][sect][st].Y() << " , " << orrubaStripsPos[1][1][sect][st].Z() << " )\n";
        }
    }

    return;
}

void GoddessCalib::FillStripsPositionsArray ( float qqq5OffX, float qqq5OffY, float QQQ5OffZ, float sX3OffX, float sX3OffY, float sX3OffZ )
{
    float barrelRadius = 3.750 * 25.4; //mm
//     float halfBarrelLength = ( 4.375 - 0.7 ) * 25.4; //mm
    float sX3ActiveLength = 75.; //mm
//     float sX3NearFrame = 3.0; //mm

    TVector3 zAxis ( 0,0,1 );

    for ( int i = 0; i < 12; i++ )
    {
//         float barrelDet_spacing = 4.8;

        TVector3 barrelDet_offset ( sX3OffX, sX3OffY, sX3OffZ );

        TVector3 refSX3D_sect0 ( 0 + barrelDet_offset.X(), barrelRadius + barrelDet_offset.Y(), sX3ActiveLength/2. + barrelDet_offset.Z() );

        float SX3_width = 40.3; //mm
//         float SX3_length = 75.; //mm

        for ( int j = 0; j < 4; j++ )
        {
            orrubaStripsPos[0][1][i][j].SetXYZ ( 0,0,1 );
            orrubaStripsPos[1][1][i][j].SetXYZ ( 0,0,1 );

            orrubaStripsPos[0][1][i][j].SetTheta ( refSX3D_sect0.Angle ( zAxis ) );
            orrubaStripsPos[1][1][i][j].SetTheta ( TMath::Pi() - refSX3D_sect0.Angle ( zAxis ) );

            orrubaStripsPos[0][1][i][j].SetPhi ( TMath::PiOver2() + i / 12. * TMath::TwoPi() );
            orrubaStripsPos[1][1][i][j].SetPhi ( TMath::PiOver2() + i / 12. * TMath::TwoPi() );

            orrubaStripsPos[0][1][i][j].SetMag ( refSX3D_sect0.Mag() );
            orrubaStripsPos[1][1][i][j].SetMag ( refSX3D_sect0.Mag() );

            orrubaStripsPos[0][1][i][j].SetRotationZ ( i / 12. * TMath::TwoPi() );
            orrubaStripsPos[1][1][i][j].SetRotationZ ( i / 12. * TMath::TwoPi() );

            TVector3 pStPosRefDetCenter ( ( ( 3./8. ) * SX3_width ) - ( j * SX3_width/4. ), 0, 0 ); // Ref taken at the center of the SX3 so strip 0 offset is 1 and a half strip width toward positive X direction

            pStPosRefDetCenter.SetPhi ( pStPosRefDetCenter.Phi() + orrubaStripsPos[0][1][i][j].RotZ() );

            orrubaStripsPos[0][1][i][j] += pStPosRefDetCenter;
            orrubaStripsPos[1][1][i][j] += pStPosRefDetCenter;
        }
    }

    for ( int i = 0; i < 4; i++ )
    {
//         float QQQ5_spacing = 4.0;

        TVector3 QQQ5DA_orig_offset ( qqq5OffX, qqq5OffY, QQQ5OffZ ); // everything in mm

        TVector3 refQQQ5D_sectA = QQQ5DA_orig_offset;

//         float QQQ5_active_length = 56.8; // mm

        float firstStripWidth = 2.55;

        TVector3 firstStripOffset ( 0, 25.2 + firstStripWidth/2., 0 ); // everything in mm

        TVector3 prevStripRefDetCenter = firstStripOffset;

        orrubaStripsPos[0][0][i][0] += firstStripOffset;
        orrubaStripsPos[1][0][i][0] += firstStripOffset;

        for ( int j = 1; j < 32; j++ )
        {
            orrubaStripsPos[0][0][i][j].SetXYZ ( 0,0,1 );
            orrubaStripsPos[1][0][i][j].SetXYZ ( 0,0,1 );

            orrubaStripsPos[0][0][i][j].SetTheta ( refQQQ5D_sectA.Angle ( zAxis ) );
            orrubaStripsPos[1][0][i][j].SetTheta ( TMath::Pi() - refQQQ5D_sectA.Angle ( zAxis ) );

            orrubaStripsPos[0][0][i][j].SetPhi ( TMath::PiOver2() + i * TMath::PiOver2() );
            orrubaStripsPos[1][0][i][j].SetPhi ( TMath::PiOver2() + i * TMath::PiOver2() );

            orrubaStripsPos[0][0][i][j].SetMag ( refQQQ5D_sectA.Mag() );
            orrubaStripsPos[1][0][i][j].SetMag ( refQQQ5D_sectA.Mag() );

            orrubaStripsPos[0][0][i][j].SetRotationZ ( i * TMath::PiOver2() );
            orrubaStripsPos[1][0][i][j].SetRotationZ ( i * TMath::PiOver2() );

            float prevStripWidth = firstStripWidth - ( j-1 ) * 0.05;
            float currStripWidth = firstStripWidth - j * 0.05;

            TVector3 pStPosRefDetCenter = prevStripRefDetCenter + TVector3 ( 0, ( prevStripWidth + currStripWidth ) / 2., 0 );
            prevStripRefDetCenter = pStPosRefDetCenter;

            pStPosRefDetCenter.SetPhi ( pStPosRefDetCenter.Phi() + orrubaStripsPos[0][0][i][j].RotZ() );

            orrubaStripsPos[0][0][i][j] += pStPosRefDetCenter;
            orrubaStripsPos[1][0][i][j] += pStPosRefDetCenter;
        }
    }

    return;
}

TVector3 GoddessCalib::GetFinalHitPosition ( int isUpstream_, int isBarrel_, int sector_, int strip_, float eNear_, float eFar_ )
{
    TVector3 hitPos;

    if ( isBarrel_ == 0 )
    {
        hitPos = orrubaStripsPos[isUpstream_][0][sector_][strip_].GetTVector3();
    }
    else if ( isBarrel_ == 1 )
    {
        float SX3_length = 75.; // mm

        string detKey = Form ( "SuperX3 %s%d position %d", ( isUpstream_ ? "U" : "D" ), sector_, strip_ );

        float recenter = ( configCalPars[detKey].second + configCalPars[detKey].first ) / 2.;

        float normalize = configCalPars[detKey].second - configCalPars[detKey].first;

        normalize = normalize == 0 ? 1 : normalize;

        float zRes = ( ( ( eNear_ - eFar_ ) / ( eNear_ + eFar_ ) )  - recenter ) / normalize;

        TVector3 zResPos ( 0, 0, zRes * SX3_length );

        TVector3 interactionPos = orrubaStripsPos[isUpstream_][1][sector_][strip_].GetTVector3() + zResPos;

        return interactionPos;
    }

    return hitPos;
}

void GoddessCalib::SetNewGeomMode ( bool geomOR, bool reacOR, bool beamELoss, bool ejecELoss )
{
    nGP.overrideGeom = geomOR;
    nGP.overrideReac = reacOR;
    nGP.computebeamELoss = beamELoss;
    nGP.computeEjectileELoss = ejecELoss;
}

void GoddessCalib::SetGeomOverride ( bool geomOR )
{
    nGP.overrideGeom = geomOR;
}

void GoddessCalib::SetReacOverride ( bool reacOR )
{
    nGP.overrideReac = reacOR;
}

void GoddessCalib::SetComputeBeamELoss ( bool beamELoss )
{
    nGP.computebeamELoss = beamELoss;
}

void GoddessCalib::SetComputeEjecELoss ( bool ejecELoss )
{
    nGP.computeEjectileELoss = ejecELoss;
}

void GoddessCalib::GenerateGeomAdjustRootfile ( string filesname, string treename, long long int nEntries, string outfname )
{
    std::vector<string> rootfiles = DecodeItemsToTreat ( filesname, "system" );

    if ( rootfiles.size() == 0 ) return;

    TFile* firstRootFile = new TFile ( rootfiles[0].c_str(), "read" );

    geomInfo = ( GoddessGeomInfos* ) firstRootFile->FindObjectAny ( "GoddessGeom" );
    reacInfo = ( GoddessReacInfos* ) firstRootFile->FindObjectAny ( "GoddessReac" );

    if ( geomInfo == nullptr )
    {
        cerr << "The rootfile(s) specified have been generated with an outdated version of the sort code.\n";
        cerr << "Required information about the geometry are missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    if ( reacInfo == nullptr )
    {
        cerr << "The rootfile(s) specified have been generated with an outdated version of the sort code.\n";
        cerr << "Required information reaction are missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    firstRootFile->Close();


    TChain* chain = new TChain ( treename.c_str(), treename.c_str() );

    for ( unsigned int i = 0; i < rootfiles.size(); i++ )
    {
        chain->Add ( rootfiles[i].c_str() );
    }

//     TVector3 targetPos ( 0, 0, 0 );
//
//     TVector3 beamDir ( 0, 0, 1 );

    TTree* outtree = new TTree ( "godGeom", "godGeom" );

    bool isBarrel;
    bool isUpstream;
    float sector_strip;
    double energy;
//     double angle;
    TVector3 pos;

    outtree->Branch ( "isBarrel", &isBarrel );
    outtree->Branch ( "isUpstream", &isUpstream );
    outtree->Branch ( "sector_strip", &sector_strip );
    outtree->Branch ( "Energy", &energy );
//     outtree->Branch ( "angle", &angle );
    outtree->Branch ( "pos", &pos );

    vector<SiDataBase>* vectSiData = new vector<SiDataBase>;

    chain->SetBranchAddress ( "si", &vectSiData );

    if ( nEntries <= 0 ) nEntries = chain->GetEntries();

    for ( long long int ev = 0; ev < nEntries; ev++ )
    {
        if ( ev%10000 == 0 )
        {
            cout << "Entry " << std::setw ( 15 ) << ev << " / " << nEntries;
            cout<< " ( " << std::fixed << std::setprecision ( 2 )  << std::setw ( 6 ) << ( ( float ) ev/nEntries ) * 100. << " % )\r" << std::flush;
        }

        chain->GetEntry ( ev );

        if ( vectSiData->size() > 0 )
        {
            for ( unsigned int i = 0; i < vectSiData->size(); i++ )
            {
                SiDataBase siData = vectSiData->at ( i );

                isUpstream = ( int ) siData.isUpstream;
                isBarrel = ( int ) siData.isBarrel;

                sector_strip = siData.sector;
                sector_strip += siData.StripMaxLayer ( 1,false ) *0.01;
                energy = siData.ESumLayer ( 1,false );
                pos = siData.PosE1();
//                 angle = siData.Angle ( 1 );

//                 if ( energy > 0 && angle > 0 ) outtree->Fill();
                if ( energy > 0 && pos.Mag() > 0 ) outtree->Fill();
            }
        }
    }

    TFile* outf = new TFile ( outfname.c_str(), "recreate" );

    outf->cd();

    outtree->Write();

    outf->mkdir ( "infos" );
    outf->cd ( "infos" );
    geomInfo->Write ( "GoddessGeom" );
    reacInfo->Write ( "GoddessReac" );
    outf->Close();

    cout << endl;

    return;
}

void GoddessCalib::GetQValWithNewGeometry ( string filename, string treeName, long long int nEntries,
        int qqq5OffX, int qqq5OffY, int qqq5OffZ, int sX3OffX, int sX3OffY, int sX3OffZ, int targetOffX, int targetOffY, int targetOffZ )
{
    if ( initialPosChain == nullptr ) return;

    string computeELossStr = nGP.computeEjectileELoss ? "" : "_no_EnLoss";

    TFile* inFile = new TFile ( filename.c_str(), "read" );

    if ( !nGP.overrideGeom || !nGP.overrideReac ) // This is true if the user doesn't want to use the geometry/reaction parameters read from the input file as a base
    {
        if ( !inFile->IsOpen() ) return;

        if ( !nGP.overrideGeom ) geomInfo = ( GoddessGeomInfos* ) inFile->FindObjectAny ( "GoddessGeom" )->Clone();
        if ( !nGP.overrideReac ) reacInfo = ( GoddessReacInfos* ) inFile->FindObjectAny ( "GoddessReac" )->Clone();
    }

    inFile->Close();

    if ( geomInfo == nullptr )
    {
        cerr << "Geometry information missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    if ( reacInfo == nullptr )
    {
        cerr << "Reaction information missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    std::pair<vector<double>, vector<double>> energyLossData;

    if ( nGP.computeEjectileELoss )
    {
        if ( stoppingPowerTable["ejectile"].empty() )
        {
            std::ifstream mass_input ( pathToGDAQ + "/share/mass_db.dat", std::ios_base::in );

            if ( !mass_input.is_open() )
            {
                std::cerr << "Failed to open the mass database. Energy loss has not been computed...\n";

                return;
            }

            string projStr = "";

            GetAtomicFormula ( mass_input, reacInfo->ejecA, reacInfo->ejecZ, projStr );

            char* tryFindStr = new char[512];

            sprintf ( tryFindStr, "*%s*range*_vs_energy*", projStr.c_str() );

            vector<string> tryFindTable = DecodeItemsToTreat ( ( string ) tryFindStr, "system", false );

            if ( tryFindTable.size() != 1 )
            {
                std::cerr << "Requested to compute the energy loss but no stopping power table was given and auto search failed...\n";
                return;
            }

            stoppingPowerTable["ejectile"] = tryFindTable[0];
        }

        energyLossData = FillGraphFromFile ( stoppingPowerTable["ejectile"] );
    }

    TVector3 beamDir ( 0, 0, 1 );

    TVector3 targetLadderDir ( 0, 0, 1 );
    targetLadderDir.SetTheta ( geomInfo->targetLadderAngle*TMath::DegToRad() );
    targetLadderDir.SetPhi ( TMath::PiOver2() );

    TVector3 totQQQ5Offset ( ( qqq5OffX-targetOffX ) *0.01, ( qqq5OffY-targetOffY ) *0.01, ( qqq5OffZ-targetOffZ ) *0.01 );
    TVector3 totSX3Offset ( ( sX3OffX-targetOffX ) *0.01, ( sX3OffY-targetOffY ) *0.01, ( sX3OffZ-targetOffZ ) *0.01 );

    char qqq5SectAliases[4] = {'A', 'B', 'C', 'D'};

    TVector3 qqq5sPos[2][4];
    //     TVector3 sX3sPos[2][12];

    double qqq5FstStripWidth = geomInfo->qqq5FirstStripWidth;
    double qqq5DeltaPitch = geomInfo->qqq5DeltaPitch;

    double origBeamEk = reacInfo->beamEk;
    double localBeamEk = reacInfo->beamEk;

    if ( nGP.computebeamELoss )
    {
        double beamEffThickness = GetEffectiveThickness ( beamDir.Angle ( targetLadderDir ) - TMath::PiOver2(), reacInfo->targetThickness );

        localBeamEk = TryGetRemainingEnergy ( pathToGDAQ + "/share/mass_db.dat", reacInfo->beamA, reacInfo->beamZ, reacInfo->beamEk, beamEffThickness, 0.001, "Interpolation" );

        cout << "Beam Energy after computing energy loss: " << localBeamEk << "MeV in effective thickness: " << beamEffThickness << " mg/cm2\n";

        reacInfo->beamEk = localBeamEk;
    }

    TVector3 targetPos = geomInfo->GetOffset ( "Target Offset" );

    cout << "Target Offset read from file: ( " << targetPos.X() << " , " << targetPos.Y() << " , " << targetPos.Z() << " ) \n";

    string sX3OffXStr, sX3OffYStr, sX3OffZStr, qqq5OffXStr, qqq5OffYStr, qqq5OffZStr, targetOffXStr, targetOffYStr, targetOffZStr;

    sX3OffXStr = GetNameCompliantStr ( sX3OffX );
    sX3OffYStr = GetNameCompliantStr ( sX3OffY );
    sX3OffZStr = GetNameCompliantStr ( sX3OffZ );
    qqq5OffXStr = GetNameCompliantStr ( qqq5OffX );
    qqq5OffYStr = GetNameCompliantStr ( qqq5OffY );
    qqq5OffZStr = GetNameCompliantStr ( qqq5OffZ );
    targetOffXStr = GetNameCompliantStr ( targetOffX );
    targetOffYStr = GetNameCompliantStr ( targetOffY );
    targetOffZStr = GetNameCompliantStr ( targetOffZ );

    cout << "Will apply the following gain: SuperX3 = " << reacInfo->sX3EnGain << " / QQQ5 = " << reacInfo->qqq5EnGain << "\n";

    for ( int up = 0; up < 2; up++ )
    {
        for ( int bar = 0; bar < 2; bar++ )
        {
            int maxSector = ( bar ? 12 : 4 );

            for ( int sect = 0; sect < maxSector; sect++ )
            {
                if ( bar == 1 )
                {
                    string histKey = Form ( "EvsA_new_geom_SX3%s%d_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_SX3gain_%dpercents%s", ( up ? "U" : "D" ), sect,
                                            sX3OffXStr.c_str(), sX3OffYStr.c_str(), sX3OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                                            RoundValue ( localBeamEk ), RoundValue ( reacInfo->sX3EnGain*100 ), computeELossStr.c_str() );

                    if ( hEvsA_SX3_NewGeom.find ( histKey ) == hEvsA_SX3_NewGeom.end() )
                    {
                        hEvsA_SX3_NewGeom[histKey] = new TH2F ( histKey.c_str(),
                                                                Form ( "Energy vs. Angle new geom SuperX3 %s%d mod det X: %d, Y: %d, Z: %d / target X: %d, Y: %d, Z: %d / Beam Ek: %3.1f"
                                                                        "SX3 Gain Adjust: %4.2f %s",
                                                                        ( up ? "U" : "D" ), sect, sX3OffX, sX3OffY ,sX3OffZ, targetOffX, targetOffY, targetOffZ,
                                                                        localBeamEk, reacInfo->sX3EnGain, nGP.computeEjectileELoss ? "" : "ignore energy loss" ),
                                                                1800, 0, 180, 1500, 0, 15 );
                    }
                    hEvsA_SX3_NewGeom[histKey]->Reset();

                    histKey = Form ( "QVal_new_geom_SX3%s%d_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_SX3gain_%dpercents%s", ( up ? "U" : "D" ), sect,
                                     sX3OffXStr.c_str(), sX3OffYStr.c_str(), sX3OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                                     RoundValue ( localBeamEk ), RoundValue ( reacInfo->sX3EnGain*100 ), computeELossStr.c_str() );

                    if ( hQval_NewGeom.find ( histKey ) == hQval_NewGeom.end() )
                    {
                        hQval_NewGeom[histKey] = new TH1F ( histKey.c_str(),
                                                            Form ( "Q-Value new geom SuperX3 %s%d mod det X: %d, Y: %d, Z: %d / target X: %d, Y: %d, Z: %d / Beam Ek: %3.1f"
                                                                    "/ SX3 Gain Adjust: %4.2f %s",
                                                                    ( up ? "U" : "D" ), sect, sX3OffX, sX3OffY ,sX3OffZ, targetOffX, targetOffY, targetOffZ,
                                                                    localBeamEk, reacInfo->sX3EnGain, nGP.computeEjectileELoss ? "" : "ignore energy loss" ),
                                                            800, -20, 20 );
                    }
                    hQval_NewGeom[histKey]->Reset();

                    histKey = Form ( "Ex_new_geom_SX3%s%d_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_SX3gain_%dpercents%s", ( up ? "U" : "D" ), sect,
                                     sX3OffXStr.c_str(), sX3OffYStr.c_str(), sX3OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                                     RoundValue ( localBeamEk ), RoundValue ( reacInfo->sX3EnGain*100 ), computeELossStr.c_str() );

                    if ( hEx_NewGeom.find ( histKey ) == hEx_NewGeom.end() )
                    {
                        hEx_NewGeom[histKey] = new TH1F ( histKey.c_str(),
                                                          Form ( "Excitation Energy new geom SuperX3 %s%d mod det X: %d, Y: %d, Z: %d / target X: %d, Y: %d, Z: %d / Beam Ek: %3.1f"
                                                                  "/ SX3 Gain Adjust: %4.2f %s",
                                                                  ( up ? "U" : "D" ), sect, sX3OffX, sX3OffY ,sX3OffZ, targetOffX, targetOffY, targetOffZ,
                                                                  localBeamEk, reacInfo->sX3EnGain, nGP.computeEjectileELoss ? "" : "ignore energy loss" ),
                                                          800, -20, 20 );
                    }
                    hEx_NewGeom[histKey]->Reset();
                }
                else
                {
                    //                     cout << "------ Preparing histogram for " << ( bar ? "SuperX3" : "QQQ5" ) << " " << ( up ? "U" : "D" ) << sect << " ------\n";

                    qqq5sPos[up][sect] = GetDetPos ( geomInfo, up, bar, sect, 1 );



                    double* qqq5BinsEdges;

                    std::list<double> binsEdgesList;
                    binsEdgesList.clear();

                    TVector3 lowEdge = qqq5sPos[up][sect];
                    double midDetPhi = lowEdge.Phi();

                    //                     cout << "Position of the low edge of the first strip is: ( " << lowEdge.X() << " , " << lowEdge.Y() << " , " << lowEdge.Z() << " ) \n";
                    //                     cout << "Phi angle is: " << midDetPhi * TMath::RadToDeg() << "\n";

                    for ( int i = 0; i < 33; i++ )
                    {
                        if ( i>0 )
                        {
                            TVector3 toNextStrip ( 0, qqq5FstStripWidth - ( i-1 ) *qqq5DeltaPitch , 0 );
                            toNextStrip.SetPhi ( midDetPhi );

                            lowEdge += toNextStrip;
                        }

                        double thetaEdge = ( lowEdge + totQQQ5Offset ).Angle ( beamDir ) * TMath::RadToDeg();

                        binsEdgesList.push_back ( thetaEdge );

                        for ( int j = 0; j < 4; j++ )
                        {
                            lowEdge.SetPhi ( midDetPhi - 3./16. * TMath::Pi() + j/8. * TMath::Pi() );

                            thetaEdge = ( lowEdge + totQQQ5Offset ).Angle ( beamDir ) * TMath::RadToDeg();

                            binsEdgesList.push_back ( thetaEdge );
                        }

                        lowEdge.SetPhi ( midDetPhi );
                    }

//                     cout << "List of bin edges done... " << binsEdgesList.size() << " elements\n";

//                     cout << "Removing duplicates...\n";
                    binsEdgesList.unique ( CheckValProxFunc ( 0.05 ) );
//                     cout << "Duplicates removed... new size: " << binsEdgesList.size() << " elements\n";

//                     cout << "Sorting it...\n";
                    binsEdgesList.sort();
//                     cout << "Sorting done...\n";


                    qqq5BinsEdges = new double[binsEdgesList.size()];
                    unsigned int counter = 0;

                    //                     cout << "Filling the final bins edges array of size " << binsEdgesList.size() <<" ...\n";

                    for ( auto listItr = binsEdgesList.begin(); listItr != binsEdgesList.end(); listItr++ )
                    {
                        qqq5BinsEdges[counter] = *listItr;
                        //                         cout << "Bin #" << counter << " : " << qqq5BinsEdges[counter] << endl;
                        counter++;
                    }

                    string histKey = Form ( "EvsA_new_geom_QQQ5%s%c_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_QQQ5Gain_%dpercents%s", ( up ? "U" : "D" ), qqq5SectAliases[sect],
                                            qqq5OffXStr.c_str(), qqq5OffYStr.c_str(), qqq5OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                                            RoundValue ( localBeamEk ), RoundValue ( reacInfo->qqq5EnGain*100 ), computeELossStr.c_str() );

                    if ( hEvsA_QQQ5_NewGeom.find ( histKey ) == hEvsA_QQQ5_NewGeom.end() )
                    {
                        hEvsA_QQQ5_NewGeom[histKey] = new TH2F ( ( histKey ).c_str(),
                                Form ( "Energy vs. Angle new geom QQQ5 %s%c mod det X: %d, Y: %d, Z: %d / target X: %d, Y: %d, Z: %d / Beam Ek: %3.1f / QQQ5 Gain Adjust: %4.2f %s",
                                       ( up ? "U" : "D" ), qqq5SectAliases[sect], qqq5OffX, qqq5OffY ,qqq5OffZ, targetOffX, targetOffY, targetOffZ,
                                       localBeamEk, reacInfo->qqq5EnGain, nGP.computeEjectileELoss ? "" : "ignore energy loss" ),
                                binsEdgesList.size()-1, qqq5BinsEdges, 1500, 0, 15 );
                    }
                    hEvsA_QQQ5_NewGeom[histKey]->Reset();

                    histKey = Form ( "QVal_new_geom_QQQ5%s%c_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_QQQ5Gain_%dpercents%s", ( up ? "U" : "D" ), qqq5SectAliases[sect],
                                     qqq5OffXStr.c_str(), qqq5OffYStr.c_str(), qqq5OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                                     RoundValue ( localBeamEk ), RoundValue ( reacInfo->qqq5EnGain*100 ), computeELossStr.c_str() );

                    if ( hQval_NewGeom.find ( histKey ) == hQval_NewGeom.end() )
                    {
                        hQval_NewGeom[histKey] = new TH1F ( histKey.c_str(),
                                                            Form ( "Q-Value new geom QQQ5 %s%c mod det X: %d, Y: %d, Z: %d / target X: %d, Y: %d, Z: %d / Beam Ek: %3.1f / QQQ5 Gain Adjust: %4.2f %s",
                                                                    ( up ? "U" : "D" ), qqq5SectAliases[sect], qqq5OffX, qqq5OffY ,qqq5OffZ, targetOffX, targetOffY, targetOffZ,
                                                                    localBeamEk, reacInfo->qqq5EnGain, nGP.computeEjectileELoss ? "" : "ignore energy loss" ),
                                                            800, -20, 20 );
                    }
                    hQval_NewGeom[histKey]->Reset();

                    histKey = Form ( "Ex_new_geom_QQQ5%s%c_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_QQQ5Gain_%dpercents%s", ( up ? "U" : "D" ), qqq5SectAliases[sect],
                                     qqq5OffXStr.c_str(), qqq5OffYStr.c_str(), qqq5OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                                     RoundValue ( localBeamEk ), RoundValue ( reacInfo->qqq5EnGain*100 ), computeELossStr.c_str() );

                    if ( hEx_NewGeom.find ( histKey ) == hEx_NewGeom.end() )
                    {
                        hEx_NewGeom[histKey] = new TH1F ( histKey.c_str(),
                                                          Form ( "Excitation Energy new geom QQQ5 %s%c mod det X: %d, Y: %d, Z: %d / target X: %d, Y: %d, Z: %d / Beam Ek: %3.1f / QQQ5 Gain Adjust: %4.2f %s",
                                                                  ( up ? "U" : "D" ), qqq5SectAliases[sect], qqq5OffX, qqq5OffY ,qqq5OffZ, targetOffX, targetOffY, targetOffZ,
                                                                  localBeamEk, reacInfo->qqq5EnGain, nGP.computeEjectileELoss ? "" : "ignore energy loss" ),
                                                          800, -20, 20 );
                    }
                    hEx_NewGeom[histKey]->Reset();

                    histKey = Form ( "QVal_vs_Strip_new_geom_QQQ5%s%c_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_QQQ5Gain_%dpercents%s", ( up ? "U" : "D" ), qqq5SectAliases[sect],
                                     qqq5OffXStr.c_str(), qqq5OffYStr.c_str(), qqq5OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                                     RoundValue ( localBeamEk ), RoundValue ( reacInfo->qqq5EnGain*100 ), computeELossStr.c_str() );

                    if ( hQvalvsStrip_QQQ5_NewGeom.find ( histKey ) == hQvalvsStrip_QQQ5_NewGeom.end() )
                    {
                        hQvalvsStrip_QQQ5_NewGeom[histKey] = new TH2F ( histKey.c_str(),
                                Form ( "Q-Value vs. Strip# new geom QQQ5 %s%c mod det X: %d, Y: %d, Z: %d / target X: %d, Y: %d, Z: %d / Beam Ek: %3.1f / QQQ5 Gain Adjust: %4.2f %s",
                                       ( up ? "U" : "D" ), qqq5SectAliases[sect], qqq5OffX, qqq5OffY ,qqq5OffZ, targetOffX, targetOffY, targetOffZ,
                                       localBeamEk, reacInfo->qqq5EnGain, nGP.computeEjectileELoss ? "" : "ignore energy loss" ),
                                32, 0, 32, 800, -20, 20 );
                    }
                    hQvalvsStrip_QQQ5_NewGeom[histKey]->Reset();
                }



                string histKey;

                string sectorStr = bar ? Form ( "%d", sect ) : Form ( "%c", qqq5SectAliases[sect] );
            }
        }
    }

    inFile = new TFile ( filename.c_str(), "read" );

    if ( !inFile->IsOpen() ) return;

    TTree* inTree = ( TTree* ) inFile->Get ( treeName.c_str() );

    if ( inTree == nullptr ) return;

    if ( nEntries <= 0 || nEntries > inTree->GetEntries() ) nEntries = inTree->GetEntries();

    bool isBarrel;
    bool isUpstream;
    float sector_strip;
    double energy;
    //     double angle;
    TVector3* pos = new TVector3();

    inTree->SetBranchAddress ( "isBarrel", &isBarrel );
    inTree->SetBranchAddress ( "isUpstream", &isUpstream );
    inTree->SetBranchAddress ( "sector_strip", &sector_strip );
    inTree->SetBranchAddress ( "Energy", &energy );
    //     inTree->SetBranchAddress ( "angle", &angle );
    inTree->SetBranchAddress ( "pos", &pos );

    float minAngleQQQ5U0 = 0;

    double qvalGsGs = reacInfo->qValGsGs;

    for ( long long int ev = 0; ev < nEntries; ev++ )
    {
        if ( ev%10000 == 0 )
        {
            cout << "Entry " << std::setw ( 15 ) << ev << " / " << nEntries;
            cout<< " ( " << std::fixed << std::setprecision ( 2 )  << std::setw ( 6 ) << ( ( float ) ev/nEntries ) * 100. << " % ) \r" << std::flush;
        }

        inTree->GetEntry ( ev );

        TVector3 adjustPos = *pos + ( isBarrel ? totSX3Offset : totQQQ5Offset );
        float angle = adjustPos.Angle ( beamDir );

        unsigned int sector = floor ( sector_strip );
        unsigned int strip = RoundValue ( ( sector_strip-sector ) * 100 );

        if ( nGP.computeEjectileELoss )
        {
//             if ( !isBarrel && isUpstream )
//             {
//                 cout << "*******************************************************\n";
//                 cout << "QQQ5 U" << sector << " :\n";
//                 cout << "Position = ( " << adjustPos.X() << " , " << adjustPos.Y() << " , " << adjustPos.Z() << " )\n";
//                 cout << "Angle with Beam Direction = " << angle*TMath::RadToDeg() << "\n";
//                 cout << "Theta = " << adjustPos.Theta() *TMath::RadToDeg() << " / Phi = " <<  adjustPos.Phi() *TMath::RadToDeg() << "\n";
//                 cout << "Angle with target ladder = " << adjustPos.Angle ( targetLadderDir ) *TMath::RadToDeg() << "\n";
//             }

            double effThickness = GetEffectiveThickness ( adjustPos.Angle ( targetLadderDir ) - TMath::PiOver2(), reacInfo->targetThickness );
            double estELoss = ComputeEnergyLoss ( energyLossData.first, energyLossData.second, energy/reacInfo->ejecA, reacInfo->ejecA, 0, effThickness, 0.01, "Interpolation" );

//             cout << "Input energy : " << energy << " / Estimated energy loss : " << estELoss << " MeV in effective thickness: " << effThickness <<endl;

            energy += estELoss;
        }

        if ( isUpstream && !isBarrel && sector == 0 )
        {
            if ( minAngleQQQ5U0 == 0 || angle*TMath::RadToDeg() < minAngleQQQ5U0 ) minAngleQQQ5U0 = angle*TMath::RadToDeg();
        }

        string detKey;

        if ( isBarrel )
        {
            float qval = SiDataBase::QValue ( reacInfo, energy*reacInfo->sX3EnGain, angle );

            detKey = Form ( "_new_geom_SX3%s%d_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_SX3gain_%dpercents%s", ( isUpstream ? "U" : "D" ), sector,
                            sX3OffXStr.c_str(), sX3OffYStr.c_str(), sX3OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                            RoundValue ( localBeamEk ), RoundValue ( reacInfo->sX3EnGain*100 ), computeELossStr.c_str() );

            hQval_NewGeom[ ( "QVal" + detKey ) ]->Fill ( qval );
            hEx_NewGeom[ ( "Ex" + detKey ) ]->Fill ( qvalGsGs - qval );

            detKey = Form ( "EvsA_new_geom_SX3%s%d_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_SX3gain_%dpercents%s", ( isUpstream ? "U" : "D" ), sector,
                            sX3OffXStr.c_str(), sX3OffYStr.c_str(), sX3OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                            RoundValue ( localBeamEk ), RoundValue ( reacInfo->sX3EnGain*100 ), computeELossStr.c_str() );

            hEvsA_SX3_NewGeom[detKey]->Fill ( angle*TMath::RadToDeg(), energy*reacInfo->sX3EnGain );
        }
        else
        {
            float qval = SiDataBase::QValue ( reacInfo, energy*reacInfo->qqq5EnGain, angle );

            detKey = Form ( "_new_geom_QQQ5%s%c_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_QQQ5Gain_%dpercents%s", ( isUpstream ? "U" : "D" ), qqq5SectAliases[sector],
                            qqq5OffXStr.c_str(), qqq5OffYStr.c_str(), qqq5OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                            RoundValue ( localBeamEk ), RoundValue ( reacInfo->qqq5EnGain*100 ), computeELossStr.c_str() );

            hQval_NewGeom[ ( "QVal" + detKey ) ]->Fill ( qval );
            hEx_NewGeom[ ( "Ex" + detKey ) ]->Fill ( qvalGsGs - qval );

            detKey = Form ( "EvsA_new_geom_QQQ5%s%c_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_QQQ5Gain_%dpercents%s", ( isUpstream ? "U" : "D" ), qqq5SectAliases[sector],
                            qqq5OffXStr.c_str(), qqq5OffYStr.c_str(), qqq5OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                            RoundValue ( localBeamEk ), RoundValue ( reacInfo->qqq5EnGain*100 ), computeELossStr.c_str() );

            hEvsA_QQQ5_NewGeom[detKey]->Fill ( angle*TMath::RadToDeg(), energy*reacInfo->qqq5EnGain );

            detKey = Form ( "QVal_vs_Strip_new_geom_QQQ5%s%c_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_QQQ5Gain_%dpercents%s", ( isUpstream ? "U" : "D" ), qqq5SectAliases[sector],
                            qqq5OffXStr.c_str(), qqq5OffYStr.c_str(), qqq5OffZStr.c_str(), targetOffXStr.c_str(), targetOffYStr.c_str(), targetOffZStr.c_str(),
                            RoundValue ( localBeamEk ), RoundValue ( reacInfo->qqq5EnGain*100 ), computeELossStr.c_str() );

            hQvalvsStrip_QQQ5_NewGeom[detKey]->Fill ( strip, qval );
        }
    }

    reacInfo->beamEk = origBeamEk;

    cout << endl;

    inFile->Close();

    return;
}

void GoddessCalib::GetQValWithNewGeometry ( TChain* chain, string configFileName, long long int nEntries,
        float qqq5OffX, float qqq5OffY, float qqq5OffZ,
        float sX3OffX, float sX3OffY, float sX3OffZ,
        int minModX, int maxModX, int minModY, int maxModY, int minModZ, int maxModZ )
{
    if ( geomInfo == nullptr )
    {
        cerr << "Geometry information missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    if ( reacInfo == nullptr )
    {
        cerr << "Reaction information missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    lastQQQ5Offsets = {qqq5OffX, qqq5OffY, qqq5OffZ};
    lastSX3Offsets = {sX3OffX, sX3OffY, sX3OffZ};

    FillStripsPositionsArray ( qqq5OffX, qqq5OffY, qqq5OffZ, sX3OffX, sX3OffY, sX3OffZ );

    ReadConfigCalPars ( configFileName );

    TVector3 targetPos ( 0, 0, 0 );

    TVector3 beamDir ( 0, 0, 1 );

    char qqq5SectAliases[4] = {'A', 'B', 'C', 'D'};
    //     float qqq5FstStripOffset = 25.2; // mm
    //     float qqq5FstStripWidth = 2.55; // mm

    unsigned int histsNumber = ( maxModZ - minModZ + 1 ) * ( maxModY - minModY + 1 ) * ( maxModX - minModX + 1 ) * 32;
    unsigned int counter = 0;

    for ( int up = 0; up < 2; up++ )
    {
        for ( int bar = 0; bar < 2; bar++ )
        {
            int maxSector = ( bar ? 12 : 4 );

            for ( int sect = 0; sect < maxSector; sect++ )
            {
                for ( int modX = minModX; modX <= maxModX; modX++ )
                {
                    for ( int modY = minModY; modY <= maxModY; modY++ )
                    {
                        for ( int modZ = minModZ; modZ <= maxModZ; modZ++ )
                        {
                            if ( counter%100 == 0 ) cout << "Prepared " << std::setw ( 8 ) << counter << " histograms out of " << histsNumber << "\r" << std::flush;

                            string histKey;

                            string sectorStr = bar ? Form ( "%d", sect ) : Form ( "%c", qqq5SectAliases[sect] );

                            histKey = Form ( "QVal_new_geom_%s%s%s_%d_%d_%d", ( bar ? "SX3" : "QQQ5" ), ( up ? "U" : "D" ), sectorStr.c_str(), modX, modY, modZ );

                            hQval_NewGeom[histKey] = new TH1F ( histKey.c_str(),
                                                                Form ( "Q-Value new geom %s %s%s mod target X: %d, Y: %d, Z: %d",
                                                                        ( bar ? "SX3" : "QQQ5" ), ( up ? "U" : "D" ), sectorStr.c_str(), modX, modY, modZ ),
                                                                800, -20, 20 );

                            counter++;
                        }
                    }
                }
            }
        }
    }

    cout << endl;

    vector<SiDataDetailed>* vectSiData = new vector<SiDataDetailed>;

    chain->SetBranchAddress ( "si", &vectSiData );

    if ( nEntries <= 0 ) nEntries = chain->GetEntries();

    for ( long long int ev = 0; ev < nEntries; ev++ )
    {
        if ( ev%10000 == 0 )
        {
            cout << "Entry " << std::setw ( 15 ) << ev << " / " << nEntries;
            cout<< " ( " << std::fixed << std::setprecision ( 2 )  << std::setw ( 6 ) << ( ( float ) ev/nEntries ) * 100. << " % ) \r" << std::flush;
        }

        chain->GetEntry ( ev );

        if ( vectSiData->size() > 0 )
        {
            for ( unsigned int i = 0; i < vectSiData->size(); i++ )
            {
                SiDataDetailed siData = vectSiData->at ( i );

                if ( siData.MultLayer ( 1,false ) == 1 )
                {
                    int isUpstream = ( int ) siData.isUpstream;
                    int isBarrel = ( int ) siData.isBarrel;

                    int sector = siData.sector;
                    int strip = siData.StripMaxLayer ( 1,false );
                    float energy = siData.ESumLayer ( 1,false );

                    float enear = 0.0, efar = 0.0;

                    if ( energy > 0.0 )
                    {
                        if ( isBarrel == 1 )
                        {
                            if ( siData.E1.en.p.size() > 0 && siData.E1.en.n.size() > 0 && siData.E1.strip.n[0] == -1 )
                            {
                                enear = siData.E1.en.p[0];
                                efar = siData.E1.en.n[0];
                            }
                            else continue;
                        }

                        TVector3 hitPos = GetFinalHitPosition ( isUpstream, isBarrel, sector, strip, enear, efar );

                        for ( int modX = minModX; modX <= maxModX; modX++ )
                        {
                            for ( int modY = minModY; modY <= maxModY; modY++ )
                            {
                                for ( int modZ = minModZ; modZ <= maxModZ; modZ++ )
                                {
                                    targetPos = TVector3 ( modX, modY, modZ );

                                    float angle = ( hitPos-targetPos ).Angle ( beamDir );

                                    if ( angle != 0 )
                                    {
                                        float qval = SiDataBase::QValue ( reacInfo, energy, angle );

                                        string detKey;

                                        string sectorStr = isBarrel ? Form ( "%d", sector ) : Form ( "%c", qqq5SectAliases[sector] );

                                        detKey = Form ( "QVal_new_geom_%s%s%s_%d_%d_%d", ( isBarrel ? "SX3" : "QQQ5" ), ( isUpstream ? "U" : "D" ), sectorStr.c_str(), modX, modY, modZ );

                                        hQval_NewGeom[detKey]->Fill ( qval );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    cout << "\n\n";

    return;
}

string GoddessCalib::DrawNewGeom ( string histName )
{
    string detType;

    if ( histName.find ( "QQQ5" ) != string::npos ) detType = "QQQ5";
    else if ( histName.find ( "SuperX3" ) != string::npos ) detType = "SX3";

    string histType;

    if ( histName.find ( "QVal" ) != string::npos ) histType = "QVal";
    else if ( histName.find ( "EvsA" ) != string::npos ) histType = "EvsA";

    string modDetX, modDetY, modDetZ;
    string modTargetX, modTargetY, modTargetZ;

    std::size_t modPos = histName.find_first_of ( " ( " );
    std::size_t separatorPos = histName.find_first_of ( ",", modPos );

    modDetX = histName.substr ( modPos+1, separatorPos- ( modPos+1 ) );
    if ( modDetX[0] == '-' ) modDetX[0] = 'm';

    modPos = separatorPos;
    separatorPos = histName.find_first_of ( ",", modPos+1 );

    modDetY = histName.substr ( modPos+1, separatorPos- ( modPos+1 ) );
    if ( modDetY[0] == '-' ) modDetY[0] = 'm';

    modPos = separatorPos;
    separatorPos = histName.find_first_of ( " ) ", modPos+1 );

    modDetZ = histName.substr ( modPos+1, separatorPos - ( modPos+1 ) );
    if ( modDetZ[0] == '-' ) modDetZ[0] = 'm';

    std::size_t firstSpacePos = histName.find_first_of ( " " );
    firstSpacePos = histName.find_first_of ( " ", firstSpacePos+1 );
    std::size_t lastSpacePos = histName.find_first_of ( " ", firstSpacePos+1 );

    char isUp = histName[firstSpacePos+1];

    string sector = histName.substr ( firstSpacePos+2, lastSpacePos - ( firstSpacePos+2 ) );

    modPos = histName.find_first_of ( " ( ", modPos );
    separatorPos = histName.find_first_of ( ",", modPos );

    modTargetX = histName.substr ( modPos+1, separatorPos- ( modPos+1 ) );
    if ( modTargetX[0] == '-' ) modTargetX[0] = 'm';

    modPos = separatorPos;
    separatorPos = histName.find_first_of ( ",", modPos+1 );

    modTargetY = histName.substr ( modPos+1, separatorPos- ( modPos+1 ) );
    if ( modTargetY[0] == '-' ) modTargetY[0] = 'm';

    modPos = separatorPos;
    separatorPos = histName.find_first_of ( " ) ", modPos+1 );

    modTargetZ = histName.substr ( modPos+1, separatorPos - ( modPos+1 ) );
    if ( modTargetZ[0] == '-' ) modTargetZ[0] = 'm';

    string detKey = Form ( "%s_new_geom_%s%c%s_det_off_%s_%s_%s_target_off_%s_%s_%s", histType.c_str(), detType.c_str(), isUp, sector.c_str(), modDetX.c_str(), modDetY.c_str(), modDetZ.c_str(), modTargetX.c_str(), modTargetY.c_str(), modTargetZ.c_str() );

    if ( histType == "QVal" && hQval_NewGeom.find ( detKey ) != hQval_NewGeom.end() )
    {
        hQval_NewGeom[detKey]->Draw();
    }
    else if ( histType == "EvsA" && detType == "QQQ5" && hEvsA_QQQ5_NewGeom.find ( detKey ) != hEvsA_QQQ5_NewGeom.end() )
    {
        hEvsA_QQQ5_NewGeom[detKey]->Draw ( "colz" );
    }
    else if ( histType == "EvsA" && detType == "SX3" && hEvsA_SX3_NewGeom.find ( detKey ) != hEvsA_SX3_NewGeom.end() )
    {
        hEvsA_SX3_NewGeom[detKey]->Draw ( "colz" );
    }
    else
    {
        TH1F* hist = dynamic_cast<TH1F*> ( gDirectory->FindObjectAny ( detKey.c_str() ) );

        if ( hist != nullptr )
        {
            hQval_NewGeom[detKey] = hist;

            hQval_NewGeom[detKey]->Draw();

            return detKey;
        }
        else
        {
            cerr<< "Unable to find " << detKey << "\n";
            cerr << "Histogram specified does not exist!!!\n";
            cerr << "Valid histogram name examples:\n";
            cerr << "    \"QVal QQQ5 UB (1,2,1) Target (0,1,1)\"";
            cerr << "    \"EvsA SuperX3 U0 (0,3,2) Target (2,0,1)\"\n";
            cerr << "Where the first 3 numbers are the offsets applied to the detector position and the last 3 the ones applied to the target position\n";

            return "";
        }
    }

    return detKey;
}

void GoddessCalib::GetBestParameters ( float mean, float fwhm, string detType, string inFileName, string betterFitMode )
{
    vector<string> detFilters = SplitString ( detType, "," );

    auto mapItr = hQval_NewGeom.begin();

    TFile* inFile = 0;
    TIterator* fileItr = 0;
    TList* lOfKey = 0;

    string bestX, bestY, bestZ, bestHistName;
    float bestChi2 = 0, bestSigma = 0;

    float newChi2, newSigma;
//
    auto linkMap = MakeLinkMap ( "chi2 bestChi2 sigma bestSigma", newChi2, bestChi2, newSigma, bestSigma );

    TH1F* hist = 0;

    if ( inFileName.empty() ) hist = mapItr->second;
    else
    {
        inFile = new TFile ( inFileName.c_str(),"read" );

        lOfKey = inFile->GetListOfKeys();

        if ( lOfKey != nullptr )
        {
            fileItr = lOfKey->MakeIterator();
            TObject* obj = fileItr->Next();

            hist = dynamic_cast<TH1F*> ( inFile->FindObjectAny ( obj->GetName() ) );
        }
    }

    int nHists = inFileName.empty() ? hQval_NewGeom.size() : lOfKey->GetSize();
    int counter = 0;

    TF1* fitRes;

    while ( hist != nullptr )
    {
        if ( counter%100 == 0 )
        {
            cout << "Treated " << std::setw ( 8 ) << counter << " / " << nHists;
            cout << " ( " << std::fixed << std::setprecision ( 1 ) << std::setw ( 4 ) << ( float ) counter/nHists*100. << " % )\r" << std::flush;
        }

        counter++;

        bool skip = true;

        string histName = ( string ) hist->GetName();

        if ( detFilters.size() > 0 )
        {
            for ( unsigned int i = 0; i < detFilters.size(); i++ )
            {
                if ( histName.find ( detFilters[i].c_str() ) != string::npos )
                {
                    skip = false;
                    break;
                }
            }
        }

        if ( !skip )
        {
            fitRes = FitQValGS ( hist, {mean, mean-2}, fwhm, 0, 0, 0 );

            newChi2 = fitRes->GetChisquare();
            newSigma = fitRes->GetParameter ( 3 );

            bool betterFit;

            if ( bestChi2 == 0 ) betterFit = true;
            else betterFit = StringFormulaComparator<float> ( betterFitMode, &linkMap );

            if ( betterFit )
            {
                bestChi2 = newChi2;
                bestSigma = newSigma;

                bestHistName = histName;

                std::size_t sepPos = histName.find_first_of ( "_", 14 );
                std::size_t nextSepPos = histName.find_first_of ( "_", sepPos+1 );

                bestX = histName.substr ( sepPos+1, nextSepPos - ( sepPos+1 ) );

                sepPos = histName.find_first_of ( "_", nextSepPos+1 );
                nextSepPos = histName.find_first_of ( "_", sepPos+1 );

                bestY = histName.substr ( sepPos+1, nextSepPos - ( sepPos+1 ) );

                sepPos = histName.find_first_of ( "_", nextSepPos+1 );

                bestZ = histName.substr ( sepPos+1 );
            }
        }

        if ( inFileName.empty() )
        {
            mapItr++;
            if ( mapItr != hQval_NewGeom.end() ) hist = mapItr->second;
            else hist = nullptr;
        }
        else
        {
            TObject* obj = fileItr->Next();

            if ( obj != nullptr ) hist = dynamic_cast<TH1F*> ( inFile->FindObjectAny ( obj->GetName() ) );
            else hist = nullptr;
        }
    }

    cout << endl;

    cout << "Target position yielding to the best fit: ( " << bestX << ", " << bestY << ", " << bestZ << " ) for hist " << bestHistName << "\n\n";
}

TF1* GoddessCalib::FitQValNewGeom ( string sectorStr, int targetX, int targetY, int targetZ )
{
    string histName = Form ( "%s (%d,%d,%d)", sectorStr.c_str(), targetX, targetY, targetZ );

    string detKey = DrawNewGeom ( histName );

    TF1* fitRes = FitQValGS ( hQval_NewGeom[detKey], {4.1, 2.0}, 0.5, 0, 0, 0 );

    cout << endl;
    cout << "Chi2: " << fitRes->GetChisquare() << endl;
    cout << "Sigma: " << fitRes->GetParameter ( 3 ) << " MeV" << endl;
    cout << "-------- Peak 1 --------" << endl;
    cout << "Mean: " << fitRes->GetParameter ( 5 ) << " MeV" << endl;
    cout << "-------- Peak 2 --------" << endl;
    cout << "Mean: " << fitRes->GetParameter ( 7 ) << " MeV" << endl;
    cout << endl;

    return fitRes;
}

void GoddessCalib::ReloadNewGeomGraphs ( string inFileName )
{
    TFile* inFile = new TFile ( inFileName.c_str(), "read" );

    if ( !inFile->IsOpen() )
    {
        cerr << "Failed to open file: " << inFileName;
        return;
    }

    auto lOK = inFile->GetListOfKeys();

    if ( lOK != nullptr && lOK->GetSize() > 0 )
    {
        cout << "The file contains " << lOK->GetSize() << " histograms...\n";
        float doPrintoutAt = 0;

        TIterator* itr = lOK->MakeIterator();

        int counter = 0;
        TObject* fileKey;

        while ( ( fileKey = itr->Next() ) )
        {
            float progress = ( float ) counter/lOK->GetSize() * 100.;

            if ( progress >= doPrintoutAt )
            {
                cout << "Loaded " << std::setw ( 8 ) << counter << " / " << lOK->GetSize();
                cout << " ( " << std::setw ( 3 ) << ( int ) progress << " % )\r" << std::flush;

                doPrintoutAt = progress;
            }

            if ( fileKey != nullptr )
            {
                string histKey = ( string ) fileKey->GetName();

                hQval_NewGeom[histKey] = ( TH1F* ) inFile->FindObjectAny ( histKey.c_str() );
            }

            counter++;
        }
    }

    cout << endl;

    return;
}

void GoddessCalib::WriteNewGeomGraphs ( string outFName )
{
    if ( outFName.empty() )
    {
        outFName = Form ( "NewGeomHists_QQQ5_%0.2f_%0.2f_%0.2f_SX3_%0.2f_%0.2f_%0.2f.root",
                          lastQQQ5Offsets[0], lastQQQ5Offsets[1], lastQQQ5Offsets[2], lastSX3Offsets[0], lastSX3Offsets[1], lastSX3Offsets[2] );
    }

    TFile* outf = new TFile ( outFName.c_str(), "recreate" );

    if ( !outf->IsOpen() )
    {
        cerr << "Failed to open file: " << outFName;
        return;
    }

    outf->cd();

    auto itr = hQval_NewGeom.begin();

    while ( itr != hQval_NewGeom.end() )
    {
        itr->second->Write();
        itr++;
    }

    outf->Close();

    return;
}

// --------------------------------------------- Display Help Functions ----------------------------- //

TH2F* GoddessCalib::PlotSX3ResStripCalGraph ( TChain* chain, bool isUpstream, unsigned short sector, unsigned short strip )
{
    string upstreamCond = isUpstream ? "si.isUpstream" : "!si.isUpstream" ;
    string cond = "si.isBarrel && " + upstreamCond;

    return PlotSX3ResStripCalGraph ( chain, "si.E1.en.n:si.E1.en.p", sector, strip, cond );
}

void GoddessCalib::PlotSX3ResStripsCalGraphsFromTree()
{
    cout << "To generate the graphs for several sectors without drawing them (MUCH faster), call" << endl;
    cout << "PlotSX3ResStripsCalGraphsFromTree(TChain* chain, bool isUpstream, int nentries, int sector1, int sector2, int sector3, int ....)" << endl;
    cout << "where \"nenteries\" controls the number of entries to treat (0 == all the entries)" << endl;
}

void GoddessCalib::PlotSX3ResStripsCalGraphs()
{
    cout << "To plot several sectors in a row, call" << endl;
    cout << "PlotSX3ResStripsCalGraphs(TChain* chain, bool isUpstream, int sector1, int sector2, int sector3, int ....)" << endl;
    cout << endl;
    cout << "You can also change what to plot and specify the conditions by hand by calling" << endl;
    cout << "PlotSX3ResStripsCalGraphs(TChain* chain, string \"what to plot\", string conditions, sector1, sector2, sector3, ....)" << endl;
}

void GoddessCalib::Help()
{
    cout << "To plot the Enear vs Efar graph for strip X of sector Y, call" << endl;
    cout << "PlotSX3ResStripCalGraph(TChain* chain, bool isUpstream, int sector1, int strip)" << endl;
    cout << endl;
    PlotSX3ResStripsCalGraphs();
    cout << endl;
    PlotSX3ResStripsCalGraphsFromTree();
    cout << endl;
    cout << "To get help with the energy calibration procedure, call \"GoddessCalib::EnCalibHelp()\"" << endl;
    cout << endl;
    cout << "To get help with the position calibration procedure, call \"GoddessCalib::PosCalibHelp()\"" << endl;
    cout << endl;
    cout << "To reload a file containing previous calibration and update it, call" << endl;
    cout << "DumpFileToResCalMap(string \"previous calib file name\")" << endl;
    cout << endl;
}

void GoddessCalib::EnCalibHelp()
{
    cout << "To start the SX3 energy calibration porecedure, type \"GoddessCalib::StartSX3EnCalib()\" after opening a TBrowser or TCanvas." << endl;
    cout << endl;
    cout << "The energy calibration for the resistive strips needs several items:" << endl;
    cout << "2 TLines following the edges of the strip in the Enear vs. Efar graph" << endl;
    cout << "One TLine following the alpha line from a run with source in the Enear vs. Efar graph" << endl;
    cout << "NOTE: You can draw the TLine following the alpha peak separately but the 2 TLine for the edges has to be done from the same graph." << endl;
    cout << "To save the info about the TLine which have been drawn, click on the \"Save TLines Info\" button (top right of the canvas)" << endl;
    cout << endl;
    cout << "The info about the TLines drawn are stored in a map that can be saved at any time."<< endl;
    cout << "To write the results of the calibrations made during the last session, call" << endl;
    cout << "WriteResCalResults(string \"result file name\", string option = \"recreate\")" << endl;
    cout << endl;
    cout << "To update a config file with the results of the calibrations made during the last session, call" << endl;
    cout << "UpdateParamsInConf(string \"config file name\", string detType = \"superX3\", bool invertContactMidDet = true, string mode = \"protected\")" << endl;
    cout << "* invertContactMidDet should be set to \"true\" for SuperX3 because of the way the contacts numbers are incremented" << endl;
    cout << "* the \"protected\" mode will prevent you to overwrite your config file and generate a new config file from the input..." << endl;
    cout << "  switch it to \"overwrite\" if you really know what you're doing" << endl;
    cout << endl;

}

void GoddessCalib::PosCalibHelp()
{
    cout << "Not written yet... Come back later!" << endl;
    cout << endl;
}

ClassImp ( GoddessCalib )







































