#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "GoddessStruct.h"

template<typename T> T* GetMemberFromName ( std::string name , SiDataBase* siDat )
{
    //Useless but this prevent warning that the variable is not used. I just don't know if I keep this function yet.
    if ( name.length() > 0 && siDat )
        return 0;

    else
        return 0;
}

template<typename T> T* GetMemberFromName ( std::string name , SiDataDetailed* siDat )
{
    if ( name == "dE_e_p" ) return ( T* ) &siDat->dE.en.p;
    else if ( name == "dE_e_n" ) return ( T* ) &siDat->dE.en.n;
    else if ( name == "E1_e_p" ) return ( T* ) &siDat->E1.en.p;
    else if ( name == "E1_e_n" ) return ( T* ) &siDat->E1.en.n;
    else if ( name == "E2_e_p" ) return ( T* ) &siDat->E2.en.p;
    else if ( name == "E2_e_n" ) return ( T* ) &siDat->E2.en.n;
    else if ( name == "dE_strip_p" ) return ( T* ) &siDat->dE.strip.p;
    else if ( name == "dE_strip_n" ) return ( T* ) &siDat->dE.strip.n;
    else if ( name == "E1_strip_p" ) return ( T* ) &siDat->E1.strip.p;
    else if ( name == "E1_strip_n" ) return ( T* ) &siDat->E1.strip.n;
    else if ( name == "E2_strip_p" ) return ( T* ) &siDat->E2.strip.p;
    else if ( name == "E2_strip_n" ) return ( T* ) &siDat->E2.strip.n;
    else if ( name == "dE_ts_p" ) return ( T* ) &siDat->dE.timestamp.p;
    else if ( name == "dE_ts_n" ) return ( T* ) &siDat->dE.timestamp.n;
    else if ( name == "E1_ts_p" ) return ( T* ) &siDat->E1.timestamp.p;
    else if ( name == "E1_ts_n" ) return ( T* ) &siDat->E1.timestamp.n;
    else if ( name == "E2_ts_p" ) return ( T* ) &siDat->E2.timestamp.p;
    else if ( name == "E2_ts_n" ) return ( T* ) &siDat->E2.timestamp.n;

    return GetMemberFromName<T> ( name , dynamic_cast<SiDataBase*> ( siDat ) );
}

SiDataBase::SiDataBase() {}

SiDataBase::~SiDataBase() {}

void SiDataBase::Clear()
{
    eSum.clear();
    stripMax.clear();
}

float* SiDataBase::SetMemberAddress ( std::string member, float** address )
{
    *address = GetMemberFromName<float> ( member, this );
    return *address;
}

int* SiDataBase::SetMemberAddress ( std::string member, int** address )
{
    *address = GetMemberFromName<int> ( member, this );
    return *address;
}

std::vector<float>* SiDataBase::SetMemberAddress ( std::string member, std::vector<float>** address )
{
    *address = GetMemberFromName<std::vector<float>> ( member, this );
    return *address;
}

std::vector<int>* SiDataBase::SetMemberAddress ( std::string member, std::vector<int>** address )
{
    *address = GetMemberFromName<std::vector<int>> ( member, this );
    return *address;
}

std::vector<unsigned long long int>* SiDataBase::SetMemberAddress ( std::string member, std::vector<unsigned long long int>** address )
{
    *address = GetMemberFromName<std::vector<unsigned long long int>> ( member, this );
    return *address;
}


float SiDataBase::ESumLayer ( short unsigned int layer, bool isNType ) const
{
    if ( eSum.size() > 0 )
    {
        for ( unsigned short i = 0; i < eSum.size(); i++ )
        {
            if ( stripMax[i] >= ( layer*100 + 300*isNType )  && stripMax[i] < ( layer*100 + 300*isNType ) + 100 )
            {
                return eSum[i];
            }
        }
    }

    return 0.0;
}

int SiDataBase::MultLayer ( short unsigned int layer, bool isNType ) const
{
    if ( mult.size() > 0 )
    {
        for ( unsigned short i = 0; i < mult.size(); i++ )
        {
            if ( stripMax[i] >= ( layer*100 + 300*isNType )  && stripMax[i] < ( layer*100 + 300*isNType ) + 100 )
            {
                return mult[i];
            }
        }
    }

    return 0;
}

int SiDataBase::StripMaxLayer ( short unsigned int layer, bool isNType ) const
{
    if ( stripMax.size() > 0 )
    {
        for ( unsigned short i = 0; i < stripMax.size(); i++ )
        {
            if ( stripMax[i] >= ( layer*100 + 300*isNType )  && stripMax[i] < ( layer*100 + 300*isNType ) + 100 )
            {
                return stripMax[i] - ( layer*100 + 300*isNType );
            }
        }
    }

    return -1;
}

long long unsigned int SiDataBase::TimestampMaxLayer ( short unsigned int layer, bool isNType ) const
{
    if ( timestampMax.size() > 0 )
    {
        for ( unsigned short i = 0; i < eSum.size(); i++ )
        {
            if ( stripMax[i] >= ( layer*100 + 300*isNType )  && stripMax[i] < ( layer*100 + 300*isNType ) + 100 )
            {
                return timestampMax[i];
            }
        }
    }

    return 0.0;
}

TVector3 SiDataBase::PosdE() const
{
    if ( pos.size() > 0 && stripMax.size() >= pos.size() )
    {
        for ( unsigned short i = 0; i < pos.size(); i++ )
        {
            if ( stripMax[i] >= 0  && stripMax[i] < 100 )
            {
                return pos[i];
            }
        }
    }

    return TVector3 ( 0, 0, 0 );
}

TVector3 SiDataBase::PosE1() const
{
    if ( pos.size() > 0 && stripMax.size() >= pos.size() )
    {
        for ( unsigned short i = 0; i < pos.size(); i++ )
        {
            if ( stripMax[i] >= 100  && stripMax[i] < 200 )
            {
                return pos[i];
            }
        }
    }

    return TVector3 ( 0, 0, 0 );
}

TVector3 SiDataBase::PosE2() const
{
    if ( pos.size() > 0 && stripMax.size() >= pos.size() )
    {
        for ( unsigned short i = 0; i < pos.size(); i++ )
        {
            if ( stripMax[i] >= 200  && stripMax[i] < 300 )
            {
                return pos[i];
            }
        }
    }

    return TVector3 ( 0, 0, 0 );
}

float SiDataBase::Angle ( short unsigned int layer, float targetOffX, float targetOffY, float targetOffZ ) const
{
    TVector3 targetPos = TVector3 ( targetOffX, targetOffY, targetOffZ );

    TVector3 beamDir ( 0, 0, 1 );

    if ( pos.size() > 0 && stripMax.size() >= pos.size() )
    {
        for ( unsigned short i = 0; i < pos.size(); i++ )
        {
            if ( stripMax[i] >= ( layer*100 )  && stripMax[i] < ( layer*100 ) + 100 )
            {
                return ( pos[i]-targetPos ).Angle ( beamDir ) * 180. / TMath::Pi();
            }
        }
    }

    return 0;
}

float SiDataBase::QValue ( GoddessReacInfos* rInfos_, float energy_, float angle_ )
{
    float Qval = -100;

    if ( rInfos_ == nullptr )
    {
        std::cerr << "Reaction info are not available!\n";
        abort();
    }

    if ( energy_ > 0.0 )
    {
        float amu = 931.5; // MeV

        if ( angle_ != 0 )
        {
            float mbeam = rInfos_->beamAtomicMass * amu;  // MeV
            float mrecoil = ( rInfos_->recoilAtomicMass == 0 ? ( rInfos_->beamA + rInfos_->targetA - rInfos_->ejecA ) * amu : rInfos_->recoilAtomicMass * amu );  // MeV

            float mejec = rInfos_->ejecAtomicMass * amu;

            Qval = ( 1+mejec/mrecoil ) * ( energy_ ) -
                   ( 1 - mbeam/mrecoil ) * ( rInfos_->beamEk ) -
                   2 * TMath::Sqrt ( mbeam*mejec* ( energy_ ) * ( rInfos_->beamEk ) ) / mrecoil * TMath::Cos ( angle_ );
        }
    }

    return Qval;
}

float SiDataBase::QValue ( float massBeam, float kBeam, float massTarget, float massEjec, float massRecoil ) const
{
    float energy = ESumLayer ( 1, false );  // MeV

    float Qval = -100;

    if ( energy > 0.0 )
    {
        float amu = 931.5; // MeV

        float labAngle = Angle ( 1 );  // degree

        if ( labAngle != 0 )
        {
            float mbeam = massBeam * amu;  // MeV
            float mrecoil = ( massRecoil == 0.0 ? ( massBeam + massTarget - massEjec ) * amu : massRecoil * amu );  // MeV

            float mejec = massEjec * amu;

            Qval = ( 1+mejec/mrecoil ) * ( energy ) - ( 1 - mbeam/mrecoil ) * ( kBeam ) - 2 * TMath::Sqrt ( mbeam*mejec* ( energy ) * ( kBeam ) ) / mrecoil * TMath::Cos ( labAngle * TMath::Pi() / 180. );
        }
    }

    return Qval;
}

ClassImp ( SiDataBase )


SiDataDetailed::SiDataDetailed() {}
SiDataDetailed::~SiDataDetailed() {}

void SiDataDetailed::Clear()
{
    SiDataBase::Clear();

    dE.Clear();
    E1.Clear();
    E2.Clear();

    SiDataBase::Clear();
}

float* SiDataDetailed::SetMemberAddress ( std::string member, float** address )
{
    *address = GetMemberFromName<float> ( member, this );
    return *address;
}

int* SiDataDetailed::SetMemberAddress ( std::string member, int** address )
{
    *address = GetMemberFromName<int> ( member, this );
    return *address;
}

std::vector<float>* SiDataDetailed::SetMemberAddress ( std::string member, std::vector<float>** address )
{
    *address = GetMemberFromName<std::vector<float>> ( member, this );
    return *address;
}

std::vector<int>* SiDataDetailed::SetMemberAddress ( std::string member, std::vector<int>** address )
{
    *address = GetMemberFromName<std::vector<int>> ( member, this );
    return *address;
}

std::vector<unsigned long long int>* SiDataDetailed::SetMemberAddress ( std::string member, std::vector<unsigned long long int>** address )
{
    *address = GetMemberFromName<std::vector<unsigned long long int>> ( member, this );
    return *address;
}

ClassImp ( SiDataDetailed )



ChValPair::ChValPair() {}
ChValPair::~ChValPair() {}

float ChValPair::GetFastCalEn ( std::map<unsigned short, std::pair<float, float>>* calibParams ) const
{
    if ( calibParams == NULL ) return value;

    float calVal;

    auto itr = calibParams->find ( channel );

    if ( itr != calibParams->end() )
        calVal = ( value - itr->second.first ) * itr->second.second;
    else
        calVal = value;

    return calVal;
}

ClassImp ( ChValPair )

ORRUBARawData::ORRUBARawData() {}
ORRUBARawData::~ORRUBARawData() {}

void ORRUBARawData::Clear()
{
    isDigital.clear();
    data.clear();
}

unsigned short ORRUBARawData::GetMultRange ( unsigned short beg, unsigned short end ) const
{
    unsigned short multi = 0;

    for ( unsigned short i = 0; i < data.size(); i++ )
    {
        if ( ( data.at ( i ) ).channel >= beg && ( data.at ( i ) ).channel <= end ) multi++;
    }

    return multi;
}


ClassImp ( ORRUBARawData )



GoddessGeomInfos::GoddessGeomInfos()
{

}

GoddessGeomInfos::GoddessGeomInfos ( std::map< std::string, double > geomInfos_ )
{
    barrelRadius = geomInfos_["Barrel Radius"];
    barrelLength = geomInfos_["Barrel Length"];

    targetLadderAngle = geomInfos_["Target Ladder Angle"];

    barrelLayerSpacing = geomInfos_["Barrel Layer Spacing"];
    endcapLayerSpacing = geomInfos_["Endcap Layer Spacing"];

    targetOffset = TVector3 ( geomInfos_["Beam Spot Offset X"], geomInfos_["Beam Spot Offset Y"], geomInfos_["Beam Spot Offset Z"] );
    superX3Offset = TVector3 ( geomInfos_["SuperX3 Offset X"], geomInfos_["SuperX3 Offset Y"], geomInfos_["SuperX3 Offset Z"] );
    qqq5Offset = TVector3 ( geomInfos_["QQQ5 Offset X"], geomInfos_["QQQ5 Offset Y"], geomInfos_["QQQ5 Offset Z"] );

    superX3ActiveLength = geomInfos_["SuperX3 Active Length"];
    superX3ActiveWidth = geomInfos_["SuperX3 Active Width"];

    bb10ActiveWidth = geomInfos_["BB10 Active Width"];

    qqq5FirstStripWidth = geomInfos_["QQQ5 First Strip Width"];
    qqq5DeltaPitch = geomInfos_["QQQ5 Delta Pitch"];

    qqq5RefPhi = geomInfos_["QQQ5 Ref Phi"];
    qqq5DeltaPhi = geomInfos_["QQQ5 Delta Phi"];

    qqq5RefRotz = geomInfos_["QQQ5 Ref Zrot"];
    qqq5DeltaRotz = geomInfos_["QQQ5 Delta Zrot"];

    superX3RefPhi = geomInfos_["SuperX3 Ref Phi"];
    superX3DeltaPhi = geomInfos_["SuperX3 Delta Phi"];

    superX3RefRotz = geomInfos_["SuperX3 Ref Zrot"];
    superX3DeltaRotz = geomInfos_["SuperX3 Delta Zrot"];
}

GoddessGeomInfos::~GoddessGeomInfos()
{

}

void GoddessGeomInfos::DumpInfo()
{
    std::cout << "This run has been treated with the following geometry:\n";
    std::cout << std::left << std::setw ( 22 ) << "Barrel Radius" << ": " << barrelRadius << "\n";
    std::cout << std::left << std::setw ( 22 ) << "Barrel Length" << ": " << barrelLength << "\n";

    std::cout << std::left << std::setw ( 22 ) << "Target Ladder Angle" << ": " << targetLadderAngle << "\n";

    std::cout << std::left << std::setw ( 22 ) << "Barrel Layer Spacing" << ": " << barrelLayerSpacing << "\n";
    std::cout << std::left << std::setw ( 22 ) << "Endcap Layer Spacing" << ": " << endcapLayerSpacing << "\n";

    std::cout << std::left << std::setw ( 22 ) << "Target Offset" << ": ( " << targetOffset.X()  << " , " << targetOffset.Y() << " , " << targetOffset.Z() << " )\n";
    std::cout << std::left << std::setw ( 22 ) << "SuperX3s Offset" << ": ( " << superX3Offset.X()  << " , " << superX3Offset.Y() << " , " << superX3Offset.Z() << " )\n";
    std::cout << std::left << std::setw ( 22 ) << "QQQ5s Offset" << ": ( " << qqq5Offset.X()  << " , " << qqq5Offset.Y() << " , " << qqq5Offset.Z() << " )\n";

    std::cout << std::left << std::setw ( 22 ) << "SuperX3 Active Length" << ": " << superX3ActiveLength << "\n";
    std::cout << std::left << std::setw ( 22 ) << "SuperX3 Active Width" << ": " << superX3ActiveWidth << "\n";

    std::cout << std::left << std::setw ( 22 ) << "BB10 Active Width" << ": " << bb10ActiveWidth << "\n";

    std::cout << std::left << std::setw ( 22 ) << "QQQ5 First Strip Width" << ": " << qqq5FirstStripWidth << "\n";
    std::cout << std::left << std::setw ( 22 ) << "QQQ5 Delta Pitch" << ": " << qqq5DeltaPitch << "\n";

    std::cout << std::left << std::setw ( 22 ) << "QQQ5 Ref. Phi" << ": " << qqq5RefPhi << "\n";
    std::cout << std::left << std::setw ( 22 ) << "QQQ5 Delta Phi" << ": " << qqq5DeltaPhi << "\n";

    std::cout << std::left << std::setw ( 22 ) << "QQQ5 Ref. RotZ" << ": " << qqq5RefRotz << "\n";
    std::cout << std::left << std::setw ( 22 ) << "QQQ5 Delta RotZ" << ": " << qqq5DeltaRotz << "\n";

    std::cout << std::left << std::setw ( 22 ) << "SuperX3 Ref. Phi" << ": " << superX3RefPhi << "\n";
    std::cout << std::left << std::setw ( 22 ) << "SuperX3 Delta Phi" << ": " << superX3DeltaPhi << "\n";

    std::cout << std::left << std::setw ( 22 ) << "SuperX3 Ref. RotZ" << ": " << superX3RefRotz << "\n";
    std::cout << std::left << std::setw ( 22 ) << "SuperX3 Delta RotZ" << ": " << superX3DeltaRotz << "\n";
}

double GoddessGeomInfos::GetMeasure ( std::string name )
{
    if ( name == "Barrel Radius" ) return barrelRadius;
    else if ( name == "Barrel Length" ) return barrelLength;
    else if ( name == "Target Ladder Angle" ) return targetLadderAngle;
    else if ( name == "Barrel Layer Spacing" ) return barrelLayerSpacing;
    else if ( name == "Endcap Layer Spacing" ) return endcapLayerSpacing;
    else if ( name == "Target Offset X" ) return targetOffset.X();
    else if ( name == "Target Offset Y" ) return targetOffset.Y();
    else if ( name == "Target Offset Z" ) return targetOffset.Z();
    else if ( name == "SuperX3s Offset X" || name == "SuperX3 Offset X" || name == "SX3s Offset X" || name == "SX3 Offset X" ) return superX3Offset.X();
    else if ( name == "SuperX3s Offset Y" || name == "SuperX3 Offset Y" || name == "SX3s Offset Y" || name == "SX3 Offset Y" ) return superX3Offset.Y();
    else if ( name == "SuperX3s Offset Z" || name == "SuperX3 Offset Z" || name == "SX3s Offset Z" || name == "SX3 Offset Z" ) return superX3Offset.Z();
    else if ( name == "QQQ5s Offset X" || name == "QQQ5 Offset X" ) return qqq5Offset.X();
    else if ( name == "QQQ5s Offset Y" || name == "QQQ5 Offset Y" ) return qqq5Offset.Y();
    else if ( name == "QQQ5s Offset Z" || name == "QQQ5 Offset Z" ) return qqq5Offset.Z();
    else if ( name == "SuperX3 Active Length" || name == "SX3 Active Length" ) return superX3ActiveLength;
    else if ( name == "SuperX3 Active Width" || name == "SX3 Active Width" ) return superX3ActiveWidth;
    else if ( name == "BB10 Active Width" ) return bb10ActiveWidth;
    else if ( name == "QQQ5 First Strip Width" ) return qqq5FirstStripWidth;
    else if ( name == "QQQ5 Delta Pitch" ) return qqq5DeltaPitch;
    else if ( name == "QQQ5 Ref. Phi" ) return qqq5RefPhi;
    else if ( name == "QQQ5 Delta Phi" ) return qqq5DeltaPhi;
    else if ( name == "QQQ5 Ref. RotZ" ) return qqq5RefRotz;
    else if ( name == "QQQ5 Delta RotZ" ) return qqq5DeltaRotz;
    else if ( name == "SuperX3 Ref. Phi" || name == "SX3 Ref. Phi" || name == "SuperX3 Ref Phi" || name == "SX3 Ref Phi" ) return superX3RefPhi;
    else if ( name == "SuperX3 Delta Phi" || name == "SX3 Delta Phi" ) return superX3DeltaPhi;
    else if ( name == "SuperX3 Ref. RotZ" || name == "SX3 Ref. RotZ" || name == "SuperX3 Ref RotZ" || name == "SX3 Ref RotZ" ) return superX3RefRotz;
    else if ( name == "SuperX3 Delta RotZ" || name == "SX3 Delta RotZ" ) return superX3DeltaRotz;
    else
    {
        std::cerr << "No parameters named " << name << " ...\n";
        return 0.0;
    }
}

TVector3 GoddessGeomInfos::GetOffset ( std::string name )
{
    if ( name == "Target Offset" ) return targetOffset;
    else if ( name == "SuperX3s Offset" || name == "SuperX3 Offset" || name == "SX3s Offset" || name == "SX3 Offset" ) return superX3Offset;
    else if ( name == "QQQ5s Offset" || name == "QQQ5 Offset" ) return qqq5Offset;
    else
    {
        std::cerr << "No parameters named " << name << " ...\n";
        return TVector3 ( 0, 0, 0 );
    }
}

GoddessReacInfos::GoddessReacInfos()
{
    qqq5EnGain = 1.0;
    sX3EnGain = 1.0;
}


GoddessReacInfos::GoddessReacInfos ( std::map< std::string, double > reacInfos_ )
{
    beamZ = reacInfos_["Beam Charge"];
    beamA = reacInfos_["Beam Mass"];
    targetZ = reacInfos_["Target Charge"];
    targetA = reacInfos_["Target Mass"];
    ejecZ = reacInfos_["Ejectile Charge"];
    ejecA = reacInfos_["Ejectile Mass"];

    beamAtomicMass = reacInfos_["Beam Atomic Mass"];
    targetAtomicMass = reacInfos_["Target Atomic Mass"];
    ejecAtomicMass = reacInfos_["Ejectile Atomic Mass"];

    if ( reacInfos_.find ( "Recoil Charge" ) != reacInfos_.end() ) recoilZ = reacInfos_["Recoil Charge"];
    else recoilZ = beamZ + targetZ - ejecZ;

    if ( reacInfos_.find ( "Recoil Mass" ) != reacInfos_.end() ) recoilA = reacInfos_["Recoil Mass"];
    else recoilA = beamA + targetA - ejecA;

    if ( reacInfos_.find ( "Recoil Atomic Mass" ) != reacInfos_.end() ) recoilAtomicMass = reacInfos_["Recoil Atomic Mass"];
    else
    {
        std::ifstream mdb ( "mass_db.dat" );

        string recAtomicForm;
        GetAtomicFormula ( mdb, recoilA, recoilZ, recAtomicForm );

        int dummyA, dummyZ;

        DecodeAtomicFormula ( mdb, recAtomicForm, dummyA, dummyZ, recoilAtomicMass );
    }

    beamEk = reacInfos_["Beam Energy"];

    qValGsGs = reacInfos_["Reaction Q-Value"];

    targetThickness = reacInfos_["Target Thickness"];

    targetDensity = reacInfos_["Target Density"];

    if ( reacInfos_.find ( "QQQ5 Gain Mod" ) != reacInfos_.end() ) qqq5EnGain = reacInfos_["QQQ5 Gain Mod"];
    else qqq5EnGain = 1.0;

    if ( reacInfos_.find ( "SuperX3 Gain Mod" ) != reacInfos_.end() ) sX3EnGain = reacInfos_["SuperX3 Gain Mod"];
    else sX3EnGain = 1.0;
}

GoddessReacInfos::~GoddessReacInfos()
{

}

void GoddessReacInfos::DumpInfo()
{
    std::cout << "This run is for the following reaction:\n";
    std::cout << std::left << std::setw ( 9 ) << "Beam:" << " Z = " << beamZ << " , A = " << beamA << " @ " << beamEk <<  " MeV\n";
    std::cout << std::left << std::setw ( 9 ) << "Target:" << " Z = " << targetZ << " , A = " << targetA << "\n";
    std::cout << std::left << std::setw ( 9 ) << "Ejectile:" << " Z = " << ejecZ << " , A = " << ejecA << "\n";
    std::cout << std::left << std::setw ( 9 ) << "Recoil:" << " Z = " << recoilZ << " , A = " << recoilA << "\n";
    std::cout << std::left << std::setw ( 9 ) << "Beam Atomic Mass: " << beamAtomicMass << "\n";
    std::cout << "Reaction Q-Value: " << qValGsGs << "\n";
    std::cout << "Target Type: " << targetType << "\n";
    std::cout << "Target Tickness: " << targetThickness << "\n";
    std::cout << "Target Density: " << targetDensity << "\n";
    std::cout << "QQQ5 gain modifier: " << qqq5EnGain << "\n";
    std::cout << "SuperX3 gain modifier: " << sX3EnGain << "\n";
}


ClassImp ( GoddessGeomInfos )
