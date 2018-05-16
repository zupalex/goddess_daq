
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <string>

using namespace std;

#include "TSystem.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"

#include "TRandom3.h"
#include "TVector3.h"

#include "TStyle.h"
#include "TColor.h"

struct SolidAngleRes
{
    unsigned long long int n;

    unsigned int sx3_sects;

    map<string, unsigned long long int> hits_map;

    TGeoVolume* top;

    unsigned long long int GetHits ( string key )
    {
        auto fnd = hits_map.find ( key );

        if ( fnd != hits_map.end() ) return hits_map[key];
        else
        {
            unsigned long long int hits = 0;

            for ( auto itr = hits_map.begin(); itr != hits_map.end(); itr++ )
            {
                if ( itr->first.find ( key+" " ) != string::npos ) hits += itr->second;
            }

            return hits;
        }
    }

    double GetPerc ( string key )
    {
        return ( double ) GetHits ( key ) / ( double ) n;
    }

    double GetSolidAngle ( string key )
    {
        return GetPerc ( key ) * 4 * TMath::Pi();
    }

    double GetMaxSolidAngle()
    {
        double max = 0;

        for ( auto itr = hits_map.begin(); itr != hits_map.end(); itr++ )
        {
            if ( itr->second > max ) max = itr->second;
        }

        return ( double ) max / ( double ) n * 4 * TMath::Pi();
    }

    void WriteResults ( string outname )
    {
        fstream outf;
        outf.open ( outname.c_str(), fstream::trunc | fstream::out );

        for ( int i = 0; i < 12; i++ )
        {
            outf << "SX3 D " << i << " ndiv " << sx3_sects << endl;

            for ( unsigned int j = 0; j < sx3_sects; j++ )
            {
                outf << j << " " << GetSolidAngle ( ( string ) "SX3 D"+to_string ( i ) + " div "+to_string ( j ) )  << endl;
            }

            outf << endl;

            outf << "SX3 U " << i << " ndiv " << sx3_sects << endl;

            for ( unsigned int j = 0; j < sx3_sects; j++ )
            {
                outf << j << " " << GetSolidAngle ( ( string ) "SX3 U"+to_string ( i ) + " div "+to_string ( j ) ) << endl;
            }

            outf << endl;
        }

        for ( int i = 0; i < 4; i++ )
        {
            outf << "QQQ5 D " << i << endl;

            for ( int j = 0; j < 32; j++ )
            {
                outf << j << " " << GetSolidAngle ( ( string ) "QQQ5 D"+to_string ( i ) + " strip "+to_string ( j ) ) << endl;
            }

            outf << endl;

            outf << "QQQ5 U " << i << endl;

            for ( int j = 0; j < 32; j++ )
            {
                outf << j << " " << GetSolidAngle ( ( string ) "QQQ5 U"+to_string ( i ) + " strip "+to_string ( j ) ) << endl;
            }

            outf << endl;
        }

        outf.close();
    }
};

void PrintComputationProgress ( unsigned long long int i, unsigned long long int n )
{
    double progress = ( double ) i/n;

    int nsteps = 100;

    int step_processed = floor ( nsteps*progress );

    cout << " Progress: [";

    for ( int i = 0; i < step_processed; i++ ) cout << "=";
    for ( int i = 0; i < nsteps-step_processed; i++ ) cout << " ";

    cout << "] " << progress*100. << " %\r";
}

SolidAngleRes GetSolidAngle ( TVector3 beamOff = TVector3 ( 0, 0, 0 ) )
{
    gSystem->Load ( "libGeom" );

    TGeoManager* geoManager = new TGeoManager ( "world", "goddess" );

    TGeoMaterial *vac = new TGeoMaterial ( "vacuum", 0, 0, 0 );
    TGeoMedium *vac_med = new TGeoMedium ( "Vacuum", 1, vac );

    TGeoVolume* top = geoManager->MakeBox ( "Top", vac_med, 20., 20., 20. );
    geoManager->SetTopVolume ( top );

    top->SetLineColor ( kBlue );
    geoManager->SetTopVisible();

    TGeoMaterial *si = new TGeoMaterial ( "si", 28.086, 14, 2.3212 );
    TGeoMedium *simed = new TGeoMedium ( "Silicon", 2, si );

    double sx3_width = 4.03; // cm
    double sx3_length = 7.5; // cm
    double sx3_thickness = 0.05; // cm

    TVector3 sx3_offset ( 0.0, 0.48, 0.37 ); // cm

    double barrel_radius = 9.843; // cm
    double barrel_length = 22.86; // cm

    SolidAngleRes res;

    res.top = top;

    TGeoVolume* sx3_master = geoManager->MakeBox ( "SX3", simed, sx3_width/2., sx3_thickness/2., sx3_length/2. ); // All dimension are "half length".
    // So Without any translation, it would be cetered on 0 from -dimension to +dimension

    int ndiv = 100;

    res.sx3_sects = ndiv;

    TVector3 sx3_ref ( sx3_offset.X(), barrel_radius + sx3_offset.Y(), sx3_length/2. + sx3_offset.Z() ); // reference position =>SX3 D0

    TVector3 sx3_vec = sx3_ref;

    TGeoVolume* sx3s[24];

    for ( int i = 0; i < 12; i++ )
    {
        TGeoCombiTrans* sx3_tran = new TGeoCombiTrans ( sx3_vec.X(), sx3_vec.Y(), sx3_ref.Z(), new TGeoRotation ( "rot", i*TMath::Pi() /6. *TMath::RadToDeg(), 0, 0 ) );

        sx3s[i] = ( TGeoVolume* ) sx3_master->Clone();

        for ( int n = 0; n < ndiv; n++ )
        {
            sx3s[i]->AddNode ( geoManager->MakeBox ( "SX3", simed, sx3_width/2., sx3_thickness/2., sx3_length/ ( 2.*ndiv ) ), n, new TGeoTranslation ( 0, 0, -sx3_length/2. + n * sx3_length / ndiv ) );
        }

//         if ( ndiv > 1 ) sx3s[i]->Divide ( "sx3_angle_bin", 3, ndiv, -sx3_width/2., 0 );

        top->AddNode ( sx3s[i], i, sx3_tran );

        for ( int d = 0; d < ndiv; d++ )
        {
            string det_key = "SX3 D" + to_string ( i ) + " div " + to_string ( d );
            res.hits_map[det_key] = 0;
        }

        sx3_vec.SetPhi ( sx3_vec.Phi() + TMath::Pi() /6. );
    }

    sx3_vec = sx3_ref;
    sx3_vec.SetTheta ( TMath::Pi() - sx3_vec.Theta() );

    for ( int i = 0; i < 12; i++ )
    {
        TGeoCombiTrans* sx3_tran = new TGeoCombiTrans ( sx3_vec.X(), sx3_vec.Y(), sx3_vec.Z(), new TGeoRotation ( "rot", i*TMath::Pi() /6. *TMath::RadToDeg(), 0, 0 ) );

        sx3s[12+i] = ( TGeoVolume* ) sx3_master->Clone();

        for ( int n = 0; n < ndiv; n++ )
        {
            sx3s[12+i]->AddNode ( geoManager->MakeBox ( "SX3", simed, sx3_width/2., sx3_thickness/2., sx3_length/ ( 2.*ndiv ) ), n, new TGeoTranslation ( 0, 0, -sx3_length/2. + n * sx3_length / ndiv ) );
        }

// 	if ( ndiv > 1 ) sx3s[12+i]->Divide ( "sx3_angle_bin", 3, ndiv, -sx3_width/2., 0 );

        top->AddNode ( sx3s[12+i], 12+i, sx3_tran );

        for ( int d = 0; d < ndiv; d++ )
        {
            string det_key = "SX3 U" + to_string ( i ) + " div " + to_string ( d );
            res.hits_map[det_key] = 0;
        }

        sx3_vec.SetPhi ( sx3_vec.Phi() + TMath::Pi() /6. );
    }

    cout << "Created SX3s" << endl;

    double qqq5_thickness = 0.05; //cm
    double qqq5_fst_strip_width = 0.255; // cm
    double qqq5_fst_strip_inner = 2.52; // cm
    double qqq5_lst_strip_outer = 8.2; // cm
    double qqq5_delta_pitch = 0.005; // cm

    TVector3 qqq5_offset ( 0.0, 0.5, -1.65 ); // cm

    TVector3 qqq5_ref ( qqq5_offset.X(), qqq5_offset.Y(), barrel_length/2. + qqq5_offset.Z() ); // reference position => QQQ5 DA

    TGeoVolume* qqq5_master = geoManager->MakeCons ( "QQQ5", vac_med, qqq5_thickness, qqq5_fst_strip_inner, qqq5_lst_strip_outer, qqq5_fst_strip_inner, qqq5_lst_strip_outer, 0, 90 );

    TGeoVolume* qqq5_strips_master[32];

//     TGeoCombiTrans* qqq5_strip_tran = new TGeoCombiTrans ( qqq5_strip_off.X(), qqq5_strip_off.Y(), qqq5_strip_off.Z(), new TGeoRotation ( "qqq5_rot", 0, 0, 45 ) );

    double this_strip_y = qqq5_fst_strip_inner;

    TGeoVolume* qqq5s[8];

    for ( int q = 0; q < 8; q++ )
    {
        qqq5s[q] = ( TGeoVolume* ) qqq5_master->Clone();

        for ( int i = 0; i < 32; i++ )
        {
            double next_strip_y = this_strip_y + qqq5_fst_strip_width - i*qqq5_delta_pitch;

            if ( q == 0 )
            {
                qqq5_strips_master[i] = geoManager->MakeCons ( ( ( string ) ( "QQQ5_strip"+to_string ( i+1 ) ) ).c_str(), simed,
                                        qqq5_thickness, this_strip_y, next_strip_y , this_strip_y, next_strip_y,
                                        0, 90 );
            }

            this_strip_y = next_strip_y;

            qqq5s[q]->AddNode ( ( TGeoVolume* ) qqq5_strips_master[i]->Clone() , i );
        }
    }

    cout << "Created QQQ5s" << endl;


    TVector3 qqq5_quad_offset = qqq5_ref;

    for ( int j = 0; j < 2; j++ )
    {
        qqq5_quad_offset = qqq5_ref;

        if ( j==1 ) qqq5_quad_offset.SetTheta ( TMath::Pi() - qqq5_quad_offset.Theta() );

        for ( int i = 0; i < 4; i++ )
        {
            qqq5_quad_offset.SetPhi ( qqq5_ref.Phi() + i * TMath::Pi() / 2. );

            TGeoCombiTrans* qqq5_strip_tran = new TGeoCombiTrans ( qqq5_quad_offset.X(), qqq5_quad_offset.Y(), qqq5_quad_offset.Z(),
                    new TGeoRotation ( "qqq5_rot", 0, 0, 45 + 90*i ) );

            top->AddNode ( qqq5s[i+j*4], j*4+i, qqq5_strip_tran );

            for ( int s = 0; s < 32; s++ )
            {
                res.hits_map[ ( string ) "QQQ5 "+ ( j==0 ? "D" : "U" ) +to_string ( i ) +" strip "+to_string ( s )] = 0;
            }
        }
    }

//     TVector3 qqq5_strip_off;
//
//     for ( int theta_iter = 0; theta_iter < 2; theta_iter++ )
//     {
//         qqq5_strip_off = qqq5_ref;
//
//         qqq5_strip_off.SetTheta ( theta_iter == 0 ? qqq5_strip_off.Theta() : TMath::Pi() - qqq5_strip_off.Theta() );
//
//         for ( int phi_iter = 0; phi_iter < 4; phi_iter++ )
//         {
//             qqq5_strip_off.SetPhi ( qqq5_ref.Phi() + phi_iter * TMath::Pi() / 2. );
//
//             for ( int i = 0; i < 32; i++ )
//             {
//                 TGeoCombiTrans* qqq5_strip_tran = new TGeoCombiTrans ( qqq5_strip_off.X(), qqq5_strip_off.Y(), qqq5_strip_off.Z(),
//                         new TGeoRotation ( "qqq5_rot", 0, 0, 45 + 90*phi_iter ) );
//
//                 top->AddNode ( qqq5_strips[i], theta_iter*4 + phi_iter + 1, qqq5_strip_tran );
//
//                 det_hits_map[ ( ( string ) "QQQ5 "+ ( theta_iter == 0 ? "D" : "U" ) +to_string ( phi_iter%4 ) +" "+to_string ( i ) )] = 0;
//             }
//         }
//     }

    geoManager->CloseGeometry();

    TRandom3 rng ( 0 );

    TVector3 orig = beamOff;
    TVector3 dir ( 0,0,1 );

    TGeoNode* node;
    int det_id;

    unsigned long long int n = 1000000;

    res.n = n;

    for ( unsigned long long int i = 0; i < n; i++ )
    {
        if ( i%10000 == 0 ) PrintComputationProgress ( i,n );

        dir.SetTheta ( rng.Uniform ( 0, 2*TMath::Pi() ) );
        dir.SetPhi ( rng.Uniform ( 0, 2*TMath::Pi() ) );

        geoManager->InitTrack ( orig.X(),orig.Y(),orig.Z(), dir.X(),dir.Y(),dir.Z() );
        node = geoManager->FindNextBoundary();

        TGeoVolume* nodevol = node->GetVolume();

        if ( nodevol != top )
        {
            int det_num = node->GetNumber();

            string det_str;

            if ( ( string ) nodevol->GetName() == "SX3" )
            {
                node = geoManager->Step();

                int sx3_div = node->GetNumber();

                det_str = ( string ) "SX3 " + ( det_num >= 12 ? "U" : "D" ) + to_string ( det_num%12 ) + " div " + to_string ( sx3_div );

                res.hits_map[det_str]++;
            }
            else if ( ( string ) nodevol->GetName() == "QQQ5" )
            {
                node = geoManager->Step();

                int strip_num = node->GetNumber();

                det_str = ( string ) "QQQ5 " + ( det_num >= 4 ? "U" : "D" ) + to_string ( det_num%4 ) + " strip " + to_string ( strip_num );

                res.hits_map[det_str]++;
            }
        }
    }

    PrintComputationProgress ( n,n );

    cout << "\n\n";

    gStyle->SetPalette ( kLightTemperature );

    double sa_max = res.GetMaxSolidAngle();

    for ( int p = 0; p < 2; p++ )
    {
        for ( int q = 0; q < 4; q++ )
        {
            auto nodes = qqq5s[q+p*4]->GetNodes();

            for ( unsigned int n = 0; n < nodes->GetSize(); n++ )
            {
                string vol_key = ( string ) "QQQ5 " + ( p == 0 ? "D" : "U" ) + to_string ( q ) + " strip " + to_string ( n );

                TGeoNode* node = ( TGeoNode* ) nodes->At ( n );

                if ( node == nullptr ) break;

                node->GetVolume()->SetLineColor ( gStyle->GetColorPalette ( floor ( res.GetSolidAngle ( vol_key ) / sa_max * 254 ) ) );
            }
        }

        for ( int s = 0; s < 12; s++ )
        {
            auto nodes = sx3s[s+p*12]->GetNodes();

            for ( unsigned int n = 0; n < nodes->GetSize(); n++ )
            {
                string vol_key = ( string ) "SX3 " + ( p == 0 ? "D" : "U" ) + to_string ( s ) + " div " + to_string ( n );

                TGeoNode* node = ( TGeoNode* ) nodes->At ( n );

                if ( node == nullptr ) break;

                node->GetVolume()->SetLineColor ( gStyle->GetColorPalette ( floor ( res.GetSolidAngle ( vol_key ) / sa_max * 255 ) ) );
            }
        }
    }

    top->Draw ( "ogle" );

    top->SetVisRaytrace ( kFALSE );

//     cout << "Detector hits:\n";

//     for ( auto itr = det_hits_map.begin(); itr != det_hits_map.end(); itr++ )
//     {
//         cout << itr->first << " : " << itr->second << "  <=> " << ( itr->second / ( double ) n ) * 100. << " %\n";
//     }

    return res;
}








