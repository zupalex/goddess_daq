#include "GoddessToolbox.h"

//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//

string GetLocalTimeAndDate()
{
    time_t t = time ( nullptr );
    struct tm* localt = localtime ( &t );

    char* timeAndDate = new char[256];

    sprintf ( timeAndDate, "%s", asctime ( localt ) );

    return ( string ) timeAndDate;
}

string GetCurrentYear()
{
    string timeAndDate = GetLocalTimeAndDate();

    size_t lastSpace = timeAndDate.find_last_of ( " " );

    string currYear = timeAndDate.substr ( lastSpace+1 );

    return FindAndReplaceInString ( currYear, "\n", "" );
}

string GetCurrentDayName()
{
    string timeAndDate = GetLocalTimeAndDate();

    size_t firstSpace = timeAndDate.find_first_of ( " " );

    string currDay = timeAndDate.substr ( 0, firstSpace );

    return currDay;
}

string GetCurrentMonthName()
{
    string timeAndDate = GetLocalTimeAndDate();

    size_t firstSpace = timeAndDate.find_first_of ( " " );
    size_t secSpace = timeAndDate.find_first_of ( " ", firstSpace+1 );

    string currMonth = timeAndDate.substr ( firstSpace+1, secSpace-firstSpace-1 );

    return currMonth;
}

int GetCurrentDayNum()
{
    string timeAndDate = GetLocalTimeAndDate();

    size_t firstNum = timeAndDate.find_first_of ( "0123456789" );
    size_t nextSpace = timeAndDate.find_first_of ( " ", firstNum+1 );

    string currDayNum = timeAndDate.substr ( firstNum, firstNum-nextSpace );

    return std::stoi ( currDayNum );
}

int GetCurrentMonthNum()
{
    string currMonthName = GetCurrentMonthName();

    if ( currMonthName == "Jan" ) return 1;
    else if ( currMonthName == "Feb" ) return 2;
    else if ( currMonthName == "Mar" ) return 3;
    else if ( currMonthName == "Apr" ) return 4;
    else if ( currMonthName == "May" ) return 5;
    else if ( currMonthName == "Jun" ) return 6;
    else if ( currMonthName == "Jul" ) return 7;
    else if ( currMonthName == "Aug" ) return 8;
    else if ( currMonthName == "Sep" ) return 9;
    else if ( currMonthName == "Oct" ) return 10;
    else if ( currMonthName == "Nov" ) return 11;
    else if ( currMonthName == "Dec" ) return 12;
    else return 0;
}

int GetCurrentHour()
{
    string timeAndDate = GetLocalTimeAndDate();

    size_t firstNum = timeAndDate.find_first_of ( "0123456789" );
    size_t nextSpace = timeAndDate.find_first_of ( " ", firstNum+1 );
    size_t firstSemicolon = timeAndDate.find_first_of ( ":", firstNum+1 );

    string currHour = timeAndDate.substr ( nextSpace+1, firstSemicolon-nextSpace-1 );

    return std::stoi ( currHour );
}

int GetCurrentMinute()
{
    string timeAndDate = GetLocalTimeAndDate();

    size_t firstSemicolon = timeAndDate.find_first_of ( ":" );
    size_t secSemicolon = timeAndDate.find_first_of ( ":", firstSemicolon+1 );

    string currMinute = timeAndDate.substr ( firstSemicolon+1, secSemicolon-firstSemicolon-1 );

    return std::stoi ( currMinute );
}

int GetCurrentSecond()
{
    string timeAndDate = GetLocalTimeAndDate();

    size_t firstSemicolon = timeAndDate.find_first_of ( ":" );
    size_t secSemicolon = timeAndDate.find_first_of ( ":", firstSemicolon+1 );
    size_t nextSpace = timeAndDate.find_first_of ( " ", secSemicolon+1 );

    string currMinute = timeAndDate.substr ( secSemicolon+1, nextSpace-secSemicolon-1 );

    return std::stoi ( currMinute );
}

//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//

bool CompareStrings ( string ref_, string search_, bool exactMatch, bool caseSensitive )
{
    if ( std::search ( ref_.begin(), ref_.end(), search_.begin(), search_.end(), ignoreCharCasePred ( caseSensitive ) ) != ref_.end() )
    {
        if ( exactMatch && ref_.length() != search_.length() ) return false;
        else return true;
    }

    return false;
}

std::vector<int> DecodeNumberString ( std::string itemsString, bool verbose )
{
    std::vector<int> sectorsNum;

    sectorsNum.clear();

    while ( itemsString.length() > 0 )
    {
        size_t foundComa = itemsString.find ( "," );
        size_t foundDash = itemsString.find ( "-" );

        std::string sub1 = "";
        std::string sub2 = "";

        if ( foundComa != std::string::npos && foundComa < foundDash )
        {
            sub1 = itemsString.substr ( 0, foundComa );
        }
        else if ( foundDash != std::string::npos )
        {
            sub1 = itemsString.substr ( 0, foundDash );
            sub2 = itemsString.substr ( foundDash+1, foundComa-foundDash-1 );
        }
        else
            sub1 = itemsString;

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
//             std::cout << "itemsString was " << itemsString;
            itemsString.replace ( 0, sub2.length() > 0 ? foundDash+1 : foundComa+1, "" );
//             std::cout << " ... is now " << itemsString << "\n";
        }
        else
        {
            itemsString.clear();
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

vector< string > GetDirContent ( string dirName, string mode, string fileExt, string mustHaveAll, string cantHaveAny, string mustHaveOneOf, string startWith, bool caseSensitive )
{
    std::vector<std::string> fileList, mustAllTags, cantTags, mustOneOfTags;

    bool mustTagsOrdered = false;

    if ( mode.find ( "\\ordered" ) != string::npos )
    {
        mustTagsOrdered = true;

//         cout << "Requested to find tags in order...\n";

        mode = FindAndReplaceInString ( mode, "\\ordered", "" );
    }

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

    if ( startWith == "*" ) startWith.clear();

    DIR* toScan = nullptr;
    dirent* dirEntry = nullptr;

    int scanMode = 0;

    if ( mode.find ( "system" ) == 0 )
    {
        if ( dirName.empty() ) dirName = "./";
        toScan = opendir ( dirName.c_str() );
        dirEntry = new dirent;

        scanMode = 1;
    }

    TIterator* rootListItr = nullptr;
    TObject* rootListEntry = nullptr;

    std::vector<string> acceptedClasses;
    acceptedClasses.clear();

    if ( mode.find ( "root" ) == 0 )
    {
        if ( mode.length() > 4 )
        {
            std::size_t classesSep = mode.find ( " " );

            while ( classesSep != std::string::npos )
            {
                std::size_t classStartPos = mode.find_first_not_of ( " \b\r\n\v", classesSep );

                if ( classStartPos == string::npos ) break;

                std::size_t nextClassesSep = mode.find ( " ", classStartPos );

                if ( nextClassesSep != std::string::npos ) acceptedClasses.push_back ( mode.substr ( classStartPos, nextClassesSep-classStartPos ) );
                else acceptedClasses.push_back ( mode.substr ( classStartPos ) );

                classesSep = nextClassesSep;
            }

            if ( acceptedClasses.size() > 0 ) cout << "Class filters: ";
            for ( unsigned int cFilt = 0; cFilt < acceptedClasses.size(); cFilt++ )
            {
                cout << "\"" << acceptedClasses[cFilt] << "\" ";
            }
            cout << endl;
        }

        if ( !dirName.empty() ) gDirectory->cd ( dirName.c_str() );

        TList* gDirList = gDirectory->GetList();
        TList* gDirKeys = gDirectory->GetListOfKeys();

        TList* gDirAllObj = new TList();
        gDirAllObj->Clear();

        if ( gDirList != nullptr && gDirList->GetSize() > 0 ) gDirAllObj->AddAll ( gDirList );
        if ( gDirKeys != nullptr && gDirKeys->GetSize() > 0 ) gDirAllObj->AddAll ( gDirKeys );

        if ( gDirAllObj != nullptr && gDirAllObj->GetSize() > 0 )
        {
            rootListItr = gDirAllObj->MakeIterator();
            rootListEntry = rootListItr->Next();
        }

        scanMode = 2;
    }

    while ( ( scanMode == 1 && ( dirEntry = readdir ( toScan ) ) != NULL ) || ( scanMode == 2 && rootListItr != nullptr && rootListEntry != nullptr ) )
    {
        std::string entName;

        if ( scanMode == 1 ) entName = dirEntry->d_name;
        else if ( scanMode == 2 )
        {
            entName = rootListEntry->GetName();

            bool skipEntry = false;
            if ( acceptedClasses.size() > 0 )
            {
                string entryClassName = ( string ) rootListEntry->ClassName();

                if ( entryClassName == "TKey" ) entryClassName = gDirectory->GetKey ( entName.c_str() )->GetClassName();

//                 cout << "Encounter object of type " << entryClassName << endl;

                skipEntry = std::find ( acceptedClasses.begin(), acceptedClasses.end(), entryClassName ) == acceptedClasses.end();
            }

            rootListEntry = rootListItr->Next();

            if ( skipEntry ) continue;
        }
        else
        {
            cerr << "Invalid mode specified...\n";
            fileList.clear();
            return fileList;
        }

        if ( entName.empty() ) continue;

        if ( startWith.empty() || ( !startWith.empty() && std::search ( entName.begin(), entName.end(), startWith.begin(), startWith.end(), ignoreCharCasePred ( caseSensitive ) ) == entName.begin() ) )
        {
            if ( fileExt.empty() || ( !fileExt.empty() && std::search ( entName.begin(), entName.end(), fileExt.begin(), fileExt.end(), ignoreCharCasePred ( caseSensitive ) ) != entName.end() ) )
            {
                bool mustAllFlag = true, cantFlag = false, mustOneOfFlag = ( mustOneOfTags.size() > 0 ? false : true );

                if ( mustAllTags.size() > 0 )
                {
                    string::iterator prevTagPos = entName.begin();

                    for ( unsigned int i = 0; i < mustAllTags.size(); i++ )
                    {
                        string::iterator startSearch = entName.begin();

                        if ( mustTagsOrdered ) startSearch = prevTagPos;

                        string::iterator currTagPos = std::search ( startSearch, entName.end(), mustAllTags[i].begin(), mustAllTags[i].end(), ignoreCharCasePred ( caseSensitive ) );

                        if ( currTagPos == entName.end() )
                        {
                            mustAllFlag = false;
                            continue;
                        }

                        prevTagPos = currTagPos;
                        prevTagPos++;
                    }
                }

                if ( cantTags.size() > 0 )
                {
                    for ( unsigned int i = 0; i < cantTags.size(); i++ )
                    {
                        if ( std::search ( entName.begin(), entName.end(), cantTags[i].begin(),  cantTags[i].end(), ignoreCharCasePred ( caseSensitive ) ) != entName.end() )
                        {
                            cantFlag = true;
                            continue;
                        }
                    }
                }

                if ( mustOneOfTags.size() > 0 )
                {
                    for ( unsigned int i = 0; i < mustOneOfTags.size(); i++ )
                    {
                        if ( std::search ( entName.begin(), entName.end(), mustOneOfTags[i].begin(),  mustOneOfTags[i].end(), ignoreCharCasePred ( caseSensitive ) ) != entName.end() ) mustOneOfFlag = true;
                    }
                }

                if ( mustAllFlag && !cantFlag && mustOneOfFlag ) fileList.push_back ( entName );
            }
        }
    }

    return fileList;
}

std::vector<std::string> DecodeItemsToTreat ( std::string itemsString, string mode, bool caseSensitive )
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
    std::size_t special = itemsString.find_first_of ( "*[]" );

    std::vector<std::string> fNameParts;
    std::vector<std::tuple<std::size_t, char, std::size_t>> sChars;

    std::string fStartWith;
    std::string fMustHaveAll;
    std::string fMustHaveOneOf;

    if ( special == std::string::npos )
    {
        files = GetDirContent ( path, mode, "", itemsString, "", "", "", caseSensitive );
    }
    else
    {
        if ( startPos == 0 ) fStartWith = itemsString.substr ( 0, special );

        if ( special < startPos )
        {
            sChars.push_back ( std::make_tuple ( special, itemsString[special], special ) );
            special = itemsString.find_first_of ( "*[]", startPos );
        }

        while ( special != std::string::npos )
        {
            fNameParts.push_back ( itemsString.substr ( startPos, special - startPos ) );

            //             std::cout << "new name part: " << itemsString.substr ( startPos, special - startPos ) << "\n";

            sChars.push_back ( std::make_tuple ( special, itemsString[special], startPos ) );

            startPos = itemsString.find_first_not_of ( "*[]", special );

            //             std::cout << "separator: " << itemsString[special] << " @ " << special << "\n";

            special = itemsString.find_first_of ( "*[]", startPos );
        }

        for ( unsigned int i = 0; i < sChars.size(); i++ )
        {
            if ( std::get<1> ( sChars[i] ) == '*' )
            {
                if ( std::get<2> ( sChars[i] ) == std::get<0> ( sChars[i] ) ) continue;

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

        if ( startPos != std::string::npos )
        {
            fNameParts.push_back ( itemsString.substr ( startPos, itemsString.length() ) );

            fMustHaveAll += itemsString.substr ( startPos, itemsString.length() );
            fMustHaveAll += " ";

            //             std::cout << "new must all tag: " << itemsString.substr ( startPos, itemsString.length() ) << "\n";

            //             std::cout << "new name part: " << itemsString.substr ( startPos, special - startPos ) << "\n";
        }

        files = GetDirContent ( path, mode, "", fMustHaveAll, "", fMustHaveOneOf, fStartWith, caseSensitive );
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

std::function<bool ( double,double ) > CheckValProxFunc ( double compVal )
{
    return [compVal] ( double a, double b )
    {
        bool res =  fabs ( a - b ) <= compVal ;
        return res;
    };
}

int RoundValue ( double val )
{
    int floorVal = ( int ) val;

    double decimals = ( val - floorVal ) * 10;
    int firstDecimal = ( int ) decimals;

    if ( firstDecimal >= 5 ) return ceil ( val );
    else return floor ( val );
}

std::pair< vector< double >, vector< double > > FillGraphFromFile ( string input )
{
    vector<double> xs, ys;
    xs.clear();
    ys.clear();

    std::ifstream inFile ( input.c_str(), std::ios_base::in );

    if ( !inFile.is_open() )
    {
        std::cerr << "Failed to open file " << input << std::endl;
        return std::make_pair ( xs, ys );
    }

    string readLine;

    while ( std::getline ( inFile, readLine ) )
    {
        std::istringstream iss;
        iss.str ( readLine );

        double x_, y_;

        iss >> x_ >> y_;

        xs.push_back ( x_ );
        ys.push_back ( y_ );
    }

    inFile.close();

    return std::make_pair ( xs, ys );
}

double EvalGraph ( vector<double> x_, vector<double> y_, double toEval )
{
    if ( x_.size() == 0 || y_.size() == 0 ) return 0.;

    auto lowItr = std::find ( x_.begin(), x_.end(), toEval );

    auto lstItr = x_.end();
    lstItr--;

    int low, up;

    low = std::distance ( x_.begin(), lowItr );

    if ( lowItr != x_.end() ) return y_[low];
    else
    {
        lowItr = std::lower_bound ( x_.begin(), x_.end(), toEval );

        if ( lowItr == x_.end() ) lowItr--;

        if ( lowItr != x_.begin() ) lowItr--;

        low = std::distance ( x_.begin(), lowItr );

        auto upItr = lowItr;
        upItr++;

        up = std::distance ( x_.begin(), upItr );

        return ( y_[up] + ( toEval - x_[up] ) * ( y_[low] - y_[up] ) / ( x_[low] - x_[up] ) );
    }

    return 0.;
}

double IntegrateGraph ( vector< double > x_, vector< double > y_, double xMin_, double xMax_, double dx_ )
{
    if ( xMin_ >= xMax_ ) return 0.0;

    double yMin = EvalGraph ( x_, y_, xMin_ );
    double yMax = EvalGraph ( x_, y_, xMax_ );

    double prevY = yMin;
    double nextX = xMin_ + dx_;

    double totIntegral = 0.0;

    while ( nextX < xMax_ )
    {
        double nextY = EvalGraph ( x_, y_, nextX );

        totIntegral += nextY * dx_ + ( prevY - nextY ) * dx_ / 2.;

        nextX += dx_;
        prevY = nextY;
    }

    totIntegral += yMax * ( xMax_-nextX ) + ( prevY - yMax ) * ( xMax_-nextX ) / 2.;

    return totIntegral;
}

double GetEffectiveThickness ( double angle, double targetThickness_ )
{
    return fabs ( targetThickness_ / ( 2 * TMath::Cos ( angle ) ) );
}

double ComputeEnergyLoss ( vector<double> energies_, vector<double> rangeOrStoppingPower_, double startingEnergy, float mass, double xMin_, double xMax_, double dx_, string mode )
{
    if ( xMin_ >= xMax_ ) return 0.0;

    if ( mode == "Integral" )
    {
        double nextX = xMin_;

        double remainingEnergy = startingEnergy;

        while ( nextX < xMax_ )
        {
            remainingEnergy -= EvalGraph ( energies_, rangeOrStoppingPower_, remainingEnergy ) * dx_ / mass;

            nextX += dx_;
        }

        remainingEnergy -= EvalGraph ( energies_, rangeOrStoppingPower_, remainingEnergy ) * ( xMax_-nextX ) / mass;

        return ( startingEnergy - remainingEnergy ) * mass;
    }
    else if ( mode == "Interpolation" )
    {
        double rangeMax = EvalGraph ( energies_, rangeOrStoppingPower_, startingEnergy );

        double remainingEnergy = EvalGraph ( rangeOrStoppingPower_, energies_, rangeMax - ( xMax_-xMin_ ) );

        return ( startingEnergy - remainingEnergy ) * mass;
    }
    else return 0.0;
}

double TryGetRemainingEnergy ( string mass_db, int mass, int charge, double startingEnergy, double thickness_, double dx_, string targetStr, double density, string tablePath, string mode )
{
    std::ifstream mass_input ( mass_db.c_str(), std::ios_base::in );

    if ( !mass_input.is_open() )
    {
        std::cerr << "Failed to open the mass database. Energy loss has not been computed...\n";

        return startingEnergy;
    }

    string elementStr = "";

    GetAtomicFormula ( mass_input, mass, charge, elementStr );

    char* tryFindStr = new char[512];

    char* densityStr = new char[128];

    if ( density == 0 ) sprintf ( densityStr, "*" );
    else sprintf ( densityStr, "%d", ( int ) density );

    sprintf ( tryFindStr, "%s/%s_in_%s_%smg_cm3*", tablePath.c_str(), elementStr.c_str(), targetStr.c_str(), densityStr );

    vector<string> tryFindTable = DecodeItemsToTreat ( ( string ) tryFindStr, "system", false );

    double remainingEnergy = startingEnergy;

    if ( tryFindTable.size() == 1 )
    {
        auto beamTable = FillGraphFromFile ( tablePath + "/" + tryFindTable[0] );

        double eLoss = ComputeEnergyLoss ( beamTable.first, beamTable.second, startingEnergy/mass, mass, 0, thickness_, dx_, mode );

//         std::cout << "Computed energy loss using table " << tryFindTable[0] << " : " << eLoss << " MeV" << std::endl;

        remainingEnergy -= eLoss;
    }

    return remainingEnergy; // in MeV
}

void ListHistograms ( string match, unsigned int limit, unsigned int startAt, bool caseSensitive )
{
    std::vector<string> matchingHists;
    matchingHists.clear();

    if ( match.empty() ) match = "*";

    matchingHists = DecodeItemsToTreat ( match, "root \\ordered TH1F TH2F TH1D TH2D", caseSensitive );

    if ( limit == 0 ) limit = 20;

    unsigned int histAmount = gDirectory->GetList()->GetSize();

    unsigned int digitsNumber = 1;

    string userIn;

    while ( ( histAmount / pow ( 10,digitsNumber ) ) >= 1 ) digitsNumber++;

    if ( startAt >= matchingHists.size() )
    {
        return;
    }

    cout << matchingHists.size() << " matching histograms...\n\n";

DisplayResults:

    for ( unsigned int i = startAt; i < std::min ( ( unsigned int ) matchingHists.size(), startAt+limit ); i++ )
    {
        cout << "-> Histogram #" << std::setw ( digitsNumber ) << i << " : " << matchingHists[i] << endl;
    }

    startAt += limit;

    if ( startAt >= matchingHists.size() )
    {
        return;
    }

UserPrompt:

    cout << "Continue = Enter / Quit = q : ";

    std::getline ( std::cin, userIn );

    if ( userIn == "q" || userIn == "quit" ) return;
    else if ( userIn.empty() ) goto DisplayResults;
    else
    {
        cerr << "Invalid input...\n";
        goto UserPrompt;
    }

    return;
}

//____________________________________________________________________________________________//
//____________________________________________________________________________________________//

int InitReadMassesForKinematic ( std::ifstream& mass_db )
{
    if ( !mass_db.is_open() )
    {
        std::cerr << "No File Found for the Masses Database!\n";
        return -1;
    }

    std::stringstream readMassDB;
    string readLine;

    int massTableLineBeg = -1;

    while ( std::getline ( mass_db, readLine ) )
    {
        if ( readLine.length() == 0 ) continue;

        readMassDB.clear();
        readMassDB.str ( readLine );

        if ( readLine.find ( "MASS" ) != string::npos &&  readLine.find ( "LIST" ) != string::npos )
        {
            massTableLineBeg += 5;
            break;
        }
    }

    return massTableLineBeg;
}

void GetRelevantInfoPositions ( string* readWord, short& posMassExcess, short& posBindingEnergy, short& posBetaDecay, short& posAMU, short& posElement )
{
    std::stringstream checkNumConversion;
    float dummyf;

    checkNumConversion.str ( readWord[posElement] );
    checkNumConversion >> dummyf;

    if ( !checkNumConversion.fail() ) // Our element is a number and not a string!
    {
//         std::cout << Form ( "Did not fail to parse Word#%d as a float ** ", posElement );
        posMassExcess++;
        posElement++;
    }

    checkNumConversion.clear();
    checkNumConversion.str ( readWord[posMassExcess] );
    checkNumConversion >> dummyf;

    if ( checkNumConversion.fail() ) // Our Mass Excess number is not a number!
    {
//         std::cout << Form ( "Failed to parse Word#%d as an float ** ", posMassExcess );
        posMassExcess++;
    }

    posBindingEnergy = posMassExcess+2;

    posBetaDecay = posBindingEnergy+3;

    if ( readWord[posMassExcess][readWord[posMassExcess].length()-1] == '#' ) posMassExcess = -1;

    if ( readWord[posBindingEnergy][readWord[posBindingEnergy].length()-1] == '#' ) posBindingEnergy = -1;

    if ( readWord[posBetaDecay] == "*" )
    {
        posAMU = posBetaDecay+1;
        posBetaDecay = -1;
    }
    else
    {
        posAMU = posBetaDecay+2;
    }

    if ( readWord[posAMU+1][readWord[posAMU+1].length()-1] == '#' ) posAMU = -1;

    return;
}

void GetAtomicFormula ( std::ifstream& mass_db, int mass, int charge, string& toReconstruct )
{
    mass_db.clear();
    mass_db.seekg ( 0 );

    string element = "";

    if ( mass == 1 && charge == 1 )
    {
        toReconstruct = "p";
    }
    else if ( mass == 2 && charge == 1 )
    {
        toReconstruct = "d";
    }
    else if ( mass == 1 && charge == 0 )
    {
        toReconstruct = "n";
    }
    else if ( mass == 3 && charge == 1 )
    {
        toReconstruct = "t";
    }
    else
    {
        std::stringstream readMassDB;
        string readLine;

        int massTableLineBeg = InitReadMassesForKinematic ( mass_db );

        for ( int i = 0; i < massTableLineBeg; i++ )
        {
            std::getline ( mass_db, readLine );
        }

        while ( std::getline ( mass_db, readLine ) )
        {
            readMassDB.clear();
            readMassDB.str ( readLine );

            short posMassExcess = 5, posBindingEnergy = -1, posBetaDecay = -1, posAMU = -1, posElement = 4;

            string readWord[17];

//         std::cout << "Read Line:\n";

            for ( int i = 0; i < 17; i++ )
            {
                readMassDB >> readWord[i];
//             std::cout << readWord[i] << "  ";
            }

//         std::cout << "\n";

            GetRelevantInfoPositions ( readWord, posMassExcess, posBindingEnergy, posBetaDecay, posAMU, posElement );

            if ( charge == std::stoi ( readWord[posElement-2] ) )
            {
                char* massChar = new char[4];
                sprintf ( massChar, "%i", mass );

                toReconstruct = massChar;
                toReconstruct += readWord[posElement];

                break;
            }
        }

        return;
    }
}

bool CharIsDigit ( char toCheck )
{
    if ( toCheck == '0' || toCheck == '1' || toCheck == '2' || toCheck == '3' || toCheck == '4' || toCheck == '5' || toCheck == '6' || toCheck == '7' || toCheck == '8' || toCheck == '9' )
        return true;
    else return false;
}

void DecodeAtomicFormula ( std::ifstream& mass_db, string toDecode, int& mass, int& charge, float& atomicMass )
{
    mass_db.clear();
    mass_db.seekg ( 0 );

    std::vector<int> massDigits;

    string element = "";
    mass = 0;
    charge = -1;

    massDigits.clear();

    if ( toDecode == "p" )
    {
        mass = 1;
        element = "H";
    }
    else if ( toDecode == "d" )
    {
        mass = 2;
        element = "H";
    }
    else if ( toDecode == "n" )
    {
        mass = 1;
        element = "n";
    }
    else if ( toDecode == "t" )
    {
        mass = 3;
        element = "H";
    }
    else
    {
        for ( unsigned int i = 0; i < toDecode.length(); i++ )
        {
            if ( CharIsDigit ( toDecode[i] ) )
            {
                string digit;
                digit = toDecode[i];

                massDigits.push_back ( std::stoi ( digit ) );
            }
            else
            {
                element += toDecode[i];
            }
        }

        for ( unsigned short i = 0; i < massDigits.size(); i++ )
        {
//         std::cout << "Retreived mass digit: " << massDigits[i] << "\n";
            mass += massDigits[i] * pow ( 10, massDigits.size() - 1 - i );
        }
    }

//     std::cout << "Retreiving the charge of element: " << element << " ...\n";
//     std::cout << "Decoded Mass: " << mass << "\n";

    std::stringstream readMassDB;
    string readLine;

    int massTableLineBeg = InitReadMassesForKinematic ( mass_db );

    for ( int i = 0; i < massTableLineBeg; i++ )
    {
        std::getline ( mass_db, readLine );
    }

    short posMassExcess = 5, posBindingEnergy = -1, posBetaDecay = -1, posAMU = -1, posElement = 4;

    while ( std::getline ( mass_db, readLine ) )
    {
        readMassDB.clear();
        readMassDB.str ( readLine );

        string readWord[17];

//         std::cout << "Read Line:\n";

        for ( int i = 0; i < 17; i++ )
        {
            readMassDB >> readWord[i];
//             std::cout << readWord[i] << "  ";
        }

//         std::cout << "\n";

        posMassExcess = 5;
        posBindingEnergy = -1;
        posBetaDecay = -1;
        posAMU = -1;
        posElement = 4;

        GetRelevantInfoPositions ( readWord, posMassExcess, posBindingEnergy, posBetaDecay, posAMU, posElement );

        charge = CheckForMatch<string> ( readWord, posElement, mass, element );

        if ( charge >= 0 ) break;
    }

    readMassDB.clear();
    readMassDB.str ( readLine );

    string mainAMStr;
    string secondaryAMStr;

    for ( short i = 0; i < posAMU+1; i++ ) readMassDB >> mainAMStr;
    readMassDB >> secondaryAMStr;

    string atomicMassStr = mainAMStr + secondaryAMStr;

//     std::size_t fstSpacePos = readLine.find_first_of ( " ", posAMU );;
//     std::size_t amEndPos = readLine.find_first_of ( " \n", fstSpacePos+1 );
//     atomicMassStr = readLine.substr ( posAMU, amEndPos-posAMU );
//
//     atomicMassStr = FindAndReplaceInString ( atomicMassStr, " ", "" );

//     std::cout << "Atomic Mass String: " << atomicMassStr << std::endl;

    atomicMass = std::stof ( atomicMassStr );
    atomicMass *= 1e-6;

    return;
}
