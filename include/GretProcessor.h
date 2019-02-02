
#ifndef GRETPROCESSOR_H
#define GRETPROCESSOR_H
#include "GRProcessor.h"
#include <map>

#define max_inputs 16

class GretProcessor : public GRProcessor {
public:
  GretProcessor();
  ~GretProcessor();
private:
  
    pair<int,int>* hole_det;
    pair<float,float>* theta_phi;
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

vector<float> Coor_Trans_Gretina ( float R, float theta, float phi, float sphere_split );
    
vector<float> Tot_Gam_Pos ( vector<float> new_face, float x, float y, float z );

float Angle_Between_Vect ( float x1, float y1, float z1, float x2, float y2, float z2 );

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
    
    TFile* rootfile;
    TTree* tree;
    
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
  
};

#endif