#include "GoddessData.h"

//ROOT Headers
#include "TDirectory.h"
#include "TFile.h"
#include "TMath.h"

//GODDESS Headers
#include "Detector.h"
#include "QQQ5.h"
#include "superX3.h"
#include "BB10.h"
#include "LiquidScint.h"


#include <iostream>
#include <fstream>
#include <TSystem.h>


double GoddessGraphEval ( TGraph* gr, double toEval, orrubaDet* det, float enear, float efar )
{
    superX3* sx3det = dynamic_cast<superX3*> ( det );

    if ( gr != nullptr )
    {
        double* grX = gr->GetX();
        double* grY = gr->GetY();

        int low = -1, up = -1;

        low = TMath::BinarySearch ( gr->GetN(), grX, toEval );

        if ( low == -1 ) return grY[0];
        else if ( grX[low] == toEval ) return grY[low];
        else
        {
            if ( low == gr->GetN() - 1 ) return grY[low];

            up = low + 1;

            if ( low != -1 && up != -1 )
            {
                if ( grX[low] == grX[up] ) return grY[low];
                else
                {
                    if ( TMath::Abs ( grY[up] - grY[low] ) >= 0.1 && sx3det != nullptr )
                    {
// 		      std::cerr << "/**\\__/**\\__/**\\__/**\\__/**\\__/**\\__/**\\__/**\\__/**\\" << std::endl;
//                         std::cerr << "Found a jump at relative position " << std::fixed << std::setprecision(3) << toEval;

//                         bool jumpUp = grY[up] - grY[low] <= -0.1;

                        std::string grName = gr->GetName();
                        std::size_t findStrip = grName.find_last_of ( "_" );

                        if ( findStrip != std::string::npos )
                        {
                            int stripNbr = std::stoi ( grName.substr ( findStrip + 1 ) );

//                             std::cerr << " for " << det->GetPosID() << " strip # " << stripNbr << std::endl;

                            if ( sx3det->enJumpsCorrectionGraphs[stripNbr].size() > 0 )
                            {
//                                 std::cerr << "Equation for jumps are known for this strip:\n";

                                auto itr = sx3det->enJumpsCorrectionGraphs[stripNbr].lower_bound ( toEval );

                                if ( itr != sx3det->enJumpsCorrectionGraphs[stripNbr].end() )
                                {
                                    std::pair<float, float> jumpEq = itr->second;

//                                     std::cerr << "y = " << jumpEq.first << " x + " << jumpEq.second << std::endl;
//                                     if ( efar > enear * jumpEq.first + jumpEq.second ) std::cerr << "Enear = " << enear << "   /   Efar = " << efar << std::endl;

                                    if ( efar < enear * jumpEq.first + jumpEq.second ) return grY[up];
                                    else return grY[low];
                                }
                            }
                        }
                        else return 1.;

//                         std::cerr << std::endl;
                    }
                    else return ( grY[up] + ( toEval - grX[up] ) * ( grY[low] - grY[up] ) / ( grX[low] - grX[up] ) );
                }
            }
        }
    }

    return 1.;
}

GoddessData::GoddessData ( PARS* pars_, GoddessConfig* gconf_ )
{
    pars = pars_;

    config = gconf_;

    gamData = new std::vector<GamData>;
    siData = new std::vector<SiDataBase>;
    siDataD = new std::vector<SiDataDetailed>;
    ionData = new std::vector<IonData>;
    gretdata = new std::vector<GretData>;

    if ( pars->noMapping )
    {
        gsRaw = new std::vector<GSRawData>;
        orrubaRaw = new ORRUBARawData();
    }

    gDirectory->pwd();
    TFile* f = gDirectory->GetFile();
    if ( !f )
    {
        std::cerr << "ERROR: Not in a ROOT File?\n";
        return;
    }

    f->mkdir ( "infos" );
    f->cd ( "/infos" );

    if ( config->geomInfos.size() > 0 )
    {
        GoddessGeomInfos* godGeomInf = new GoddessGeomInfos ( config->geomInfos );
        godGeomInf->Write ( "GoddessGeom" );
    }
    if ( config->reacInfos.size() > 0 )
    {
       godReacInf = new GoddessReacInfos ( config->reacInfos );
        godReacInf->targetType = config->targetType;
        godReacInf->Write ( "GoddessReac" );
    }

    f->cd ( "../" );

    if ( !pars->noHists ) f->cd ( "/trees" );
    else f->cd();

    if ( pars->noCalib >= 0 )
    {
        std::string treeName = "god";
        std::string treeTitle = "GODDESS Tree";

        if ( pars->noCalib == 1 )
        {
            treeName = "sorted";
            treeTitle = "GODDESS Sorted not Calibrated Tree";
        }

        tree = new TTree ( treeName.c_str(), treeTitle.c_str() );
        tree->Branch ( "timestamp", &firstTimestamp );
        if ( pars->GammaProcessor == 0 )
        {
            tree->Branch ( "gam", &gamData );
        }
        else if ( pars->GammaProcessor == 1 )
        {
            tree->Branch ( "gret", &gretdata );
        }
        else
        {
            cerr<<"No viable GammaProcessor chosen."<<endl;
            return;
        }

        if ( pars->siDetailLvl == 2 || pars->noCalib == 1 ) tree->Branch ( "si", &siDataD, 32000, 0 );
        else if ( pars->siDetailLvl == 1 ) tree->Branch ( "si", &siData, 32000, 0 );

        tree->Branch ( "ic", &ionData );
    }

    pars->mainTree = tree;

    if ( pars->noCalib == 2 )
    {
        if ( pars->GammaProcessor == 0 )
        {
            gamData_snc = new std::vector<GamData>;
        }
        else if ( pars->GammaProcessor == 1 )
        {
            gretdata_snc = new std::vector<GretData>;
        }
        else
        {
            cerr<<"No viable GammaProcessor chosen."<<endl;
            return;
        }
        siData_snc = new std::vector<SiDataDetailed>;
        ionData_snc = new std::vector<IonData>;

        sortedTree = new TTree ( "sorted", "GODDESS Sorted not Calibrated Tree" );
        sortedTree->Branch ( "timestamp", &firstTimestamp );
        if ( pars->GammaProcessor == 0 )
        {
            sortedTree->Branch ( "gam", &gamData_snc );
        }
        else if ( pars->GammaProcessor == 1 )
        {
            sortedTree->Branch ( "raw_gret", &gretdata_snc );
        }
        else
        {
            cerr<<"No viable GammaProcessor chosen."<<endl;
            return;
        }
        if ( pars->siDetailLvl > 0 ) sortedTree->Branch ( "si", &siData_snc, 32000, 0 );
        sortedTree->Branch ( "ic", &ionData_snc );
    }

    if ( pars->noMapping )
    {
        rawTree = new TTree ( "raw", "GODDESS Raw Tree" );
        rawTree->Branch ( "timestamp", &firstTimestamp );
        rawTree->Branch ( "si", &orrubaRaw );
        if ( pars->GammaProcessor == 0 )
        {
            rawTree->Branch ( "gam", &gsRaw );
        }
        else if ( pars->GammaProcessor == 1 )
        {
            rawTree->Branch ( "raw_gret", &gretdata );
        }
        else
        {
            cerr<<"No viable GammaProcessor chosen."<<endl;
            return;
        }
    }

    if ( !pars->noHists )
    {
        // ORRUBA histograms
        f->cd ( "/hists" );
        TDirectory* dirOrruba = gDirectory->mkdir ( "orruba" );
        dirOrruba->cd();
        TDirectory* dirRaw = gDirectory->mkdir ( "raw" );
        dirRaw->cd();
        enRawA = new TH2F ( "enRawA", "Raw Analog Energies;Energy [Ch];Channel", 4196, 0, 4096, 400, 0, 400 );
        enRawD = new TH2F ( "enRawD", "Raw Digital Energies;Energy [Ch];Channel", 4196, 0, 1E6, 400, 0, 400 );
        enCalA = new TH2F ( "enCalA", "Calibrated Analog Energies;Energy [MeV];Channel", 4196, 0, 4096, 400, 0, 400 );
        enCalD = new TH2F ( "enCalD", "Calibrated Digital Energies;Energy [MeV];Channel", 4196, 0, 4096, 400, 0, 400 );
        analogMult = new TH1F ( "analogMult", "Number of triggers in the analog system;Multiplicty;Counts / Multiplicty", 5, 0, 5 );
        analogADCMult = new TH1F ( "analogADCMult", "Number of ADCs readout per trigger;Multiplicty;Counts / Multiplicty", 400, 0, 400 );
        digitalMult = new TH1F ( "digitalMult", "Number of triggers in the digital system;Multiplicty;Counts / Multiplicty", 400, 0, 400 );
        hDetPosMult = new TH1F ( "detPosMult", "Number of sectors hit in an event;Sector Multiplicity;Counts / Multiplicity", 32, 0, 32 );
        detMult = new TH1F ( "detMult", "Number of detectors hit in an event; Detector Multiplicity;Counts / Multiplicty", 48, 0, 48 );

        dirOrruba->cd();
        endcapHitPatternUpstream = new TH2F ( "hitEndcapUp", "Upstream Endcap Hit Pattern", 16, 0, TMath::TwoPi(), 32, 0, 32 );
        endcapHitPatternDownstream = new TH2F ( "hitEndcapDown", "Downstream Endcap Hit Pattern", 16, 0, TMath::TwoPi(), 32, 0, 32 );
        sX3HitPattern = new TH2F ( "sX3HitPattern", "Cumulative SuperX3 Hit Pattern;Azimuthal Angle [deg];Z Position [mm]", 48, 0, 360, 8, -80, 80 );

        //dirOrruba->cd();
        //InitSuperX3Hists();

        //dirOrruba->cd();
        //InitQQQ5Hists();

        //dirOrruba->cd();
        //InitBB10Hists();

        //dirOrruba->cd();
        //InitGammaHists();

        //gDirectory->cd("/hists");
        //TDirectory *dirLiquidScint = gDirectory->mkdir("LiquidScint");
        //dirLiquidScint->cd();
        //InitLiquidScintHists();
    }
}

void GoddessData::InitBB10Hists()
{
    TClonesArray* bb10s = config->GetBB10s();
    int nbb10s = bb10s->GetEntries();
    TDirectory* dirBB10 = gDirectory->mkdir ( "bb10" );

    for ( int i = 0; i < nbb10s; i++ )
    {
        BB10* bb10 = ( BB10* ) bb10s->At ( i );
        std::string name = bb10->GetPosID();
        int daqType = bb10->GetDAQType();
        dirBB10->cd();
        TDirectory* dirDet = gDirectory->mkdir ( name.c_str() );
        dirDet->cd();
        gDirectory->mkdir ( "en" )->cd();

        int maxRawEn = 4096;
        int BB10_Enbins = 4096;
        if ( daqType == GEB_TYPE_DFMA )
        {
            maxRawEn = 5e5;
        }
        BB10RawEn.emplace ( name,
                            new TH2F ( Form ( "BB10RawEn_%s", name.c_str() ), Form ( "Raw BB10 %s energy per strip;Energy [Ch];Channel", name.c_str() ), BB10_Enbins, 0, maxRawEn, 8, 0, 8 ) );
        BB10CalEn.emplace ( name, new TH2F ( Form ( "BB10CalEn_%s", name.c_str() ), Form ( "Cal BB10 %s energy per strip;Energy [Ch];Channel", name.c_str() ), BB10_Enbins, 0, 15, 8, 0, 8 ) );

    }
}

void GoddessData::InitQQQ5Hists()
{
    TClonesArray* qqq5s = config->GetQQQ5s();
    int nqqq5s = qqq5s->GetEntries();
    TDirectory* dirQQQ5 = gDirectory->mkdir ( "qqq5" );

    for ( int i = 0; i < nqqq5s; i++ )
    {
        QQQ5* qqq5 = ( QQQ5* ) qqq5s->At ( i );
        std::string name = qqq5->GetPosID();
        int daqType = qqq5->GetDAQType();
        dirQQQ5->cd();
        TDirectory* dirDet = gDirectory->mkdir ( name.c_str() );
        dirDet->cd();
        gDirectory->mkdir ( "en" )->cd();

        int maxRawEn = 4096;
        int qqq5_Enbins = 4096;
        if ( daqType == GEB_TYPE_DFMA )
        {
            maxRawEn = 5e5;
        }

        QQQenRawFront.emplace ( name,
                                new TH2F ( Form ( "QQQenRawFront_%s", name.c_str() ), Form ( "Raw QQQ5 %s energy per front strip;Energy [Ch];Channel", name.c_str() ), qqq5_Enbins, 0, maxRawEn, 32, 0,
                                           32 ) );

        QQQenCalFront.emplace ( name,
                                new TH2F ( Form ( "QQQenCalFront_%s", name.c_str() ), Form ( "Cal QQQ5 %s energy per front strip;Energy [MeV];Channel", name.c_str() ), qqq5_Enbins, 0, 15, 32, 0, 32 ) );

        QQQenRawBack.emplace ( name,
                               new TH2F ( Form ( "QQQenRawBack_%s", name.c_str() ), Form ( "Raw QQQ5 %s energy per back strip;Energy [Ch];Channel", name.c_str() ), qqq5_Enbins, 0, maxRawEn, 4, 0, 4 ) );

        QQQenCalBack.emplace ( name,
                               new TH2F ( Form ( "QQQenCalBack_%s", name.c_str() ), Form ( "Cal QQQ5 %s energy per back strip;Energy [MeV];Channel", name.c_str() ), qqq5_Enbins, 0, 15, 4, 0, 4 ) );

        dirDet->cd();
        gDirectory->mkdir ( "mult" )->cd();

        QQQHitPat.emplace ( name, new TH2F ( Form ( "QQQHitPat_%s", name.c_str() ), Form ( "QQQ5 Hit Pattern %s;Front [strip];Back [strip]", name.c_str() ), 32, 0, 32, 4, 0, 4 ) );
        QQQFrontMult.emplace ( name, new TH1F ( Form ( "QQQFrontMult_%s", name.c_str() ), Form ( "QQQ5 %s multiplicity;multiplicity", name.c_str() ), 20, 0, 20 ) );
        QQQBackMult.emplace ( name, new TH1F ( Form ( "QQQBackMult_%s", name.c_str() ), Form ( "QQQ5 %s multiplicitymultiplicity", name.c_str() ), 20, 0, 20 ) );
    }

}
void GoddessData::InitSuperX3Hists()
{
    TDirectory* dirSX3 = gDirectory->mkdir ( "sx5" );

    TClonesArray* sx3s = config->GetSuperX3s();
    int nsx3s = sx3s->GetEntries();

    for ( int i = 0; i < nsx3s; i++ )
    {
        std::string name = ( ( superX3* ) sx3s->At ( i ) )->GetPosID();
        dirSX3->cd();

        superX3* sx3 = ( superX3* ) sx3s->At ( i );
        int daqType = sx3->GetDAQType();

        TDirectory* dirDet = gDirectory->mkdir ( name.c_str() );
        dirDet->cd();
        gDirectory->mkdir ( "en" )->cd();

        int maxRawEn = 4096;
        int sx3_Enbins = 4096;
        if ( daqType == GEB_TYPE_DFMA )
        {
            maxRawEn = 5e5;
        }

        sX3stripEnRaw[name] = new TH2F ( Form ( "sX3stripEnRaw%s", name.c_str() ), Form ( "SuperX3 strip raw energy vs strip %s;energy [ch];strip", name.c_str() ), sx3_Enbins, 0, maxRawEn,
                                         8, 0, 8 );
        sX3stripEnCal[name] = new TH2F ( Form ( "sX3stripEnCal%s", name.c_str() ), Form ( "SuperX3 strip cal energy vs strip %s;energy [keV];strip", name.c_str() ), sx3_Enbins, 0,
                                         maxRawEn, 8, 0, 8 );
        sX3backEnRaw[name] = new TH2F ( Form ( "sX3backEnRaw%s", name.c_str() ), Form ( "SuperX3 back raw energy vs strip %s;energy [ch];strip", name.c_str() ), sx3_Enbins, 0, maxRawEn, 4,
                                        0, 4 );
        sX3backEnCal[name] = new TH2F ( Form ( "sX3backEnCal%s", name.c_str() ), Form ( "SuperX3 back cal energy vs strip %s;energy [keV];strip", name.c_str() ), sx3_Enbins, 0, 15, 4, 0,
                                        4 );
        sx3StripTotEn[name] = new TH2F ( Form ( "sX3StripTotEn%s", name.c_str() ), Form ( "SuperX3 total energy vs strip %s;energy [ch];strip", name.c_str() ), 1024, 0, maxRawEn, 4, 0, 4 );

        dirDet->cd();
        gDirectory->mkdir ( "pos" )->cd();

        sX3nearFar[name] = new TH2F*[4];
        sX3nearFarCal[name] = new TH2F*[4];
        sX3posRaw_enRaw[name] = new TH2F*[4];
        sX3posRaw_enCal[name] = new TH2F*[4];
        sX3posCal_enCal[name] = new TH2F*[4];

        for ( int strip = 0; strip < 4; strip++ )
        {
            sX3posRaw_enCal[name][strip] = new TH2F ( Form ( "sX3posRaw_enCal_%s_%i", name.c_str(), strip ), Form ( "super X3 raw position vs cal energy %s_%i", name.c_str(), strip ), 512,
                    -1, 1, 1024, 0, maxRawEn );

            sX3posCal_enCal[name][strip] = new TH2F ( Form ( "sX3posCal_enCal_%s_%i", name.c_str(), strip ), Form ( "super X3 cal position vs cal energy %s_%i", name.c_str(), strip ), 512,
                    -1, 1, 1024, 0, maxRawEn );

            sX3posRaw_enRaw[name][strip] = new TH2F ( Form ( "sX3posRaw_enRaw_%s_%i", name.c_str(), strip ), Form ( "super X3 raw position vs raw energy %s_%i", name.c_str(), strip ), 512,
                    -1, 1, 1024, 0, maxRawEn );

            sX3nearFar[name][strip] = new TH2F ( Form ( "sX3nearFar_%s_%i", name.c_str(), strip ), Form ( "sX3 near strip vs far strip %s %i", name.c_str(), strip ), 1024, 0, maxRawEn,
                                                 1024, 0, maxRawEn );
            sX3nearFarCal[name][strip] = new TH2F ( Form ( "sX3nearFarCal_%s_%i", name.c_str(), strip ), Form ( "sX3 near strip vs far calibrated strip %s %i", name.c_str(), strip ), 1024,
                                                    0, maxRawEn, 1024, 0, maxRawEn );
        }

        dirDet->cd();
        gDirectory->mkdir ( "mult" )->cd();

        sX3HitPat[name] = new TH2F ( Form ( "sX3HitPat_%s", name.c_str() ), Form ( "SuperX3 Hit Pattern %s;Front [strip];Back [strip]", name.c_str() ), 8, 0, 8, 4, 0, 4 );

        sX3frontMult[name] = new TH1F ( Form ( "sX3FrontMult_%s", name.c_str() ), Form ( "super X3 %s multiplicity;multiplicity", name.c_str() ), 20, 0, 20 );

        sX3backMult[name] = new TH1F ( Form ( "sX3BackMult_%s", name.c_str() ), Form ( "super X3 %s multiplicity;multiplicity", name.c_str() ), 20, 0, 20 );

    }

}
void GoddessData::InitLiquidScintHists()
{
    std::vector<LiquidScint*> liquids = config->GetLiquidScints();
    int nliquids = liquids.size();

    for ( int iLiq = 0; iLiq < nliquids; iLiq++ )
    {
        std::string name = liquids[iLiq]->GetDescription();
        LiquidScint_PSD_E[name] = new TH2F ( Form ( "LiquidScint_PSD_E_%s", name.c_str() ), Form ( "Liquid Scintillator PSD vs E %s", name.c_str() ), 512, 0, 4096, 512, 0, 4096 );
        LiquidScint_psdRaw[name] = new TH1F ( Form ( "LiquidScint_PSD_%s", name.c_str() ), Form ( "Liquid Scintillator PSD %s", name.c_str() ), 4096, 0, 4096 );
        LiquidScint_enRaw[name] = new TH1F ( Form ( "LiquidScint_E_%s", name.c_str() ), Form ( "Liquid Scintillator E %s", name.c_str() ), 4096, 0, 4096 );
        LiquidScint_tacRaw[name] = new TH1F ( Form ( "LiquidScint_TAC_%s", name.c_str() ), Form ( "Liquid Scintillator TAC %s", name.c_str() ), 4096, 0, 4096 );
        hLiqRawEnNeuGate[name] = new TH1F ( Form ( "hLiqRawEnNeuGate_%s", name.c_str() ), Form ( "Liquid Scint Raw E, Neutron Gated %s;Energy [ch]", name.c_str() ), 4096, 0, 4096 );
        hLiqPSDNeuGate[name] = new TH1F ( Form ( "hLiqPSDNeuGate_%s", name.c_str() ), Form ( "Liquid Scint PSD, Neutron Gated %s;PSD [ch]", name.c_str() ), 4096, 0, 4096 );
        hLiqTACNeuGate[name] = new TH1F ( Form ( "hLiqTACNeuGate_%s", name.c_str() ), Form ( "Liquid Scint TAC, Neutron Gated %s;TAC [ch]", name.c_str() ), 4096, 0, 4096 );
        hGamNeuGate[name] = new TH1F ( Form ( "hGamNeuGate_%s", name.c_str() ), Form ( "Gammas, Neutron Gated %s;Energy [ch]", name.c_str() ), 1000, 0, 4000 );
        hGamVsLiqNeuGate[name] = new TH2F ( Form ( "hGamVsLiqNeuGate_%s", name.c_str() ), Form ( "Gammas vs Liquid E, Neutron Gated %s;Energy [ch]", name.c_str() ), 512, 0, 4096, 500, 0,
                                            4000 );

    }
}

void GoddessData::InitGammaHists()
{
    TDirectory* dirDet = gDirectory->mkdir ( "gamma" );
    dirDet->cd();
    gDirectory->mkdir ( "gamma" )->cd();

    upstreamGam = new TH1F ( "upstreamGam", "gammas gated on upstream particles", 4096, 0, 4096 );
    downstreamGam = new TH1F ( "downstreamGam", "gammas gated on downstream particles", 4096, 0, 4096 );
}

int GoddessData::Fill ( GEB_EVENT* gebEvt, std::vector<DGSEVENT>* dgsEvts, std::vector<DFMAEVENT>* dgodEvts, std::vector<AGODEVENT>* agodEvts, std::vector<GRETEVENT>* gretEvts )
{

//   cerr<<"In fill."<<endl;

    bool doCalibrate = ( pars->noCalib ) % 2 == 0;

    //Map of channels to suppress, This occurs if they were not found in the map, i.e. no detector is mapped to this channel in the config file
    static std::map<std::pair<short, short>, bool> suppressCh;

    //Loop over gebEvt and get the lowest timestmap
    firstTimestamp = gebEvt->ptgd[0]->timestamp;

    for ( unsigned int i = 1; i < gebEvt->ptgd.size(); i++ )
    {
        if ( gebEvt->ptgd[i]->timestamp < firstTimestamp )
        {
            firstTimestamp = gebEvt->ptgd[i]->timestamp;
        }
    }

    if ( pars->noMapping )
    {
        orrubaRaw->Clear();
        gsRaw->clear();
    }

    if ( !pars->noHists ) analogMult->Fill ( agodEvts->size() );

    // getting data from analog events

    for ( size_t i = 0; i < agodEvts->size(); i++ )
    {
        AGODEVENT agodEvt = agodEvts->at ( i );

        if ( !pars->noHists ) analogADCMult->Fill ( agodEvt.values.size() );

        unsigned long long int timestamp = agodEvt.timestamp;

        for ( size_t j = 0; j < agodEvt.values.size(); j++ )
        {

            unsigned long int value = agodEvt.values[j];
            unsigned short channel = agodEvt.channels[j];

            DAQchannel = channel;
            DAQCh_Energy[channel] = value;


// 	    cerr<<"Channel: "<<channel<<endl;
// 	    cerr<<"Value: "<<value<<endl;

            if ( pars->noMapping )
            {
                orrubaRaw->isDigital.push_back ( false );

                ChValPair newChValPair;

                newChValPair.channel = channel;
                newChValPair.value = value;

                orrubaRaw->data.push_back ( newChValPair );

            }

            if ( !pars->noHists ) enRawA->Fill ( value, channel );

            std::pair<short, short> key = std::make_pair ( GEB_TYPE_AGOD, channel );
            if ( suppressCh.find ( key ) != suppressCh.end() )
            {
//            	cerr << "Skipped " << GEB_TYPE_AGOD << " / " << channel << endl;
                continue;
            }

            Detector* det = config->SetRawValue ( GEB_TYPE_AGOD, channel, value, pars->ignoreThresholds, timestamp );

            if ( !det )
            {
                suppressCh[key] = true;

                continue;
            }
            //det->SetTimestamp(timestamp);

            std::string posID;
            orrubaDet* siDet = dynamic_cast<orrubaDet*> ( det );
            IonChamber* ionChamber_ = dynamic_cast<IonChamber*> ( det );
            LiquidScint* liquidScint_ = dynamic_cast<LiquidScint*> ( det );

            if ( siDet )
            {

                posID = siDet->GetPosID();

                //We only push the detector back onto the silicon stack if we haven't already added it.
                if ( siDets.find ( posID ) == siDets.end() )
                {
                    siDets[posID] = siDet;
                    //siAnalogMult++;
                }
            }
            else if ( liquidScint_ )
            {
                posID = liquidScint_->GetDescription();
                liquidScints[posID] = liquidScint_;
            }
            else if ( ionChamber_ )
            {
                if ( pars->GammaProcessor == 0 )
                {
                    posID = "ion";
                    ionChamber = ionChamber_;
                }
            }

             firedDets[posID] = det;
        }
    }

//     cerr<<"After agod fill."<<endl;

    if ( !pars->noHists ) digitalMult->Fill ( dgodEvts->size() );
    // getting data from digital events
    for ( size_t i = 0; i < dgodEvts->size(); i++ )
    {
        DFMAEVENT dgodEvt = dgodEvts->at ( i );
        unsigned int value = dgodEvt.ehi;
        unsigned short channel = dgodEvt.tid;
        unsigned long long timestamp = dgodEvt.LEDts;

        DAQchannel = channel;
        //DAQCh_Energy[channel] = value; //filling this will overwrite the analog
        if ( !pars->noHists ) enRawD->Fill ( value, channel );

        if ( pars->noMapping )
        {
            orrubaRaw->isDigital.push_back ( true );

            ChValPair newChValPair;

            newChValPair.channel = channel;
            newChValPair.value = value;

            orrubaRaw->data.push_back ( newChValPair );
        }

        std::pair<short, short> key = std::make_pair ( GEB_TYPE_DFMA, channel );

        if ( suppressCh.find ( key ) != suppressCh.end() )
        {
            continue;
        }

        Detector* det = config->SetRawValue ( GEB_TYPE_DFMA, channel, value, pars->ignoreThresholds, timestamp );

        if ( !det )
        {
            suppressCh[key] = true;
            continue;
        }

        //Take whatever the timestamp is for this channel.
        //  This is not clear that it is the best method as one detector may have various
        //  timestamps
        //det->SetTimestamp(timestamp);

        std::string posID;
        orrubaDet* siDet = dynamic_cast<orrubaDet*> ( det );
        IonChamber* ionChamber_ = dynamic_cast<IonChamber*> ( det );
        LiquidScint* liquidScint_ = dynamic_cast<LiquidScint*> ( det );

        if ( siDet )
        {
            posID = siDet->GetPosID();
            //We only push the detector back onto the silicon stack if we haven't already added it.
            if ( siDets.find ( posID ) == siDets.end() )
            {
                siDets[posID] = siDet;
                //siDigitalMult++;
            }
        }
        else if ( liquidScint_ )
        {
            posID = liquidScint_->GetDescription();
            liquidScints[posID] = liquidScint_;
        }
        else if ( ionChamber_ )
        {
            posID = "ion";
            ionChamber = ionChamber_;
        }

        firedDets[posID] = det;
    }


    for ( auto detItr = siDets.begin(); detItr != siDets.end(); detItr++ )
    {
        detItr->second->SortAndCalibrate ( doCalibrate );

    }


    int userFilterFlag = 0;

    if ( pars->noCalib != -1 )
    {
        userFilterFlag = FillTrees ( gebEvt, dgsEvts, gretEvts );
    }


    if ( pars->noMapping )
    {
        rawTree->Fill();
    }

//We clear everything here since we know what was actually fired.
    for ( auto itr = firedDets.begin(); itr != firedDets.end(); ++itr )
    {
        itr->second->Clear();
    }

    firedDets.clear();
    siDets.clear();
    liquidScints.clear();

    for ( int i = 0; i < 400; i++ )
    {
        DAQCh_Energy[i] = 0.0;
    }

    return userFilterFlag;
}

void GoddessData::FillHists ( std::vector<DGSEVENT>* dgsEvts )
{
    std::map<std::string, int> numSectorHits;

    unsigned short numDetsOverThresh = 0;
// loop over fired detectors
    for ( auto detItr = siDets.begin(); detItr != siDets.end(); ++detItr )
    {
        //Get detector pointer from iteratot
        orrubaDet* det = detItr->second;
        //Get some information about the detector.
        std::string detPosID = det->GetPosID();
        std::string detType = det->IsA()->GetName();
        //unsigned short sector = det->GetSector();

        if ( det->GetContactMult() )
        {
            numDetsOverThresh++;
        }

        //Get the maps of the raw and calibrated energies for front and back.
        // Each map has the channel that fired as the key.
        siDet::ValueMap frontRawEn = det->GetRawEn ( siDet::pType );
        siDet::ValueMap frontCalEn = det->GetCalEn ( siDet::pType );
        siDet::ValueMap backRawEn = det->GetRawEn ( siDet::nType );
        siDet::ValueMap backCalEn = det->GetCalEn ( siDet::nType );

        //Handle BB10 detectors.
        if ( detType == "BB10" )
        {
            //---Raw Energy---
            for ( auto itr = frontRawEn.begin(); itr != frontRawEn.end(); ++itr )
            {
                BB10RawEn[detPosID]->Fill ( itr->second, itr->first );
            }
            //---Cal Energy---
            for ( auto itr = frontCalEn.begin(); itr != frontCalEn.end(); ++itr )
            {
                BB10CalEn[detPosID]->Fill ( itr->second, itr->first );
            }
        }
        //Handle QQQ5 detectors.
        else if ( detType == "QQQ5" )
        {
            //---Raw Energy---
            for ( auto itr = frontRawEn.begin(); itr != frontRawEn.end(); ++itr )
            {
                QQQenRawFront[detPosID]->Fill ( itr->second, itr->first );
            }
            for ( auto itr = backRawEn.begin(); itr != backRawEn.end(); ++itr )
            {
                QQQenRawBack[detPosID]->Fill ( itr->second, itr->first );
            }

            //Lets ignore the hits with all strips below threhsold.
            if ( det->GetContactMult() == 0 )
            {
                continue;
            }

            //---Multiplicty---
            QQQFrontMult[detPosID]->Fill ( frontCalEn.size() );
            QQQBackMult[detPosID]->Fill ( backCalEn.size() );

            //---Cal Energy---
            for ( auto itr = frontCalEn.begin(); itr != frontCalEn.end(); ++itr )
            {
                QQQenCalFront[detPosID]->Fill ( itr->second, itr->first );
            }
            for ( auto itr = backCalEn.begin(); itr != backCalEn.end(); ++itr )
            {
                QQQenCalBack[detPosID]->Fill ( itr->second, itr->first );
            }

            //---Hit Patterns---
            TH2F* endcapHitPattern;
            if ( det->GetUpStream() )
            {
                endcapHitPattern = endcapHitPatternUpstream;
            }
            else
            {
                endcapHitPattern = endcapHitPatternDownstream;
            }
            for ( auto itrFront = frontCalEn.begin(); itrFront != frontCalEn.end(); ++itrFront )
            {
                for ( auto itrBack = backCalEn.begin(); itrBack != backCalEn.end(); ++itrBack )
                {
                    QQQHitPat[detPosID]->Fill ( itrFront->first, itrBack->first );
                    float angle = ( ( QQQ5* ) det )->GetAzimuthalBins() [itrBack->first] * TMath::DegToRad() + TMath::Pi() / 16;
                    endcapHitPattern->Fill ( angle, itrFront->first );
                }
            }
        } //Done with QQQ5 detectors
        //Handle Super X3 detectors
        else if ( detType == "superX3" )
        {
            superX3* sx3 = ( superX3* ) det;

            //---Raw Energy---
            for ( auto itr = frontRawEn.begin(); itr != frontRawEn.end(); ++itr )
            {
                sX3stripEnRaw[detPosID]->Fill ( itr->second, itr->first );
            }
            for ( auto itr = backRawEn.begin(); itr != backRawEn.end(); ++itr )
            {
                sX3backEnRaw[detPosID]->Fill ( itr->second, itr->first );
            }

            //---Raw Positions---
            //for loop over 8 contacts/4 strips
            for ( int i = 0; i < 4; i++ )
            {
                unsigned short near = sx3->GetNearContact ( i );
                unsigned short far = sx3->GetFarContact ( i );
                if ( ( frontRawEn.find ( near ) != frontRawEn.end() ) && ( frontRawEn.find ( far ) != frontRawEn.end() ) )
                {
                    sX3posRaw_enRaw[detPosID][i]->Fill ( ( ( frontRawEn[far] - frontRawEn[near] ) / ( frontRawEn[far] + frontRawEn[near] ) ), ( frontRawEn[far] + frontRawEn[near] ) );
                    sX3nearFar[detPosID][i]->Fill ( frontRawEn[far], frontRawEn[near] );
                    sx3StripTotEn[detPosID]->Fill ( frontRawEn[far] + frontRawEn[near], i );
                }
            }

            //Lets ignore the detectors with all strips below threhsold.
            if ( det->GetContactMult() == 0 )
            {
                continue;
            }
            sX3frontMult[detPosID]->Fill ( frontCalEn.size() );
            sX3backMult[detPosID]->Fill ( backCalEn.size() );

            //---Cal Energy---
            for ( auto itr = frontCalEn.begin(); itr != frontCalEn.end(); ++itr )
            {
                sX3stripEnCal[detPosID]->Fill ( itr->second, itr->first );
            }
            for ( auto itr = backCalEn.begin(); itr != backCalEn.end(); ++itr )
            {
                sX3backEnCal[detPosID]->Fill ( itr->second, itr->first );
            }

            //for loop over 8 contacts/4 strips
            /// -- Trying to take the values that were further calibrated in superX3 class
            /// -- and fill histograms here. All are returning zeros.
            for ( int i = 0; i < 4; i++ )
            {
                unsigned short near = sx3->GetNearContact ( i );
                unsigned short far = sx3->GetFarContact ( i );
                if ( ( frontCalEn.find ( near ) != frontCalEn.end() ) && ( frontCalEn.find ( far ) != frontCalEn.end() ) )
                {
                    sX3posRaw_enCal[detPosID][i]->Fill ( sx3->GetStripPosRaw() [i], sx3->GetResEn ( true ) [i] );
                    sX3posCal_enCal[detPosID][i]->Fill ( sx3->GetStripPosCal() [i], sx3->GetResEn ( true ) [i] );
                    sX3nearFarCal[detPosID][i]->Fill ( sx3->enNearCal[i], sx3->enFarCal[i] );
                }
            }

            numSectorHits[detPosID]++;
            // hit pattern
            for ( auto itrFront = frontCalEn.begin(); itrFront != frontCalEn.end(); ++itrFront )
            {
                for ( auto itrBack = backCalEn.begin(); itrBack != backCalEn.end(); ++itrBack )
                {
                    sX3HitPat[detPosID]->Fill ( itrFront->first, itrBack->first );
                    float angle = ( ( superX3* ) det )->GetAzimuthalCenterBins() [itrFront->first];
                    float zPos = ( ( superX3* ) det )->GetZCenterBins() [itrBack->first];
                    sX3HitPattern->Fill ( angle, zPos );
                }
            }
        } //Done with Super X3 detectors

        //Loop over the DGS events.
        for ( size_t i = 0; i < dgsEvts->size(); i++ )
        {
            //Fill a histogram based on upstream / downstream.
            if ( det->GetUpStream() )
            {
                upstreamGam->Fill ( dgsEvts->at ( i ).ehi );
            }
            else
            {
                downstreamGam->Fill ( dgsEvts->at ( i ).ehi );
            }
        }
    } //Finished looping over fired silicon detectors.

//Begin loop of liquid scintillators.
    for ( auto lsItr = liquidScints.begin(); lsItr != liquidScints.end(); ++lsItr )
    {
        std::string description = lsItr->first;
        LiquidScint* liquidScint = lsItr->second;

        float rawEnergy = liquidScint->GetRawEnergy();
        float psd_ = liquidScint->GetRawPSD();
        float tac_ = liquidScint->GetRawTAC();

        //Fill Raw properties.
        LiquidScint_PSD_E[description]->Fill ( rawEnergy, psd_ );
        LiquidScint_enRaw[description]->Fill ( rawEnergy );
        LiquidScint_psdRaw[description]->Fill ( psd_ );
        if ( tac_ != 0 )
        {
            LiquidScint_tacRaw[description]->Fill ( tac_ );
        }

        //Rough neutron gates based on PSD and Timing.
        if ( ( description == "90deg" && psd_ > 350 && tac_ < 660 ) || ( description == "downstream" && psd_ > 725 && tac_ > 2141 ) )
        {
            hLiqRawEnNeuGate[description]->Fill ( rawEnergy );
            hLiqPSDNeuGate[description]->Fill ( psd_ );
            hLiqTACNeuGate[description]->Fill ( tac_ );
            for ( size_t i = 0; i < dgsEvts->size(); i++ )
            {
                hGamNeuGate[description]->Fill ( dgsEvts->at ( i ).ehi );
                hGamVsLiqNeuGate[description]->Fill ( rawEnergy, dgsEvts->at ( i ).ehi );
            }
        }
    } //End loop over liquid scintillators.

//Fill the detector and position multiplicity histograms.
    detMult->Fill ( numDetsOverThresh );
    hDetPosMult->Fill ( numSectorHits.size() );
}

int GoddessData::FillTrees ( GEB_EVENT* gebEvt, std::vector<DGSEVENT>* dgsEvts, std::vector<GRETEVENT>* gretEvts )
{
  
//    godReacInf = new GoddessReacInfos ( config->reacInfos );
//   
//  if (godReacInf->beamA == 95 && godReacInf->beamZ == 42)
//  {
//     int dif=0;
//     int b = 0;
// 
//     for (unsigned int sievt=0; sievt<siData->size(); sievt++ )
//     {
//         for ( unsigned int gamevt = 0; gamevt<gamData->size(); gamevt++ )
//         {
// 	  dif = siData->at ( sievt ).time-gamData->at ( gamevt ).time;
//             if ( dif > -100 && dif < 50 )
//             {
// 	      
//             sinew[b]=( siData->at ( sievt ) );
//             sinewD[b]=( siDataD->at ( sievt ) );
//             newGam[b]= ( gamData->at ( gamevt ) );
// 	    
// 	    b++;
// 	    }
// 
//         }
//      }
// 
//     siData->clear();
//     siDataD->clear();
//     gamData->clear();
// 
//     siData->insert(siData->begin(),begin(sinew),&sinew[b]);
//     siDataD->insert(siDataD->begin(),begin(sinewD),&sinewD[b]);
//     gamData->insert(gamData->begin(),begin(newGam),&newGam[b]);
//  }

//   cerr<<"In FillTrees."<<endl;

//Reminder: pars->noCalib == ...
//                          0 writes just the sorted and calibrated tree.
//                          1 writes just the sorted but non calibrated tree.
//                          2 writes both the sorted calibrated and non calibrated trees.
//The following line declares and defines an int used to loop...
// ->twice if we need to fill 2 different trees (pars->noCalib == 2) or
// ->just once if we just want to get one tree in the end (pars->noCalib == 0 or 1)
    int noCalType = pars->noCalib == 2 ? 2 : 1;

    for ( int nc = 0; nc < noCalType; nc++ )
    {
        ///A map of SiData based on the position in the barrel.
        std::map<std::string, unsigned short> siMap;

        siMap.clear();

        bool writeDetails = false;

        //pars->noCalib + nc will range from 0 to 3 since nc can only be 0 if pars->noCalib is 0 or 1, and can be 0 or 1 if pars->noCalib is 2
        bool doCalibrate = ( pars->noCalib + nc ) % 2 == 0;

        if ( pars->siDetailLvl == 2 || pars->noCalib == 1 || ( pars->noCalib == 2 && nc == 1 ) )
        {
            writeDetails = true;
        }

        if ( pars->siDetailLvl > 0 )
        {
            for ( auto detItr = siDets.begin(); detItr != siDets.end(); ++detItr )
            {
                orrubaDet* det = detItr->second;

                //Skip detectors with no contacts above threshold.
                if ( ( pars->noCalib == 0 || ( pars->noCalib > 0 && pars->ignoreThresholds == 0 ) ) && det->GetContactMult ( doCalibrate ) == 0 )
                {
                    continue;
                }

                std::string detPosID = det->GetPosID();
                std::string detType = det->IsA()->GetName();

                //Get the first part of the position ID indicating the location in the barrel / end cap.
                std::string sectorStr = detPosID.substr ( 0, detPosID.find ( '_' ) );
                //Check if this position has been defined before.
                bool firstDet = false;

                SiDataBase* datum = 0;

                if ( siMap.find ( sectorStr ) == siMap.end() )
                {
                    firstDet = true;

                    if ( writeDetails )
                    {
                        writeDetails = true;
                        SiDataDetailed newSiData;


                        if ( nc == 0 )
                        {
                            siDataD->push_back ( newSiData );
                            datum = & ( siDataD->at ( siDataD->size() - 1 ) );
                            siMap[sectorStr] = siDataD->size() - 1;
                        }
                        else
                        {
                            siData_snc->push_back ( newSiData );
                            datum = & ( siData_snc->at ( siData_snc->size() - 1 ) );
                            siMap[sectorStr] = siData_snc->size() - 1;
                        }
                    }
                    else
                    {
                        SiDataBase newSiData;
                        siData->push_back ( newSiData );
                        datum = & ( siData->at ( siData->size() - 1 ) );
                        siMap[sectorStr] = siData->size() - 1;
                    }

                    datum->Clear();

                    //Set the detector location info.
                    datum->sector = det->GetSector();

                    if ( detType == "QQQ5" )
                    {
                        datum->isBarrel = false;
                    }
                    else
                    {
                        datum->isBarrel = true;
                    }

                    datum->isUpstream = det->GetUpStream();

                    //Set the telescope ID to make the selection of a specific portion of the detection system easier. See GoddessStruct.h for more info.
                    datum->telescopeID = 1000 * ( datum->isBarrel + 1 ) + 100 * ( datum->isUpstream + 1 ) + datum->sector;
                }
                else
                {
                    if ( writeDetails )
                    {
                        if ( nc == 0 ) datum = & ( siDataD->at ( siMap[sectorStr] ) );
                        else datum = & ( siData_snc->at ( siMap[sectorStr] ) );
                    }
                    else datum = & ( siData->at ( siMap[sectorStr] ) );
                }

                std::vector<float>* eP = 0;
                std::vector<float>* eN = 0;

                std::vector<int>* stripP = 0;
                std::vector<int>* stripN = 0;

                std::vector<unsigned long long int>* tsP = 0;
                std::vector<unsigned long long int>* tsN = 0;

                float eSumP = 0;
                int stripMaxP = -1;
                int multP = 0;
                unsigned long long int tsMaxP = 0;
                float eSumN = 0;
                int stripMaxN = -1;
                int multN = 0;
                unsigned long long int tsMaxN = 0;

                float enear_tot = 0.0, efar_tot = 0.0;

                if ( writeDetails )
                {
                    switch ( det->GetDepth() )
                    {
                    case 0:
                        datum->SetMemberAddress ( "dE_e_p", &eP );
                        datum->SetMemberAddress ( "dE_e_n", &eN );
                        datum->SetMemberAddress ( "dE_strip_p", &stripP );
                        datum->SetMemberAddress ( "dE_strip_n", &stripN );
                        datum->SetMemberAddress ( "dE_ts_p", &tsP );
                        datum->SetMemberAddress ( "dE_ts_n", &tsN );
                        break;
                    case 1:
                        datum->SetMemberAddress ( "E1_e_p", &eP );
                        datum->SetMemberAddress ( "E1_strip_p", &stripP );
                        datum->SetMemberAddress ( "E1_e_n", &eN );
                        datum->SetMemberAddress ( "E1_strip_n", &stripN );
                        datum->SetMemberAddress ( "E1_ts_p", &tsP );
                        datum->SetMemberAddress ( "E1_ts_n", &tsN );
                        break;
                    case 2:
                        datum->SetMemberAddress ( "E2_e_p", &eP );
                        datum->SetMemberAddress ( "E2_strip_p", &stripP );
                        datum->SetMemberAddress ( "E2_e_n", &eN );
                        datum->SetMemberAddress ( "E2_strip_n", &stripN );
                        datum->SetMemberAddress ( "E2_ts_p", &tsP );
                        datum->SetMemberAddress ( "E2_ts_n", &tsN );
                        break;
                    }

                    det->GetHitsInfo ( "front strips", stripP );
                    det->GetHitsInfo ( "back strips", stripN );

                    det->GetHitsInfo ( "back timestamps", tsN );

                    if ( ! ( detType == "superX3" && det->GetDepth() == 1 ) )
                    {
                        det->GetHitsInfo ( "front timestamps", tsP );

                        if ( doCalibrate )
                        {
                            det->GetHitsInfo ( "front energies cal", eP );
                            det->GetHitsInfo ( "back energies cal", eN );
                        }
                        else
                        {
                            det->GetHitsInfo ( "front energies raw", eP );
                            det->GetHitsInfo ( "back energies raw", eN );
                        }
                    }
                    else
                    {
                        std::vector<int> dummyStripN;

                        dummyStripN.resize ( stripP->size() );
                        std::fill ( dummyStripN.begin(), dummyStripN.end(), -1 );

                        stripN->insert ( stripN->begin(), dummyStripN.begin(), dummyStripN.end() );

                        det->GetHitsInfo ( "near timestamps", tsP );
                        std::vector<unsigned long long int> farTs;
                        det->GetHitsInfo ( "far timestamps", &farTs );

                        tsN->insert ( tsN->begin(), farTs.begin(), farTs.end() );

                        if ( doCalibrate )
                        {
                            det->GetHitsInfo ( "near energies cal", eP );
                            det->GetHitsInfo ( "far energies cal", eN );

                            std::vector<float> backEn;
                            det->GetHitsInfo ( "back energies cal", &backEn );

                            eN->insert ( eN->end(), backEn.begin(), backEn.end() );
                        }
                        else
                        {
                            det->GetHitsInfo ( "near energies raw", eP );
                            det->GetHitsInfo ( "far energies raw", eN );

                            std::vector<float> backEn;
                            det->GetHitsInfo ( "back energies raw", &backEn );

                            eN->insert ( eN->end(), backEn.begin(), backEn.end() );
                        }
                    }
                }

                eSumP = det->GetEnSum ( false, doCalibrate );
                eSumN = det->GetEnSum ( true, doCalibrate );

                det->GetMaxHitInfo ( &stripMaxP, &tsMaxP, &stripMaxN, &tsMaxN, doCalibrate );

                multP = det->GetMultiplicity ( false, doCalibrate );
                multN = det->GetMultiplicity ( true, doCalibrate );

                TVector3 eventPos ( 0, 0, 0 );

                if ( stripMaxP >= 0 )
                {
                    eventPos = det->GetEventPosition ( doCalibrate );
                    datum->pos.push_back ( eventPos );
                }

//                 if ( *eSumP != 0.0 )
                if ( eSumP > 0 )
                {
                    if ( eventPos != TVector3 ( 0, 0, 0 ) && dynamic_cast<superX3*> ( det ) != nullptr )
                    {
                        superX3* sx3det = ( superX3* ) det;
                        TGraph* shiftGr = sx3det->enShiftVsPosGraph[stripMaxP];

                        if ( shiftGr != nullptr )
                        {
                            double relPos = ( eventPos.Z() - det->GetPStripCenterPos ( stripMaxP ).Z() ) / 75.;

                            double shiftCoeff = GoddessGraphEval ( shiftGr, relPos, det, enear_tot, efar_tot );
//                             shiftCoeff = shiftGr->Eval ( ( eventPos.Z() - det->GetPStripCenterPos ( stripMaxP ).Z() ) / 75. );

                            eSumP *= shiftCoeff;
                        }
                    }

                    datum->eSum.push_back ( eSumP );
                    datum->stripMax.push_back ( stripMaxP + 100 * det->GetDepth() );
                    datum->timestampMax.push_back ( tsMaxP );
                    datum->mult.push_back ( multP );
                }

//                 if ( *eSumN != 0.0 )
                if ( eSumN > 0 )
                {
                    datum->eSum.push_back ( eSumN );
                    datum->stripMax.push_back ( stripMaxN + 100 * det->GetDepth() + 300 );
                    datum->timestampMax.push_back ( tsMaxN );
                    datum->mult.push_back ( multN );
                }
            }
        }
    }


//Loop over the DGS events
    if ( pars->GammaProcessor == 0 )
    {
        for ( unsigned int dgsEvtNum = 0; dgsEvtNum < ( dgsEvts->size() ); dgsEvtNum++ )
        {
            // For the moment I do not want to fill the tree with gamma if there was nothing in ORRUBA

            GamData datum;

            datum.type = dgsEvts->at ( dgsEvtNum ).tpe;
            datum.num = dgsEvts->at ( dgsEvtNum ).tid;

            datum.time = dgsEvts->at ( dgsEvtNum ).event_timestamp;

            switch ( pars->noCalib )
            {
            case 0:
                datum.en = dgsEvts->at ( dgsEvtNum ).ehi;
                break;
            case 1:
                datum.en = dgsEvts->at ( dgsEvtNum ).post_rise_energy - dgsEvts->at ( dgsEvtNum ).pre_rise_energy;
                break;
            case 2:
                datum.en = dgsEvts->at ( dgsEvtNum ).post_rise_energy - dgsEvts->at ( dgsEvtNum ).pre_rise_energy;
                gamData_snc->push_back ( datum );

                datum.en = dgsEvts->at ( dgsEvtNum ).ehi;
                break;
            }

            gamData->push_back ( datum );

            if ( pars->noMapping )
            {
                GSRawData rawDatum;
                rawDatum.type = dgsEvts->at ( dgsEvtNum ).tpe;
                rawDatum.num = dgsEvts->at ( dgsEvtNum ).tid;
                rawDatum.post_rise_energy = dgsEvts->at ( dgsEvtNum ).post_rise_energy;
                rawDatum.pre_rise_energy = dgsEvts->at ( dgsEvtNum ).pre_rise_energy;

                gsRaw->push_back ( rawDatum );
            }
        }
    }

    if ( pars->GammaProcessor == 1 )
    {


        for ( unsigned int gretEvtnum = 0; gretEvtnum<gretEvts->size(); gretEvtnum++ )
        {
            // For the moment I do not want to fill the tree with gamma if there was nothing in ORRUBA

            GretData datum;

            datum.crystal_num = gretEvts->at ( gretEvtnum ).quad*4 + gretEvts->at ( gretEvtnum ).crystal;


            datum.time = gretEvts->at ( gretEvtnum ).timestamp;


            datum.x = gretEvts->at ( gretEvtnum ).x;
            datum.y = gretEvts->at ( gretEvtnum ).y;
            datum.z = gretEvts->at ( gretEvtnum ).z;

            switch ( pars->noCalib )
            {
            case 0:
                datum.en = gretEvts->at ( gretEvtnum ).e;
                break;
            case 1:
                datum.en = gretEvts->at ( gretEvtnum ).raw_e;
                break;
            case 2:
                datum.en = gretEvts->at ( gretEvtnum ).raw_e;
                gretdata_snc->push_back ( datum );

                datum.en = gretEvts->at ( gretEvtnum ).e;

                break;
            }

            gretdata->push_back ( datum );

// 	    cerr<<"Gret: "<<gretdata->size()<<endl;

            if ( pars->noMapping )
            {
                GretData rawDatum;
                rawDatum.crystal_num = gretEvts->at ( gretEvtnum ).quad*4 + gretEvts->at ( gretEvtnum ).crystal;
                rawDatum.en = gretEvts->at ( gretEvtnum ).raw_e;

// 		cerr<<rawDatum.x<<" "<<rawDatum.y<<" "<<rawDatum.z<<endl;

                rawDatum.x = gretEvts->at ( gretEvtnum ).x;
                rawDatum.y = gretEvts->at ( gretEvtnum ).y;
                rawDatum.z = gretEvts->at ( gretEvtnum ).z;

                gretdata->push_back ( rawDatum );
            }
        }
    }

//     cerr<<"After Gret."<<endl;

//Deal with the ion chamber
    if ( firedDets.find ( "ion" ) != firedDets.end() )
    {
        IonData datum;
        datum.dE = ionChamber->GetAnodeDE();
        datum.resE = ionChamber->GetAnodeResE();
        datum.E = ionChamber->GetAnodeE();

        ionData->push_back ( datum );

        if ( pars->noCalib == 2 )
        {
            ionData_snc->push_back ( datum );
        }
    }

//     cerr<<"After fired dets."<<endl;

//Deal with the neutron detectors
    for ( auto lsItr = liquidScints.begin(); lsItr != liquidScints.end(); ++lsItr )
    {
        //UNCOMMENT THIS OR DO SOMETHING HERE WHEN WE WANT TO USE IT
        //LiquidScint *liqDet = lsItr->second;
    }


    int uff = ( !pars->userFilter.empty() ? GetWriteEventFlag ( gebEvt ) : 1 );


    if ( uff == 1 )
    {
        tree->Fill();


        if ( pars->noCalib == 2 ) sortedTree->Fill();
    }


    if ( pars->GammaProcessor == 0 )
    {
        gamData->clear();
    }
    siData->clear();
    siDataD->clear();
    ionData->clear();
    if ( pars->GammaProcessor == 1 )
    {
        gretdata->clear();
    }


    if ( pars->noCalib == 2 )
    {
        if ( pars->GammaProcessor == 0 )
        {
            gamData_snc->clear();
        }
        siData_snc->clear();
        ionData_snc->clear();
        if ( pars->GammaProcessor == 1 )
        {
            gretdata_snc->clear();
        }
    }


    return uff;
}



