#include "GoddessGeomUtils.h"
#include <TIterator.h>

GoddessGeomUtils* gGU = new GoddessGeomUtils();

void GoddessGeomUtils::PrintOutStripsPositions ( )
{
    cout << "QQQ5 Upstream Strips Positions:\n";

    for ( int sect = 0; sect < 4; sect++ )
    {
        cout << " ** Sector " << sect << " **\n";

        for ( int st = 0; st < 32; st++ )
        {
            cout << "   -> Strip " << st << " : ( " << orrubaStripsPos[1][0][sect][st].X() << " , " <<  orrubaStripsPos[1][0][sect][st].Y() << " , " << orrubaStripsPos[1][0][sect][st].Z() << " )\n";
        }
    }

    cout << "\n\nSuperX3 Upstream Strips Positions:\n";

    for ( int sect = 0; sect < 12; sect++ )
    {
        cout << " ** Sector " << sect << " **\n";

        for ( int st = 0; st < 4; st++ )
        {
            cout << "   -> Strip " << st << " : ( " << orrubaStripsPos[1][1][sect][st].X() << " , " <<  orrubaStripsPos[1][1][sect][st].Y() << " , " << orrubaStripsPos[1][1][sect][st].Z() << " )\n";
        }
    }

    return;
}

void GoddessGeomUtils::FillStripsPositionsArray ( float qqq5OffX, float qqq5OffY, float QQQ5OffZ, float sX3OffX, float sX3OffY, float sX3OffZ )
{
    float barrelRadius = 3.750 * 25.4; //mm
//     float halfBarrelLength = ( 4.375 - 0.7 ) * 25.4; //mm
    float sX3ActiveLength = 75.; //mm
//     float sX3NearFrame = 3.0; //mm

    TVector3 zAxis ( 0,0,1 );

    for ( int i = 0; i < 12; i++ )
    {
//         float barrelDet_spacing = 4.8;

        TVector3 barrelDet_offset ( sX3OffX, sX3OffY, sX3OffZ );

        TVector3 refSX3D_sect0 ( 0 + barrelDet_offset.X(), barrelRadius + barrelDet_offset.Y(), sX3ActiveLength/2. + barrelDet_offset.Z() );

        float SX3_width = 40.3; //mm
//         float SX3_length = 75.; //mm

        for ( int j = 0; j < 4; j++ )
        {
            orrubaStripsPos[0][1][i][j].SetXYZ ( 0,0,1 );
            orrubaStripsPos[1][1][i][j].SetXYZ ( 0,0,1 );

            orrubaStripsPos[0][1][i][j].SetTheta ( refSX3D_sect0.Angle ( zAxis ) );
            orrubaStripsPos[1][1][i][j].SetTheta ( TMath::Pi() - refSX3D_sect0.Angle ( zAxis ) );

            orrubaStripsPos[0][1][i][j].SetPhi ( TMath::PiOver2() + i / 12. * TMath::TwoPi() );
            orrubaStripsPos[1][1][i][j].SetPhi ( TMath::PiOver2() + i / 12. * TMath::TwoPi() );

            orrubaStripsPos[0][1][i][j].SetMag ( refSX3D_sect0.Mag() );
            orrubaStripsPos[1][1][i][j].SetMag ( refSX3D_sect0.Mag() );

            orrubaStripsPos[0][1][i][j].SetRotationZ ( i / 12. * TMath::TwoPi() );
            orrubaStripsPos[1][1][i][j].SetRotationZ ( i / 12. * TMath::TwoPi() );

            TVector3 pStPosRefDetCenter ( ( ( 3./8. ) * SX3_width ) - ( j * SX3_width/4. ), 0, 0 ); // Ref taken at the center of the SX3 so strip 0 offset is 1 and a half strip width toward positive X direction

            pStPosRefDetCenter.SetPhi ( pStPosRefDetCenter.Phi() + orrubaStripsPos[0][1][i][j].RotZ() );

            orrubaStripsPos[0][1][i][j] += pStPosRefDetCenter;
            orrubaStripsPos[1][1][i][j] += pStPosRefDetCenter;
        }
    }

    for ( int i = 0; i < 4; i++ )
    {
//         float QQQ5_spacing = 4.0;

        TVector3 QQQ5DA_orig_offset ( qqq5OffX, qqq5OffY, QQQ5OffZ ); // everything in mm

        TVector3 refQQQ5D_sectA = QQQ5DA_orig_offset;

//         float QQQ5_active_length = 56.8; // mm

        float firstStripWidth = 2.55;

        TVector3 firstStripOffset ( 0, 25.2 + firstStripWidth/2., 0 ); // everything in mm

        TVector3 prevStripRefDetCenter = firstStripOffset;

        orrubaStripsPos[0][0][i][0] += firstStripOffset;
        orrubaStripsPos[1][0][i][0] += firstStripOffset;

        for ( int j = 1; j < 32; j++ )
        {
            orrubaStripsPos[0][0][i][j].SetXYZ ( 0,0,1 );
            orrubaStripsPos[1][0][i][j].SetXYZ ( 0,0,1 );

            orrubaStripsPos[0][0][i][j].SetTheta ( refQQQ5D_sectA.Angle ( zAxis ) );
            orrubaStripsPos[1][0][i][j].SetTheta ( TMath::Pi() - refQQQ5D_sectA.Angle ( zAxis ) );

            orrubaStripsPos[0][0][i][j].SetPhi ( TMath::PiOver2() + i * TMath::PiOver2() );
            orrubaStripsPos[1][0][i][j].SetPhi ( TMath::PiOver2() + i * TMath::PiOver2() );

            orrubaStripsPos[0][0][i][j].SetMag ( refQQQ5D_sectA.Mag() );
            orrubaStripsPos[1][0][i][j].SetMag ( refQQQ5D_sectA.Mag() );

            orrubaStripsPos[0][0][i][j].SetRotationZ ( i * TMath::PiOver2() );
            orrubaStripsPos[1][0][i][j].SetRotationZ ( i * TMath::PiOver2() );

            float prevStripWidth = firstStripWidth - ( j-1 ) * 0.05;
            float currStripWidth = firstStripWidth - j * 0.05;

            TVector3 pStPosRefDetCenter = prevStripRefDetCenter + TVector3 ( 0, ( prevStripWidth + currStripWidth ) / 2., 0 );
            prevStripRefDetCenter = pStPosRefDetCenter;

            pStPosRefDetCenter.SetPhi ( pStPosRefDetCenter.Phi() + orrubaStripsPos[0][0][i][j].RotZ() );

            orrubaStripsPos[0][0][i][j] += pStPosRefDetCenter;
            orrubaStripsPos[1][0][i][j] += pStPosRefDetCenter;
        }
    }

    return;
}

TVector3 GoddessGeomUtils::GetFinalHitPosition ( int isUpstream_, int isBarrel_, int sector_, int strip_, float eNear_, float eFar_ )
{
    TVector3 hitPos;

    if ( isBarrel_ == 0 )
    {
        hitPos = orrubaStripsPos[isUpstream_][0][sector_][strip_].GetTVector3();
    }
    else if ( isBarrel_ == 1 )
    {
        float SX3_length = 75.; // mm

        string detKey = Form ( "SuperX3 %s%d position %d", ( isUpstream_ ? "U" : "D" ), sector_, strip_ );

        float recenter = ( configCalPars[detKey].second + configCalPars[detKey].first ) / 2.;

        float normalize = configCalPars[detKey].second - configCalPars[detKey].first;

        normalize = normalize == 0 ? 1 : normalize;

        float zRes = ( ( ( eNear_ - eFar_ ) / ( eNear_ + eFar_ ) )  - recenter ) / normalize;

        TVector3 zResPos ( 0, 0, zRes * SX3_length );

        TVector3 interactionPos = orrubaStripsPos[isUpstream_][1][sector_][strip_].GetTVector3() + zResPos;

        return interactionPos;
    }

    return hitPos;
}

void GoddessGeomUtils::SetNewGeomMode ( bool geomOR, bool reacOR, bool beamELoss, bool ejecELoss )
{
    nGP.overrideGeom = geomOR;
    nGP.overrideReac = reacOR;
    nGP.computeBeamELoss = beamELoss;
    nGP.computeEjectileELoss = ejecELoss;
}

void GoddessGeomUtils::SetGeomOverride ( bool geomOR )
{
    nGP.overrideGeom = geomOR;
}

void GoddessGeomUtils::SetReacOverride ( bool reacOR )
{
    nGP.overrideReac = reacOR;
}

void GoddessGeomUtils::SetComputeBeamELoss ( int beamELoss )
{
    nGP.computeBeamELoss = beamELoss;
}

void GoddessGeomUtils::SetComputeEjecELoss ( int ejecELoss )
{
    nGP.computeEjectileELoss = ejecELoss;
}

void GoddessGeomUtils::GenerateGeomAdjustRootfile ( string filesname, string treename, long long int nEntries, string outfname )
{
    std::vector<string> rootfiles = DecodeItemsToTreat ( filesname, "system" );

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

//     TVector3 targetPos ( 0, 0, 0 );
//
//     TVector3 beamDir ( 0, 0, 1 );

    TTree* outtree = new TTree ( "godGeom", "godGeom" );

    bool isBarrel;
    bool isUpstream;
    float sector_strip;
    double energy;
//     double angle;
    TVector3 pos;

    outtree->Branch ( "isBarrel", &isBarrel );
    outtree->Branch ( "isUpstream", &isUpstream );
    outtree->Branch ( "sector_strip", &sector_strip );
    outtree->Branch ( "Energy", &energy );
//     outtree->Branch ( "angle", &angle );
    outtree->Branch ( "pos", &pos );

    vector<SiDataBase>* vectSiData = new vector<SiDataBase>;

    chain->SetBranchAddress ( "si", &vectSiData );

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

                isUpstream = ( int ) siData.isUpstream;
                isBarrel = ( int ) siData.isBarrel;

                sector_strip = siData.sector;
                sector_strip += siData.StripMaxLayer ( 1,false ) *0.01;
                energy = siData.ESumLayer ( 1,false );
                pos = siData.PosE1();
//                 angle = siData.Angle ( 1 );

//                 if ( energy > 0 && angle > 0 ) outtree->Fill();
                if ( energy > 0 && pos.Mag() > 0 ) outtree->Fill();
            }
        }
    }

    TFile* outf = new TFile ( outfname.c_str(), "recreate" );

    outf->cd();

    outtree->Write();

    outf->mkdir ( "infos" );
    outf->cd ( "infos" );
    geomInfo->Write ( "GoddessGeom" );
    reacInfo->Write ( "GoddessReac" );
    outf->Close();

    cout << endl;

    return;
}

void* GoddessGeomUtils::RecalculateAngleAndQVal ( void* args )
{
    int* eLossMode = ( int* ) args;

    for ( unsigned int offi = 0; offi < gAD.qqq5Offsets->size(); offi++ )
    {
        double sectPhi = gAD.isBarrel ? gAD.sX3sPos[gAD.isUpstream][gAD.sector].Phi() : gAD.qqq5sPos[gAD.isUpstream][gAD.sector].Phi();
//             double sectTheta = isBarrel ? sX3sPos[isUpstream][sector].Theta() : qqq5sPos[isUpstream][sector].Theta();

        TVector3 localOff = gAD.isBarrel ? gAD.sx3Offsets->at ( offi ) *0.01 : gAD.qqq5Offsets->at ( offi ) *0.01;

        localOff.SetPhi ( sectPhi );

        if ( gAD.isUpstream ) localOff.SetZ ( -localOff.Z() );

        TVector3 adjustPos = * ( gAD.pos ) + localOff - gAD.targetOffsets->at ( offi ) *0.01;

        float angle = adjustPos.Angle ( gAD.beamDir );

        double correctedEnergy = gAD.initialEnergy;

        if ( nGP.computeEjectileELoss > 0 )
        {
            //             if ( !isBarrel && isUpstream )
            //             {
            //                 cout << "*******************************************************\n";
            //                 cout << "QQQ5 U" << sector << " :\n";
            //                 cout << "Position = ( " << adjustPos.X() << " , " << adjustPos.Y() << " , " << adjustPos.Z() << " ) \n";
            //                 cout << "Angle with Beam Direction = " << angle*TMath::RadToDeg() << "\n";
            //                 cout << "Theta = " << adjustPos.Theta() *TMath::RadToDeg() << " / Phi = " <<  adjustPos.Phi() *TMath::RadToDeg() << "\n";
            //                 cout << "Angle with target ladder = " << adjustPos.Angle ( targetLadderDir ) *TMath::RadToDeg() << "\n";
            //             }

            double effThickness = GetEffectiveThickness ( adjustPos.Angle ( gAD.targetLadderDir ) - TMath::PiOver2(), reacInfo->targetThickness );
            double estELoss = ComputeEnergyLoss ( gAD.energyLossData.first, gAD.energyLossData.second, gAD.initialEnergy/reacInfo->ejecA, reacInfo->ejecA, 0, effThickness, 0.01, "Interpolation" );

            //             cout << "Input energy : " << initialEnergy << " / Estimated energy loss : " << estELoss << " MeV in effective thickness: " << effThickness <<endl;

            correctedEnergy += estELoss;
        }

        double energy;

        if ( *eLossMode >= 2 ) energy = correctedEnergy;
        else energy = gAD.initialEnergy;

        energy *= gainAdjusts[gAD.globStripID];

        string computeELossStr = ( *eLossMode%2 != 0 ) ? "" : "_no_BeamEnLoss";
        computeELossStr += ( *eLossMode >= 2 ) ? "" : "_no_EjecEnLoss";

        GoddessReacInfos* gri;

        if ( *eLossMode%2 == 0 ) gri = reacInfo;
        else gri = gAD.localReacInfo;

        string detKey;

        float qval;

        string commonKey = Form ( "_QQQ5_mod_%s_%s_%s_%d_SX3_mod_%s_%s_%s_%d_target_off_%s_%s_%s_beamEk_%d%s",
                                  gAD.qqq5OffXStr[offi].c_str(), gAD.qqq5OffYStr[offi].c_str(), gAD.qqq5OffZStr[offi].c_str(), RoundValue ( gri->qqq5EnGain*100 ),
                                  gAD.sX3OffXStr[offi].c_str(), gAD.sX3OffYStr[offi].c_str(), gAD.sX3OffZStr[offi].c_str(), RoundValue ( gri->sX3EnGain*100 ),
                                  gAD.targetOffXStr[offi].c_str(), gAD.targetOffYStr[offi].c_str(), gAD.targetOffZStr[offi].c_str(),
                                  RoundValue ( gri->beamEk ), computeELossStr.c_str() );

        if ( gAD.isBarrel )
        {
            qval = SiDataBase::QValue ( gri, energy*gri->sX3EnGain, angle );

            detKey = Form ( "EvsA_new_geom_SX3%s%d", ( gAD.isUpstream ? "U" : "D" ), gAD.sector );

            hEvsA_SX3_NewGeom[detKey + commonKey]->Fill ( angle*TMath::RadToDeg(), energy*gri->sX3EnGain );
        }
        else
        {
            qval = SiDataBase::QValue ( gri, energy*gri->qqq5EnGain, angle );

            detKey = Form ( "EvsA_new_geom_QQQ5%s%d", ( gAD.isUpstream ? "U" : "D" ), gAD.sector );

            hEvsA_QQQ5_NewGeom[detKey + commonKey]->Fill ( angle*TMath::RadToDeg(), energy*gri->qqq5EnGain );

            //                         detKey = Form ( "QVal_vs_Strip_new_geom_QQQ5%s%c_det_off_%s_%s_%s_target_off_%s_%s_%s_beamEk_%d_QQQ5Gain_%dpercents%s", ( isUpstream ? "U" : "D" ), qqq5SectAliases[sector],
            //                                         qqq5OffXStr[offi].c_str(), qqq5OffYStr[offi].c_str(), qqq5OffZStr[offi].c_str(), targetOffXStr[offi].c_str(), targetOffYStr[offi].c_str(), targetOffZStr[offi].c_str(),
            //                                         RoundValue ( gri->beamEk ), RoundValue ( gri->qqq5EnGain*100 ), computeELossStr.c_str() );
            //
            //                         hQvalvsStrip_QQQ5_NewGeom[detKey]->Fill ( strip, qval );
        }

        hQval_NewGeom[ ( "QVal_vs_Strips" + commonKey ) ]->Fill ( gAD.globStripID, qval );
        hEx_NewGeom[ ( "Ex_vs_Strips" + commonKey ) ]->Fill ( gAD.globStripID, reacInfo->qValGsGs - qval );
    }

    return nullptr;
}

void GoddessGeomUtils::GetQValWithNewGeometry ( string filename, string treeName, long long int nEntries, vector< TVector3 > qqq5Offsets, vector< TVector3 > sx3Offsets, vector< TVector3 > targetOffsets )
{
    TFile* inFile = new TFile ( filename.c_str(), "read" );

    if ( !nGP.overrideGeom || !nGP.overrideReac ) // This is true if the user doesn't want to use the geometry/reaction parameters read from the input file as a base
    {
        if ( !inFile->IsOpen() ) return;

        if ( !nGP.overrideGeom ) geomInfo = ( GoddessGeomInfos* ) inFile->FindObjectAny ( "GoddessGeom" )->Clone();
        if ( !nGP.overrideReac ) reacInfo = ( GoddessReacInfos* ) inFile->FindObjectAny ( "GoddessReac" )->Clone();
    }

    inFile->Close();

    if ( geomInfo == nullptr )
    {
        cerr << "Geometry information missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    if ( reacInfo == nullptr )
    {
        cerr << "Reaction information missing...\n";
        cerr << "Aborting...\n";
        return;
    }

    gAD.qqq5Offsets = &qqq5Offsets;
    gAD.sx3Offsets = &sx3Offsets;
    gAD.targetOffsets = &targetOffsets;

    gAD.sX3OffXStr.clear();
    gAD.sX3OffYStr.clear();
    gAD.sX3OffZStr.clear();
    gAD.qqq5OffXStr.clear();
    gAD.qqq5OffYStr.clear();
    gAD.qqq5OffZStr.clear();
    gAD.targetOffXStr.clear();
    gAD.targetOffYStr.clear();
    gAD.targetOffZStr.clear();

    if ( nGP.computeEjectileELoss )
    {
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

        gAD.energyLossData = FillGraphFromFile ( stoppingPowerTable["ejectile"] );
    }

    gAD.beamDir = TVector3 ( 0, 0, 1 );

    gAD.targetLadderDir = TVector3 ( 0, 0, 1 );
    gAD.targetLadderDir.SetTheta ( geomInfo->targetLadderAngle*TMath::DegToRad() );
    gAD.targetLadderDir.SetPhi ( TMath::PiOver2() );

//     char qqq5SectAliases[4] = {'A', 'B', 'C', 'D'};

    gAD.localReacInfo = ( GoddessReacInfos* ) reacInfo->Clone();

    if ( nGP.computeBeamELoss )
    {
        double beamEffThickness = GetEffectiveThickness ( gAD.beamDir.Angle ( gAD.targetLadderDir ) - TMath::PiOver2(), gAD.localReacInfo->targetThickness );

        gAD.localReacInfo->beamEk = TryGetRemainingEnergy ( pathToGDAQ + "/share/mass_db.dat", gAD.localReacInfo->beamA, gAD.localReacInfo->beamZ, gAD.localReacInfo->beamEk, beamEffThickness, 0.001,
                                    gAD.localReacInfo->targetType, gAD.localReacInfo->targetDensity, "./", "Interpolation" );

        cout << "Beam Energy after computing energy loss: " << gAD.localReacInfo->beamEk << "MeV in effective thickness: " << beamEffThickness << " mg/cm2\n";
    }

    double qqq5FstStripWidth = geomInfo->qqq5FirstStripWidth;
    double qqq5DeltaPitch = geomInfo->qqq5DeltaPitch;

    TVector3 targetPos = geomInfo->GetOffset ( "Target Offset" );
    cout << "Target Offset read from file: ( " << targetPos.X() << " , " << targetPos.Y() << " , " << targetPos.Z() << " ) \n";

    cout << "Will apply the following gain: SuperX3 = " << reacInfo->sX3EnGain << " / QQQ5 = " << reacInfo->qqq5EnGain << "\n";

    int startBELiter;
    switch ( nGP.computeBeamELoss )
    {
    case 0:
        startBELiter = 0;
        break;
    case 1:
        startBELiter = 1;
        break;
    case 2:
        startBELiter = 0;
        break;
    }

    int maxBELiter;
    switch ( nGP.computeBeamELoss )
    {
    case 0:
        maxBELiter = 0;
        break;
    case 1:
        maxBELiter = 1;
        break;
    case 2:
        maxBELiter = 1;
        break;
    }

    int startEELiter;
    switch ( nGP.computeEjectileELoss )
    {
    case 0:
        startEELiter = 0;
        break;
    case 1:
        startEELiter = 1;
        break;
    case 2:
        startEELiter = 0;
        break;
    }

    int maxEELiter;
    switch ( nGP.computeEjectileELoss )
    {
    case 0:
        maxEELiter = 0;
        break;
    case 1:
        maxEELiter = 1;
        break;
    case 2:
        maxEELiter = 1;
        break;
    }

    // sector ID in this case are sector number + 4 if superX3 + 16 if upstream
    vector<int> ignoreSectorsList = {0, 1, 2, 3, 4+0, 4+1, 4+2, 4+3, 4+6, 4+8, 4+9, 4+10, 4+11, 16+4+1, 16+4+2, 16+4+3, 16+4+4, 16+4+5, 16+4+6, 16+4+7, 16+4+8, 16+4+10};

    bool fillQQQ5Pos = true;
    bool fillSX3Pos = true;

    TVector3 qqq5sPos[2][4];
    TVector3 sX3sPos[2][12];

    for ( unsigned int offi = 0; offi < qqq5Offsets.size(); offi++ )
    {
        if ( offi>0 )
        {
            fillQQQ5Pos = false;
            fillSX3Pos = false;
        }

        int sX3OffX = sx3Offsets[offi].X(), sX3OffY = sx3Offsets[offi].Y(), sX3OffZ = sx3Offsets[offi].Z();
        int qqq5OffX = qqq5Offsets[offi].X(), qqq5OffY = qqq5Offsets[offi].Y(), qqq5OffZ = qqq5Offsets[offi].Z();
        int targetOffX = targetOffsets[offi].X(), targetOffY = targetOffsets[offi].Y(), targetOffZ = targetOffsets[offi].Z();

        gAD.sX3OffXStr.push_back ( GetNameCompliantStr ( sX3OffX ) );
        gAD.sX3OffYStr.push_back ( GetNameCompliantStr ( sX3OffY ) );
        gAD.sX3OffZStr.push_back ( GetNameCompliantStr ( sX3OffZ ) );
        gAD.qqq5OffXStr.push_back ( GetNameCompliantStr ( qqq5OffX ) );
        gAD.qqq5OffYStr.push_back ( GetNameCompliantStr ( qqq5OffY ) );
        gAD.qqq5OffZStr.push_back ( GetNameCompliantStr ( qqq5OffZ ) );
        gAD.targetOffXStr.push_back ( GetNameCompliantStr ( targetOffX ) );
        gAD.targetOffYStr.push_back ( GetNameCompliantStr ( targetOffY ) );
        gAD.targetOffZStr.push_back ( GetNameCompliantStr ( targetOffZ ) );

        for ( int belItr = startBELiter; belItr <= maxBELiter; belItr++ )
        {
            for ( int eelItr = startEELiter; eelItr <= maxEELiter; eelItr++ )
            {
                string computeELossStr = belItr ? "" : "_no_BeamEnLoss";
                computeELossStr += eelItr ? "" : "_no_EjecEnLoss";

                GoddessReacInfos* gri;

                if ( belItr == 0 ) gri = reacInfo;
                else gri = gAD.localReacInfo;

                string commonName = Form ( "_QQQ5_mod_%s_%s_%s_%d_SX3_mod_%s_%s_%s_%d_target_off_%s_%s_%s_beamEk_%d%s",
                                           gAD.qqq5OffXStr[offi].c_str(), gAD.qqq5OffYStr[offi].c_str(), gAD.qqq5OffZStr[offi].c_str(), RoundValue ( gri->qqq5EnGain*100 ),
                                           gAD.sX3OffXStr[offi].c_str(), gAD.sX3OffYStr[offi].c_str(), gAD.sX3OffZStr[offi].c_str(), RoundValue ( gri->sX3EnGain*100 ),
                                           gAD.targetOffXStr[offi].c_str(), gAD.targetOffYStr[offi].c_str(), gAD.targetOffZStr[offi].c_str(),
                                           RoundValue ( gri->beamEk ), computeELossStr.c_str()
                                         );

                string commonTitle = Form ( " / QQQ5 mod X: %d, Y: %d, Z: %d, gain: %4.2f / SuperX3 mod X: %d, Y: %d, Z: %d, gain: %4.2f / target X: %d, Y: %d, Z: %d / Beam Ek: %3.1f / %s",
                                            qqq5OffX, qqq5OffY ,qqq5OffZ, gri->qqq5EnGain, sX3OffX, sX3OffY ,sX3OffZ, gri->sX3EnGain, targetOffX, targetOffY, targetOffZ,
                                            gri->beamEk, nGP.computeEjectileELoss ? "" : "ignore ejectile energy loss" );

                string histKey = "QVal_vs_Strips" + commonName;

                if ( hQval_NewGeom.find ( histKey ) == hQval_NewGeom.end() )
                {
                    hQval_NewGeom[histKey] = new TH2F ( histKey.c_str(), ( ( string ) "Q-Value new geom vs. Strips" + commonTitle ).c_str(), 500, 0, 500, 800, -20, 20 );
                }
                hQval_NewGeom[histKey]->Reset();

                histKey = "Ex_vs_Strips" + commonName;

                if ( hEx_NewGeom.find ( histKey ) == hEx_NewGeom.end() )
                {
                    hEx_NewGeom[histKey] = new TH2F ( histKey.c_str(), ( ( string ) "Excitation Energy new geom vs. Strips" + commonTitle ).c_str(), 500, 0, 500, 800, -20, 20 );
                }
                hEx_NewGeom[histKey]->Reset();

                for ( int up = 0; up < 2; up++ )
                {
                    for ( int bar = 0; bar < 2; bar++ )
                    {
                        int maxSector = ( bar ? 12 : 4 );

                        for ( int sect = 0; sect < maxSector; sect++ )
                        {
                            int sectorID = up*16+bar*4+sect;

                            if ( std::find ( ignoreSectorsList.begin(), ignoreSectorsList.end(), sectorID ) != ignoreSectorsList.end() ) continue;

                            if ( bar == 1 )
                            {
                                if ( fillSX3Pos ) sX3sPos[up][sect] = GetDetPos ( geomInfo, up, bar, sect, 1 );

                                histKey = Form ( "EvsA_new_geom_SX3%s%d", ( up ? "U" : "D" ), sect ) + commonName;

                                if ( hEvsA_SX3_NewGeom.find ( histKey ) == hEvsA_SX3_NewGeom.end() )
                                {
                                    hEvsA_SX3_NewGeom[histKey] = new TH2F ( histKey.c_str(), ( ( string ) "Energy vs. Angle new geom" + commonTitle ).c_str(), 1800, 0, 180, 1500, 0, 15 );
                                }
                                hEvsA_SX3_NewGeom[histKey]->Reset();
                            }
                            else
                            {
                                //                     cout << "------ Preparing histogram for " << ( bar ? "SuperX3" : "QQQ5" ) << " " << ( up ? "U" : "D" ) << sect << " ------\n";

                                if ( fillQQQ5Pos ) qqq5sPos[up][sect] = GetDetPos ( geomInfo, up, bar, sect, 1 );

                                double* qqq5BinsEdges;

                                std::list<double> binsEdgesList;
                                binsEdgesList.clear();

                                TVector3 lowEdge = qqq5sPos[up][sect];
                                double midDetPhi = lowEdge.Phi();

                                TVector3 detOffset = qqq5Offsets[offi]*0.01;
                                detOffset.SetPhi ( midDetPhi );

                                if ( up ) detOffset.SetZ ( -detOffset.Z() );

                                lowEdge += detOffset;

                                //                     cout << "Position of the low edge of the first strip is: ( " << lowEdge.X() << " , " << lowEdge.Y() << " , " << lowEdge.Z() << " ) \n";
                                //                     cout << "Phi angle is: " << midDetPhi * TMath::RadToDeg() << "\n";

                                for ( int i = 0; i < 33; i++ )
                                {
                                    if ( i>0 )
                                    {
                                        TVector3 toNextStrip = TVector3 ( 0, qqq5FstStripWidth - ( i-1 ) *qqq5DeltaPitch , 0 );

                                        toNextStrip.SetPhi ( midDetPhi );

                                        lowEdge += toNextStrip;
                                    }

                                    double thetaEdge = ( lowEdge - targetOffsets[offi]*0.01 ).Angle ( gAD.beamDir ) * TMath::RadToDeg();

                                    binsEdgesList.push_back ( thetaEdge );

//                                     for ( int j = 0; j < 4; j++ )
//                                     {
//                                         lowEdge.SetPhi ( midDetPhi - 3./16. * TMath::Pi() + j/8. * TMath::Pi() );
//
//                                         thetaEdge = ( lowEdge - targetOffsets[offi]*0.01 ).Angle ( gAD.beamDir ) * TMath::RadToDeg();
//
//                                         binsEdgesList.push_back ( thetaEdge );
//                                     }
//
//                                     lowEdge.SetPhi ( midDetPhi );
                                }

                                //                     cout << "List of bin edges done... " << binsEdgesList.size() << " elements\n";

                                //                     cout << "Removing duplicates...\n";
                                binsEdgesList.unique ( CheckValProxFunc ( 0.05 ) );
                                //                     cout << "Duplicates removed... new size: " << binsEdgesList.size() << " elements\n";

                                //                     cout << "Sorting it...\n";
                                binsEdgesList.sort();
                                //                     cout << "Sorting done...\n";


                                qqq5BinsEdges = new double[binsEdgesList.size()];
                                unsigned int counter = 0;

                                //                     cout << "Filling the final bins edges array of size " << binsEdgesList.size() <<" ...\n";

                                for ( auto listItr = binsEdgesList.begin(); listItr != binsEdgesList.end(); listItr++ )
                                {
                                    qqq5BinsEdges[counter] = *listItr;
                                    //                         cout << "Bin #" << counter << " : " << qqq5BinsEdges[counter] << endl;
                                    counter++;
                                }

                                histKey = Form ( "EvsA_new_geom_QQQ5%s%d", ( up ? "U" : "D" ), sect ) + commonName;

                                if ( hEvsA_QQQ5_NewGeom.find ( histKey ) == hEvsA_QQQ5_NewGeom.end() )
                                {
                                    hEvsA_QQQ5_NewGeom[histKey] = new TH2F ( ( histKey ).c_str(), ( ( string ) "Energy vs. Angle new geom" + commonTitle ).c_str(),
                                            binsEdgesList.size()-1, qqq5BinsEdges, 1500, 0, 15 );
                                }
                                hEvsA_QQQ5_NewGeom[histKey]->Reset();
                            }
                        }
                    }
                }
            }
        }
    }

    inFile = new TFile ( filename.c_str(), "read" );

    if ( !inFile->IsOpen() ) return;

    TTree* inTree = ( TTree* ) inFile->Get ( treeName.c_str() );

    if ( inTree == nullptr ) return;

    if ( nEntries <= 0 || nEntries > inTree->GetEntries() ) nEntries = inTree->GetEntries();

//     bool isBarrel;
//     bool isUpstream;
//     float sector_strip;
//     double initialEnergy;
//     double correctedEnergy;
//     GoddessReacInfos* gri;
//     //     double angle;
//     TVector3* pos = new TVector3();

    inTree->SetBranchAddress ( "isBarrel", & ( gAD.isBarrel ) );
    inTree->SetBranchAddress ( "isUpstream", & ( gAD.isUpstream ) );
    inTree->SetBranchAddress ( "sector_strip", & ( gAD.sector_strip ) );
    inTree->SetBranchAddress ( "Energy", & ( gAD.initialEnergy ) );
    //     inTree->SetBranchAddress ( "angle", &angle );
    inTree->SetBranchAddress ( "pos", & ( gAD.pos ) );

    for ( long long int ev = 0; ev < nEntries; ev++ )
    {
        if ( ev%10000 == 0 )
        {
            cout << "Entry " << std::setw ( 15 ) << ev << " / " << nEntries;
            cout<< " ( " << std::fixed << std::setprecision ( 2 )  << std::setw ( 6 ) << ( ( float ) ev/nEntries ) * 100. << " % ) \r" << std::flush;
        }

        inTree->GetEntry ( ev );

        gAD.sector = floor ( gAD.sector_strip );

        gAD.sectorID = gAD.isUpstream*16+gAD.isBarrel*4+gAD.sector;

        if ( std::find ( ignoreSectorsList.begin(), ignoreSectorsList.end(), gAD.sectorID ) != ignoreSectorsList.end() ) continue;

        //         unsigned int strip = RoundValue ( ( sector_strip-sector ) * 100 );

        gAD.strip = RoundValue ( ( gAD.sector_strip-gAD.sector ) * 100 );

        gAD.globStripID = ToStripID ( gAD.isUpstream, gAD.isBarrel, true, gAD.sector, gAD.strip );

        for ( int belItr = startBELiter; belItr <= maxBELiter; belItr++ )
        {
            for ( int eelItr = startEELiter; eelItr <= maxEELiter; eelItr++ )
            {
                int* eLMode = new int ( belItr + 2*eelItr );

                RecalculateAngleAndQVal ( ( void* ) eLMode );
            }
        }
    }

    cout << endl;

    inFile->Close();

    return;
}

void GoddessGeomUtils::GetQValWithNewGeometry ( string filename, string treeName, long long int nEntries,
        int qqq5OffX, int qqq5OffY, int qqq5OffZ, int sX3OffX, int sX3OffY, int sX3OffZ, int targetOffX, int targetOffY, int targetOffZ )
{
    vector<TVector3> sx3Offs, qqq5Offs, targOffs;

    sx3Offs.push_back ( TVector3 ( sX3OffX, sX3OffY, sX3OffZ ) );
    qqq5Offs.push_back ( TVector3 ( qqq5OffX, qqq5OffY, qqq5OffZ ) );
    targOffs.push_back ( TVector3 ( targetOffX, targetOffY, targetOffZ ) );

    GetQValWithNewGeometry ( filename, treeName, nEntries, qqq5Offs, sx3Offs, targOffs );
}

void GoddessGeomUtils::GetQValWithNewGeometry ( string filename, string treeName, long long int nEntries, string offsetsList )
{
    std::ifstream offList;
    offList.open ( offsetsList.c_str(), std::ios::in );

    vector<TVector3> sx3Offs, qqq5Offs, targetOffs;

    string readLine;

    while ( std::getline ( offList, readLine ) && !offList.eof() )
    {
        if ( readLine.empty() ) continue;

        std::istringstream iss_;
        iss_.str ( readLine );

        double sx3Ox, sx3Oy, sx3Oz, qqq5Ox, qqq5Oy, qqq5Oz, targOx, targOy, targOz;

        iss_ >> qqq5Ox >> qqq5Oy >> qqq5Oz >> sx3Ox >> sx3Oy >> sx3Oz >> targOx >> targOy >> targOz;

        sx3Offs.push_back ( TVector3 ( sx3Ox, sx3Oy, sx3Oz ) );
        qqq5Offs.push_back ( TVector3 ( qqq5Ox, qqq5Oy, qqq5Oz ) );
        targetOffs.push_back ( TVector3 ( targOx, targOy, targOz ) );
    }

    GetQValWithNewGeometry ( filename, treeName, nEntries, qqq5Offs, sx3Offs, targetOffs );
}

void GoddessGeomUtils::GetQValWithNewGeometry ( string filename, string treeName, long long int nEntries,
        int minQQQ5OffX, int maxQQQ5OffX, int minQQQ5OffY, int maxQQQ5OffY, int minQQQ5OffZ, int maxQQQ5OffZ, int stepQQQ5,
        int minSX3OffX, int maxSX3OffX, int minSX3OffY, int maxSX3OffY, int minSX3OffZ, int maxSX3OffZ, int stepSX3,
        int minTargetOffX, int maxTargetOffX, int minTargetOffY, int maxTargetOffY, int minTargetOffZ, int maxTargetOffZ, int stepTarget )
{
    vector<TVector3> sx3Offs, qqq5Offs, targetOffs;

    for ( int iq = minQQQ5OffX; iq <= maxQQQ5OffX; iq+=stepQQQ5 )
    {
        for ( int jq = minQQQ5OffY; jq <= maxQQQ5OffY; jq+=stepQQQ5 )
        {
            for ( int kq = minQQQ5OffZ; kq <= maxQQQ5OffZ; kq+=stepQQQ5 )
            {
                for ( int is = minSX3OffX; is <= maxSX3OffX; is+=stepSX3 )
                {
                    for ( int js = minSX3OffY; js <= maxSX3OffY; js+=stepSX3 )
                    {
                        for ( int ks = minSX3OffZ; ks <= maxSX3OffZ; ks+=stepSX3 )
                        {
                            for ( int it = minTargetOffX; it <= maxTargetOffX; it+=stepTarget )
                            {
                                for ( int jt = minTargetOffY; jt <= maxTargetOffY; jt+=stepTarget )
                                {
                                    for ( int kt = minTargetOffZ; kt <= maxTargetOffZ; kt+=stepTarget )
                                    {
                                        targetOffs.push_back ( TVector3 ( it, jt, kt ) );
                                        sx3Offs.push_back ( TVector3 ( is, js, ks ) );
                                        qqq5Offs.push_back ( TVector3 ( iq, jq, kq ) );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    GetQValWithNewGeometry ( filename, treeName, nEntries, qqq5Offs, sx3Offs, targetOffs );
}

std::pair< double, double > GoddessGeomUtils::FindPeakPos ( TH1* input, int nPeaks, double resolution, double sigma, double threshold )
{
    TSpectrum* spec = new TSpectrum ( nPeaks, resolution );

    spec->Search ( input, sigma, "", threshold );

//     double* peaksX = new double[spec->GetNPeaks()];
//     double* peaksY = new double[spec->GetNPeaks()];

    double* peaksX = spec->GetPositionX();
    double* peaksY = spec->GetPositionY();

    int bestPeak = -1;
    double bestChi2 = 1e10, bestAmpDiff = 1e10, bestSigma = 1e10;

    for ( int i = 0; i < spec->GetNPeaks(); i++ )
    {
        int foundBetter = 0;

        vector<string> means = {Form ( "[0]: %f", peaksX[i] ), "[1]: [0]-0.1"};

        TF1* fitFunc = FitQVal ( input, means, 0.01, 0.3, peaksX[i] - 0.6, peaksX[i] + 0.6, "TSpectrum", true );

        double ampDiff = fabs ( fitFunc->GetParameter ( 3 ) - input->GetBinContent ( floor ( fitFunc->GetParameter ( 4 ) /input->GetXaxis()->GetBinWidth ( 1 ) ) +1 ) );
        ampDiff += fabs ( fitFunc->GetParameter ( 5 ) - input->GetBinContent ( floor ( ( fitFunc->GetParameter ( 4 ) +fitFunc->GetParameter ( 6 ) ) /input->GetXaxis()->GetBinWidth ( 1 ) ) +1 ) );
        ampDiff /= 2;

        if ( ampDiff < bestAmpDiff )
        {
            bestAmpDiff = ampDiff;
            foundBetter++;
        }

        if ( fitFunc->GetChisquare() < bestChi2 )
        {
            bestChi2 = fitFunc->GetChisquare();
            foundBetter++;
        }

        if ( fitFunc->GetParameter ( 2 ) < bestSigma )
        {
            bestSigma = fitFunc->GetParameter ( 2 );
            foundBetter++;
        }

        if ( foundBetter >= 2 )
        {
            bestPeak = i;
        }
    }

    FitQVal ( input, {Form ( "[0]: %f", peaksX[bestPeak] ), "[1]: [0]-0.1"}, 0.01, 0.3, peaksX[bestPeak] - 0.6, peaksX[bestPeak] + 0.6, "linear", false );

    return std::make_pair ( peaksX[bestPeak], peaksY[bestPeak] );
}

TGraph* GoddessGeomUtils::FindKinematicsLines ( TH2* input, int projWidth )
{
    int nBinsX = input->GetNbinsX();

    int nSteps = nBinsX/projWidth;

    TGraph* gr = new TGraph ( nSteps );

    for ( int i = 0; i < nSteps; i++ )
    {
        TH1D* hProj = input->ProjectionY ( "_py", i*projWidth, i*projWidth+projWidth-1 );

        if ( hProj->Integral() == 0 ) continue;

        while ( hProj->GetXaxis()->GetBinWidth ( 1 ) < 0.05 ) hProj->Rebin ( 2 );

        hProj->GetXaxis()->SetRangeUser ( 2,5 );

        auto fRes = FindPeakPos ( hProj, 5 );

        gr->SetPoint ( i, input->GetXaxis()->GetBinCenter ( ( 2*i*projWidth+projWidth ) /2 ), fRes.first );
    }

    for ( int i = 0; i < gr->GetN(); i++ )
    {
        double x_, y_;

        gr->GetPoint ( i, x_, y_ );

        if ( y_ == 0 )
        {
            gr->RemovePoint ( i );
            i--;
        }
    }

    return gr;
}

TF1* GoddessGeomUtils::FindBestGeom ( string fName, string detStr )
{
    TFile* inFile = new TFile ( fName.c_str(), "read" );

    if ( !inFile->IsOpen() )
    {
        cerr << "Failed to open file " << fName << endl;
        return nullptr;
    }

    TF1* bestFit = nullptr;
    double bestIntegral = 0;

    string bestOffName;
    TH2F* bestHist = nullptr;

    double bestChi2 = -1;

    auto lOK = inFile->GetListOfKeys();

    auto itr = lOK->MakeIterator();

    TObject* obj = itr->Next();

    bool failedOnce = false;

    vector<int> xOffs, yOffs, zOffs;
    vector<double> sigmas, integrals, chi2s;

    TCanvas* bestFitCan = new TCanvas ( "Best_Fit", "Best Fit" );

FindBestFitLoop:

    while ( obj != nullptr )
    {
        string objName = obj->GetName();

        if ( objName.find ( "Ex_vs_Strips" ) != string::npos )
        {
            TH2F* h2 = ( TH2F* ) inFile->Get ( obj->GetName() );

            size_t targOffPos = objName.find ( "target_off_" );

            if ( targOffPos == string::npos ) continue;

            size_t xOffPos = objName.find_first_of ( "0123456789", targOffPos );
            size_t offSepPos = objName.find_first_of ( "_", xOffPos );

            int offMod;

            offMod = ( objName[xOffPos-1] == 'm' ) ? -1 : 1;

            xOffs.push_back ( offMod * stof ( objName.substr ( xOffPos, offSepPos-xOffPos ) ) );

            size_t yOffPos = objName.find_first_of ( "0123456789", offSepPos );
            offSepPos = objName.find_first_of ( "_", yOffPos );

            offMod = ( objName[yOffPos-1] == 'm' ) ? -1 : 1;

            yOffs.push_back ( offMod * stof ( objName.substr ( yOffPos, offSepPos-xOffPos ) ) );

            size_t zOffPos = objName.find_first_of ( "0123456789", offSepPos );
            offSepPos = objName.find_first_of ( "_", zOffPos );

            offMod = ( objName[zOffPos-1] == 'm' ) ? -1 : 1;

            zOffs.push_back ( offMod * stof ( objName.substr ( zOffPos, offSepPos-xOffPos ) ) );

            if ( h2 != nullptr )
            {
                TH1D* hEx = DrawGodHist ( h2, detStr );

                TF1* newFit = FitQVal ( hEx,
                {"[0]: 0 (min=-0.02;max=0.02)", "[1]: [0]+0.288 (min=0.288;max=0.288)", "[2]: [0]+2.035 (min=2.023;max=2.047)", "[3]: [0]+2.406 (min=2.400; max=2.418)"},
                0.02, 0.5, -1, 3, "TSpectrum", false );

                double newIntegral = newFit->Integral ( -1, 2.5 );

                integrals.push_back ( newIntegral );
                sigmas.push_back ( newFit->GetParameter ( 2 ) );
                chi2s.push_back ( newFit->GetChisquare() );

                if ( ( bestFit == nullptr &&
                        ( fabs ( newFit->GetParameter ( 4 ) ) <= 0.05 /*&& fabs ( newFit->GetParameter ( 6 ) - 0.288 ) <=0.02
                          && fabs ( newFit->GetParameter ( 8 ) - 2.035 ) <=0.02 && fabs ( newFit->GetParameter ( 10 ) - 2.406 ) <=0.02*/ ) )
                        || ( bestFit != nullptr
                             /*&& fabs ( bestChi2-newFit->GetChisquare() ) / bestChi2 <= 0.5*/
                             && fabs ( newFit->GetParameter ( 4 ) ) <= fabs ( bestFit->GetParameter ( 4 ) )
                             && fabs ( newFit->GetParameter ( 6 ) - 0.288 ) <= fabs ( bestFit->GetParameter ( 6 ) - 0.288 )
                             && fabs ( newFit->GetParameter ( 8 ) - 2.035 ) <= fabs ( bestFit->GetParameter ( 8 ) - 2.035 )
                             && fabs ( newFit->GetParameter ( 10 ) - 2.406 ) <= fabs ( bestFit->GetParameter ( 10 ) - 2.406 )
                             && newFit->GetParameter ( 2 ) <= bestFit->GetParameter ( 2 )
                             /*&& newFit->GetParameter ( 5 ) / newFit->GetParameter ( 3 ) > 1 && newFit->GetParameter ( 5 ) / newFit->GetParameter ( 3 ) < 5
                             /*&& newIntegral >= bestIntegral*/ ) )
                {
                    if ( bestChi2 < 0 ) bestChi2 = newFit->GetChisquare();
                    else if ( newFit->GetChisquare() < bestChi2 ) bestChi2 = newFit->GetChisquare();

                    bestFit = newFit;

                    bestIntegral = newIntegral;

                    bestOffName = objName;

                    bestHist = ( TH2F* ) h2->Clone();
                }
            }
        }

        obj = itr->Next();
    }

    TH2F* h_x_y_chi2 = new TH2F ( "h_x_y_chi2", "Chi2 for (x,y) sets", 11, -1100, 1100, 11, -1100, 1100 );
    TH2F* h_x_z_chi2 = new TH2F ( "h_x_z_chi2", "Chi2 for (x,z) sets", 11, -1100, 1100, 11, -1100, 1100 );
    TH2F* h_y_z_chi2 = new TH2F ( "h_y_z_chi2", "Chi2 for (y,z) sets", 11, -1100, 1100, 11, -1100, 1100 );
    TH2F* h_x_y_sigma = new TH2F ( "h_x_y_sigma", "Sigma for (x,y) sets", 11, -1100, 1100, 11, -1100, 1100 );
    TH2F* h_x_z_sigma = new TH2F ( "h_x_z_sigma", "Sigma for (x,z) sets", 11, -1100, 1100, 11, -1100, 1100 );
    TH2F* h_y_z_sigma = new TH2F ( "h_y_z_sigma", "Sigma for (y,z) sets", 11, -1100, 1100, 11, -1100, 1100 );
    TH2F* h_x_y_integral = new TH2F ( "h_x_y_integral", "Integral for (x,y) sets", 11, -1100, 1100, 11, -1100, 1100 );
    TH2F* h_x_z_integral = new TH2F ( "h_x_z_integral", "Integral for (x,z) sets", 11, -1100, 1100, 11, -1100, 1100 );
    TH2F* h_y_z_integral = new TH2F ( "h_y_z_integral", "Integral for (y,z) sets", 11, -1100, 1100, 11, -1100, 1100 );

    TH2F* h_x_y_tot = new TH2F ( "h_x_y_tot", "Global Factor for (x,y) sets", 11, -1100, 1100, 11, -1100, 1100 );
    TH2F* h_x_z_tot = new TH2F ( "h_x_z_tot", "Global Factor for (x,z) sets", 11, -1100, 1100, 11, -1100, 1100 );
    TH2F* h_y_z_tot = new TH2F ( "h_y_z_tot", "Global Factor for (y,z) sets", 11, -1100, 1100, 11, -1100, 1100 );

    for ( unsigned int i = 0; i < xOffs.size(); i++ )
    {
        h_x_y_chi2->Fill ( xOffs[i], yOffs[i], chi2s[i] );
        h_x_z_chi2->Fill ( xOffs[i], zOffs[i], chi2s[i] );
        h_y_z_chi2->Fill ( yOffs[i], zOffs[i], chi2s[i] );

        double rev_sigma = sigmas[i]/0.07;
        if ( rev_sigma < 1 ) rev_sigma = 1/rev_sigma;

        h_x_y_sigma->Fill ( xOffs[i], yOffs[i], rev_sigma );
        h_x_z_sigma->Fill ( xOffs[i], zOffs[i], rev_sigma );
        h_y_z_sigma->Fill ( yOffs[i], zOffs[i], rev_sigma );

        h_x_y_integral->Fill ( xOffs[i], yOffs[i], integrals[i] );
        h_x_z_integral->Fill ( xOffs[i], zOffs[i], integrals[i] );
        h_y_z_integral->Fill ( yOffs[i], zOffs[i], integrals[i] );
    }

    double normFactorChi2 = h_x_y_chi2->GetMinimum();
    double normFactorSigma = h_x_y_sigma->GetMinimum();
    double normFactorIntegral = h_x_y_integral->GetMaximum();

    for ( int i = 1; i <= h_x_y_chi2->GetNbinsX(); i++ )
    {
        for ( int j = 1; j <= h_x_y_chi2->GetNbinsY(); j++ )
        {
            double xVal = h_x_y_chi2->GetXaxis()->GetBinCenter ( i );
            double yVal = h_x_y_chi2->GetYaxis()->GetBinCenter ( j );

            double binContent = h_x_y_chi2->GetBinContent ( i,j ) / normFactorChi2;
            binContent += h_x_y_sigma->GetBinContent ( i,j ) / normFactorSigma;
            binContent += normFactorIntegral / h_x_y_integral->GetBinContent ( i,j );

            h_x_y_tot->Fill ( xVal, yVal, binContent );
        }
    }

    normFactorChi2 = h_x_z_chi2->GetMinimum();
    normFactorSigma = h_x_z_sigma->GetMinimum();
    normFactorIntegral = h_x_z_integral->GetMaximum();

    for ( int i = 1; i <= h_x_z_chi2->GetNbinsX(); i++ )
    {
        for ( int j = 1; j <= h_x_z_chi2->GetNbinsY(); j++ )
        {
            double xVal = h_x_z_chi2->GetXaxis()->GetBinCenter ( i );
            double yVal = h_x_z_chi2->GetYaxis()->GetBinCenter ( j );

            double binContent = h_x_z_chi2->GetBinContent ( i,j ) / normFactorChi2;
            binContent += h_x_z_sigma->GetBinContent ( i,j ) / normFactorSigma;
            binContent += normFactorIntegral / h_x_z_integral->GetBinContent ( i,j );

            h_x_z_tot->Fill ( xVal, yVal, binContent );
        }
    }

    normFactorChi2 = h_y_z_chi2->GetMinimum();
    normFactorSigma = h_y_z_sigma->GetMinimum();
    normFactorIntegral = h_y_z_integral->GetMaximum();

    for ( int i = 1; i <= h_y_z_chi2->GetNbinsX(); i++ )
    {
        for ( int j = 1; j <= h_y_z_chi2->GetNbinsY(); j++ )
        {
            double xVal = h_y_z_chi2->GetXaxis()->GetBinCenter ( i );
            double yVal = h_y_z_chi2->GetYaxis()->GetBinCenter ( j );

            double binContent = h_y_z_chi2->GetBinContent ( i,j ) / normFactorChi2;
            binContent += h_y_z_sigma->GetBinContent ( i,j ) / normFactorSigma;
            binContent += normFactorIntegral / h_y_z_integral->GetBinContent ( i,j );

            h_y_z_tot->Fill ( xVal, yVal, binContent );
        }
    }

    TCanvas* fitResCan = new TCanvas ( "Fit_Results", "Fit Results" );

    fitResCan->Divide ( 3,4 );

    fitResCan->cd ( 1 );
    h_x_y_chi2->Draw ( "colz" );
    h_x_y_chi2->SetStats ( kFALSE );

    fitResCan->cd ( 2 );
    h_x_z_chi2->Draw ( "colz" );
    h_x_z_chi2->SetStats ( kFALSE );

    fitResCan->cd ( 3 );
    h_y_z_chi2->Draw ( "colz" );
    h_y_z_chi2->SetStats ( kFALSE );

    fitResCan->cd ( 4 );
    h_x_y_sigma->Draw ( "colz" );
    h_x_y_sigma->SetStats ( kFALSE );

    fitResCan->cd ( 5 );
    h_x_z_sigma->Draw ( "colz" );
    h_x_z_sigma->SetStats ( kFALSE );

    fitResCan->cd ( 6 );
    h_y_z_sigma->Draw ( "colz" );
    h_y_z_sigma->SetStats ( kFALSE );

    fitResCan->cd ( 7 );
    h_x_y_integral->Draw ( "colz" );
    h_x_y_integral->SetStats ( kFALSE );

    fitResCan->cd ( 8 );
    h_x_z_integral->Draw ( "colz" );
    h_x_z_integral->SetStats ( kFALSE );

    fitResCan->cd ( 9 );
    h_y_z_integral->Draw ( "colz" );
    h_y_z_integral->SetStats ( kFALSE );

    fitResCan->cd ( 10 );
    h_x_y_tot->Draw ( "colz" );
    h_x_y_tot->SetStats ( kFALSE );

    fitResCan->cd ( 11 );
    h_x_z_tot->Draw ( "colz" );
    h_x_z_tot->SetStats ( kFALSE );

    fitResCan->cd ( 12 );
    h_y_z_tot->Draw ( "colz" );
    h_y_z_tot->SetStats ( kFALSE );

    if ( bestFit == nullptr )
    {
        cerr << "Failed to determine a proper fit..." <<endl;
        return nullptr;
    }

    if ( !failedOnce && fabs ( bestChi2-bestFit->GetChisquare() ) /bestChi2 <= 0.5 )
    {
        failedOnce = true;
        goto FindBestFitLoop;
    }

    bestFitCan->cd();

    cout << "Best fit for graph " << bestOffName << endl;

    cout << "Fit Results: \n";
    for ( unsigned int i = 0; i < 4; i++ )
    {
        cout << "Peak #" << std::setw ( 2 ) << std::left << i << " : ";

        cout << bestFit->GetParameter ( 4 ) + ( i > 0 ? bestFit->GetParameter ( 4 + 2*i ) : 0 );
        cout << " +/- " << bestFit->GetParError ( 4 ) + bestFit->GetParError ( 4 + 2*i ) << " MeV";
        if ( i > 0 )
        {
            cout << " ( Energy difference = " << fabs ( bestFit->GetParameter ( 4 + 2*i ) ) << " +/- " << bestFit->GetParError ( 4 + 2*i );
            cout << " / Ratio = " << bestFit->GetParameter ( 3 + 2*i ) << " )";
        }

        cout << endl;

    }
    cout << "Sigma = " << bestFit->GetParameter ( 2 ) << " MeV\n";
    cout << "Chi2 = " << bestFit->GetChisquare() << "\n";

    auto besth = DrawGodHist ( bestHist, detStr );
    besth->GetXaxis()->SetRangeUser ( -2, 4 );
    TSpectrum* spec = new TSpectrum();
    auto backh = spec->Background ( besth,50 );
    besth->Add ( backh, -1 );
    besth->Draw();

    bestFit->SetRange ( -1, 3 );
    bestFit->Draw ( "same" );
    bestFit->SetNpx ( 500 );

    return bestFit;
}

void GoddessGeomUtils::WriteNewGeomGraphs ( string outFName, string opts )
{
    if ( outFName.empty() )
    {
        outFName = Form ( "NewGeomHists_QQQ5_%0.2f_%0.2f_%0.2f_SX3_%0.2f_%0.2f_%0.2f.root",
                          lastQQQ5Offsets[0], lastQQQ5Offsets[1], lastQQQ5Offsets[2], lastSX3Offsets[0], lastSX3Offsets[1], lastSX3Offsets[2] );
    }

    TFile* outf = new TFile ( outFName.c_str(), opts.c_str() );

    if ( !outf->IsOpen() )
    {
        cerr << "Failed to open file: " << outFName;
        return;
    }

    outf->cd();

    auto itr = hQval_NewGeom.begin();

    while ( itr != hQval_NewGeom.end() )
    {
        itr->second->Write();
        itr++;
    }

    auto itr2 = hEx_NewGeom.begin();

    while ( itr2 != hEx_NewGeom.end() )
    {
        itr2->second->Write();
        itr2++;
    }

    auto itr3 = hEvsA_SX3_NewGeom.begin();

    while ( itr3 != hEvsA_SX3_NewGeom.end() )
    {
        itr3->second->Write();
        itr3++;
    }

    auto itr4 = hEvsA_QQQ5_NewGeom.begin();

    while ( itr4 != hEvsA_QQQ5_NewGeom.end() )
    {
        itr4->second->Write();
        itr4++;
    }

    outf->Close();

    return;
}
