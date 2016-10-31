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

float SiDataBase::Angle ( short unsigned int layer ) const
{
    if ( pos.size() > 0 && stripMax.size() >= pos.size() )
    {
        for ( unsigned short i = 0; i < pos.size(); i++ )
        {
            if ( stripMax[i] >= ( layer*100 )  && stripMax[i] < ( layer*100 ) + 100 )
            {
                return pos[i].Angle ( TVector3 ( 0, 0, 1 ) ) * 180. / TMath::Pi();
            }
        }
    }

    return 0;
}

float SiDataBase::QValue ( float massBeam, float kBeam, float massTarget, float massEjec, float recoilMass ) const
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
            float mrecoil = (recoilMass == 0.0 ? ( massBeam + massTarget - massEjec ) * amu : recoilMass); // MeV

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
