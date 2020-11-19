#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <cassert>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <list>
#include <functional>
#include <algorithm>
//#include "se84_detclasses.h"
//#include "se84_analysis_functions.cxx"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TChain.h"

void monte_carlo_solid_angle()
{

	cout << "=========================================================================" << endl;
    cout << "             Monte Carlo Simulation of Detector Solid Angle              " << endl;
    cout << "=========================================================================" << endl;
    cout << " " << endl;
    cout << "|=======================================================================|" << endl;
    cout << "|                                                                       |" << endl;
    cout << "|  This script will calculate the solid angle of the detectors using the|" << endl;
    cout << "|  Monte Carlo method.                                                  |" << endl;
    cout << "|                                                              H. Sims  |" << endl;
    cout << "|=======================================================================|" << endl;

    
    //TH3D* alpha_pos = new TH3D("alpha_pos", "alpha_pos", 1000, -2, 2, 1000, -2, 2, 1000, -2, 2);
    //TH3D* alpha_pos_DET = new TH3D("alpha_pos_DET", "alpha_pos_DET", 1000, -2, 2, 1000, -2, 2, 1000, -2, 2);


    TCanvas* c1 = new TCanvas("c1", "c1", 400, 400);
    TCanvas* c2 = new TCanvas("c2", "c2", 400, 400);

    TH1D* theta_test = new TH1D("theta_test", "theta_test", 1000, -4, 4);
    TH1D* phi_test = new TH1D("phi_test", "phi_test", 1000, -1, 8);

    TH1D* alpha_spectrum = new TH1D("alpha_spectrum", "alpha_spectrum", 75, 0, 75 );

    TH2D* planar_pos = new TH2D("planar_pos", "planar_pos", 1000, -500, 500, 1000, -500, 500);
    TH2D* planar_pos_det = new TH2D("planar_pos_det", "planar_pos_det", 1000, -500, 500, 1000, -500, 500);

    int n_iterations = 1.55E+7;

    double theta_alpha, phi_alpha, rand1, rand2;
    double x0 = 0, y0 = 0, z0 = 0, x1, y1, z1;

    //Setting the geometry of the detector: What alpha angles would be captured by the detector?
    double z_offset = 65 + 113; //mm
    double radius = 106.8; //mm

    double t, x_int, y_int, z_int;
    
    srand(NULL);

    for(int i=0; i< n_iterations; i++ )
    {
    	rand1 = ((double) rand() / (RAND_MAX));
    	rand2 = ((double) rand() / (RAND_MAX));

    	theta_alpha = acos((2*rand1) - 1.0);
    	phi_alpha = 2*3.14159 * rand2;

    	theta_test->Fill(theta_alpha);
    	phi_test->Fill(phi_alpha);

    	x1 = sin(theta_alpha) * cos(phi_alpha);
    	y1 = sin(theta_alpha) * sin(phi_alpha);
    	z1 = cos(theta_alpha);

    	t = (radius - y0) / (y1 - y0);

    	x_int = x0 + (t*(x1 - x0));
    	y_int = y0 + (t*(y1 - y0));
    	z_int = z0 + (t*(z1 - z0));

    	if(x_int <= 20.0 && x_int >= -20.0 && z_int >= z_offset && z_int <= z_offset + 75.0 && y1 >= 0)
    	{
    		planar_pos_det->Fill(z_int, x_int);

    		alpha_spectrum->Fill(z_int - z_offset);
    	}
    	else
    	{
    		planar_pos->Fill(z_int, x_int);
    	}
    }

    planar_pos_det->SetFillColor(kRed);
    planar_pos_det->SetMarkerColor(kRed);


    c1->cd();
    alpha_spectrum->Draw();


    c2->cd();
    planar_pos->Draw();
    planar_pos_det->Draw("SAME");


	return;

}