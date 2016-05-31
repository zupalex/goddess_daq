#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <type_traits>

#include "TROOT.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TList.h"
#include "TLine.h"
#include "TGraph.h"
#include "TTree.h"
#include "TH2F.h"
#include "TFile.h"

std::map<string, double[4][5]> resistiveStripsCalMap;

std::map<string, TGraph*> resStripsGraphsMap;

void GetCornersCoordinates ( TCanvas* can, bool isUpstream, unsigned short sector, unsigned short strip, string detectorType = "SuperX3", float refEnergy1 = 5.813 )
{
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

        if ( line == NULL ) continue;

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
            if ( tempLine == NULL )
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

    double slopeTop, slopeBot, slopeNeg;
    double offTop, offBot, offNeg;

    slopeTop = ( topLine->GetY2() - topLine->GetY1() ) / ( topLine->GetX2() - topLine->GetX1() );
    slopeBot = ( botLine->GetY2() - botLine->GetY1() ) / ( botLine->GetX2() - botLine->GetX1() );
    slopeNeg = ( negLine->GetY2() - negLine->GetY1() ) / ( negLine->GetX2() - negLine->GetX1() );

    offTop = ( topLine->GetX1() * topLine->GetY2() - topLine->GetX2() * topLine->GetY1() ) / ( topLine->GetX1() - topLine->GetX2() );
    offBot = ( botLine->GetX1() * botLine->GetY2() - botLine->GetX2() * botLine->GetY1() ) / ( botLine->GetX1() - botLine->GetX2() );
    offNeg = ( negLine->GetX1() * negLine->GetY2() - negLine->GetX2() * negLine->GetY1() ) / ( negLine->GetX1() - negLine->GetX2() );

//     std::cout << "Neg Line : " << slopeNeg << " * x + " << offNeg << std::endl;
//     std::cout << "Top Line : " << slopeTop << " * x + " << offTop << std::endl;
//     std::cout << "Bottom Line : " << slopeBot << " * x + " << offBot << std::endl;

    double xIntersect, yIntersect;

    xIntersect = ( offTop - offBot ) / ( slopeBot - slopeTop );
    yIntersect = slopeTop * xIntersect + offTop;

    std::cout << "Intersection between top and bottom at ( " << xIntersect << " ; " << yIntersect << " )" << std::endl;
    std::cout << "Correction factor for the near strip = " << -1/slopeNeg << std::endl;
    std::cout << "Slope for energy calibration = " << refEnergy1/ ( offNeg - yIntersect ) << std::endl;

    string calMapKey = detectorType;
    calMapKey += isUpstream ? " U" : " D";
    calMapKey += std::to_string ( sector );

    auto calRes = resistiveStripsCalMap[calMapKey][strip];

    calRes[0] = 1;
    calRes[1] = xIntersect;
    calRes[2] = yIntersect;
    calRes[3] = -1/slopeNeg;
    calRes[4] = refEnergy1/ ( offNeg - yIntersect );

    return;
}

bool DumpFileToResCalMap ( string fileName )
{
    std::ifstream readFile ( fileName.c_str() );

    if ( !readFile.is_open() )
    {
        std::cerr << "Failed to open file " << fileName << std::endl;
        return false;
    }

    string dump;

    string lineID, detID, stripID, dummy;

    std::cout << "Reading " << fileName << "..." << std::endl;

    while ( std::getline ( readFile, dump ) )
    {
        istringstream readLine;
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


        if ( lineID == "Res." )
        {
            readLine >> dummy >> stripID;

            unsigned short stripNbr = std::stoul ( stripID.substr ( 1 ) );

            float xinter, yinter, slope_gm, slope_encal;

            readLine >> xinter >> yinter >> slope_gm >> slope_encal;

            auto readCal = resistiveStripsCalMap[detID][stripNbr];

            readCal[0] = 1;
            readCal[1] = xinter;
            readCal[2] = yinter;
            readCal[3] = slope_gm;
            readCal[4] = slope_encal;
        }
    }

    return true;
}

void WriteResCalResults ( string fileName, string mode = "recreate" )
{
    if ( mode == "update" )
    {
        if ( DumpFileToResCalMap ( fileName.c_str() ) ) remove ( fileName.c_str() );
    }

    std::ofstream outStream ( fileName.c_str() );

    outStream << "Columns are: \"Strip #\"      \"Offset X\"      \"Offset Y\"      \"Slope (gain match)\"     \"Slope (energy calibration)\"\n";

    for ( auto itr = resistiveStripsCalMap.begin(); itr != resistiveStripsCalMap.end(); itr++ )
    {
        outStream << "\n" << itr->first << "\n";

        for ( unsigned short i = 0; i < 4; i++ )
        {
            if ( itr->second[i][0] == 1 )
            {
                outStream << "Res. Strip #" << i << "   " << itr->second[i][1] << "   " << itr->second[i][2] << "   " << itr->second[i][3] << "   " << itr->second[i][4] << "\n";
            }
        }
    }

    return;
}

bool UpdateResParamsInConf ( string configFile, bool invertContactMidDet = true, string mode = "protected" )
{
    std::ifstream readFile ( configFile.c_str() );

    string path = configFile.substr ( 0, configFile.find_last_of ( "/" ) );

    int foundExt = configFile.find ( "." );

    string newName = configFile;
    newName.insert ( foundExt, "_new" );

    std::ofstream outStream ( newName.c_str() );

    if ( !readFile.is_open() )
    {
        std::cerr << "Failed to open file " << configFile << std::endl;
        return false;
    }

    string dump;

    string lineID, detID, stripID, dummy;

    std::cout << "Reading " << configFile << "..." << std::endl;

    while ( std::getline ( readFile, dump ) )
    {
        istringstream readLine;
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

            int posInFile = readFile.tellg();

            std::cout << "Found a superX3 entry: " << detID << std::endl;

            std::getline ( readFile, dump );
            outStream << dump << "\n";

            std::getline ( readFile, dump );
            outStream << dump << "\n";

            string detKey = "SuperX3 " + detID.substr ( 0,detID.find ( "-" ) );

            if ( resistiveStripsCalMap.find ( detKey ) != resistiveStripsCalMap.end() )
            {
                for ( int i =0; i < 4; i++ )
                {
                    if ( resistiveStripsCalMap[detKey][i][0] == 1 )
                    {
                        std::cout << "Replacing the parameters for strip #" << i << " with the new values..." << std::endl;

                        std::getline ( readFile, dump );
                        std::getline ( readFile, dump );
                        std::getline ( readFile, dump );

                        if ( i < 2 || !invertContactMidDet )
                        {
                            outStream << "enCal p " << i*2 << " " << resistiveStripsCalMap[detKey][i][1] << " " << resistiveStripsCalMap[detKey][i][3] << "\n";
                            outStream << "enCal p " << i*2 + 1 << " " << resistiveStripsCalMap[detKey][i][2] << " " << 1 << "\n";
                            outStream << "enCal resStrip " << i << " 0 " << resistiveStripsCalMap[detKey][i][4] << "\n";
                        }
                        else
                        {
                            outStream << "enCal p " << i*2 << " " << resistiveStripsCalMap[detKey][i][2] << " " << 1 << "\n";
                            outStream << "enCal p " << i*2 + 1 << " " << resistiveStripsCalMap[detKey][i][1] << " " << resistiveStripsCalMap[detKey][i][3] << "\n";
                            outStream << "enCal resStrip " << i << " 0 " << resistiveStripsCalMap[detKey][i][4] << "\n";
                        }
                    }
                    else
                    {
                        for ( int i =0; i < 3; i++ )
                        {
                            std::getline ( readFile, dump );
                            outStream << dump << "\n";
                        }
                    }
                }
            }
            else
            {
                for ( int i = 0; i < 12; i++ )
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


TGraph* PlotSX3ResStripCalGraph ( TTree* tree, string varToPlot, unsigned short sector, unsigned short strip, string conditions )
{
    std::cout << "Plotting sector #" << sector << " strip #" << strip << "..." << std::endl;

    tree->Draw ( Form ( "%s", varToPlot.c_str() ), Form ( "%s && si.sector == %d && si.E1.strip.p@.size() > 0 && si.E1.strip.p == %d", conditions.c_str(), sector, strip ) );

//     std::cout << "Retrieving TGraph*..." << std::endl;

    TGraph* newGraph = ( TGraph* ) gPad->GetPrimitive ( "Graph" );

    if ( newGraph == NULL ) return 0;

    string gName = Form ( "sector%d_strip%d", sector, strip );

    newGraph->SetName ( gName.c_str() );

    string currPath = ( string ) gDirectory->GetPath();

    TFile* f = new TFile ( "Resistive_Strips_Calib_Graphs.root", "update" );

    if ( f->FindKey ( gName.c_str() ) != NULL || f->FindObject ( gName.c_str() ) != NULL )
    {
//         std::cout << "Deleting existing TGraph..." << std::endl;
        string objToDelete = gName + ";*";
        f->Delete ( objToDelete.c_str() );
    }

    f->cd();

//     std::cout << "Writing new TGraph..." << std::endl;

    newGraph->Write();

    f->Close();

    gDirectory->cd ( currPath.c_str() );

//     std::cout << "Storing the new TGraph in the TGraph map..." << std::endl;

    resStripsGraphsMap[Form ( "sector %d strip %d", sector, strip )] = newGraph;

    return newGraph;
}

TGraph* PlotSX3ResStripCalGraph ( TTree* tree, bool isUpstream, unsigned short sector, unsigned short strip )
{
    string upstreamCond = isUpstream ? "si.isUpstream" : "!si.isUpstream" ;
    string cond = "si.isBarrel && " + upstreamCond;

    return PlotSX3ResStripCalGraph ( tree, "si.E1.en.n:si.E1.en.p", sector, strip, cond );
}

template<typename T> void PlotSX3ResStripsCalGraphs ( TTree* tree, string varToPlot, string conditions, T sector )
{
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 0, conditions );
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 1, conditions );
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 2, conditions );
    PlotSX3ResStripCalGraph ( tree, varToPlot, sector, 3, conditions );

    return;
}

template<typename First, typename... Rest> void PlotSX3ResStripsCalGraphs ( TTree* tree, string varToPlot, string conditions, First fstSector, Rest... otherSectors )
{
    if ( std::is_same<decltype ( fstSector ), int>::value  || ( unsigned short ) fstSector < 0 )
    {
        PlotSX3ResStripsCalGraphs<unsigned short> ( tree, varToPlot, conditions, fstSector );

        if ( sizeof... ( otherSectors ) > 0 )
        {
            PlotSX3ResStripsCalGraphs ( tree, varToPlot, conditions, otherSectors... );
        }
    }
    else
    {
        std::cerr << "One of the sector specified has an invalid type! (expecting positive integer)" << std::endl;
    }

    return;
}

template<typename First, typename... Rest> void PlotSX3ResStripsCalGraphs ( TTree* tree, bool isUpstream, First fstSector, Rest... otherSectors )
{
    string upstreamCond = isUpstream ? "si.isUpstream" : "!si.isUpstream" ;
    string cond = "si.isBarrel && " + upstreamCond;

    PlotSX3ResStripsCalGraphs<First, Rest...> ( tree, "si.E1.en.n:si.E1.en.p", cond, fstSector, otherSectors... );

    return;
}

void PlotSX3ResStripsCalGraphs()
{
    std::cout << "To plot several sectors in a row, call" << std::endl;
    std::cout << "PlotSX3ResStripsCalGraphs(TTree* tree, bool isUpstream, int sector1, int sector2, int sector3, int ....)" << std::endl;
    std::cout << std::endl;
    std::cout << "You can also change what to plot and specify the conditions by hand by calling" << std::endl;
    std::cout << "PlotSX3ResStripsCalGraphs(TTree* tree, string \"what to plot\", string conditions, sector1, sector2, sector3, ....)" << std::endl;
}

void CalibHelp()
{
    std::cout << "To plot the strip X of sector Y, call" << std::endl;
    std::cout << "PlotSX3ResStripCalGraph(TTree* tree, bool isUpstream, int sector1, int strip)" << std::endl;
    std::cout << std::endl;
    PlotSX3ResStripsCalGraphs();
    std::cout << std::endl;
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
