#include "QQQ5.h"
#include <cmath>
#include <iostream>

#include "TMath.h"

QQQ5::QQQ5()
{
    siDet::SetNumContacts ( 32, 4 );
    Clear();
}
/**We assuming the position provided for the detector is at the radial center of the
 * detector and the clockwise is aligned at the rotation angle.
 */
QQQ5::QQQ5 ( std::string serial_Num, unsigned short sector_, unsigned short depth_,
             bool up_Stream, SolidVector position ) :
    orrubaDet ( serial_Num, sector_, depth_, up_Stream, position )
{
    siDet::SetNumContacts ( 32, 4 );
    ConstructBins();
    Clear();
}

QQQ5::~QQQ5() {}

/**Construct arrays of bin edges for ROOT histograms based on the provided position
 * of the detector. Currently constructs binning in the n, p and z direction in mm
 * and in the azimuthal and polar direction in degrees.
 *
 *
 * \note Assumes that the detector lies in the XY plane with the p strip normals
 * along the radial dimension rho.
 */
void QQQ5::ConstructBins()
{
//     float QQQ5_active_length = 56.8; // mm

    float firstStripWidth = 2.55;

    TVector3 firstStripOffset ( 0, 25.2 + firstStripWidth/2., 0 ); // everything in mm

    TVector3 prevStripRefDetCenter = firstStripOffset;

    pStripCenterPos[0] = detPos.GetTVector3() + firstStripOffset;

    for ( int i = 1; i < 32; i++ )
    {
        float prevStripWidth = firstStripWidth - ( i-1 ) * 0.05;
        float currStripWidth = firstStripWidth - i * 0.05;

        TVector3 pStPosRefDetCenter = prevStripRefDetCenter + TVector3 ( 0, ( prevStripWidth + currStripWidth ) / 2., 0 );
        prevStripRefDetCenter = pStPosRefDetCenter;

        pStPosRefDetCenter.SetPhi ( pStPosRefDetCenter.Phi() + detPos.RotZ() );

        pStripCenterPos[i] = detPos.GetTVector3() + pStPosRefDetCenter;
    }

//     for ( int i = 0; i < 4; i++ )
//     {
//         TVector3 nStPosRefDetCenter = firstStripOffset + TVector3 ( 0, QQQ5_active_length/2., 0 );
//
//         nStripCenterPos[i] = detPos.GetTVector3() + nStPosRefDetCenter;
//         nStripCenterPos[i].SetPhi ( nStripCenterPos[i].Phi() + ( 3 - 2*i ) /8. * TMath::PiOver4() );
//     }
}

void QQQ5::Clear()
{
    siDet::Clear();

    enPtype = std::make_pair ( 0, 0.0 );
    enNtype = std::make_pair ( 0, 0.0 );
    enCal = 0;

    eventPos.SetXYZ ( 0, 0, 0 );
}

void QQQ5::SetPosID()
{
    if ( upStream )
    {
        posID.append ( "U" );
    }
    else
    {
        posID.append ( "D" );
    }

    static const char *sectorCode[4] = {"A", "B", "C", "D"};
    posID.append ( sectorCode[sector] );

    posID.append ( "_" );

    if ( depth == 0 )
    {
        posID.append ( "dE" );
    }
    else if ( depth == 1 )
    {
        posID.append ( "E1" );
    }
    else if ( depth == 2 )
    {
        posID.append ( "E2" );
    }
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
void QQQ5::SetRawValue ( unsigned int contact, bool nType, int rawValue, int ignThr )
{
    if ( !ValidContact ( contact, nType ) )
    {
        ;
        char type = 'p';
        if ( nType )
        {
            type = 'n';
        }
        std::cerr << "ERROR: Unable to set raw value for QQQ5 " << serialNum << " " << type << "-type contact: " << contact << "!\n";
    }

    //Call parent method to handle calibration.
    siDet::SetRawValue ( contact, nType, rawValue, ignThr );

    /*
    if ( nType )
    {
        enNtype.first = contact;
        enNtype.second += GetCalEnergy ( contact, nType );
        enCal += GetCalEnergy ( contact, nType );
    }
    else
    {
        enPtype.first = contact;
        enPtype.second += GetCalEnergy ( contact, nType );
        enCal += GetCalEnergy ( contact, nType );
    }
    */
}

TVector3 QQQ5::GetEventPosition ( int pStripHit, int nStripHit, float eNear, float eFar )
{
    ( void ) eNear; // to prevent useless warning about this variable not being used currently...
    ( void ) eFar; // to prevent useless warning about this variable not being used currently...

    TVector3 interactionPos = pStripCenterPos[pStripHit];

    if ( nStripHit >= 0 ) interactionPos.SetPhi ( interactionPos.Phi() - 3./16. * TMath::Pi() + nStripHit/8. * TMath::Pi() );

    return interactionPos;
}


ClassImp ( QQQ5 )


