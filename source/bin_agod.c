#include <iostream>

#include "gdecomp.h"
#include "GEBSort.h"
#include "GTMerge.h"

#include "TH1D.h"
#include "TH1F.h"
#include "TH2F.h"

typedef struct PAYLOAD
{
  char p[MAXDATASIZE];
} PAYLOAD;

typedef struct TRACK_STRUCT
{
  int n;
  GEBDATA *gd;
  PAYLOAD *payload;
} TRACK_STRUCT;


/* pointers to ROOT spectra */


/* parameters */

extern PARS Pars;

struct AGODEVENT {
	std::vector<short> channels;
	std::vector<short> values;
	unsigned long long timestamp;
};

int AGODEvDecompose (unsigned int *ev, int len, AGODEVENT *AGODEvent){
	AGODEvent->channels.clear();
	AGODEvent->values.clear();

	unsigned int readWords = 0;
	unsigned long long timestamp = 0;
	for (int i=0;i<len;i++) {
		unsigned int datum = *ev++;
		unsigned short channel = datum & 0xFFFF;
		unsigned short value = (datum >> 16) & 0xFFFF;

		if (channel >= 244 && channel <=246) {
			timestamp |= (unsigned long long) value << (16 * (246-channel));
		}
		else {
			AGODEvent->channels.push_back(channel);	
			AGODEvent->values.push_back(value);	
			readWords++;
		}
	}	
	AGODEvent->timestamp = timestamp;	
}

AGODEVENT AGODEvent[MAXCOINEV];

extern DGSEVENT DGSEvent[MAXCOINEV];
extern int ng;

TH1D *h1_agod_en;
TH2F *h2_agod_en;
TH2F *h2_dTg_agod;
TH2F *h2_g_agod;

/*-----------------------------------------------------*/

int sup_agod()
{
  /* declarations */

  TH1D *mkTH1D (char *, char *, int, double, double);
  TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

  h1_agod_en = mkTH1D((char *)"agod_all_en",(char *)"agod_all_en",4096,0,4096);
  h2_agod_en = mkTH2F((char *)"agod_en",(char *)"agod_en",4096,0,4096,400,0,400);

  h2_dTg_agod = mkTH2F((char *)"dTg_agod",(char *)"dTg_agod",4000,-2000,2000,400,0,400);

  h2_g_agod  = mkTH2F((char *)"g_agod",(char *)"g_agod",4000,0,4000,4000,0,400000);

  /* list what we have */

  printf (" we have define the following spectra:\n");

  Pars.wlist = gDirectory->GetList ();
  Pars.wlist->Print ();

};

/* ----------------------------------------------------------------- */

int bin_agod (GEB_EVENT * GEB_event)
{

  /* declarations */

  char str[128];
  int ndssd;
  int ndfma;
  int nfp;
  int nsubev;

  /* prototypes */

  int GebTypeStr (int type, char str[]);

  if (Pars.CurEvNo <= Pars.NumToPrint)
    printf ("entered bin_agod:\n");

  ndfma = 0;
  ndssd = 0;
  nsubev = 0;
  nfp = 0;

  /* loop through the coincidence event and fish out GEB_TYPE_AGOD data */

  for (int i = 0; i < GEB_event->mult; i++) {
    // look for analog marker 0x13 = 19
    if (GEB_event->ptgd[i]->type == 19) {
      if (Pars.CurEvNo <= Pars.NumToPrint) {
        GebTypeStr (GEB_event->ptgd[i]->type, str);
        printf ("bin_template, %2i> %2i, %s, TS=%lli\n", i, GEB_event->ptgd[i]->type, str, GEB_event->ptgd[i]->timestamp);
      }
      AGODEvDecompose ((unsigned int *) GEB_event->ptinp[i], GEB_event->ptgd[i]->length / sizeof (unsigned int), &AGODEvent[nsubev]);

      nsubev++;
    }
  }

  // histogram incrementation 
  for (int i=0;i<nsubev;i++) {
	  for (int j=0;j<AGODEvent[i].values.size();j++) {
		  h2_agod_en->Fill(AGODEvent[i].values[j],AGODEvent[i].channels[j]);
		  h1_agod_en->Fill(AGODEvent[i].values[j]);
	  }
  }

  // time differences 

  double dTg_agod;
  dTg_agod = 0.0;

  for(int i=0;i<nsubev;i++) {
	  if((AGODEvent[i].timestamp > 0) && (ng > 0)) {
		  for (int j=0;j<AGODEvent[i].values.size();j++) {
			  dTg_agod = double(DGSEvent[0].event_timestamp) - double(AGODEvent[i].timestamp);
			  h2_dTg_agod->Fill(dTg_agod,AGODEvent[i].channels[j]);
		  }
	  }
  }

  for(int i=0;i<nsubev;i++) {
	  for(int j=0;j<ng;j++) {
		  dTg_agod = double(DGSEvent[j].event_timestamp) - double(AGODEvent[i].timestamp);
		  for (int j=0;j<AGODEvent[i].values.size();j++) {
			  if ((AGODEvent[i].channels[j]==10)&&(dTg_agod>260)&(dTg_agod<300)) {
				  h2_g_agod->Fill(DGSEvent[j].ehi,AGODEvent[i].values[j]);
			  }
		  }
	  }
  }

  /* done */

  if (Pars.CurEvNo <= Pars.NumToPrint)
	  printf ("exit bin_agod\n");

  return (0);

}
