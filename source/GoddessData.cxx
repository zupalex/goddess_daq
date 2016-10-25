#include "GoddessData.h"

//ROOT Headers
#include "TDirectory.h"
#include "TFile.h"
#include "TMath.h"

//GEBSort Headers
#include "gdecomp.h"

//GODDESS Headers
#include "Detector.h"
#include "QQQ5.h"
#include "superX3.h"
#include "BB10.h"
#include "LiquidScint.h"

#include <iostream>
#include <fstream>

GoddessData::GoddessData ( std::string configFilename )
{
    PARS* Pars = SortManager::sinstance()->execParams;

    config = new GoddessConfig ( "goddess.position", configFilename );

    gamData = new std::vector<GamData>;
    siData = new std::vector<SiDataBase>;
    siDataD = new std::vector<SiDataDetailed>;
    ionData = new std::vector<IonData>;

    if ( Pars->noMapping )
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

    if ( !Pars->noHists ) f->cd ( "/trees" );
    else f->cd ();

    if ( Pars->noCalib >= 0 )
    {
        std::string treeName = "god";
        std::string treeTitle = "GODDESS Tree";

        if ( Pars->noCalib == 1 )
        {
            treeName = "sorted";
            treeTitle = "GODDESS Sorted not Calibrated Tree";
        }

        tree = new TTree ( treeName.c_str(), treeTitle.c_str() );
        tree->Branch ( "timestamp", &firstTimestamp );
        tree->Branch ( "gam", &gamData );

        if ( Pars->siDetailLvl == 2 || Pars->noCalib == 1 ) tree->Branch ( "si", &siDataD, 32000, 0 );
        else if ( Pars->siDetailLvl == 1 ) tree->Branch ( "si", &siData, 32000, 0 );

        tree->Branch ( "ic", &ionData );
    }

    if ( Pars->noCalib == 2 )
    {
        gamData_snc = new std::vector<GamData>;
        siData_snc = new std::vector<SiDataDetailed>;
        ionData_snc = new std::vector<IonData>;

        sortedTree = new TTree ( "sorted", "GODDESS Sorted not Calibrated Tree" );
        sortedTree->Branch ( "timestamp", &firstTimestamp );
        sortedTree->Branch ( "gam", &gamData_snc );
        if ( Pars->siDetailLvl > 0 ) sortedTree->Branch ( "si", &siData_snc, 32000, 0 );
        sortedTree->Branch ( "ic", &ionData_snc );
    }

    if ( Pars->noMapping )
    {
        rawTree = new TTree ( "raw", "GODDESS Raw Tree" );
        rawTree->Branch ( "timestamp", &firstTimestamp );
        rawTree->Branch ( "si", &orrubaRaw );
        rawTree->Branch ( "gam", &gsRaw );
    }

    if ( !Pars->noHists )
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
//     PARS* Pars = SortManager::sinstance()->execParams;

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
        BB10RawEn.emplace ( name, new TH2F ( Form ( "BB10RawEn_%s", name.c_str() ), Form ( "Raw BB10 %s energy per strip;Energy [Ch];Channel", name.c_str() ), BB10_Enbins, 0, maxRawEn, 8, 0, 8 ) );
        BB10CalEn.emplace ( name, new TH2F ( Form ( "BB10CalEn_%s", name.c_str() ), Form ( "Cal BB10 %s energy per strip;Energy [Ch];Channel", name.c_str() ), BB10_Enbins, 0, 15, 8, 0, 8 ) );

    }
}

void GoddessData::InitQQQ5Hists()
{
//     PARS* Pars = SortManager::sinstance()->execParams;

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

        QQQenRawFront.emplace ( name, new TH2F ( Form ( "QQQenRawFront_%s", name.c_str() ), Form ( "Raw QQQ5 %s energy per front strip;Energy [Ch];Channel", name.c_str() ), qqq5_Enbins, 0, maxRawEn, 32, 0, 32 ) );

        QQQenCalFront.emplace ( name, new TH2F ( Form ( "QQQenCalFront_%s", name.c_str() ), Form ( "Cal QQQ5 %s energy per front strip;Energy [MeV];Channel", name.c_str() ), qqq5_Enbins, 0, 15, 32, 0, 32 ) );

        QQQenRawBack.emplace ( name, new TH2F ( Form ( "QQQenRawBack_%s", name.c_str() ), Form ( "Raw QQQ5 %s energy per back strip;Energy [Ch];Channel", name.c_str() ), qqq5_Enbins, 0, maxRawEn, 4, 0, 4 ) );

        QQQenCalBack.emplace ( name, new TH2F ( Form ( "QQQenCalBack_%s", name.c_str() ), Form ( "Cal QQQ5 %s energy per back strip;Energy [MeV];Channel", name.c_str() ), qqq5_Enbins, 0, 15, 4, 0, 4 ) );

        dirDet->cd();
        gDirectory->mkdir ( "mult" )->cd();

        QQQHitPat.emplace ( name, new TH2F ( Form ( "QQQHitPat_%s", name.c_str() ), Form ( "QQQ5 Hit Pattern %s;Front [strip];Back [strip]", name.c_str() ), 32, 0, 32, 4, 0, 4 ) );
        QQQFrontMult.emplace ( name, new TH1F ( Form ( "QQQFrontMult_%s", name.c_str() ), Form ( "QQQ5 %s multiplicity;multiplicity", name.c_str() ), 20, 0, 20 ) );
        QQQBackMult.emplace ( name, new TH1F ( Form ( "QQQBackMult_%s", name.c_str() ), Form ( "QQQ5 %s multiplicitymultiplicity", name.c_str() ), 20, 0, 20 ) );
    }

}
void GoddessData::InitSuperX3Hists()
{
//     PARS* Pars = SortManager::sinstance()->execParams;

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

        sX3stripEnRaw[name] = new TH2F ( Form ( "sX3stripEnRaw%s", name.c_str() ),
                                         Form ( "SuperX3 strip raw energy vs strip %s;energy [ch];strip", name.c_str() ), sx3_Enbins, 0, maxRawEn, 8, 0, 8 );
        sX3stripEnCal[name] = new TH2F ( Form ( "sX3stripEnCal%s", name.c_str() ),
                                         Form ( "SuperX3 strip cal energy vs strip %s;energy [keV];strip", name.c_str() ), sx3_Enbins, 0, maxRawEn, 8, 0, 8 );
        sX3backEnRaw[name] = new TH2F ( Form ( "sX3backEnRaw%s", name.c_str() ),
                                        Form ( "SuperX3 back raw energy vs strip %s;energy [ch];strip", name.c_str() ), sx3_Enbins, 0, maxRawEn, 4, 0, 4 );
        sX3backEnCal[name] = new TH2F ( Form ( "sX3backEnCal%s", name.c_str() ),
                                        Form ( "SuperX3 back cal energy vs strip %s;energy [keV];strip", name.c_str() ), sx3_Enbins, 0, 15, 4, 0, 4 );
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
            sX3posRaw_enCal[name][strip] = new TH2F ( Form ( "sX3posRaw_enCal_%s_%i", name.c_str(), strip ),
                    Form ( "super X3 raw position vs cal energy %s_%i", name.c_str(), strip ), 512, -1, 1, 1024, 0, maxRawEn );

            sX3posCal_enCal[name][strip] = new TH2F ( Form ( "sX3posCal_enCal_%s_%i", name.c_str(), strip ),
                    Form ( "super X3 cal position vs cal energy %s_%i", name.c_str(), strip ), 512, -1, 1, 1024, 0, maxRawEn );

            sX3posRaw_enRaw[name][strip] = new TH2F ( Form ( "sX3posRaw_enRaw_%s_%i", name.c_str(), strip ),
                    Form ( "super X3 raw position vs raw energy %s_%i", name.c_str(), strip ), 512, -1, 1, 1024, 0, maxRawEn );

            sX3nearFar[name][strip] = new TH2F ( Form ( "sX3nearFar_%s_%i", name.c_str(), strip ),
                                                 Form ( "sX3 near strip vs far strip %s %i", name.c_str(), strip ), 1024, 0, maxRawEn, 1024, 0, maxRawEn );
            sX3nearFarCal[name][strip] = new TH2F ( Form ( "sX3nearFarCal_%s_%i", name.c_str(), strip ),
                                                    Form ( "sX3 near strip vs far calibrated strip %s %i", name.c_str(), strip ), 1024, 0, maxRawEn, 1024, 0, maxRawEn );
        }

        dirDet->cd();
        gDirectory->mkdir ( "mult" )->cd();

        sX3HitPat[name] = new TH2F ( Form ( "sX3HitPat_%s", name.c_str() ),
                                     Form ( "SuperX3 Hit Pattern %s;Front [strip];Back [strip]", name.c_str() ), 8, 0, 8, 4, 0, 4 );

        sX3frontMult[name] = new TH1F ( Form ( "sX3FrontMult_%s", name.c_str() ),
                                        Form ( "super X3 %s multiplicity;multiplicity", name.c_str() ), 20, 0, 20 );

        sX3backMult[name] = new TH1F ( Form ( "sX3BackMult_%s", name.c_str() ),
                                       Form ( "super X3 %s multiplicity;multiplicity", name.c_str() ), 20, 0, 20 );

    }

}
void GoddessData::InitLiquidScintHists()
{
//     PARS* Pars = SortManager::sinstance()->execParams;

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
        hGamVsLiqNeuGate[name] = new TH2F ( Form ( "hGamVsLiqNeuGate_%s", name.c_str() ), Form ( "Gammas vs Liquid E, Neutron Gated %s;Energy [ch]", name.c_str() ), 512, 0, 4096, 500, 0, 4000 );

    }
}

void GoddessData::InitGammaHists()
{
//     PARS* Pars = SortManager::sinstance()->execParams;

    TDirectory* dirDet = gDirectory->mkdir ( "gamma" );
    dirDet->cd();
    gDirectory->mkdir ( "gamma" )->cd();

    upstreamGam = new TH1F ( "upstreamGam", "gammas gated on upstream particles", 4096, 0, 4096 );
    downstreamGam = new TH1F ( "downstreamGam", "gammas gated on downstream particles", 4096, 0, 4096 );
}

int GoddessData::Fill ( GEB_EVENT* gebEvt, std::vector<DGSEVENT>* dgsEvts, std::vector<DFMAEVENT>* dgodEvts, std::vector<AGODEVENT>* agodEvts )
{
    PARS* Pars = SortManager::sinstance()->execParams;

    //Map of channels to suppress, This occurs if they were not found in the map.
    static std::map<std::pair<short, short>, bool> suppressCh;

    //Loop over GEB_event and get the lowest timestmap
    firstTimestamp = gebEvt->ptgd[0]->timestamp;

    for ( unsigned int i = 1; i < gebEvt->ptgd.size(); i++ )
    {
        if ( gebEvt->ptgd[i]->timestamp < firstTimestamp )
        {
            firstTimestamp = gebEvt->ptgd[i]->timestamp;
        }
    }

    if ( Pars->noMapping )
    {
        orrubaRaw->Clear();
        gsRaw->clear();
    }

    if ( !Pars->noHists ) analogMult->Fill ( agodEvts->size() );

    // getting data from analog events
    for ( size_t i = 0; i < agodEvts->size(); i++ )
    {
        AGODEVENT agodEvt = agodEvts->at ( i );

        if ( !Pars->noHists ) analogADCMult->Fill ( agodEvt.values.size() );

        unsigned long long int timestamp = agodEvt.timestamp;

        for ( size_t j = 0; j < agodEvt.values.size(); j++ )
        {
            unsigned long int value = agodEvt.values[j];
            unsigned short channel = agodEvt.channels[j];
            DAQchannel = channel;
            DAQCh_Energy[channel] = value;

            if ( Pars->noMapping )
            {
                orrubaRaw->isDigital.push_back ( false );

                ChValPair newChValPair;

                newChValPair.channel = channel;
                newChValPair.value = value;

                orrubaRaw->data.push_back ( newChValPair );
            }

            //unsigned long long timestamp = agodEvt.timestamp;

            if ( !Pars->noHists ) enRawA->Fill ( value, channel );

            std::pair<short, short> key = std::make_pair ( GEB_TYPE_AGOD, channel );
            if ( suppressCh.find ( key ) != suppressCh.end() )
            {
                continue;
            }

            Detector* det = config->SetRawValue ( GEB_TYPE_AGOD, channel, value, Pars->ignoreThresholds, timestamp );

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
                posID = "ion";
                ionChamber = ionChamber_;
            }

            firedDets[posID] = det;
        }
    }

    if ( !Pars->noHists ) digitalMult->Fill ( dgodEvts->size() );
    // getting data from digital events
    for ( size_t i = 0; i < dgodEvts->size(); i++ )
    {
        DFMAEVENT dgodEvt = dgodEvts->at ( i );
        unsigned int value = dgodEvt.ehi;
        unsigned short channel = dgodEvt.tid;
        unsigned long long timestamp = dgodEvt.LEDts;

        DAQchannel = channel;
        //DAQCh_Energy[channel] = value; //filling this will overwrite the analog
        if ( !Pars->noHists ) enRawD->Fill ( value, channel );

        if ( Pars->noMapping )
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

        Detector* det = config->SetRawValue ( GEB_TYPE_DFMA, channel, value, Pars->ignoreThresholds, timestamp );

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

    //FillHists(dgsEvts);

    int userFilterFlag = 0;

    if ( Pars->noCalib != -1 )
    {
        userFilterFlag = FillTrees ( dgsEvts/*,dgodEvts,agodEvts*/ );
    }

    if ( Pars->noMapping )
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
//     PARS* Pars = SortManager::sinstance()->execParams;

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
                    sX3posRaw_enRaw[detPosID][i]->Fill ( ( ( frontRawEn[far] - frontRawEn[near] ) / ( frontRawEn[far] + frontRawEn[near] ) ) , ( frontRawEn[far] + frontRawEn[near] ) );
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
                    sX3posRaw_enCal[detPosID][i]->Fill ( sx3->GetStripPosRaw() [i], sx3->GetStripEnergies() [i] );
                    sX3posCal_enCal[detPosID][i]->Fill ( sx3->GetStripPosCal() [i], sx3->GetStripEnergies() [i] );
                    sX3nearFarCal[detPosID][i]->Fill ( sx3->GetNearCalEnergy() [i], sx3->GetFarCalEnergy() [i] );
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

    //Fill the detector and position multiplicty histograms.
    detMult->Fill ( numDetsOverThresh );
    hDetPosMult->Fill ( numSectorHits.size() );
}

int GoddessData::FillTrees ( std::vector<DGSEVENT>* dgsEvts/*, std::vector<DFMAEVENT> *dgodEvts, std::vector<AGODEVENT> *agodEvts*/ )
{
    PARS* Pars = SortManager::sinstance()->execParams;

    //Reminder: Pars->noCalib == ...
    //                          0 writes just the sorted and calibrated tree.
    //                          1 writes just the sorted but non calibrated tree.
    //                          2 writes both the sorted calibrated and non calibrated trees.
    //The following line declares and defines an int used to loop...
    // ->twice if we need to fill 2 different trees (Pars->noCalib == 2) or
    // ->just once if we just want to get one tree in the end (Pars->noCalib == 0 or 1)
    int noCalType = Pars->noCalib == 2 ? 2 : 1;

    for ( int nc = 0; nc < noCalType; nc++ )
    {
        ///A map of SiData based on the position in the barrel.
        std::map<std::string, unsigned short> siMap;

        siMap.clear();

        bool writeDetails = false;

        if ( Pars->siDetailLvl == 2 || Pars->noCalib == 1  || ( Pars->noCalib == 2 && nc == 1 ) )
        {
            writeDetails = true;
        }

        if ( Pars->siDetailLvl > 0 )
        {
            for ( auto detItr = siDets.begin(); detItr != siDets.end(); ++detItr )
            {
                orrubaDet* det = detItr->second;

                //Skip detectors with no contacts above threshold.
                if ( ( Pars->noCalib == 0 || ( Pars->noCalib > 0 && Pars->ignoreThresholds == 0 ) ) && det->GetContactMult() == 0 )
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
                            datum = & ( siDataD->at ( siDataD->size()-1 ) );
                            siMap[sectorStr] = siDataD->size()-1;
                        }
                        else
                        {
                            siData_snc->push_back ( newSiData );
                            datum = & ( siData_snc->at ( siData_snc->size()-1 ) );
                            siMap[sectorStr] = siData_snc->size()-1;
                        }
                    }
                    else
                    {
                        SiDataBase newSiData;
                        siData->push_back ( newSiData );
                        datum = & ( siData->at ( siData->size()-1 ) );
                        siMap[sectorStr] = siData->size()-1;
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
                }

                //Retreive the <strip, energy> map for the front and back side. Get the raw map or calibrated map according to how GEBSort was run.
                siDet::ValueMap enPMap, enNMap;

                //Pars->noCalib + nc will range from 0 to 3 since nc can only be 0 if Pars->noCalib is 0 or 1, and can be 0 or 1 if Pars->noCalib is 2
                if ( ( Pars->noCalib + nc ) % 2 == 0 )
                {
                    enPMap = det->GetCalEn ( false );
                    enNMap = det->GetCalEn ( true );
                }
                else
                {
                    enPMap = det->GetRawEn ( false );
                    enNMap = det->GetRawEn ( true );
                }

                siDet::TimeMap tsPMap, tsNMap;

                tsPMap = det->GetTsMap ( false );
                tsNMap = det->GetTsMap ( true );

                if ( enPMap.size() > 0 )
                {
                    float enMax = 0.0;
                    std::vector<int> alreadyTreatedStrips;
                    alreadyTreatedStrips.clear();

                    for ( auto stripItr = enPMap.begin(); stripItr != enPMap.end(); ++stripItr )
                    {
                        if ( ! ( detType == "superX3" && det->GetDepth() == 1 ) )
                        {
                            multP++;

                            if ( writeDetails )
                            {
                                stripP->push_back ( stripItr->first );
                                eP->push_back ( stripItr->second );
//                                 tsP->push_back ( tsPMap[stripItr->first] );
                                tsP->push_back ( 0 );
                            }

                            if ( ( Pars->noCalib + nc ) % 2 == 0 )
                            {
                                eSumP += stripItr->second;

                                if ( stripItr->second > enMax )
                                {
                                    stripMaxP = stripItr->first;
                                    enMax = stripItr->second;
                                    tsMaxP = tsPMap[stripItr->first];
                                }
                            }
                        }
                        else
                        {
                            int st_ = superX3::GetStrip ( stripItr->first );

                            if ( std::find ( alreadyTreatedStrips.begin(), alreadyTreatedStrips.end(), st_ ) != alreadyTreatedStrips.end() ) continue;

                            multP++;

                            alreadyTreatedStrips.push_back ( st_ );

                            int nearStrip = superX3::GetNearContact ( st_ );
                            int farStrip = superX3::GetFarContact ( st_ );

                            float en_near = 0.0;
                            float en_far = 0.0;

                            auto nearItr = enPMap.find ( nearStrip );
                            auto farItr = enPMap.find ( farStrip );

                            en_near = ( nearItr != enPMap.end() ) ? nearItr->second : 0.0;
                            en_far = ( farItr != enPMap.end() ) ? farItr->second : 0.0;

                            if ( writeDetails )
                            {
                                eP->push_back ( en_near );
//                                 tsP->push_back ( tsPMap[nearStrip] );
                                stripP->push_back ( st_ );

                                eN->push_back ( en_far );
//                                 tsN->push_back ( tsPMap[farStrip] );
                                stripN->push_back ( -1 );
                            }

                            float en_ = 0.0;

                            if ( en_near > 0.0 && en_far > 0.0 ) en_ = en_near + en_far;

                            if ( en_ > 0.0 )
                            {
                                if ( ( Pars->noCalib + nc ) % 2 == 0 )
                                {
                                    std::vector<float>* resStripParCal = ( ( superX3* ) det )->GetResStripParCal();

                                    enear_tot += en_near;
                                    efar_tot += en_far;

                                    eSumP += ( en_ - resStripParCal[st_].at ( 0 ) ) * resStripParCal[st_].at ( 1 );

                                    if ( en_ > enMax )
                                    {
                                        stripMaxP = st_;
                                        enMax = en_;
                                        tsMaxP = ( tsPMap[nearStrip] + tsPMap[farStrip] ) / 2;
                                    }
                                }
                            }
                        }
                    }
                }

                if ( enNMap.size() > 0 )
                {
                    float enMax = 0.0;

                    //Get the strips which fired and the energy deposites in each of them for the front side
                    for ( auto stripItr = enNMap.begin(); stripItr != enNMap.end(); ++stripItr )
                    {
                        multN++;

                        if ( writeDetails )
                        {
                            stripN->push_back ( stripItr->first );
                            eN->push_back ( stripItr->second );
//                             tsN->push_back ( tsNMap[stripItr->first] );
                        }

                        if ( ( Pars->noCalib + nc ) % 2 == 0 )
                        {
                            eSumN += stripItr->second;

                            if ( stripItr->second > enMax )
                            {
                                stripMaxN = stripItr->first;
                                enMax = stripItr->second;
                                tsMaxN = tsNMap[stripItr->first];
                            }
                        }
                    }
                }

//                 if ( *eSumP != 0.0 )
                if ( eSumP > 0 )
                {
                    datum->eSum.push_back ( eSumP );
                    datum->stripMax.push_back ( stripMaxP + 100*det->GetDepth() );
                    datum->timestampMax.push_back ( tsMaxP );
                    datum->mult.push_back ( multP );
                }

//                 if ( *eSumN != 0.0 )
                if ( eSumN > 0 )
                {
                    datum->eSum.push_back ( eSumN );
                    datum->stripMax.push_back ( stripMaxN + 100*det->GetDepth() + 300 );
                    datum->timestampMax.push_back ( tsMaxN );
                    datum->mult.push_back ( multN );
                }

                if ( stripMaxP >= 0 )
                {
                    datum->pos.push_back ( det->GetEventPosition ( stripMaxP, stripMaxN, enear_tot, efar_tot ) );
                }
            }
        }
    }

    //Loop over the DGS events
    for ( unsigned int dgsEvtNum = 0; dgsEvtNum < ( dgsEvts->size() ); dgsEvtNum++ )
    {
        // For the moment I do not want to fill the tree with gamma if there was nothing in ORRUBA

        GamData datum;

        datum.type = dgsEvts->at ( dgsEvtNum ).tpe;
        datum.num = dgsEvts->at ( dgsEvtNum ).tid;

        datum.time = dgsEvts->at ( dgsEvtNum ).event_timestamp;

        switch ( Pars->noCalib )
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

        if ( Pars->noMapping )
        {
            GSRawData rawDatum;
            rawDatum.type = dgsEvts->at ( dgsEvtNum ).tpe;
            rawDatum.num = dgsEvts->at ( dgsEvtNum ).tid;
            rawDatum.post_rise_energy = dgsEvts->at ( dgsEvtNum ).post_rise_energy;
            rawDatum.pre_rise_energy = dgsEvts->at ( dgsEvtNum ).pre_rise_energy;

            gsRaw->push_back ( rawDatum );
        }
    }

    //Deal with the ion chamber
    if ( firedDets.find ( "ion" ) != firedDets.end() )
    {
        IonData datum;
        datum.dE = ionChamber->GetAnodeDE();
        datum.resE = ionChamber->GetAnodeResE();
        datum.E = ionChamber->GetAnodeE();

        ionData->push_back ( datum );

        if ( Pars->noCalib == 2 )
        {
            ionData_snc->push_back ( datum );
        }
    }

    //Deal with the neutron detectors
    for ( auto lsItr = liquidScints.begin(); lsItr != liquidScints.end(); ++lsItr )
    {
        //UNCOMMENT THIS OR DO SOMETHING HERE WHEN WE WANT TO USE IT
        //LiquidScint *liqDet = lsItr->second;
    }

    //if (!gamData->empty() && !siData->empty()) tree->Fill();
    //std::cout << ionData->size() << '\n';

    SortManager::sinstance()->SetGamDets ( gamData );
    SortManager::sinstance()->SetSiDets ( siData );
    SortManager::sinstance()->SetIonChamber ( ionData );

    int uff = ( !Pars->userFilter.empty() ? SortManager::sinstance()->GetWriteEventFlag() : 1 );

    tree->Fill();

    if ( Pars->noCalib == 2 )
    {
        sortedTree->Fill();
    }

    gamData->clear();
    siData->clear();
    siDataD->clear();
    ionData->clear();

    if ( Pars->noCalib == 2 )
    {
        gamData_snc->clear();
        siData_snc->clear();
        ionData_snc->clear();
    }

    return uff;
}















