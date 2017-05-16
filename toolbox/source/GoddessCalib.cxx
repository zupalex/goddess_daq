#include "GoddessCalib.h"

GoddessCalib* GoddessCalib::s_instance = 0;
GoddessCalib* gC;

map<string, string> stoppingPowerTable;

map<string, std::pair<float, float>> configCalPars;

GoddessCalib::GoddessCalib() : GoddessAnalysis()
{
    calChain = nullptr;

    InitGUI();

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

void ReadConfigCalPars ( string configFileName )
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

            if ( sectorsList[i] <= 6 && sectorsList[i] != 0 )
            {
                binnum = 2000;
                starthere = 0;
                endhere = 150000;
            }
            else if ( sectorsList[i] > 6 || sectorsList[i] == 0 )
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

//         float edge = fitfunc->GetParameter ( 1 ) - TMath::Sqrt ( -2*pow ( fitfunc->GetParameter ( 2 ),2 ) * TMath::Log ( 0.7 ) );
        float edge = fitfunc->GetX ( 0.7 * fitfunc->GetParameter ( 0 ),
                                     ( fitRight ? fitfunc->GetParameter ( 1 ) : fitfunc->GetParameter ( 1 ) - 5*fitfunc->GetParameter ( 2 ) ),
                                     ( fitRight ? fitfunc->GetParameter ( 1 ) + 5*fitfunc->GetParameter ( 2 ) : fitfunc->GetParameter ( 1 ) ) );

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

void GoddessCalib::GenerateGainAdjustfile ( string filesname, string treename, long long int nEntries, string outfname, double minAdjust, double maxAdjust, double stepWidth )
{
    std::vector<string> rootfiles = DecodeItemsToTreat ( filesname, "system", true, true );

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

    auto energyLossData = FillGraphFromFile ( stoppingPowerTable["ejectile"] );

    TVector3 targetPos = geomInfo->GetOffset ( "Target Offset" );

    TVector3 beamDir ( 0, 0, 1 );

    auto targetLadderDir = TVector3 ( 0, 0, 1 );
    targetLadderDir.SetTheta ( geomInfo->targetLadderAngle*TMath::DegToRad() );
    targetLadderDir.SetPhi ( TMath::PiOver2() );

    double beamEffThickness = GetEffectiveThickness ( beamDir.Angle ( targetLadderDir ) - TMath::PiOver2(), reacInfo->targetThickness );

    reacInfo->beamEk = TryGetRemainingEnergy ( pathToGDAQ + "/share/mass_db.dat", reacInfo->beamA, reacInfo->beamZ, reacInfo->beamEk, beamEffThickness, 0.001,
                       reacInfo->targetType, reacInfo->targetDensity, "./", "Interpolation" );

    cout << "Beam Energy after computing energy loss: " << reacInfo->beamEk << "MeV in effective thickness: " << beamEffThickness << " mg/cm2\n";

    vector<SiDataBase>* vectSiData = new vector<SiDataBase>;

    chain->SetBranchAddress ( "si", &vectSiData );

    double adjust = minAdjust;

    map<int, TH1F*> hQQQ5[4][32];

    while ( adjust <= maxAdjust )
    {
        for ( int sect = 0; sect < 4; sect++ )
        {
            for ( int strip = 0; strip < 32; strip++ )
            {
                hQQQ5[sect][strip][RoundValue ( adjust*100 )] = new TH1F ( Form ( "hEx_QQQ5_U%d_%d_gain_%d", sect, strip, RoundValue ( adjust*100 ) ),
                        Form ( "Excitation Energy QQQ5 U%d strip %d gain mod %d %%", sect, strip, RoundValue ( adjust*100 ) ),
                        800, -20, 20 );
            }
        }

        adjust+= stepWidth;
    }

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

                double energy = siData.ESumLayer ( 1,false );
                int sector = siData.sector;
                int strip = siData.StripMaxLayer ( 1, false );

                if ( siData.isUpstream && !siData.isBarrel && sector >= 0 && sector <= 3 && strip >= 0 && strip <= 31 )
                {
                    double effThickness = GetEffectiveThickness ( siData.PosE1().Angle ( targetLadderDir ) - TMath::PiOver2(), reacInfo->targetThickness );
                    double estELoss = ComputeEnergyLoss ( energyLossData.first, energyLossData.second, energy/reacInfo->ejecA, reacInfo->ejecA, 0, effThickness, 0.01, "Interpolation" );

                    //             cout << "Input energy : " << initialEnergy << " / Estimated energy loss : " << estELoss << " MeV in effective thickness: " << effThickness <<endl;

                    energy += estELoss;

                    adjust = minAdjust;

                    while ( adjust <= maxAdjust )
                    {
                        double qVal = SiDataBase::QValue ( reacInfo, energy*adjust, siData.PosE1().Angle ( beamDir ) );

                        double eX = reacInfo->qValGsGs - qVal;

                        hQQQ5[sector][strip][RoundValue ( adjust*100 )]->Fill ( eX );

                        adjust+= stepWidth;
                    }
                }
            }
        }
    }

    TFile* outf = new TFile ( outfname.c_str(), "recreate" );

    outf->cd();

    for ( int sect = 0; sect < 4; sect++ )
    {
        for ( int strip = 0; strip < 32; strip++ )
        {
            for ( auto itr = hQQQ5[sect][strip].begin(); itr != hQQQ5[sect][strip].end(); itr++ ) itr->second->Write();
        }
    }

    outf->mkdir ( "infos" );
    outf->cd ( "infos" );
    geomInfo->Write ( "GoddessGeom" );
    reacInfo->Write ( "GoddessReac" );
    outf->Close();

    cout << endl;

    return;
}

TF1* FitQVal ( TH1* hist, vector< string > mean, float fwhm_min, float fwhm_max, float minBound, float maxBound, bool verbose )
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
            if ( dependantFrom[i] == -1 ) cout << fitFunc->GetParameter ( 4 + 2*i ) << " +/- " << fitFunc->GetParError ( 4 + 2*i ) << " MeV\n";
            else
            {
                cout << fitFunc->GetParameter ( 4 + dependantFrom[i]*i ) + fitFunc->GetParameter ( 4 + 2*i );
                cout << " +/- " << fitFunc->GetParError ( 4 + dependantFrom[i]*i ) + fitFunc->GetParError ( 4 + 2*i ) << " MeV ( Depends from Peak #" << dependantFrom[i] << " : ";
                cout << " energy difference = " << fabs ( fitFunc->GetParameter ( 4 + 2*i ) ) << " +/- " << fitFunc->GetParError ( 4 + 2*i );
                cout << " / Ratio = " << fitFunc->GetParameter ( 3 + 2*i ) / fitFunc->GetParameter ( 3 + dependantFrom[i]*i ) << " )\n";
            }
        }
        cout << "Sigma = " << fitFunc->GetParameter ( 2 ) << " MeV\n";
        cout << "Chi2 = " << fitFunc->GetChisquare() << "\n";
    }

    return fitFunc;
}

TF1* FitQValGS ( TH1* hist, vector<float> mean, float fwhm, float peakRatio, float minBound, float maxBound, bool verbose )
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

// vector<double> GoddessCalib::AdjustQValSpectrum ( vector<std::map<int, TH1F*>>* hists, float peakPos, float fwhm, string crossSectionInput,
//         float minBound, float maxBound, int minModEndcaps_, int maxModEndcaps_, string betterFitMode )
// {
//     vector<double> finalMods;
//
//     double testMods[32];
//
//     for ( int i = 0; i < 32; i++ )
//     {
//         finalMods.push_back ( 100 );
//     }
//
//     TH1F* sum = AddAllStrips ( hists, finalMods );
//
//     sum->Draw();
//
//     // ---------------------
//
//     double peakRatio = GetRatioGSvsFirstEx ( crossSectionInput, 120, 160 );
//
//     TF1* firstFitFunc = FitQValGS ( sum, {peakPos, peakPos-2}, fwhm, peakRatio, minBound, maxBound );
//
//     double firstChi2, firstMagn, firstMean, firstSigma, firstIntegral, firstHistIntegral, firstOffset, firstSlope;
//
//     firstMagn = firstFitFunc->GetParameter ( 4 );
//     firstMean = firstFitFunc->GetParameter ( 5 );
//     firstSigma = firstFitFunc->GetParameter ( 3 );
//
//     firstOffset = firstFitFunc->GetParameter ( 0 );
//     firstSlope = firstFitFunc->GetParameter ( 1 );
//
//     firstChi2 = firstFitFunc->GetChisquare();
//
//     firstIntegral = firstMagn * TMath::Sqrt ( 2*TMath::Pi() ) * firstSigma;
//
//     double totIntegral = firstFitFunc->Integral ( firstMean - 3*firstSigma, firstMean + 3*firstSigma );
//     double bkgrIntegral = 6*firstSigma * ( firstSlope*firstMean + firstOffset ) ;
//     firstHistIntegral = totIntegral - bkgrIntegral;
//
//     // ------------------------
//
//     TF1* bestFitFunc = ( TF1* ) firstFitFunc->Clone();
//
//     double bestChi2, bestMagn, bestMean, bestSigma, bestIntegral, bestHistIntegral, bestOffset, bestSlope;
//
//     bestMagn = firstMagn;
//     bestMean = firstMean;
//     bestSigma = firstSigma;
//
//     bestOffset = firstOffset;
//     bestSlope = firstSlope;
//
//     bestChi2 = firstChi2;
//
//     bestIntegral = firstIntegral;
//
//     bestHistIntegral = firstHistIntegral;
//
//     // -------------------------
//
//     TF1* fitFunc;
//
//     double chi2, magn, mean, sigma, integral, histIntegral, offset, slope;
//
//     // -------------------------
//
//     auto linkMap = MakeLinkMap ( "chi2 bestChi2 firstChi2 sigma bestSigma firstSigma magn bestMagn firstMagn gaussIntegral bestGaussIntegral firstGaussIntegral rawIntegral bestRawIntegral firstRawIntegral",
//                                  chi2, bestChi2, firstChi2, sigma, bestSigma, firstSigma, magn, bestMagn, firstMagn, integral, bestIntegral, firstIntegral, histIntegral, bestHistIntegral, firstHistIntegral );
//
//     cout << "\n";
//
//     for ( int i = 0; i < 32; i++ )
//     {
//         std::cout << "Adjusted " << std::setw ( 2 ) << i << " / 32 strips...\r" << std::flush;
//
//         for ( int j = 0; j < 32; j++ )
//         {
//             testMods[j] = finalMods[j];
//         }
//
//         int modCoeff = minModEndcaps_;
//
//         while ( modCoeff <= maxModEndcaps_ )
//         {
//             testMods[i] = modCoeff;
//
//             TH1F* testSum = AddAllStrips ( hists, testMods );
//
//             fitFunc = FitQValGS ( testSum, {peakPos, peakPos-2}, fwhm, peakRatio, minBound, maxBound );
//
//             magn = fitFunc->GetParameter ( 4 );
//             mean = fitFunc->GetParameter ( 5 );
//             sigma = fitFunc->GetParameter ( 3 );
//
//             offset = fitFunc->GetParameter ( 0 );
//             slope = fitFunc->GetParameter ( 1 );
//
//             chi2 = fitFunc->GetChisquare();
//
//             totIntegral = fitFunc->Integral ( mean - 3*sigma, mean + 3*sigma );
//             bkgrIntegral = 6*sigma * ( slope*mean + offset ) ;
//             histIntegral = totIntegral - bkgrIntegral;
//
//             integral = magn * TMath::Sqrt ( 2*TMath::Pi() ) * sigma;
//
//             bool goodFit = false;
//
//             goodFit = StringFormulaComparator<double> ( betterFitMode, &linkMap );
//
//             if ( goodFit ) finalMods[i] = modCoeff;
//
//             if ( chi2 < bestChi2 && sigma <= bestSigma && magn >= bestMagn && integral >= bestIntegral && histIntegral >= bestHistIntegral )
//             {
//                 bestFitFunc = ( TF1* ) fitFunc->Clone();
//
//                 bestMagn = magn;
//                 bestMean = mean;
//                 bestSigma = sigma;
//
//                 bestOffset = offset;
//                 bestSlope = slope;
//
//                 bestChi2 = chi2;
//
//                 bestIntegral = integral;
//
//                 bestHistIntegral = histIntegral;
//             }
//
//             modCoeff++;
//         }
//     }
//
//     cout << "\n";
//
//     for ( int i = 0; i < 8; i++ )
//     {
//         cout << std::setw ( 2 ) << i << ": " << std::setw ( 6 ) << finalMods[i] << "   |   ";
//         cout << std::setw ( 2 ) << i+8 << ": " << std::setw ( 6 ) << finalMods[i+8] << "   |   ";
//         cout << std::setw ( 2 ) << i+16 << ": " << std::setw ( 6 ) << finalMods[i+16] << "   |   ";
//         cout << std::setw ( 2 ) << i+24 << ": " << std::setw ( 6 ) << finalMods[i+24] << "\n";
//     }
//
//     cout << "\n";
//
//     if ( hists == &hQVal_QQQ5UA ) endcapsStripsCalMap["QQQ5 UA"] = finalMods;
//     else if ( hists == &hQVal_QQQ5UB ) endcapsStripsCalMap["QQQ5 UB"] = finalMods;
//     else if ( hists == &hQVal_QQQ5UC ) endcapsStripsCalMap["QQQ5 UC"] = finalMods;
//     else if ( hists == &hQVal_QQQ5UD ) endcapsStripsCalMap["QQQ5 UD"] = finalMods;
//     else
//     {
//         cerr << "Map used as an input is not recognized. Results are not stored...\n";
//     }
//
//     return finalMods;
// }

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















































