
/* sorter for DGS merged data data */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stddef.h>
#include <values.h>
#include <zlib.h>

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
#include "TGraph.h"

#include "TMapFile.h"

#include "efftape.h"
#include "gsII.h"

#define CC_ID1 10               /* instead of 29 as default */
#define CC_ID2 11               /* instead of 39 as default */

#define GRETINA 0
#include "DGSSort.h"
#include "GTMerge.h"

/* global variables */

//int nEvents = 2000000000;
int nEvents = 1;
char ROOTFileOption[STRLEN] = "UNDEFINED";
char GTSortInputFile[STRLEN];
int UseShareMemFile = FALSE;
unsigned int StartMapAddress = 0;
char ShareMemFile[STRLEN] = "GTSort.map";
int InputSrc = NOTDEF;
int InputSourceOnCommandLine = 0;
int HaveRootFileName = 0;
int WeWereSignalled = FALSE;    /* signal  */
int UseRootFile = FALSE, SizeShareMemFile = FALSE;
int UpdateRootFile = 0;
char ROOTFile[STRLEN] = "GTSort.root";
char spname[STRLEN];
int firstEvent = 0;
int GSudpPort = 1101;
float Beta = 0;
int NumToPrint = 25;
int DumpEvery = 10;
#if(USEZLIB==0)
off_t DGSDataFile;
#endif
#if(USEZLIB==1)
gzFile zDGSDataFile;
#endif

int CurEvNo = 0, NprintEvNo = 0;
int bufPos = 0;
int nBadTestPat = 0;
int egemin = 2;
double oldLEDTs = 0;
double oldCFDTs = 0;
time_t tdmp = 0, tdmplast;
float ehiGeOffset[NCHANNELS];
float ehiGeGain[NCHANNELS];
int ehiDoGainCor = 0;
int minid = MAXINT, maxid = -MAXINT;

char CommandFileName[STRLEN] = "GSSort.command";

int time_stamp ();
TH1D *mkTH1D (char *, char *, int, double, double);
TH2F *mkTH2F (char *, char *, int, double, double, int, double, double);

/* macros */

#define WRITEALLHISTS  \
  gDirectory->cd("root:/"); \
  wlist = gDirectory->GetList(); \
  if(!UpdateRootFile) f1= new TFile(ROOTFile, ROOTFileOption); \
  if (ComPressLevel>NOTDEF) f1->SetCompressionLevel(ComPressLevel); \
  printf("writing all spectra to [%s]\n", ROOTFile); \
  printf("be patient... "); \
  fflush(stdout); \
  t1 = time(NULL); \
  wlist->Write(0,TObject::kOverwrite); \
  t2 = time(NULL); \
  printf("DONE! on "); \
  time_stamp(); \
  printf("file size: %i, ",f1->GetSize()); \
  printf("compression level: %i, ",f1->GetCompressionLevel()); \
  printf("and factor: %f\n",f1->GetCompressionFactor()); \
  printf("uncompressed root file size: %f\n",f1->GetSize()*f1->GetCompressionFactor()); \
  printf("writeout time: %i seconds\n", t2 - t1); \
  printf("at %7.2f Mbytes/sec\n", (float) f1->GetSize() / (t2 - t1) / 1000000.0); \
  printf("on "); \
  time_stamp(); \
  fflush(stdout);

#define UPDSSHMEM \
  t1 = time(NULL); \
  mfile->Update(); \
  t2 = time(NULL); \
  printf("done! "); \
  printf("shared memory size: %i\n", mfile->GetSize()); \
  printf("update time: %i seconds ", t2 - t1);				\
  printf("at %7.2f Mbytes/sec\n", (float) mfile->GetSize() / (t2 - t1) / 1000000.0); \
  printf("to mapfile [%s] on ",ShareMemFile); \
  time_stamp(); \
  fflush(stdout);

/* our coincident event (defined in gtMerge.h) */

COINEV CEvent;
CONTROL control;                /* not actualy used */

/* user */

#include "UserInclude.h"

#define DEBUG1 0
/* Doppler correction and Ge calibrations */

#define NGSGE 110

float cal_off[NGSGE], cal_gain[NGSGE];
double angle[NGSGE], anglephi[NGSGE];
double DopCorFac[NGSGE], ACFac[NGSGE];


void SetBeta ();

/*-------------------*/

/*----------------------------------------------------------------------------*/

int
DGSGetDiskEv ()
{

  /* declarations */

  static int nn = 0, i, i1;
  int siz;

#if(0)
  printf ("DGSGetDiskEv: called, nn=%i\n", nn);
  fflush (stdout);
#endif

  /* read the length of the event */

#if(USEZLIB==0)
  siz = read (DGSDataFile, (char *) &CEvent.len, sizeof (unsigned short int));
  if (siz != sizeof (unsigned short int))
    return (1);
  siz = read (DGSDataFile, (char *) &CEvent.lenclean, sizeof (unsigned char));
  if (siz != sizeof (unsigned char))
    return (1);
  siz = read (DGSDataFile, (char *) &CEvent.lendirty, sizeof (unsigned char));
  if (siz != sizeof (unsigned char))
    return (1);
  siz = read (DGSDataFile, (char *) &CEvent.lenbgo, sizeof (unsigned char));
  if (siz != sizeof (unsigned char))
    return (1);
#endif

#if(USEZLIB==1)
  siz = gzread (zDGSDataFile, (char *) &CEvent.len, sizeof (unsigned short int));
  if (siz != sizeof (unsigned short int))
    return (1);
  siz = gzread (zDGSDataFile, (char *) &CEvent.lenclean, sizeof (unsigned char));
  if (siz != sizeof (unsigned char))
    return (1);
  siz = gzread (zDGSDataFile, (char *) &CEvent.lendirty, sizeof (unsigned char));
  if (siz != sizeof (unsigned char))
    return (1);
  siz = gzread (zDGSDataFile, (char *) &CEvent.lenbgo, sizeof (unsigned char));
  if (siz != sizeof (unsigned char))
    return (1);
#endif

#if(0)
  printf ("DGSGetDiskEv: CEvent.len = %i, CEvent.lenclean = %i, CEvent.lendirty = %i, CEvent.lenbgo = %i\n", CEvent.len,
          CEvent.lenclean, CEvent.lendirty, CEvent.lenbgo);
  fflush (stdout);
#endif

  /* read the individual 'events' */

#if(0)
  for (i = 0; i < CEvent.len; i++)
    {
      siz = read (DGSDataFile, (char *) &CEvent.DGSEvent[i], sizeof (DGSEVENT));
      if (siz != sizeof (DGSEVENT))
        return (2);
    };
#endif

#if(1)
  for (i = 0; i < CEvent.len; i++)
    {
#if(USEZLIB==0)
      siz = read (DGSDataFile, (char *) &CEvent.DGSEvent[i], DGSEVENT_BASELEN);
#endif
#if(USEZLIB==1)
      siz = gzread (zDGSDataFile, (char *) &CEvent.DGSEvent[i], DGSEVENT_BASELEN);
#endif
      if (siz != DGSEVENT_BASELEN)
        return (2);
      i1 = CEvent.DGSEvent[i].traceLen * sizeof (short int);
#if(USEZLIB==0)
      siz = read (DGSDataFile, (char *) &CEvent.DGSEvent[i].trace[0], i1);
#endif
#if(USEZLIB==1)
      siz = gzread (zDGSDataFile, (char *) &CEvent.DGSEvent[i].trace[0], i1);
#endif
      if (siz != i1)
        return (2);

    };
#endif

#if(0)
  printf ("DGSGetDiskEv: read %i individual events\n", CEvent.len);
  fflush (stdout);
  if (0)
    exit (0);
#endif

  /* done */
  nn++;
#if(0)
  if (nn % 100000 == 0)
    printf ("read %i events\n", nn);
#endif
  return (0);

}


/*----------------------------------------------------------------------------*/

int
main (int argc, char **argv)
{

  /*--------------*/
  /* declarations */
  /*--------------*/

  int j, i, HaveChatFile = 0;
  char *p;
  char ChatFileName[STRLEN];
  int DGSacq (char *);
  int time_stamp ();
  char str2[STRLEN], str3[STRLEN], str4[STRLEN];



  /* inform of version of root used */

  printf ("from %s on %s\n", __FILE__, __DATE__);
  printf ("RCS version $Id: DGSSort.cxx,v 1.9 2013/05/16 17:17:19 tl Exp $\n");

  /*------*/
  /* help */
  /*------*/

  if (argc < 2)
    {
      printf ("\n");
      printf
        ("use: %s -chat file [-help] [-version] [-input disk|net|tape file|drive|port UPDATE|RECREATE  -rootfile name.root]\n",
         argv[0]);
      printf ("\n");
      return (0);
    };

  /*--------------------*/
  /* parse command line */
  /* and call DGSacq     */
  /*--------------------*/

  j = 1;                        /* current command line arg position */

  if (argc > 1)
    while (j < argc)
      {

        if ((p = strstr (argv[j], "-version")) != NULL)
          {
            printf ("$Id: DGSSort.cxx,v 1.9 2013/05/16 17:17:19 tl Exp $\n");
            printf ("This version has Float 2D matrices\n");
            exit (0);
          }
        else if ((p = strstr (argv[j], "-help")) != NULL)
          {
            printf ("\n");
            printf ("GTSort is documented on the WWW, URL:\n");
            printf ("\n");
            printf ("  http://www.phy.anl.gov/gs/doc/GTSort\n");
            printf ("\n");
            exit (0);

          }
        else if ((p = strstr (argv[j], "-input")) != NULL)
          {

            /* check that user specified enough arguments */

            j++;
            if ((argc - j) < 3)
              {
                printf ("you must specify -input disk|tape file|drive|port UPDATE|RECREATE\n");
                exit (0);
              };

            /* input source will be specified on command line. */
            /* Set flag to prevent processing of */
            /* input source in chat script  */

            InputSourceOnCommandLine = 1;

            /* determine input source */

            strcpy (str2, argv[j++]);
            strcpy (str3, argv[j++]);
            strcpy (ROOTFileOption, argv[j++]);

            printf ("\n");
            printf ("*** GSSort will take \"input source\" from command line\n");

            if (strcmp ("tape", str2) == 0)
              {
                printf ("will take input from tape\n");
                InputSrc = TAPE;
                fflush (stdout);
              }
            else if (strcmp ("disk", str2) == 0)
              {
                printf ("will take input from disk\n");
                strcpy (GTSortInputFile, str3);
                InputSrc = DISK;
                fflush (stdout);
              }
            else if (strcmp ("net", str2) == 0)
              {
                printf ("DGSSort cannot take data from the net; quit\n");
                exit (0);
              }
            else
              {
                printf ("unknown input option: %s\n", str2);
                printf ("aborting\n");
                fflush (stdout);
                exit (0);
              };

            if ((p = strstr (ROOTFileOption, "UPDATE")) != NULL)
              {
                UpdateRootFile = TRUE;
                printf ("will update root file\n");
              }
            else if ((p = strstr (ROOTFileOption, "RECREATE")) != NULL)
              {
                UpdateRootFile = FALSE;
                printf ("will recreate root file\n");
              }
            else
              {
                printf ("option <%s> not valid!\n", str4);
                printf ("valid options are: RECREATE or UPDATE\n\n");
                exit (0);
              };

            printf ("\n");

          }
        else if ((p = strstr (argv[j], "-chat")) != NULL)
          {
            j++;
            strcpy (ChatFileName, argv[j++]);
            printf ("will read sorting instructions from chatfile: %s\n", ChatFileName);
            system ("ulimit -a");
            HaveChatFile = 1;
          }
        else if ((p = strstr (argv[j], "-rootfile")) != NULL)
          {
            j++;
            strcpy (ROOTFile, argv[j++]);
            printf ("rootfile name specified on command line\n");
            printf ("__will store spectra in rootfile: %s\n", ROOTFile);
            HaveRootFileName = 1;
            UseRootFile = 1;
          }
        else
          {
            printf ("command line argument not understood!\n");

            printf ("%s: I was called as: \n--->[", argv[0]);
            for (i = 0; i < argc; i++)
              {
                printf ("%s ", argv[i]);
                fflush (stdout);
              }
            printf ("]\non ");
            time_stamp ();
            exit (0);

          }
      };

  /* now start the sorter */

  if (HaveChatFile)
    DGSacq (ChatFileName);
  else
    {
      printf ("you must specify a chat script\n");
      exit (0);
    }

}

/*--------------------------------------------------------------------------*/

void
signal_catcher (int sigval)
{
  int time_stamp ();
  printf ("\nGSSort/DGSacq received signal <%i> on ", sigval);
  time_stamp ();
  WeWereSignalled = TRUE;
  fflush (stdout);
}

/*---------------------------------------------------------------------------*/

void
sdummyload (Long_t size)
{

  /* dummy load a shared memory to find out what */
  /* start address it chooses................... */

  TMapFile *m;
  m = TMapFile::Create ("dummy.map", "recreate", size);
  StartMapAddress = (unsigned int) m->GetMmallocDesc ();
  m->Print ();

  /* close and remove dummy map file */

  m->Close ();
  gSystem->Exec ("\\rm dummy.map");
}

int
GSSort_read_chat (char *name)
{

  /* declarations */

  FILE *fp, *fp1;
  char *pc, *pc1, str[STRLEN] = { '0' }, str1[STRLEN] =
  {
  '0'}, str2[STRLEN] =
  {
  '0'};
  char str3[STRLEN], str4[STRLEN], str5[STRLEN], str6[STRLEN];
  int nn = 0, nni = 0, st, PType;
  char *p;
  int i, k, i1, i2, i3, i4, i5, i6;
  int j1, j2, j3, j4, j5, j6, j7;
  float f1, f2, f3, f4;
  int echo = 0, nret;
  double d1;

  /* prototypes */

  TCutG *rd2dwin (Char_t *);
  int FindPEvMatNo (char *, int *);
  void FindCondNo (char *, int *);
  int SetFERAVSN (char *, int);
  void InitializeFERAvsntable ();
  void ZeroFERAtypeVSN ();
  void PrintFERATypes ();
  void SetNPosWarn (int);
  void SetRecordVer_tape (int);
  void SetRecordVer_disk (int);
  void SetRecordVer_net (int);
  int str_decomp (char *, int, int *);
  void FindTimeMaskNo (char *, int *);
  int RdOffFile (char *, int *);
  int RdGeCalFile (char *, float *, float *);
  void CheckNoArgs (int, int, char *);
  void SetSpecial (char *str);
  void SetExportModNWords (char *, int);
  void SetlongRaNCHANNELSTDCNWords (char *, int);
  void setIsomerIDs (int);
  void SetClockPar (int, float, float);
  void SetFERAvsntable (int, int);
  void SetnFeraDebug (int);

  /* initialize */


  /* open chat file */

  if ((fp = fopen (name, "r")) == NULL)
    {
      printf ("error: could not open chat file: <%s>\n", name);
      exit (0);
    };
  printf ("chat file: <%s> open\n", name);
  printf ("\n");
  fflush (stdout);

  /* read content and act */

  pc = fgets (str, STRLEN, fp);

  /* rmEndComment(str, STRLEN); */

  while (pc != NULL)
    {
      if (echo)
        printf ("chat->%s", str);
      fflush (stdout);

      /* attemp to interpret the line */

      if ((p = strstr (str, "nevents")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &nEvents);
          CheckNoArgs (nret, 2, str);
          printf ("will sort a max of %i events\n", nEvents);
          fflush (stdout);

        }
      else if (str[0] == 35)
        {
          /* '#' comment line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if (str[0] == 59)
        {
          /* ';' comment line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if (str[0] == 10)
        {
          /* empty line, do nothing */
          nni--;                /* don't count as instruction */

        }
      else if ((p = strstr (str, "firstevent")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &firstEvent);
          CheckNoArgs (nret, 2, str);
          printf ("will start sorting at event %d\n", firstEvent);
          fflush (stdout);

        }
      else if ((p = strstr (str, "abort")) != NULL)
        {

          printf ("will abort\n");
          fclose (fp);
          printf ("\n");
          printf ("chat file: <%s> closed\n", name);
          printf ("processed %i sort instructions and %i lines\n", nni, nn);
          printf ("\n");
          fflush (stdout);
          exit (0);

        }
      else if ((p = strstr (str, "output")) != NULL)
        {
          if (!HaveRootFileName)
            {
              nret = sscanf (str, "%s %s", str1, ROOTFile);
              CheckNoArgs (nret, 2, str);
              printf ("will store spectra in rootfile: %s\n", ROOTFile);
              UseRootFile = 1;
              fflush (stdout);
            }
          else
            {
              printf ("ignoring chatfile option - since rootfile name is specified on commandline\n");
            };

        }
      else if ((p = strstr (str, "sharedmem")) != NULL)
        {
          st = nret = sscanf (str, "%s %s %i", str1, ShareMemFile, &SizeShareMemFile);
          CheckNoArgs (nret, 3, str);
          printf ("will use shared memory file: %s\n", ShareMemFile);
          printf ("__of max size: %i bytes\n", SizeShareMemFile);
          UseShareMemFile = 1;
          fflush (stdout);

        }
      else if ((p = strstr (str, "startmapaddress")) != NULL)
        {
          nret = sscanf (str, "%s %s", str1, str2);
          if ((strstr (str2, "auto")) != NULL)
            {
              printf ("%s\n", str2);
              sdummyload (600000000);
              CheckNoArgs (nret, 2, str);
            }
          else
            {
              nret = sscanf (str, "%s 0x%x", str1, &StartMapAddress);
              CheckNoArgs (nret, 2, str);
            }
          printf ("will start shared mem at address: 0x%8.8x\n", StartMapAddress);
          fflush (stdout);

        }
      else if ((p = strstr (str, "include")) != NULL)
        {
          nret = sscanf (str, "%s %s", str1, str2);
          CheckNoArgs (nret, 2, str);
          printf ("will now include chatscript %s\n", str2);
          fflush (stdout);
          GSSort_read_chat (str2);
          printf ("done including chatscript %s\n", str2);
          fflush (stdout);

        }
      else if ((p = strstr (str, "rootfileoption")) != NULL)
        {

          if (!InputSourceOnCommandLine)
            {

              nret = sscanf (str, "%s %s", str1, ROOTFileOption);
              CheckNoArgs (nret, 2, str);
              printf ("will update %s with option <%s>\n", ROOTFile, ROOTFileOption);


              if ((p = strstr (ROOTFileOption, "UPDATE")) != NULL)
                {
                  UpdateRootFile = TRUE;
                  printf ("will update root file\n");
                }
              else if ((p = strstr (ROOTFileOption, "RECREATE")) != NULL)
                {
                  UpdateRootFile = FALSE;
                  printf ("will recreate root file\n");
                }
              else
                {
                  printf ("option <%s> not valid!\n", ROOTFileOption);
                  printf ("valid options are: RECREATE or UPDATE\n\n");
                  exit (0);
                };
            }
          else
            {
              printf ("__ignoring chatscript option since it is specified on command line\n");
            };

          fflush (stdout);

        }
      else if ((p = strstr (str, "printevents")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &NumToPrint);
          CheckNoArgs (nret, 2, str);
          printf ("will print details of the first %i events\n", NumToPrint);
          fflush (stdout);

        }
      else if ((p = strstr (str, "beta")) != NULL)
        {
          nret = sscanf (str, "%s %f", str1, &Beta);
          CheckNoArgs (nret, 2, str);
          printf ("will use Beta (v/c) correction of %f\n", Beta);
          fflush (stdout);

        }
      else if ((p = strstr (str, "dumpevery")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &DumpEvery);
          CheckNoArgs (nret, 2, str);
          printf ("will dump to output file every %i minutes\n", DumpEvery);
          fflush (stdout);

        }
      else if ((p = strstr (str, "egemin")) != NULL)
        {
          nret = sscanf (str, "%s %i", str1, &egemin);
          CheckNoArgs (nret, 2, str);
          printf ("will require %i minimum germanium signal\n", egemin);
          fflush (stdout);

        }
      else if ((p = strstr (str, "exit")) != NULL)
        {

          printf ("will skip rest of chat file\n");
          fclose (fp);
          printf ("\n");
          printf ("chat file: <%s> closed\n", name);
          printf ("processed %i sort instructions and %i lines\n", nni, nn);
          printf ("\n");
          fflush (stdout);
          return (0);

        }
      else if ((p = strstr (str, "input")) != NULL)
        {

          if (!InputSourceOnCommandLine)
            {
              nret = sscanf (str, "%s %s %s", str1, str2, str3);
              CheckNoArgs (nret, 3, str);
              if (strcmp ("tape", str2) == 0)
                {
                  printf ("will take input from tape\n");
                  InputSrc = TAPE;
                  fflush (stdout);
                }
              else if (strcmp ("disk", str2) == 0)
                {
                  printf ("will take input from disk\n");
                  strcpy (GTSortInputFile, str3);
                  InputSrc = DISK;
                  fflush (stdout);
                }
              else
                {
                  printf ("unknown input option: %s\n", str2);
                  printf ("aborting\n");
                  fflush (stdout);
                  exit (0);
                };
            }
#include "UserChat.h"

          else
            {
              printf ("input source specified on commandline, ignoring chatfile specification\n");
            }


        }
      else
        {

      /*-----------------------------*/
          /* chatscript read error point */
      /*-----------------------------*/

          printf ("line %2.2i in chat script, option :%s \n__not understood\n", nn, str);
          printf ("%i\n", str[0]);
          printf ("aborting\n");
          fflush (stdout);
          exit (0);
        };

      /* read next line in chat script */

      nn++;                     /* line counter */
      nni++;                    /* instruction counter */
      pc = fgets (str, STRLEN, fp);

    };

  /* done */

  fclose (fp);
  printf ("\n");
  printf ("chat file: <%s> closed\n", name);
  printf ("processed %i sort instructions and %i lines\n", nni, nn);
  printf ("\n");
  fflush (stdout);
  return (0);

};


/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

int
showStatus (GTEVENT * Event)
{
  printf ("read %i events; ", (CurEvNo - firstEvent));
  printf ("Beta=%6.4f; ", (float) Beta);
  printf ("time since last update %i minutes\n", (int) tdmp);
  printf ("Event->pos=%10i, read %6i Mbytes; ", Event->pos, (Event->pos * 4) / 1024000);
  printf ("seen %i bad EOEs\n", nBadTestPat);
  printf ("CommandFileName=\"%s\"\n", CommandFileName);

#include "UserStat.h"

  /* done */

  fflush (stdout);

  return (0);

};

/*----------------------------------------------------------------------------*/

/* allow for user functions here */

#include "UserFunctions.h"


/*----------------------------------------------------------------------------*/

int
DGSacq (char *ChatFileName)
{

  /* declarations */

  int st = 0, eov = 0, nWords = 0, i1, i, j, nret, siz;
  int ii, jj, ngs, nchico2, ichico;
  char str[256], str1[256], str2[246];
  FILE *fp;
  time_t t1, t2;
  Int_t ComPressLevel = NOTDEF;
  char *p;
  double rn, dtmpehi, d1;
  DGSHEADER dgsHeader;
  long long int tac, tFP;
  int nFP;
  float r1;
  static int firsttime = 1;
  static long long int startTS;

  /* root spectra pointers */

  TH1 *hhtemp;

  TList *wlist;
  TList *zlist;
  TIterator *hiterator;

  TMapFile *mfile;
  TFile *f1;

  TH1D *tmpTH1D = NULL;
  TH2F *tmpTH2F = NULL;

  TH1D *ehi[NGE + 1];
  TH1D *sumehi;
  TH1D *anyehi;
  TH1D *ehi_raw[NGE + 1];
  TH1D *sumehi_raw;
  TH1D *eside[NGE + 1];
  TH1D *ebgo[NGE + 1];
  TH1D *eaux[NGE + 1];
  TH1D *dt;
  TH2F *arrayhit;
  TH2F *gg;

  TH1D *hitpat;
  TH1D *hitpatge;
  TH1D *hitpatbgo;
  TH1D *tacsp;
  TH1D *rate;
  TH1D *left, *right, *xx;

  /* simple spectra */

  int hpge[NGE + 1];

  /* prototypes */

  int GSSort_read_chat (char *);
  int wr_spe (char *, int *, float *);
  int printDgsHeader (DGSHEADER);
  int GTPrintEvent (FILE *, GTEVENT *, DGSEVENT *);
  int GTPrintHeader (FILE *, GTEVENT *);

  /* allow user to declare variables here */

  printf ("\n");
  printf ("executing UserDeclare.h code\n");
  printf ("\n");
#include "UserDeclare.h"

  /*-------*/
  /* Hello */
  /*-------*/

  printf ("\n");
  printf ("DGSSOrt running on: ");
  time_stamp ();
  printf ("$Id: DGSSort.cxx,v 1.9 2013/05/16 17:17:19 tl Exp $\n");
  printf ("\n");

  /*------------*/
  /* initialize */
  /*------------*/

  printf ("\n");
  printf ("initializing\n");
  printf ("\n");
  fflush (stdout);

  for (i = 0; i < NGE; i++)
    hpge[i] = 0;

  /*------------------*/
  /* read chat script */
  /*------------------*/

  GSSort_read_chat (ChatFileName);
  printf ("checking proper input of chat file...\n");
  if (InputSrc == NOTDEF)
    {
      printf ("you must specify an input source\n");
      exit (1);
    }
  else if (InputSrc == DISK)
    {

#if(USEZLIB==0)
      /* attempt to open input file */

      DGSDataFile = open (GTSortInputFile, O_RDONLY, 0);
      if (DGSDataFile == -1)
        {
          printf ("could not open\"%s\"; quit\n", GTSortInputFile);
          exit (1);
        }
      else
        printf ("input file \"%s\" is open\n", GTSortInputFile);
#endif

#if(USEZLIB==1)
      /* attempt to open input file */

      zDGSDataFile = gzopen (GTSortInputFile, "r");
      if (zDGSDataFile == NULL)
        {
          printf ("could not open zipped \"%s\"; quit\n", GTSortInputFile);
          exit (1);
        }
      else
        printf ("zipped input file \"%s\" is open\n", GTSortInputFile);
#endif


    }
  else
    {
      printf ("input source not recognized, quit\n");
      exit (1);
    };


  /* read the data file header */

  printf ("attemping to read header\n");
#if(USEZLIB==0)
  siz = read (DGSDataFile, (char *) &dgsHeader, sizeof (DGSHEADER));
  if (siz != sizeof (DGSHEADER))
    {
      printf ("could not read header of unzipped file %i\n", i);
      exit (1);
    };
#endif
#if(USEZLIB==1)
  siz = gzread (zDGSDataFile, (char *) &dgsHeader, sizeof (DGSHEADER));
  if (siz != sizeof (DGSHEADER))
    {
      printf ("could not read header of zipped file %i\n", i);
      exit (1);
    };
#endif

  siz = printDgsHeader (dgsHeader);
  if (siz != 0)
    {
      printf ("bad header; quit\n");
      exit (1);
    };

  /*---------------------*/
  /* other sanety checks */
  /*---------------------*/

  if (InputSrc == NOTDEF)
    {
      printf ("you must specify an input source!\n");
      printf ("quitting...\n");
      exit (1);
    };

  if (UseShareMemFile && UseRootFile)
    {
      printf ("you cannot use shared memory and a root disk\n");
      printf ("at the same time!\n");
      exit (1);
    };

  /* force user to declare intension with root file */
  /* so I can't be blamed for any overwrites!!      */

  if (!UseShareMemFile)
    if ((p = strstr (ROOTFileOption, "UNDEFINED")) != NULL)
      {
        printf ("for root files you must specify either:\n");
        printf ("\n");
        printf ("    rootfileoption RECREATE\n");
        printf ("or\n");
        printf ("    rootfileoption UPDATE\n");
        printf ("\n");
        printf ("please modify your chat file and try again\n");
        exit (-1);
      };

  /*------------------------*/
  /* execute user init code */
  /*------------------------*/

  printf ("\n");
  printf ("executing UserInit.h code\n");
  printf ("\n");

//#include "UserInit.h"

  /*-----------------------------------*/
  /* if we are going to use the        */
  /* shared map file, create it!       */
  /* be careful about the map address! */
  /*-----------------------------------*/

  if (UseShareMemFile)
    {
      printf ("\n");

      if (StartMapAddress != 0)
        {
          TMapFile::SetMapAddress ((Long_t) StartMapAddress);
          printf ("shared mem start address set to 0x%8.8x\n", StartMapAddress);
        }
      else
        printf ("will use system default for shared mem start address\n");

      mfile = TMapFile::Create (ShareMemFile, "RECREATE", SizeShareMemFile, "GS shared mem");
      if (mfile == NULL)
        {
          printf ("failed to create TMapFile\n");
          exit (-1);
        };

      printf ("shared memory [%s] created, size: %i bytes\n", ShareMemFile, SizeShareMemFile);
      fflush (stdout);
      mfile->Print ();
      printf ("\n");

    };


  NprintEvNo = 0;
  CurEvNo = 0;

  if (!UseShareMemFile)
    {
      DumpEvery = 2000000000;
      printf ("\n");
      printf ("_since rootfile: setting `DumpEvery` to infinity..!\n");
      printf ("\n");
    };

  /* delete any command file */

  sprintf (str, "\\rm -f %s", CommandFileName);
  system (str);
  printf ("deleted %s\n", str);

  /*------------------------------------------*/
  /* if we are using root file, then either   */
  /* read in old rootfile or create a nev one */
  /*------------------------------------------*/


  if (!UseShareMemFile)
    if (UpdateRootFile)
      {

        /* check here whether the old root file exists */

        fp = fopen (ROOTFile, "r");
        if (fp == NULL)
          {
            printf ("could not open old rootfile: %s\n", ROOTFile);
            printf ("the old rootfile must exist if you  \n");
            printf ("want to use the UPDATE option\n");
            printf ("aborting...\n");
            exit (0);
          };
        fclose (fp);

        /* read in old root file */

        f1 = NULL;
        f1 = new TFile (ROOTFile, "UPDATE");
        printf ("read old root file <%s>\n", ROOTFile);
        if (!f1->IsOpen ())
          {
            printf ("could not open file....\n\n");
            exit (-1);
          };
        printf ("base=<%s>\n", f1->GetPath ());
        f1->Print ();

      }
    else
      {
        /* create the rootfile */

        f1 = NULL;
        f1 = new TFile (ROOTFile, "RECREATE");
        printf ("root file <%s>\n", ROOTFile);
        if (!f1->IsOpen ())
          {
            printf ("could not open file....\n\n");
            exit (-1);
          };
        printf ("base=<%s>\n", f1->GetPath ());
        f1->Print ();
      };

  /*---------------------------*/
  /* create basic root spectra */
  /*---------------------------*/

#define MAXSECONDS 3600*3
  printf ("\n");
  sprintf (str1, "rate");
  sprintf (str2, "DGS");
  rate = mkTH1D (str1, str2, MAXSECONDS, 0, MAXSECONDS - 1);
  printf ("%s;", str1);
  printf ("\n");

  printf ("\n");
  sprintf (str1, "tac");
  sprintf (str2, "DGS");
  tacsp = mkTH1D (str1, str2, TACLEN, -TACLEN / 2, TACLEN / 2);
  printf ("%s;", str1);
  printf ("\n");

  printf ("\n");
  sprintf (str1, "hitpat");
  sprintf (str2, "DGS");
  hitpat = mkTH1D (str1, str2, NCHANNELS, 0, NCHANNELS);
  printf ("%s;", str1);
  printf ("\n");

  sprintf (str1, "hitpatGe");
  sprintf (str2, "DGS");
  hitpatge = mkTH1D (str1, str2, NGE, 0, NGE);
  printf ("%s;", str1);
  printf ("\n");

  sprintf (str1, "hitpatBGO");
  sprintf (str2, "DGS");
  hitpatbgo = mkTH1D (str1, str2, NGE, 0, NGE);
  printf ("%s;", str1);
  printf ("\n");

  sprintf (str1, "left");
  sprintf (str2, "left");
  left = mkTH1D (str1, str2, 2048, 1, 2048);
  printf ("%s;", str1);
  printf ("\n");

  sprintf (str1, "right");
  sprintf (str2, "right");
  right = mkTH1D (str1, str2, 2048, 1, 2048);
  printf ("%s;", str1);
  printf ("\n");

  sprintf (str1, "xx");
  sprintf (str2, "xx");
  xx = mkTH1D (str1, str2, 4096, -2048, 2048);
  printf ("%s;", str1);
  printf ("\n");

  /* energy spectra */

  sprintf (str2, "DGS");
  sprintf (str1, "sumehi");
  sumehi = mkTH1D (str1, str2, L14BITS, 1, L14BITS);
  sumehi->SetXTitle ("Channel");
  sumehi->SetYTitle ("Counts");

  sprintf (str2, "DGS");
  sprintf (str1, "anyehi");
  anyehi = mkTH1D (str1, str2, L14BITS, 1, L14BITS);
  anyehi->SetXTitle ("Channel");
  anyehi->SetYTitle ("Counts");


  sprintf (str2, "DGS");
  sprintf (str1, "sumehi_raw");
  sumehi_raw = mkTH1D (str1, str2, L14BITS, 1, L14BITS);
  sumehi_raw->SetXTitle ("Channel");
  sumehi_raw->SetYTitle ("Counts");

  for (i = 0; i <= NGE; i++)
    {
      sprintf (str2, "DGS");
      sprintf (str1, "ehi%3.3i", i);
      ehi[i] = mkTH1D (str1, str2, L14BITS, 1, L14BITS);
      ehi[i]->SetXTitle ("Channel");
      ehi[i]->SetYTitle ("Counts");

      sprintf (str2, "DGS");
      sprintf (str1, "ehi%3.3i_raw", i);
      ehi_raw[i] = mkTH1D (str1, str2, L14BITS, 1, L14BITS);
      ehi_raw[i]->SetXTitle ("Channel");
      ehi_raw[i]->SetYTitle ("Counts");

      sprintf (str1, "eside%3.3i", i);
      eside[i] = mkTH1D (str1, str2, L14BITS, 1, L14BITS);
      eside[i]->SetXTitle ("Channel");
      eside[i]->SetYTitle ("Counts");

      sprintf (str1, "ebgo%3.3i", i);
      ebgo[i] = mkTH1D (str1, str2, L14BITS, 1, L14BITS);
      ebgo[i]->SetXTitle ("Channel");
      ebgo[i]->SetYTitle ("Counts");

      sprintf (str1, "eaux%3.3i", i);
      eaux[i] = mkTH1D (str1, str2, L14BITS, 1, L14BITS);
      eaux[i]->SetXTitle ("Channel");
      eaux[i]->SetYTitle ("Counts");

    }


  /* module hit pattern */

  sprintf (str1, "arrayhit");
  sprintf (str2, "DGS hit pattern");
  arrayhit = mkTH2F (str1, str2, 100, 0, 100, 11, 0, 10);
  arrayhit->SetXTitle ("Digitizer");
  arrayhit->SetYTitle ("Channel");

  /* make the gamma gamma matrix */

  sprintf (str1, "gg");
  sprintf (str2, "gamma gamma matrix");
  gg = mkTH2F (str1, str2, 2048, 1, 2048, 2048, 1, 2048);

  printf ("\n");
  printf ("executing UserInit.h code\n");
  printf ("\n");
#include "UserInit.h"

  /* update shared mem with minimal info       */
  /* so it is not empty before the first update */
  /* shared memory wellness checkpoint          */

  if (UseShareMemFile)
    {
      printf ("\n");
      printf ("Note: if the command below fails,\n");
      printf ("increase the shared memory size!\n");
      printf ("\n");
      printf ("updating empty shared mem file... ");
      fflush (stdout);
      mfile->Update ();
      printf ("Done!\n");
      printf ("\n");
      fflush (stdout);
    };

  /*----------------------*/
  /* setup signal catcher */
  /*----------------------*/

#ifdef LINUX
  signal (SIGHUP, signal_catcher);
#endif
#ifdef SOLARIS
  sigset (SIGHUP, signal_catcher);
#endif








  /*---------------*/
  /* start sorting */
  /*---------------*/

  printf ("started sorting... ");
  if (InputSrc == DISK)
    printf ("from disk...\n");
  else if (InputSrc == NET)
    {
      printf ("from net... SHOULD NOT HAPPEN\n");
      exit (1);
    };
  fflush (stdout);

  tdmplast = time (NULL);
  while (st >= 0 && (CurEvNo - firstEvent) < nEvents && eov == 0)
    {

      /* zap [this may be too safe and slow...; yes it is] */

      //memset ((char *) &CEvent, 0, sizeof (COINEV));

    /*----------------*/
      /* get next event */
    /*----------------*/

      st = DGSGetDiskEv ();

#if(0)
      printf ("st=%i\n", st);
      fflush (stdout);
#endif

      /*----------------------------------------*/
      /* allow user to manipulate raw data here */
      /*----------------------------------------*/

#include "UserRawEv.h"


      if (st != 0 && InputSrc == DISK)
        {
          printf (" DGSGetDiskEv returned %i\n", st);

          /* terminate sort */

          eov = 1;

          /* note: */
          /* we might want to wait and try DGSGetDiskEv */
          /* later to give the impresssion of interactivity */
          /* here in some future version... */

        }

      if (st == 0)
        {

          /*----------------------------*/
          /* good event, now process it */
          /*----------------------------*/

          /* PLEASE NOTE: */
          /* DO NOT intrepret the data, already done for gtMerged data. */
          /* If you do it again, you will screw it up because the data */
          /* has been byte swapped already */

          /* statistics */

          CurEvNo++;
          NprintEvNo++;

          /* preserve the very fist timestamp we see */

          if (CurEvNo == 1)
            startTS = CEvent.DGSEvent[0].LEDts;

#include "UserGoodEv.h"


          //for (i = 0; i < CEvent.len; i++)
          //nWords += (HDRLENWORDS + LENEOVWORDS + CEvent.GTEvent[i].len * 2);

          /* debug print some events */

          if (CurEvNo <= NumToPrint)
            {
              printf ("+++++++++++++++++++++++++++++++\n");
              printf ("event # %i with multiplicity %i looks like this:\n", CurEvNo, CEvent.len);
              for (i = 0; i < CEvent.len; i++)
                {
                  GTPrintEvent (stdout, &CEvent.GTEvent[i], &CEvent.DGSEvent[i]);
//                  GTPrintHeader (stdout, &CEvent.GTEvent[i]);
                  fflush (stdout);
                };
              for (i = 0; i < CEvent.len; i++)
                printf ("%2i: ehi=%i\n", i, CEvent.DGSEvent[i].ehi);
            };

#include "UserPreCond.h"

          for (i = 0; i < CEvent.len; i++)
            {

              /*------------------------------------------*/
              /* bin basic things if the ID is in raNCHANNELS.. */
              /* Note: we should pileup reject here, but.. */
              /* if we do we loose all good events.. */
              /* which makes no sense!?............. */
              /*------------------------------------------*/


//do differently              if (CEvent.DGSEvent[i].pu == 0 || CEvent.DGSEvent[i].pu == 1)
              if (CEvent.DGSEvent[i].id <= NCHANNELS)
                if (1)
                  {

                    /* sanity check */

                    if (CEvent.DGSEvent[i].tpe == GE || CEvent.DGSEvent[i].tpe == BGO)
                      if (CEvent.DGSEvent[i].tid < 0 || CEvent.DGSEvent[i].tid > NGE)
                        {
                          printf ("bad event+++++++++++++++++\n");
                          printf ("event # %i with multiplicity %i looks like this:\n", CurEvNo, CEvent.len);
                          for (i = 0; i < CEvent.len; i++)
                            {
                              GTPrintEvent (stdout, &CEvent.GTEvent[i], &CEvent.DGSEvent[i]);
//                              GTPrintHeader (stdout, &CEvent.GTEvent[i]);
                              fflush (stdout);
                            };
                          for (i = 0; i < CEvent.len; i++)
                            printf ("%2i: ehi=%i\n", i, CEvent.DGSEvent[i].ehi);
                          printf ("CEvent.DGSEvent[i].tid= %i\n", CEvent.DGSEvent[i].tid);
                          printf ("QUIT");
                          printf ("QUIT");
                          printf ("QUIT");
                          printf ("QUIT");
                          exit (1);
                        };

                    /* grand hitpattern */

                    if (CEvent.DGSEvent[i].id > 0 && CEvent.DGSEvent[i].id < NCHANNELS)
                      hitpat->Fill ((double) CEvent.DGSEvent[i].id, 1);

                    if (CEvent.DGSEvent[i].tpe == GE && !CEvent.DGSEvent[i].flag)
                      if (CEvent.DGSEvent[i].tid > 0 && CEvent.DGSEvent[i].tid < NGE)
                        if (CEvent.DGSEvent[i].ehi > 5)
                          {
                            hitpatge->Fill ((double) CEvent.DGSEvent[i].tid, 1);
                            hpge[CEvent.DGSEvent[i].tid]++;
                          };

                    if (CEvent.DGSEvent[i].tpe == BGO)
                      if (CEvent.DGSEvent[i].tid > 0 && CEvent.DGSEvent[i].tid < NGE)
                        hitpatbgo->Fill ((double) CEvent.DGSEvent[i].tid, 1);

                    /* total array hit pattern */

                    if (CEvent.DGSEvent[i].board_id > 0 && CEvent.DGSEvent[i].board_id <= 100)
                      if (CEvent.DGSEvent[i].chan_id < 10)
                        arrayhit->Fill (CEvent.DGSEvent[i].board_id, CEvent.DGSEvent[i].chan_id, 1);

                    /* energies */

//                      printf ("CEvent.DGSEvent[i].tid=%i; ", CEvent.DGSEvent[i].tid);
//                      printf ("CEvent.DGSEvent[i].ehi=%i\n", CEvent.DGSEvent[i].ehi);

                    if (CEvent.DGSEvent[i].tpe == GE)
                      if (CEvent.DGSEvent[i].ehi < L14BITS)
                        {
                          ehi_raw[CEvent.DGSEvent[i].tid]->Fill ((double) CEvent.DGSEvent[i].ehi, 1);
                          sumehi_raw->Fill ((double) CEvent.DGSEvent[i].ehi, 1);
                          if (!CEvent.DGSEvent[i].flag)
                            {
                              ehi[CEvent.DGSEvent[i].tid]->Fill ((double) CEvent.DGSEvent[i].ehi, 1);
                              sumehi->Fill ((double) CEvent.DGSEvent[i].ehi, 1);
                            };
                        };

                    if (CEvent.DGSEvent[i].tpe == BGO)
                      if (CEvent.DGSEvent[i].ehi < L14BITS)
                        ebgo[CEvent.DGSEvent[i].tid]->Fill ((double) CEvent.DGSEvent[i].ehi, 1);

                    if (CEvent.DGSEvent[i].tpe == SIDE)
                      if (CEvent.DGSEvent[i].ehi < L14BITS)
                        eside[CEvent.DGSEvent[i].tid]->Fill ((double) CEvent.DGSEvent[i].ehi, 1);

                  };
            };

          if (CEvent.DGSEvent[i].ehi < L14BITS && CEvent.DGSEvent[i].ehi > 0)
            anyehi->Fill ((double) CEvent.DGSEvent[i].ehi, 1);

          /*--------------------*/
          /* gamma gamma matrix */
          /*--------------------*/

          if (CEvent.len >= 2)
            {
              for (i = 0; i < CEvent.len; i++)
                if (CEvent.DGSEvent[i].tpe == GE && !CEvent.DGSEvent[i].flag)
                  if (CEvent.DGSEvent[i].baseline >= -1000000)
                    for (j = i + 1; j < CEvent.len; j++)
                      if (CEvent.DGSEvent[j].tpe == GE && !CEvent.DGSEvent[j].flag)
                        if (CEvent.DGSEvent[j].baseline >= -1000000)
                          if (CEvent.DGSEvent[i].ehi > 0 && CEvent.DGSEvent[j].ehi > 0)
                            if (CEvent.DGSEvent[i].ehi < 2047 && CEvent.DGSEvent[j].ehi < 2047)
                              {
                                gg->Fill (CEvent.DGSEvent[i].ehi, CEvent.DGSEvent[j].ehi);
                              };
            };


          /* rate spectrum */

          for (i = 0; i < CEvent.len; i++)
            {
              d1 = CEvent.DGSEvent[i].LEDts - startTS;
              d1 /= 100000000;
              if (d1 > 0 && d1 < MAXSECONDS);
              rate->Fill (d1, 1);
//              printf("rate inc %f tpe=%i flag=%i\n", (float)d1,CEvent.DGSEvent[i].tpe,CEvent.DGSEvent[i].flag);
            };

          /*----------------------------*/
          /* make a CHICO2 tac spectrum */
          /*----------------------------*/

          /* require CHICO2, tid=1 (as tid=2 are scalers) */
          /* and one clean germanium. Each clean germanium will */
          /* update the tac spectrum */

          for (j = 0; j < CEvent.len; j++)
            if (CEvent.DGSEvent[j].tpe == CHICO2 && CEvent.DGSEvent[j].tid == 1)
//            if (CEvent.DGSEvent[j].tpe == CHICO2)
              for (i = j + 1; i < CEvent.len; i++)
//                if (CEvent.DGSEvent[i].tpe == GE )
//                if (CEvent.DGSEvent[i].tpe == GE && !CEvent.DGSEvent[i].flag)
                if (CEvent.DGSEvent[i].tpe == GE || CEvent.DGSEvent[i].tpe == BGO)                 
                  {
                    tac = CEvent.DGSEvent[i].LEDts - CEvent.DGSEvent[j].LEDts;
                    if (tac > (long long int) -TACLEN / 2)
                      if (tac < (long long int) TACLEN / 2)
                        tacsp->Fill ((double) tac, 1);
//                    printf ("%i,%i: tac inc %f, tpe=%i, tid=%i \n", i, j, (float) tac, CEvent.DGSEvent[j].tpe,
//                            CEvent.DGSEvent[j].tid);

                  }

          /*-----------------*/
          /* FMA focal plane */
          /*-----------------*/

          r1 = 0.0;
          for (i = 0; i < CEvent.len; i++)
            {
              if (CEvent.DGSEvent[i].tpe == FP)
                {
                  if (CEvent.DGSEvent[i].tid == 1 && CEvent.DGSEvent[i].ehi < 2048)
                    {
                      left->Fill ((double) CEvent.DGSEvent[i].ehi, 1);
                      r1 += CEvent.DGSEvent[i].ehi;
                    }
                  if (CEvent.DGSEvent[i].tid == 2 && CEvent.DGSEvent[i].ehi < 2048)
                    {
                      right->Fill ((double) CEvent.DGSEvent[i].ehi, 1);
                      r1 -= CEvent.DGSEvent[i].ehi;
                    }

                }
            };

          if (r1 != 0.0)
            xx->Fill (r1 / 2.0, 1);

          /*-------------------------*/
          /* execute user event code */
          /*-------------------------*/

#include "UserEv.h"
        };

      /*---------------------*/
      /* house keeping...... */
      /* done every so often */
      /*---------------------*/

      if (CurEvNo % 100 == 0)
        {

          /* calc time since last dump */

          tdmp = time (NULL);
          tdmp -= tdmplast;
          tdmp /= 60;           /* now minutes */

        };

    /*-----------------------------------------------------------*/
      /* dump all spectra on signal or dump every DumpEvery events */
      /* or respond to 'interactive' command...................... */
    /*-----------------------------------------------------------*/

      if (WeWereSignalled || (int) tdmp >= DumpEvery)
        {

          /* disarm signal */

          WeWereSignalled = FALSE;

          /* check for command file */

          fp = fopen (CommandFileName, "r");
          if (fp != NULL)
            {

              printf ("found command file: %s\n", CommandFileName);
              fgets (str, STRLEN, fp);
              printf ("with command: %s\n", str);

              if ((p = strstr (str, "dumpevery")) != NULL)
                {

                  sscanf (str, "%s %i", str1, &DumpEvery);
                  printf ("will dump to output file every %i minutes\n", DumpEvery);
                  fflush (stdout);

                }
              else if ((p = strstr (str, "printevents")) != NULL)
                {
                  /* reset print event counter */

                  nret = sscanf (str, "%s %i", str1, &i1);
                  if (nret == 2)
                    NumToPrint = i1;
                  printf ("will print %i events\n", NumToPrint);
                  NprintEvNo = 0;

                }
              else if ((p = strstr (str, "status")) != NULL)
                {

                  for (i = 0; i < CEvent.len; i++)
                    showStatus (&CEvent.GTEvent[i]);


                }
              else if ((p = strstr (str, "stopsort")) != NULL)
                {
                  /* simulate end of event to stop sort */

                  eov = 1;

                }
              else if ((p = strstr (str, "zapall")) != NULL)
                {

                  /* zap spectra */

                  if (UseShareMemFile)
                    {
                      zlist = mfile->GetDirectory ()->GetList ();
                      hiterator = zlist->MakeIterator ();
                      while (hhtemp = (TH1 *) hiterator->Next ())
                        {
                          hhtemp->Reset ();
                        }
                      printf ("all spectra were zapped @ ");
                      time_stamp ();
                      fflush (stdout);

                      /* update */

                      printf ("updating shared mem... ");
                      UPDSSHMEM;
                    }
                  else
                    {
                      /* do nothing */
                    }
                }
              else if ((p = strstr (str, "zap")) != NULL)
                {
                  /* extract spectrum name */

                  sscanf (str, "%s %s", str1, spname);
                  hhtemp = (TH1D *) gROOT->FindObject (spname);
                  if (UseShareMemFile)
                    {
                      hhtemp = (TH1 *) mfile->Remove (spname);
                      if (hhtemp != NULL)
                        {
                          hhtemp->Print ();
                          hhtemp->Reset ();
                          mfile->Add (hhtemp, spname);
                          mfile->Update (hhtemp);
                        }
                      printf ("spectrum %s zapped @ ", spname);
                      time_stamp ();
                      fflush (stdout);
                      /* update */
                    }
                  else
                    {
                      /* do nothing */
                    };

                }
              else
                printf ("command not understood\n");

              /* delete command file */

              fclose (fp);
              sprintf (str, "\\rm %s", CommandFileName);
              system (str);
              printf ("%s\n", str);

            }
          else
            {
              printf ("\"%s\" was not found\n", CommandFileName);

              /* update sh mem or writeout root file */

              printf ("time since last dump: %i minute(s)\n", (int) tdmp);
              tdmp = 0;
              if (!UseShareMemFile)
                {

                  printf ("*---------------------------------\n");
                  printf ("* you cannot update a disk file.  \n");
                  printf ("  you must wait for sort to finish\n");
                  printf ("  or stop the sort! Ignoring you...\n");
                  printf ("*---------------------------------\n");

                }
              else
                {
                  printf ("updating shared mem... ");

                  UPDSSHMEM;
                  for (i = 0; i < CEvent.len; i++)
                    showStatus (&CEvent.GTEvent[i]);
                };

              tdmplast = time (NULL);

            };

          printf ("continuing the sort...\n");
          fflush (stdout);

        };


    };

  /*-----------------------*/
  /* we are done sorting!! */
  /* save all ROOT spectra */
  /*-----------------------*/

  printf ("\n");
  printf ("Sorting is done\n");
  printf ("attempting to save root or map file\n");
  printf ("\n");
  fflush (stdout);

  /* if we were using shared memory */

  if (UseShareMemFile)
    {
      UPDSSHMEM mfile->Print ();
      printf ("\n");
      mfile->ls ();
      printf ("\n");
    };

  /* if we were using rootfile */

  if (!UseShareMemFile)
    {
      printf ("attempting to close root file...\n");
      fflush (stdout);

      printf ("f1->Write();\n");
      fflush (stdout);
      f1->Write ();
      printf ("f1->Print();\n");
      fflush (stdout);
      f1->Print ();
      printf ("f1->Close();\n");
      fflush (stdout);
      f1->Close ();
      printf ("done saving rootfile \"%s\n\n", ROOTFile);
      fflush (stdout);
    }
  printf ("\n");

  /*-------------------------*/
  /* print simple statistics */
  /*-------------------------*/

  for (i = 0; i < CEvent.len; i++)
    showStatus (&CEvent.GTEvent[i]);

#include "UserExit.h"

  printf ("\n");
  for (i = 0; i <= NGE; i++)
    if (hpge[i] > 0)
      printf ("ge # %3i has %10i hits\n", i, hpge[i]);
  printf ("\n");

  /* done */

  printf ("minid=%i,maxid=%i, NCHANNELS=%i\n", minid, maxid, NCHANNELS);

  printf ("\n ** DGSSort done!\n\n");
  time_stamp ();
  return (0);

}



/*-------------------------------------------------*/
/*----------------------------------------------------*/

TH1D *
mkTH1D (char *str1, char *str2, int nn, double lo, double hi)
{
  TH1D *tmppt;

  if (!UpdateRootFile)
    {
      tmppt = new TH1D (str1, str2, nn, lo, hi);
      printf ("Created Object \"%s\", %p\n, \"%s\"", str1, tmppt, str2);
    }
  else
    {
      tmppt = (TH1D *) gROOT->FindObject (str1);
      printf ("Found Object \"%s\", %p\n", str1, tmppt);
    }

  return (tmppt);

}

/*----------------------------------------------------*/

TH2F *
mkTH2F (char *str1, char *str2, int nn1, double lo1, double hi1, int nn2, double lo2, double hi2)
{
  TH2F *tmppt;

  if (!UpdateRootFile)
    {
      tmppt = new TH2F (str1, str2, nn1, lo1, hi1, nn2, lo2, hi2);
      printf ("Created Object \"%s\", %p\n", str1, tmppt);
    }
  else
    {
      tmppt = (TH2F *) gROOT->FindObject (str1);
      printf ("Found Object \"%s\", %p\n", str1, tmppt);
    };

  return (tmppt);

}



/*--------------------------------------------------------*/

void
CheckNoArgs (int required, int actual, char *str)
{

  if (required < actual)
    {
      printf ("argument problem with chat option\n");
      printf ("--> %s\n", str);
      printf ("required # arguments: %i\n", required - 1);
      printf ("actual   # arguments: %i\n", actual - 1);
      printf ("Please fix and try again, quitting...\n");
      exit (1);
    };

}

void
SetBeta ()
{

  /* delarations */

  int i;
  double d1;

  /*-------------------------------------*/
  /* find Doppler and aberration factors */
  /*-------------------------------------*/

  for (i = 0; i < NGSGE; i++)
    {
      //printf("det %3.3i-> ", i);
      d1 = angle[i] / 57.29577951;
      DopCorFac[i] = (1 - Beta * cos (d1)) / sqrt (1 - Beta * Beta);
      //printf("dop cor fac: %6.4f; ", DopCorFac[i]);
      ACFac[i] = DopCorFac[i] * DopCorFac[i];
      //printf("aberration cor fac: %6.4f\n", ACFac[i]);

    };
  fflush (stdout);
  /* done */

}


/*----------------------------------------------------*/
