#include "BB10.h"
#include <cmath>
#include <iostream>
#include "TMath.h"

ClassImp ( BB10 )

BB10::BB10()
{
    siDet::SetNumContacts ( 8, 0 );
    Clear();
}
BB10::BB10 ( std::string serial_Num, unsigned short sector_, unsigned short depth_,
             bool up_Stream, SolidVector det_Pos ) :
    orrubaDet ( serial_Num, sector_, depth_, up_Stream, det_Pos )
{
    siDet::SetNumContacts ( 8, 0 );
    Clear();
}

/**Construct arrays of bin edges for ROOT histograms based on the provided position
 * of the detector. Currently constructs binning in the p and z direction in mm
 * and in the azimuthal and polar direction in degrees.
 *
 *
 * \note Assumes that the p type strips lie parallel to the z-axis and the detector
 * lies perpendicular to the cylindrical radial dimension rho.
 */
void BB10::ConstructBins()
{
    for ( int i = 0; i < 8; i++ )
    {
        TVector3 pStPosRefDetCenter ( ( ( 7./16. ) * activeWidth ) - ( i * activeWidth/8. ), 0, 0 ); // Ref taken at the center of the SX3 so strip 0 offset is 1 and a half strip width toward positive X direction

        pStPosRefDetCenter.SetPhi ( pStPosRefDetCenter.Phi() + detPos.RotZ() );

        pStripCenterPos[i] = detPos.GetTVector3() + pStPosRefDetCenter;
    }
}

void BB10::Clear()
{
    siDet::Clear();

    stripsP.clear();
    enRawP.clear();
    enCalP.clear();
    timeP.clear();

    eventPos.SetXYZ ( 0, 0, 0 );
}

void BB10::SetGeomParams ( map< string, double > geomInfos_ )
{
    activeWidth = geomInfos_["BB10 Active Width"];
}

/**This method is called when a contact energy is updated. We call the parent
 * siDet::SetRawValue to handle storing the raw and calibrated value. If the update
 * was a p type contact we check if another p contact in the same strip has been set
 * and if so we make a call to compute the position the event occurred in the strip.
 *
 * \param contact The number of the contact which was updated.
 *  \param rawValue The raw contact value in channels.
 * \param nType Whether the contact was n Type.
 */
void BB10::SetRawValue ( unsigned int contact, bool nType, int rawValue, int ignThr )
{
    siDet::SetRawValue ( contact, nType, rawValue, ignThr );
}

void BB10::SetEnShiftVsPosGraph ( std::string graphFileName )
{
    if ( !graphFileName.empty() )
    {
        TFile* grFile = new TFile ( graphFileName.c_str(), "read" );

        if ( !grFile->IsOpen() ) return;
    }
}

float BB10::GetEnSum ( bool nType, bool calibrated, float pos )
{
    float enSum = 0.0;

    std::vector<float>* toSum;

    if ( !nType && calibrated ) toSum = &enCalP;
    else if ( !nType && !calibrated ) toSum = &enRawP;
    else return 0.0;

    for ( unsigned int i = 0; i < ( *toSum ).size(); i++ )
    {
        enSum += toSum->at ( i );
    }
    
    enSum += pos;

    return enSum;
}

float BB10::GetPosCh(bool calibrated, bool in_encal, float e_near, float e_far)
{
  float posch = 0;
  if (calibrated){
  posch = 0;
  }
  else posch =0;
  
  return posch;
}

float BB10::UpdatePosCh(float posch)
{
  float pos = posch * 0;
  
  return pos;
}

void BB10::SortAndCalibrate ( bool doCalibrate )
{
    siDet::ValueMap enPMap;

    enPMap = GetRawEn ( false );

    siDet::TimeMap tsPMap;

    tsPMap = GetTsMap ( false );

    for ( auto stripItr = enPMap.begin(); stripItr != enPMap.end(); ++stripItr )
    {
        int st_ = stripItr->first;
        float en_ = stripItr->second;

        stripsP.push_back ( st_ );
        enRawP.push_back ( en_ );
        timeP.push_back ( tsPMap[st_] );

        if ( doCalibrate )
        {
            std::vector<std::vector<double>> calPars = GetEnParCal ( false );

            enCalP.push_back ( en_ * calPars[st_][1] + calPars[st_][0] );
        }
    }

//     if ( doCalibrate )
//     {
//         siDet::ValueMap enCalPMap;
//
//         enCalPMap = GetCalEn ( false );
//
//         for ( auto stripItr = enCalPMap.begin(); stripItr != enCalPMap.end(); ++stripItr )
//         {
//             enCalP.push_back ( stripItr->second );
//         }
//     }
}

int BB10::GetContactMult ( bool calibrated )
{
    if ( calibrated ) return enCalP.size();
    else return enRawP.size();
}

int BB10::GetContactMult ( bool /*contactType*/, bool calibrated )
{
    if ( calibrated ) return enRawP.size();
    else return enRawP.size();
}

std::vector< float > BB10::GetHitsInfo ( std::string info, std::vector<float>* dest )
{
    std::vector<float> request;

    if ( info == "front energies raw" ) request = enRawP;
    else if ( info == "front energies cal" ) request = enCalP;
    else if ( info == "back energies raw" ) request.clear();
    else if ( info == "back energies cal" ) request.clear();
    else std::cerr << "WARNING in BB10::GetHitsInfo -> requested member " << info << " does not exist!\n";

    if ( dest != nullptr ) *dest = request;

    return request;
}

std::vector< int > BB10::GetHitsInfo ( std::string info, std::vector<int>* dest )
{
    std::vector<int> request;

    if ( info == "front strips" ) request = stripsP;
    else if ( info == "back strips" ) request.clear();
    else std::cerr << "WARNING in BB10::GetHitsInfo -> requested member " << info << " does not exist!\n";

    if ( dest != nullptr ) *dest = request;

    return request;
}

std::vector< long long unsigned int > BB10::GetHitsInfo ( std::string info, std::vector<long long unsigned int>* dest )
{
    std::vector<long long unsigned int> request;

    if ( info == "front timestamps" ) request = timeP;
    else if ( info == "back timestamps" ) request.clear();
    else std::cerr << "WARNING in BB10::GetHitsInfo -> requested member " << info << " does not exist!\n";

    if ( dest != nullptr ) *dest = request;

    return request;
}

void BB10::GetMaxHitInfo ( int* stripMaxP, long long unsigned int* timeSampMaxP, int* /*stripMaxN*/, long long unsigned int* /*timeSampMaxN*/, bool calibrated )
{
    std::vector<float>* energiesP_;

    if ( calibrated ) energiesP_ = &enCalP;
    else energiesP_ = &enRawP;

    float enMax = 0.0;

    for ( unsigned int i = 0; i < energiesP_->size(); i++ )
    {
        if ( energiesP_->at ( i ) > enMax )
        {
            if ( stripMaxP != nullptr ) *stripMaxP = stripsP.at ( i );
            enMax = energiesP_->at ( i );
            if ( timeSampMaxP != nullptr ) *timeSampMaxP = timeP.at ( i );
        }
    }
}

int BB10::GetMultiplicity ( bool /*nType*/, bool calibrated )
{
    if ( calibrated ) return enCalP.size();
    else if ( !calibrated ) return enRawP.size();
    else return 0;
}

TVector3 BB10::GetEventPosition ( bool calibrated )
{
    int pStripHit = 0;
    GetMaxHitInfo ( &pStripHit, nullptr, nullptr, nullptr, calibrated );

    TVector3 interactionPos = pStripCenterPos[pStripHit];

    return interactionPos;
}

