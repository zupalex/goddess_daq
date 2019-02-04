#include "GretProcessor.h"

vector<float> GretProcessor::Coor_Trans_Gretina ( float radius, float theta, float phi, float sphere_split )
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

vector<float> GretProcessor::Tot_Gam_Pos ( vector<float> new_face, float x, float y, float z )
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

float GretProcessor::Angle_Between_Vect ( float x1, float y1, float z1, float x2, float y2, float z2 )
{
    float dotproduct = x1*x2+y1*y2+z1*z2;
    float norm1 = pow ( pow ( x1,2 ) +pow ( y1,2 ) +pow ( z1,2 ),1/2 );
    float norm2 = pow ( pow ( x2,2 ) +pow ( y2,2 ) +pow ( z2,2 ),1/2 );

    float ang = acos ( dotproduct/ ( norm1*norm2 ) );

    return ang;
}

int GretProcessor::Gret_Tracking ( GRProcessor::Gretina_header header, float sphere_split )
{

    //record initial info for branches
    crystalID.push_back ( header.crystal_id );
    num_of_inter.push_back ( header.num );
    tote.push_back ( header.tot_e );
    timestamp.push_back ( header.timestamp );


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



        for ( unsigned int interaction = 0; interaction<event.size(); interaction++ ) //find the highest energy of the sub gamma rays and use the highest energy as the first interaction
        {

            if ( event[interaction].e > highest_energy )
            {
                highest_energy = event[interaction].e;

                hit = interaction;
            }

        }

        tot_energy = event[hit].tot_e; //set first interaction position as the pos of tot_e


        for ( unsigned int i = 0; i<event.size(); i++ )
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

            if ( pars->beta != 0 )
            {
                tot_energy = tot_energy * ( 1 - pars->beta * cos ( angles.first ) ) / sqrt ( 1 - pars->beta * pars->beta );
            }

        }
        Gamma.e = tot_energy;


        Gamma.x = event[hit].x;
        Gamma.y = event[hit].y;
        Gamma.z = event[hit].z;
        Gamma.seg = event[hit].seg;
        Gamma.seg_ener = event[hit].seg_ener;
        Gamma.crystal = crystal_num;
        Gamma.quad = crystal_num;
        Gamma.raw_e = header.tot_e;
	Gamma.timestamp = header.timestamp;
        Gammas->push_back ( Gamma );
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

            tot_energy = event[hit].tot_e;

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

                tot_energy += second_event[ff].tot_e;

                if ( pars->beta != 0 )
                {
                    tot_energy = tot_energy * ( 1 - pars->beta * cos ( angles.first ) ) / sqrt ( 1 - pars->beta * pars->beta );
                }
            }
            Gamma.e = tot_energy;

            Gamma.x = second_event[hit].x;
            Gamma.y = second_event[hit].y;
            Gamma.z = second_event[hit].z;
            Gamma.seg = second_event[hit].seg;
            Gamma.seg_ener = second_event[hit].seg_ener;
            Gamma.crystal = crystal_num;
            Gamma.quad = crystal_num;
            Gamma.raw_e = header.tot_e;
	    Gamma.timestamp = header.timestamp;
            Gammas->push_back ( Gamma );
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

            tot_energy = event[hit].tot_e;

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

                if ( pars->beta != 0 )
                {
                    tot_energy = tot_energy * ( 1 - pars->beta * cos ( angles.first ) ) / sqrt ( 1 - pars->beta * pars->beta );
                }

            }
            Gamma.e = tot_energy;

            Gamma.x = third_event[hit].x;
            Gamma.y = third_event[hit].y;
            Gamma.z = third_event[hit].z;
            Gamma.seg = third_event[hit].seg;
            Gamma.seg_ener = third_event[hit].seg_ener;
            Gamma.crystal = crystal_num;
            Gamma.quad = crystal_num;
            Gamma.raw_e = header.tot_e;
	    Gamma.timestamp = header.timestamp;
            Gammas->push_back ( Gamma );
            //energy_temp.push_back ( Gamma.e );

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

    return 0;
}

int GretProcessor::Bingr ( GEB_EVENT* theGEBEvent, GRProcessor::Gretina_header* theGretEvt )
{
    /* declarations */

    char str[128];
    //DGSEVENT dgsEvt[MAXCOINEV];

    int RelEvT = 0;

    /* prototypes */

    int GebTypeStr ( int type, char str[] );

    /* Print debug */

    if ( pars->CurEvNo <= pars->NumToPrint )
    {
        printf ( "entered bin_dgs:\n" );
    }

    /* loop through the coincidence event and fish out DGS data */
    /* (gamma rays) count in ng */

    *ng = 0;

    for ( unsigned int i = 0; i < theGEBEvent->ptgd.size(); i++ )
    {

        if ( theGEBEvent->ptgd[i]->type == GEB_TYPE_DECOMP )
        {
            if ( pars->CurEvNo <= pars->NumToPrint )
            {
                GebTypeStr ( theGEBEvent->ptgd[i]->type, str );
                printf ( "bin_mode1, %2i> %2i, %s, TS=%lli\n", i, theGEBEvent->ptgd[i]->type, str,
                         theGEBEvent->ptgd[i]->timestamp );
            }

            //put what you want to do with data here

            GretProcessor::Gret_Tracking ( theGretEvt[i], pars->sphere_split );

            ( *ng ) ++;
        }
    }

    // Initialize EvTimeStam0
    if ( pars->CurEvNo < 100 )
    {
        printf ( "\n\nCurEvNo: %i", pars->CurEvNo );
        for ( int i = 0; i < *ng; i++ )
        {
            printf ( "\n thedgsEvt[%i].event_timestamp: %llu", i, theGretEvt[i].timestamp );
        }
    }

    if ( EvTimeStam0 == 0 )
    {
        EvTimeStam0 = theGretEvt[0].timestamp;
    }
    RelEvT = ( int ) ( ( theGretEvt[0].timestamp - EvTimeStam0 ) / 100000000 ); // overflow?
    if ( !pars->noHists ) hEventCounter->Fill ( RelEvT );


    return ( 0 );



}
