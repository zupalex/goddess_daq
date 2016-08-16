#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TH1.h"
#include "TH2.h"
#include "TObjArray.h"
#include "TObject.h"
#include "TKey.h"
#include "TSystem.h"
#include "TCutG.h"
#include "TTree.h"
#include "gdecomp.h"

#include "veto_pos.h"
#include "GEBSort.h"
#include "GTMerge.h"

#include "ProcessManagers.h"

#define MAXH 50
#define MAXK 40
#define MAXNGAM 20

#define MINDOPFAC 0.5
#define MAXDOPFAC 1.5

/* pointers to ROOT spectra */


//TH2F *SMAP_firsthits;
//TH1D *fmsp;
//TH2F *ecos_raw;
//TH2F *ecos_dopcor;
//TH1D *dopfac;
//TH1D *polangle;
//TH1D *gmult;
//TH1D *sumTrackE;
//TH1D *TrackE_1gate;
//TH1D *TrackE_2gates;
//TH1D *TrackE_3gates;
//TH2F *fomXe;
//TH2F *ndetXfom;
//TH2F *HK;
//TH1D *rate_mode1;
//TH2F *gg;
//TH2F *ndet_e;
//TH2F *rad_e;
//TH1D *radius_first;
//TH2F *evsr_first;
//TH2F *ngamXsumTrackE;

typedef struct PAYLOAD
{
    char p[MAXDATASIZE];
} PAYLOAD;

typedef struct TRACK_STRUCT
{
    int n;
    GEBDATA* gd;
    PAYLOAD* payload;
} TRACK_STRUCT;



/* parameters */

extern PARS Pars;

/*-----------------------------------------------------*/

int
print_tracked_gamma_rays ( FILE* fp, TRACKED_GAMMA_HIT* grh )
{

    /* declarations */

    int j;
    float r1;

    fprintf ( fp, "number of gamma rays: %i\n", grh->ngam );
    for ( j = 0; j < grh->ngam; j++ )
    {
        fprintf ( fp, "esum=%6.3f, ", grh->gr[j].esum );
        fprintf ( fp, "ndet (#interactions)=%2i, ", grh->gr[j].ndet );
        fprintf ( fp, "fom=%6.3f, ", grh->gr[j].fom );
        fprintf ( fp, "tracked=%i\n", grh->gr[j].tracked );
        fprintf ( fp, "1'th hit: (%6.3f, %6.3f, %6.3f), e= %6.1f keV\n", grh->gr[j].x0, grh->gr[j].y0, grh->gr[j].z0,
                  grh->gr[j].e0 );
        fprintf ( fp, "2'nd hit: (%6.3f, %6.3f, %6.3f), e= %6.1f keV\n", grh->gr[j].x1, grh->gr[j].y1, grh->gr[j].z1,
                  grh->gr[j].e1 );
        r1 = grh->gr[j].x0 * grh->gr[j].x0 + grh->gr[j].y0 * grh->gr[j].y0 + grh->gr[j].z0 * grh->gr[j].z0;
        r1 = sqrtf ( r1 );
        fprintf ( fp, "1'th interaction radius: %9.1f mm\n", r1 );
    };

    /* done */

    return ( 0 );

}

/*-----------------------------------------------------*/

int
sup_mode1()
{
    /* declarations */

    //char str1[STRLEN], str2[STRLEN];//unused
    //float pi; //unused
    //int i; //unused
    unsigned int seed;

    if ( !Pars.noHists )
    {
        TH1D* mkTH1D ( char*, char*, int, double, double );
        TH2F* mkTH2F ( char*, char*, int, double, double, int, double, double );
    }

    int get_a_seed ( unsigned int* );

    //gDirectory->mkdir("bin_mode1")->cd();

    /* define spectra */

    //sprintf (str1, "ecos_raw");
    //sprintf (str2, "ecos_raw");
    //ecos_raw = mkTH2F (str1, str2, 2048, 0, 2047, 1000, -1, 1);
    //sprintf (str1, "uncorrected Energy");
    //ecos_raw->SetXTitle (str1);
    //sprintf (str1, "cos()");
    //ecos_raw->SetYTitle (str1);

    //sprintf (str1, "ecos_dopcor");
    //sprintf (str2, "ecos_dopcor");
    //ecos_dopcor = mkTH2F (str1, str2, 2048, 0, 2047, 1000, -1, 1);
    //sprintf (str1, "doppler corrected Energy");
    //ecos_dopcor->SetXTitle (str1);
    //sprintf (str1, "cos()");
    //ecos_dopcor->SetYTitle (str1);

    //sprintf (str1, "SMAP_firsthits");
    //sprintf (str2, "SMAP_firsthits");
    //SMAP_firsthits = mkTH2F (str1, str2, 256, -180, 180, 256, 0, 180);
    //sprintf (str1, "horizontal");
    //SMAP_firsthits->SetXTitle (str1);
    //sprintf (str1, "vertical");
    //SMAP_firsthits->SetYTitle (str1);

    //sprintf (str1, "fm");
    //sprintf (str2, "figure of merit");
    //fmsp = mkTH1D (str1, str2, 1024, 0, 2.1);
    //fmsp->SetXTitle (str1);

    //sprintf (str1, "polangle");
    //sprintf (str2, "polangle");
    //polangle = mkTH1D (str1, str2, 1024, 0, 180);
    //polangle->SetXTitle (str1);

    //sprintf (str1, "dopfac");
    //sprintf (str2, "dopfac");
    //dopfac = mkTH1D (str1, str2, 2048, MINDOPFAC, MAXDOPFAC);
    //dopfac->SetXTitle (str1);

    //sprintf (str1, "gmult");
    //sprintf (str2, "tracked gamma ray mutiplicity");
    //gmult = mkTH1D (str1, str2, 21, 0, 20);
    //gmult->SetXTitle (str1);

    //sprintf (str1, "sumTrackE");
    //sumTrackE = mkTH1D (str1, str1, LONGLEN, 1, LONGLEN);
    //sumTrackE->SetXTitle (str1);

    //sprintf (str1, "TrackE_1gate");
    //TrackE_1gate = mkTH1D (str1, str1, LONGLEN, 1, LONGLEN);
    //TrackE_1gate->SetXTitle (str1);

    //sprintf (str1, "TrackE_2gates");
    //TrackE_2gates = mkTH1D (str1, str1, LONGLEN, 1, LONGLEN);
    //TrackE_2gates->SetXTitle (str1);

    //sprintf (str1, "TrackE_3gates");
    //TrackE_3gates = mkTH1D (str1, str1, LONGLEN, 1, LONGLEN);
    //TrackE_3gates->SetXTitle (str1);

    //sprintf (str1, "fomXe");
    //sprintf (str2, "fomXe");
    //fomXe = mkTH2F (str1, str2, LONGLEN, 1, LONGLEN, 200, 0, 2.0);
    //fomXe->SetXTitle ("e");
    //fomXe->SetYTitle ("fom");

    //sprintf (str1, "ndetXfom");
    //ndetXfom = mkTH2F (str1, str1, 8, 1, 8, 200, 0, 2.0);
    //ndetXfom->SetXTitle ("ndet (# interaction points)");
    //ndetXfom->SetYTitle ("fom");

    //sprintf (str1, "ngamXsumTrackE");
    //sprintf (str2, "# gammarays vs gamma ray energy");
    //ngamXsumTrackE = mkTH2F (str1, str2, MAXNGAM, 1, MAXNGAM, LONGLEN, 1, LONGLEN);
    //sprintf (str1, "ngam");
    //ngamXsumTrackE->SetXTitle (str1);
    //sprintf (str1, "sumTrackE");
    //ngamXsumTrackE->SetYTitle (str1);

    //sprintf (str1, "HK");
    //sprintf (str2, "HK");
    //HK = mkTH2F (str1, str2, 1024, 0, MAXK, 1024, 0, MAXH);
    //sprintf (str1, "K, # gamma rays");
    //HK->SetXTitle (str1);
    //sprintf (str1, "H, summed energy");
    //HK->SetYTitle (str1);

    //sprintf (str1, "rate_mode1");
    //sprintf (str2, "rate_mode1");
    //rate_mode1 = mkTH1D (str1, str2, RATELEN, 0, RATELEN);
    //rate_mode1->SetXTitle (str1);

    //sprintf (str1, "gg");
    //sprintf (str2, "tracked gg matrix");
    //gg = mkTH2F (str1, str2, Pars.GGMAX, 1, Pars.GGMAX, Pars.GGMAX, 1, Pars.GGMAX);
    //sprintf (str1, "g1");
    //gg->SetXTitle (str1);
    //sprintf (str1, "g2");
    //gg->SetYTitle (str1);


    //sprintf (str1, "ndet_e");
    //sprintf (str2, "interaction points vs gamma energy");
    //ndet_e = mkTH2F (str1, str2, 9, 1, 10, Pars.GGMAX, 1, (double) Pars.GGMAX);
    //sprintf (str1, "ndet");
    //ndet_e->SetXTitle (str1);
    //sprintf (str1, "e");
    //ndet_e->SetYTitle (str1);

    //sprintf (str1, "rad_e");
    //sprintf (str2, "rad_e");
    //rad_e = mkTH2F (str1, str2, 2048, 0, 300, Pars.GGMAX, 1, Pars.GGMAX);
    //sprintf (str1, "radius");
    //rad_e->SetXTitle (str1);
    //sprintf (str1, "E (keV)");
    //rad_e->SetYTitle (str1);

    //sprintf (str1, "radius_first");
    //sprintf (str2, "radius (first points)");
    //radius_first = mkTH1D (str1, str2, 4096, RMIN, RMAX);
    //radius_first->SetXTitle (str1);

    //sprintf (str1, "evsr_first");
    //sprintf (str2, "evsr_first");
    //evsr_first = mkTH2F (str1, str2, MEDIUMLEN, RMIN, RMAX, MEDIUMLEN, 1, MEDIUMLEN);
    //sprintf (str1, "radius (cm)");
    //evsr_first->SetXTitle (str1);
    //sprintf (str1, "energy");
    //evsr_first->SetYTitle (str1);


    //gDirectory->cd("/");
    /* list what we have */

//  printf (" we have define the following spectra:\n");

//  Pars.wlist = gDirectory->GetList ();
//  Pars.wlist->Print ();

    get_a_seed ( &seed );
    srand ( seed );

    return ( 0 );
}

/* ----------------------------------------------------------------- */

int
bin_mode1 ( GEB_EVENT* GEB_event )
{

    /* declarations */
    //int m, crystalno, moduleno,detno, nMode2 = 0; //unused
    //int xx, yy, zz; //unused
    int k, l, i, j, ii, jj, nMode1 = 0;
    float sX, sY, polAng, aziAng, rr, dp, rn;
    double d1, d2, d3;
    float RAD2DEG = 0.01745329;
    char str[128];
    //float detDopFac; //unused
    TRACKED_GAMMA_HIT* grh;
    //PAYLOAD *ptinp; //unused
    //GEBDATA *ptgd; //unused
    static long long int t0;
    static int firsttime = 1;
    float polang[MAX_GAMMA_RAYS];
    float doppler_factor[MAX_GAMMA_RAYS];
    //int ngamInEvent = 0;//unused

    /* define the gates */

    //int numgate, num_sp1;//unused
    int ngate, mult_g, e1, gate_spe[4000], newgat, imult;
    float e_good[40];
    int ener_l[40], ener_h[40];
    //num_sp1 = 1;
    //numgate = Pars.numgggates;


    /* prototypes */

    int GebTypeStr ( int type, char str[] );
    float findAzimuthFromCartesian ( float, float );
    float findPolarFromCartesian ( float, float, float, float* );



    if ( Pars.CurEvNo <= Pars.NumToPrint )
    {
        printf ( "--------------------\nentered bin_mode1:\n" );
    }

#if(0)
    /* find the number of tracked headers in event */

    nMode1 = 0;
    for ( i = 0; i < GEB_event->mult; i++ )
    {
        if ( GEB_event->ptgd[i]->type == GEB_TYPE_TRACK )
        {
            nMode1++;
        }
        if ( GEB_event->ptgd[i]->type == GEB_TYPE_DECOMP )
        {
            nMode2++;
        }
    };
    printf ( "nMode2=%i, nMode1=%i\n", nMode2, nMode1 );
    if ( nMode1 != 1 )
    {
        printf ( "nMode1=%i\n", nMode1 );
    }
    assert ( nMode1 <= 1 );
#endif

    /* process mode 1 data */

    nMode1 = 0;
    for ( i = 0; i < GEB_event->mult; i++ )
    {

        if ( GEB_event->ptgd[i]->type == GEB_TYPE_TRACK )
        {

            if ( Pars.CurEvNo <= Pars.NumToPrint )
            {
                GebTypeStr ( GEB_event->ptgd[i]->type, str );
                printf ( "bin_mode1, %2i> %2i, %s, TS=%lli;\n", i, GEB_event->ptgd[i]->type, str,
                         GEB_event->ptgd[i]->timestamp );
            }

            /* mode 1 rate spectrum, x=minute, y=Hz */

            if ( firsttime )
            {
                firsttime = 0;
                t0 = GEB_event->ptgd[i]->timestamp;
            };
            d1 = ( double ) ( GEB_event->ptgd[i]->timestamp - t0 );
            d1 /= 100000000;
            d1 /= 60;
            if ( !Pars.noHists && d1 > 0 && d1 < ( double ) RATELEN )
                //rate_mode1->Fill (d1, 1 / 60.0);
            {
                nMode1++;
            }

            grh = ( TRACKED_GAMMA_HIT* ) GEB_event->ptinp[i];

            //if ( !Pars.noHists ) gmult->Fill (grh->ngam, 1);

            /* check for multiplicity requirements */

            if ( grh->ngam < Pars.multlo || grh->ngam > Pars.multhi )
            {
                return ( 0 );
            }

            if ( Pars.CurEvNo <= Pars.NumToPrint )
            {
                printf ( "ngam=%i\n", grh->ngam );
                for ( j = 0; j < grh->ngam; j++ )
                {
                    printf ( "%2i: esum=%8.0f, fom=%4.2f, tracked %i\n", j, grh->gr[j].esum, grh->gr[j].fom,
                             grh->gr[j].tracked );
                };
            };

            /* modify FOM for single inteaction */

            for ( j = 0; j < grh->ngam; j++ )
                if ( grh->gr[j].tracked )
                    if ( grh->gr[j].ndet == 1 )
                        if ( grh->gr[j].esum > Pars.maxsnglintrE )
                        {
                            grh->gr[j].fom = Pars.maxsnglintrEFOM;
                        }

            /* find doppler correction factors */

            if ( Pars.beta >= 0.000 )
            {
                for ( j = 0; j < grh->ngam; j++ )
                    if ( grh->gr[j].tracked )
                    {

                        /* note: x,y,z and rr are in mm */
                        /* so are Pars.target_x|y|z */

                        /* find normalized vector from target position */

                        rr = ( grh->gr[j].x0 - Pars.target_x ) * ( grh->gr[j].x0 - Pars.target_x )
                             + ( grh->gr[j].y0 - Pars.target_y ) * ( grh->gr[j].y0 - Pars.target_y )
                             + ( grh->gr[j].z0 - Pars.target_z ) * ( grh->gr[j].z0 - Pars.target_z );
                        rr = sqrtf ( rr );

                        /* bin rr but in cm */

                        if ( j == 0 )
                            if ( rr / 10 > RMIN && rr / 10 < RMAX )
                            {
                                //if ( !Pars.noHists ) radius_first->Fill ((double) rr / 10, 1);
                                //need to make sure the following if statment is supposed to control the evsr Fill
                                if ( grh->gr[j].esum > 0 && grh->gr[j].esum < MEDIUMLEN ) {};
                                //if ( !Pars.noHists ) evsr_first->Fill ((double) rr / 10, grh->gr[j].esum);
                            };

                        /* dot with beam direction */

                        dp = ( ( grh->gr[j].x0 - Pars.target_x ) * Pars.beamdir[0] +
                               ( grh->gr[j].y0 - Pars.target_y ) * Pars.beamdir[1] +
                               ( grh->gr[j].z0 - Pars.target_z ) * Pars.beamdir[2] ) / rr;

                        /* find polar angle and bin it */

                        if ( dp < -1.0 )
                        {
                            dp = -1.0;
                        }
                        if ( dp > 1.0 )
                        {
                            dp = 1.0;
                        }
                        polang[j] = acosf ( dp );
                        //if ( !Pars.noHists ) polangle->Fill (polang[j] / M_PI * 180, 1);

                        if ( grh->gr[j].ndet >= Pars.ndetlimlo && grh->gr[j].ndet <= Pars.ndetlimhi )
                        {
                            d1 = ( double ) ( grh->gr[j].esum );
                            if ( d1 < 0 )
                            {
                                d1 = 0;
                            }
                            if ( d1 > Pars.GGMAX )
                            {
                                d1 = Pars.GGMAX - 1;
                            }

                            //if ( !Pars.noHists ) rad_e->Fill (rr, d1, 1.0);
                        };

                        /* find a bin doppler factor */

                        rr = 1.0 - Pars.beta * Pars.beta;
                        doppler_factor[j] = sqrt ( rr ) / ( 1.0 - Pars.beta * cos ( polang[j] ) );
                        if ( doppler_factor[j] > MINDOPFAC && doppler_factor[j] < MAXDOPFAC )
                            //if ( !Pars.noHists ) dopfac->Fill (doppler_factor[j], 1);

                            if ( Pars.CurEvNo <= Pars.NumToPrint )
                            {
                                printf ( "doppler_factor[%i]=%f\n", j, doppler_factor[j] );
                            }
                    };

                /* apply doppler corrections */

                for ( j = 0; j < grh->ngam; j++ )
                    if ( grh->gr[j].tracked )
                    {

                        //if ( !Pars.noHists ) ecos_raw->Fill (grh->gr[j].esum, cos (polang[j]));

                        if ( Pars.CurEvNo <= Pars.NumToPrint )
                            printf ( "dopler correction: esum %f e0 %f e1 %f -> ", grh->gr[j].esum, grh->gr[j].e0,
                                     grh->gr[j].e1 );

                        grh->gr[j].esum /= doppler_factor[j];
                        //if ( !Pars.noHists ) ecos_dopcor->Fill (grh->gr[j].esum, cos (polang[j]));
                        grh->gr[j].e0 /= doppler_factor[j];
                        grh->gr[j].e1 /= doppler_factor[j];
                        if ( Pars.CurEvNo <= Pars.NumToPrint )
                        {
                            printf ( "%f %f %f\n", grh->gr[j].esum, grh->gr[j].e0, grh->gr[j].e1 );
                        }

                    };
            };

            if ( Pars.CurEvNo <= Pars.NumToPrint )
            {
                print_tracked_gamma_rays ( stdout, grh );
            }

            /* energy spectra vs ndet */

            for ( j = 0; j < grh->ngam; j++ )
                if ( grh->gr[j].tracked )
                    if ( grh->gr[j].ndet >= Pars.ndetlimlo && grh->gr[j].ndet <= Pars.ndetlimhi )
                    {
                        //if (grh->gr[j].ndet < 10)
                        //if (grh->gr[j].esum < Pars.GGMAX)
                        //if ( !Pars.noHists ) ndet_e->Fill ((double) grh->gr[j].ndet, (double) grh->gr[j].esum, 1);
                    };


            for ( j = 0; j < grh->ngam; j++ )
                if ( grh->gr[j].tracked )
                {
                    polAng = findPolarFromCartesian ( grh->gr[j].x0, grh->gr[j].y0, grh->gr[j].z0, &rr );
                    aziAng = findAzimuthFromCartesian ( grh->gr[j].x0, grh->gr[j].y0 );


                    /* SMAP coordinates */

                    sX = aziAng * sinf ( polAng ) / RAD2DEG;
                    sY = polAng / RAD2DEG;  /* + 1.5; */

                    /* update */

                    if ( Pars.CurEvNo <= Pars.NumToPrint )
                    {
                        printf ( "aziAng=%6.2f\n", aziAng / M_PI * 180 );
                        printf ( "polAng=%6.2f\n", polAng / M_PI * 180 );
                        printf ( " sX,sY=%6.2f,%6.2f\n", sX, sY );
                        fflush ( stdout );
                    };

                    //  if (sX > -180 && sX < 180)
                    //    if (sY > 0 && sY < 180)
                    //      //if ( !Pars.noHists ) SMAP_firsthits->Fill (sX, sY, 1);
                };



            /* esum vs fom matrix */

            for ( j = 0; j < grh->ngam; j++ )
                if ( grh->gr[j].tracked )
                {
                    d1 = ( double ) ( grh->gr[j].esum );
                    //if ( grh->gr[j].fom < 2.0 && grh->gr[j].fom >= 0 )
                    //if ( d1 > 0 && d1 < LONGLEN )
                    //if ( grh->gr[j].ndet >= Pars.ndetlimlo && grh->gr[j].ndet <= Pars.ndetlimhi )
                    //{

                    //if ( !Pars.noHists ) fomXe->Fill (d1, grh->gr[j].fom, 1);
//                        printf("*** %f %f\n", (float)d1, (float)grh->gr[j].fom);

                    //if ( !Pars.noHists )
                    //{
                    //if ( grh->gr[j].fom >= Pars.fomlo[grh->gr[j].ndet] )
                    //if ( grh->gr[j].fom <= Pars.fomhi[grh->gr[j].ndet] )
                    //{
                    //sumTrackE->Fill (d1, 1);
                    //if (grh->ngam < MAXNGAM)
                    //ngamXsumTrackE->Fill (grh->ngam, d1, 1);
                    //};
                    //}
                    //};
                };

            /* fom spectrum */

            if ( !Pars.noHists )
            {
                /*for ( j = 0; j < grh->ngam; j++ )
                    if ( grh->gr[j].tracked )
                        if ( grh->gr[j].fom >= 0 && grh->gr[j].fom < 2.0 )
                        {
                            //fmsp->Fill (grh->gr[j].fom, 1);
                            //ndetXfom->Fill (grh->gr[j].ndet, grh->gr[j].fom);
                        }*/
            }

            /* HK matrix */

            d2 = 0;
            for ( j = 0; j < grh->ngam; j++ )
                if ( grh->gr[j].tracked )
                {
                    d2 += ( double ) ( grh->gr[j].esum );
                }
            d2 /= 1000;
            rn = ( float ) rand() / RAND_MAX;
            d1 = ( grh->ngam + ( rn - 0.5 ) );
            /*if ( !Pars.noHists )
            {
                if ( d1 > ( double ) 0 && d1 < ( double ) MAXK )
                    if ( d2 > ( double ) 0 && d2 < ( double ) MAXH )
                    {
            //                printf ("HK: ngam=%i, %f %f\n", grh->ngam, (float) d1, (float) d2);
            //                fflush (stdout);
                        //HK->Fill (d1, d2, 1);
                    };
            }*/


            /* je viens de rajouter cette routine pour gated spectra stored in a matrix  */

            ngate = 0;
            mult_g = 0;
            newgat = 0;
            d3 = 0;
            e1 = 0;
            if ( grh->ngam >= 3 && grh->ngam < 20 )
            {
                for ( k = 0; k < grh->ngam; k++ )
                    if ( grh->gr[k].tracked )
                        if ( grh->gr[k].ndet >= Pars.ndetlimlo && grh->gr[k].ndet <= Pars.ndetlimhi )
                            if ( grh->gr[k].fom >= Pars.fomlo[grh->gr[k].ndet] && grh->gr[k].fom <= Pars.fomhi[grh->gr[k].ndet] )
                                if ( grh->gr[k].ndet >= Pars.ndetlimlo && grh->gr[k].ndet <= Pars.ndetlimhi )
                                {

                                    d3 = ( double ) grh->gr[k].esum;
                                    if ( d3 >= 0 && d3 <= 4000 )
                                    {
                                        e_good[mult_g] = d3;
//                              printf ("kkkk number of eenrgies------%i, e_good%f \n", mult_g, e_good[mult_g]);
                                        mult_g++;

                                    }
                                };
            };


            imult = mult_g;
            for ( jj = 0; jj < 4000; jj++ )
            {
                gate_spe[jj] = 0;
            };

            for ( ii = 0; ii < Pars.numgggates; ii++ )
            {
                ener_l[ii] = Pars.gg_gate_pos[ii] - Pars.gg_gate_width[ii];
                ener_h[ii] = Pars.gg_gate_pos[ii] + Pars.gg_gate_width[ii];

                for ( jj = ener_l[ii]; jj < ener_h[ii]; jj++ )
                {

                    gate_spe[jj] = 1;
                };

            };


            newgat = ngate;

            ngate = 0;
            newgat = 0;

            for ( ii = 0; ii < imult; ii++ )
            {
                e1 = ( int ) e_good[ii];
                if ( gate_spe[e1] == 1 )
                {
                    ngate++;

                }
            };

            newgat = ngate;

            //if(ngate >=3) printf(" le nombre newgat %i, %i, %i, %i, %i, %i\n", ngate, newgat, newgat1, newgat2, newgat3, newgat4);

            /* build a 1 gated spectrum spike free */

            if ( imult >= 3 && newgat >= 1 )
            {
                for ( ii = 0; ii < imult; ii++ )
                {
                    e1 = ( int ) e_good[ii];
                    if ( newgat == 1 )
                    {
                        if ( !Pars.noHists && gate_spe[e1] == 0 )
                        {
                            //TrackE_1gate->Fill ((double) e1, 1);
                        };
                    };
                };
                for ( ii = 0; ii < imult; ii++ )
                {
                    e1 = ( int ) e_good[ii];
                    //if (newgat >= 2)
                    //if ( !Pars.noHists ) TrackE_1gate->Fill ((double) e1, 1);
                };
// double gates

                for ( ii = 0; ii < imult; ii++ )
                {
                    e1 = ( int ) e_good[ii];
                    if ( newgat == 2 )
                    {
                        if ( gate_spe[e1] == 0 )
                        {
                            //if ( !Pars.noHists ) TrackE_2gates->Fill ((double) e1, 1);
                        };
                    };
                };
                for ( ii = 0; ii < imult; ii++ )
                {
                    e1 = ( int ) e_good[ii];
                    //if (newgat >= 3)
                    //if ( !Pars.noHists ) TrackE_2gates->Fill ((double) e1, 1);

                };

                for ( ii = 0; ii < imult; ii++ )
                {
                    e1 = ( int ) e_good[ii];
                    if ( newgat == 3 )
                    {
                        if ( !Pars.noHists && gate_spe[e1] == 0 )
                        {
                            //TrackE_3gates->Fill ((double) e1, 1);
                        };
                    };
                };
                for ( ii = 0; ii < imult; ii++ )
                {
                    e1 = ( int ) e_good[ii];
                    if ( !Pars.noHists && newgat >= 4 )
                    {

                        //TrackE_3gates->Fill ((double) e1, 1);
                    };

                };

            };


            /* gamma gamma matrix */

            if ( grh->ngam >= 2 )
                for ( k = 0; k < grh->ngam; k++ )
                    if ( grh->gr[k].tracked )
                        if ( grh->gr[k].ndet >= Pars.ndetlimlo && grh->gr[k].ndet <= Pars.ndetlimhi )
                            if ( grh->gr[k].fom >= Pars.fomlo[grh->gr[k].ndet] && grh->gr[k].fom <= Pars.fomhi[grh->gr[k].ndet] )
                                if ( grh->gr[k].ndet >= Pars.ndetlimlo && grh->gr[k].ndet <= Pars.ndetlimhi )
                                    for ( l = k + 1; l < grh->ngam; l++ )
                                        if ( grh->gr[l].tracked )
                                            if ( grh->gr[l].ndet >= Pars.ndetlimlo && grh->gr[l].ndet <= Pars.ndetlimhi )
                                                if ( grh->gr[l].fom >= Pars.fomlo[grh->gr[l].ndet]
                                                        && grh->gr[l].fom <= Pars.fomhi[grh->gr[l].ndet] )
                                                    if ( grh->gr[l].ndet >= Pars.ndetlimlo && grh->gr[l].ndet <= Pars.ndetlimhi )
                                                    {

                                                        d1 = ( double ) grh->gr[k].esum;
                                                        if ( d1 < 0 )
                                                        {
                                                            d1 = 0;
                                                        }
                                                        if ( d1 > Pars.GGMAX )
                                                        {
                                                            d1 = Pars.GGMAX;
                                                        }

                                                        d2 = ( double ) grh->gr[l].esum;
                                                        if ( d2 < 0 )
                                                        {
                                                            d2 = 0;
                                                        }
                                                        if ( d2 > Pars.GGMAX )
                                                        {
                                                            d2 = Pars.GGMAX;
                                                        }

                                                        //if ( !Pars.noHists ) gg->Fill (d1, d2, 1.0);
                                                        //if ( !Pars.noHists ) gg->Fill (d2, d1, 1.0);

                                                    };


        };                      /* if (GEB_event->ptgd[i]->type == GEB_TYPE_TRACK) */

    };



    /* done */

    if ( Pars.CurEvNo <= Pars.NumToPrint )
    {
        printf ( "exit bin_mode1\n" );
    }

    return ( 0 );

}
