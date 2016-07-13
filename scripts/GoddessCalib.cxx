#include "GoddessCalib.h"

GoddessCalib* GoddessCalib::s_instance = 0;

GoddessCalib::GoddessCalib() : GoddessAnalysis()
{
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

    vertFrame->AddFrame ( sSX3CB, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 100, 100, 20, 20 ) );

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

    // ------ Adding the button to read existing calib parameters from file ------ //

    TGTextButton* dCPB = new TGTextButton ( vertFrame, "Reload Prev. Cal. Params", "GoddessCalib::sinstance()->DumpFileToResCalMap()" );
    dCPB->SetName ( "Reload Prev. Cal. Params" );

    vertFrame->AddFrame ( dCPB, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 100, 100, 10, 20 ) );

    // ------ Adding the second horizontal subframe for writing and updating the calib parameters ------ //

    TGHorizontalFrame* subHorzFrame2 = new TGHorizontalFrame ( vertFrame, 200, 400 );
    subHorzFrame2->SetName ( "SubHorizontalFrame2" );
    subHorzFrame2->SetBackgroundColor ( 0x4d004d );

    TGTextButton* wCPB = new TGTextButton ( subHorzFrame2, "Write New Cal. Params", "GoddessCalib::sinstance()->WriteResCalResults()" );
    wCPB->SetName ( "Write New Cal. Params" );

    TGTextButton* uCFB = new TGTextButton ( subHorzFrame2, "Update Goddess Config File", "GoddessCalib::OnClickUpdateConf()" );
    uCFB->SetName ( "Update Goddess Config File" );

    subHorzFrame2->AddFrame ( wCPB, new TGLayoutHints ( kLHintsCenterX, 20, 25, 0, 0 ) );
    subHorzFrame2->AddFrame ( uCFB, new TGLayoutHints ( kLHintsCenterX, 25, 20, 0, 0 ) );

    vertFrame->AddFrame ( subHorzFrame2, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 0, 0, 20, 10 ) );

    // ------ Adding text input field for the config file name ------ //

    PopupInputfield ( vertFrame, 12, "Config Full Name", new TGLayoutHints ( kLHintsRight, 0, 20, 0, 20 ), false );

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

    subHorzFrame3->AddFrame ( eAlphaLabel, new TGLayoutHints ( kLHintsCenterX , 10, 10, 5, 5 ) );
    subHorzFrame3->AddFrame ( alphaEn1IF, new TGLayoutHints ( kLHintsCenterX , 10, 30, 5, 5 ) );

    subHorzFrame3->AddFrame ( gCornerB, new TGLayoutHints ( kLHintsCenterX , 10, 10, 5, 5 ) );

    vertFrame->AddFrame ( subHorzFrame3, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 0, 0, 40, 20 ) );

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

    subHorzFrame4->AddFrame ( fNameLabel, new TGLayoutHints ( kLHintsCenterX , 10, 10, 5, 5 ) );
    subHorzFrame4->AddFrame ( fNameIF, new TGLayoutHints ( kLHintsCenterX , 10, 35, 5, 5 ) );

    subHorzFrame4->AddFrame ( gEdgeB, new TGLayoutHints ( kLHintsCenterX , 10, 10, 5, 5 ) );

    vertFrame->AddFrame ( subHorzFrame4, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 0, 0, 0, 70 ) );

    // ------ Wraping everything in the main frame ------ //

    controlFrame->AddFrame ( vertFrame );

    controlFrame->MapSubwindows();
    controlFrame->Resize ( controlFrame->GetDefaultSize() );
    controlFrame->MapWindow();

//     controlFrame->GetL

    currDetType = "";
    currRefEn1 = 0.0;
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

    return s_instance;
}

TGWindow* GoddessCalib::FindWindowByName ( std::string winName )
{
    std::string altName = winName;

    while ( altName.find ( " " ) != std::string::npos )
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

TGFrame* GoddessCalib::FindFrameByName ( TGCompositeFrame* pFrame, std::string frameName )
{
//     std::cout << "Scanning Frame " << pFrame->GetName() << "\n";

    std::string altName = frameName;

    while ( altName.find ( " " ) != std::string::npos )
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

void GoddessCalib::PopupInputfield ( TGWindow* pWin, short textSize, std::string label, TGLayoutHints* layHints, bool isEditable )
{
    TGGroupFrame* newGroup = new TGGroupFrame ( pWin, label.c_str(), kVerticalFrame );
    newGroup->SetTitlePos ( TGGroupFrame::kLeft );
    newGroup->SetBackgroundColor ( 0x4d004d );
    newGroup->SetTextColor ( 0xffffff );

    TGTextEntry* newTextEntry = new TGTextEntry ( newGroup );
    newTextEntry->SetName ( label.c_str() );

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

std::vector<unsigned short> DecodeSectorsString ( std::string sectorsString, bool verbose = false )
{
    std::vector<unsigned short> sectorsNum;

    sectorsNum.clear();

    while ( sectorsString.length() > 0 )
    {
        size_t foundComa = sectorsString.find ( "," );
        size_t foundDash = sectorsString.find ( "-" );

        std::string sub1 = "";
        std::string sub2 = "";

        if ( foundComa != std::string::npos && foundComa < foundDash )
        {
            sub1 = sectorsString.substr ( 0, foundComa );
        }
        else if ( foundDash != std::string::npos )
        {
            sub1 = sectorsString.substr ( 0, foundDash );
            sub2 = sectorsString.substr ( foundDash+1, foundComa );
        }
        else
            sub1 = sectorsString;

        if ( sub1.length() > 0 )
        {
            while ( sub1.find ( " " ) != std::string::npos )
            {
                sub1.replace ( sub1.find ( " " ), 1, "" );
            }

            sectorsNum.push_back ( stoi ( sub1 ) );
        }
        if ( sub2.length() > 0 )
        {
            while ( sub1.find ( " " ) != std::string::npos )
            {
                sub1.replace ( sub1.find ( " " ), 1, "" );
            }

            int lowerBound = stoi ( sub1 );
            int upperBound = stoi ( sub2 );

            for ( int i = lowerBound+1; i < upperBound; i++ )
            {
                sectorsNum.push_back ( i );
            }
        }

        if ( foundComa != std::string::npos || foundDash != std::string::npos )
        {
//                 std::cout << "sectorsString was " << sectorsString;
            sectorsString.replace ( 0, sub2.length() > 0 ? foundDash+1 : foundComa+1, "" );
//                 std::cout << " ... is now " << sectorsString << "\n";
        }
        else
        {
            sectorsString.clear();
        }
    }

    if ( verbose )
    {
        std::cout << "Decoded the sectors string:\n";

        for ( unsigned int i = 0; i <sectorsNum.size(); i++ )
        {
            std::cout << "-> " << sectorsNum[i] << "\n";
        }
    }

    return sectorsNum;
}

void GoddessCalib::ValidatePlotEnCalGraphs()
{
    TGWindow* plotEnCalWin = FindWindowByName ( "Plot Energy Calibration Graphs" );

    if ( plotEnCalWin != NULL )
    {
        TGMainFrame* mf = ( TGMainFrame* ) plotEnCalWin->GetMainFrame();

        TGTextEntry* tE;
        TGCheckButton* cB;

        std::string treeName, entriesStr, sectorsStr;
        bool isUS;

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Tree Name IF" ) );
        treeName = tE->GetText();

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Entries IF" ) );
        entriesStr = tE->GetText();

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Sectors IF" ) );
        sectorsStr = tE->GetText();

        cB = dynamic_cast<TGCheckButton*> ( FindFrameByName ( mf, "Is Upstream CB" ) );
        isUS = cB->GetState();

        TTree* tree = ( TTree* ) gDirectory->Get ( treeName.c_str() );

        if ( tree == NULL ) return;

        long long int nentries = stoul ( entriesStr );

        if ( nentries == 0 || nentries > tree->GetEntries() ) nentries = tree->GetEntries();

        GoddessCalib::sinstance()->PlotSX3ResStripsCalGraphsFromTree ( tree, nentries, isUS, DecodeSectorsString ( sectorsStr ) );
    }
}

void GoddessCalib::OnClickPlotEnCalGraphs()
{
    TGWindow* plotEnCalWin = FindWindowByName ( "Plot Energy Calibration Graphs" );

    if ( plotEnCalWin == NULL )
    {
        TGMainFrame* plotEnCalMF = new TGMainFrame ( gClient->GetRoot(),100, 200 );
        plotEnCalMF->SetName ( "Plot Energy Calibration Graphs" );
        plotEnCalMF->SetWindowName ( "Plot Energy Calibration Graphs" );

        TGMatrixLayout* mLay = new TGMatrixLayout ( plotEnCalMF, 5, 2, 10, 10 );
        plotEnCalMF->SetLayoutManager ( mLay );

        TGLabel* treeNameLabel = new TGLabel ( plotEnCalMF, "Tree Name:" );
        TGTextEntry* treeNameIF = new TGTextEntry ( plotEnCalMF );
        treeNameIF->SetName ( "Tree Name IF" );
        treeNameIF->SetAlignment ( kTextRight );

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

        plotEnCalMF->AddFrame ( sectorsLabel );
        plotEnCalMF->AddFrame ( sectorsIF );

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

        std::string treeName, sectorsStr;
        unsigned long long int nentries;
        int nBinsX, binMinX, binMaxX, nBinsY, binMinY, binMaxY;
        bool isUS;

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Tree Name IF" ) );
        treeName = tE->GetText();

        if ( treeName.empty() ) return;

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Entries IF" ) );
        nentries = std::stoul ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "NBinsX IF" ) );
        nBinsX = std::stoi ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "BinMinX IF" ) );
        binMinX = std::stoi ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "BinMaxX IF" ) );
        binMaxX = std::stoi ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "NBinsY IF" ) );
        nBinsY = std::stoi ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "BinMinY IF" ) );
        binMinY = std::stoi ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "BinMaxY IF" ) );
        binMaxY = std::stoi ( tE->GetText() );

        tE = dynamic_cast<TGTextEntry*> ( FindFrameByName ( mf, "Sectors IF" ) );
        sectorsStr = tE->GetText();

        cB = dynamic_cast<TGCheckButton*> ( FindFrameByName ( mf, "Is Upstream CB" ) );
        isUS = cB->GetState();

        DecodeSectorsString ( sectorsStr );

        TTree* tree = ( TTree* ) gDirectory->Get ( treeName.c_str() );

        if ( tree == NULL ) return;

        if ( nentries == 0 || nentries > tree->GetEntries() ) nentries = tree->GetEntries();

        GoddessCalib::sinstance()->DrawPosCalHistBatch ( tree, isUS, nentries, nBinsX, binMinX, binMaxX, nBinsY, binMinY, binMaxY, "", DecodeSectorsString ( sectorsStr ) );
    }
}

void GoddessCalib::OnClickPlotPosCalGraphs()
{
    TGWindow* plotPosCalWin = FindWindowByName ( "Plot Position Calibration Graphs" );

    if ( plotPosCalWin == NULL )
    {
        TGMainFrame* plotPosCalMF = new TGMainFrame ( gClient->GetRoot(),100, 200 );
        plotPosCalMF->SetName ( "Plot Position Calibration Graphs" );
        plotPosCalMF->SetWindowName ( "Plot Position Calibration Graphs" );

//         TGMatrixLayout* mLay = new TGMatrixLayout ( plotPosCalMF, 7, 2, 10, 10 );
//         plotPosCalMF->SetLayoutManager ( new TGColumnLayout ( plotPosCalMF ) );

        TGCompositeFrame* firstFrame = new TGCompositeFrame ( plotPosCalMF, 100, 100 );
        firstFrame->SetLayoutManager ( new TGMatrixLayout ( firstFrame, 3, 2, 5, 5 ) );

        TGLabel* treeNameLabel = new TGLabel ( firstFrame, "Tree Name:" );
        TGTextEntry* treeNameIF = new TGTextEntry ( firstFrame );
        treeNameIF->SetName ( "Tree Name IF" );
        treeNameIF->SetAlignment ( kTextRight );

        firstFrame->AddFrame ( treeNameLabel );
        firstFrame->AddFrame ( treeNameIF );

        TGLabel* nEntriesLabel = new TGLabel ( firstFrame, "Entries to treat (0 == all):" );
        TGNumberEntryField* nEntriesIF = new TGNumberEntryField ( firstFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive );
        nEntriesIF->SetName ( "Entries IF" );

        firstFrame->AddFrame ( nEntriesLabel );
        firstFrame->AddFrame ( nEntriesIF );

        TGLabel* isUpstreamLabel = new TGLabel ( firstFrame, "Upstream?" );
        TGCheckButton* isUpstreamCheckB = new TGCheckButton ( firstFrame );
        isUpstreamCheckB->SetName ( "Is Upstream CB" );

        firstFrame->AddFrame ( isUpstreamLabel );
        firstFrame->AddFrame ( isUpstreamCheckB );

        plotPosCalMF->AddFrame ( firstFrame, new TGLayoutHints ( kLHintsCenterX, 0, 0, 0, 0 ) );

        TGCompositeFrame* secondFrame = new TGCompositeFrame ( plotPosCalMF, 100, 100 );
        secondFrame->SetLayoutManager ( new TGMatrixLayout ( secondFrame, 2, 4, 5, 0 ) );

        TGLabel* xAxisBinsLabel = new TGLabel ( secondFrame, "X Axis Binning (nbins, min, max)" );

        TGNumberEntryField* nbinsXIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber );
        TGDimension defDim = nbinsXIF->GetDefaultSize();
        nbinsXIF->SetName ( "NBinsX IF" );
        TGNumberEntryField* binMinXIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber );
        binMinXIF->SetName ( "BinMinX IF" );
        TGNumberEntryField* binMaxXIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber );
        binMaxXIF->SetName ( "BinMaxX IF" );

        nbinsXIF->Resize ( 100, defDim.fHeight );
        binMinXIF->Resize ( 100, defDim.fHeight );
        binMaxXIF->Resize ( 100, defDim.fHeight );

        secondFrame->AddFrame ( xAxisBinsLabel );
        secondFrame->AddFrame ( nbinsXIF );
        secondFrame->AddFrame ( binMinXIF );
        secondFrame->AddFrame ( binMaxXIF );

        TGLabel* yAxisBinsLabel = new TGLabel ( secondFrame, "Y Axis Binning (nbins, min, max)" );

        TGNumberEntryField* nbinsYIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber );
        nbinsYIF->SetName ( "NBinsY IF" );
        TGNumberEntryField* binMinYIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber );
        binMinYIF->SetName ( "BinMinY IF" );
        TGNumberEntryField* binMaxYIF = new TGNumberEntryField ( secondFrame, -1, 0, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber );
        binMaxYIF->SetName ( "BinMaxY IF" );

        nbinsYIF->Resize ( 100, defDim.fHeight );
        binMinYIF->Resize ( 100, defDim.fHeight );
        binMaxYIF->Resize ( 100, defDim.fHeight );

        secondFrame->AddFrame ( yAxisBinsLabel );
        secondFrame->AddFrame ( nbinsYIF );
        secondFrame->AddFrame ( binMinYIF );
        secondFrame->AddFrame ( binMaxYIF );

        plotPosCalMF->AddFrame ( secondFrame, new TGLayoutHints ( kLHintsCenterX, 0, 0, 0, 0 ) );

        TGCompositeFrame* thirdFrame = new TGCompositeFrame ( plotPosCalMF, 100, 100 );
        thirdFrame->SetLayoutManager ( new TGMatrixLayout ( thirdFrame, 2, 2, 5, 5 ) );

        TGLabel* sectorsLabel = new TGLabel ( thirdFrame, "Sectors to treat \ne.g. : \"1, 4-8, 10\"" );
        TGTextEntry* sectorsIF = new TGTextEntry ( thirdFrame );
        sectorsIF->SetName ( "Sectors IF" );

        thirdFrame->AddFrame ( sectorsLabel );
        thirdFrame->AddFrame ( sectorsIF );

        TGTextButton* processButton = new TGTextButton ( thirdFrame, "Process", "GoddessCalib::ValidatePlotPosCalGraphs()" );
        TGTextButton* cancelButton = new TGTextButton ( thirdFrame, "Cancel", "GoddessCalib::FindWindowByName(\"Plot Position Calibration Graphs\")->UnmapWindow();" );

        thirdFrame->AddFrame ( processButton );
        thirdFrame->AddFrame ( cancelButton );

        plotPosCalMF->AddFrame ( thirdFrame, new TGLayoutHints ( kLHintsCenterX, 0, 0, 0, 0 ) );

        plotPosCalMF->MapSubwindows();
        plotPosCalMF->Resize ( plotPosCalMF->GetDefaultSize() );
        plotPosCalMF->MapWindow();
    }
    else if ( !plotPosCalWin->IsMapped() )
    {
        plotPosCalWin->MapSubwindows();
        plotPosCalWin->MapWindow();
    }
}

void GoddessCalib::OnClickUpdateConf()
{
    TGTextEntry* configNameIF = dynamic_cast<TGTextEntry*> ( FindFrameByName ( GoddessCalib::sinstance()->GetControlFrame(), "Config Full Name" ) );

//     if(configNameIF != NULL) std::cout << "Found the Input Field for the Config Name...\n";

    if ( !configNameIF->IsEnabled() )
    {
//         std::cout << "Config Name Input Field was disabled. Enabling it...\n";

        configNameIF->SetState ( kTRUE );
        configNameIF->SetEnabled ( kTRUE );
    }
    else
    {
        std::string confName = configNameIF->GetText();

        configNameIF->SetText ( "" );

//         std::cout << "Config Name Input Field was enabled. Read " << confName << "\n";

        if ( confName.length() > 0 ) GoddessCalib::sinstance()->UpdateResParamsInConf ( confName );

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
        std::cerr << "No Canvas available!\n";
        std::cerr << "To get help with the energy calibration procedure, type GoddessCalib::EnCalibHelp()\n";
        return;
    }

    auto canPrimList = gPad->GetCanvas()->GetListOfPrimitives();

    TGraph* gr;

    std::string grName_ = "";

    for ( int i = 0; i < canPrimList->GetSize(); i++ )
    {
        gr = dynamic_cast<TGraph*> ( canPrimList->At ( i ) );

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

        if ( sectPos != std::string::npos && stripPos != std::string::npos && separatorPos != std::string::npos )
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
        std::cerr << "The graph used do not fulfill the name requirement! Did you produce it with the GoddessCalib class built-in functions?\n";
        std::cerr << "To get help with the energy calibration procedure, type GoddessCalib::EnCalibHelp()\n";
        return;
    }

    TGNumberEntryField* alphaEnIF = ( TGNumberEntryField* ) FindWindowByName ( "alphaEn1IF" );

    float alphaEn1 = std::stof ( alphaEnIF->GetText() );

    GoddessCalib::sinstance()->currRefEn1 = alphaEn1;

    GoddessCalib::sinstance()->GetCornersCoordinates ( gPad->GetCanvas(), ( bool ) isUS_, ( short unsigned int ) sect_, ( short unsigned int ) strip_, GoddessCalib::sinstance()->currDetType, GoddessCalib::sinstance()->currRefEn1 );

    std::cout << std::endl;

    return;
}

void GoddessCalib::ValidateGetStripsEdges()
{
    if ( GoddessCalib::sinstance() == NULL ) return;

    TGTextEntry* fileNameIF = ( TGTextEntry* ) FindFrameByName ( GoddessCalib::sinstance()->s_instance->GetControlFrame(), "fNameIF" );

    std::string fileName = fileNameIF->GetText();

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
        std::cerr << "Invalid input file name..." << std::endl;
        return;
    }

    std::cout << "Opened file: " << input->GetName() << "\n";

    TGWindow* prompt = FindWindowByName ( "Get Strips Edges" );

    TGCheckButton* drawResCB = ( TGCheckButton* ) FindFrameByName ( prompt, "Draw Results CB" );
    TGNumberEntryField* projWidthIF = ( TGNumberEntryField* ) FindFrameByName ( prompt, "Proj Width IF" );

    int projWidth_ = std::stoi ( projWidthIF->GetText() );
    bool doDraw = drawResCB->GetState();

    GoddessCalib::sinstance()->GetStripsEdges ( input, projWidth_, doDraw );

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

        getStripsEdgesMF->SetLayoutManager ( new TGMatrixLayout ( getStripsEdgesMF, 3, 2, 5, 5 ) );

        TGLabel* projWidthLabel = new TGLabel ( getStripsEdgesMF, "Proj. Width (bins):" );
        TGTextEntry* projWidthIF = new TGTextEntry ( getStripsEdgesMF );
        projWidthIF->SetName ( "Proj Width IF" );
        projWidthIF->SetAlignment ( kTextRight );
        projWidthIF->SetText ( "200" );

        getStripsEdgesMF->AddFrame ( projWidthLabel );
        getStripsEdgesMF->AddFrame ( projWidthIF );

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

void GoddessCalib::StartSX3EnCalib ( std::string detectorType, double refEnergy1 )
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

void GoddessCalib::InitializeCalMapKey ( std::string mapKey, unsigned short strip )
{
    auto itr = resStripsCalMap.find ( mapKey );

    if ( itr == resStripsCalMap.end() )
    {
        std::cout << "No entry found for " << mapKey << " strip #" << strip << " in current session's map. Initializing it with -100" << std::endl;

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
            std::cout << "No entry found for " << mapKey << " strip #" << strip << ". Initializing it with -100" << std::endl;

            for ( int i = 0; i < 6; i++ )
            {
                resStripsCalMap[mapKey][strip].push_back ( -100 );
            }
        }
    }
}

void GoddessCalib::GetCornersCoordinates ( TCanvas* can, bool isUpstream, unsigned short sector, unsigned short strip, std::string detectorType, double refEnergy1 )
{
    std::string calMapKey = detectorType;
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

//         std::cout << "Found a line in the list at position #" << i << std::endl;
//         std::cout << "X1 : " << x1 << "   /   Y1 : " << y1 << std::endl;
//         std::cout << "X2 : " << x2 << "   /   Y2 ; " << y2 << std::endl;
//
//         std::cout << "Slope = " << slope << std::endl;

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

    std::vector<double>* calRes = &resStripsCalMap[calMapKey][strip];

    xIntersect = ( xIntersect == -100 ? calRes->at ( 0 ) : xIntersect );
    yIntersect = ( yIntersect == -100 ? calRes->at ( 1 ) : yIntersect );
    nearCorrFactor = ( nearCorrFactor == -100 ? calRes->at ( 2 ) : nearCorrFactor );

    calRes->at ( 0 ) = xIntersect;
    calRes->at ( 1 ) = yIntersect;
    calRes->at ( 2 ) = nearCorrFactor;

    if ( negLine != nullptr && calRes->at ( 0 ) != -100 && calRes->at ( 1 ) != -100 )
    {
        energyCalCoeff = refEnergy1 / ( ( negLine->GetX1() - calRes->at ( 1 ) ) * ( -slopeNeg ) + negLine->GetY1()  - calRes->at ( 2 ) );
    }

    energyCalCoeff = ( energyCalCoeff == -100 ? calRes->at ( 3 ) : energyCalCoeff );

    std::cout << "Intersection between top and bottom at ( " << xIntersect << " ; " << yIntersect << " )" << std::endl;
    std::cout << "Correction factor for the near strip = " << nearCorrFactor << std::endl;
    std::cout << "Slope for energy calibration = " << energyCalCoeff << std::endl;

    calRes->at ( 3 ) = energyCalCoeff;

    return;
}

bool GoddessCalib::DumpFileToResCalMap ( std::string fileName )
{
    std::ifstream readFile ( fileName.c_str() );

    if ( !readFile.is_open() )
    {
        std::cerr << "Failed to open file " << fileName << " for previous calibration reading (if it did not exist before, the file has now been created)" << std::endl;
        return false;
    }

    std::string dump;

    std::string lineID, detID, stripID, dummy;

    std::cout << "Reading " << fileName << "..." << std::endl;

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

            std::cout << "Found " << detID << " at position " << posInFile << " in input file..." << std::endl;

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

            std::cout << "Read the following values: " << xinter << " / " <<  yinter << " / " << slope_gm << " / " << slope_encal << " / " << pos_left_edge << " / " << pos_right_edge << std::endl;

            std::vector<double>* readCal = &resStripsCalMap[detID][stripNbr];

            if ( readCal->at ( 0 ) == -100 ) readCal->at ( 0 ) = xinter == -100 ? readCal->at ( 0 ) : xinter;
            if ( readCal->at ( 1 ) == -100 ) readCal->at ( 1 ) = yinter == -100 ? readCal->at ( 1 ) : yinter;
            if ( readCal->at ( 2 ) == -100 ) readCal->at ( 2 ) = slope_gm == -100 ? readCal->at ( 2 ) : slope_gm;
            if ( readCal->at ( 3 ) == -100 ) readCal->at ( 3 ) = slope_encal == -100 ? readCal->at ( 3 ) : slope_encal;
            if ( readCal->at ( 4 ) == -100 ) readCal->at ( 4 ) = pos_left_edge == -100 ? readCal->at ( 4 ) : pos_left_edge;
            if ( readCal->at ( 5 ) == -100 ) readCal->at ( 5 ) = pos_right_edge == -100 ? readCal->at ( 5 ) : pos_right_edge;

            std::cout << "New values: " << readCal->at ( 0 ) << " / " <<  readCal->at ( 1 ) << " / " << readCal->at ( 2 ) << " / " << readCal->at ( 3 ) << " / " << readCal->at ( 4 ) << " / " << readCal->at ( 5 ) << std::endl;
        }
    }

    return true;
}

void GoddessCalib::WriteResCalResults ( std::string fileName, std::string mode )
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

        for ( unsigned short i = 0; i < 4; i++ )
        {
            if ( itr->second.find ( i ) != itr->second.end() )
            {
                outStream << "Res. Strip #" << i << "   " << itr->second[i][0] << "   " << itr->second[i][1] << "   " << itr->second[i][2] << "   " << itr->second[i][3];
                outStream << "   " << itr->second[i][4] << "   " << itr->second[i][5] << "\n";
            }
        }
    }

    return;
}

bool GoddessCalib::UpdateResParamsInConf ( std::string configFile, bool invertContactMidDet, std::string mode )
{
    std::ifstream readFile ( configFile.c_str() );

    if ( !readFile.is_open() )
    {
        std::cerr << "Failed to open input file " << configFile << std::endl;
        return false;
    }

    std::string path = configFile.substr ( 0, configFile.find_last_of ( "/" ) );

    int foundExt = configFile.find ( "." );

    std::string newName = configFile;
    newName.insert ( foundExt, "_new" );

    std::ofstream outStream ( newName.c_str() );

    if ( !outStream.is_open() )
    {
        std::cerr << "Failed to open output file " << configFile << std::endl;
        return false;
    }

    std::string dump;

    std::string lineID, detID, stripID, dummy;

    std::cout << "Reading " << configFile << "..." << std::endl;

    while ( std::getline ( readFile, dump ) )
    {
        std::istringstream readLine;
        readLine.str ( dump );

        readLine >> lineID;

        if ( lineID != "superX3" )
        {
            outStream << dump << "\n";
            continue;
        }

        else
        {
            outStream << dump << "\n";

            readLine >> dummy >> detID;

//             int posInFile = readFile.tellg();

            std::cout << "Found a superX3 entry: " << detID << std::endl;

            std::getline ( readFile, dump );
            outStream << dump << "\n";

            std::getline ( readFile, dump );
            outStream << dump << "\n";

            std::string detKey = "SuperX3 " + detID.substr ( 0,detID.find ( "-" ) );

            if ( resStripsCalMap.find ( detKey ) != resStripsCalMap.end() )
            {
                for ( int i =0; i < 4; i++ )
                {
                    if ( resStripsCalMap[detKey].find ( i ) != resStripsCalMap[detKey].end() )
                    {
                        std::cout << "Replacing the parameters for strip #" << i << " with the new values..." << std::endl;

                        float offNear, offFar, slopeNear, slopeFar, offEnCal, slopeEnCal, pos_left_edge, pos_right_edge;

                        std::getline ( readFile, dump );
                        readLine.clear();
                        readLine.str ( dump );

                        readLine >> dummy >> dummy>> dummy >> offNear >> slopeNear;

                        offNear = resStripsCalMap[detKey][i][0] != -100 ? resStripsCalMap[detKey][i][0] : offNear;
                        slopeNear = resStripsCalMap[detKey][i][2] != -100 ? resStripsCalMap[detKey][i][2] : slopeNear;

                        std::getline ( readFile, dump );
                        readLine.clear();
                        readLine.str ( dump );

                        readLine >> dummy >> dummy>> dummy >> offFar >> slopeFar;

                        offFar = resStripsCalMap[detKey][i][1] != -100 ? resStripsCalMap[detKey][i][1] : offFar;

                        if ( i < 2 || !invertContactMidDet )
                        {
                            outStream << "enCal p " << i*2 << " " << offNear << " " << slopeNear << "\n";
                            outStream << "enCal p " << i*2 + 1 << " " << offFar << " " << slopeFar << "\n";
                        }
                        else
                        {
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

TGraph* GoddessCalib::PlotSX3ResStripCalGraph ( TTree* tree, std::string varToPlot, unsigned short sector, unsigned short strip, std::string conditions )
{
    std::size_t upStreamCondPos = conditions.find ( "si.isUpstream" );

    std::string upStreamCond = "U";

    if ( upStreamCondPos == std::string::npos )
    {
        std::cerr << "isUpstream condition missing..." << std::endl;
        return 0;
    }

    if ( conditions[upStreamCondPos-1] == '!' ) upStreamCond = "D";

    std::cout << "Plotting sector #" << sector << " strip #" << strip << "..." << std::endl;

    tree->Draw ( Form ( "%s", varToPlot.c_str() ), Form ( "%s && si.sector == %d && si.E1.strip.p@.size() > 0 && si.E1.strip.p == %d", conditions.c_str(), sector, strip ) );

    TGraph* newGraph = ( TGraph* ) gPad->GetPrimitive ( "Graph" );

    if ( newGraph == nullptr ) return 0;

    std::string gName = Form ( "sector%s%d_strip%d", upStreamCond.c_str(), sector, strip );

    newGraph->SetName ( gName.c_str() );
    newGraph->SetTitle ( gName.c_str() );

    std::string currPath = ( std::string ) gDirectory->GetPath();

    std::string rootFileName = "Resistive_Strips_Calib_Graphs_";

    std::string treeFName = tree->GetCurrentFile()->GetName();

    std::size_t begRunName = treeFName.find ( "run" );
    std::size_t endRunName = treeFName.find ( "_", begRunName );

    if ( begRunName != std::string::npos && endRunName != std::string::npos ) rootFileName += treeFName.substr ( begRunName, endRunName ) + ".root";
    else rootFileName += treeFName;

    TFile* f = new TFile ( rootFileName.c_str(), "update" );

    if ( f->FindKey ( gName.c_str() ) != nullptr || f->FindObject ( gName.c_str() ) != nullptr )
    {
        std::string objToDelete = gName + ";*";
        f->Delete ( objToDelete.c_str() );
    }

    f->cd();

    newGraph->Write();

    f->Close();

    gDirectory->cd ( currPath.c_str() );

    resStripsEnCalGraphsMap[Form ( "sector %s%d strip %d", upStreamCond.c_str(), sector, strip )] = newGraph;

    return newGraph;
}

void GoddessCalib::PlotSX3ResStripsCalGraphsFromTree ( TTree* tree, long int nentries, bool isUpstream_, std::vector< unsigned short > sectorsList )
{
    std::cout << sectorsList.size() << " sectors will be treated..." << std::endl;

    int sizeOfSectors = sectorsList.size();

//     int numberOfGraphs = sizeOfSectors*4;

    for ( int i =0; i < sizeOfSectors; i++ )
    {
        for ( int j =0; j < 4; j++ )
        {
            std::string grID = Form ( "sector%s%d_strip%d", isUpstream_ ? "U" : "D", sectorsList[i], j );

            std::cout << "Creating graph " << grID << std::endl;

            TGraph* newGraph = new TGraph();

            newGraph->SetName ( grID.c_str() );
            newGraph->SetTitle ( grID.c_str() );

            resStripsEnCalGraphsMap[Form ( "sector %s%d strip %d", isUpstream_ ? "U" : "D", sectorsList[i], j )] = newGraph;

//             std::cout << "Stored graph in the TGraph map..." << std::endl;
        }
    }

    if ( nentries == 0 || nentries > tree->GetEntries() ) nentries = tree->GetEntries();

    std::cout << nentries << " entries will be treated" <<std::endl;

    std::vector<SiDataDetailed>* siInfo = new std::vector<SiDataDetailed>();
    siInfo->clear();

    std::cout << "Preparing the readout of the tree..." << std::endl;

    tree->SetBranchAddress ( "si", &siInfo );

//     std::cout << "Linked the \"si\" branch to a SiDataDetailed object..." << std::endl;

    for ( int i = 0; i < nentries; i++ )
    {
        tree->GetEntry ( i );

        if ( i%10000 == 0 ) std::cout << "Treated " << i << " / " << nentries << " entries ( " << ( ( float ) i ) / ( ( float ) nentries ) * 100. << "% )\r" << std::flush;

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
                    std::string grID = Form ( "sector %s%d strip %d", isUpstream_ ? "U" : "D", sectorNbr, siInfo->at ( j ).E1.strip.p[st] );

                    TGraph* gr = resStripsEnCalGraphsMap[grID];

                    gr->SetPoint ( gr->GetN(), siInfo->at ( j ).E1.en.p[st], siInfo->at ( j ).E1.en.n[st] );
                }
            }
        }
    }

    std::cout << std::endl;

    std::string currPath = ( std::string ) gDirectory->GetPath();

    std::string rootFileName = "Resistive_Strips_Calib_Graphs_";

    std::string treeFName = tree->GetCurrentFile()->GetName();

    std::size_t begRunName = treeFName.find ( "run" );
    std::size_t endRunName = treeFName.find ( "_", begRunName );

    if ( begRunName != std::string::npos && endRunName != std::string::npos ) rootFileName += treeFName.substr ( begRunName, endRunName ) + ".root";
    else rootFileName += treeFName;

    TFile* f = new TFile ( rootFileName.c_str(), "update" );

    f->cd();

    for ( auto itr = resStripsEnCalGraphsMap.begin(); itr != resStripsEnCalGraphsMap.end(); itr++ )
    {
        TGraph* gr = itr->second;

        if ( f->FindKey ( gr->GetName() ) != NULL || f->FindObject ( gr->GetName() ) != NULL )
        {
            std::string objToDelete = gr->GetName();
            objToDelete += ";*";
            f->Delete ( objToDelete.c_str() );
        }

        std::cout << "Writing " << gr->GetName() << " to file..." << std::endl;

        gr->Write();
    }

    f->Close();

    gDirectory->cd ( currPath.c_str() );
}

TGraph* GoddessCalib::DrawPosCalGraph ( TTree* tree, bool isUpstream_, int nentries, unsigned short sector_, unsigned short strip_ )
{
    TGraph* newGraph = new TGraph();

    DrawPosCal ( tree, isUpstream_, sector_,strip_, nentries, newGraph );

    newGraph->Draw ( "AP" );

    return newGraph;
}

TH2F* GoddessCalib::DrawPosCalHist ( TTree* tree, bool isUpstream_, int nentries, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts, unsigned short sector_, unsigned short strip_ )
{
    std::string hname = Form ( "hPosCal_sector%s%d_strip%d", isUpstream_ ? "U" : "D", sector_, strip_ );

    std::string isUpstreamID = isUpstream_ ? "U" : "D";
    std::string hKey = Form ( "%s%d_%d", isUpstreamID.c_str(), sector_, strip_ );
    resStripsPosCalGraphsMap[hKey.c_str()] = new TH2F ( hname.c_str(), hname.c_str(), nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY );

    DrawPosCal ( tree, isUpstream_, sector_,strip_, nentries, resStripsPosCalGraphsMap[hKey.c_str()] );

    resStripsPosCalGraphsMap[hKey.c_str()]->Draw ( drawOpts.c_str() );

    return resStripsPosCalGraphsMap[hKey.c_str()];
}

std::map<std::string, TH2F*> GoddessCalib::DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries,
        int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts, unsigned short sector_ )
{
    return DrawPosCalHistBatch ( tree, isUpstream_, nentries,nbinsX,binMinX, binMaxX, nbinsY, binMinY, binMaxY, drawOpts, sector_ );
}

std::map<std::string, TH2F*> GoddessCalib::DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries,
        int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts, std::vector< unsigned short > sectorsList )
{
    std::string isUpstreamID = isUpstream_ ? "U" : "D";

    for ( unsigned short i = 0; i < sectorsList.size(); i++ )
    {
        for ( int j = 0; j < 4; j++ )
        {
            std::string hname = Form ( "%s%d_%d", isUpstreamID.c_str(), sectorsList[i], j );

            std::cout << "Initializing graph for sector " << isUpstreamID << sectorsList[i] << " strip " << j << " and storing it in the map at key \"" << hname << "\"" << std::endl;

            resStripsPosCalGraphsMap[hname.c_str()] = new TH2F ( hname.c_str(), hname.c_str(), nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY );
        }
    }

    std::vector<SiDataDetailed>* siDataVect = new std::vector<SiDataDetailed>();
    siDataVect->clear();

    tree->SetBranchAddress ( "si", &siDataVect );

    if ( nentries > tree->GetEntries() || nentries == 0 ) nentries = tree->GetEntries();

    for ( int i = 0; i < nentries; i++ )
    {
        tree->GetEntry ( i );

        if ( i%10000 == 0 ) std::cout << "Treated " << i << " / " << nentries << " entries ( " << ( ( float ) i ) / ( ( float ) nentries ) * 100. << " % )\r" << std::flush;

        if ( siDataVect->size() == 0 ) continue;

        for ( unsigned short j = 0; j < siDataVect->size(); j++ )
        {
            SiDataDetailed siData = siDataVect->at ( j );

            if ( siData.isBarrel && siData.isUpstream == isUpstream_ && siData.E1.en.p.size() > 0 && siData.E1.en.n.size() > 0 )
            {
                int sect = -1;

                for ( unsigned short sec = 0; sec < sectorsList.size(); sec++ )
                {
                    if ( siData.sector == sectorsList[sec] ) sect = siData.sector;
                }

                if ( sect == -1 ) continue;

                for ( unsigned short k = 0; k < siData.E1.en.p.size(); k++ )
                {
                    TH2F* hh = resStripsPosCalGraphsMap[Form ( "%s%d_%d", isUpstreamID.c_str(), sect, siData.E1.strip.p[k] )];
                    hh->Fill ( ( siData.E1.en.p[k] - siData.E1.en.n[k] ) / ( siData.E1.en.p[k] + siData.E1.en.n[k] ), ( siData.E1.en.p[k] + siData.E1.en.n[k] ) );
                    break;
                }
            }
        }
    }

    std::cout << std::endl;

    WritePosCalHistsToFile ( tree, "resistive_Strips_PosCal_hists" );

    return resStripsPosCalGraphsMap;
}

void GoddessCalib::DrawSX3EnCalGraph ( bool isUpstream, short unsigned int sector, short unsigned int strip )
{
    std::string isUpstreamID = isUpstream ? "U" : "D";
    std::string hKey = Form ( "sector %s%d strip %d", isUpstreamID.c_str(), sector, strip );

    auto itr = resStripsEnCalGraphsMap.find ( hKey );

    if ( itr != resStripsEnCalGraphsMap.end() )
    {
        itr->second->Draw ( "AP" );
    }
    else
        std::cerr << "This graph has not been generated yet!" << std::endl;
}

void GoddessCalib::DrawSX3PosCalHist ( bool isUpstream, short unsigned int sector, short unsigned int strip, std::string drawOpts )
{
    std::string isUpstreamID = isUpstream ? "U" : "D";
    std::string hKey = Form ( "%s%d_%d", isUpstreamID.c_str(), sector, strip );

    auto itr = resStripsPosCalGraphsMap.find ( hKey );

    if ( itr != resStripsPosCalGraphsMap.end() )
    {
        itr->second->Draw ( drawOpts.c_str() );
    }
    else
        std::cerr << "This graph has not been generated yet!" << std::endl;
}

void GoddessCalib::WritePosCalHistsToFile ( TTree* tree, std::string fileName )
{
    std::string currPath = ( std::string ) gDirectory->GetPath();

    std::string rootFileName = fileName + "_";

    std::string treeFName = tree->GetCurrentFile()->GetName();

    std::size_t begRunName = treeFName.find ( "run" );
    std::size_t endRunName = treeFName.find ( "_", begRunName );

    if ( begRunName != std::string::npos && endRunName != std::string::npos ) rootFileName += treeFName.substr ( begRunName, endRunName ) + ".root";
    else rootFileName += treeFName;

    TFile* f = new TFile ( rootFileName.c_str(), "update" );

    for ( auto itr = resStripsPosCalGraphsMap.begin(); itr != resStripsPosCalGraphsMap.end(); itr++ )
    {
        if ( f->FindKey ( itr->second->GetName() ) != nullptr || f->FindObject ( itr->second->GetName() ) != nullptr )
        {
            std::string objToDelete = itr->second->GetName();
            objToDelete += ";*";
            f->Delete ( objToDelete.c_str() );
        }

        f->cd();

        itr->second->Write();
    }

    f->Close();

    gDirectory->cd ( currPath.c_str() );
}

TGraph* GoddessCalib::GetSX3EnCalGraph ( bool isUpstream, short unsigned int sector, short unsigned int strip )
{
    std::string isUpstreamID = isUpstream ? "U" : "D";
    std::string hKey = Form ( "sector %s%d strip %d", isUpstreamID.c_str(), sector, strip );

    auto itr = resStripsEnCalGraphsMap.find ( hKey );

    if ( itr != resStripsEnCalGraphsMap.end() )
    {
        return itr->second;
    }
    else
    {
        std::cerr << "This graph has not been generated yet!" << std::endl;
        return 0;
    }
}

TH2F* GoddessCalib::GetSX3PosCalHist ( bool isUpstream, short unsigned int sector, short unsigned int strip )
{
    std::string isUpstreamID = isUpstream ? "U" : "D";
    std::string hKey = Form ( "%s%d_%d", isUpstreamID.c_str(), sector, strip );

    auto itr = resStripsPosCalGraphsMap.find ( hKey );

    if ( itr != resStripsPosCalGraphsMap.end() )
    {
        return itr->second;
    }
    else
    {
        std::cerr << "This graph has not been generated yet!" << std::endl;
        return 0;
    }
}

int GoddessCalib::GetPosCalEnBinMax ( TH2F* input )
{
    int fstBin = input->GetXaxis()->GetFirst();
    int lstBin = input->GetXaxis()->GetLast();

    int binMax = 0;

    TH1D* proj = input->ProjectionY ( ( ( std::string ) "projY_" + input->GetName() ).c_str(), fstBin, lstBin );

    binMax = proj->GetMaximumBin();

    return binMax;
}

TH1D* GoddessCalib::GetPosCalProjX ( TH2F* input, int projCenter, int projWidth )
{
    TH1D* proj = input->ProjectionX ( ( ( std::string ) "projX_" + input->GetName() ).c_str(), projCenter - projWidth/2, projCenter + projWidth/2 );

    return proj;
}

TF1* GoddessCalib::FitLeftEdge ( TH2F* input, int projWidth )
{
    int binMaxY = GetPosCalEnBinMax ( input );

    TH1D* projX = GetPosCalProjX ( input, binMaxY, projWidth );

    int binMaxX = projX->GetMaximumBin();

    double maxContent = projX->GetBinContent ( binMaxX );

    int startBin = binMaxX;
    double startBinContent = maxContent;

    bool fellBelow20Perc = false;
    int binShoulder = -1;

    int counterMax = projX->GetNbinsX() / 30;

    while ( startBin > projX->GetXaxis()->GetFirst() && binShoulder == -1 )
    {
        int prevBin = startBin;
        double prevBinContent = startBinContent;

        int nextBin = startBin - 1;
        double nextBinContent = projX->GetBinContent ( nextBin );
        bool foundIncrease = false;

        int counter = 0;

        while ( nextBin > projX->GetXaxis()->GetFirst() && counter < counterMax )
        {
            if ( nextBinContent < startBinContent*0.2 )
            {
                fellBelow20Perc = true;
                binShoulder = startBin;
                break;
            }

            if ( nextBinContent > prevBinContent )
            {
                if ( !foundIncrease ) foundIncrease = true;
                else break;
            }

//             if ( nextBinContent > startBinContent ) break;

            prevBin = nextBin;
            prevBinContent = nextBinContent;

            nextBin--;
            nextBinContent = projX->GetBinContent ( nextBin );

            counter++;
        }

        startBin--;
        startBinContent = projX->GetBinContent ( startBin );
    }

    std::cout << "Found the left shoulder at bin #" << binShoulder << " (value = " << projX->GetBinCenter ( binShoulder ) << ")" << std::endl;

    TF1* fitfunc = new TF1 ( Form ( "myfit_%s",input->GetName() ), "[0]*exp(-0.5*((x-[1])/[2])**2)", projX->GetBinCenter ( binShoulder - counterMax ), projX->GetBinCenter ( binShoulder ) );

    if ( binShoulder != -1 )
    {
        fitfunc->FixParameter ( 0, projX->GetBinContent ( binShoulder ) );
        fitfunc->FixParameter ( 1, projX->GetBinCenter ( binShoulder ) );
        fitfunc->SetParameter ( 2, projX->GetBinCenter ( binShoulder ) - projX->GetBinCenter ( binShoulder - 1 ) );

        projX->Fit ( fitfunc, "QRMN" );

        float leftEdge = fitfunc->GetParameter ( 1 ) - TMath::Sqrt ( -2*pow ( fitfunc->GetParameter ( 2 ),2 ) * TMath::Log ( 0.7 ) );

        std::cout << "Found the left strip edge at " << leftEdge << std::endl;

        std::string hname = input->GetName();
        std::string calMapKey = "SuperX3 " + hname.substr ( 0, hname.find ( "_" ) );

        int stripNbr = std::stoi ( hname.substr ( hname.find ( "_" ) + 1 ) );

        InitializeCalMapKey ( calMapKey, stripNbr );

        resStripsCalMap[calMapKey][stripNbr][4] = leftEdge;
    }
    return fitfunc;
}

TF1* GoddessCalib::FitRightEdge ( TH2F* input, int projWidth )
{
    int binMaxY = GetPosCalEnBinMax ( input );

    TH1D* projX = GetPosCalProjX ( input, binMaxY, projWidth );

    int binMaxX = projX->GetMaximumBin();

    double maxContent = projX->GetBinContent ( binMaxX );

    int startBin = binMaxX;
    double startBinContent = maxContent;

    bool fellBelow20Perc = false;
    int binShoulder = -1;

    int counterMax = projX->GetNbinsX() / 30;

    while ( startBin < projX->GetXaxis()->GetLast() && binShoulder == -1 )
    {
        int prevBin = startBin;
        double prevBinContent = startBinContent;

        int nextBin = startBin + 1;
        double nextBinContent = projX->GetBinContent ( nextBin );
        bool foundIncrease = false;

        int counter = 0;

        while ( nextBin < projX->GetXaxis()->GetLast() && counter < counterMax )
        {
            if ( nextBinContent < startBinContent*0.2 )
            {
                fellBelow20Perc = true;
                binShoulder = startBin;
                break;
            }

            if ( nextBinContent > prevBinContent )
            {
                if ( !foundIncrease ) foundIncrease = true;
                else break;
            }

            prevBin = nextBin;
            prevBinContent = nextBinContent;

            nextBin++;
            nextBinContent = projX->GetBinContent ( nextBin );

            counter++;
        }

        startBin++;
        startBinContent = projX->GetBinContent ( startBin );
    }

    std::cout << "Found the right shoulder at bin #" << binShoulder << " (value = " << projX->GetBinCenter ( binShoulder ) << ")" << std::endl;

    TF1* fitfunc = new TF1 ( Form ( "myfit_%s",input->GetName() ), "[0]*exp(-0.5*((x-[1])/[2])**2)", projX->GetBinCenter ( binShoulder ), projX->GetBinCenter ( binShoulder + counterMax ) );

    if ( binShoulder != -1 )
    {
        fitfunc->FixParameter ( 0, projX->GetBinContent ( binShoulder ) );

        fitfunc->FixParameter ( 0, projX->GetBinContent ( binShoulder ) );
        fitfunc->FixParameter ( 1, projX->GetBinCenter ( binShoulder ) );
        fitfunc->SetParameter ( 2, projX->GetBinCenter ( binShoulder ) - projX->GetBinCenter ( binShoulder - 1 ) );

        projX->Fit ( fitfunc, "QRMN" );

        float rightEdge = fitfunc->GetParameter ( 1 ) + TMath::Sqrt ( -2*pow ( fitfunc->GetParameter ( 2 ),2 ) * TMath::Log ( 0.7 ) );

        std::cout << "Found the right strip edge at " << rightEdge << std::endl;

        std::string hname = input->GetName();
        std::string calMapKey = "SuperX3 " + hname.substr ( 0, hname.find ( "_" ) );

        int stripNbr = std::stoi ( hname.substr ( hname.find ( "_" ) + 1 ) );

        InitializeCalMapKey ( calMapKey, stripNbr );

        resStripsCalMap[calMapKey][stripNbr][5] = rightEdge;
    }
    return fitfunc;
}

void GoddessCalib::GetStripsEdges ( int projWidth, bool drawResults )
{
    for ( auto itr = resStripsPosCalGraphsMap.begin(); itr != resStripsPosCalGraphsMap.end(); itr++ )
    {
        std::cout << "Retreiving the edges of sector " << itr->first.substr ( 0, itr->first.find ( "_" ) ) << " strip #" << itr->first.substr ( itr->first.find ( "_" ) ) << " ..." << std::endl;

        TF1* lfit = FitLeftEdge ( itr->second, projWidth );
        TF1* rfit = FitRightEdge ( itr->second, projWidth );

        if ( drawResults )
        {
            TCanvas* newCanvas = new TCanvas ( Form ( "c_%s", itr->first.c_str() ) );

            newCanvas->cd();

            GetPosCalProjX ( itr->second, GetPosCalEnBinMax ( itr->second ), projWidth )->Draw();
            lfit->Draw ( "same" );
            rfit->Draw ( "same" );
        }
    }

    return;
}

void GoddessCalib::GetStripsEdges ( TH2F* input, int projWidth, bool drawResults )
{
    std::string hname = input->GetName();

    std::cout << "Retreiving the edges of sector " << hname.substr ( 0, hname.find ( "_" ) ) << " strip #" << hname.substr ( hname.find ( "_" ) ) << " ..." << std::endl;

    TF1* lfit = FitLeftEdge ( input, projWidth );
    TF1* rfit = FitRightEdge ( input, projWidth );

    if ( drawResults )
    {
        TCanvas* newCanvas = new TCanvas ( Form ( "c_%s", hname.c_str() ) );

        newCanvas->cd();

        GetPosCalProjX ( input, GetPosCalEnBinMax ( input ), projWidth )->Draw();
        lfit->Draw ( "same" );
        rfit->Draw ( "same" );
    }

    return;
}

void GoddessCalib::GetStripsEdges ( TFile* input, int projWidth, bool drawResults )
{
    auto lOK = input->GetListOfKeys();

    for ( int i = 0; i < lOK->GetSize(); i++ )
    {
        TH2F* hist = dynamic_cast<TH2F*> ( input->Get ( lOK->At ( i )->GetName() ) );

        if ( hist != nullptr )
        {
            std::string hname = hist->GetName();

            if ( ! ( hname[0] == 'U' || hname[0] == 'D' ) && ! ( hname.find ( "_" ) == 2 || hname.find ( "_" ) == 3 ) ) continue;

            std::cout << "Retreiving the edges of sector " << hname.substr ( 0, hname.find ( "_" ) ) << " strip #" << hname.substr ( hname.find ( "_" ) ) << " ..." << std::endl;

            TF1* lfit = FitLeftEdge ( hist, projWidth );
            TF1* rfit = FitRightEdge ( hist, projWidth );

            if ( drawResults )
            {
                TCanvas* newCanvas = new TCanvas ( Form ( "c_%s", hname.c_str() ) );

                newCanvas->cd();

                GetPosCalProjX ( hist, GetPosCalEnBinMax ( hist ), projWidth )->Draw();
                lfit->Draw ( "same" );
                rfit->Draw ( "same" );
            }
        }
    }

    return;
}

TGraph* GoddessCalib::PlotSX3ResStripCalGraph ( TTree* tree, bool isUpstream, unsigned short sector, unsigned short strip )
{
    std::string upstreamCond = isUpstream ? "si.isUpstream" : "!si.isUpstream" ;
    std::string cond = "si.isBarrel && " + upstreamCond;

    return PlotSX3ResStripCalGraph ( tree, "si.E1.en.n:si.E1.en.p", sector, strip, cond );
}

void GoddessCalib::PlotSX3ResStripsCalGraphsFromTree()
{
    std::cout << "To generate the graphs for several sectors without drawing them (MUCH faster), call" << std::endl;
    std::cout << "PlotSX3ResStripsCalGraphsFromTree(TTree* tree, bool isUpstream, int nentries, int sector1, int sector2, int sector3, int ....)" << std::endl;
    std::cout << "where \"nenteries\" controls the number of entries to treat (0 == all the entries)" << std::endl;
}

void GoddessCalib::PlotSX3ResStripsCalGraphs()
{
    std::cout << "To plot several sectors in a row, call" << std::endl;
    std::cout << "PlotSX3ResStripsCalGraphs(TTree* tree, bool isUpstream, int sector1, int sector2, int sector3, int ....)" << std::endl;
    std::cout << std::endl;
    std::cout << "You can also change what to plot and specify the conditions by hand by calling" << std::endl;
    std::cout << "PlotSX3ResStripsCalGraphs(TTree* tree, string \"what to plot\", string conditions, sector1, sector2, sector3, ....)" << std::endl;
}

void GoddessCalib::Help()
{
    std::cout << "To plot the Enear vs Efar graph for strip X of sector Y, call" << std::endl;
    std::cout << "PlotSX3ResStripCalGraph(TTree* tree, bool isUpstream, int sector1, int strip)" << std::endl;
    std::cout << std::endl;
    PlotSX3ResStripsCalGraphs();
    std::cout << std::endl;
    PlotSX3ResStripsCalGraphsFromTree();
    std::cout << std::endl;
    std::cout << "To get help with the energy calibration procedure, call \"GoddessCalib::EnCalibHelp()\"" << std::endl;
    std::cout << std::endl;
    std::cout << "To get help with the position calibration procedure, call \"GoddessCalib::PosCalibHelp()\"" << std::endl;
    std::cout << std::endl;
    std::cout << "To reload a file containing previous calibration and update it, call" << std::endl;
    std::cout << "DumpFileToResCalMap(string \"previous calib file name\")" << std::endl;
    std::cout << std::endl;
}

void GoddessCalib::EnCalibHelp()
{
    std::cout << "To start the SX3 energy calibration porecedure, type \"GoddessCalib::StartSX3EnCalib()\" after opening a TBrowser or TCanvas." << std::endl;
    std::cout << std::endl;
    std::cout << "The energy calibration for the resistive strips needs several items:" << std::endl;
    std::cout << "2 TLines following the edges of the strip in the Enear vs. Efar graph" << std::endl;
    std::cout << "One TLine following the alpha line from a run with source in the Enear vs. Efar graph" << std::endl;
    std::cout << "NOTE: You can draw the TLine following the alpha peak separately but the 2 TLine for the edges has to be done from the same graph." << std::endl;
    std::cout << "To save the info about the TLine which have been drawn, click on the \"Save TLines Info\" button (top right of the canvas)" << std::endl;
    std::cout << std::endl;
    std::cout << "The info about the TLines drawn are stored in a map that can be saved at any time."<< std::endl;
    std::cout << "To write the results of the calibrations made during the last session, call" << std::endl;
    std::cout << "WriteResCalResults(string \"result file name\", string option = \"recreate\")" << std::endl;
    std::cout << std::endl;
    std::cout << "To update a config file with the results of the calibrations made during the last session, call" << std::endl;
    std::cout << "UpdateResParamsInConf(string \"config file name\", bool invertContactMidDet = true, string mode = \"protected\")" << std::endl;
    std::cout << "* invertContactMidDet should be set to \"true\" for SuperX3 because of the way the contacts numbers are incremented" << std::endl;
    std::cout << "* the \"protected\" mode will prevent you to overwrite your config file and generate a new config file from the input..." << std::endl;
    std::cout << "  switch it to \"overwrite\" if you really know what you're doing" << std::endl;
    std::cout << std::endl;

}

void GoddessCalib::PosCalibHelp()
{
    std::cout << "Not written yet... Come back later!" << std::endl;
    std::cout << std::endl;
}

ClassImp ( GoddessCalib )





















