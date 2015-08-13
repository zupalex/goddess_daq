
/**************************/
/*TRACE ANALYSIS FUNCTIONS*/			\
/**************************/











// smooth3ch filter
void smoothFilter3ch(short int trace[], int trLen, short int smoothTrace2[]) {

 int i;

 for (i=5;i<trLen;i++) {
	smoothTrace2[i]= (trace[i-1] + 2*trace[i] + trace[i+1])/4;
 }
}


// smooth5ch filter
void smoothFilter5ch(short int trace[], int trLen, short int smoothTrace2[]) {

 int i;

 for (i=5;i<trLen;i++) {
	smoothTrace2[i]= (trace[i-2] + 2*trace[i-1] + 4*trace[i] + 2*trace[i+1] + trace[i+2])/10;
 }
}

// smooth7ch filter
void smoothFilter7ch(short int trace[], int trLen, short int smoothTrace2[]) {

 int i;

 for (i=5;i<trLen;i++) {
	smoothTrace2[i]= (trace[i-3] + 2*trace[i-2] + 4*trace[i-1] + 8*trace[i] + 4*trace[i+1] +2*trace[i+2] + trace[i+3])/22;
 }
}



// D filter 
void dFilter(short int trace[], int trLen, int dTrace[]) {

 int i;

 for (i=1; i<trLen; i++) {
   dTrace[i]=trace[i]-trace[i-1];
 }
}

// D filter 
void dFilterNEW(short int trace[], int trLen, short int dTrace[]) {

 int i;

 for (i=1; i<trLen; i++) {
   dTrace[i]=trace[i]-trace[i-5];
 }
}


void averaging(short int trace[], int trLen, short int dTrace[]){

  int i;

  for(i=1;i<trLen;i++){
    dTrace[i] = (trace[i-2] + trace[i-1] + trace[i] + trace[i+1] + trace[i+2])/5;
  }



}


// DD filter 
void ddFilter(short int trace[], int trLen, int ddTrace[]){

 int i;

 for (i=10; i<trLen-4; i++) {
   ddTrace[i]=trace[i]-2*trace[i-1]+trace[i-2];
 }

}

// DD filter small signals
void ddFilterNEW(short int trace[], int trLen, short int ddTrace[]){

 int i;

 for (i=10; i<trLen-4; i++) {
   ddTrace[i]=trace[i]-2*trace[i-5]+trace[i-10];
 }  

}

// LE filter  
void leFilter(short int trace[], int trLen, int k, int leTrace[], int& t0, int& t1, int leThr) {

 int i;
 t0 = 0;
 t1 = 0;

  

 for (i=k;i<trLen;i++) {  
   leTrace[i]=trace[i]-trace[i-k];
   //if(i<100)printf("trace[%i]: %i, trace[%i]: %i, leTrace[%i]: %i\n",i,trace[i],i-k,trace[i-k],leTrace[i]);
   
   if( (leTrace[i]>leThr) && (leTrace[i-1]<leThr) && (t0>0) && (t1==0) ){
     t1 = i;
   }
   if( (leTrace[i]>leThr) && (leTrace[i-1]< leThr) && (t0==0)){
     t0 = i;
   }

 }

//printf("\n#2a trace[10]: %i\n",trace[10]);
//printf("trLen: %i, k: %i\n",trLen,k);
//printf("\n#2b letrace[50]: %i\n",leTrace[50]);

}



// trigger 1 on trace
void trigTrace(int trace[], int trLen, int trStart, int trStop, int thr, int &t0) {

  int i;
  
  int safetyTrace = 0;
  if ( trStop < trLen ) {
    safetyTrace = trStop;
  } else {
    safetyTrace = trLen;
  }

  for (i=trStart;i<safetyTrace;i++) {  
    if ((trace[i]>=thr)&&(trace[i+1]<thr)&&(t0==0)) {
      t0=i;
      break;
    }
  }
  
}


void negd2(int trace[], int trLen, int trStart, int trStop, int thr, int &numtrigs) {

  int safetyTrace = 0;
  if ( trStop < trLen ) {
    safetyTrace = trStop;
  } else {
    safetyTrace = trLen;
  }

  int i;
   
  for(i=trStart;i<safetyTrace;i++){
    if((trace[i]>=thr) && (trace[i+1] < thr)){
      numtrigs++;
    }
  } 


}
 

// Trapezoidal filter
void trapFilter(short int trace[], int trLen, int m, int k, int trapTrace[]){

   int i;
   for(i=m+k+m;i<trLen;i++){
      trapTrace[i] = trapTrace[i-1] + (trace[i]-trace[i-m])-(trace[i-m-k]-trace[i-m-k-m]);
   }

}

// extract energies from PU traces
void GetPUEn(int t1, int t0, int trace[], int trlen, float &e0, float &e1, int m, int k, int n, float trise) {

  if ( n > 0 ) {
    ;
  }

   //int tr[1000];
   float ebl;
   //float s1, s2;//unused
   //float corr;//unused
   float dt;
   int iw0, iw1;
   int i;

   // globals: m, kk, trise

   //tr=trace;

   /*
   for (i=0; i<10; i++) {
     fprintf(ftest,"%d  %d \n", i, trace[i]);
   }
   */

   if (t1-t0>k) { // time separation longer than k 



        // integration windows
        if (t1-t0-k>m) iw0=m; else iw0=t1-t0-k; // 11-11 correction
        //if (iw0<0) iw0=1;
        if (trlen-t1-k>m) iw1=m; else iw1=trlen-t1-k; //11-11 correction
        //if (iw1<0) iw1=1;
        //if (iw1>iw0) iw1=iw0;




        // front energies
        e0=0; e1=0;
        for (i=t0+k;i<t0+k+iw0;i++) {
	  e0=e0+trace[i]-trace[i-iw0-k];
        }
        for (i=t1+k;i<t1+iw1+k;i++) { 
	  e1=e1+trace[i]-trace[i-k-t1+t0-iw1];
        }



        // PZ correction
        /*if (iw0>2*n) { // gate wide enough to get slope   
          s1=0; s2=0;
          for (i=t0+k;i<t0+k+n;i++) {
	    s1=s1+trace[i];
          }
          for (i=t0+k+iw0-n;i<t0+k+iw0;i++) {
	    s2=s2+trace[i];
        }
	  corr=float(s1-s2)/float(n)*float(iw0+k)/float(iw0-n)*float(iw1); // 11-11 correction
        //corr=0;
        e1=e1+corr;
        }*/


   
        e0=e0*float(m)/float(iw0); // gain correction // 11-11 correction
        e1=e1*float(m)/float(iw1); //  gain correction // 11-11 corrrection
        e1=e1-e0;



   } else { // t1-t0 < k



     if (t1-t0<=trise) {
   
 
        e0=0; e1=0;
        dt=t1-t0;
        // integration of the begining of 1st signal
        for (i=t0; i<t1;i++) {
	  e0=e0+trace[i];
        }
        //printf("integral %f\n", e0);
        ebl=0;    
        // base line inegration
        for (i=t0-m; i<t0;i++) {
	  ebl=ebl+trace[i];
        }
        e0=e0-ebl*dt/float(m); // energy of 1st signal
        // integration of 1st and 2nd signal
        for (i=t1+k;i<t1+m+k;i++) { 
	  e1=e1+trace[i];
        }           

        e1=e1-ebl; // energy sum of 1st and 2nd signal
        e0=2.0*trise*float(m)*e0/dt/dt; // energy of 1st signal
        e1=e1-e0; // enery of 2nd signal

     } else {

        e0=0; e1=0;
        dt=t1-t0-trise;
        // integration of the begining of 1st signal
        for (i=t0+trise; i<t1;i++) {
	  e0=e0+trace[i];
        }
        //printf("integral %f\n", e0);
        ebl=0;    
        // base line inegration
        for (i=t0-m; i<t0;i++) {
	  ebl=ebl+trace[i];
        }
        e0=e0-ebl*dt/float(m); // energy of 1st signal
        // integration of 1st and 2nd signal
        for (i=t1+k;i<t1+m+k;i++) { 
	  e1=e1+trace[i];
        }           

        e1=e1-ebl; // energy sum of 1st and 2nd signal
        e0=float(m)*e0/dt; // energy of 1st signal
        e1=e1-e0; // enery of 2nd signal

     }

   }

  }


