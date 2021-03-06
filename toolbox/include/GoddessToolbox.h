#ifndef __GODDESSTOOLBOX__
#define __GODDESSTOOLBOX__

#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <random>
#include <type_traits>
#include <cxxabi.h>
#include <pthread.h>
#include <dirent.h>
#include <utility>
#include <tuple>
#include <array>
#include <chrono>
#include <ctime>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <list>
#include <functional>
#include <cassert>
#include <algorithm>

#include "TObject.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TMath.h"
#include "TKey.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TCutG.h"
#include "TDirectory.h"

using namespace std;

const char esc ( 27 );

extern string pathToGDAQ;

//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//

string GetLocalTimeAndDate ( bool doPrint = false );
string GetCurrentYear ( bool doPrint = false );

string GetCurrentDayName ( bool doPrint = false );
string GetCurrentMonthName ( bool doPrint = false );

int GetCurrentDayNum ( bool doPrint = false );
int GetCurrentMonthNum ();

int GetCurrentHour ( bool doPrint = false );
int GetCurrentMinute ( bool doPrint = false );
int GetCurrentSecond ( bool doPrint = false );

//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//

inline function<bool ( char,char ) > ignoreCharCasePred ( bool caseSensitive )
{
    return [caseSensitive] ( char a, char b ) -> bool
    {
        if ( !caseSensitive ) return ( std::tolower ( a ) == std::tolower ( b ) );
        else return a == b;
    };
}

template<typename T> struct has_operator_equal
{
    template<typename U> static auto test ( U* ) -> decltype ( declval<U>() == declval<U>() );
    template<typename> static auto test ( ... ) -> false_type;

    static typename is_same<bool, decltype ( test<T> ( nullptr ) ) >::type type;
    static const bool value = is_same<bool, decltype ( test<T> ( nullptr ) ) >::value;
};

template<typename T> struct has_redirect_output
{
    template<typename U> static auto test ( U* ) -> decltype ( cout << declval<U>() );
    template<typename> static auto test ( ... ) -> false_type;

    static typename is_same<decltype ( cout << "whatever" ), decltype ( test<T> ( nullptr ) ) >::type type;
    static const bool value = ( is_same<decltype ( cout << "whatever" ), decltype ( test<T> ( nullptr ) ) >::value );
};

//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//


bool CompareStrings ( string ref_, string search_, bool exactMatch = true, bool caseSensitive = true );

vector<int> DecodeNumberString ( string itemsString, bool verbose = false );

vector<string> DecodeTags ( string tagsStr );

vector<string> GetDirContent ( string dirName = "./", string mode = "root", string endWith = "",
                               string mustHaveAll = "", string cantHaveAny = "", string mustHaveOneOf = "", string startWith = "", bool caseSensitive = true, bool includePath = false );

vector<string> DecodeItemsToTreat ( string itemsString, string mode = "root", bool caseSensitive = true, bool includePath = false );

vector<string> SplitString ( string toSplit, string splitter );

//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//


void ReadDetectorID ( string DetectorID, bool* isUpstream, unsigned short* numsector, bool* isBarrel, int* layernum, bool* side, bool* isSX3 );

string GetDetectorID ( bool isUpstream, bool isBarrel, unsigned int sector );

string GetNameCompliantStr ( int input );

int ToStripID ( bool isUpstream_, bool isBarrel_, bool isFront_, int sector_, int strip_ );
vector<int> ToStripID ( string sectorStr, bool displayList = false );
void ToStripID ( vector<int>* dest, string sectorStr );

template<typename... Rest> void ToStripID ( vector<int>* dest, string sectorStr1, Rest... otherSectorStr )
{
    dest->clear();

    ToStripID ( dest, sectorStr1 );

    ToStripID ( dest, otherSectorStr... );
}

template<typename... Rest> vector<int> ToStripID ( string sectorStr1, Rest... otherSectorStr )
{
    vector<int> stripIDsList;

    ToStripID<string> ( &stripIDsList, sectorStr1, otherSectorStr... );

    return stripIDsList;
}

string FromStripID ( int stripID_, bool& isUpstream_, bool& isBarrel_, bool& isFront_, int& sector_, int& strip_ );
string FromStripID ( int stripID_ );

//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//


void ListHistograms ( string match = "", unsigned int limit = 0, unsigned int startAt = 0, bool caseSensitive = true );
void WriteHistograms ( string outfile, string match = "*", bool caseSensitive = true );

//_______________________________________________________________________________________________________________________________________________//

template<false_type& fType1, typename rType2, typename KeyType, typename ValType> void PrintMapKeysAndValues ( map<KeyType, ValType> map_ )
{
    cout << "No valid Keys to redirect to the standard output...\n";
}

//_______________________________________________________________________________________________________________________________________________//

template<true_type& tType1, false_type& fType2, typename KeyType, typename ValType> void PrintMapKeysAndValues ( map<KeyType, ValType> map_ )
{
    auto itr = map_.begin();

    cout << "Map Keys:\n";

    while ( itr != map_.end() )
    {
        cout << itr->first << endl;

        itr++;
    }
}

//_______________________________________________________________________________________________________________________________________________//

template<true_type& tType1, true_type& tType2, typename KeyType, typename ValType> void PrintMapKeysAndValues ( map<KeyType, ValType> map_ )
{
    auto itr = map_.begin();

    cout << "Map Content:\n";

    while ( itr != map_.end() )
    {
        cout << itr->first << " : " << itr->second << endl;

        itr++;
    }
}

//_______________________________________________________________________________________________________________________________________________//

template<typename KeyType = string, typename ValType = int> void PrintMapContent ( map<KeyType, ValType> map_ )
{
    PrintMapKeysAndValues<has_redirect_output<KeyType>::type, has_redirect_output<ValType>::type, KeyType, ValType> ( map_ );
}

//_______________________________________________________________________________________________________________________________________________//

inline string FindAndReplaceInString ( string input, string toReplace, string substitute, unsigned int nTimes = 0, unsigned int startIndex = 0 )
{
    int repSize = toReplace.length();

    size_t foundPos = input.find ( toReplace.c_str() );

    unsigned int counter = 0;
    unsigned int replaced = 0;
    bool noLimit = false;
    size_t skipPos = 0;
    if ( nTimes == 0 )
    {
        nTimes = replaced+1;
        noLimit = true;
    }

    string newString = input;

    while ( foundPos != string::npos && replaced < nTimes )
    {
        if ( counter >= startIndex )
        {
            newString.replace ( foundPos, repSize, substitute );

            foundPos = newString.find ( toReplace.c_str(), skipPos );
            replaced++;
        }
        else
        {
            skipPos = foundPos+repSize;
            foundPos = newString.find ( toReplace.c_str(), skipPos );
        }

        counter++;
        if ( noLimit ) nTimes = replaced+1;
    }

    return newString;
}

//_______________________________________________________________________________________________________________________________________________//

inline string ReplaceSpecialVariables ( string varStr )
{
    map<string, double> specialVars;

    specialVars["Pi"] = TMath::Pi();

    for ( auto sVarsItr = specialVars.begin(); sVarsItr != specialVars.end(); sVarsItr++ )
    {
        varStr = FindAndReplaceInString ( varStr, sVarsItr->first, Form ( "%3.10f", sVarsItr->second ) );
    }

    return varStr;
}

//_______________________________________________________________________________________________________________________________________________//

template<typename T1, typename T2> void AddLinkMapEntries ( map<string, T1*>* linkMap_, string str, T2& val )
{
    ( *linkMap_ ) [str] = ( T1* ) ( &val );

    return;
}

//_______________________________________________________________________________________________________________________________________________//

template<typename T1, typename T2, typename... Rest> void AddLinkMapEntries ( map<string, T1>* linkMap_, string varsStr, T2& var1, Rest&... varRest )
{
    size_t fstMemPosStart = varsStr.find_first_not_of ( " " );

    size_t fstMemPosEnd = varsStr.find_first_of ( " ", fstMemPosStart );

    string var1Str = varsStr.substr ( fstMemPosStart, fstMemPosEnd - fstMemPosStart );

    string newVarsStr = varsStr.substr ( fstMemPosEnd+1 );

    AddLinkMapEntries ( linkMap_, var1Str, var1 );

    AddLinkMapEntries ( linkMap_, newVarsStr, varRest... );

    return;
}

//_______________________________________________________________________________________________________________________________________________//

template<typename T, typename... Rest> map<string, T*> MakeLinkMap ( string varsStr, T& var1, Rest&... varRest )
{
    map<string, T*> linkMap;

    AddLinkMapEntries ( &linkMap, varsStr, var1, varRest... );

    return linkMap;
}

//_______________________________________________________________________________________________________________________________________________//

template<typename T> string SubstituteStrInFormula ( string formula, map<string, T*> linkMap )
{
    string modFormula = formula, buffFormula;

    for ( auto itr = linkMap.begin(); itr != linkMap.end(); itr++ )
    {
        string toSubstitute = itr->first;

        size_t  subPos = modFormula.find ( toSubstitute.c_str() );

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

//_______________________________________________________________________________________________________________________________________________//

template<typename T> T EvalSimpleString ( string toEval, T* result = nullptr )
{
    toEval = ReplaceSpecialVariables ( toEval );

    size_t invalidCharacter = toEval.find_first_not_of ( " +-*/0123456789." );

    if ( invalidCharacter != string::npos )
    {
        cerr << "Invalid formula: " << toEval << endl;

        return ( ( T ) 0 );
    }

    map<size_t, char> operators;
    operators.clear();

    map<size_t, T> members;
    members.clear();

    size_t newMemberStartPos;

    newMemberStartPos = toEval.find_first_of ( "0123456789" );

    size_t newOperatorPos = toEval.find_first_of ( "+-*/" );

    if ( newOperatorPos == string::npos && toEval.find_first_of ( "0123456789" ) != string::npos ) return ( ( T ) stod ( toEval ) );

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
            members[newMemberStartPos] = ( T ) stod ( toEval.substr ( newMemberStartPos, opItr->first - newMemberStartPos ) );

            newMemberStartPos = toEval.find_first_of ( "0123456789", opItr->first );

            if ( operators.find ( newMemberStartPos ) != operators.end() ) newMemberStartPos = toEval.find_first_of ( "0123456789", opItr->first );
        }
        else
        {
            members[newMemberStartPos] = ( T ) stod ( toEval.substr ( newMemberStartPos ) );
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

            size_t foundDigit = toEval.find_first_of ( "0123456789", prevItr->first );

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
        //         int opNumber = ( int ) distance ( operators.begin(), opItr );
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

//_______________________________________________________________________________________________________________________________________________//

template<typename T> T EvalComplexString ( string toEval, T* result = nullptr )
{
    string toEvalCp = toEval;
    string newEvalStr;

    size_t openBracketPos, closeBracketPos;

    openBracketPos = toEvalCp.find_last_of ( "(" );
    closeBracketPos = toEvalCp.find_first_of ( ")", openBracketPos );

    while ( openBracketPos != string::npos && closeBracketPos != string::npos )
    {
        bool doAbs = false;

        size_t absPos = toEval.find ( "abs" );

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

//_______________________________________________________________________________________________________________________________________________//

template<typename T1, typename T2> T2 EvalString ( string toEval, map<string, T1*>* linkMap_ = nullptr, T2* result = nullptr )
{
    string toEvalSub = toEval;

    if ( linkMap_ != nullptr ) toEvalSub = SubstituteStrInFormula<T1> ( toEval, *linkMap_ );

    size_t foundBracket = toEvalSub.find_last_of ( "(" );

    if ( foundBracket != string::npos ) return EvalComplexString<T2> ( toEvalSub, result );
    else return EvalSimpleString<T2> ( toEvalSub, result );
}

//_______________________________________________________________________________________________________________________________________________//

inline double EvalString ( string toEval, map<string, double*>* linkMap_ = nullptr, double* result = nullptr )
{
    return EvalString<double, double> ( toEval, linkMap_, result );
}

//_______________________________________________________________________________________________________________________________________________//

template<typename T> bool SimpleFormulaComparator ( string compStr, map<string, T*>* linkMap_ = nullptr )
{
    size_t lessThanPos, moreThanPos, equalPos;

    string compStrSub = compStr;

    if ( linkMap_ != nullptr ) compStrSub = SubstituteStrInFormula ( compStr, *linkMap_ );

    //     cout << "Comparison string after substitution: " << compStrSub << "\n";

    size_t opPos = compStrSub.find_first_of ( "<>=" );

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

//_______________________________________________________________________________________________________________________________________________//

template<typename T> bool ComplexFormulaComparator ( string compStr, map<string, T*>* linkMap_ = nullptr )
{
    map<size_t, char> andOrSigns;
    map<size_t, bool> members;

    size_t newAndOrSign, newMember;

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

//_______________________________________________________________________________________________________________________________________________//

template<typename T> bool StringFormulaComparator ( string compStr, map<string, T*>* linkMap_ = nullptr )
{
    string toEvalCp = compStr;
    string newEvalStr;

    size_t openBracketPos, closeBracketPos;

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

//_______________________________________________________________________________________________________________________________________________//

inline bool StringFormulaComparator ( string compStr )
{
    return StringFormulaComparator<double> ( compStr );
}

//_______________________________________________________________________________________________________________________________________________//

function<bool ( double,double ) > CheckValProxFunc ( double compVal );

int RoundValue ( double val );

pair<vector<double>, vector<double>> FillGraphFromFile ( string input );

double EvalGraph ( vector<double> x_, vector<double> y_, double toEval );
double IntegrateGraph ( vector<double> x_, vector<double> y_, double xMin_, double xMax_, double dx_ );

double GetEffectiveThickness ( double angle, double targetThickness_ );

double ComputeEnergyLoss ( vector<double> energies_, vector<double> rangeOrStoppingPower_, double startingEnergy, float mass, double xMin_, double xMax_, double dx_, string mode = "Interpolation" );

double TryGetRemainingEnergy ( string mass_db, int mass, int charge, double startingEnergy, double thickness_, double dx_,
                               string targetStr = "*", double density = 0, string tablePath = "./", string mode = "Interpolation" );

//_______________________________________________________________________________________________________________________________________________//

int InitReadMassesForKinematic ( ifstream& mass_db );

void GetRelevantInfoPositions ( string* readWord, short& posMassExcess, short& posBindingEnergy, short& posBetaDecay, short& posAMU, short& posElement );

void GetAtomicFormula ( ifstream& mass_db, int mass, int charge, string& toReconstruct );
void DecodeAtomicFormula ( ifstream& mass_db, string toDecode, int& mass, int& charge, float& atomicMass );

bool CharIsDigit ( char toCheck );

template<typename T> bool IsSameValue ( T a_, T b_ )
{
    return a_ == b_;
}

template<typename T1, typename T2> bool IsSameValue ( T1 a_, T2 b_ )
{
    if ( !is_same<T1, T2 >::value ) return false;
    else return IsSameValue<T1> ( a_, b_ );
}

template<typename T2> inline int CheckForMatch ( string* readWord, short posElement, short massCheck, T2 chargeCheck )
{
    //     cout << "Performing the CheckForMatch function with: " << posMassExcess << " / " << posBindingEnergy << " / " << posBetaDecay << " / " << posAMU;
    //     cout << " / " << posElement << " / " << massCheck << " / " << chargeCheck << " / " << memberID << " / "<< "\n";

    int charge = -1;

    int foundMatch = -1;

    if ( stoi ( readWord[posElement-1] ) == massCheck )
    {
        if ( is_same<int, decltype ( chargeCheck ) >::value )
        {
            if ( IsSameValue ( stoi ( readWord[posElement-2] ), chargeCheck ) ) foundMatch = 1;
        }
        else if ( is_same<string, decltype ( chargeCheck ) >::value )
        {
            //             cout << "Searching Element by Atomic Symbol " << chargeCheck << " ...\n";

            if ( IsSameValue ( readWord[posElement], chargeCheck ) )
            {
                foundMatch = 1;
            }
        }
    }

    if ( foundMatch >= 0 )
    {
        //         cout << "Found a matching pattern: " << foundMatch << " ...\n";

        charge = stoi ( readWord[posElement-2] );

        //         cout << "Decoded charge: " << charge << " ...\n";
    }

    return charge;
}


//_________________________________________GEOMETRY AND TRIGONOMETRY FUNCTIONS & UTILITIES_______________________________________________________//


float findAzimuthFromCartesian ( float xx, float yy );
float findPolarFromCartesian ( float xx, float yy, float zz, float* rr );

//_________________________________________RANDOM INITIALIZER UTILITIES_______________________________________________________//

int GetASeed ( unsigned int *seed );

//_________________________________________BINARY FILES UTILITIES_______________________________________________________//

struct BinDecHeadStruct
{
    int type;
    int length;
    unsigned long long timestamp;
};

extern ifstream binaryFileToRead;
extern string currentBinaryFileName;
extern std::pair<BinDecHeadStruct*, char*>* gebWord;
extern char* binPayload;

unsigned long long int BytesCounter ( string fName );

bool OpenBinaryFile ( string fname );

char* GetAWord ( ifstream& inFile, int nBytesToRead, bool invertWords = false );

char* GetAWord ( ifstream& inFile, string binType = "GEB", bool invertWords = false );

char* GetAWord ( string binType, bool invertWords = false );

char* GetAWord ( string binType, unsigned long long int eventNbr, bool invertWords = false );

char* GetAWord ( string fname, string binType = "GEB", unsigned long long int eventNbr = 0, bool invertWords = false );

void ReadGEBHeader();

template<typename T = unsigned int> T ReadPayload ( int wordNum, unsigned short firstBit, unsigned short lastBit, int bitShift, bool doPrint )
{
    //   cout << "Reading payload for word #" << wordNum << " ( first byte = " << firstByte << " / last byte = " << lastByte << " )\n";

    if ( binPayload == NULL ) return 0;

    T* buf = ( T* ) ( binPayload + wordNum * sizeof ( T ) ); // binPayload is a char* so the incrementation of the pointer is by sizeof(char) which is 1...

    long long int bitmask = 0;

    for ( unsigned short i = 0; i <= lastBit-firstBit; i++ )
    {
        bitmask += pow ( 2, i );
    }

    T res;

    if ( bitShift >= 0 )
    {
        res = ( ( ( *buf >> firstBit ) & bitmask ) << bitShift );
    }
    else
    {
        res = ( ( ( *buf >> firstBit ) & bitmask ) >> bitShift );
    }

    if ( doPrint )
    {
        cout << "Word #" << wordNum << ": " << res << "\n";
    }

    return res;
}

char* ReadBinData ( int size );

void InvertEndianness ( char** word, int sizeofword );

template<typename T = unsigned int> T DecodeBinData ( char* data, unsigned short firstBit, unsigned short lastBit, int bitShift, bool doPrint = false )
{
    unsigned long long int* buffer = new unsigned long long int;

    if ( is_same<T,char>::value )
    {
        *buffer = static_cast<int> ( *data );
    }
    else
    {
        buffer = ( unsigned long long int* ) data;
    }

    long long unsigned int bitmask = ( unsigned long long int ) pow ( 2, (lastBit-firstBit+1) ) - 1;

    if ( bitShift >= 0 )
    {
        *buffer = ( ( ( *buffer >> firstBit ) & bitmask ) << bitShift );
    }
    else
    {
        *buffer = ( ( ( *buffer >> firstBit ) & bitmask ) >> bitShift );
    }

    T* res = ( T* ) buffer;

    if ( doPrint )
    {
        cout << "Data: " << data << endl;
        cout << "Raw cast: " << * ( ( T* ) data ) << endl;
        cout << "Extracted: " << *res << "\n";
    }

    return *res;
}

template<typename T = unsigned int> T ReadBinData ( int nSkipBytes, unsigned short firstBit, unsigned short lastBit, int bitShift, bool invertWords, bool doPrint = false )
{
    long long unsigned int currPos = binaryFileToRead.tellg();
    binaryFileToRead.seekg ( currPos+ nSkipBytes );

    char* read = ReadBinData ( sizeof ( T ) );

    if ( invertWords ) InvertEndianness ( &read, sizeof ( T ) );

    return DecodeBinData<T> ( read, firstBit, lastBit, bitShift, doPrint );
}

bool GEBBinaryEvFilter ( std::pair<BinDecHeadStruct*, char*>* readWord_ );

// TTree* MakeRootTree ( int* type_, int* channel_, unsigned long int* value_, unsigned long long int* timestamp_ )
// {
//     TTree* newTree = new TTree ( "unpacked", "Unpacked Tree" );
//
//     newTree->Branch ( "type", type_ );
//     newTree->Branch ( "channel", channel_ );
//     newTree->Branch ( "value", value_ );
//     newTree->Branch ( "timestamp", timestamp_ );
//
//     return newTree;
// }

void DecodeGEBBinary ( string fname, unsigned long int nevents, unsigned long int fstevent = 0, bool invertWords = true, bool quietMode = false, string outRootName = "",
                       string mapFileName = "/mnt/hgfs/Dropbox/ORNL/goddess_daq/goddess_daq/share/map.dat" );

// TGraph* GetGEBWaveForm();

// void InitializePlotGEBWaveForm ( string fname, string binType = "GEB", bool invertWords = true );

// TH2F* PlotGEBWaveForm ( std::function<bool() > channelFilterFn = []()
// {
//     return true;
// }, long long int reqN = 1, long long int maxEv = -1, bool invertWords = true );
// 
// TH2F* PlotGEBWaveForm ( string channelFilterFn, long long int reqN = 1, long long int maxEv = -1, bool invertWords = true, bool init = true );

extern int compDiffCounter;

extern ifstream binCompFile1;
extern ifstream binCompFile2;

void CompareGEBBinaries ( bool strictComp = false, bool invertWords = true );


void CompareGEBBinaries ( string fName1, string fName2, bool strictComp = false, bool invertWords = true );

#endif





























