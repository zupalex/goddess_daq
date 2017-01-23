#include "GoddessToolbox.h"

std::vector<int> DecodeNumberString ( std::string sectorsString, bool verbose )
{
    std::vector<int> sectorsNum;

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

        int fstPush;

        if ( sub1.length() > 0 )
        {
            while ( sub1.find ( " " ) != std::string::npos )
            {
                sub1.replace ( sub1.find ( " " ), 1, "" );
            }

            bool isDigit = true;

            for ( unsigned int i = 0; i < sub1.length(); i++ )
            {
                if ( !isdigit ( sub1[i] ) ) isDigit = false;
            }

            if ( isDigit ) fstPush = stoi ( sub1 );
            else
            {
                fstPush = ( int ) sub1[0] - ( int ) ( 'A' );

                if ( sub1.length() > 1 || fstPush > 25 )
                {
                    cerr << "Invalid string to decode\n";

                    sectorsNum.clear();

                    return sectorsNum;
                }
            }

            sectorsNum.push_back ( fstPush );

            if ( sub2.length() > 0 )
            {
                while ( sub2.find ( " " ) != std::string::npos )
                {
                    sub2.replace ( sub2.find ( " " ), 1, "" );
                }

                int lowerBound = fstPush;
                int upperBound;

                isDigit = true;

                for ( unsigned int i = 0; i < sub2.length(); i++ )
                {
                    if ( !isdigit ( sub2[i] ) ) isDigit = false;
                }

                if ( isDigit ) upperBound = stoi ( sub2 );
                else
                {
                    upperBound = ( int ) sub2[0] - ( int ) ( 'A' );

                    if ( sub2.length() > 1 || upperBound > 25 || upperBound < lowerBound )
                    {
                        cerr << "Invalid string to decode\n";

                        sectorsNum.clear();

                        return sectorsNum;
                    }
                }

                for ( int i = lowerBound+1; i < upperBound; i++ )
                {
                    sectorsNum.push_back ( i );
                }
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

std::vector<std::string> DecodeTags ( std::string tagsStr )
{
    std::vector<std::string> tags;

    std::size_t startPos = tagsStr.find_first_not_of ( " ," );;

    std::size_t separator = tagsStr.find_first_of ( " ,", startPos );

    std::string newTag;

    while ( separator != std::string::npos && startPos != std::string::npos )
    {
        newTag = tagsStr.substr ( startPos, separator - startPos );

        tags.push_back ( newTag );

        startPos = tagsStr.find_first_not_of ( " ,", separator );
        separator = tagsStr.find_first_of ( " ,", startPos );
    }

    if ( startPos != std::string::npos )
    {
        newTag = tagsStr.substr ( startPos, tagsStr.length() - startPos );

        tags.push_back ( newTag );
    }

    return tags;
}

vector< string > GetDirContent ( string dirName, string mode, string fileExt, string mustHaveAll, string cantHaveAny, string mustHaveOneOf, string startWith )
{
    std::vector<std::string> fileList, mustAllTags, cantTags, mustOneOfTags;

    if ( !mustHaveAll.empty() )
    {
        mustAllTags = DecodeTags ( mustHaveAll );
    }

    if ( !cantHaveAny.empty() )
    {
        cantTags = DecodeTags ( cantHaveAny );
    }

    if ( !mustHaveOneOf.empty() )
    {
        mustOneOfTags = DecodeTags ( mustHaveOneOf );
    }

    DIR* toScan = nullptr;
    dirent* dirEntry = nullptr;

    if ( mode == "system" )
    {
        if ( dirName.empty() ) dirName = "./";
        toScan = opendir ( dirName.c_str() );
        dirEntry = new dirent;
    }

    TIterator* rootListItr = nullptr;
    TObject* rootListEntry = nullptr;

    if ( mode == "root" )
    {
        if ( !dirName.empty() ) gDirectory->cd ( dirName.c_str() );

        auto rootLOK = gDirectory->GetList();

        if ( rootLOK != nullptr )
        {
            rootListItr = rootLOK->MakeIterator();
            rootListEntry = rootListItr->Next();
        }
    }

    while ( ( mode == "system" && ( dirEntry = readdir ( toScan ) ) != NULL ) || ( mode == "root" && rootListItr != nullptr && rootListEntry != nullptr ) )
    {
        std::string entName;

        if ( mode == "system" ) entName = dirEntry->d_name;
        else if ( mode == "root" )
        {
            entName = rootListEntry->GetName();

            rootListEntry = rootListItr->Next();
        }
        else
        {
            cerr << "Invalid mode specified...\n";
            fileList.clear();
            return fileList;
        }

        if ( startWith.empty() || ( !startWith.empty() && entName.find ( startWith.c_str() ) == 0 ) )
        {
            if ( fileExt.empty() || ( !fileExt.empty() && entName.find ( fileExt.c_str() ) != std::string::npos ) )
            {
                bool mustAllFlag = true, cantFlag = false, mustOneOfFlag = ( mustOneOfTags.size() > 0 ? false : true );

                if ( mustAllTags.size() > 0 )
                {
                    for ( unsigned int i = 0; i < mustAllTags.size(); i++ )
                    {
                        if ( entName.find ( mustAllTags[i].c_str() ) == std::string::npos ) mustAllFlag = false;
                    }
                }

                if ( cantTags.size() > 0 )
                {
                    for ( unsigned int i = 0; i < cantTags.size(); i++ )
                    {
                        if ( entName.find ( cantTags[i].c_str() ) != std::string::npos ) cantFlag = true;
                    }
                }

                if ( mustOneOfTags.size() > 0 )
                {
                    for ( unsigned int i = 0; i < mustOneOfTags.size(); i++ )
                    {
                        if ( entName.find ( mustOneOfTags[i].c_str() ) != std::string::npos ) mustOneOfFlag = true;
                    }
                }

                if ( mustAllFlag && !cantFlag && mustOneOfFlag ) fileList.push_back ( entName );
            }
        }
    }

    return fileList;
}

std::vector<std::string> DecodeItemsToTreat ( std::string itemsString, string mode )
{
    std::size_t pathEnd = itemsString.find_last_of ( "/" );

    std::string path = "";

    if ( pathEnd != std::string::npos )
    {
        path = itemsString.substr ( 0, pathEnd );
        itemsString = itemsString.substr ( pathEnd+1 );
    }

    std::vector<std::string> files;

    if ( itemsString == "*" ) return GetDirContent ( path, mode, "", "", "", "", "" );

    std::size_t startPos = itemsString.find_first_not_of ( "*[]" );
    std::size_t special = itemsString.find_first_of ( "*[]", startPos );

    std::vector<std::string> fNameParts;
    std::vector<std::tuple<std::size_t, char, std::size_t>> sChars;

    std::string fStartWith;
    std::string fMustHaveAll;
    std::string fMustHaveOneOf;

    if ( special == std::string::npos )
    {
        files.push_back ( itemsString );
    }
    else
    {
        if ( startPos == 0 ) fStartWith = itemsString.substr ( 0, special );

        while ( special != std::string::npos )
        {
            fNameParts.push_back ( itemsString.substr ( startPos, special - startPos ) );

//             std::cout << "new name part: " << itemsString.substr ( startPos, special - startPos ) << "\n";

            sChars.push_back ( std::make_tuple ( special, itemsString[special], startPos ) );

//             std::cout << "separator: " << itemsString[special] << " @ " << special << "\n";

            startPos = itemsString.find_first_not_of ( "*[]", special );
            special = itemsString.find_first_of ( "*[]", startPos );
        }

        if ( startPos != std::string::npos )
        {
            fNameParts.push_back ( itemsString.substr ( startPos, itemsString.length() ) );

            fMustHaveAll += itemsString.substr ( startPos, itemsString.length() );
            fMustHaveAll += " ";

//             std::cout << "new must all tag: " << itemsString.substr ( startPos, itemsString.length() ) << "\n";

//             std::cout << "new name part: " << itemsString.substr ( startPos, special - startPos ) << "\n";
        }

        for ( unsigned int i = 0; i < sChars.size(); i++ )
        {
            if ( std::get<1> ( sChars[i] ) == '*' )
            {
                fMustHaveAll += itemsString.substr ( std::get<2> ( sChars[i] ), std::get<0> ( sChars[i] ) - std::get<2> ( sChars[i] ) );
                fMustHaveAll += " ";

//                 std::cout << "new must all tag: " << itemsString.substr ( std::get<2> ( sChars[i] ), std::get<0> ( sChars[i] ) - std::get<2> ( sChars[i] ) ) << "\n";
            }

            else if ( std::get<1> ( sChars[i] ) == '[' )
            {
                if ( i == sChars.size()-1 || std::get<1> ( sChars[i+1] ) != ']' )
                {
                    std::cerr << "Failed to decode the files input...\n";

                    return files;
                }
                else
                {
                    std::string toDecode = itemsString.substr ( std::get<0> ( sChars[i] ) +1, std::get<0> ( sChars[i+1] ) - std::get<0> ( sChars[i] ) - 1 );

                    std::vector<int> filesNbr = DecodeNumberString ( toDecode, false );

                    std::string baseStr = itemsString.substr ( std::get<2> ( sChars[i] ), std::get<0> ( sChars[i] ) - std::get<2> ( sChars[i] ) );

                    for ( unsigned int j = 0; j < filesNbr.size(); j++ )
                    {
                        fMustHaveOneOf += Form ( "%s%d", baseStr.c_str(), filesNbr[j] );
                        fMustHaveOneOf += " ";

//                         std::cout << "new must one of tag: " << Form ( "%s%d", baseStr.c_str(), filesNbr[j] ) << "\n";
                    }

                    i++;
                }
            }

            else if ( std::get<1> ( sChars[i] ) == ']' )
            {
                std::cerr << "Failed to decode the files input...\n";

                return files;
            }
        }

        files = GetDirContent ( path, mode, "", fMustHaveAll, "", fMustHaveOneOf, fStartWith );
    }

    return files;
}

vector< string > SplitString ( string toSplit, string splitter )
{
    std::size_t startPos = 0;
    std::size_t comaPos = toSplit.find ( splitter.c_str() );

    vector<string> splitString;
    splitString.clear();

    while ( comaPos != string::npos )
    {
        splitString.push_back ( toSplit.substr ( startPos, comaPos- startPos ) );

        startPos = comaPos+splitter.length();
        comaPos = toSplit.find ( splitter.c_str(), startPos );
    }

    splitString.push_back ( toSplit.substr ( startPos ) );

    return splitString;
}

// string FindAndReplaceInString ( string input, string toReplace, string substitute, unsigned int nTimes, unsigned int startIndex )
// {
//     int repSize = toReplace.length();
// 
// //     vector<std::size_t> foundPosList;
// 
//     std::size_t foundPos = input.find ( toReplace.c_str() );
// 
// //     while ( foundPos != string::npos )
// //     {
// //         foundPosList.push_back ( foundPos );
// //
// //         foundPos = input.find ( toReplace.c_str(), foundPos+1 );
// //     }
// //
// //     nTimes = std::min ( nTimes, ( unsigned int ) ( foundPosList.size()-startIndex ) );
// //
// //     string newString = input;
// //
// //     for ( unsigned int i = startIndex; i < startIndex+nTimes; i++ )
// //     {
// //         newString.replace ( foundPosList[i], repSize, substitute );
// //     }
// 
//     unsigned int counter = 0;
//     unsigned int replaced = 0;
//     bool noLimit = false;
//     size_t skipPos = 0;
//     if ( nTimes == 0 )
//     {
//         nTimes = replaced+1;
//         noLimit = true;
//     }
// 
//     string newString = input;
// 
//     while ( foundPos != string::npos && replaced < nTimes )
//     {
//         if ( counter >= startIndex )
//         {
//             newString.replace ( foundPos, repSize, substitute );
// 
//             foundPos = newString.find ( toReplace.c_str(), skipPos );
//             replaced++;
//         }
//         else
//         {
//             skipPos = foundPos+repSize;
//             foundPos = newString.find ( toReplace.c_str(), skipPos );
//         }
// 
//         counter++;
//         if ( noLimit ) nTimes = replaced+1;
//     }
// 
//     return newString;
// }

void ReadDetectorID ( std::string DetectorID, bool* isUpstream, unsigned short* numsector, bool* isBarrel, int* layernum, bool* side, bool* isSX3 )
{
    std::string str ( DetectorID );
    std::string IDtype;
    std::stringstream ss ( str );

    std::vector <std::string> ID;

    while ( ss >> IDtype )
        ID.push_back ( IDtype );

    std::string UorD = ID[1].substr ( 0,1 );
    std::string sector = ID[1].substr ( 1 );
    std::string defaultTreeName1;
    std::string det = ID[0].substr ( 0 );
    std::cout << "Entered:" << std::endl;
    for ( unsigned int l=0; l<ID.size(); l++ )
    {
        std::cout << ' ' << ID.at ( l );
        std::cout << '\n';
    }

    if ( ID.size() == 2 )

    {
        if ( det == "BB10" )
        {

            *side = true;
            *isBarrel=true;
            *layernum=1;
            *numsector = stoi ( sector );
            *isUpstream = false;
            *isSX3=false;

        }

        else if ( det == "SX3" )
        {
            *side = true;
            *numsector = stoi ( sector );
            *layernum=2;
            *isBarrel=true;
            *isSX3=true;
            if ( UorD == "U" )
            {
                *isUpstream = true;
                std::cout << "Upstream ";
            }
            else
            {
                *isUpstream = false;
                std::cout << "Downstream ";
            }
        }

        else
        {
            return;
        }
    }

    else if ( ID.size() == 4 )
    {
        std::cout << "Entered:" << std::endl;
        for ( unsigned int l=0; l<ID.size(); l++ )
        {
            std::cout << ' ' << ID.at ( l );
            std::cout << '\n';
        }
//         int ulength = ID[1].length();
        std::string upstream = ID[1];

        std::string detlayer = ID[2].substr ( 0 );
        std::string detside = ID[3].substr ( 0 );

        *isUpstream = ( UorD == "U" ? true : false );

        if ( sector == "A" ) *numsector = 0;
        else if ( sector == "B" ) *numsector = 1;
        else if ( sector == "C" ) *numsector = 2;
        else if ( sector == "D" ) *numsector = 3;
        else *numsector = stoi ( sector );

        *isSX3 = false;
        *isBarrel = true;

        if ( det == "SX3" || det == "sx3" || det == "superX3" || det == "SuperX3" || det == "SUPERX3" )  *isSX3 = true;
        else if ( det == "BB10" || det == "bb10" ) *isSX3 = false;
        else if ( det == "QQQ5" || det == "qqq5" ) *isBarrel = false;
        else
        {
            std::cout<<"Unknown detector named"<<std::endl;
            return;
        }

        if ( ID[2] == "dE" ) *layernum = 1;
        else if ( ID[2] == "dE" ) *layernum = 2;
        else if ( ID[2] == "dE" ) *layernum = 3;

        if ( detside == "p" ) *side = true;
        else if ( detside == "n" ) *side = false;
    }

    else
    {
        std::cout << "Incorrect Input for:" << DetectorID << std::endl;
    }
}

string GetDetectorID ( bool isUpstream, bool isBarrel, unsigned int sector )
{
    string detID = Form ( "%s %s%d", ( isBarrel ? "SuperX3" : "QQQ5" ), ( isUpstream ? "U" : "D" ), sector );

    return detID;
}

string GetNameCompliantStr ( int input )
{
    char* output = new char[128];

    if ( input >= 0 ) sprintf ( output, "%d", input );
    else sprintf ( output, "m%d", -input );

    return ( string ) output;
}

// string ReplaceSpecialVariables ( string varStr )
// {
//     map<string, double> specialVars;
//
//     specialVars["Pi"] = TMath::Pi();
//
//     for ( auto sVarsItr = specialVars.begin(); sVarsItr != specialVars.end(); sVarsItr++ )
//     {
//         varStr = FindAndReplaceInString ( varStr, sVarsItr->first, Form ( "%3.10f", sVarsItr->second ) );
//     }
//
//     return varStr;
// }
