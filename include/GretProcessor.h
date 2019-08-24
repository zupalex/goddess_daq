#ifndef GRETPROCESSOR_H
#define GRETPROCESSOR_H

#include "GEBSort.h"
#include "GTMerge.h"



class GretProcessor 
{

public:
    GretProcessor ( int nGsGe_, int* ng_, PARS* pars_ );

    int* ng;
    
    
    GRETHEADER rgret;
      //various variables
    int32_t timestamp_0 = -1; //inital time for delta_t
    int32_t delta_t = -1; //computed length of gamma ray addback
    int32_t event_build_time = 25; //predetermined max gamma ray addback time
    GRETHIT one_hit; //sub gamma ray struct
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
    //vector<GRETHIT>* second_event; //vector for if there is a second gamma ray in the gamma ray addback vector outside of 15 degrees
    //vector<GRETHIT>* third_event; //see above, but if there is a third
    float weighted_x = 0; //weighted energy positon x
    float weighted_y = 0; //weighted energy position y
    float weighted_z = 0; //weighted energy position z
    float weighted_e = 0; //tot energy of the sub gamma rays
    
    //create map to determine crystal coordinates
    pair<int,int> hole_det;
    pair<float,float> theta_phi;
    map<pair<int,int>,pair<float,float>> angles_map;
    map<pair<int,int>,pair<float,float>>::iterator itr;

    
    //vectors & associated structs to make branches
    vector<int> crystalID;
    vector<int> num_of_inter; //number of interaction points from decomp
    vector<float> tote;
    vector<float> e2;
    vector<int> timestamp;
    TH1D* hEventCounter;

    GRETEVENT Gamma;
    vector<GRETEVENT*> Gammas;
    vector<float> energy_temp;
    float tot_energy;
    
    vector<float> Coor_Trans_Gretina ( float R, float theta, float phi, vector<float> sphere_split );
    
    vector<float> Tot_Gam_Pos ( vector<float> new_face, float x, float y, float z );
    
    float Angle_Between_Vect ( float x1, float y1, float z1, float x2, float y2, float z2 );

    int Gret_Tracking (GEB_EVENT* theGEBEvent, GRETEVENT* thegretEvt, vector<float> sphere_split);
  
    int BinGR(GEB_EVENT* theGEBEvent, GRETEVENT* thegretEvt, vector<float> sphere_split );
    
    float DopplerCorrection ( float in_energy, float phi );
  
  
  

private:
    int nGsGe;

    PARS* pars;
    
        /* Gain Calibrtation */
    float M; // changed from 350.0
    
    unsigned long long int  EvTimeStam0 = 0;

};

#endif