#include "GoddessCalib.h"

GoddessCalib* GoddessCalib::s_instance = 0;
GoddessCalib* gC;

GoddessCalib::GoddessCalib() : GoddessAnalysis()
{
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

            if ( begRunName != string::npos && endRunName != string::npos ) rootFileName += "_" + treeFName.substr ( begRunName, endRunName ) + ".root";
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

                    if ( begRunName != string::npos && endRunName != string::npos ) rootFileName += "_" + treeFName.substr ( begRunName, endRunName ) + ".root";
                    else rootFileName += "_" + treeFName;
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
            auto lOK = gDirectory->GetListOfKeys();

            if ( lOK->GetSize() > 0 )
            {
                for ( int i = 0; i < lOK->GetSize(); i++ )
                {
                    TTree* testTree = ( TTree* ) lOK->At ( i );

                    if ( testTree != nullptr )
                    {
                        GoddessCalib::sinstance()->calTreeName = testTree->GetName();
                        GoddessCalib::sinstance()->AddFileToChain ( gDirectory->GetName() );
                        chain = GoddessCalib::sinstance()->calChain;
                    }
                }
            }
        }

        if ( chain == NULL ) return;

        long long int nentries = stoul ( entriesStr );

        if ( nentries == 0 || nentries > chain->GetEntries() ) nentries = chain->GetEntries();

        GoddessCalib::sinstance()->PlotSX3ResStripsCalGraphsFromTree ( chain, nentries, isUS, DecodeSectorsString ( sectorsStr ) );
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

        DecodeSectorsString ( sectorsStr );

        TChain* chain = GoddessCalib::sinstance()->calChain;

        if ( chain == NULL )
        {
            auto lOK = gDirectory->GetListOfKeys();

            if ( lOK->GetSize() > 0 )
            {
                for ( int i = 0; i < lOK->GetSize(); i++ )
                {
                    TTree* testTree = ( TTree* ) lOK->At ( i );

                    if ( testTree != nullptr )
                    {
                        GoddessCalib::sinstance()->calTreeName = testTree->GetName();
                        GoddessCalib::sinstance()->AddFileToChain ( gDirectory->GetName() );
                        chain = GoddessCalib::sinstance()->calChain;
                    }
                }
            }
        }

        if ( chain == NULL ) return;

        if ( nentries == 0 || nentries > chain->GetEntries() ) nentries = chain->GetEntries();

        GoddessCalib::sinstance()->DrawPosCalHistBatch ( chain, isUS, nentries, nBinsX, binMinX, binMaxX, nBinsY, binMinY, binMaxY, "", DecodeSectorsString ( sectorsStr ), configName );
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
        configFileIF->Resize ( 300, defDim.fHeight );

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
        outFileIF->Resize ( 300, defDim.fHeight );
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

        string outFName = GoddessCalib::sinstance()->GetAutoOutFileName ( "Resistive_Strips_EnCal_Graphs" );
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
        cerr << "The graph used do not fulfill the name requirement! Did you produce it with the GoddessCalib class built-in functions?\n";
        cerr << "To get help with the energy calibration procedure, type GoddessCalib::EnCalibHelp()\n";
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


    int projWidth_ = projWidthIF->GetNumber();
    double thr_ = projThrIF->GetNumber();
    bool doDraw = drawResCB->GetState();

    GoddessCalib::sinstance()->GetStripsEdges ( input, projWidth_, thr_, alphaEn1,  doDraw );

    return;
}

void GoddessCalib::OnClickGetStripsEdges()
{
    TGWindow* getStripsEdgesWin = FindWindowByName ( "Get Strips Edges" );

    if ( getStripsEdgesWin == NULL )
    {
        TGMainFrame* getStripsEdgesMF = new TGMainFrame ( gClient->GetRoot(),100, 100 );
        getStripsEdgesMF->SetName ( "Get Strips Edges" );
        getStripsEdgesMF->SetWindowName ( "Get Strips Edges" );

        getStripsEdgesMF->SetLayoutManager ( new TGMatrixLayout ( getStripsEdgesMF, 4, 2, 5, 5 ) );

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

        TGLabel* drawResultsLabel = new TGLabel ( getStripsEdgesMF, "Draw Results?" );
        TGCheckButton* drawResultsCB = new TGCheckButton ( getStripsEdgesMF );
        drawResultsCB->SetName ( "Draw Results CB" );
        drawResultsCB->SetState ( kButtonDown );

        getStripsEdgesMF->AddFrame ( drawResultsLabel );
        getStripsEdgesMF->AddFrame ( drawResultsCB );

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

    gStyle->SetMarkerColor ( 4 );
    gStyle->SetMarkerSize ( 2 );
    gStyle->SetMarkerStyle ( 3 );

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

void GoddessCalib::AddFileToChain ( TFile* file )
{
    string fileName = file->GetName();

    return AddFileToChain ( fileName );
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

    grFile->cd();

    for ( auto itr = enShiftVsPosGraphs.begin(); itr != enShiftVsPosGraphs.end(); itr++ )
    {
        if ( grFile->FindObjectAny ( itr->first.c_str() ) != nullptr )
        {
            grFile->Delete ( itr->first.c_str() );
        }

        itr->second->Write();
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

void GoddessCalib::PlotSX3ResStripsCalGraphsFromTree ( TChain* chain, long int nentries, bool isUpstream_, vector< unsigned short > sectorsList )
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

        if ( i%10000 == 0 ) cout << "Treated " << i << " / " << nentries << " entries ( " << ( ( float ) i ) / ( ( float ) nentries ) * 100. << "% )\r" << std::flush;

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
        int nbinsX, float binMinX, float binMaxX, int nbinsY, float binMinY, float binMaxY, string drawOpts, vector< unsigned short > sectorsList, string configFileName )
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

        if ( i%10000 == 0 ) cout << "Treated " << i << " / " << nentries << " entries ( " << ( ( float ) i ) / ( ( float ) nentries ) * 100. << " % )\r" << std::flush;

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
    TH1D* proj = input->ProjectionX ( ( ( string ) "projX_" + input->GetName() ).c_str(), projCenter - projWidth/2, projCenter + projWidth/2 );

    return proj;
}

TH1D* GoddessCalib::GetPosCalProjY ( TH2F* input, int projCenter, int projWidth )
{
    TH1D* proj = input->ProjectionY ( ( ( string ) "projY_" + input->GetName() ).c_str(), projCenter - projWidth/2, projCenter + projWidth/2 );

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

TF1* GoddessCalib::FitEdges ( TH2F* input, int projWidth, double threshold, bool fitRight )
{
    int binMaxY = GetPosCalEnBinMax ( input, threshold );

    TH1D* projX = GetPosCalProjX ( input, binMaxY, projWidth );
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

    cout << "Found the " << ( fitRight ? "left" : "right" ) << " shoulder at bin #" << binShoulder << " (value = " << projX->GetBinCenter ( binShoulder ) << ")" << endl;

    TF1 *fitfunc = new TF1 ( Form ( "myfit_left_%s",input->GetName() ), ( fitRight ? flatTopGaussRight : flatTopGaussLeft ), projX->GetBinCenter ( binShoulder - 2*windowWidth ), projX->GetBinCenter ( binShoulder + windowWidth ), 4 );

    if ( binShoulder != -1 )
    {
        fitfunc->SetParameter ( 0, projX->GetBinContent ( binShoulder ) );
        fitfunc->SetParameter ( 1, projX->GetBinCenter ( binShoulder ) );
        fitfunc->SetParameter ( 2, 2*projX->GetBinWidth ( binShoulder ) );

        fitfunc->SetParameter ( 3, 0 );

        projX->Fit ( fitfunc, "QRMN" );

        float edge = fitfunc->GetParameter ( 1 ) - TMath::Sqrt ( -2*pow ( fitfunc->GetParameter ( 2 ),2 ) * TMath::Log ( 0.7 ) );

        cout << "Found the " << ( fitRight ? "left" : "right" ) << " strip edge at " << edge << endl;

        string hname = input->GetName();
        string calMapKey = "SuperX3 " + hname.substr ( 0, hname.find ( "_" ) );

        int stripNbr = std::stoi ( hname.substr ( hname.find ( "_" ) + 1 ) );

        InitializeCalMapKey ( calMapKey, stripNbr );

        if ( fitRight ) resStripsCalMap[calMapKey][stripNbr][5] = edge;
        else resStripsCalMap[calMapKey][stripNbr][4] = edge;
    }
    return fitfunc;
}

TGraph* GoddessCalib::GetEnergyShiftVsPosition ( TH2F* input, int nPoints, float startPoint, float endPoint, double threshold, double peakPos )
{
    string graphName = "SuperX3_" + ( string ) input->GetName();

    enShiftVsPosGraphs[graphName] = new TGraph ( nPoints );

    float projWidth = ( endPoint-startPoint ) / nPoints;

    TH1D* projY = 0;

    for ( int i = 0; i < nPoints; i++ )
    {
        int projBin = input->GetXaxis()->FindBin ( startPoint + projWidth * ( i + 1/2. ) );

        projY = GetPosCalProjY ( input, projBin, projY->FindBin ( startPoint + ( i+1 ) * projWidth ) - projY->FindBin ( startPoint + i * projWidth ) );

        projY->GetXaxis()->SetRangeUser ( threshold, projY->GetBinCenter ( projY->GetXaxis()->GetLast() ) );

        int binMax = projY->GetMaximumBin();

        enShiftVsPosGraphs[graphName]->SetPoint ( i, startPoint + projWidth * ( i + 1/2. ), peakPos / projY->GetBinCenter ( binMax ) );
    }

    return enShiftVsPosGraphs[graphName];
}

void GoddessCalib::GetStripsEdges ( TH2F* input, int projWidth, double threshold, double peakPos, bool drawResults )
{
    string hname = input->GetName();

    cout << "Retreiving the edges of sector " << hname.substr ( 0, hname.find ( "_" ) ) << " strip #" << hname.substr ( hname.find ( "_" ) +1 ) << " ..." << endl;

    TF1* lfit = FitEdges ( input, projWidth, threshold, false );
    TF1* rfit = FitEdges ( input, projWidth, threshold, true );

    TGraph* enShiftGraph = GetEnergyShiftVsPosition ( input, 25, lfit->GetParameter ( 1 ), rfit->GetParameter ( 1 ), threshold, peakPos );

    if ( drawResults )
    {
        TCanvas* newCanvas = new TCanvas ( Form ( "c_%s", hname.c_str() ) );

        newCanvas->Divide ( 2, 1 );

        newCanvas->GetPad ( 1 )->cd();

        GetPosCalProjX ( input, GetPosCalEnBinMax ( input, threshold ), projWidth )->Draw();
        lfit->Draw ( "same" );
        rfit->Draw ( "same" );

        newCanvas->GetPad ( 2 )->cd();

        enShiftGraph->Draw ( "AP" );
    }

    return;
}

void GoddessCalib::GetStripsEdges ( int projWidth, double threshold, double peakPos, bool drawResults )
{
    for ( auto itr = resStripsPosCalGraphsMap.begin(); itr != resStripsPosCalGraphsMap.end(); itr++ )
    {
        GetStripsEdges ( itr->second, projWidth, threshold, peakPos, drawResults );
    }

    return;
}

void GoddessCalib::GetStripsEdges ( TFile* input, int projWidth, double threshold, double peakPos, bool drawResults )
{
    auto lOK = input->GetListOfKeys();

    for ( int i = 0; i < lOK->GetSize(); i++ )
    {
        TH2F* hist = dynamic_cast<TH2F*> ( input->Get ( lOK->At ( i )->GetName() ) );

        if ( hist != nullptr )
        {
            string hname = hist->GetName();

            if ( ! ( hname[0] == 'U' || hname[0] == 'D' ) && ! ( hname.find ( "_" ) == 2 || hname.find ( "_" ) == 3 ) ) continue;

            GetStripsEdges ( hist, projWidth, threshold, peakPos, drawResults );
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

TF1* GoddessCalib::FitQValGS ( TH1F* hist, vector<float> mean, float fwhm, float peakRatio, float minBound, float maxBound )
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

    fitFunc->FixParameter ( 2, peakRatio );

    fitFunc->SetParameter ( 0, 1 );
    fitFunc->SetParameter ( 1, 0.1 );

    fitFunc->SetParameter ( 3, fwhm );

    for ( unsigned int i = 0; i < mean.size(); i++ )
    {
        int parNum = 4 + 2*i;

        fitFunc->SetParameter ( parNum, 10 );
        fitFunc->SetParameter ( parNum+1, mean[i] );
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

                        newQval = ( 1+ejecMass/recoilMass ) * ( newEn ) - ( 1 - beamMass/recoilMass ) * ( beamEk )
                                  - 2 * TMath::Sqrt ( beamMass*ejecMass* ( newEn ) * ( beamEk ) ) / recoilMass * TMath::Cos ( angle * TMath::Pi() / 180. );

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

        float recenter = configCalPars[detKey].second - abs ( configCalPars[detKey].second - configCalPars[detKey].first ) / 2.;

        float normalize = configCalPars[detKey].second - configCalPars[detKey].first;

        normalize = normalize == 0 ? 1 : normalize;

        float zRes = ( ( ( eNear_ - eFar_ ) / ( eNear_ + eFar_ ) )  - recenter ) / normalize;

        TVector3 zResPos ( 0, 0, zRes * SX3_length );

        TVector3 interactionPos = orrubaStripsPos[isUpstream_][1][sector_][strip_].GetTVector3() + zResPos;

        return interactionPos;
    }

    return hitPos;
}

void GoddessCalib::GetQValWithNewGeometry ( TChain* chain, string configFileName, long long int nEntries,
        float qqq5OffX, float qqq5OffY, float QQQ5OffZ,
        float sX3OffX, float sX3OffY, float sX3OffZ,
        int minModX, int maxModX, int minModY, int maxModY, int minModZ, int maxModZ )
{
    lastQQQ5Offsets = {qqq5OffX, qqq5OffY, QQQ5OffZ};
    lastSX3Offsets = {sX3OffX, sX3OffY, sX3OffZ};

    FillStripsPositionsArray ( qqq5OffX, qqq5OffY, QQQ5OffZ, sX3OffX, sX3OffY, sX3OffZ );

    ReadConfigCalPars ( configFileName );

    TVector3 targetPos ( 0, 0, 0 );

    TVector3 beamDir ( 0, 0, 1 );

    char qqq5SectAliases[4] = {'A', 'B', 'C', 'D'};

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
                            string histKey;

                            string sectorStr = bar ? Form ( "%d", sect ) : Form ( "%c", qqq5SectAliases[sect] );

                            histKey = Form ( "QVal_new_geom_%s%s%s_%d_%d_%d", ( bar ? "SX3" : "QQQ5" ), ( up ? "U" : "D" ), sectorStr.c_str(), modX, modY, modZ );

                            hQval_NewGeom[histKey] = new TH1F ( histKey.c_str(),
                                                                Form ( "Q-Value new geom %s %s%s mod target X: %d, Y: %d, Z: %d",
                                                                        ( bar ? "SX3" : "QQQ5" ), ( up ? "U" : "D" ), sectorStr.c_str(), modX, modY, modZ ),
                                                                800, -20, 20 );
                        }
                    }
                }
            }
        }
    }

    vector<SiDataDetailed>* vectSiData = new vector<SiDataDetailed>;

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
                                        float qval = ( 1+ejecMass/recoilMass ) * ( energy ) - ( 1 - beamMass/recoilMass ) * ( beamEk )
                                                     - 2 * TMath::Sqrt ( beamMass*ejecMass* ( energy ) * ( beamEk ) ) / recoilMass * TMath::Cos ( angle );

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

string GoddessCalib::DrawQValNewGeom ( string histName )
{
    string detType;

    if ( histName.find ( "QQQ5" ) != string::npos ) detType = "QQQ5";
    else if ( histName.find ( "SuperX3" ) != string::npos ) detType = "SX3";

    std::size_t modPos = histName.find_first_of ( "(" );
    std::size_t separatorPos = histName.find_first_of ( ",", modPos );

    string modX, modY, modZ;

    modX = histName.substr ( modPos+1, separatorPos- ( modPos+1 ) );

    modPos = separatorPos;
    separatorPos = histName.find_first_of ( ",", modPos+1 );

    modY = histName.substr ( modPos+1, separatorPos- ( modPos+1 ) );

    modPos = separatorPos;
    separatorPos = histName.find_first_of ( ")", modPos+1 );

    modZ = histName.substr ( modPos+1, separatorPos - ( modPos+1 ) );

    std::size_t firstSpacePos = histName.find_first_of ( " " );
    std::size_t lastSpacePos = histName.find_first_of ( " ", firstSpacePos+1 );

    char isUp = histName[firstSpacePos+1];

    string sector = histName.substr ( firstSpacePos+2, lastSpacePos - ( firstSpacePos+2 ) );

    string detKey = Form ( "QVal_new_geom_%s%c%s_%s_%s_%s", detType.c_str(), isUp, sector.c_str(), modX.c_str(), modY.c_str(), modZ.c_str() );

    if ( hQval_NewGeom.find ( detKey ) != hQval_NewGeom.end() )
    {
        hQval_NewGeom[detKey]->Draw();
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
            cerr << "    \"QQQ5 UB (1,2,1)\"";
            cerr << "    \"SuperX3 U0 (0,3,2)\"\n";
            cerr << "Where the last 3 numbers are the offsets applied to the target position\n";

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
            cout << "Treated " << std::setw ( 8 ) << counter << " / " << nHists << " ( " << std::fixed << std::setprecision ( 1 ) << std::setw ( 4 ) << ( float ) counter/nHists*100. << " % )\r" << std::flush;
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

    string detKey = DrawQValNewGeom ( histName );

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
































