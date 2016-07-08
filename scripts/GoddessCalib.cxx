#include "GoddessCalib.h"

GoddessCalib* GoddessCalib::s_instance = 0;

GoddessCalib::GoddessCalib() : GoddessAnalysis()
{
    controlFrame = new TGMainFrame ( gClient->GetRoot(),200,200 );

    controlFrame->Connect ( "Destroyed()", "GoddessCalib", sinstance(), "DeleteSInstance()" );

    TGVerticalFrame* vertFrame = new TGVerticalFrame ( controlFrame, 200, 200 );

    vertFrame->SetBackgroundColor ( 0x4d004d );

    TGTextButton* sSX3CB = new TGTextButton ( vertFrame, "Start SuperX3 Calib", "GoddessCalib::StartSX3EnCalib()" );
    sSX3CB->SetFont ( "-*-helvetica-medium-r-*-*-20-*-*-*-*-*-iso8859-1" );

    vertFrame->AddFrame ( sSX3CB, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 100, 100, 20, 20 ) );

    TGTextButton* dCPB = new TGTextButton ( vertFrame, "Reload Prev. Cal. Params", "GoddessCalib::sinstance()->DumpFileToResCalMap()" );

    vertFrame->AddFrame ( dCPB, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 100, 100, 0, 20 ) );

    TGHorizontalFrame* subHorzFrame = new TGHorizontalFrame ( vertFrame, 200, 400 );
    subHorzFrame->SetBackgroundColor ( 0x4d004d );

    TGTextButton* wCPB = new TGTextButton ( subHorzFrame, "Write New Cal. Params", "GoddessCalib::sinstance()->WriteResCalResults()" );
    TGTextButton* uCFB = new TGTextButton ( subHorzFrame, "Update Goddess Config File", "GoddessCalib::sinstance()->UpdateResParamsInConf()" );

    subHorzFrame->AddFrame ( wCPB, new TGLayoutHints ( kLHintsCenterX, 10, 10, 0, 0 ) );
    subHorzFrame->AddFrame ( uCFB, new TGLayoutHints ( kLHintsCenterX, 10, 10, 0, 0 ) );
    
    vertFrame->AddFrame ( subHorzFrame, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 100, 100, 20, 20 ) );

    TGTextButton* gCB = new TGTextButton ( vertFrame, "Save TLines Info", "GoddessCalib::OnClickGetLinesInfo()" );

    vertFrame->AddFrame ( gCB, new TGLayoutHints ( kLHintsCenterX | kLHintsTop, 100, 100, 60, 150 ) );

    controlFrame->AddFrame ( vertFrame );

    controlFrame->MapSubwindows();
    controlFrame->Resize ( controlFrame->GetDefaultSize() );
    controlFrame->MapWindow();

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

    GoddessCalib::sinstance()->GetCornersCoordinates ( gPad->GetCanvas(), ( bool ) isUS_, ( short unsigned int ) sect_, ( short unsigned int ) strip_, GoddessCalib::sinstance()->currDetType, GoddessCalib::sinstance()->currRefEn1 );

    std::cout << std::endl;

    return;
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

            if(readCal->at ( 0 ) == -100) readCal->at ( 0 ) = xinter == -100 ? readCal->at ( 0 ) : xinter;
            if(readCal->at ( 1 ) == -100) readCal->at ( 1 ) = yinter == -100 ? readCal->at ( 1 ) : yinter;
            if(readCal->at ( 2 ) == -100) readCal->at ( 2 ) = slope_gm == -100 ? readCal->at ( 2 ) : slope_gm;
            if(readCal->at ( 3 ) == -100) readCal->at ( 3 ) = slope_encal == -100 ? readCal->at ( 3 ) : slope_encal;
            if(readCal->at ( 4 ) == -100) readCal->at ( 4 ) = pos_left_edge == -100 ? readCal->at ( 4 ) : pos_left_edge;
            if(readCal->at ( 5 ) == -100) readCal->at ( 5 ) = pos_right_edge == -100 ? readCal->at ( 5 ) : pos_right_edge;

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

    std::string path = configFile.substr ( 0, configFile.find_last_of ( "/" ) );

    int foundExt = configFile.find ( "." );

    std::string newName = configFile;
    newName.insert ( foundExt, "_new" );

    std::ofstream outStream ( newName.c_str() );

    if ( !readFile.is_open() )
    {
        std::cerr << "Failed to open file " << configFile << std::endl;
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

std::map<std::string, TH2F*> GoddessCalib::DrawPosCalHistBatch ( TTree* tree, bool isUpstream_, int nentries, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, std::string drawOpts, unsigned short sector_ )
{
    return DrawPosCalHistBatch ( tree, isUpstream_, nentries,nbinsX,binMinX, binMaxX, nbinsY, binMinY, binMaxY, drawOpts, sector_ );
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












