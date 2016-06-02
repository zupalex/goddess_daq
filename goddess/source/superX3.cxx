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
    ConstructBins();
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
//     float pStripPitch = 40.3 / 4; //mm
//     float nStripPitch = 75 / 4; //mm
//
//     //float detRotation = TMath::PiOver2() + detPos.Theta();
//     float computedRotZ = detPos.Phi() + TMath::PiOver2();
//     if ( computedRotZ < 0 )
//     {
//         computedRotZ += TMath::TwoPi();
//     }
//     float angleDiff = detPos.RotZ() - computedRotZ;
//     if ( fabs ( angleDiff ) > 0.001 )
//     {
//         std::cerr << "WARNING: Detector " << serialNum << " is not normal to the XY radial direction! " << detPos.RotZ() << " != " << TMath::PiOver2() + detPos.Phi() << "\n";
//     }
//
//     SolidVector cornerPos ( detPos.X() - 2 * pStripPitch * cos ( detPos.RotZ() ) * cos ( detPos.RotPhi() ),
//                             detPos.Y() - 2 * pStripPitch * sin ( detPos.RotZ() ) * cos ( detPos.RotPhi() ),
//                             detPos.Z() - 2 * nStripPitch * cos ( detPos.RotPhi() ),
//                             detPos.RotZ(),
//                             detPos.RotPhi() );
//
//     for ( unsigned int strip = 0; strip <= 4; strip++ )
//     {
//         //compute the binning along the p and n type strip directions in mm.
//         binsP[strip] = strip * pStripPitch;
//         binsN[strip] = strip * nStripPitch;
//
//         nStripEdgePos[strip].SetXYZ (
//             cornerPos.X() + 2 * pStripPitch * cos ( cornerPos.RotZ() ) * cos ( detPos.RotPhi() ),
//             cornerPos.Y() + 2 * pStripPitch * sin ( cornerPos.RotZ() ) * cos ( detPos.RotPhi() ),
//             cornerPos.Z() + binsN[strip] * cos ( detPos.RotPhi() ) );
//         //The strips have x and y computed from detector vector projected onto the
//         //XY vector from the detector origin to the strip edge.
//         pStripEdgePos[strip].SetXYZ ( cornerPos.X() + binsP[strip] * cos ( cornerPos.RotZ() ) * cos ( detPos.RotPhi() ),
//                                       cornerPos.Y() + binsP[strip] * sin ( cornerPos.RotZ() ) * cos ( detPos.RotPhi() ),
//                                       cornerPos.Z() + 2 * nStripPitch * cos ( detPos.RotPhi() ) );
//
//         //Z Position of each nStrip edge
//         binsZ[strip] = nStripEdgePos[strip].Z();
//         //Azimuthal angle
//         binsAzimuthal[strip] = pStripEdgePos[strip].Phi();
//         if ( binsAzimuthal[strip] < 0 )
//         {
//             binsAzimuthal[strip] += TMath::TwoPi();
//         }
//         binsAzimuthal[strip] *= TMath::RadToDeg();
//         //Polar angle is computed in reverse as the higher strips are at smaller
//         //  angles and the list must be in ascending order.
//         binsPolar[4 - strip] = TMath::RadToDeg() * nStripEdgePos[strip].Theta();
//     }
//
//     for ( unsigned int strip = 0; strip < 4; strip++ )
//     {
//         binsPCenter[strip] = ( binsP[strip] + binsP[strip + 1] ) / 2.;
//         binsNCenter[strip] = ( binsN[strip] + binsN[strip + 1] ) / 2.;
//         binsZCenter[strip] = ( binsZ[strip] + binsZ[strip + 1] ) / 2.;
//         binsAzimuthalCenter[strip] = ( binsAzimuthal[strip] + binsAzimuthal[strip + 1] ) / 2.;
//     }

    /*
    #ifdef VERBOSE
        std::cout << serialNum << "\tcenter:\t";
        detPos.Print();
        std::cout << "\tcorner:\t";
        cornerPos.Print();
        std::cout << "\tpStrip:\t";
        pStripEdgePos[0].Print();
        std::cout << "\t       \t";
        pStripEdgePos[4].Print();
        std::cout << "\tnStrip:\t";
        nStripEdgePos[0].Print();
        std::cout << "\t       \t";
        nStripEdgePos[4].Print();
    #endif*/

    float SX3_width = 40.3; //mm
    float SX3_length = 75.; //mm

    for ( int i = 0; i < 4; i++ )
    {
        TVector3 pStPosRefDetCenter ( ( ( 3./8. ) * SX3_width ) - ( i * SX3_width/4. ), 0, 0 ); // Ref taken at the center of the SX3 so strip 0 offset is 1 and a half strip width toward positive X direction

        pStPosRefDetCenter.SetPhi ( pStPosRefDetCenter.Phi() + detPos.RotZ() );

        pStripCenterPos[i] = detPos.GetTVector3() + pStPosRefDetCenter;

        TVector3 nStPosRefDetCenter ( 0, 0, ( ( 3./8. ) * SX3_length ) - ( i * SX3_length/4. ) ); // Ref taken at the center of the SX3 so strip 0 offset is 1 and a half strip width toward positive Z direction

        nStripCenterPos[i] = detPos.GetTVector3() + nStPosRefDetCenter;
    }
}

void superX3::Clear()
{
    siDet::Clear();

    enCalPstrip.clear();
    stripPosRaw.clear();
    stripPosCal.clear();
    for ( int i = 0; i < 4; i++ )
    {
        stripContactMult[i] = 0;
    }

    enPtype = std::make_pair ( 0, 0.0 );
    enNtype = std::make_pair ( 0, 0.0 );
    enCal = 0;

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

    //Energy calibrations done here using parameters in front of enCal resStrip in goddess.config
    float stripCalEnergy = ( stripEnergy - parStripEnCal[strip].at ( 0 ) ) * parStripEnCal[strip].at ( 1 );
    enCalPstrip[strip] += stripCalEnergy;
    enPtype.first = strip;
    enPtype.second += stripCalEnergy;

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

    //Call parent method to handle calibration.
//     if ( nType ) siDet::SetRawValue ( contact, nType, rawValue, ignThr );
//     else siDet::SetRawValue ( contact, nType, rawValue, 2 );
    siDet::SetRawValue ( contact, nType, rawValue, ignThr );

//     if ( !nType ) UpdatePosition ( GetStrip ( contact ) );

    /*
    if ( nType )
    {
        //Set the energy value only if the multiplicity is 1.
        enNtype.first = contact;
        enNtype.second += GetCalEnergy ( contact, nType );
        enCal += GetCalEnergy ( contact, nType );
    }
    else
    {
        //Determine which strip this contact is in.
        int strip = GetStrip ( contact );
        stripContactMult[strip]++;

        //If more than one contact fired we can compute position and energy.
        if ( stripContactMult[strip] > 1 )
        {
            UpdatePosition ( GetStrip ( contact ) );
            //Store the energy only if the multiplicity is one.
            //enPtype += enCalPstrip[strip];//this is filled in UpdatePosition
            enCal += GetCalEnergy ( contact, nType );
        }
    }
    */
}

int superX3::GetStrip ( int contact )
{
    int strip = contact / 2;
    return strip;
}

TVector3 superX3::GetEventPosition ( int pStripHit, int nStripHit, float eNear, float eFar )
{
    float SX3_length = 75.; // mm

    TVector3 zResPos ( 0, 0, ( ( eNear - eFar ) / (eNear + eFar) ) * ( SX3_length/2. ) );

    TVector3 interactionPos = pStripCenterPos[pStripHit] + zResPos;

    return interactionPos;
}


ClassImp ( superX3 )


