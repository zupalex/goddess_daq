#include "goddess_analysis_macros.h"
#include "GoddessCalib.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::clog;
using std::endl;

TH2F *histToTreat = 0;
std::map<Int_t, std::pair<TH1D*, TF1*>> myFitsResults;
std::map<Int_t, TF1*> fitsWithIssues;
std::map<Int_t, TCanvas*> listOfResCanvas;

float reqChi2Max = 60.;
float reqChi2Min = 1.;

string fitMode = "random";

unsigned short lastNbrOfGauss;
float lastEstWidth, lastThr, lastUpLim;
bool lastIgnoreTail;

std::map<unsigned short, std::pair<float, float>> myCalMap;

TH2F* SetHistToTreat ( TH2F* inHist )
{
    histToTreat = inHist;

    myFitsResults.clear();
    fitsWithIssues.clear();

    return histToTreat;
}

void SetFitMatchLevel ( float chi2Max_, float chi2Min_ )
{
    reqChi2Max = chi2Max_;
    reqChi2Min = chi2Min_;
}

void SetFitMode ( string mode )
{
    if ( ! ( mode == "iteration" || mode == "random" ) )
    {
        cerr << "The fitting mode is set to an improper value (" << fitMode << "). Expecting \"iteration\" or \"random\"" << std::endl;
        cerr << "Change will be ignored..." << std::endl;
        return;
    }

    fitMode = mode;
}

vector<TH1*> GetHistsFromFile ( TFile *file )
{
    vector<TH1*> listOfHists;

    listOfHists.clear();

    if ( file != NULL && file->IsOpen() )
    {
        auto lOK = file->GetListOfKeys();

        if ( lOK != NULL )
        {
            for ( Int_t i = 0; i < lOK->GetSize(); i++ )
            {
                TH1* retHist = ( TH1* ) file->Get ( lOK->At ( i )->GetName() );

                if ( retHist != NULL ) listOfHists.push_back ( retHist );
            }
        }
    }

    cout << "Retrieved the histograms from the file: " << std::endl;

    file->ls();

    return listOfHists;
}

Double_t myGauss ( Double_t *x, Double_t *par )
{
    Double_t res = 0;

    for ( unsigned short i = 0; i < par[0]; i++ )
    {
        res += par[2*i+2] * TMath::Exp ( -pow ( x[0] - par[2*i+3],2 ) / pow ( 2 * par[1],2 ) );
    }

    return res;
}

TF1* InitMyGaussParams ( TH1D* inHist, float estWidth, unsigned short nbrOfGauss, Int_t mean, Int_t xMin, Int_t xMax, bool ignoreTail = true, bool rdmParams = false )
{
    if ( ignoreTail ) nbrOfGauss = 1;

    unsigned short nParams = nbrOfGauss*2+2;

    Double_t params[nParams];

    float finalRange[2] = { ( float ) xMin, ( float ) xMax };

    double maxPerForFit = 0.15;
    double widthFracForFit = 2.;

    if ( rdmParams )
    {
        std::random_device rd;
        std::mt19937 gen ( rd() );
        std::uniform_real_distribution<> distrib ( 0.15, 0.3 );

        maxPerForFit = distrib ( gen );

        distrib = std::uniform_real_distribution<> ( 0.15,5. );

        widthFracForFit = distrib ( gen );
    }

    params[0] = nbrOfGauss;
    params[1] = estWidth;

    Double_t scale = inHist->GetXaxis()->GetXmax() /1000.;

    for ( unsigned short ng = 0; ng < nbrOfGauss; ng++ )
    {
        params[2*ng+2] = inHist->GetBinContent ( inHist->GetMaximumBin() );
        params[2*ng+3] = mean-ng*scale;
    }

    if ( ignoreTail )
    {
        Double_t thr = inHist->GetBinContent ( inHist->GetMaximumBin() ) * maxPerForFit;
        Double_t endPlateau = inHist->GetBinContent ( inHist->GetMaximumBin() ) * 0.9;
        Double_t curThr = inHist->GetBinContent ( inHist->GetMaximumBin() );

        int tt = 0;
        int tp = 0;

        while ( curThr > thr )
        {
            if ( tp == 0 && inHist->GetBinContent ( inHist->GetMaximumBin() + tt ) < endPlateau )
            {
                tp = tt-1;
            }

            curThr = inHist->GetBinContent ( inHist->GetMaximumBin() + tt );
            tt++;
        }

        finalRange[0] = inHist->GetBinCenter ( inHist->GetMaximumBin() + tp - tt * widthFracForFit );
        finalRange[1] = inHist->GetBinCenter ( inHist->GetMaximumBin() + tt );
        //finalRange[0] = (int) inHist->GetBinCenter(inHist->GetMaximumBin()) - estWidth/widthFracForFit;
    }

    TF1 *func = new TF1 ( "fit_mygauss",myGauss,finalRange[0], finalRange[1], nParams );

    func->SetParameters ( params );

    func->FixParameter ( 0, nbrOfGauss );

    func->SetParLimits ( 1, 0, xMax - xMin );

    for ( unsigned short ng = 0; ng < nbrOfGauss; ng++ )
    {
        func->SetParLimits ( 2*ng+2, inHist->GetBinContent ( inHist->GetMaximumBin() ) /2., inHist->GetBinContent ( inHist->GetMaximumBin() ) *2 );
        func->SetParLimits ( 2*ng+3, finalRange[0], finalRange[1] );
    }

    func->SetRange ( finalRange[0], finalRange[1] );

    func->SetNpx ( 1000 );

    return func;
}

TF1* InitMyGaussParams ( TH1D* inHist, float estWidth, unsigned short nbrOfGauss, Int_t mean, Int_t xMin, Int_t xMax, bool ignoreTail = true, double maxPercentOffset = 0, double widthFracOffset = 0 )
{
    if ( ignoreTail ) nbrOfGauss = 1;

    unsigned short nParams = nbrOfGauss*2+2;

    Double_t params[nParams];

    float finalRange[2] = { ( float ) xMin, ( float ) xMax };

    double maxPerForFit = 0.15 + maxPercentOffset;
    double widthFracForFit = 0.33 + widthFracOffset;

    params[0] = nbrOfGauss;
    params[1] = estWidth;

    Double_t scale = inHist->GetXaxis()->GetXmax() /1000.;

    for ( unsigned short ng = 0; ng < nbrOfGauss; ng++ )
    {
        params[2*ng+2] = inHist->GetBinContent ( inHist->GetMaximumBin() );
        params[2*ng+3] = mean-ng*scale;
    }

    if ( ignoreTail )
    {
        Double_t thr = inHist->GetBinContent ( inHist->GetMaximumBin() ) * maxPerForFit;
        Double_t curThr = inHist->GetBinContent ( inHist->GetMaximumBin() );

        int tt = 0;

        while ( curThr > thr )
        {
            curThr = inHist->GetBinContent ( inHist->GetMaximumBin() + tt );
            tt++;
        }

        finalRange[0] = inHist->GetBinCenter ( inHist->GetMaximumBin() - tt / widthFracOffset );
        finalRange[1] = inHist->GetBinCenter ( inHist->GetMaximumBin() + tt );
        //finalRange[0] = (int) inHist->GetBinCenter(inHist->GetMaximumBin()) - estWidth/widthFracForFit;
    }

    TF1 *func = new TF1 ( "fit_mygauss",myGauss,finalRange[0], finalRange[1], nParams );

    func->SetParameters ( params );

    func->FixParameter ( 0, nbrOfGauss );

    func->SetParLimits ( 1, 0, xMax - xMin );

    for ( unsigned short ng = 0; ng < nbrOfGauss; ng++ )
    {
        func->SetParLimits ( 2*ng+2, inHist->GetBinContent ( inHist->GetMaximumBin() ) /2., inHist->GetBinContent ( inHist->GetMaximumBin() ) *2 );
        func->SetParLimits ( 2*ng+3, finalRange[0], finalRange[1] );
    }

    func->SetRange ( finalRange[0], finalRange[1] );

    func->SetNpx ( 1000 );

    return func;
}

bool IsChi2Wrong ( TF1* func, float upLim = -10., float lowLim = -10. )
{
    if ( upLim < 0 ) upLim = reqChi2Max;
    if ( lowLim < 0 ) lowLim = reqChi2Min;

    return ( func->GetChisquare() > upLim || func->GetChisquare() < lowLim );
}

bool IsParamWrong ( TF1* func, unsigned short parNumber, double minVal, double maxVal )
{
    double parToEvaluate = func->GetParameter ( parNumber );

    return ( parToEvaluate < minVal || parToEvaluate > maxVal );
}

TF1* FitWithMyGauss ( TH1D* inHist, Int_t beginFittedRange, float estWidth, unsigned short nbrOfGauss, Int_t mean, Int_t xMin, Int_t xMax, bool ignoreTail = true, bool shutUp = true )
{
    TF1 *fitRes = InitMyGaussParams ( inHist, estWidth, nbrOfGauss, mean, xMin, xMax, ignoreTail, false );

    string fitOpt = shutUp ? "QRMN" : "RMN";

    if ( inHist->Integral() != 0 ) inHist->Fit ( fitRes, fitOpt.c_str(), "" );

    unsigned short endlessBreak = 0;
    unsigned short maxFixAttemp = 500;

    Double_t maxBinContent = inHist->GetBinContent ( inHist->GetMaximumBin() );
    Double_t ampLowLim = maxBinContent/2.;
    Double_t ampHighLim = maxBinContent*1.5;

    if ( inHist->Integral() != 0 )
    {
        if ( fitMode == "iteration" )
        {
            if ( IsChi2Wrong ( fitRes ) || IsParamWrong ( fitRes, 2, ampLowLim, ampHighLim ) )
            {
                cerr << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*" << std::endl;
                cerr << "Potential issue for the fit of the range starting at  " << beginFittedRange << std::endl;
                cerr << "Trying to fix it..." << std::endl;

                for ( int ll = 0; ll < 60; ll++ )
                {
                    for ( int rr = 0; rr < 60; rr++ )
                    {
                        fitRes = InitMyGaussParams ( inHist, estWidth, nbrOfGauss, mean, xMin, xMax, ignoreTail, ll*0.005, rr*0.5 );

                        inHist->Fit ( fitRes, fitOpt.c_str(), "" );

                        if ( ! ( IsChi2Wrong ( fitRes ) || IsParamWrong ( fitRes, 2, ampLowLim, ampHighLim ) ) )
                        {
                            cout << "Issue seems fixed after " << ll+rr << " attempts..." << std::endl << std::endl;
                            break;
                        }
                    }

                    if ( ! ( IsChi2Wrong ( fitRes ) || IsParamWrong ( fitRes, 2, ampLowLim, ampHighLim ) ) ) break;
                }
            }

            if ( IsChi2Wrong ( fitRes ) || IsParamWrong ( fitRes, 2, ampLowLim, ampHighLim ) )
            {
                cerr << "/!\\/!\\/!\\/!\\ WARNING: Issue not resolved for the range starting at  " << beginFittedRange << " .../!\\/!\\/!\\/!\\" << std::endl << std::endl;
            }
        }

        else if ( fitMode == "random" )
        {
            while ( ( IsChi2Wrong ( fitRes ) || IsParamWrong ( fitRes, 2, ampLowLim, ampHighLim ) ) && endlessBreak <= maxFixAttemp )
            {
                if ( endlessBreak == 0 )
                {
                    cerr << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*" << std::endl;
                    cerr << "Potential issue for the fit of the range starting at  " << beginFittedRange << std::endl;
                    cerr << "Trying to fix it..." << std::endl;
                }

                fitRes = InitMyGaussParams ( inHist, estWidth, nbrOfGauss, mean, xMin, xMax, ignoreTail, true );

                inHist->Fit ( fitRes, fitOpt.c_str(), "" );

                endlessBreak++;
            }

            if ( IsChi2Wrong ( fitRes ) || IsParamWrong ( fitRes, 2, ampLowLim, ampHighLim ) )
            {
                cerr << "/!\\/!\\/!\\/!\\ WARNING: Issue still not resolved after " << endlessBreak << " attemps for range starting at " << beginFittedRange << ".../!\\/!\\/!\\/!\\" << std::endl << std::endl;

                fitsWithIssues[beginFittedRange] = fitRes;
            }
            else if ( endlessBreak > 0 )
            {
                cout << "Issue seems fixed after " << endlessBreak << " attempts..." << std::endl << std::endl;
            }
        }

        else cerr << "The fitting mode is set to an improper value (" << fitMode << "). Expecting \"iteration\" or \"random\"" << std::endl;
    }

    return fitRes;
}

std::pair<TH1D*, TF1*> GetCentroidFromProj ( TH2F* inHist, float estWidth, unsigned short nbrOfGauss, Int_t binMin, Int_t binMax, float threshold, float upperLimit = 0, string histName = "_py", bool ignoreTail = true )
{
    if ( histToTreat == NULL || inHist != histToTreat ) SetHistToTreat ( inHist );

    TH1D *projY = inHist->ProjectionY ( histName.c_str(), binMin, binMax-1 );

    if ( upperLimit == 0 ) upperLimit = projY->GetYaxis()->GetXmax();

    projY->GetXaxis()->SetRangeUser ( threshold, upperLimit );

    Int_t binMaxContent = projY->GetBinCenter ( projY->GetMaximumBin() );

    TF1 *fitFunc = FitWithMyGauss ( projY, inHist->GetXaxis()->GetBinCenter ( ( int ) binMin ), estWidth, nbrOfGauss, binMaxContent, threshold, upperLimit, ignoreTail );

    return std::pair<TH1D*, TF1*> ( projY, fitFunc );
}

std::pair<TH1D*, TF1*> FitStrip ( TH2F* inHist, float estWidth, unsigned short nbrOfGauss, Int_t strip, float threshold, float upperLimit = 0, bool ignoreTail = true )
{
    char histName[10];

    Int_t realStrip = inHist->GetXaxis()->GetBinCenter ( ( int ) strip );

    sprintf ( histName, "_strip%d", realStrip );

    myFitsResults[realStrip] = GetCentroidFromProj ( inHist, estWidth, nbrOfGauss, strip, strip+1, threshold, upperLimit, histName, ignoreTail );

    lastEstWidth = estWidth;
    lastNbrOfGauss = nbrOfGauss;
    lastThr = threshold;
    lastUpLim = upperLimit;
    lastIgnoreTail = ignoreTail;

    return myFitsResults[realStrip];
}

std::pair<TH1D*, TF1*> ReFitStrip ( Int_t strip )
{
    return FitStrip ( histToTreat, lastEstWidth, lastNbrOfGauss, strip, lastThr, lastUpLim, lastIgnoreTail );
}

std::map<Int_t, std::pair<TH1D*, TF1*>> FitRangeOfStrips ( TH2F* inHist, float estWidth, unsigned short nbrOfGauss, Int_t stripMin, Int_t stripMax, float threshold, float upperLimit = 0, bool ignoreTail = true )
{
    for ( Int_t i = 0; i <= stripMax-stripMin; i++ )
    {
        auto result = FitStrip ( inHist, estWidth, nbrOfGauss, stripMin+i, threshold, upperLimit, ignoreTail );
    }

    return myFitsResults;
}

std::map<Int_t, std::pair<TH1D*, TF1*>> FitAllStrips ( TH2F* inHist, float estWidth, unsigned short nbrOfGauss, float threshold, float upperLimit = 0, bool ignoreTail = true )
{
    auto xaxis = inHist->GetXaxis();

    return FitRangeOfStrips ( inHist, estWidth, nbrOfGauss, xaxis->GetFirst(), xaxis->GetLast(), threshold, upperLimit, ignoreTail );
}

TCanvas* DrawStripResult ( unsigned short strip )
{
    if ( myFitsResults.find ( strip ) == myFitsResults.end() )
    {
        return NULL;
    }

    TCanvas *can = new TCanvas ( Form ( "can_strip%d",strip ) );

    can->cd();
    myFitsResults[strip].first->Draw();
    myFitsResults[strip].second->Draw ( "same" );

    listOfResCanvas[strip] = can;

    return can;
}

void CloseAllResults()
{
    for ( auto itr = listOfResCanvas.begin(); itr != listOfResCanvas.end(); itr++ )
    {
        if ( itr->second != NULL )
        {
            itr->second->Close();
        }
    }
}

vector<TCanvas*> DrawAllResults()
{
    vector<TCanvas*> resCans;

    resCans.clear();

    for ( auto itr = myFitsResults.begin(); itr != myFitsResults.end(); itr++ )
    {
        resCans.push_back ( DrawStripResult ( itr->first ) );
    }

    return resCans;
}

template<typename T> T PromptMessage ( string messageToDisplay, T* inputToMod )
{
    bool inputIsValid = false;

    if ( typeid ( *inputToMod ) == typeid ( bool ) )
    {
        char userInput[256];

        while ( !inputIsValid )
        {
            inputIsValid = true;

            cout << messageToDisplay;
            std::cin >> userInput;

            bool aYes = ( strcmp ( userInput, "y" ) == 0 || strcmp ( userInput, "yes" ) == 0 );
            bool aNo = ( strcmp ( userInput, "n" ) == 0 || strcmp ( userInput, "no" ) == 0 );

            if ( aYes || aNo ) *inputToMod = aYes ? aYes : !aNo;
            else
            {
                inputIsValid = false;
                cerr << "\nInvalid input. Answer [yes/y] or [no/n]\n" << std::endl;
            }
        }
    }
    else
    {
        while ( !inputIsValid )
        {
            inputIsValid = true;

            cout << messageToDisplay;
            std::cin >> *inputToMod;

            if ( std::cin.fail() )
            {
                inputIsValid = false;

                std::cin.clear();
                std::cin.ignore();

                int status;

                cerr << "Invalid input. Please enter a " << abi::__cxa_demangle ( typeid ( *inputToMod ).name(), 0, 0, &status ) << " only..." << std::endl;
            }
        }
    }

    return *inputToMod;
}

void WriteFitResToFile ( string outPath = "./", string outFile = "FitRes", float peakEn = -10 )
{
    char userIn[256];

    bool dumpBool = false;

    cout << "You are about to write the fit results to a text file..." << std::endl;

    if ( PromptMessage ( "Do you want to change the default path (\""+ outPath + "\") ?  ", &dumpBool ) )
    {
        PromptMessage ( "Enter the new value...\n", &outPath );
    }

    cout << "File will be written there: " << outPath << std::endl;

promptFileName:
    if ( PromptMessage<bool> ( "Do you want to change the current file name (\"" + outFile + "\") ?  ", &dumpBool ) )
    {
        PromptMessage ( "Enter the new value...\n", &outFile );
    }

    string outFullName = outPath + outFile + ".txt";

    std::ofstream outStream ( outFullName.c_str() );

    if ( outStream.is_open() )
    {
        if ( peakEn > 0.0 ) outStream << std::left << std::setw ( 8 ) << "Strip#" << std::setw ( 13 ) << "Peak Pos." << std::setw ( 15 ) << "Fit Slope" << std::setw ( 14 ) << "Chi Square" << "\n\n";
        else outStream << std::left << std::setw ( 8 ) << "Strip#" << std::setw ( 13 ) << "Peak Pos." << std::setw ( 14 ) << "Chi Square" << "\n\n";

        for ( auto itr = myFitsResults.begin(); itr != myFitsResults.end(); itr++ )
        {
            Double_t centroid = 0;
            Double_t maxVal = 0;

            TF1 *fr = itr->second.second;

            unsigned short p = 2;

            while ( p < fr->GetNpar() )
            {
                Double_t amp = fr->GetParameter ( p++ );

                if ( amp > maxVal )
                {
                    maxVal = amp;
                    centroid = fr->GetParameter ( p++ );
                }
                else
                {
                    p++;
                }
            }

            if ( peakEn > 0.0 ) outStream << std::left << std::setw ( 8 ) << itr->first << std::setw ( 13 ) << centroid << std::setw ( 15 ) << ( centroid != 0 ? peakEn/centroid : 0 )  << std::setw ( 14 ) << fr->GetChisquare() << "\n";
            else outStream << std::left << std::setw ( 8 ) << itr->first << std::setw ( 13 ) << centroid << std::setw ( 14 ) << fr->GetChisquare() << "\n";
        }

        outStream.close();

        cout << "Fit results written to file: " << outFullName << std::endl;
    }
    else
    {
        cerr << "Unabled to create file " << outFullName << std::endl;
    }
}

void DisplayFailedFitsList()
{
    if ( fitsWithIssues.size() > 0 )
    {
        cerr << std::endl;
        cerr << "/!\\/!\\/!\\/!\\ WARNING -> The following fits \"failed\" /!\\/!\\/!\\/!\\" << std::endl;
        cerr << "     Please note that \"failed\" fit does not necesaarily mean \"bad\"" << std::endl;
        cerr << "                It may result from:" << std::endl;
        cerr << "                 ->a binning too small in the source 2D histogram" << std::endl;
        cerr << "                 ->a very bad estimation of the width" << std::endl;
        cerr << "                 ->unrealistic requirement on the final Chi2 or amplitude" << std::endl;
        cerr << std::endl;

        cerr << reqChi2Min << " < Required Chi2 < " << reqChi2Max << std::endl;

        for ( auto itr = fitsWithIssues.begin(); itr != fitsWithIssues.end(); itr++ )
        {
            cerr << "Strip# " << itr->first  << " (Chi2 = " << itr->second->GetChisquare() << ")" << std::endl;
        }

        cerr << std::endl;

        bool displayFaulty = false;

        if ( PromptMessage ( "Do you want to display these ones only?   ", &displayFaulty ) )
        {
            CloseAllResults();

            for ( auto itr = fitsWithIssues.begin(); itr != fitsWithIssues.end(); itr++ )
            {
                DrawStripResult ( itr->first );
            }
        }
    }
}

void DoMyJob ( TH2F* inHist, float estWidth, unsigned short nbrOfGauss, bool doDraw, bool doWrite, float peakEn, float threshold, float upperLimit = 0, bool ignoreTail = true )
{
    FitAllStrips ( inHist, estWidth, nbrOfGauss, threshold, upperLimit, ignoreTail );

    if ( doDraw ) DrawAllResults();

    if ( doWrite ) WriteFitResToFile ( "./", "FitRes", peakEn );

    DisplayFailedFitsList();
}

int MultipleChoicePrompt ( char* message, vector<string> choices, vector<string> desc = {} )
{
    char* userIn = new char[256];

    unsigned short choicesSize = choices.size();

    cout << message << " (\"q\" to abort, \"b\" to go backward)\n";

    for ( unsigned short i = 0; i < choicesSize; i++ )
    {
        cout << "[" << i << "] " << choices[i] << std::endl;
        if ( desc.size() > 0 && desc[i].length() > 0 ) cout << "     --> " << desc[i] << std::endl;
    }

    cout << std::endl;

    std::cin >> userIn;
    cout << std::endl;

    if ( strcmp ( userIn, "q" ) == 0 ) return -3;
    else if ( strcmp ( userIn, "b" ) == 0 ) return -2;
    else
    {
        for ( unsigned short i = 0; i < choicesSize; i++ )
        {
            if ( strcmp ( userIn, to_string ( i ).c_str() ) == 0 )
            {
                strcpy ( userIn, choices[i].c_str() );
                return i;
            }
        }

        cerr << "Invalid input..." << std::endl;
    }

    return -1;
}

char* GetComparatorSign ( string comparator )
{
    char* sign = new char[4];

    if ( comparator.find ( "equal" ) <= comparator.length() ) strcpy ( sign, "==" );
    else if ( comparator.find ( "greater" ) <= comparator.length() ) strcpy ( sign, ">" );
    else if ( comparator.find ( "less" ) <= comparator.length() ) strcpy ( sign, "<" );

    return sign;
}

char* GetConditionalType ( string comparator )
{
    char* sign = new char[4];

    if ( comparator.find ( "and" ) <= comparator.length() ) strcpy ( sign, "&&" );
    else if ( comparator.find ( "or" ) <= comparator.length() ) strcpy ( sign, "||" );

    return sign;

}

void DoMyJob()
{
    char userIn[256];

    float userWidth;
    unsigned short userGaussNbr = 1;
    float userThr;
    float userUpperLimit;
    float userPeakEn = -10;
    bool userDraw = false;
    bool userWrite = true;
    bool userCalib = true;
    bool userIgnore = true;

prompt2DHist:
    if ( histToTreat == NULL )
    {
        cout << "Enter the name of the 2D histogram used as a source or type \"new\" to generate a new one... (\"q\" to quit)\n";
        std::cin >> userIn;
        cout << std::endl;

        if ( strcmp ( userIn, "q" ) == 0 ) return;
        else if ( strcmp ( userIn, "new" ) == 0 )
        {
            cout << "Come back later when it's implemented" << std::endl;
            goto prompt2DHist;
        }
        else
        {
            TH2F* hist = SetHistToTreat ( ( TH2F* ) gDirectory->Get ( userIn ) );

            if ( hist == NULL )
            {
                cerr << "The 2D histogram specified does not exists!\n\n";
                goto prompt2DHist;
            }
        }
    }

    PromptMessage ( "Do you want to ignore the tails of the peaks? (tip: you should...)  ", &userIgnore );
    if ( !userIgnore )
    {
        PromptMessage ( "How many gaussians do you want to use for the fits?  ", &userGaussNbr );
    }

    PromptMessage ( "What is roughly the width of the peak OBSERVED?   ", &userWidth );

    PromptMessage ( "Threshold to start detection of the peak:  ", &userThr );

    PromptMessage ( "Upper limit to stop the detection of the peak:  ", &userUpperLimit );

    PromptMessage ( "Will you want all the fit results to be displayed?  ", &userDraw );

    PromptMessage ( "Will you want to write the fit results to a file?  ", &userWrite );

    PromptMessage ( "Do you want to get the calibration parameters as well?  ", &userWrite );
    if ( userCalib == 1 )
    {
        PromptMessage ( "What is the energy of the peak to fit?   ", &userPeakEn );
    }

    DoMyJob ( histToTreat, userWidth, userGaussNbr, userDraw, userWrite, userPeakEn, userThr, userUpperLimit, userIgnore );
}
/*
std::map<unsigned short, std::pair<float, float>> GetCalibMap ( unsigned short fstCh, string inFileName )
{
    std::map<unsigned short, std::pair<float, float>> calMap_;

    ifstream fileToRead;
    fileToRead.open ( inFileName.c_str() );

    float p0Read, p1Read;

    unsigned short ch = fstCh;

    if ( fileToRead.is_open() )
    {
        while ( !fileToRead.eof() )
        {
            fileToRead >> p0Read >> p1Read;

            cout << "Read " << ch << " ->" << p0Read << " / " << p1Read << std::endl;

            calMap_[ch] = std::make_pair<float, float> ( ( float ) p0Read, ( float ) p1Read );

            ch++;
        }
    }

    fileToRead.close();

    myCalMap = calMap_;

    return calMap_;
}

// *************************************************************************************************************** //

void GetEnHistsPerStrip ( TChain* data_, string sectorsStr = "all", string stripsStr = "all" )
{
    cout << "The following files will be treated:\n";

    data_->ls();

    vector<SiDataBase>* siData = new vector<SiDataBase>;

    data_->SetBranchAddress ( "si", &siData );

    vector<unsigned short> sectorsList, stripsList;

    if ( sectorsStr == "all" ) sectorsList = {0, 1, 2, 3};
    else sectorsList = DecodeSectorsString ( sectorsStr );


    if ( stripsStr == "all" )
    {
        for ( int i = 0; i < 32; i++ ) stripsList.push_back ( i );
    }
    else stripsList = DecodeSectorsString ( stripsStr );

    TH1F* hQQQ5En[4][32];

    string QQQ5Str = "ABCD";

    for ( int i = 0; i < 4; i++ )
    {
        for ( int j = 0; j < 32; j++ )
        {
            hQQQ5En[i][j] = new TH1F ( Form ( "hQQQ5En_U%c_s%d",QQQ5Str[i], j ), Form ( "QQQ5 U%c s%d Energy Histogram",QQQ5Str[i], j ), 500, 0, 10 );
        }
    }

    for ( long long int i = 0; i < data_->GetEntries(); i++ )
    {
        if ( i%10000 == 0 )
        {
            cout << "Treated " << std::setw ( 9 ) << i << " / " << std::setw ( 9 ) << data_->GetEntries();
            cout << " ( " << std::setw ( 5 ) << std::fixed << std::setprecision ( 2 ) << ( float ) i/data_->GetEntries() *100. << "% )\r" << std::flush;
        }

        data_->GetEntry ( i );

        for ( unsigned int j = 0; j < siData->size(); j++ )
        {
            if ( siData->at ( j ).isUpstream && !siData->at ( j ).isBarrel &&  siData->at ( j ).MultLayer ( 1, false ) == 1 )
            {
                int sector = siData->at ( j ).sector;
                int strip = siData->at ( j ).StripMaxLayer ( 1, false );

//                 if ( std::find ( sectorsList.begin(), sectorsList.end(), sector ) != sectorsList.end()
//                         && std::find ( stripsList.begin(), stripsList.end(), strip ) != stripsList.end() )
//                 {
//                     hQQQ5En[sector][strip]->Fill ( siData->at ( j ).ESumLayer ( 1, false ) );
//                 }

                hQQQ5En[sector][strip]->Fill ( siData->at ( j ).ESumLayer ( 1, false ) );
            }
        }
    }

    cout << "\n\n";

    TFile* outFile = new TFile ( "QQQ5_Energy_per_strip.root", "recreate" );

    outFile->cd();

    for ( unsigned int i = 0; i < sectorsList.size(); i++ )
    {
        for ( unsigned int j = 0; j < stripsList.size(); j++ )
        {
            hQQQ5En[sectorsList[i]][stripsList[j]]->Write();
        }
    }

    outFile->Close();

    return;
}

void GetEnHistsPerStrip ( string fName, string treeName, string sectorsStr = "all", string stripsStr = "all" )
{
    TChain* data_ = new TChain ( treeName.c_str(), treeName.c_str() );

    vector<string> listOfFiles = DecodeFilesToTreat ( fName );

    for ( unsigned int i = 0; i < listOfFiles.size(); i++ )
    {
        data_->Add ( listOfFiles[i].c_str() );
    }

    return GetEnHistsPerStrip ( data_, sectorsStr, stripsStr );
}*/

// // *************************************************************************************************************************************** //
// 
// vector<std::map<int, TH1F*>> hEn_QQQ5UA;
// vector<std::map<int, TH1F*>> hEn_QQQ5UB;
// vector<std::map<int, TH1F*>> hEn_QQQ5UC;
// vector<std::map<int, TH1F*>> hEn_QQQ5UD;
// 
// vector<std::map<int, TH1F*>> hQVal_QQQ5UA;
// vector<std::map<int, TH1F*>> hQVal_QQQ5UB;
// vector<std::map<int, TH1F*>> hQVal_QQQ5UC;
// vector<std::map<int, TH1F*>> hQVal_QQQ5UD;
// 
// TH2F* hEn_vs_strip_QQQ5UA;
// TH2F* hEn_vs_strip_QQQ5UB;
// TH2F* hEn_vs_strip_QQQ5UC;
// TH2F* hEn_vs_strip_QQQ5UD;
// 
// TH2F* hQval_vs_strip_QQQ5UA;
// TH2F* hQval_vs_strip_QQQ5UB;
// TH2F* hQval_vs_strip_QQQ5UC;
// TH2F* hQval_vs_strip_QQQ5UD;
// 
// TH2F* hEn_vs_strip_QQQ5UA_mod;
// TH2F* hEn_vs_strip_QQQ5UB_mod;
// TH2F* hEn_vs_strip_QQQ5UC_mod;
// TH2F* hEn_vs_strip_QQQ5UD_mod;
// 
// TH2F* hQval_vs_strip_QQQ5UA_mod;
// TH2F* hQval_vs_strip_QQQ5UB_mod;
// TH2F* hQval_vs_strip_QQQ5UC_mod;
// TH2F* hQval_vs_strip_QQQ5UD_mod;
// 
// const int minMod = 40;
// const int maxMod = 160;
// 
// TH1F* AddAllStrips ( vector<std::map<int, TH1F*>> hists, int modCoeff_ = 100 )
// {
//     TH1F* hSum = ( TH1F* ) hists[0][modCoeff_]->Clone();
// 
//     for ( int i = 1; i < 32; i++ )
//     {
//         hSum->Add ( hists[i][modCoeff_] );
//     }
// 
//     return hSum;
// }
// 
// TH1F* AddAllStrips ( vector<std::map<int, TH1F*>> hists, int modCoeffs[32] )
// {
//     TH1F* hSum = ( TH1F* ) hists[0][modCoeffs[0]]->Clone();
// 
//     for ( int i = 1; i < 32; i++ )
//     {
//         hSum->Add ( hists[i][modCoeffs[i]] );
//     }
// 
//     return hSum;
// }
// 
// TH1F* AddAllStrips ( vector<std::map<int, TH1F*>> hists, vector<int> modCoeffs )
// {
//     TH1F* hSum = ( TH1F* ) hists[0][modCoeffs[0]]->Clone();
// 
//     for ( int i = 1; i < 32; i++ )
//     {
//         hSum->Add ( hists[i][modCoeffs[i]] );
//     }
// 
//     return hSum;
// }
// 
// TF1* FitQValGS ( TH1F* hist, float mean, float fwhm, float minBound = 0, float maxBound = 0 )
// {
//     TF1* fitFunc = new TF1 ( "fitFunc", "[0] * TMath::Exp ( -pow ( x - [1],2 ) / pow ( 2 * [2],2 ) ) + [3] * TMath::Exp ( -pow ( x - [4],2 ) / pow ( 2 * [5],2 ) ) + [6] + [7] * x", -20, 20 );
// 
//     if ( minBound == 0 ) minBound = mean - 2 - fwhm;
//     if ( maxBound == 0 ) maxBound = mean + fwhm*3;
// 
//     fitFunc->SetParameters ( 10, mean, fwhm, 10, mean - 2, fwhm, 1, -0.1 );
// 
//     hist->Fit ( fitFunc, "Q", "", minBound, maxBound );
// 
//     return fitFunc;
// }
// 
// TH2F* GetQvalVsStrip ( vector<std::map<int, TH1F*>> src, TH2F* dest, int modCoeff_ = 100 )
// {
//     dest->Reset();
// 
//     for ( int i = 0; i < 32; i++ )
//     {
//         unsigned int nBins = src[i][modCoeff_]->GetNbinsX();
// 
//         for ( unsigned int j = 0; j < nBins; j++ )
//         {
//             dest->Fill ( i, src[i][modCoeff_]->GetBinCenter ( j ), src[i][modCoeff_]->GetBinContent ( j ) );
//         }
//     }
// 
//     return dest;
// }
// 
// TH2F* GetQvalVsStrip ( vector<std::map<int, TH1F*>> src, TH2F* dest, int modCoeffs[32] )
// {
//     dest->Reset();
// 
//     for ( int i = 0; i < 32; i++ )
//     {
//         unsigned int nBins = src[i][modCoeffs[i]]->GetNbinsX();
// 
//         for ( unsigned int j = 0; j < nBins; j++ )
//         {
//             dest->Fill ( i, src[i][modCoeffs[i]]->GetBinCenter ( j ), src[i][modCoeffs[i]]->GetBinContent ( j ) );
//         }
//     }
// 
//     return dest;
// }
// 
// TH2F* GetQvalVsStrip ( vector<std::map<int, TH1F*>> src, TH2F* dest, vector<int> modCoeffs )
// {
//     dest->Clear();
// 
//     for ( int i = 0; i < 32; i++ )
//     {
//         unsigned int nBins = src[i][modCoeffs[i]]->GetNbinsX();
// 
//         for ( unsigned int j = 0; j < nBins; j++ )
//         {
//             dest->Fill ( i, src[i][modCoeffs[i]]->GetBinCenter ( j ), src[i][modCoeffs[i]]->GetBinContent ( j ) );
//         }
//     }
// 
//     return dest;
// }
// 
// void InitSiEnergySpectra ( unsigned int nBins, int binMin, int binMax )
// {
//     for ( int i = 0; i < 32; i++ )
//     {
//         float modCoeff = minMod;
// 
//         std::map<int, TH1F*> newMapEntryA, newMapEntryB, newMapEntryC, newMapEntryD;
// 
//         while ( modCoeff <= maxMod )
//         {
//             newMapEntryA[modCoeff] = new TH1F ( Form ( "hEn_QQQ5_UA_s%d_mod%f", i, modCoeff ), Form ( "Si Energy QQQ5 UA strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
//             newMapEntryB[modCoeff] = new TH1F ( Form ( "hEn_QQQ5_UB_s%d_mod%f", i, modCoeff ), Form ( "Si Energy QQQ5 UB strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
//             newMapEntryC[modCoeff] = new TH1F ( Form ( "hEn_QQQ5_UC_s%d_mod%f", i, modCoeff ), Form ( "Si Energy QQQ5 UC strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
//             newMapEntryD[modCoeff] = new TH1F ( Form ( "hEn_QQQ5_UD_s%d_mod%f", i, modCoeff ), Form ( "Si Energy QQQ5 UD strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
// 
//             modCoeff += 1;
//         }
// 
//         hEn_QQQ5UA.push_back ( newMapEntryA );
//         hEn_QQQ5UB.push_back ( newMapEntryB );
//         hEn_QQQ5UC.push_back ( newMapEntryC );
//         hEn_QQQ5UD.push_back ( newMapEntryD );
//     }
// 
//     hEn_vs_strip_QQQ5UA = new TH2F ( "hEn_vs_strip_QQQ5UA", "Si Energy vs. Strip# QQQ5 UA", 32, 0, 32, nBins, binMin, binMax );
//     hEn_vs_strip_QQQ5UB = new TH2F ( "hEn_vs_strip_QQQ5UB", "Si Energy vs. Strip# QQQ5 UB", 32, 0, 32, nBins, binMin, binMax );
//     hEn_vs_strip_QQQ5UC = new TH2F ( "hEn_vs_strip_QQQ5UC", "Si Energy vs. Strip# QQQ5 UC", 32, 0, 32, nBins, binMin, binMax );
//     hEn_vs_strip_QQQ5UD = new TH2F ( "hEn_vs_strip_QQQ5UD", "Si Energy vs. Strip# QQQ5 UD", 32, 0, 32, nBins, binMin, binMax );
// 
//     hEn_vs_strip_QQQ5UA_mod = new TH2F ( "hEn_vs_strip_QQQ5UA_mod", "Si Energy vs. Strip# QQQ5 UA mod", 32, 0, 32, nBins, binMin, binMax );
//     hEn_vs_strip_QQQ5UB_mod = new TH2F ( "hEn_vs_strip_QQQ5UB_mod", "Si Energy vs. Strip# QQQ5 UB mod", 32, 0, 32, nBins, binMin, binMax );
//     hEn_vs_strip_QQQ5UC_mod = new TH2F ( "hEn_vs_strip_QQQ5UC_mod", "Si Energy vs. Strip# QQQ5 UC mod", 32, 0, 32, nBins, binMin, binMax );
//     hEn_vs_strip_QQQ5UD_mod = new TH2F ( "hEn_vs_strip_QQQ5UD_mod", "Si Energy vs. Strip# QQQ5 UD mod", 32, 0, 32, nBins, binMin, binMax );
// 
//     return;
// }
// 
// void InitQValSpectra ( unsigned int nBins, int binMin, int binMax )
// {
//     for ( int i = 0; i < 32; i++ )
//     {
//         float modCoeff = minMod;
// 
//         std::map<int, TH1F*> newMapEntryA, newMapEntryB, newMapEntryC, newMapEntryD;
// 
//         while ( modCoeff <= maxMod )
//         {
//             newMapEntryA[modCoeff] = new TH1F ( Form ( "hQVal_QQQ5_UA_s%d_mod%f", i, modCoeff ), Form ( "Q-Value QQQ5 UA strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
//             newMapEntryB[modCoeff] = new TH1F ( Form ( "hQVal_QQQ5_UB_s%d_mod%f", i, modCoeff ), Form ( "Q-Value QQQ5 UB strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
//             newMapEntryC[modCoeff] = new TH1F ( Form ( "hQVal_QQQ5_UC_s%d_mod%f", i, modCoeff ), Form ( "Q-Value QQQ5 UC strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
//             newMapEntryD[modCoeff] = new TH1F ( Form ( "hQVal_QQQ5_UD_s%d_mod%f", i, modCoeff ), Form ( "Q-Value QQQ5 UD strip #%d mod %f", i, modCoeff ), nBins, binMin, binMax );
// 
//             modCoeff += 1;
//         }
// 
//         hQVal_QQQ5UA.push_back ( newMapEntryA );
//         hQVal_QQQ5UB.push_back ( newMapEntryB );
//         hQVal_QQQ5UC.push_back ( newMapEntryC );
//         hQVal_QQQ5UD.push_back ( newMapEntryD );
//     }
// 
//     hQval_vs_strip_QQQ5UA = new TH2F ( "hQval_vs_strip_QQQ5UA", "Q-Value vs. Strip# QQQ5 UA", 32, 0, 32, nBins, binMin, binMax );
//     hQval_vs_strip_QQQ5UB = new TH2F ( "hQval_vs_strip_QQQ5UB", "Q-Value vs. Strip# QQQ5 UB", 32, 0, 32, nBins, binMin, binMax );
//     hQval_vs_strip_QQQ5UC = new TH2F ( "hQval_vs_strip_QQQ5UC", "Q-Value vs. Strip# QQQ5 UC", 32, 0, 32, nBins, binMin, binMax );
//     hQval_vs_strip_QQQ5UD = new TH2F ( "hQval_vs_strip_QQQ5UD", "Q-Value vs. Strip# QQQ5 UD", 32, 0, 32, nBins, binMin, binMax );
// 
//     hQval_vs_strip_QQQ5UA_mod = new TH2F ( "hQval_vs_strip_QQQ5UA_mod", "Q-Value vs. Strip# QQQ5 UA mod", 32, 0, 32, nBins, binMin, binMax );
//     hQval_vs_strip_QQQ5UB_mod = new TH2F ( "hQval_vs_strip_QQQ5UB_mod", "Q-Value vs. Strip# QQQ5 UB mod", 32, 0, 32, nBins, binMin, binMax );
//     hQval_vs_strip_QQQ5UC_mod = new TH2F ( "hQval_vs_strip_QQQ5UC_mod", "Q-Value vs. Strip# QQQ5 UC mod", 32, 0, 32, nBins, binMin, binMax );
//     hQval_vs_strip_QQQ5UD_mod = new TH2F ( "hQval_vs_strip_QQQ5UD_mod", "Q-Value vs. Strip# QQQ5 UD mod", 32, 0, 32, nBins, binMin, binMax );
// 
//     return;
// }
// 
// void GenerateEnergyHistPerStrip ( TChain* chain )
// {
//     InitSiEnergySpectra ( 200, 0, 10 );
//     InitQValSpectra ( 500,-15,10 );
// 
//     vector<SiDataBase>* siData = new vector<SiDataBase>;
// 
//     chain->SetBranchAddress ( "si", &siData );
// 
//     float massBeam = 134.;
//     float beamEk = 1337;
//     float massTarget = 2.;
//     float massRecoil = 135.;
//     float massEjec = 1.;
//     float qValGsGs = 4.1;
// 
//     vector<std::map<int, TH1F*>>* hQValPerSector[4];
// 
//     hQValPerSector[0] = &hQVal_QQQ5UA;
//     hQValPerSector[1] = &hQVal_QQQ5UB;
//     hQValPerSector[2] = &hQVal_QQQ5UC;
//     hQValPerSector[3] = &hQVal_QQQ5UD;
// 
//     vector<std::map<int, TH1F*>>* hEnPerSector[4];
// 
//     hEnPerSector[0] = &hEn_QQQ5UA;
//     hEnPerSector[1] = &hEn_QQQ5UB;
//     hEnPerSector[2] = &hEn_QQQ5UC;
//     hEnPerSector[3] = &hEn_QQQ5UD;
// 
//     float angle, qval, exEn, modCoeff;
//     int sector, strip, mult;
//     double siEn, newEn, newQval;
// 
//     for ( long long int i = 0; i < chain->GetEntries(); i++ )
//     {
//         if ( i%10000 == 0 )
//         {
//             cout << "Treated " << std::setw ( 9 ) << i << " / " << std::setw ( 9 ) << chain->GetEntries();
//             cout << " ( " << std::setw ( 5 ) << std::fixed << std::setprecision ( 2 ) << ( float ) i/chain->GetEntries() *100. << "% )\r" << std::flush;
//         }
// 
//         chain->GetEntry ( i );
// 
//         for ( unsigned int j = 0; j < siData->size(); j++ )
//         {
//             angle = siData->at ( j ).Angle ( 1 );
// //             qval = siData->at ( j ).QValue ( massBeam, beamEk, massTarget, massEjec );
// 
// //             exEn = -qval + qValGsGs;
// 
//             sector = siData->at ( j ).sector;
//             strip = siData->at ( j ).StripMaxLayer ( 1, false );
//             mult = siData->at ( j ).MultLayer ( 1, false );
// 
//             siEn = siData->at ( j ).ESumLayer ( 1, false );
// 
//             if ( angle != 0 && siEn > 0 && mult == 1 )
//             {
//                 if ( siData->at ( j ).isUpstream && !siData->at ( j ).isBarrel )
//                 {
//                     modCoeff = minMod;
// 
//                     while ( modCoeff <= maxMod )
//                     {
//                         newEn = siEn* ( modCoeff/100. );
// 
//                         newQval = ( 1+massEjec/massRecoil ) * ( newEn ) - ( 1 - massBeam/massRecoil ) * ( beamEk )
//                                   - 2 * TMath::Sqrt ( massBeam*massEjec* ( newEn ) * ( beamEk ) ) / massRecoil * TMath::Cos ( angle * TMath::Pi() / 180. );
// 
//                         ( hEnPerSector[sector]->at ( strip ) ) [modCoeff]->Fill ( newEn );
// 
//                         ( hQValPerSector[sector]->at ( strip ) ) [modCoeff]->Fill ( newQval );
// 
//                         modCoeff += 1;
//                     }
//                 }
//             }
//         }
//     }
// 
//     cout << "\n";
// 
//     GetQvalVsStrip ( hQVal_QQQ5UA, hQval_vs_strip_QQQ5UA );
//     GetQvalVsStrip ( hQVal_QQQ5UB, hQval_vs_strip_QQQ5UB );
//     GetQvalVsStrip ( hQVal_QQQ5UC, hQval_vs_strip_QQQ5UC );
//     GetQvalVsStrip ( hQVal_QQQ5UD, hQval_vs_strip_QQQ5UD );
// 
//     return;
// }
// 
