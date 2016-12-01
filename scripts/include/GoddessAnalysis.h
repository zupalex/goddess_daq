#ifndef __GODDESSANALYSIS__
#define __GODDESSANALYSIS__

#include "goddess_analysis_macros.h"

vector<unsigned short> DecodeSectorsString ( string sectorsString, bool verbose = false );

vector<string> DecodeTags ( string tagsStr );

vector<string> GetDirContent ( string dirName = "./", string fileExt = "", string mustHaveAll = "", string cantHaveAny = "", string mustHaveOneOf = "", string startWith = "" );

vector<string> DecodeFilesToTreat ( string filesStr );

vector<string> SplitString ( string toSplit, string splitter );

string FindAndReplaceInString ( string input, string toReplace, string substitute, unsigned int nTimes = 0, int startIndex = 0 );

class GoddessAnalysis : public TObject, public TQObject
{
private:
    string defaultTreeName1;
    string defaultTreeName2;
    string defaultFileName;

public:
    GoddessAnalysis();
    virtual ~GoddessAnalysis();

    // ---------------- General useful functions -------------------------- //

    template<typename T> static void DisplayMapKeys ( std::map<string, T> map_ );
    inline static void DisplayMapKeys ( std::map<string, float> map_ );

    inline static int DecodeStripNumber ( int st_ );

    template<typename T> static void GetListOfSectorsToTreat ( vector<unsigned short>* sectorsList, T sector );

    template<typename First, typename... Rest> static void GetListOfSectorsToTreat ( vector<unsigned short>* sectorsList, First fstSector, Rest... otherSectors );

    template<typename First, typename... Rest> TH2F* DrawEnergyVsAngleSX3 ( TChain* chain, int nentries,
            string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, bool isUpstream_, string strips,
            First fstSector, Rest... otherSectors );

    TH2F* DrawEnergyVsAngleSX3 ( TChain* chain, int nentries,
                                 string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, bool isUpstream_, string strips,
                                 unsigned short sector );

    vector<unsigned short> GetStripsListToTreat ( string strips );

    // ------------------ For the check mapping tool ------------------------ //

    GoddessAnalysis ( string filename );
    GoddessAnalysis ( string filename, string treename, string treename2 );

    void SetDefaultFileAndTrees ( string fileName, string treeName1, string treeName2 );

    void CheckMapping ( string filename, string treename, string treename2, unsigned short channel1, unsigned short channel2, bool Digital, string DetectorID );

    void CheckMapping ( unsigned short channel1, unsigned short channel2, bool Digital, string DetectorID );

    // ****************** Reaction parameters *************************** //

    float beamEk;
    float beamMass;
    float targetMass;
    float ejecMass;
    float recoilMass;
    float qvalGsGs;

    void SetReacParameters ( float beamMass_ = 134., float beamEk_ = 1338., float targetMass_ = 2., float ejecMass_ = 1., float recoilMass_ = 135., float qValGsGs_ = 4.1 );
    void SetBeamParameters ( float beamMass_ = 134., float beamEk_ = 1338. );
    void SetBeamEk ( float beamEk_ = 1338. );
    void SetBeamMass ( float beamMass_ = 134. );
    void SetTargetMass ( float targetMass_ = 2. );
    void SetEjectileMass ( float ejecMass_ = 1. );
    void SetRecoilMass ( float recoilMass_ = 135. );
    void SetQvalueGsToGs ( float qvalGsGs_ = 4.1 );

    // ------------------ For the User Macros ------------------------------- //

    TChain* userChain;
    TTree* userTree;

    void AddFileToTreat ( TFile* inFile, string treeName );
    void AddFileToTreat ( string inFile, string treeName );
    void AddFileToTreat ( vector<string> inFile, string treeName );

    template<typename T> void InitUserAnalysis ( string treeName, T inFile1 );
    template<typename First, typename... Rest> void InitUserAnalysis ( string treeName, First inFile1, Rest... inFileRest );

    ClassDef ( GoddessAnalysis, 1 )
};

template<typename T> void GoddessAnalysis::InitUserAnalysis ( string treeName, T inFile1 )
{
    AddFileToTreat ( ( string ) inFile1, treeName );
}

template<typename First, typename... Rest> void GoddessAnalysis::InitUserAnalysis ( string treeName, First inFile1, Rest... inFileRest )
{
    InitUserAnalysis<string> ( treeName, inFile1 );

    InitUserAnalysis ( treeName, inFileRest... );
}

template<typename T> void GoddessAnalysis::DisplayMapKeys ( std::map<string, T> map_ )
{
    for ( auto itr = map_.begin(); itr != map_.end(); itr++ )
    {
        cout << itr->first << endl;
    }
}

inline void GoddessAnalysis::DisplayMapKeys ( std::map<string, float> map_ )
{
    for ( auto itr = map_.begin(); itr != map_.end(); itr++ )
    {
        cout << itr->first << endl;
    }
}

inline int GoddessAnalysis::DecodeStripNumber ( int st_ )
{
    return st_%100;
}

template<typename T> void GoddessAnalysis::GetListOfSectorsToTreat ( vector<unsigned short>* sectorsList, T sector )
{
    cout << "adding " << sector << " to the list of sectors to treat..." << endl;

    sectorsList->push_back ( sector );
}

template<typename First, typename... Rest> void GoddessAnalysis::GetListOfSectorsToTreat ( vector<unsigned short>* sectorsList, First fstSector, Rest... otherSectors )
{
    GetListOfSectorsToTreat<unsigned short> ( sectorsList, fstSector );

    GetListOfSectorsToTreat ( sectorsList, otherSectors... );
}

template<typename First, typename... Rest> TH2F* GoddessAnalysis::DrawEnergyVsAngleSX3 ( TChain* chain, int nentries,
        string hname, int nbinsX, int binMinX, int binMaxX, int nbinsY, int binMinY, int binMaxY, string drawOpts, bool isUpstream_, string strips,
        First fstSector, Rest... otherSectors )
{
    vector<unsigned short> sectorsList;
    sectorsList.clear();

    GetListOfSectorsToTreat<First, Rest...> ( &sectorsList, fstSector, otherSectors... );

    vector<unsigned short> stripsList = GetStripsListToTreat ( strips );

    vector<SiDataBase>* siDataVect = new vector<SiDataBase>();
    siDataVect->clear();

    chain->SetBranchAddress ( "si", &siDataVect );

    if ( nentries > chain->GetEntries() || nentries == 0 ) nentries = chain->GetEntries();

    TH2F* hist = new TH2F ( hname.c_str(), hname.c_str(), nbinsX, binMinX, binMaxX, nbinsY, binMinY, binMaxY );

    cout << endl;

    for ( int i = 0; i < nentries; i++ )
    {
        chain->GetEntry ( i );

        if ( i%10000 == 0 ) cout << "Treated " << i << " / " << nentries << " entries ( " << ( ( float ) i ) / ( ( float ) nentries ) * 100. << " % )\r" << std::flush;

        if ( siDataVect->size() == 0 ) continue;

        for ( unsigned short j = 0; j < siDataVect->size(); j++ )
        {
            SiDataBase siData = siDataVect->at ( j );

            if ( siData.pos.size() == 0 ) continue;

            if ( siData.isUpstream == isUpstream_ )
            {
                int sect = -1;

                for ( unsigned short k = 0; k < sectorsList.size(); k++ )
                {
                    if ( siData.sector == sectorsList[k] ) sect = sectorsList[k];
                }

                if ( sect == -1 ) continue;

                int stPos = -1;

                for ( unsigned short st = 0; st < siData.stripMax.size(); st++ )
                {
                    for ( unsigned short k = 0; k < stripsList.size(); k++ )
                    {
                        if ( DecodeStripNumber ( siData.stripMax[st] ) == stripsList[k] ) stPos = st;
                    }
                }

                if ( stPos == -1 ) continue;

                if ( siData.pos[stPos].Mag() > 0.0 && siData.isBarrel )
                {
                    hist->Fill ( -siData.pos[stPos].Angle ( TVector3 ( 0, 0, 1 ) ) *180/TMath::Pi(), siData.eSum[stPos] );
                }
            }
        }
    }

    hist->Draw ( drawOpts.c_str() );

    cout << endl;

    return hist;
}

template<typename T1, typename T2> void AddLinkMapEntries ( map<string, T1*>* linkMap_, string str, T2& val )
{
    ( *linkMap_ ) [str] = ( T1* ) ( &val );

    return;
}

template<typename T1, typename T2, typename... Rest> void AddLinkMapEntries ( map<string, T1>* linkMap_, string varsStr, T2& var1, Rest&... varRest )
{
    std::size_t fstMemPosStart = varsStr.find_first_not_of ( " " );

    std::size_t fstMemPosEnd = varsStr.find_first_of ( " ", fstMemPosStart );

    string var1Str = varsStr.substr ( fstMemPosStart, fstMemPosEnd - fstMemPosStart );

    string newVarsStr = varsStr.substr ( fstMemPosEnd+1 );

    AddLinkMapEntries ( linkMap_, var1Str, var1 );

    AddLinkMapEntries ( linkMap_, newVarsStr, varRest... );

    return;
}

template<typename T, typename... Rest> map<string, T*> MakeLinkMap ( string varsStr, T& var1, Rest&... varRest )
{
    map<string, T*> linkMap;

    AddLinkMapEntries ( &linkMap, varsStr, var1, varRest... );

    return linkMap;
}

template<typename T> string SubstituteStrInFormula ( string formula, map<string, T*> linkMap )
{
    string modFormula = formula, buffFormula;

    for ( auto itr = linkMap.begin(); itr != linkMap.end(); itr++ )
    {
        string toSubstitute = itr->first;

        std::size_t  subPos = modFormula.find ( toSubstitute.c_str() );

        while ( subPos != string::npos )
        {
            buffFormula = modFormula;

            modFormula = buffFormula.substr ( 0, subPos );

            modFormula += Form ( "%Lf", ( long double ) ( * ( itr->second ) ) );

            modFormula += buffFormula.substr ( subPos+toSubstitute.length() );

            subPos = modFormula.find ( toSubstitute.c_str() );
        }
    }

    return modFormula;
}

template<typename T> T EvalSimpleString ( string toEval, T* result = nullptr )
{
    std::size_t invalidCharacter = toEval.find_first_not_of ( " +-*/0123456789." );

    if ( invalidCharacter != string::npos )
    {
        cerr << "Invalid formula: " << toEval << endl;

        return ( ( T ) 0 );
    }

    map<std::size_t, char> operators;
    operators.clear();

    map<std::size_t, T> members;
    members.clear();

    std::size_t newMemberStartPos;

    newMemberStartPos = toEval.find_first_of ( "0123456789" );

    std::size_t newOperatorPos = toEval.find_first_of ( "+-*/" );

    if ( newOperatorPos == string::npos && toEval.find_first_of ( "0123456789" ) != string::npos ) return ( ( T ) std::stod ( toEval ) );

    while ( newOperatorPos != string::npos )
    {
        operators[newOperatorPos] = toEval[newOperatorPos];

        newOperatorPos = toEval.find_first_of ( "+-*/", newOperatorPos+1 );
    }

//     cout << "found the following operators:\n";
//     for ( auto itr = operators.begin(); itr != operators.end(); itr++ )
//     {
//         cout << itr->second << " (@ pos " << itr->first << " )\n";
//     }

    auto opItr = operators.begin();


    while ( newMemberStartPos != string::npos )
    {
        if ( opItr != operators.end() )
        {
            members[newMemberStartPos] = ( T ) std::stod ( toEval.substr ( newMemberStartPos, opItr->first - newMemberStartPos ) );

            newMemberStartPos = toEval.find_first_of ( "0123456789", opItr->first );

            if ( operators.find ( newMemberStartPos ) != operators.end() ) newMemberStartPos = toEval.find_first_of ( "0123456789", opItr->first );
        }
        else
        {
            members[newMemberStartPos] = ( T ) std::stod ( toEval.substr ( newMemberStartPos ) );
            break;
        }

        opItr++;
    }


//     cout << "the number members are:\n";
//     for ( auto itr = members.begin(); itr != members.end(); itr++ )
//     {
//         cout << itr->second << " (@ pos " << itr->first << " )\n";
//     }

    // Checking if the first character of the string is an operator

    opItr = operators.begin();
    auto memItr = members.begin();

    if ( opItr->first < memItr->first )
    {
        if ( opItr->second == '+' )
        {
            operators.erase ( opItr );
            opItr = operators.begin();
        }
        else if ( opItr->second == '-' )
        {
            operators.erase ( opItr );
            opItr = operators.begin();
            memItr->second *= -1.;
        }
        else
        {
            cerr << "Invalid formula: " << toEval << endl;

            return ( ( T ) 0 );
        }
    }

    // Checking double operators (e.g. + -, -+, --, ...)

    opItr = operators.begin();

    while ( opItr != operators.end() )
    {
        if ( opItr != operators.begin() )
        {
            auto prevItr = opItr;
            prevItr--;

            std::size_t foundDigit = toEval.find_first_of ( "0123456789", prevItr->first );

            if ( foundDigit > opItr->first )
            {
//                 cout << "2 successive signs without digits detected: " << prevItr->first << " and " << opItr->first << "\n";

                if ( opItr->second == '+' )
                {
                    operators.erase ( opItr );
                    opItr = operators.begin();
                }
                else if ( opItr->second == '-' )
                {
                    memItr = members.upper_bound ( opItr->first );
                    memItr->second *= -1.;

                    operators.erase ( opItr );
                    opItr = operators.begin();
                }
                else
                {
                    cerr << "Invalid formula: " << toEval << endl;

                    return ( ( T ) 0 );
                }
            }
        }

        opItr++;
    }

//     memItr = members.begin();
//     opItr = operators.begin();
//     cout << "New formula after removing double signs: ";
//     while ( memItr != members.end() )
//     {
//         cout << "(" << memItr->second << ")" << ( opItr != operators.end() ? opItr->second : ' ' );
//         memItr++;
//         opItr++;
//     }
//     cout << "\n";

    // Transforming the "-" operators to "+ (-1) * value"

    opItr = operators.begin();

    memItr = members.begin();

    while ( memItr != members.end() )
    {
        opItr = operators.upper_bound ( memItr->first );
        memItr++;

        if ( opItr != operators.end() && opItr->second == '-' )
        {
            opItr->second = '+';

            memItr->second *= -1.;
        }
    }

//     memItr = members.begin();
//     opItr = operators.begin();
//     cout << "New formula after converting the minus signs: ";
//     while ( memItr != members.end() )
//     {
//         cout << "(" << memItr->second << ")" << ( opItr != operators.end() ? opItr->second : ' ' );
//         memItr++;
//         opItr++;
//     }
//     cout << "\n";

    if ( members.size() != operators.size() +1 )
    {
        cerr << "Invalid formula: " << toEval << endl;

        return ( ( T ) 0 );
    }

    while ( operators.size() > 0 )
    {
        opItr = operators.begin();
        memItr = members.begin();

//         cout << "Looking for a * or / operator...\n";

        while ( opItr != operators.end() )
        {
            if ( opItr->second == '*' )
            {
//                 cout << "Found * @ pos " << opItr->first << " ...\n";
                break;
            }
            else if ( opItr->second == '/' )
            {
//                 cout << "Found / @ pos " << opItr->first << " ...\n";
                break;
            }

            opItr++;
        }

        if ( opItr == operators.end() ) opItr--;

        double subRes;

        memItr = members.upper_bound ( opItr->first );
        double val2 = memItr->second;

        memItr--;
        double val1 = memItr->second;

//         cout << "Current operation to process is: " << val1 << opItr->second << val2 << "\n";
//         int opNumber = ( int ) std::distance ( operators.begin(), opItr );
//         cout << "Operator number: " << opNumber << " @ pos " << opItr->first << "\n";

        if ( opItr->second == '*' ) subRes = val1 * val2;
        else if ( opItr->second == '/' ) subRes = val1 / val2;
        else if ( opItr->second == '+' ) subRes = val1 + val2;
        else
        {
            cerr << "Invalid formula: " << toEval << endl;

            return ( ( T ) 0 );
        }

//         cout << "Result of the operation is: " << subRes << "\n";

        memItr->second = subRes;

        memItr++;
        members.erase ( memItr );

        operators.erase ( opItr );
    }

    memItr = members.begin();

    if ( result != nullptr ) *result = memItr->second;

    return memItr->second;
}

template<typename T> T EvalComplexString ( string toEval, T* result = nullptr )
{
    string toEvalCp = toEval;
    string newEvalStr;

    std::size_t openBracketPos, closeBracketPos;

    openBracketPos = toEvalCp.find_last_of ( "(" );
    closeBracketPos = toEvalCp.find_first_of ( ")", openBracketPos );

    //     if ( openBracketPos == string::npos && closeBracketPos == string::npos )
    //     {
    //         cout << "No brackets detected. Fall back to the simple evaluation...\n";
    //     }

    while ( openBracketPos != string::npos && closeBracketPos != string::npos )
    {
        bool doAbs = false;

        std::size_t absPos = toEval.find ( "abs" );

        while ( absPos != string::npos )
        {
            if ( absPos == openBracketPos - 3 || ( absPos == openBracketPos - 4 && toEval[openBracketPos-1] == ' ' ) )
            {
                doAbs = true;
                break;
            }

            absPos = toEval.find ( "abs", absPos+1 );
        }

        if ( absPos == string::npos )
        {
            absPos = toEval.find ( "Abs" );

            while ( absPos != string::npos )
            {
                if ( absPos == openBracketPos - 3 || ( absPos == openBracketPos - 4 && toEval[openBracketPos-1] == ' ' ) )
                {
                    doAbs = true;
                    break;
                }

                absPos = toEval.find ( "Abs", absPos+1 );
            }
        }

        if ( absPos == string::npos ) newEvalStr = toEvalCp.substr ( 0, openBracketPos );
        else newEvalStr = toEvalCp.substr ( 0, absPos );

        double eval;
        if ( doAbs ) eval = TMath::Abs ( EvalSimpleString<double> ( toEvalCp.substr ( openBracketPos+1, closeBracketPos - openBracketPos - 1 ) ) );
        else eval = EvalSimpleString<double> ( toEvalCp.substr ( openBracketPos+1, closeBracketPos - openBracketPos - 1 ) );
        newEvalStr += Form ( "%f", eval );

        newEvalStr += toEvalCp.substr ( closeBracketPos+1, toEvalCp.length()- closeBracketPos - 1 );

        toEvalCp = newEvalStr;

        //         cout << "Operation processed. New string value: " << newEvalStr << "\n";

        openBracketPos = toEvalCp.find_last_of ( "(" );
        closeBracketPos = toEvalCp.find_first_of ( ")", openBracketPos );
    }

    return EvalSimpleString<T> ( toEvalCp, result );
}

template<typename T1, typename T2> T2 EvalString ( string toEval, map<string, T1*>* linkMap_ = nullptr, T2* result = nullptr )
{
    string toEvalSub = toEval;

    if ( linkMap_ != nullptr ) toEvalSub = SubstituteStrInFormula<T1> ( toEval, *linkMap_ );

    std::size_t foundBracket = toEvalSub.find_last_of ( "(" );

    if ( foundBracket != string::npos ) return EvalComplexString<T2> ( toEvalSub, result );
    else return EvalSimpleString<T2> ( toEvalSub, result );
}

inline double EvalString ( string toEval, map<string, double*>* linkMap_ = nullptr, double* result = nullptr )
{
    return EvalString<double, double> ( toEval, linkMap_, result );
}

template<typename T> bool SimpleFormulaComparator ( string compStr, map<string, T*>* linkMap_ = nullptr )
{
    std::size_t lessThanPos, moreThanPos, equalPos;

    string compStrSub = compStr;

    if ( linkMap_ != nullptr ) compStrSub = SubstituteStrInFormula ( compStr, *linkMap_ );

//     cout << "Comparison string after substitution: " << compStrSub << "\n";

    std::size_t opPos = compStrSub.find_first_of ( "<>=" );

    if ( opPos == string::npos )
    {
        double expr = EvalString<double, double> ( compStr );

        if ( expr != 0 ) return true;
        else return false;
    }

    string leftMemberStr = compStrSub.substr ( 0, opPos );

    //     cout << "Extracted left member: " << leftMemberStr << "\n";

    double leftMember = EvalString<double, double> ( leftMemberStr );

    opPos = compStrSub.find_last_of ( "<>=" );

    string rightMemberStr = compStrSub.substr ( opPos+1 );

    //     cout << "Extracted right member: " << rightMemberStr << "\n";

    double rightMember = EvalString<double, double> ( rightMemberStr );

    // -------- Processing the comparison ------------------ //

    lessThanPos = compStrSub.find ( "<" );
    moreThanPos = compStrSub.find ( ">" );
    equalPos = compStrSub.find ( "=" );

    if ( lessThanPos != string::npos && moreThanPos == string::npos )
    {
        if ( equalPos != string::npos ) return leftMember <= rightMember;
        else return leftMember < rightMember;
    }
    else if ( moreThanPos != string::npos && lessThanPos == string::npos )
    {
        if ( equalPos != string::npos ) return leftMember >= rightMember;
        else return leftMember > rightMember;
    }
    else if ( moreThanPos == string::npos && lessThanPos == string::npos && equalPos != string::npos ) return leftMember == rightMember;

    cerr << "Unable to decode the comparator string!\n";

    return false;
}

template<typename T> bool ComplexFormulaComparator ( string compStr, map<string, T*>* linkMap_ = nullptr )
{
    map<std::size_t, char> andOrSigns;
    map<std::size_t, bool> members;

    std::size_t newAndOrSign, newMember;

    newAndOrSign = compStr.find_first_of ( "|&" );
    newMember = compStr.find_first_not_of ( " |&" );

    if ( newAndOrSign < newMember || ( newAndOrSign != string::npos && compStr[newAndOrSign] != compStr[newAndOrSign+1] ) )
    {
        cerr << "Invalid formula: " << compStr << " ...\n";
        return false;
    }

    if ( newAndOrSign == string::npos ) return SimpleFormulaComparator<T> ( compStr, linkMap_ );

    while ( newAndOrSign != string::npos )
    {
        members[newMember] = SimpleFormulaComparator<T> ( compStr.substr ( newMember, newAndOrSign-newMember ), linkMap_ );
        andOrSigns[newAndOrSign] = compStr[newAndOrSign];

        newMember = compStr.find_first_not_of ( " |&", newAndOrSign+2 );
        newAndOrSign = compStr.find_first_of ( "|&", newAndOrSign+2 );

        if ( newAndOrSign != string::npos && compStr[newAndOrSign] != compStr[newAndOrSign+1] )
        {
            cerr << "Invalid formula: " << compStr << " ...\n";
            return false;
        }

        if ( newAndOrSign == string::npos ) members[newMember] = SimpleFormulaComparator<T> ( compStr.substr ( newMember ), linkMap_ );
    }

    auto compItr = andOrSigns.begin();
    auto memItr = members.begin();

//     cout << "The different comparator signs are:\n";
//     while ( compItr != andOrSigns.end() )
//     {
//         cout << compItr->second << " @ pos " << compItr->first << "\n";
//         compItr++;
//     }
//
//     cout << "The different members of the formula are:\n";
//     while ( memItr != members.end() )
//     {
//         cout << memItr->second << " @ pos " << memItr->first << "\n";
//         memItr++;
//     }

    while ( andOrSigns.size() > 0 )
    {
        compItr = andOrSigns.begin();

        while ( compItr != andOrSigns.end() )
        {
            if ( compItr->second == '&' )
            {
                break;
            }

            compItr++;
        }

        if ( compItr == andOrSigns.end() ) compItr--;

        bool subRes, rightRes, leftRes;

        memItr = members.upper_bound ( compItr->first );

        rightRes = memItr->second;
        memItr--;
        leftRes = memItr->second;

        if ( compItr->second == '&' ) subRes = leftRes && rightRes;
        else subRes = leftRes || rightRes;

        memItr->second = subRes;

        memItr++;
        members.erase ( memItr );

        andOrSigns.erase ( compItr );
    }

    memItr = members.begin();

    return memItr->second;
}

template<typename T> bool StringFormulaComparator ( string compStr, map<string, T*>* linkMap_ = nullptr )
{
    string toEvalCp = compStr;
    string newEvalStr;

    std::size_t openBracketPos, closeBracketPos;

    openBracketPos = toEvalCp.find_last_of ( "(" );
    closeBracketPos = toEvalCp.find_first_of ( ")", openBracketPos );

    if ( ( openBracketPos == string::npos && closeBracketPos != string::npos ) || ( openBracketPos != string::npos && closeBracketPos == string::npos ) )
    {
        cerr << "Invalid formula: " << compStr << " ...\n";
        return false;
    }

    if ( openBracketPos == string::npos )
    {
        return ComplexFormulaComparator<T> ( compStr,linkMap_ );
    }

    while ( openBracketPos != string::npos && closeBracketPos != string::npos )
    {
        newEvalStr = toEvalCp.substr ( 0, openBracketPos );

        bool eval;
        eval = ComplexFormulaComparator<T> ( toEvalCp.substr ( openBracketPos+1, closeBracketPos - openBracketPos - 1 ), linkMap_ );
        newEvalStr += Form ( "%d", ( int ) eval );

        newEvalStr += toEvalCp.substr ( closeBracketPos+1, toEvalCp.length()- closeBracketPos - 1 );

        toEvalCp = newEvalStr;

        //         cout << "Operation processed. New string value: " << newEvalStr << "\n";

        openBracketPos = toEvalCp.find_last_of ( "(" );
        closeBracketPos = toEvalCp.find_first_of ( ")", openBracketPos );
    }

    return ComplexFormulaComparator<T> ( toEvalCp, linkMap_ );
}

inline bool StringFormulaComparator ( string compStr )
{
    return StringFormulaComparator<double> ( compStr );
}

// -------------------- User Analysis Macros Stuffs --------------------------------------- //

extern GoddessAnalysis* gA;

extern std::map<string, std::pair<TObject*, vector<GamData*>>> histsMap;
extern vector<string> specialHists;

inline void LoadTrees()
{
    cout << "To initialize the chain of runs, type:\n   LoadTrees( (string) treeName, (string) fileName1, (string) fileName2, (string) ... )\n\n";
}

template<typename First, typename... Rest> void LoadTrees ( string treeName, First fileName1, Rest... fileNameRest )
{
    gA = new GoddessAnalysis();

    gA->InitUserAnalysis ( treeName, fileName1, fileNameRest... );
}

void ResetHistsStates ( bool all = false );

bool CheckHistState ( string histName, GamData* gD );

bool CheckHistState ( TObject* hist, GamData* gD );

void AddToHistState ( string histName, GamData* gD );

void AddToHistState ( TObject* hist, GamData* gD );

template<typename THist> void AddHists ( THist* h1, THist* h2 )
{
    if ( std::is_same<THist, TH1F>::value || std::is_same<THist, TH2F>::value )
    {
        h1->Add ( h2 );
    }
}

void AddHists ( TH1F* h1, TH1F* h2 );

void AddHists ( TH2F* h1, TH2F* h2 );

template<typename THist, typename... Rest> void AddHists ( THist* h1, THist* h2, Rest... otherHists )
{
    if ( std::is_same<THist, TH1F>::value || std::is_same<THist, TH2F>::value )
    {
        AddHists ( h1, h2 );
        AddHists ( h1, otherHists... );
    }
}

template<typename THist, typename... Rest> THist* DrawSum ( THist* h1, THist* h2, Rest... otherHists )
{
    if ( std::is_same<THist, TH1F>::value || std::is_same<THist, TH2F>::value )
    {
        THist* hSum = ( THist* ) h1->Clone();

        AddHists ( hSum, h2, otherHists... );

        hSum->Draw();

        return hSum;
    }
    else return nullptr;
}

TH1F* DrawSum ( TH1F* h1, TH1F* h2, bool cloneFirst = true, bool doDraw = true );

TH1F* DrawSum ( TH1F** hists, string toSum );

TH2F* DrawSum ( TH2F** hists, string toSum );

TH1F* DrawSum ( TH1F** hists );

void PrintProgress ( long long int maxEvents_, long long int currEvt );

void PrintHistsMapContent();

TH1F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX, bool addToSpecialList = false );

TH2F* MakeNewHist ( string name, string title, unsigned int nBinsX, int minX, int maxX, unsigned int nBinsY, int minY, int maxY, bool addToSpecialList = false );


#endif





















