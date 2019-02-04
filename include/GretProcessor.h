
#ifndef GRETPROCESSOR_H
#define GRETPROCESSOR_H
#include "GRProcessor.h"
#include <map>

class GretProcessor : public GRProcessor {
  
private: 
 
public:
  GretProcessor():GRProcessor( GRProcessor::nGsGe, GRProcessor::tlkup, GRProcessor::tid, GRProcessor::ng, GRProcessor::pars){};
  ~GretProcessor();
  
   
    
    pair<int,int> hole_det;
    pair<float,float> theta_phi;

    map<pair<int,int>,pair<float,float>>::iterator itr;

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
    //vector<float> energy_temp;
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
    unsigned int hit; //the position of the highest energy sub gamma ray in the gamma ray addback vector
    float angle_between_hit; //angle between the determine first hit and the subsequent hits
    vector<Gretina_hit> second_event; //vector for if there is a second gamma ray in the gamma ray addback vector outside of 15 degrees
    vector<Gretina_hit> third_event; //see above, but if there is a third
    float weighted_x = 0; //weighted energy positon x
    float weighted_y = 0; //weighted energy position y
    float weighted_z = 0; //weighted energy position z
    float weighted_e = 0; //tot energy of the sub gamma ray

    //for reading the binary
    int readsuccess = 1;
    int buffsuccess = 1;
    
    int Gret_Tracking (GRProcessor::Gretina_header header, float sphere_split);
    
    int Bingr ( GEB_EVENT* theGEBEvent, Gretina_header* theGretEvt );
  
};



#endif