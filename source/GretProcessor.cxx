#include "GretProcessor.h"

GretProcessor::GretProcessor ( int nGsGe_, int* ng_, PARS* pars_ )
{
    nGsGe = nGsGe_;

    pars = pars_;

    M = 350.0;
    
    ng = ng_;

    angles_map[make_pair<int,int> ( 1,0 )] = make_pair<float,float> ( 35.3,19.58 );
    angles_map[make_pair<int,int> ( 1,1 )] = make_pair<float,float> ( 47.6,42.3 );
    angles_map[make_pair<int,int> ( 1,2 )] = make_pair<float,float> ( 30.6,54.72 );
    angles_map[make_pair<int,int> ( 1,3 )] = make_pair<float,float> ( 16.65,19.58 );

    angles_map[make_pair<int,int> ( 2,0 ) ]=make_pair<float,float> ( 35.3,91.58 );
    angles_map[make_pair<int,int> ( 2,1 )] =make_pair<float,float> ( 47.6,11.43 );
    angles_map[make_pair<int,int> ( 2,2 )]=make_pair<float,float> ( 30.6,126.72 );
    angles_map[make_pair<int,int> ( 2,3 )]=make_pair<float,float> ( 16.65,91.58 );

    angles_map[make_pair<int,int> ( 3,0 )]=make_pair<float,float> ( 35.3,163.58 );
    angles_map[make_pair<int,int> ( 3,1 )]=make_pair<float,float> ( 47.6,186.3 );
    angles_map[make_pair<int,int> ( 3,2 )]=make_pair<float,float> ( 30.6,198.72 );
    angles_map[make_pair<int,int> ( 3,3 )]=make_pair<float,float> ( 16.65,163.58 );

    angles_map[make_pair<int,int> ( 4,0 )]=make_pair<float,float> ( 35.3,235.58 );
    angles_map[make_pair<int,int> ( 4,1 )]=make_pair<float,float> ( 47.6,258.3 );
    angles_map[make_pair<int,int> ( 4,2 )]=make_pair<float,float> ( 30.6,270.72 );
    angles_map[make_pair<int,int> ( 4,3 )]=make_pair<float,float> ( 16.65,235.58 );

    angles_map[make_pair<int,int> ( 5,0 )]=make_pair<float,float> ( 35.3,307.58 );
    angles_map[make_pair<int,int> ( 5,1 )]=make_pair<float,float> ( 47.6,330.3 );
    angles_map[make_pair<int,int> ( 5,2 )]=make_pair<float,float> ( 30.6,342.2 ) ;
    angles_map[make_pair<int,int> ( 5,3 )]=make_pair<float,float> ( 16.65,307.58 );

    angles_map[make_pair<int,int> ( 6,0 )]=make_pair<float,float> ( 67.71,74.63 );
    angles_map[make_pair<int,int> ( 6,1 )]=make_pair<float,float> ( 55.02,91.23 );
    angles_map[make_pair<int,int> ( 6,2 )]=make_pair<float,float> ( 48.92,68.77 );
    angles_map[make_pair<int,int> ( 6,3 )]=make_pair<float,float> ( 64.18,54.55 );

    angles_map[make_pair<int,int> ( 7,0 )]=make_pair<float,float> ( 67.71,146.63 );
    angles_map[make_pair<int,int> ( 7,1 )]=make_pair<float,float> ( 55.02,163.23 ) ;
    angles_map[make_pair<int,int> ( 7,2 )]=make_pair<float,float> ( 48.92,140.77 );
    angles_map[make_pair<int,int> ( 7,3 )]=make_pair<float,float> ( 64.18,126.55 );

    angles_map[make_pair<int,int> ( 8,0 )]=make_pair<float,float> ( 67.71,218.63 );
    angles_map[make_pair<int,int> ( 8,1 )]=make_pair<float,float> ( 55.02,235.23 );
    angles_map[make_pair<int,int> ( 8,2 )]=make_pair<float,float> ( 48.92,212.77 );
    angles_map[make_pair<int,int> ( 8,3 )]=make_pair<float,float> ( 64.18,198.55 );

    angles_map[make_pair<int,int> ( 9,0 )]=make_pair<float,float> ( 67.71,290.63 );
    angles_map[make_pair<int,int> ( 9,1 )]=make_pair<float,float> ( 55.02,307.23 ) ;
    angles_map[make_pair<int,int> ( 9,2 )]=make_pair<float,float> ( 48.92,284.77 );
    angles_map[make_pair<int,int> ( 9,3 )]=make_pair<float,float> ( 64.18,270.55 );

    angles_map[make_pair<int,int> ( 10,0 )]=make_pair<float,float> ( 67.71,2.63 );
    angles_map[make_pair<int,int> ( 10,1 )]=make_pair<float,float> ( 55.02,19.23 );
    angles_map[make_pair<int,int> ( 10,2 )]=make_pair<float,float> ( 48.92,356.77 );
    angles_map[make_pair<int,int> ( 10,3 )]=make_pair<float,float> ( 64.18,342.55 );

    angles_map[make_pair<int,int> ( 11,0 )]=make_pair<float,float> ( 87.15,8.71 );
    angles_map[make_pair<int,int> ( 11,1 )]=make_pair<float,float> ( 105.79,13.65 );
    angles_map[make_pair<int,int> ( 11,2 )]=make_pair<float,float> ( 92.85,27.29 ) ;
    angles_map[make_pair<int,int> ( 11,3 )]=make_pair<float,float> ( 74.21,22.35 );

    angles_map[make_pair<int,int> ( 12,0 )]=make_pair<float,float> ( 97.01,47.25 );
    angles_map[make_pair<int,int> ( 12,1 )]=make_pair<float,float> ( 100.78,66.39 );
    angles_map[make_pair<int,int> ( 12,2 )]=make_pair<float,float> ( 82.99,60.75 );
    angles_map[make_pair<int,int> ( 12,3 )]=make_pair<float,float> ( 79.22,41.61 );

    angles_map[make_pair<int,int> ( 13,0 )]=make_pair<float,float> ( 87.15,80.71 ) ;
    angles_map[make_pair<int,int> ( 13,1 )]=make_pair<float,float> ( 105.79,85.65 );
    angles_map[make_pair<int,int> ( 13,2 )]=make_pair<float,float> ( 92.85,99.29 );
    angles_map[make_pair<int,int> ( 13,3 )]=make_pair<float,float> ( 74.21,94.35 );

    angles_map[make_pair<int,int> ( 14,0 )]=make_pair<float,float> ( 97.01,119.25 );
    angles_map[make_pair<int,int> ( 14,1 )]=make_pair<float,float> ( 100.78,138.39 );
    angles_map[make_pair<int,int> ( 14,2 )]=make_pair<float,float> ( 82.99,132.75 );
    angles_map[make_pair<int,int> ( 14,3 )]=make_pair<float,float> ( 79.22,113.61 );

    angles_map[make_pair<int,int> ( 15,0 )]=make_pair<float,float> ( 87.15,152.71 );
    angles_map[make_pair<int,int> ( 15,1 )]=make_pair<float,float> ( 105.79,157.65 ) ;
    angles_map[make_pair<int,int> ( 15,2 )]=make_pair<float,float> ( 92.85,171.29 ) ;
    angles_map[make_pair<int,int> ( 15,3 )]=make_pair<float,float> ( 74.21,166.35 ) ;

    angles_map[make_pair<int,int> ( 16,0 )]=make_pair<float,float> ( 97.01,191.25 );
    angles_map[make_pair<int,int> ( 16,1 )]=make_pair<float,float> ( 100.78,210.39 );
    angles_map[make_pair<int,int> ( 16,2 )]=make_pair<float,float> ( 82.99,204.75 );
    angles_map[make_pair<int,int> ( 16,3 )]=make_pair<float,float> ( 79.22,185.61 );

    angles_map[make_pair<int,int> ( 17,0 )]=make_pair<float,float> ( 87.15,224.71 ) ;
    angles_map[make_pair<int,int> ( 17,1 )]=make_pair<float,float> ( 105.79,229.65 ) ;
    angles_map[make_pair<int,int> ( 17,2 )]=make_pair<float,float> ( 92.85,243.29 );
    angles_map[make_pair<int,int> ( 17,3 )]=make_pair<float,float> ( 74.21,238.35 ) ;

    angles_map[make_pair<int,int> ( 18,0 )]=make_pair<float,float> ( 97.01,263.25 ) ;
    angles_map[make_pair<int,int> ( 18,1 )]=make_pair<float,float> ( 100.78,282.39 ) ;
    angles_map[make_pair<int,int> ( 18,2 )]=make_pair<float,float> ( 82.99,276.75 ) ;
    angles_map[make_pair<int,int> ( 18,3 )]=make_pair<float,float> ( 79.22,257.61 ) ;

    angles_map[make_pair<int,int> ( 19,0 )]=make_pair<float,float> ( 87.15,296.71 ) ;
    angles_map[make_pair<int,int> ( 19,1 )]=make_pair<float,float> ( 105.79,301.65 ) ;
    angles_map[make_pair<int,int> ( 19,2 )]=make_pair<float,float> ( 92.85,315.29 ) ;
    angles_map[make_pair<int,int> ( 19,3 )]=make_pair<float,float> ( 74.21,310.35 ) ;

    angles_map[make_pair<int,int> ( 20,0 )]=make_pair<float,float> ( 97.01,335.25 ) ;
    angles_map[make_pair<int,int> ( 20,1 )]=make_pair<float,float> ( 100.78,354.39 ) ;
    angles_map[make_pair<int,int> ( 20,2 )]=make_pair<float,float> ( 82.99,348.75 ) ;
    angles_map[make_pair<int,int> ( 20,3 )]=make_pair<float,float> ( 79.22,310.35 ) ;

    angles_map[make_pair<int,int> ( 21,0 )]=make_pair<float,float> ( 112.29,33.37 ) ;
    angles_map[make_pair<int,int> ( 21,1 )]=make_pair<float,float> ( 124.98,16.77 ) ;
    angles_map[make_pair<int,int> ( 21,2 )]=make_pair<float,float> ( 131.08,39.23 ) ;
    angles_map[make_pair<int,int> ( 21,3 )]=make_pair<float,float> ( 115.82,53.45 ) ;

    angles_map[make_pair<int,int> ( 22,0 )]=make_pair<float,float> ( 112.29,105.37 ) ;
    angles_map[make_pair<int,int> ( 22,1 )]=make_pair<float,float> ( 124.98,88.77 ) ;
    angles_map[make_pair<int,int> ( 22,2 )]=make_pair<float,float> ( 131.08,111.23 );
    angles_map[make_pair<int,int> ( 22,3 )]=make_pair<float,float> ( 115.82,125.45 ) ;

    angles_map[make_pair<int,int> ( 23,0 )]=make_pair<float,float> ( 112.29,177.37 ) ;
    angles_map[make_pair<int,int> ( 23,1 )]=make_pair<float,float> ( 124.98,160.77 ) ;
    angles_map[make_pair<int,int> ( 23,2 )]=make_pair<float,float> ( 131.08,183.23 ) ;
    angles_map[make_pair<int,int> ( 23,3 )]=make_pair<float,float> ( 115.82,197.45 ) ;

    angles_map[make_pair<int,int> ( 24,0 )]=make_pair<float,float> ( 112.29,249.37 );
    angles_map[make_pair<int,int> ( 24,1 )]=make_pair<float,float> ( 124.98,232.77 ) ;
    angles_map[make_pair<int,int> ( 24,2 )]=make_pair<float,float> ( 131.08,255.23 ) ;
    angles_map[make_pair<int,int> ( 24,3 )]=make_pair<float,float> ( 115.82,269.45 ) ;

    angles_map[make_pair<int,int> ( 25,0 )]=make_pair<float,float> ( 112.29,321.37 ) ;
    angles_map[make_pair<int,int> ( 25,1 )]=make_pair<float,float> ( 124.98,304.77 ) ;
    angles_map[make_pair<int,int> ( 25,2 )]=make_pair<float,float> ( 131.08,327.23 ) ;
    angles_map[make_pair<int,int> ( 25,3 )]=make_pair<float,float> ( 115.82,341.45 ) ;

    angles_map[make_pair<int,int> ( 26,0 )]=make_pair<float,float> ( 144.7,88.42 ) ;
    angles_map[make_pair<int,int> ( 26,1 )]=make_pair<float,float> ( 132.4,65.7 ) ;
    angles_map[make_pair<int,int> ( 26,2 )]=make_pair<float,float> ( 149.4,53.28 ) ;
    angles_map[make_pair<int,int> ( 26,3 )]=make_pair<float,float> ( 163.35,88.42 ) ;

    angles_map[make_pair<int,int> ( 27,0 )]=make_pair<float,float> ( 144.7,160.42 ) ;
    angles_map[make_pair<int,int> ( 27,1 )]=make_pair<float,float> ( 132.4,137.7 ) ;
    angles_map[make_pair<int,int> ( 27,2 )]=make_pair<float,float> ( 149.4,125.8 ) ;
    angles_map[make_pair<int,int> ( 27,3 )]=make_pair<float,float> ( 163.35,160.42 ) ;

    angles_map[make_pair<int,int> ( 28,0 )]=make_pair<float,float> ( 144.7,232.42 ) ;
    angles_map[make_pair<int,int> ( 28,1 )]=make_pair<float,float> ( 132.4,209.7 ) ;
    angles_map[make_pair<int,int> ( 28,2 )]=make_pair<float,float> ( 149.4,197.8 ) ;
    angles_map[make_pair<int,int> ( 28,3 )]=make_pair<float,float> ( 163.35,232.42 );

    angles_map[make_pair<int,int> ( 29,0 )]=make_pair<float,float> ( 144.7,304.42 ) ;
    angles_map[make_pair<int,int> ( 29,1 )]=make_pair<float,float> ( 132.4,281.7 ) ;
    angles_map[make_pair<int,int> ( 29,2 )]=make_pair<float,float> ( 149.4,269.28 ) ;
    angles_map[make_pair<int,int> ( 29,3 )]=make_pair<float,float> ( 163.35,304.42 ) ;

    angles_map[make_pair<int,int> ( 30,0 )]=make_pair<float,float> ( 144.7,16.42 ) ;
    angles_map[make_pair<int,int> ( 30,1 )]=make_pair<float,float> ( 132.4,253.7 ) ;
    angles_map[make_pair<int,int> ( 30,2 )]=make_pair<float,float> ( 149.4,341.28 ) ;
    angles_map[make_pair<int,int> ( 30,3 )]=make_pair<float,float> ( 163.35,16.42 ) ;

}

vector< float > GretProcessor::Coor_Trans_Gretina ( float radius, float theta, float phi, float sphere_split )
{


    float x_1 = radius*cos ( theta ) *sin ( phi );
    float y_1 = radius*sin ( theta ) *sin ( phi );
    float z_1 = radius* cos ( phi );

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
}

float GretProcessor::Angle_Between_Vect ( float x1, float y1, float z1, float x2, float y2, float z2 )
{
    float dotproduct = x1*x2+y1*y2+z1*z2;
    float norm1 = pow ( pow ( x1,2 ) +pow ( y1,2 ) +pow ( z1,2 ),1/2 );
    float norm2 = pow ( pow ( x2,2 ) +pow ( y2,2 ) +pow ( z2,2 ),1/2 );

    float angle = acos ( dotproduct/ ( norm1*norm2 ) );

    return angle;
}

vector< float > GretProcessor::Tot_Gam_Pos ( vector<float> new_face, float x, float y, float z )
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

int GretProcessor::Gret_Tracking (GEB_EVENT* theGEBEvent, GRETEVENT* thegretEvt, float sphere_split )
{
  int i = 0;
  GRETHEADER* thegretHdr;

  while (theGEBEvent->ptinp.size())
  {
    thegretHdr = (GRETHEADER*) theGEBEvent->ptinp[i];
    
    

        //determine position of crystal
        hole_num = thegretHdr->crystal_id/4; //quad number.
        crystal_num = thegretHdr->crystal_id%4; //crystal num 0-3 in quad
        hole_det.first = hole_num;
        hole_det.second = crystal_num;

        itr = angles_map.find ( hole_det );
        if ( itr != angles_map.end() )
        {
            theta_phi.first = itr->second.first;
            theta_phi.second = itr->second.second;
        }

        crystal_face = Coor_Trans_Gretina ( R,theta_phi.first,theta_phi.second, sphere_split );



        // if not the first timestamp in the file or a gamma ray below 100kev, compute delta_t
        if ( timestamp_0 != -1 )
        {
            delta_t = ( thegretHdr->timestamp-timestamp_0 );
        }


        //if it is the first timestamp in the file or a gamma ray after a hit below 100kev
        if ( timestamp_0 == -1 )
        {
            if ( thegretHdr->tot_e >515 ) //two methods for determining position based on tot_e. If above 515kev, use binary positions
            {
//
                for ( int hit1=0; hit1<thegretHdr->num; hit1++ )
                {

                    if ( thegretHdr->tot_e<100 ) //get rid of xray noise
                    {
                        continue;

                    }

                    tot_gamma_pos = Tot_Gam_Pos ( crystal_face, thegretHdr->inputs[hit1].x, thegretHdr->inputs[hit1].y, thegretHdr->inputs[hit1].z ); //determine total pos of gamma ray with crystal face


                    one_hit.x = tot_gamma_pos[0];
                    one_hit.y = tot_gamma_pos[1];
                    one_hit.z = tot_gamma_pos[2];
                    one_hit.r = tot_gamma_pos[3];
                    one_hit.theta = tot_gamma_pos[4];
                    one_hit.phi = tot_gamma_pos[5];
                    tot_gamma_pos.clear();
                    one_hit.e = thegretHdr->inputs[hit1].e;
                    one_hit.seg = thegretHdr->inputs[hit1].seg;
                    one_hit.seg_ener = thegretHdr->inputs[hit1].seg_ener;
                    one_hit.raw_e = thegretHdr->tot_e;
                    one_hit.quad = hole_num;
                    one_hit.crystal = crystal_num;
                    event.push_back ( one_hit );

                    timestamp_0 = thegretHdr->timestamp;

                }
            }
            if ( thegretHdr->tot_e<515 ) //if crystal tot_e is below 515kev, must use a weighted energy position.
            {
                for ( int whit1 = 0; whit1<thegretHdr->num; whit1++ ) //determine weighted energies by running through all the sub gamma rays
                {

                    weighted_x += thegretHdr->inputs[whit1].x*thegretHdr->inputs[whit1].e;
                    weighted_y += thegretHdr->inputs[whit1].y*thegretHdr->inputs[whit1].e;
                    weighted_z += thegretHdr->inputs[whit1].z*thegretHdr->inputs[whit1].e;
                    weighted_e += thegretHdr->inputs[whit1].e;
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
                one_hit.raw_e = thegretHdr->tot_e;
                one_hit.quad = hole_num;
                one_hit.crystal = crystal_num;
                event.push_back ( one_hit );

                timestamp_0 = thegretHdr->timestamp;
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



            for ( unsigned int interaction = 0; interaction<event.size(); interaction++ ) //find the highest energy of the sub gamma rays and use the highest energy as the first interaction
            {

                if ( event[interaction].e > highest_energy )
                {
                    highest_energy = event[interaction].e;

                    hit = interaction;
                }

            }

            tot_energy = event[hit].e; //set first interaction position as the pos of tot_e


            for ( unsigned int p = 0; p<event.size(); p++ )
            {
                if ( p == hit ) //ignore the highest interaction
                {
                    continue;
                }

                if ( event[p].quad == event[hit].quad && event[p].crystal == event[hit].crystal )
                {
                    continue;
                }
                angle_between_hit = Angle_Between_Vect ( event[hit].x, event[hit].y, event[hit].z, event[p].x,event[p].y,event[p].z ); //determine how far away your sub gamma rays are. If within 15 degrees addback
                //angle_between_hit = Scattering_Angle(highest_energy,event[i].e);

                if ( angle_between_hit> 15 || angle_between_hit<-15 ) //outside of 15 degrees save for second gamma ray
                {
                    one_hit.x = event[p].x;
                    one_hit.y = event[p].y;
                    one_hit.z = event[p].z ;
                    one_hit.r = event[p].r;
                    one_hit.theta = event[p].theta;
                    one_hit.phi = event[p].phi;
                    one_hit.e = event[p].e;
                    one_hit.seg = event[p].seg;
                    one_hit.seg_ener = event[p].seg_ener;
                    one_hit.raw_e = event[p].raw_e;
                    one_hit.quad = event[p].quad;
                    one_hit.crystal = event[p].crystal;
                    second_event.push_back ( one_hit );

                    continue;
                }

                tot_energy += event[p].e; //addback

            }
            Gamma.e = tot_energy;


            thegretEvt->x = event[hit].x;
            thegretEvt->y = event[hit].y;
            thegretEvt->z = event[hit].z;
            thegretEvt->timestamp = third_event[hit].timestamp;
            thegretEvt->raw_e = third_event[hit].raw_e;
            thegretEvt->quad = third_event[hit].quad;
            thegretEvt->crystal = third_event[hit].crystal;
            thegretEvt->theta = third_event[hit].theta;
            thegretEvt->phi = third_event[hit].phi;
//             Gammas->push_back ( Gamma );
            //energy_temp.push_back ( Gamma.e );


            //reset variables to zero so as to use in upcoming event
            tot_energy = 0;
            highest_energy = 0;



            //repeat above but for sub gamma rays saved for second event
            if ( second_event.size() != 0 )
            {
                for ( unsigned int set = 0; set<second_event.size(); set++ )
                {
                    if ( second_event[set].e > highest_energy )
                    {
                        highest_energy = second_event[set].e;

                        hit = set;
                    }
                }

                tot_energy = event[hit].e;

                for ( unsigned int ff = 0; ff<second_event.size(); ff++ )
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

                    tot_energy += second_event[ff].e;
                }
                thegretEvt->e = tot_energy;

                thegretEvt->x = second_event[hit].x;
                thegretEvt->y = second_event[hit].y;
                thegretEvt->z = second_event[hit].z;
                thegretEvt->timestamp = third_event[hit].timestamp;
                thegretEvt->raw_e = third_event[hit].raw_e;
                thegretEvt->quad = third_event[hit].quad;
                thegretEvt->crystal = third_event[hit].crystal;
                thegretEvt->theta = third_event[hit].theta;
               thegretEvt->phi = third_event[hit].phi;
//                 Gammas->push_back ( Gamma );
                //energy_temp.push_back ( Gamma.e );

                tot_energy = 0;
                highest_energy = 0;
            }


            //repeat again for the chance of a third event. However, do not keep for fourth event. Fourth event is very unlikely.
            if ( third_event.size() != 0 )
            {
                for ( unsigned int settwo= 0; settwo<event.size(); settwo++ )
                {
                    if ( event[settwo].e > highest_energy )
                    {
                        highest_energy = event[settwo].e;

                        hit = settwo;
                    }
                }

                tot_energy = event[hit].e;

                for ( unsigned int fff = 0; fff<third_event.size(); fff++ )
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
                thegretEvt->e = tot_energy;

                thegretEvt->x = third_event[hit].x;
                thegretEvt->y = third_event[hit].y;
                thegretEvt->z = third_event[hit].z;
                thegretEvt->timestamp = third_event[hit].timestamp;
                thegretEvt->raw_e = third_event[hit].raw_e;
                thegretEvt->quad = third_event[hit].quad;
                thegretEvt->crystal = third_event[hit].crystal;
                thegretEvt->theta = third_event[hit].theta;
                thegretEvt->phi = third_event[hit].phi;
//                 Gammas->push_back ( Gamma );
                //energy_temp.push_back ( Gamma.e );

                tot_energy = 0;
            }

            //clear vectors to use for next gamma ray sequence
            event.clear();
            second_event.clear();
            third_event.clear();

            //save the information from the current header for the next gamma ray if over 100kev.
            for ( int hit3=0; hit3<thegretHdr->num; hit3++ )
            {
                if ( thegretHdr->tot_e<100 )
                {
                    continue;
                    timestamp_0 = -1;
                }

                if ( thegretHdr->tot_e > 515 )
                {

                    tot_gamma_pos = Tot_Gam_Pos ( crystal_face, thegretHdr->inputs[hit3].x, thegretHdr->inputs[hit3].y, thegretHdr->inputs[hit3].z );
                    one_hit.x = tot_gamma_pos[0];
                    one_hit.y = tot_gamma_pos[1];
                    one_hit.z = tot_gamma_pos[2];
                    one_hit.r = tot_gamma_pos[3];
                    one_hit.theta = tot_gamma_pos[4];
                    one_hit.phi = tot_gamma_pos[5];
                    tot_gamma_pos.clear();
                    one_hit.e = thegretHdr->tot_e;
                    one_hit.seg = thegretHdr->inputs[hit3].seg;
                    one_hit.seg_ener = thegretHdr->inputs[hit3].seg_ener;
                    one_hit.raw_e = thegretHdr->tot_e;
                    one_hit.quad = hole_num;
                    one_hit.crystal = crystal_num;
                    event.push_back ( one_hit );

                    timestamp_0 = thegretHdr->timestamp;
                }
            }
            if ( thegretHdr->tot_e<515 )
            {
                for ( int thit1 = 0; thit1<thegretHdr->num; thit1++ )
                {

                    weighted_x += thegretHdr->inputs[thit1].x*thegretHdr->inputs[thit1].e;
                    weighted_y += thegretHdr->inputs[thit1].y*thegretHdr->inputs[thit1].e;
                    weighted_z += thegretHdr->inputs[thit1].z*thegretHdr->inputs[thit1].e;
                    weighted_e += thegretHdr->inputs[thit1].e;
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
                one_hit.seg = 0;
                one_hit.seg_ener = 0;
                one_hit.raw_e = thegretHdr->tot_e;
                one_hit.quad = hole_num;
                one_hit.crystal = crystal_num;
                event.push_back ( one_hit );

                timestamp_0 = thegretHdr->timestamp;
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

            for ( int hit2=0; hit2<thegretHdr->num; hit2++ )
            {
                if ( thegretHdr->inputs[hit2].e == 0 )
                {
                    continue;
                }

                if ( thegretHdr->tot_e > 515 )
                {

                    tot_gamma_pos = Tot_Gam_Pos ( crystal_face, thegretHdr->inputs[hit2].x, thegretHdr->inputs[hit2].y, thegretHdr->inputs[hit2].z );
                    one_hit.x = tot_gamma_pos[0];
                    one_hit.y = tot_gamma_pos[1];
                    one_hit.z = tot_gamma_pos[2];
                    one_hit.r = tot_gamma_pos[3];
                    one_hit.theta = tot_gamma_pos[4];
                    one_hit.phi = tot_gamma_pos[5];
                    tot_gamma_pos.clear();
                    one_hit.e = thegretHdr->tot_e;
                    one_hit.seg = thegretHdr->inputs[hit2].seg;
                    one_hit.seg_ener = thegretHdr->inputs[hit2].seg_ener;
                    one_hit.raw_e = thegretHdr->tot_e;
                    one_hit.quad = hole_num;
                    one_hit.crystal = crystal_num;
                    event.push_back ( one_hit );

                    timestamp_0 = thegretHdr->timestamp;
                }
            }
            if ( thegretHdr->tot_e<515 )
            {
                for ( int thit2 = 0; thit2<thegretHdr->num; thit2++ )
                {

                    weighted_x += thegretHdr->inputs[thit2].x*thegretHdr->inputs[thit2].e;
                    weighted_y += thegretHdr->inputs[thit2].y*thegretHdr->inputs[thit2].e;
                    weighted_z += thegretHdr->inputs[thit2].z*thegretHdr->inputs[thit2].e;
                    weighted_e += thegretHdr->inputs[thit2].e;
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
                one_hit.raw_e = thegretHdr->tot_e;
                one_hit.quad = hole_num;
                one_hit.crystal = crystal_num;
                event.push_back ( one_hit );

                timestamp_0 = thegretHdr->timestamp;
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

// thegretEvt = &Gammas->at(0);

    return -1;
}

int GretProcessor::BinGR ( GEB_EVENT* theGEBEvent, GRETEVENT* thegretEvt, float sphere_split )
{
    /* declarations */

    char str[128];
    //DGSEVENT dgsEvt[MAXCOINEV];

    float Energy;

    /* prototypes */

    int GebTypeStr ( int type, char str[] );

    /* Print debug */

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "entered bin_dgs:\n" );
    }

//     std::cerr << "****************Entered bin_dgs****************\n";
//     std::cerr << "theGEBEvent size = " << theGEBEvent->ptgd.size() << "\n";

    /* loop through the coincidence event and fish out DGS data */
    /* (gamma rays) count in ng */

    *ng = 0;

   

    for ( unsigned int i = 0; i < theGEBEvent->ptgd.size(); i++ )
    {
//       std::cerr << "Found theGEBEvent type " << theGEBEvent->ptgd[i]->type << " (DGS type is " << GEB_TYPE_DGS << ")\n";
//       std::cerr << "--> length: " << theGEBEvent->ptgd[i]->length << "\n";
//       std::cerr << "--> timestamp: " << theGEBEvent->ptgd[i]->timestamp << "\n";

        if ( theGEBEvent->ptgd[i]->type == GEB_TYPE_DECOMP )
        {
            if ( pars->CurEvNo <= pars->NumToPrint )
            {
                GebTypeStr ( theGEBEvent->ptgd[i]->type, str );
                printf ( "bin_mode1, %2i> %2i, %s, TS=%lli\n", i, theGEBEvent->ptgd[i]->type, str,
                         theGEBEvent->ptgd[i]->timestamp );
            }

             Gret_Tracking ( (GEB_EVENT*) theGEBEvent, &thegretEvt[*ng], sphere_split );

//             std::cerr << "DGS event: theGEBEvent TS = " << theGEBEvent->ptgd[i]->timestamp << " / DGSEents TS = " << thedgsEvt[*ng].event_timestamp << "\n";

            ( *ng ) ++;
        }
    }

    // Initialize EvTimeStam0

    if ( EvTimeStam0 == 0 )
    {
        EvTimeStam0 = thegretEvt[0].timestamp;
    }

// GS ENERGY CALIBRATION

    double d1;

    /* Compton Suppression Loop */

    for ( int i = 0; i < *ng; i++ )
    {

        if ( pars->beta != 0 )
        {
//                 std::cerr << "Beta used for Doppler Correction: " << pars->beta << std::endl;

            d1 = thegretEvt[i].theta / 57.29577951;
            Energy = thegretEvt[i].e * ( 1 - pars->beta * cos ( d1 ) ) / sqrt ( 1 - pars->beta * pars->beta );
        }

        Energy = thegretEvt[i].e;
    }

if ( pars->CurEvNo <= pars->NumToPrint )
{
    printf ( "exit bin_gret\n" );
}

return ( 0 );



}
