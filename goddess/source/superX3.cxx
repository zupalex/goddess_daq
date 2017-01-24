#include "superX3.h"

#include <cmath>
#include <iostream>

#include "TMath.h"

superX3::superX3()
{
    siDet::SetNumContacts ( 8, 4 );
    Clear();
}

superX3::superX3 ( std::string serial_Num, unsigned short sector_, unsigned short depth_,
                   bool up_Stream, SolidVector position ) :
    orrubaDet ( serial_Num, sector_, depth_, up_Stream, position )
{
    siDet::SetNumContacts ( 8, 4 );
    Clear();
}

superX3::~superX3() {}

/**Construct arrays of bin edges for ROOT histograms based on the provided position
 * of the detector. Currently constructs binning in the n, p and z direction in mm
 * and in the azimuthal and polar direction in degrees.
 *
 *
 * \note Assumes that the p type strips lie parallel to the z-axis and the n type
 * strips are perpendicular to the cylindrical radial dimension rho.
 */
void superX3::ConstructBins()
{
    for ( int i = 0; i < 4; i++ )
    {
        // We get the position of the strip center relative to the detector center assuming that the detector lies on the (X,Z) plane
        TVector3 pStPosRefDetCenter ( ( ( 3./8. ) * activeWidth ) - ( i * activeWidth/4. ), 0, 0 ); // Ref taken at the center of the SX3 so strip 0 offset is 1 and a half strip width toward positive X direction

        // We then rotate that vector by the detector rotation around the Z axis
        pStPosRefDetCenter.SetPhi ( pStPosRefDetCenter.Phi() + detPos.RotZ() );

        // The final position fo the center of the strip is the vector giving the detector center to which we add the vector going from the detector center to the strip center
        pStripCenterPos[i] = detPos.GetTVector3() + pStPosRefDetCenter;

        TVector3 nStPosRefDetCenter ( 0, 0, ( ( 3./8. ) * activeLength ) - ( i * activeLength/4. ) ); // Ref taken at the center of the SX3 so strip 0 offset is 1 and a half strip width toward positive Z direction

        nStripCenterPos[i] = detPos.GetTVector3() + nStPosRefDetCenter;
    }
}

void superX3::Clear()
{
    siDet::Clear();

    stripPosRaw.clear();
    stripPosCal.clear();
    for ( int i = 0; i < 4; i++ )
    {
        stripContactMult[i] = 0;
    }

    stripsP.clear();
    enNearRaw.clear();
    enFarRaw.clear();
    enNearCal.clear();
    enFarCal.clear();
    timeNear.clear();
    timeFar.clear();

    stripsN.clear();
    enRawN.clear();
    enCalN.clear();
    timeN.clear();

    eventPos.SetXYZ ( 0, 0, 0 );
}

/**Both the raw and calibrated positions are calculated as well as the energy
 * deposited in the strip. The energy is computed as
 * \f$ E = N + \sum_i p_i * F^i \f$, where \f$N\f$ and \f$F\f$ are the near and far
 * contacts respectively and \f$p_i\f$ is the ith coefficient of the polynomial
 * calibration function.
 *
 * \note The event position is only computed in the p strip multiplicity is 1. The
 * higher multiplicity events are not currently supported.
 *
 *  \param[in] strip The number of the strip to update the position.
 */
void superX3::UpdatePosition ( int strip )
{
    //Compute the near and far contacts
    int nearContact = 2 * strip, farContact = 2 * strip + 1;
    if ( strip > 1 )
    {
        nearContact = 2 * strip + 1;
        farContact = 2 * strip;
    }

    //Check if both contacts have had there energy set.
    //This does not work!
    if ( !ContactHit ( nearContact, false ) || !ContactHit ( farContact, false ) )
    {
        return;
    }

    //Collect energy values that were gain matched in siDet class
    float nearEnergy = GetCalEnergy ( nearContact );
    float farEnergy = GetCalEnergy ( farContact );

    //This works
    if ( ( nearEnergy == 0 || farEnergy == 0 ) || ( nearEnergy < 0 || farEnergy < 0 ) )
    {
        return;
    }

    float stripEnergy = nearEnergy + farEnergy;

    //Compute the resistive strip position by
    // Pos = (N - F) / E
    float stripPosRaw_ = ( nearEnergy - farEnergy ) / stripEnergy;
    float stripPosCal_ = stripPosRaw_;//put in position calibration parameters here

    stripPosRaw[strip] += stripPosRaw_;
    stripPosCal[strip] += stripPosCal_;

    //Compute the event position.
    //If the p and n strip multiplicity is 1 we can easily find the position.
    //if (enCalPstrip.size() == 1) {
    //We construct the event position from the following:
    //  X: The middle point of the pStrip
    //  Y: The middle point of the pStrip
    //  Z: The calibrated position reported from the resistive strip.
    float xValue = ( pStripEdgePos[strip].X() + pStripEdgePos[strip + 1].X() ) / 2.;
    float yValue = ( pStripEdgePos[strip].Y() + pStripEdgePos[strip + 1].Y() ) / 2.;
    eventPos.SetXYZ ( xValue, yValue, stripPosCal_ );
    //}
    //Otherwise we just zero the vector. Why???
    //else  eventPos.SetXYZ(0,0,0);

}
/**The near contacts are the one closest to the cable header.
 *
 * \param[in] strip The strip number for which we should compute the contact number.
 * \return The number of the contact nearest the header for the specified strip.

 * \The numbering of the contact is as followed
 * \near contact #0 ------------resistive strip #0------------ far contact #1
 * \near contact #2 ------------resistive strip #1------------ far contact #3
 * \near contact #5 ------------resistive strip #2------------ far contact #4
 * \near contact #7 ------------resistive strip #3------------ far contact #6
 */
unsigned short superX3::GetNearContact ( unsigned short strip )
{
    return strip > 1 ? 2 * strip + 1 : 2 * strip;
}

/**The far contacts are the one farthest to the cable header.
 *
 * \param[in] strip The strip number for which we should compute the contact number.
 * \return The number of the contact farthest form the header for the specified strip.

 * \The numbering of the contact is as followed
 * \near contact #0 ------------resistive strip #0------------ far contact #1
 * \near contact #2 ------------resistive strip #1------------ far contact #3
 * \near contact #5 ------------resistive strip #2------------ far contact #4
 * \near contact #7 ------------resistive strip #3------------ far contact #6
 */
unsigned short superX3::GetFarContact ( unsigned short strip )
{
    return strip > 1 ? 2 * strip : 2 * strip + 1;
}


void superX3::SetStripPosCalibPars ( int strip, std::vector<float> pars )
{
    if ( !ValidStrip ( strip ) )
    {
        fprintf ( stderr, "ERROR: Unable to assign strip position calibration parameters!\n" );
        return;
    }

    if ( pars.size() != 2 )
    {
        std::cerr << "ERROR: Number of calibration parameters specified for SX3 Position calibration incorrect (Expected 2, got " << pars.size() << ")" << std::endl;
    }

    parPosCal[strip] = pars;
}

void superX3::SetStripEnCalibPars ( int strip, std::vector<float> pars )
{
    if ( !ValidStrip ( strip ) )
    {
        fprintf ( stderr, "ERROR: Unable to assign strip energy calibration parameters!\n" );
        return;
    }

    parStripEnCal[strip] = pars;
}

/**
 * \param[in] strip The strip number to validate.
 *  \return True is strip exists.
 */
bool superX3::ValidStrip ( int strip )
{
    if ( strip < 0 || strip > 3 )
    {
        fprintf ( stderr, "ERROR: Invalid strip specification for illegal strip %d in detector.\n", strip );
        return false;
    }
    return true;
}

void superX3::SetGeomParams ( map< string, double > geomInfos_ )
{
    activeLength = geomInfos_["SuperX3 Active Length"];
    activeWidth = geomInfos_["SuperX3 Active Width"];
}

/**This method is called when a contact energy is updated. We call the parent
 * siDet::SetRawValue to handle storing the raw and calibrated value. If the update
 * was a p type contact we check if another p contact in the same strip has been set
 * and if so we make a call to compute the position the event occurred in the strip.
 *
 * \param[in] contact The number of the contact which was updated.
 *  \param[in] rawValue The raw contact value in channels.
 * \param[in] nType Whether the contact was n Type.
 */
void superX3::SetRawValue ( unsigned int contact, bool nType, int rawValue, int ignThr )
{
    if ( !ValidContact ( contact, nType ) )
    {
        char type = 'p';
        if ( nType )
        {
            type = 'n';
        }
        std::cerr << "ERROR: Unable to set raw value for Super X3 " << serialNum << " " << type << "-type contact: " << contact << "!\n";
        return;
    }

    siDet::SetRawValue ( contact, nType, rawValue, ignThr );
}

int superX3::GetStrip ( int contact )
{
    int strip = contact / 2;
    return strip;
}

void superX3::SetEnShiftVsPosGraph ( std::string graphFileName )
{
//     std::cerr << "Setting Energy Shift vs. Pos graphs...\n";

    for ( int i = 0; i < 4; i++ )
    {
        enShiftVsPosGraph[i] = nullptr;
    }

    if ( !graphFileName.empty() )
    {
        TFile* grFile = new TFile ( graphFileName.c_str(), "read" );

        if ( !grFile->IsOpen() )
        {
//             std::cerr << "File Energy Shift vs.Position not found...\n";
            return;
        }

        bool haveJumps = grFile->FindObjectAny ( "jumps" ) != nullptr;

        for ( int i = 0; i < 4; i++ )
        {
            grFile->cd();

            std::string detKey = Form ( "SuperX3_%c%d_%d", ( upStream ? 'U' : 'D' ), sector, i ) ;

            if ( grFile->FindObjectAny ( detKey.c_str() ) != nullptr )
            {
//                 std::cerr << "Found a Energy shift vs. Position graph for Super X3 " << ( upStream ? 'U' : 'D' ) << sector << " strip # " << i << " ...\n";

                enShiftVsPosGraph[i] = ( TGraph* ) ( grFile->FindObjectAny ( detKey.c_str() ) )->Clone();

                if ( haveJumps )
                {
//                     std::cerr << "The file contains info relative to jumps...\n";

                    std::string toFind = detKey + "_jump_at_";

//                     std::cerr << "Searching for graphs containing the following string: " << toFind << std::endl;

                    grFile->cd ( "/jumps" );

                    auto lOK = gDirectory->GetListOfKeys();

                    if ( lOK != nullptr )
                    {
                        TIterator* fItr = lOK->MakeIterator();
                        TObject* itrObj = fItr->Next();

                        while ( itrObj != nullptr )
                        {
                            TGraph* jumpsGr = dynamic_cast<TGraph*> ( grFile->FindObjectAny ( itrObj->GetName() ) );

                            if ( jumpsGr != nullptr )
                            {
                                std::string jumpGrName = jumpsGr->GetName();

//                                 std::cerr << "Found jump graph with name " << jumpGrName << std::endl;

                                if ( jumpGrName.find ( toFind ) != std::string::npos )
                                {
                                    double xA, yA, xB, yB;

                                    jumpsGr->GetPoint ( 0, xA, yA );
                                    jumpsGr->GetPoint ( 1, xB, yB );

//                                     double slope = ( yB - yA ) / ( xB - xA );
//                                     double offset = ( xB * yA - xA * yB ) / ( xB - xA );

                                    float jumpKey = std::stof ( jumpGrName.substr ( toFind.length() ) );

//                                     std::cerr << "Found jumps info for that detector at position " << jumpKey << std::endl;

//                                     enJumpsCorrectionGraphs[i][jumpKey] = std::make_pair ( slope, offset );

                                    TF1* fitf = new TF1 ( "linear func", "[0] * x + [1]", 0, 1000 );

                                    jumpsGr->Fit ( fitf, "QRM", "" );

                                    enJumpsCorrectionGraphs[i][jumpKey] = std::make_pair ( fitf->GetParameter ( 0 ), fitf->GetParameter ( 1 ) );
                                }
                            }

                            itrObj = fItr->Next();
                        }
                    }
                }
            }
            else
            {
//                 std::cerr << "No Energy shift vs. Position graph has been found for Super X3 " << ( upStream ? 'U' : 'D' ) << sector << " ...\n";
            }
        }

        grFile->Close();
    }

    return;
}

float superX3::GetNearEn ( bool calibrated )
{
    std::vector<float>* energies;

    if ( calibrated ) energies = &enNearCal;
    else energies = &enNearRaw;

    float near_en = 0.0;

    for ( unsigned int i = 0; i < energies->size(); i++ )
    {
        near_en += energies->at ( i );
    }

    return near_en;
}

float superX3::GetFarEn ( bool calibrated )
{
    std::vector<float>* energies;

    if ( calibrated ) energies = &enFarCal;
    else energies = &enFarRaw;

    float far_en = 0.0;

    for ( unsigned int i = 0; i < energies->size(); i++ )
    {
        far_en += energies->at ( i );
    }

    return far_en;
}

std::vector< float > superX3::GetResEn ( bool calibrated )
{
    std::vector<float> resEn;
    resEn.clear();

    std::vector<float>* toSumNear = 0;
    std::vector<float>* toSumFar = 0;

    if ( calibrated )
    {
        toSumNear = &enNearCal;
        toSumFar = &enFarCal;
    }
    else if ( !calibrated )
    {
        toSumNear = &enNearRaw;
        toSumFar = &enFarRaw;
    }

    if ( toSumNear->size() == toSumFar->size() )
    {
        for ( unsigned int i = 0; i < ( *toSumNear ).size(); i++ )
        {
            resEn.push_back ( toSumNear->at ( i ) + toSumFar->at ( i ) );
        }
    }

    return resEn;
}

float superX3::GetEnSum ( bool nType, bool calibrated )
{
    float enSum = 0;

    std::vector<float>* toSum;

    bool deletePtr = false;

    if ( nType && calibrated ) toSum = &enCalN;
    else if ( nType && !calibrated ) toSum = &enRawN;
    else if ( !nType && calibrated )
    {
        toSum = new std::vector<float>;
        *toSum = GetResEn ( true );
        deletePtr = true;
    }
    else if ( !nType && !calibrated )
    {
        toSum = new std::vector<float>;
        *toSum = GetResEn ( false );
        deletePtr = true;
    }
    else return 0.0;


    for ( unsigned int i = 0; i < ( *toSum ).size(); i++ )
    {
        enSum += toSum->at ( i );
    }

    if ( deletePtr ) delete ( toSum );

    return enSum;
}

void superX3::SortAndCalibrate ( bool doCalibrate )
{
    siDet::ValueMap enPMap, enNMap;

    siDet::TimeMap tsPMap, tsNMap;

    tsPMap = GetTsMap ( false );
    tsNMap = GetTsMap ( true );

    std::vector<int> alreadyTreatedStrips;
    alreadyTreatedStrips.clear();

    enPMap = GetRawEn ( false );

    for ( auto stripItr = enPMap.begin(); stripItr != enPMap.end(); ++stripItr )
    {
        int st_ = superX3::GetStrip ( stripItr->first );

        if ( std::find ( alreadyTreatedStrips.begin(), alreadyTreatedStrips.end(), st_ ) != alreadyTreatedStrips.end() ) continue;

        alreadyTreatedStrips.push_back ( st_ );

        int nearStrip = superX3::GetNearContact ( st_ );
        int farStrip = superX3::GetFarContact ( st_ );

        float en_near = 0.0;
        float en_far = 0.0;

        auto nearItr = enPMap.find ( nearStrip );
        auto farItr = enPMap.find ( farStrip );

        en_near = ( nearItr != enPMap.end() ) ? nearItr->second : 0.0;
        en_far = ( farItr != enPMap.end() ) ? farItr->second : 0.0;

        if ( en_near > 0.0 && en_far > 0.0 )
        {
            stripsP.push_back ( st_ );
            timeNear.push_back ( tsPMap[nearStrip] );
            timeFar.push_back ( tsPMap[farStrip] );

            enNearRaw.push_back ( en_near );
            enFarRaw.push_back ( en_far );

            if ( doCalibrate )
            {
                std::vector<std::vector<float>> calPars = GetEnParCal ( false );

                en_near = en_near * calPars[nearStrip][1] - calPars[nearStrip][0];
                en_far = en_far * calPars[farStrip][1] - calPars[farStrip][0];

                std::vector<float>* resStripParCal = GetResStripParCal();

                en_near = en_near * resStripParCal[st_].at ( 1 ) + resStripParCal[st_].at ( 0 ) / 2.;
                en_far = en_far * resStripParCal[st_].at ( 1 ) + resStripParCal[st_].at ( 0 ) / 2.;

                if ( en_near > 0.0 && en_far > 0.0 )
                {
                    enNearCal.push_back ( en_near );
                    enFarCal.push_back ( en_far );
                }
            }
        }
    }

    enNMap = GetRawEn ( true );

    for ( auto stripItr = enNMap.begin(); stripItr != enNMap.end(); ++stripItr )
    {
        int st_ = stripItr->first;
        float en_ = stripItr->second;

        stripsN.push_back ( st_ );
        enRawN.push_back ( en_ );
        timeN.push_back ( tsNMap[st_] );

        if ( doCalibrate )
        {
            std::vector<std::vector<float>> calPars = GetEnParCal ( true );

            enCalN.push_back ( en_ * calPars[st_][1] + calPars[st_][0] );
        }
    }
}

int superX3::GetContactMult ( bool calibrated )
{
    if ( calibrated ) return enNearCal.size() + enCalN.size();
    else return enNearRaw.size() + enRawN.size();
}

int superX3::GetContactMult ( bool contactType, bool calibrated )
{
    if ( contactType == siDet::nType )
    {
        if ( calibrated ) return enCalN.size();
        else return enRawN.size();
    }
    else
    {
        if ( calibrated ) return enNearCal.size();
        else return enNearRaw.size();
    }
}

std::vector< float > superX3::GetHitsInfo ( std::string info, std::vector<float>* dest )
{
    std::vector<float> request;

    if ( info == "near energies raw" ) request = enNearRaw;
    else if ( info == "far energies raw" ) request = enFarRaw;
    else if ( info == "back energies raw" ) request = enRawN;
    else if ( info == "near energies cal" ) request = enNearCal;
    else if ( info == "far energies cal" ) request = enFarCal;
    else if ( info == "back energies cal" ) request = enCalN;
    else std::cerr << "WARNING in superX3::GetHitsInfo -> requested member " << info << " does not exist!\n";

    if ( dest != nullptr ) *dest = request;

    return request;
}

std::vector< int > superX3::GetHitsInfo ( std::string info, std::vector<int>* dest )
{
    std::vector<int> request;

    if ( info == "front strips" ) request = stripsP;
    else if ( info == "back strips" ) request = stripsN;
    else std::cerr << "WARNING in superX3::GetHitsInfo -> requested member " << info << " does not exist!\n";

    if ( dest != nullptr ) *dest = request;

    return request;
}

std::vector< long long unsigned int > superX3::GetHitsInfo ( std::string info, std::vector< long long unsigned int >* dest )
{
    std::vector<long long unsigned int> request;

    if ( info == "near timestamps" ) request = timeNear;
    else if ( info == "far timestamps" ) request = timeFar;
    else if ( info == "back timestamps" ) request = timeN;
    else std::cerr << "WARNING in superX3::GetHitsInfo -> requested member " << info << " does not exist!\n";

    if ( dest != nullptr ) *dest = request;

    return request;
}


void superX3::GetMaxHitInfo ( int* stripMaxP, long long unsigned int* timeSampMaxP, int* stripMaxN, long long unsigned int* timeSampMaxN, bool calibrated )
{
    std::vector<float>* energiesN_;
    std::vector<float>* energiesNear_;
    std::vector<float>* energiesFar_;

    if ( calibrated )
    {
        energiesN_ = &enCalN;
        energiesNear_ = &enNearCal;
        energiesFar_ = &enFarCal;
    }
    else
    {
        energiesN_ = &enRawN;
        energiesNear_ = &enNearRaw;
        energiesFar_ = &enFarRaw;
    }

    float enMax = 0;



    for ( unsigned int i = 0; i < energiesNear_->size(); i++ )
    {
        if ( energiesNear_->at ( i ) + energiesFar_->at ( i ) > enMax )
        {
            if ( stripMaxP != nullptr ) *stripMaxP = stripsP.at ( i );
            enMax = energiesNear_->at ( i ) + energiesFar_->at ( i );
            if ( timeSampMaxP != nullptr ) *timeSampMaxP = ( timeNear.at ( i ) + timeFar.at ( i ) ) /2.;
        }
    }

    enMax = 0;

    for ( unsigned int i = 0; i < energiesN_->size(); i++ )
    {
        if ( energiesN_->at ( i ) > enMax )
        {
            if ( stripMaxN != nullptr ) *stripMaxN = stripsN.at ( i );
            enMax = energiesN_->at ( i );
            if ( timeSampMaxN != nullptr ) *timeSampMaxN = timeN.at ( i );
        }
    }
}

int superX3::GetMultiplicity ( bool nType, bool calibrated )
{
    if ( nType && calibrated ) return enCalN.size();
    else if ( nType && !calibrated ) return enRawN.size();
    else if ( !nType && calibrated ) return enNearCal.size();
    else if ( !nType && !calibrated ) return enNearRaw.size();
    else return 0;
}

TVector3 superX3::GetEventPosition ( bool calibrated )
{
    int pStripHit, nStripHit;
    GetMaxHitInfo ( &pStripHit, nullptr, &nStripHit, nullptr, calibrated );

    float eNear, eFar;

    eNear = GetNearEn ( calibrated );
    eFar = GetFarEn ( calibrated );

    float recenter = ( parPosCal[pStripHit].at ( 1 ) + parPosCal[pStripHit].at ( 0 ) ) / 2.;

    float normalize = parPosCal[pStripHit].at ( 1 ) - parPosCal[pStripHit].at ( 0 );

    normalize = ( normalize < 0.01 ) ? 1 : normalize;

    float zRes = ( ( ( eNear - eFar ) / ( eNear + eFar ) )  - recenter ) / normalize;

    if ( !upStream ) zRes *= -1;

    TVector3 zResPos ( 0, 0, zRes * activeLength );

    TVector3 interactionPos = pStripCenterPos[pStripHit] + zResPos;

    return interactionPos;
}


ClassImp ( superX3 )




