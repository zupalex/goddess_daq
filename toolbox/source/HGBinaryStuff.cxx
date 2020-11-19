#include "GoddessStruct.h"

#include <string.h>
#include <iostream>
#include <string>
#include <sstream>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"
#include "TSystem.h"

using namespace std;

#define max_inputs 16 //define this as an option in sort. That way if there is ever any more Gretina crystals you can fix this by user instead of hardcode. 



struct GlobalHeader
{
    int32_t type;
    int32_t length;
    int64_t timestamp;
};

struct Gretina_header
{
    int type;
    int crystal_id;
    int num; //number of interaction points from decomp
    float tot_e;
    int core_e[4];
    long long int timestamp;
    long long trig_time;
    float t0;
    float cfd;
    float chisq;
    float norm_chisq;
    float baseline;
    float prestep;
    float poststep;
    int pad; //non0 gives error type
    struct
    {
        float x;
        float y;
        float z;
        float e;
        int seg;
        float seg_ener;
    } intpts[max_inputs];
};

struct Gretina_hit
{
    float x;
    float y;
    float z;
    float r;
    float theta;
    float phi;
    float e;
    int seg;
    float seg_ener;
    float tot_e;
    int quad;
    int crystal;
};


struct Gretina_Gamma_Ray
{
    float x;
    float y;
    float z;
    float e;
    float seg;
    float seg_ener;
};

vector<float> Coor_Trans_Gretina ( float R, float theta, float phi, float sphere_split )
{


    float x_1 = R*cos ( theta ) *sin ( phi );
    float y_1 = R*sin ( theta ) *sin ( phi );
    float z_1 = R* cos ( phi );

    float d = sphere_split/2;
    float x_2 = d; //remember to add a negative in the other part of the code for the opposite half of the sphere. aka cos90 vs cos180
    float y_2 = 0;
    float z_2 = 0;

    vector<float> c = {x_1+x_2, y_1+y_2,z_1+z_2};
    //float r_f = (c[0]^2+c[1]^2+c[2]^2)^(1/2);
    //float theta_f = (tan(c[1]/c[0]))^-1;
    //float phi_f = (cos(c[2]/r_f))^-1;

    //vector<float> c_new = {r_f,theta_f,phi_f};

    //This code returns the cartesian coordinates of the front face of the crystals, but you can set it to return the spherical coordates by uncommenting the above line.

    return c;
};

pair<float,float> Find_Original_Coor ( float hole_number, int det_number )
{
    ifstream file ( "Gretina_original_angles.csv", ifstream::in );

    string find_mount_hole;
    float mount;
    string phi;
    string theta;
    pair<float,float> angles;

    while ( !file.eof() )
    {
        getline ( file,find_mount_hole,',' );
        mount = atof ( find_mount_hole.c_str() );


        if ( mount == hole_number )
        {
            for ( int move_by = 0; move_by< ( det_number+1 ); move_by++ ) //the file has theta and phi for the mount position too
            {
                getline ( file,theta,',' );
                getline ( file,phi,',' );
            }
            angles.first = atof ( theta.c_str() );
            angles.second = atof ( phi.c_str() );
            break;
        }
    }
    return angles; // reads out first = theta second = phi
}


vector<float> Tot_Gam_Pos ( vector<float> new_face, float x, float y, float z )
{
    vector<float> new_pos;

    float tot_x = x + new_face[0];
    float tot_y = y + new_face[1];
    float tot_z = z + new_face[2];

    new_pos.push_back ( tot_x );
    new_pos.push_back ( tot_y );
    new_pos.push_back ( tot_z );

    float r = pow ( pow ( tot_x,2 ) +pow ( tot_y,2 ) +pow ( tot_z,2 ) , 1/2 );
    float theta = atan ( tot_y/tot_x );
    float phi = atan ( pow ( pow ( tot_x,2 ) +pow ( tot_y,2 ) ,1/2 ) /tot_z );

    new_pos.push_back ( r );
    new_pos.push_back ( theta );
    new_pos.push_back ( phi );

    return new_pos;
}

float Angle_Between_Vect ( float x1, float y1, float z1, float x2, float y2, float z2 )
{
    float dotproduct = x1*x2+y1*y2+z1*z2;
    float norm1 = pow ( pow ( x1,2 ) +pow ( y1,2 ) +pow ( z1,2 ),1/2 );
    float norm2 = pow ( pow ( x2,2 ) +pow ( y2,2 ) +pow ( z2,2 ),1/2 );

    float angle = acos ( dotproduct/ ( norm1*norm2 ) );

    return angle;
}

float Scattering_Angle ( float first_hit_e, float e_d )
{

    float angle = acos ( 1- ( ( 1/ ( first_hit_e-e_d ) )-1/first_hit_e ) *511*3e8 );

    return angle;
}

float DopplerCorrectSimple ( float e, float v, float theta )
{
    float Energy = e* ( 1- ( v/3e8 ) *cos ( theta ) ) /sqrt ( 1-pow ( v/3e8,2 ) );

    return Energy;
}


void ReadGretina ( string filename, string newfile, float sphere_split = 0 )
{

  
  //create headers to read Gretina
    Gretina_header header;
    GlobalHeader global_header;

    //create map to determine crystal coordinates
    pair<int,int> hole_det;
    pair<float,float> theta_phi;
    map<pair<int,int>,pair<float,float>> angles_map =
    {
        {make_pair<int,int> ( 1,0 ),make_pair<float,float> ( 35.3,19.58 ) },
        {make_pair<int,int> ( 1,1 ),make_pair<float,float> ( 47.6,42.3 ) },
        {make_pair<int,int> ( 1,2 ),make_pair<float,float> ( 30.6,54.72 ) },
        {make_pair<int,int> ( 1,3 ),make_pair<float,float> ( 16.65,19.58 ) },

        {make_pair<int,int> ( 2,0 ),make_pair<float,float> ( 35.3,91.58 ) },
        {make_pair<int,int> ( 2,1 ),make_pair<float,float> ( 47.6,11.43 ) },
        {make_pair<int,int> ( 2,2 ),make_pair<float,float> ( 30.6,126.72 ) },
        {make_pair<int,int> ( 2,3 ),make_pair<float,float> ( 16.65,91.58 ) },

        {make_pair<int,int> ( 3,0 ),make_pair<float,float> ( 35.3,163.58 ) },
        {make_pair<int,int> ( 3,1 ),make_pair<float,float> ( 47.6,186.3 ) },
        {make_pair<int,int> ( 3,2 ),make_pair<float,float> ( 30.6,198.72 ) },
        {make_pair<int,int> ( 3,3 ),make_pair<float,float> ( 16.65,163.58 ) },

        {make_pair<int,int> ( 4,0 ),make_pair<float,float> ( 35.3,235.58 ) },
        {make_pair<int,int> ( 4,1 ),make_pair<float,float> ( 47.6,258.3 ) },
        {make_pair<int,int> ( 4,2 ),make_pair<float,float> ( 30.6,270.72 ) },
        {make_pair<int,int> ( 4,3 ),make_pair<float,float> ( 16.65,235.58 ) },

        {make_pair<int,int> ( 5,0 ),make_pair<float,float> ( 35.3,307.58 ) },
        {make_pair<int,int> ( 5,1 ),make_pair<float,float> ( 47.6,330.3 ) },
        {make_pair<int,int> ( 5,2 ),make_pair<float,float> ( 30.6,342.2 ) },
        {make_pair<int,int> ( 5,3 ),make_pair<float,float> ( 16.65,307.58 ) },

        {make_pair<int,int> ( 6,0 ),make_pair<float,float> ( 67.71,74.63 ) },
        {make_pair<int,int> ( 6,1 ),make_pair<float,float> ( 55.02,91.23 ) },
        {make_pair<int,int> ( 6,2 ),make_pair<float,float> ( 48.92,68.77 ) },
        {make_pair<int,int> ( 6,3 ),make_pair<float,float> ( 64.18,54.55 ) },

        {make_pair<int,int> ( 7,0 ),make_pair<float,float> ( 67.71,146.63 ) },
        {make_pair<int,int> ( 7,1 ),make_pair<float,float> ( 55.02,163.23 ) },
        {make_pair<int,int> ( 7,2 ),make_pair<float,float> ( 48.92,140.77 ) },
        {make_pair<int,int> ( 7,3 ),make_pair<float,float> ( 64.18,126.55 ) },

        {make_pair<int,int> ( 8,0 ),make_pair<float,float> ( 67.71,218.63 ) },
        {make_pair<int,int> ( 8,1 ),make_pair<float,float> ( 55.02,235.23 ) },
        {make_pair<int,int> ( 8,2 ),make_pair<float,float> ( 48.92,212.77 ) },
        {make_pair<int,int> ( 8,3 ),make_pair<float,float> ( 64.18,198.55 ) },

        {make_pair<int,int> ( 9,0 ),make_pair<float,float> ( 67.71,290.63 ) },
        {make_pair<int,int> ( 9,1 ),make_pair<float,float> ( 55.02,307.23 ) },
        {make_pair<int,int> ( 9,2 ),make_pair<float,float> ( 48.92,284.77 ) },
        {make_pair<int,int> ( 9,3 ),make_pair<float,float> ( 64.18,270.55 ) },

        {make_pair<int,int> ( 10,0 ),make_pair<float,float> ( 67.71,2.63 ) },
        {make_pair<int,int> ( 10,1 ),make_pair<float,float> ( 55.02,19.23 ) },
        {make_pair<int,int> ( 10,2 ),make_pair<float,float> ( 48.92,356.77 ) },
        {make_pair<int,int> ( 10,3 ),make_pair<float,float> ( 64.18,342.55 ) },

        {make_pair<int,int> ( 11,0 ),make_pair<float,float> ( 87.15,8.71 ) },
        {make_pair<int,int> ( 11,1 ),make_pair<float,float> ( 105.79,13.65 ) },
        {make_pair<int,int> ( 11,2 ),make_pair<float,float> ( 92.85,27.29 ) },
        {make_pair<int,int> ( 11,3 ),make_pair<float,float> ( 74.21,22.35 ) },

        {make_pair<int,int> ( 12,0 ),make_pair<float,float> ( 97.01,47.25 ) },
        {make_pair<int,int> ( 12,1 ),make_pair<float,float> ( 100.78,66.39 ) },
        {make_pair<int,int> ( 12,2 ),make_pair<float,float> ( 82.99,60.75 ) },
        {make_pair<int,int> ( 12,3 ),make_pair<float,float> ( 79.22,41.61 ) },

        {make_pair<int,int> ( 13,0 ),make_pair<float,float> ( 87.15,80.71 ) },
        {make_pair<int,int> ( 13,1 ),make_pair<float,float> ( 105.79,85.65 ) },
        {make_pair<int,int> ( 13,2 ),make_pair<float,float> ( 92.85,99.29 ) },
        {make_pair<int,int> ( 13,3 ),make_pair<float,float> ( 74.21,94.35 ) },

        {make_pair<int,int> ( 14,0 ),make_pair<float,float> ( 97.01,119.25 ) },
        {make_pair<int,int> ( 14,1 ),make_pair<float,float> ( 100.78,138.39 ) },
        {make_pair<int,int> ( 14,2 ),make_pair<float,float> ( 82.99,132.75 ) },
        {make_pair<int,int> ( 14,3 ),make_pair<float,float> ( 79.22,113.61 ) },

        {make_pair<int,int> ( 15,0 ),make_pair<float,float> ( 87.15,152.71 ) },
        {make_pair<int,int> ( 15,1 ),make_pair<float,float> ( 105.79,157.65 ) },
        {make_pair<int,int> ( 15,2 ),make_pair<float,float> ( 92.85,171.29 ) },
        {make_pair<int,int> ( 15,3 ),make_pair<float,float> ( 74.21,166.35 ) },

        {make_pair<int,int> ( 16,0 ),make_pair<float,float> ( 97.01,191.25 ) },
        {make_pair<int,int> ( 16,1 ),make_pair<float,float> ( 100.78,210.39 ) },
        {make_pair<int,int> ( 16,2 ),make_pair<float,float> ( 82.99,204.75 ) },
        {make_pair<int,int> ( 16,3 ),make_pair<float,float> ( 79.22,185.61 ) },

        {make_pair<int,int> ( 17,0 ),make_pair<float,float> ( 87.15,224.71 ) },
        {make_pair<int,int> ( 17,1 ),make_pair<float,float> ( 105.79,229.65 ) },
        {make_pair<int,int> ( 17,2 ),make_pair<float,float> ( 92.85,243.29 ) },
        {make_pair<int,int> ( 17,3 ),make_pair<float,float> ( 74.21,238.35 ) },

        {make_pair<int,int> ( 18,0 ),make_pair<float,float> ( 97.01,263.25 ) },
        {make_pair<int,int> ( 18,1 ),make_pair<float,float> ( 100.78,282.39 ) },
        {make_pair<int,int> ( 18,2 ),make_pair<float,float> ( 82.99,276.75 ) },
        {make_pair<int,int> ( 18,3 ),make_pair<float,float> ( 79.22,257.61 ) },

        {make_pair<int,int> ( 19,0 ),make_pair<float,float> ( 87.15,296.71 ) },
        {make_pair<int,int> ( 19,1 ),make_pair<float,float> ( 105.79,301.65 ) },
        {make_pair<int,int> ( 19,2 ),make_pair<float,float> ( 92.85,315.29 ) },
        {make_pair<int,int> ( 19,3 ),make_pair<float,float> ( 74.21,310.35 ) },

        {make_pair<int,int> ( 20,0 ),make_pair<float,float> ( 97.01,335.25 ) },
        {make_pair<int,int> ( 20,1 ),make_pair<float,float> ( 100.78,354.39 ) },
        {make_pair<int,int> ( 20,2 ),make_pair<float,float> ( 82.99,348.75 ) },
        {make_pair<int,int> ( 20,3 ),make_pair<float,float> ( 79.22,310.35 ) },

        {make_pair<int,int> ( 21,0 ),make_pair<float,float> ( 112.29,33.37 ) },
        {make_pair<int,int> ( 21,1 ),make_pair<float,float> ( 124.98,16.77 ) },
        {make_pair<int,int> ( 21,2 ),make_pair<float,float> ( 131.08,39.23 ) },
        {make_pair<int,int> ( 21,3 ),make_pair<float,float> ( 115.82,53.45 ) },

        {make_pair<int,int> ( 22,0 ),make_pair<float,float> ( 112.29,105.37 ) },
        {make_pair<int,int> ( 22,1 ),make_pair<float,float> ( 124.98,88.77 ) },
        {make_pair<int,int> ( 22,2 ),make_pair<float,float> ( 131.08,111.23 ) },
        {make_pair<int,int> ( 22,3 ),make_pair<float,float> ( 115.82,125.45 ) },

        {make_pair<int,int> ( 23,0 ),make_pair<float,float> ( 112.29,177.37 ) },
        {make_pair<int,int> ( 23,1 ),make_pair<float,float> ( 124.98,160.77 ) },
        {make_pair<int,int> ( 23,2 ),make_pair<float,float> ( 131.08,183.23 ) },
        {make_pair<int,int> ( 23,3 ),make_pair<float,float> ( 115.82,197.45 ) },

        {make_pair<int,int> ( 24,0 ),make_pair<float,float> ( 112.29,249.37 ) },
        {make_pair<int,int> ( 24,1 ),make_pair<float,float> ( 124.98,232.77 ) },
        {make_pair<int,int> ( 24,2 ),make_pair<float,float> ( 131.08,255.23 ) },
        {make_pair<int,int> ( 24,3 ),make_pair<float,float> ( 115.82,269.45 ) },

        {make_pair<int,int> ( 25,0 ),make_pair<float,float> ( 112.29,321.37 ) },
        {make_pair<int,int> ( 25,1 ),make_pair<float,float> ( 124.98,304.77 ) },
        {make_pair<int,int> ( 25,2 ),make_pair<float,float> ( 131.08,327.23 ) },
        {make_pair<int,int> ( 25,3 ),make_pair<float,float> ( 115.82,341.45 ) },

        {make_pair<int,int> ( 26,0 ),make_pair<float,float> ( 144.7,88.42 ) },
        {make_pair<int,int> ( 26,1 ),make_pair<float,float> ( 132.4,65.7 ) },
        {make_pair<int,int> ( 26,2 ),make_pair<float,float> ( 149.4,53.28 ) },
        {make_pair<int,int> ( 26,3 ),make_pair<float,float> ( 163.35,88.42 ) },

        {make_pair<int,int> ( 27,0 ),make_pair<float,float> ( 144.7,160.42 ) },
        {make_pair<int,int> ( 27,1 ),make_pair<float,float> ( 132.4,137.7 ) },
        {make_pair<int,int> ( 27,2 ),make_pair<float,float> ( 149.4,125.8 ) },
        {make_pair<int,int> ( 27,3 ),make_pair<float,float> ( 163.35,160.42 ) },

        {make_pair<int,int> ( 28,0 ),make_pair<float,float> ( 144.7,232.42 ) },
        {make_pair<int,int> ( 28,1 ),make_pair<float,float> ( 132.4,209.7 ) },
        {make_pair<int,int> ( 28,2 ),make_pair<float,float> ( 149.4,197.8 ) },
        {make_pair<int,int> ( 28,3 ),make_pair<float,float> ( 163.35,232.42 ) },

        {make_pair<int,int> ( 29,0 ),make_pair<float,float> ( 144.7,304.42 ) },
        {make_pair<int,int> ( 29,1 ),make_pair<float,float> ( 132.4,281.7 ) },
        {make_pair<int,int> ( 29,2 ),make_pair<float,float> ( 149.4,269.28 ) },
        {make_pair<int,int> ( 29,3 ),make_pair<float,float> ( 163.35,304.42 ) },

        {make_pair<int,int> ( 30,0 ),make_pair<float,float> ( 144.7,16.42 ) },
        {make_pair<int,int> ( 30,1 ),make_pair<float,float> ( 132.4,253.7 ) },
        {make_pair<int,int> ( 30,2 ),make_pair<float,float> ( 149.4,341.28 ) },
        {make_pair<int,int> ( 30,3 ),make_pair<float,float> ( 163.35,16.42 ) }

    };

    map<pair<int,int>,pair<float,float>>::iterator itr;

    
    //vectors & associated structs to make branches
    vector<int> crystalID;
    vector<int> num_of_inter; //number of interaction points from decomp
    vector<float> tote;
    vector<float> e2;
    vector<int> timestamp;

    Gretina_Gamma_Ray Gamma;
    vector<Gretina_Gamma_Ray>* Gammas;
    vector<float> energy_temp;
    float tot_energy;
//
    
    //make the rootfile
    TFile* rootfile = new TFile ( newfile.c_str(),"recreate" );
    TTree* tree = new TTree ( "Gretina","Gretina" );
    tree->Branch("Crystal_id",&crystalID);
    tree->Branch("number_iteractions",&num_of_inter);
    tree->Branch("Timestamp", &timestamp);
    tree->Branch ( "raw_tot_en", &tote );
    tree->Branch ( "Tracked",&energy_temp );
    

    
    //various variables
    int32_t timestamp_0 = -1; //inital time for delta_t
    int32_t delta_t = -1; //computed length of gamma ray addback
    int32_t event_build_time = 25; //predetermined max gamma ray addback time
    vector<Gretina_hit> event; //one gamma ray vector
    Gretina_hit one_hit; //sub gamma ray struct
    float hole_num; //mount hole number for Gretina Quad
    float crystal_num; //crystal number in associated Quad 0-3
    pair<float,float> angles; //angles determined from Gretina map
    float R = 178.2; //radius of Gretina inner sphere
    vector<float> crystal_face; //coordinates of Gretina crystal face (first->theta, second->phi)
    int b = 0; //just a counter
    vector<float> tot_gamma_pos; //position of the sub gamma ray in relation to the target
    float highest_energy; //highest energy in gamma ray addback vector
    int hit; //the position of the highest energy sub gamma ray in the gamma ray addback vector
    float angle_between_hit; //angle between the determine first hit and the subsequent hits
    vector<Gretina_hit> second_event; //vector for if there is a second gamma ray in the gamma ray addback vector outside of 15 degrees
    vector<Gretina_hit> third_event; //see above, but if there is a third
    float weighted_x = 0; //weighted energy positon x
    float weighted_y = 0; //weighted energy position y
    float weighted_z = 0; //weighted energy position z
    float weighted_e = 0; //tot energy of the sub gamma rays

    //for reading the binary
    int readsuccess = 1;
    int buffsuccess = 1;
    
    
    //open the binary
    int fd = open ( filename.c_str(), O_RDONLY );

    if ( fd == -1 )
    {
        cout<<"Could not open file."<<endl;
        return;
    }

    cout<<"Opened file."<<endl;

    
    //while loop for reading through the binary file
    while ( readsuccess != 0 )
    {
      
      //read global header
        readsuccess = read ( fd, ( void* ) &global_header,sizeof ( global_header ) );

        if ( global_header.type != 1 )
        {
            cout<<" Global header not Type 1 data."<<endl;
            return;
        }

        
        //read data header
        buffsuccess = read ( fd, ( void* ) &header,sizeof ( header ) );
	
	//record initial info for branches
	crystalID.push_back(header.crystal_id);
	num_of_inter.push_back(header.num);
        tote.push_back ( header.tot_e );
	timestamp.push_back(header.timestamp);

	
	//determine position of crystal
        hole_num = header.crystal_id/4; //quad number.
        crystal_num = header.crystal_id%4; //crystal num 0-3 in quad
        hole_det.first = hole_num;
        hole_det.second = crystal_num;

        itr = angles_map.find ( hole_det );
        if ( itr != angles_map.end() )
        {
            theta_phi.first = itr->second.first;
            theta_phi.second = itr->second.second;
        }

        crystal_face = Coor_Trans_Gretina ( R,theta_phi.first,theta_phi.second,sphere_split );


	
	// if not the first timestamp in the file or a gamma ray below 100kev, compute delta_t
        if ( timestamp_0 != -1 )
        {
            delta_t = ( header.timestamp-timestamp_0 );
        }

        
        //if it is the first timestamp in the file or a gamma ray after a hit below 100kev
        if ( timestamp_0 == -1 )
        {
            if ( header.tot_e >515 ) //two methods for determining position based on tot_e. If above 515kev, use binary positions
            {
//
                for ( int hit1=0; hit1<header.num; hit1++ )
                {

                    if ( header.tot_e<100 ) //get rid of xray noise
                    {
                        continue;

                    }

                    tot_gamma_pos = Tot_Gam_Pos ( crystal_face, header.intpts[hit1].x, header.intpts[hit1].y, header.intpts[hit1].z ); //determine total pos of gamma ray with crystal face


                    one_hit.x = tot_gamma_pos[0];
                    one_hit.y = tot_gamma_pos[1];
                    one_hit.z = tot_gamma_pos[2];
                    one_hit.r = tot_gamma_pos[3];
                    one_hit.theta = tot_gamma_pos[4];
                    one_hit.phi = tot_gamma_pos[5];
                    tot_gamma_pos.clear();
                    one_hit.e = header.intpts[hit1].e;
                    one_hit.seg = header.intpts[hit1].seg;
                    one_hit.seg_ener = header.intpts[hit1].seg_ener;
                    one_hit.tot_e = header.tot_e;
                    one_hit.quad = hole_num;
                    one_hit.crystal = crystal_num;
                    event.push_back ( one_hit );

                    timestamp_0 = header.timestamp; 

                }
            }
            if ( header.tot_e<515 ) //if crystal tot_e is below 515kev, must use a weighted energy position.
            {
                for ( int whit1 = 0; whit1<header.num; whit1++ ) //determine weighted energies by running through all the sub gamma rays
                {

                    weighted_x += header.intpts[whit1].x*header.intpts[whit1].e;
                    weighted_y += header.intpts[whit1].y*header.intpts[whit1].e;
                    weighted_z += header.intpts[whit1].z*header.intpts[whit1].e;
                    weighted_e += header.intpts[whit1].e;
                }

                tot_gamma_pos = Tot_Gam_Pos ( crystal_face, weighted_x/weighted_e, weighted_y/weighted_e, weighted_z/weighted_e ); //determine hit pos of weighted energy position from the target.

                one_hit.x = tot_gamma_pos[0];
                //cout<<one_hit.x<<endl;
                one_hit.y = tot_gamma_pos[1];
                //cout<<one_hit.y<<endl;
                one_hit.z = tot_gamma_pos[2];
                //cout<<one_hit.z<<endl;
                one_hit.r = tot_gamma_pos[3];
                one_hit.theta = tot_gamma_pos[4];
                one_hit.phi = tot_gamma_pos[5];
                tot_gamma_pos.clear();
                one_hit.e = weighted_e;
                //cout<<one_hit.e<<endl;
                //one_hit.e = header.tot_e;
                one_hit.seg = 0;;
                one_hit.seg_ener = 0;
                one_hit.tot_e = header.tot_e;
                one_hit.quad = hole_num;
                one_hit.crystal = crystal_num;
                event.push_back ( one_hit );

                timestamp_0 = header.timestamp;
                weighted_e = 0;
                weighted_x = 0;
                weighted_y = 0;
                weighted_z = 0;
            }
        }

        if ( delta_t > event_build_time ) // this is where the gamma ray addback addition ends. aka where delta_t is too high.
        {
            highest_energy = 0;
            hit = 0;


//             ----------------------------------------------------
//             this is where the tracking code should go to run through each "event" or separated dt
//             -----------------------------------------------------



            for ( int interaction = 0; interaction<event.size(); interaction++ ) //find the highest energy of the sub gamma rays and use the highest energy as the first interaction
            {

                if ( event[interaction].e > highest_energy )
                {
                    highest_energy = event[interaction].e;

                    hit = interaction;
                }

            }
            
            tot_energy = event[hit].tot_e; //set first interaction position as the pos of tot_e


            for ( int i = 0; i<event.size(); i++ )
            {
                if ( i == hit ) //ignore the highest interaction
                {
                    continue;
                }

                if ( event[i].quad == event[hit].quad && event[i].crystal == event[hit].crystal )
                {
                    continue;
                }
                angle_between_hit = Angle_Between_Vect ( event[hit].x, event[hit].y, event[hit].z, event[i].x,event[i].y,event[i].z ); //determine how far away your sub gamma rays are. If within 15 degrees addback
                //angle_between_hit = Scattering_Angle(highest_energy,event[i].e);

                if ( angle_between_hit> 15 || angle_between_hit<-15 ) //outside of 15 degrees save for second gamma ray
                {
                    one_hit.x = event[i].x;
                    one_hit.y = event[i].y;
                    one_hit.z = event[i].z ;
                    one_hit.r = event[i].r;
                    one_hit.theta = event[i].theta;
                    one_hit.phi = event[i].phi;
                    one_hit.e = event[i].e;
                    one_hit.seg = event[i].seg;
                    one_hit.seg_ener = event[i].seg_ener;
                    one_hit.tot_e = event[i].tot_e;
                    one_hit.quad = event[i].quad;
                    one_hit.crystal = event[i].crystal;
                    second_event.push_back ( one_hit );

                    continue;
                }

                tot_energy += event[i].tot_e; //addback

            }
            Gamma.e = tot_energy;


            Gamma.x = event[hit].x;
            Gamma.y = event[hit].y;
            Gamma.z = event[hit].z;
            Gamma.seg = event[hit].seg;
            Gamma.seg_ener = event[hit].seg_ener;
            Gammas->push_back ( Gamma );
            energy_temp.push_back ( Gamma.e );

	    
	    //reset variables to zero so as to use in upcoming event
            tot_energy = 0;
            highest_energy = 0;



	    //repeat above but for sub gamma rays saved for second event
            if ( second_event.size() != 0 )
            {
                for ( int set = 0; set<second_event.size(); set++ )
                {
                    if ( second_event[set].e > highest_energy )
                    {
                        highest_energy = second_event[set].e;

                        hit = set;
                    }
                }

                tot_energy = event[hit].tot_e;

                for ( int ff = 0; ff<second_event.size(); ff++ )
                {
                    if ( ff == hit || event[hit].e == highest_energy )
                    {
                        continue;
                    }

                    if ( event[ff].quad == event[hit].quad && event[ff].crystal == event[hit].crystal )
                    {
                        continue;
                    }
                    angle_between_hit = Angle_Between_Vect ( event[hit].x, event[hit].y, event[hit].z, event[ff].x,event[ff].y,event[ff].z );
                    if ( angle_between_hit> 15 || angle_between_hit<-15 )
                    {
                        one_hit.x = second_event[ff].x;
                        one_hit.y = second_event[ff].y;
                        one_hit.z = second_event[ff].z ;
                        one_hit.r = second_event[ff].r;
                        one_hit.theta = second_event[ff].theta;
                        one_hit.phi = second_event[ff].phi;
                        one_hit.e = second_event[ff].e;
                        one_hit.seg = second_event[ff].seg;
                        one_hit.seg_ener = second_event[ff].seg_ener;
                        third_event.push_back ( one_hit );

                        continue;
                    }

                    tot_energy += second_event[ff].tot_e;
                }
                Gamma.e = tot_energy;

                Gamma.x = second_event[hit].x;
                Gamma.y = second_event[hit].y;
                Gamma.z = second_event[hit].z;
                Gamma.seg = second_event[hit].seg;
                Gamma.seg_ener = second_event[hit].seg_ener;
                Gammas->push_back ( Gamma );
                energy_temp.push_back ( Gamma.e );

                tot_energy = 0;
                highest_energy = 0;
            }

            
            //repeat again for the chance of a third event. However, do not keep for fourth event. Fourth event is very unlikely.
            if ( third_event.size() != 0 )
            {
                for ( int settwo= 0; settwo<event.size(); settwo++ )
                {
                    if ( event[settwo].e > highest_energy )
                    {
                        highest_energy = event[settwo].e;

                        hit = settwo;
                    }
                }
                
                tot_energy = event[hit].tot_e;

                for ( int fff = 0; fff<third_event.size(); fff++ )
                {
                    if ( fff == hit )
                    {
                        continue;
                    }

                    if ( event[fff].quad == event[hit].quad && event[fff].crystal == event[hit].crystal )
                    {
                        continue;
                    }
                    angle_between_hit = Angle_Between_Vect ( event[hit].x, event[hit].y, event[hit].z, event[fff].x,event[fff].y,event[fff].z );
                    if ( angle_between_hit> 15 )
                    {
                        cout<<"Sub-gamma would be in a fourth event."<<endl;
                        continue;
                    }

                    tot_energy += third_event[fff].e;

                }
                Gamma.e = tot_energy;

                Gamma.x = third_event[hit].x;
                Gamma.y = third_event[hit].y;
                Gamma.z = third_event[hit].z;
                Gamma.seg = third_event[hit].seg;
                Gamma.seg_ener = third_event[hit].seg_ener;
                Gammas->push_back ( Gamma );
                energy_temp.push_back ( Gamma.e );

                tot_energy = 0;
            }

            //clear vectors to use for next gamma ray sequence
            event.clear();
            second_event.clear();
            third_event.clear();

	    //save the information from the current header for the next gamma ray if over 100kev.
            for ( int hit3=0; hit3<header.num; hit3++ )
            {
                if ( header.tot_e<100 )
                {
                    continue;
                    timestamp_0 = -1;
                }

                if ( header.tot_e > 515 )
                {

                    tot_gamma_pos = Tot_Gam_Pos ( crystal_face, header.intpts[hit3].x, header.intpts[hit3].y, header.intpts[hit3].z );
                    one_hit.x = tot_gamma_pos[0];
                    one_hit.y = tot_gamma_pos[1];
                    one_hit.z = tot_gamma_pos[2];
                    one_hit.r = tot_gamma_pos[3];
                    one_hit.theta = tot_gamma_pos[4];
                    one_hit.phi = tot_gamma_pos[5];
                    tot_gamma_pos.clear();
                    one_hit.e = header.tot_e;
                    one_hit.seg = header.intpts[hit3].seg;
                    one_hit.seg_ener = header.intpts[hit3].seg_ener;
                    one_hit.tot_e = header.tot_e;
                    one_hit.quad = hole_num;
                    one_hit.crystal = crystal_num;
                    event.push_back ( one_hit );

                    timestamp_0 = header.timestamp;
                }
            }
            if ( header.tot_e<515 )
            {
                for ( int thit1 = 0; thit1<header.num; thit1++ )
                {

                    weighted_x += header.intpts[thit1].x*header.intpts[thit1].e;
                    weighted_y += header.intpts[thit1].y*header.intpts[thit1].e;
                    weighted_z += header.intpts[thit1].z*header.intpts[thit1].e;
                    weighted_e += header.intpts[thit1].e;
                }

                tot_gamma_pos = Tot_Gam_Pos ( crystal_face, weighted_x/weighted_e, weighted_y/weighted_e, weighted_z/weighted_e );

                one_hit.x = tot_gamma_pos[0];
                one_hit.y = tot_gamma_pos[1];
                one_hit.z = tot_gamma_pos[2];
                one_hit.r = tot_gamma_pos[3];
                one_hit.theta = tot_gamma_pos[4];
                one_hit.phi = tot_gamma_pos[5];
                tot_gamma_pos.clear();
                one_hit.e = weighted_e;
                one_hit.seg = 0;;
                one_hit.seg_ener = 0;
                one_hit.tot_e = header.tot_e;
                one_hit.quad = hole_num;
                one_hit.crystal = crystal_num;
                event.push_back ( one_hit );

                timestamp_0 = header.timestamp;
                weighted_e = 0;
                weighted_x = 0;
                weighted_y = 0;
                weighted_z = 0;
            }

            delta_t = -1;

        }
        
        //If delta_t is within event build time add to the current gamma ray
        if ( delta_t>=0 && delta_t<event_build_time )
        {

            for ( int hit2=0; hit2<header.num; hit2++ )
            {
                if ( header.intpts[hit2].e == 0 )
                {
                    continue;
                }

                if ( header.tot_e > 515 )
                {

                    tot_gamma_pos = Tot_Gam_Pos ( crystal_face, header.intpts[hit2].x, header.intpts[hit2].y, header.intpts[hit2].z );
                    one_hit.x = tot_gamma_pos[0];
                    one_hit.y = tot_gamma_pos[1];
                    one_hit.z = tot_gamma_pos[2];
                    one_hit.r = tot_gamma_pos[3];
                    one_hit.theta = tot_gamma_pos[4];
                    one_hit.phi = tot_gamma_pos[5];
                    tot_gamma_pos.clear();
                    one_hit.e = header.tot_e;
                    one_hit.seg = header.intpts[hit2].seg;
                    one_hit.seg_ener = header.intpts[hit2].seg_ener;
                    one_hit.tot_e = header.tot_e;
                    one_hit.quad = hole_num;
                    one_hit.crystal = crystal_num;
                    event.push_back ( one_hit );

                    timestamp_0 = header.timestamp;
                }
            }
            if ( header.tot_e<515 )
            {
                for ( int thit2 = 0; thit2<header.num; thit2++ )
                {

                    weighted_x += header.intpts[thit2].x*header.intpts[thit2].e;
                    weighted_y += header.intpts[thit2].y*header.intpts[thit2].e;
                    weighted_z += header.intpts[thit2].z*header.intpts[thit2].e;
                    weighted_e += header.intpts[thit2].e;
                }

                tot_gamma_pos = Tot_Gam_Pos ( crystal_face, weighted_x/weighted_e, weighted_y/weighted_e, weighted_z/weighted_e );

                one_hit.x = tot_gamma_pos[0];
                one_hit.y = tot_gamma_pos[1];
                one_hit.z = tot_gamma_pos[2];
                one_hit.r = tot_gamma_pos[3];
                one_hit.theta = tot_gamma_pos[4];
                one_hit.phi = tot_gamma_pos[5];
                tot_gamma_pos.clear();
                one_hit.e = weighted_e;
                one_hit.seg = 0;;
                one_hit.seg_ener = 0;
                one_hit.tot_e = header.tot_e;
                one_hit.quad = hole_num;
                one_hit.crystal = crystal_num;
                event.push_back ( one_hit );

                timestamp_0 = header.timestamp;
                weighted_e = 0;
                weighted_x = 0;
                weighted_y = 0;
                weighted_z = 0;
            }

        }


        b++; //add to the counter to see code progress

        if ( ( int ) b%20000 == 0 )
        {
            cout<<"Event number: "<<b<<"\r"<<flush;
        }
    }

    cout<<"Finished running through file."<<endl;

    tree->Fill();
    tree->Write();

    rootfile->Close();

    return;
};





struct header
{
    int type;
    int payloadsize;
    unsigned long long int timestamp;
};

void ReadDigitalHeader ( string filename )
{
    int fd = open ( filename.c_str(), O_RDONLY );

    if ( fd==-1 )
    {
        cout<<"Could not open file."<<endl;
        return;
    }

    cout<<"Opened file."<<endl;



    header myheader;

    char buff[1000];

    int successhd = 1;
    int successpl = 1;

    double b = 0;
    vector<double> vecttimestamp;
    vector<double> eventnum;

    while ( successhd != 0 && successpl != 0 )
    {

        successhd = read ( fd, ( void* ) &myheader,sizeof ( header ) );

        //cout<<"Read file."<<endl;

        if ( successhd == -1 )
        {
            cout<<"Error."<<endl;
            return;
        }
//         else if ( successhd<sizeof ( header ) )
//         {
//             cout<<"Not enough bytes read."<<endl;
//             //return;
//         }
        else if ( successhd == 0 )
        {
            cout<<"End of file reached."<<endl;
        }

        vecttimestamp.push_back ( ( double ) myheader.timestamp );

//   cout<<"============================================================"<<endl;
//   cout<<"Event number: "<<b<<endl;
//   //cout<<"File type: "<<myheader.type<<endl;
//   //cout<<"Payload Size: "<<myheader.payloadsize<<endl;
//   cout<<"Timestamp: "<<myheader.timestamp<<endl;

        successpl = read ( fd, ( void* ) buff,myheader.payloadsize );

        if ( successpl ==-1 )
        {
            cout<<"Error."<<endl;
            return;
        }
//         else if ( successpl<myheader.payloadsize )
//         {
//             cout<<"Not enough bytes read."<<endl;
//             //return;
//         }
        else if ( successpl == 0 )
        {
            cout<<"End of file reached."<<endl;
        }

        eventnum.push_back ( b );
        b++;

        if ( ( int ) b%20000 == 0 )
        {
            cout<<"Event number: "<<b<<"\r"<<flush;
        }

    }

    TCanvas* c = new TCanvas ( "c","Payload Size vs. Event Number",200,10,700,500 );
    TGraph* graph = new TGraph ( vecttimestamp.size(),&eventnum[0],&vecttimestamp[0] );
    graph->Draw ( "ALP" );
    c->Update();

    return;
}


struct analogheader
{
    char type[4];
    int num_words;
    unsigned int data[8192];
};

void ReadAnalogData ( string filename )
{
    int fd = open ( filename.c_str(),O_RDONLY );

    if ( fd == -1 )
    {
        cout<<"Could not open file."<<endl;
        return;
    }

    cout<<"Opened file."<<endl;

    analogheader header;

    int readsuccess = 1;

    int dir = 0;
    int head = 0;
    int pac = 0;
    int scal = 0;
    int dead = 0;
    int data = 0;
    int other = 0;
    map<string,int> otherstuff;

    unsigned int valuemask = 0xFFFF0000;
    unsigned int channelmask = 0x0000FFFF;
    int value;
    int channel;
    unsigned int unusedbit = 0xFFFFFF80;

    vector<int>* values = new vector<int>;
    vector<int>* channels = new vector<int>;

    TFile* f = new TFile ( "LearningROOTTrees.root","recreate" );
    TTree* tree= new TTree ( "analog_data","goddess_tree" );
    tree->Branch ( "val",&values );
    tree->Branch ( "channel",&channels );

    while ( readsuccess != 0 )
    {
        readsuccess = read ( fd, ( void* ) &header,sizeof ( header ) );

        //cout<< header.type<<endl;

        if ( ( string ) header.type ==  "DIR " )
        {
            cout<<"found it"<<endl;
            dir+=1;
        }
        else if ( ( string ) header.type == "HEAD" )
        {
            ++head;
        }
        else if ( ( string ) header.type == "PAC " )
        {
            ++pac;
        }
        else if ( ( string ) header.type == "SCAL" )
        {
            ++scal;
        }
        else if ( ( string ) header.type == "DEAD" )
        {
            ++dead;
        }
        else if ( ( string ) header.type == "DATA" )
        {
            ++data;

//             if ( data == 1 )
//             {
//                 for ( int r =0; r<8192; r++ )
//                 {
            //printf("Decimal %u : Hexidecimal %x \n",header.data[r], header.data[r]);



            for ( int bit = 0; bit < 8192; bit++ )
            {
                channel = ( header.data[bit] & channelmask );
                channel &= ~unusedbit;
                value = ( header.data[bit] & valuemask ) >>16;



                values->push_back ( value );
                channels->push_back ( channel );


//                     cout<<"------------------------------------------------------------------"<<endl;
//                     printf ( "Channel in decimal %u : hexidecimal %x \n",channel, channel );
//                     printf ( "Value in decimal %u : hexidecimal %x \n", value, value );
//                     if ( value == 65535 )
//                     {
//                         cout<<"=======================End+of+Event==============================="<<endl;
//                     }


            }
            //}
            //}

        }
        else
        {
            otherstuff[ ( string ) header.type];
            ++other;
        }
    }

    cout<<dir<<" directory(ies)."<<endl;
    cout<<head<<" header(s)."<<endl;
    cout<<data<<" data sets."<<endl;
    cout<<pac<<" PAC(s)."<<endl;
    cout<<scal<<" Scaler(s)."<<endl;
    cout<<dead<<" Deadtime(s)."<<endl;
    cout<<other<<" Other types."<<endl;
    cout<<"Other types are:"<<endl;
    for ( auto itr:otherstuff )
//     {
//         cout<<"\""<<itr.first<<"\""<<endl;
//     }
//
//     tree->Fill();
//
//
//     tree->Write();
//     f->Close();



        return;
}


//macro to add histogram files
//NOTE: This macro is kept for back compatibility only.
//Use instead the executable $ROOTSYS/bin/hadd
//
//This macro will add histograms from a list of root files and write them
//to a target root file. The target file is newly created and must not be
//identical to one of the source files.
//
//Author: Sven A. Schmidt, sven.schmidt@cern.ch
//Date:   13.2.2001

//This code is based on the hadd.C example by Rene Brun and Dirk Geppert,
//which had a problem with directories more than one level deep.
//(see macro hadd_old.C for this previous implementation).
//
//The macro from Sven has been enhanced by
//   Anne-Sylvie Nicollerat <Anne-Sylvie.Nicollerat@cern.ch>
// to automatically add Trees (via a chain of trees).
//
//To use this macro, modify the file names in function hadd.
//
//NB: This macro is provided as a tutorial.
//    Use $ROOTSYS/bin/hadd to merge many histogram files

TList *FileList;
TFile *Target;

void MergeRootfile ( TDirectory *target, TList *sourcelist );

//The below main is probably not necessary with your mods.

// int main( int argc, char **argv ) {
//
//   if (argc < 4) {
//      printf("******Error in invoking hadd\n");
//      printf("===>  hadd targetfile source1 source2 ...\n");
//      printf("      This program will add histograms from a list of root files and write them\n");
//      printf("      to a target root file. The target file is newly created and must not be\n");
//      printf("      identical to one of the source files.\n");
//      printf("      supply at least two source files for this to make sense... ;-)\n");
//      return 1;
//   }
//    FileList = new TList();
//
//   cout << "Target file: " << argv[1] << endl;
//   Target = TFile::Open( argv[1], "RECREATE" );
//
//   for ( int i = 2; i < argc; i++ ) {
//     cout << "Source file " << i-1 << ": " << argv[i] << endl;
//     FileList->Add( TFile::Open( argv[i] ) );
//   }
//
//   MergeRootfile( Target, FileList );
//
//   return 0;
// }


void hadd ( vector<string> files, string newfile )
{
    // Prepare the files to me merged

//The below makes copies of one file as an example, so it is not what we want.
//     if ( gSystem->AccessPathName ( newfile.c_str() )
// {
//     for ( int file = 0; file<files.size(); file++ )
//         {
//             gSystem->CopyFile ( files[file].c_str(), newfile.c_str() );
//         }
//     }

    // in an interactive ROOT session, edit the file names
    // Target and FileList, then
    // root > .L hadd.C
    // root > hadd()

    Target = TFile::Open ( newfile.c_str(), "RECREATE" );

    FileList = new TList();

    for ( int file=0; file<files.size(); file++ )
    {
        FileList->Add ( TFile::Open ( files[file].c_str() ) );
    }

    MergeRootfile ( Target, FileList );

}

void MergeRootfile ( TDirectory *target, TList *sourcelist )
{

    //  cout << "Target path: " << target->GetPath() << endl;
    TString path ( ( char* ) strstr ( target->GetPath(), ":" ) );
    path.Remove ( 0, 2 );

    TFile *first_source = ( TFile* ) sourcelist->First();
    first_source->cd ( path );
    TDirectory *current_sourcedir = gDirectory;

    // loop over all keys in this directory
    TChain *globChain = 0;
    TIter nextkey ( current_sourcedir->GetListOfKeys() );
    TKey *key;
    while ( ( key = ( TKey* ) nextkey() ) )
    {

        // read object from first source file
        first_source->cd ( path );
        TObject *obj = key->ReadObj();

        if ( obj->IsA()->InheritsFrom ( "TH1" ) )
        {
            // descendant of TH1 -> merge it

            //      cout << "Merging histogram " << obj->GetName() << endl;
            TH1 *h1 = ( TH1* ) obj;

            // loop over all source files and add the content of the
            // correspondant histogram to the one pointed to by "h1"
            TFile *nextsource = ( TFile* ) sourcelist->After ( first_source );
            while ( nextsource )
            {

                // make sure we are at the correct directory level by cd'ing to path
                nextsource->cd ( path );
                TH1 *h2 = ( TH1* ) gDirectory->Get ( h1->GetName() );
                if ( h2 )
                {
                    h1->Add ( h2 );
                    delete h2; // don't know if this is necessary, i.e. if
                    // h2 is created by the call to gDirectory above.
                }

                nextsource = ( TFile* ) sourcelist->After ( nextsource );
            }
        }
        else if ( obj->IsA()->InheritsFrom ( "TTree" ) )
        {

            // loop over all source files create a chain of Trees "globChain"
            TString obj_name;
            if ( path.Length() )
            {
                obj_name = path + "/" + obj->GetName();
            }
            else
            {
                obj_name = obj->GetName();
            }

            globChain = new TChain ( obj_name );
            globChain->Add ( first_source->GetName() );
            TFile *nextsource = ( TFile* ) sourcelist->After ( first_source );
            //      const char* file_name = nextsource->GetName();
            // cout << "file name  " << file_name << endl;
            while ( nextsource )
            {

                globChain->Add ( nextsource->GetName() );
                nextsource = ( TFile* ) sourcelist->After ( nextsource );
            }

        }
        else if ( obj->IsA()->InheritsFrom ( "TDirectory" ) )
        {
            // it's a subdirectory

            cout << "Found subdirectory " << obj->GetName() << endl;

            // create a new subdir of same name and title in the target file
            target->cd();
            TDirectory *newdir = target->mkdir ( obj->GetName(), obj->GetTitle() );

            // newdir is now the starting point of another round of merging
            // newdir still knows its depth within the target file via
            // GetPath(), so we can still figure out where we are in the recursion
            MergeRootfile ( newdir, sourcelist );

        }
        //attempt to define a GODDESS object. Will come back to this.
//     else if ( obj->IsA()->InheritsFrom( "GoddessGeomInfos" ) ) {
//
//       GoddessGeomInfos *geom = (GoddessGeomInfos*)obj;
//
//       // loop over all source files and add the content of the
//       // correspondant histogram to the one pointed to by "h1"
//       TFile *nextsource = (TFile*)sourcelist->After( first_source );
//       while ( nextsource ) {
//
//         // make sure we are at the correct directory level by cd'ing to path
//         nextsource->cd( path );
//         GoddessGeomInfos *geom2 = (GoddessGeomInfos*)gDirectory->Get( geom2->GetName() );
//
//         nextsource = (TFile*)sourcelist->After( nextsource );
//       }
//     }
        else
        {

            // object is of no type that we know or can handle
            cout << "Unknown object type, name: "
                 << obj->GetName() << " title: " << obj->GetTitle() << endl;
        }

        // now write the merged histogram (which is "in" obj) to the target file
        // note that this will just store obj in the current directory level,
        // which is not persistent until the complete directory itself is stored
        // by "target->Write()" below
        if ( obj )
        {
            target->cd();

            //!!if the object is a tree, it is stored in globChain...
            if ( obj->IsA()->InheritsFrom ( "TTree" ) )
                globChain->Merge ( target->GetFile(),0,"keep" );
            else
                obj->Write ( key->GetName() );
        }

    } // while ( ( TKey *key = (TKey*)nextkey() ) )

    // save modifications to target file
    target->Write();

}


// void MergeRootfile ( TDirectory *target, TList *sourcelist )
// {
//
//     //  cout << "Target path: " << target->GetPath() << endl;
//     TString path ( ( char* ) strstr ( target->GetPath(), ":" ) );
//     path.Remove ( 0, 2 );
//
//     TFile *first_source = ( TFile* ) sourcelist->First();
//     first_source->cd ( path );
//     TDirectory *current_sourcedir = gDirectory;
//     //gain time, do not add the objects in the list in memory
//     Bool_t status = TH1::AddDirectoryStatus();
//     TH1::AddDirectory ( kFALSE );
//
//     // loop over all keys in this directory
//     TChain *globChain = 0;
//     TIter nextkey ( current_sourcedir->GetListOfKeys() );
//     TKey *key, *oldkey=0;
//     while ( ( key = ( TKey* ) nextkey() ) )
//     {
//
//         //keep only the highest cycle number for each key
//         if ( oldkey && !strcmp ( oldkey->GetName(),key->GetName() ) ) continue;
//
//         // read object from first source file
//         first_source->cd ( path );
//         TObject *obj = key->ReadObj();
//
//         if ( obj->IsA()->InheritsFrom ( TH1::Class() ) )
//         {
//             // descendant of TH1 -> merge it
//
//             //      cout << "Merging histogram " << obj->GetName() << endl;
//             TH1 *h1 = ( TH1* ) obj;
//
//             // loop over all source files and add the content of the
//             // correspondant histogram to the one pointed to by "h1"
//             TFile *nextsource = ( TFile* ) sourcelist->After ( first_source );
//             while ( nextsource )
//             {
//
//                 // make sure we are at the correct directory level by cd'ing to path
//                 nextsource->cd ( path );
//                 TKey *key2 = ( TKey* ) gDirectory->GetListOfKeys()->FindObject ( h1->GetName() );
//                 if ( key2 )
//                 {
//                     TH1 *h2 = ( TH1* ) key2->ReadObj();
//                     h1->Add ( h2 );
//                     delete h2;
//                 }
//
//                 nextsource = ( TFile* ) sourcelist->After ( nextsource );
//             }
//         }
//         else if ( obj->IsA()->InheritsFrom ( TTree::Class() ) )
//         {
//
//             // loop over all source files create a chain of Trees "globChain"
//             const char* obj_name= obj->GetName();
//
//             globChain = new TChain ( obj_name );
//             globChain->Add ( first_source->GetName() );
//             TFile *nextsource = ( TFile* ) sourcelist->After ( first_source );
//             //      const char* file_name = nextsource->GetName();
//             // cout << "file name  " << file_name << endl;
//             while ( nextsource )
//             {
//
//                 globChain->Add ( nextsource->GetName() );
//                 nextsource = ( TFile* ) sourcelist->After ( nextsource );
//             }
//
//         }
//         else if ( obj->IsA()->InheritsFrom ( TDirectory::Class() ) )
//         {
//             // it's a subdirectory
//
//             cout << "Found subdirectory " << obj->GetName() << endl;
//
//             // create a new subdir of same name and title in the target file
//             target->cd();
//             TDirectory *newdir = target->mkdir ( obj->GetName(), obj->GetTitle() );
//
//             // newdir is now the starting point of another round of merging
//             // newdir still knows its depth within the target file via
//             // GetPath(), so we can still figure out where we are in the recursion
//             MergeRootfile ( newdir, sourcelist );
//
//         }
//         else
//         {
//
//             // object is of no type that we know or can handle
//             cout << "Unknown object type, name: "
//             << obj->GetName() << " title: " << obj->GetTitle() << endl;
//         }
//
//         // now write the merged histogram (which is "in" obj) to the target file
//         // note that this will just store obj in the current directory level,
//         // which is not persistent until the complete directory itself is stored
//         // by "target->Write()" below
//         if ( obj )
//         {
//             target->cd();
//
//             //!!if the object is a tree, it is stored in globChain...
//             if ( obj->IsA()->InheritsFrom ( TTree::Class() ) )
//                 globChain->Merge ( target->GetFile(),0,"keep" );
//             else
//                 obj->Write ( key->GetName() );
//         }
//
//     } // while ( ( TKey *key = (TKey*)nextkey() ) )
//
//     // save modifications to target file
//     target->SaveSelf ( kTRUE );
//     TH1::AddDirectory ( status );
// }
//
//
//

