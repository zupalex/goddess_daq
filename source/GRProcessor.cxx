#include "GEBSort.h"
#include "GTMerge.h"
#include "DGSProcessor.h"
#include "GRProcessor.h"

GRProcessor::GRProcessor(int nGsGe_,  int* tlkup_, int* tid_, int* ng_, PARS* pars_)
{
      nGsGe = nGsGe_;

    pars = pars_;

    M = 350.0;

    ehigain = new float[nGsGe + 1];
    ehioffset = new float[nGsGe + 1];
    ehibase = new float[nGsGe + 1];
    ehiPZ = new float[nGsGe + 1];

    GRProcessor::angle = new double[nGsGe];
    GRProcessor::anglePhi = new double[nGsGe];

    GRProcessor::dopCorFac = new double[nGsGe];
    GRProcessor::aCFac = new double[nGsGe];

    tlkup = tlkup_;
    tid = tid_;
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



