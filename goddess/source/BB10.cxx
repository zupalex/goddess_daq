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
    ConstructBins();
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
    float BB10_width = 40.3; //mm
    float BB10_length = 75.; //mm

    for ( int i = 0; i < 8; i++ )
    {
        TVector3 pStPosRefDetCenter ( ( ( 7./16. ) * BB10_width ) - ( i * BB10_width/8. ), 0, 0 ); // Ref taken at the center of the SX3 so strip 0 offset is 1 and a half strip width toward positive X direction

        pStPosRefDetCenter.SetPhi ( pStPosRefDetCenter.Phi() + detPos.RotZ() );

        pStripCenterPos[i] = detPos.GetTVector3() + pStPosRefDetCenter;
    }
}

void BB10::Clear()
{
    siDet::Clear();
    enPtype = std::make_pair ( 0, 0.0 );

    eventPos.SetXYZ ( 0, 0, 0 );
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
void BB10::SetRawValue ( unsigned int contact, bool nType, int rawValue, int ignThr )
{
    siDet::SetRawValue ( contact, nType, rawValue, ignThr );

    if ( !nType )
    {
        enPtype.first = contact;
        enPtype.second += GetCalEnergy ( contact, nType );
    }
}

TVector3 BB10::GetEventPosition ( int pStripHit, int nStripHit, float eNear, float eFar )
{
    TVector3 interactionPos = pStripCenterPos[pStripHit];

    return interactionPos;
}

