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


map<string, TH1F*> nSideCalGraphs;
map<string, TH2F*> nSideCalGraphs2D;

void GetBacksideEnCalPlots ( TTree* tree, long long int nentries = 0, long long firstEv = 0 )
{
    string qqq5Aliases = "ABCD";

    for ( int sect = 0; sect < 4; sect++ )
    {
        for ( int strip = 0; strip < 4; strip++ )
        {
            string hkey = Form ( "QQQ5 U%c n%d", qqq5Aliases[sect], strip );
            string hname = Form ( "QQQ5_U%c_n%d", qqq5Aliases[sect], strip );

            if ( sect < 2 ) nSideCalGraphs[hkey] = new TH1F ( hname.c_str(), hkey.c_str(), 3000 , 0, 300000 );
            else nSideCalGraphs[hkey] = new TH1F ( hname.c_str(), hkey.c_str(), 1000 , 0, 10000 );
        }

        string hkey = Form ( "QQQ5 U%c n", qqq5Aliases[sect] );
        string hname = Form ( "QQQ5_U%c_n", qqq5Aliases[sect] );

        if ( sect < 2 ) nSideCalGraphs2D[hkey] = new TH2F ( hname.c_str(), hkey.c_str(), 4, 0, 4, 3000 , 0, 300000 );
        else nSideCalGraphs2D[hkey] = new TH2F ( hname.c_str(), hkey.c_str(), 4, 0, 4, 2000 , 0, 10000 );
    }

    for ( int sect = 0; sect < 12; sect++ )
    {
        for ( int strip = 0; strip < 4; strip++ )
        {
            string hkey = Form ( "SuperX3 U%d n%d", sect, strip );
            string hname = Form ( "SuperX3_U%d_n%d", sect, strip );

            if ( sect > 0 && sect < 7 ) nSideCalGraphs[hkey] = new TH1F ( hname.c_str(), hkey.c_str(), 3000 , 0, 300000 );
            else nSideCalGraphs[hkey] = new TH1F ( hname.c_str(), hkey.c_str(), 2000 , 0, 10000 );
        }

        string hkey = Form ( "SuperX3 U%d n", sect );
        string hname = Form ( "SuperX3_U%d_n", sect );

        if ( sect > 0 && sect < 7 ) nSideCalGraphs2D[hkey] = new TH2F ( hname.c_str(), hkey.c_str(), 4, 0, 4, 3000 , 0, 300000 );
        else nSideCalGraphs2D[hkey] = new TH2F ( hname.c_str(), hkey.c_str(), 4, 0, 4, 2000 , 0, 10000 );
    }

    vector<SiDataDetailed>* siDataVect = new vector<SiDataDetailed>;

    tree->SetBranchAddress ( "si", &siDataVect );

    if ( firstEv > tree->GetEntries()-1 ) return;

    if ( nentries == 0 || nentries + firstEv > tree->GetEntries() ) nentries = tree->GetEntries() - firstEv;

    for ( long long int ev = firstEv; ev < nentries; ev++ )
    {
        if ( ev%10000 == 0 )
        {
            cout << "Events treated: " << std::setw ( 9 ) << ev-firstEv << " / " << nentries << " ( " << std::fixed << std::setprecision ( 2 ) << std::setw ( 6 ) << ( float ) ( ev-firstEv ) / nentries * 100. << " % )\r" << std::flush;
        }

        tree->GetEntry ( ev );

        if ( siDataVect->size() > 0 )
        {
            for ( unsigned int i = 0; i < siDataVect->size(); i++ )
            {
                SiDataDetailed siData = siDataVect->at ( i );

                bool isUpstream = siData.isUpstream;
                bool isBarrel = siData.isBarrel;
                int sector = siData.sector;

                if ( isUpstream && siData.E1.en.n.size() > 0 )
                {
                    for ( unsigned int j = 0; j < siData.E1.en.n.size(); j++ )
                    {
                        if ( siData.E1.strip.n[j] >= 0 )
                        {
                            string hkey;

                            if ( isBarrel ) hkey = Form ( "SuperX3 U%d n", sector );
                            else hkey = Form ( "QQQ5 U%c n", qqq5Aliases[sector] );

                            nSideCalGraphs[Form ( "%s%d", hkey.c_str(), siData.E1.strip.n[j] )]->Fill ( siData.E1.en.n[j] );

                            nSideCalGraphs2D[hkey]->Fill ( siData.E1.strip.n[j], siData.E1.en.n[j] );
                        }
                    }
                }
            }
        }
    }

    cout << endl;

    return;
}

void WriteBacksideGraphsToFile ( string fileName )
{
    TFile* outf = new TFile ( fileName.c_str(), "recreate" );

    if ( !outf->IsOpen() )
    {
        cerr << "Unabled to open file " << fileName << endl;
        return;
    }

    for ( auto itr = nSideCalGraphs.begin(); itr != nSideCalGraphs.end(); itr++ )
    {
        itr->second->Write();
    }

    for ( auto itr = nSideCalGraphs2D.begin(); itr != nSideCalGraphs2D.end(); itr++ )
    {
        itr->second->Write();
    }

    outf->Close();

    return;
}

TH2F* Get2DPlotsFrom1D ( TFile* inputf, string baseName )
{
    TIterator* fItr;

    auto lOK = inputf->GetListOfKeys();

    vector<TH1F*> histsToConvert;
    histsToConvert.clear();

    if ( lOK != nullptr )
    {
        fItr = lOK->MakeIterator();

        TObject* obj = fItr->Next();

        while ( obj != nullptr )
        {
            TH1F* hist = dynamic_cast<TH1F*> ( inputf->FindObjectAny ( obj->GetName() ) );

            if ( hist != nullptr )
            {
                string histName = hist->GetName();

                if ( histName.find ( baseName.c_str() ) != string::npos )
                {
                    cout << "Found " << histName << endl;

                    histsToConvert.push_back ( ( TH1F* ) hist->Clone() );
                }
            }

            obj = fItr->Next();
        }
    }

    TH2F* converted = nullptr;

    cout << histsToConvert.size() << " histograms to combine...\n";

    if ( histsToConvert.size() > 0 )
    {
        int nBinsX = histsToConvert.size();
        int nBinsY = histsToConvert[0]->GetXaxis()->GetNbins();

        float binYMin = histsToConvert[0]->GetXaxis()->GetBinLowEdge ( histsToConvert[0]->GetXaxis()->GetFirst() );
        float binYMax = histsToConvert[0]->GetXaxis()->GetBinUpEdge ( histsToConvert[0]->GetXaxis()->GetLast() );

        converted = new TH2F ( Form ( "Energy_vs_Strip_%s", baseName.c_str() ), Form ( "Energy vs. Strip %s", baseName.c_str() ), nBinsX, 0, histsToConvert.size(), nBinsY, binYMin, binYMax );

        for ( unsigned int i = 0; i < histsToConvert.size(); i++ )
        {
            for ( int bin = 0; bin < nBinsY; bin++ )
            {
                converted->Fill ( i, histsToConvert[i]->GetBinCenter ( bin ), histsToConvert[i]->GetBinContent ( bin ) );
            }
        }
    }

    return converted;
}
